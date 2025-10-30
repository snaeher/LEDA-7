/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _min_cut.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/min_cut.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/map.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

int MIN_CUT(const graph& G, const edge_array<int>& weight, 
                             list<node>& C, bool use_heuristic) 
{ return MIN_CUT_MF(G,weight,C,use_heuristic); }


int CUT_VALUE(const graph&G, const edge_array<int>& weight, const list<node>& L)
{ node_array<bool> in_L(G,false);
  node v; 
  forall(v,L) in_L[v] = true;
  int cut_value = 0;
  edge e;
  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if ( in_L[v] != in_L[w] ) cut_value += weight[e];
  }

  return cut_value;
}



//------------------------------------------------------------------------------
// Stoer/Wagner Algorithm
//------------------------------------------------------------------------------

static void combine_s_and_t(GRAPH<list<node>,int>& G, node s, node t)
{ G[s].conc(G[t]);

  node_array<edge> s_edge(G,nil); 
  edge e;
  forall_adj_edges(e,s)  s_edge[G.opposite(s,e)] =  e; 
  forall_adj_edges(e,t)
  { node v = G.opposite(t,e);
    if ( v == s) continue;
    if (s_edge[v] == nil) G.new_edge(s,v,G[e]); 
    else G[s_edge[v]] += G[e]; 
  }
 
  G.del_node(t); 
}

  
int MIN_CUT_SW(const graph& G0, const edge_array<int>& weight, 
               list<node>& C, bool use_heuristic) 
{ 
  edge e;
  forall_edges(e,G0) 
    if ( weight[e] < 0 ) 
      LEDA_EXCEPTION(1,"MIN_CUT: no negative weights");

  
  GRAPH<list<node>,int> G;
  G.make_undirected();
    
  node_array<node> partner(G0);
   
  node v; 
  forall_nodes(v,G0)
  { partner[v] = G.new_node(); 
    G[partner[v]].append(v); 
  }

  forall_edges(e, G0) 
  { if (source(e) == target(e)) continue; 
/*
    G.new_edge(partner[source(e)], partner[target(e)],weight[e]); 
*/
    int we = weight[e];
    G.new_edge(partner[source(e)], partner[target(e)],we); 
   }


  node a = G.first_node();   
  int best_value = MAXINT; 
  int cut_weight = MAXINT; 


  while ( G.number_of_nodes() >= 2 ) 
  { node t = a; 
    node s = nil; 
    node_array<bool> in_PQ(G,false); 

    node_pq<int> PQ(G); 

    forall_nodes(v,G) 
    if (v != a) 
    { PQ.insert(v,0); 
      in_PQ[v] = true; 
     }
    
    forall_adj_edges(e,a)
      PQ.decrease_inf(G.opposite(a,e),PQ.prio(G.opposite(a,e)) - G[e]); 
                                           
    while (!PQ.empty())
    { s = t; 
      cut_weight =  -PQ.prio(PQ.find_min()); 
      t = PQ.del_min(); 
      in_PQ[t] = false; 
      forall_adj_edges(e,t)
      { if (in_PQ[v = G.opposite(t,e)]) 
           PQ.decrease_p(v,PQ.prio(v) - G[e]); 
       }
    }

    bool new_best_cut = false;
    if ( cut_weight < best_value )
    { C = G[t]; 
      best_value = cut_weight; 
      new_best_cut = true;
      }
    combine_s_and_t(G,s,t); 

                                       
    if ( use_heuristic ) 
    { bool one_more_round = true;
      while ( one_more_round )
      { one_more_round = false;
        forall_adj_edges(e,s) 
        { node t = G.opposite(s,e);    
          if ( G[e] >= best_value ) 
          { combine_s_and_t(G,s,t); one_more_round = true; break; }
        }  
      }

      if ( new_best_cut )
      { bool one_more_round = true;
        while ( one_more_round )
        { one_more_round = false;
          forall_edges(e,G) 
          { node s = G.source(e);
            node t = G.target(e);    
            if ( G[e] >= best_value ) 
            { combine_s_and_t(G,s,t); one_more_round = true; break; }
          }  
        }
      }
    }
  }

  return best_value; 
}

LEDA_END_NAMESPACE

//------------------------------------------------------------------------------
// Maxflow on undirected graphs (by sn 10/2008)
//------------------------------------------------------------------------------

#if (defined(__SUNPRO_CC) && __SUNPRO_CC <= 0x550) || (defined(_MSC_VER) && _MSC_VER < 1300) || defined(__BORLANDC__) || defined(__mipspro__) || defined(__DMC__) || defined(__HP_aCC)

LEDA_BEGIN_NAMESPACE
int MIN_CUT_MF(const graph& G_orig, const edge_array<int>& weight_orig, 
                                    list<node>& C, bool use_heuristic)
{
  LEDA_EXCEPTION(1,"MIN_CUT_MF not available for this platform");
  return 0;
}

LEDA_END_NAMESPACE

#else


#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>

#define MF_UNDIRECTED
#include <LEDA/graph/templates/max_flow.h>


LEDA_BEGIN_NAMESPACE


typedef max_flow<int>::node_data nd_type;
typedef max_flow<int>::edge_data ed_type;
  
const int ns = sizeof(nd_type)/3;
const int es = sizeof(ed_type)/4;
  
typedef static_fgraph<opposite_graph,data_slots<ns+1>,data_slots<es+2> > 
                                                                     st_graph;
typedef st_graph::node st_node;
typedef st_graph::edge st_edge;
  
typedef node_slot1<nd_type,st_graph,0>  node_data_array;
typedef node_slot1<node,st_graph,ns>    orig_array;

typedef edge_slot1<ed_type,st_graph,0> edge_data_array;
typedef edge_slot<int,st_graph,es>     cap_array;
typedef edge_slot<int,st_graph,es+1>   flow_array;

typedef  max_flow<int, st_graph, node_data_array, edge_data_array> MaxFlow;


node min_weight_node(const graph& G, const edge_array<int>& weight, int& val)
{
  node_array<int> W(G,0);

  node v;
  forall_nodes(v,G)
  { int sum = 0;
    edge e;
    forall_inout_edges(e,v) sum += weight[e];
    W[v] = sum;
   }

  node u = G.first_node();
  forall_nodes(v,G) 
      if (W[v] < W[u]) u = v;

  val = W[u];

  return u;
}



void contract(graph& G, node a, node b, edge_array<int>& weight, 
                                        node_array<edge>& s_edge,
                                        list<edge>& L, int best_val)
{ 
  if (G.degree(b) == 0) return;

  edge e;
  forall_inout_edges(e,a) s_edge[G.opposite(a,e)] =  e; 

  list<edge> del;
  list<edge> mov;

  forall_inout_edges(e,b)
  { node v = G.opposite(b,e);
    if ( v == a) { del.append(e); continue; }
    edge x = s_edge[v];
    if (x == nil)
      mov.append(e);
    else 
      { weight[x] += weight[e]; 
        if (weight[x] >= best_val) L.append(x);
        del.append(e);
       }
  }

  forall_inout_edges(e,a)  s_edge[G.opposite(a,e)] =  nil; 
 

  forall(e,mov)
  { node v = G.opposite(b,e);
    G.move_edge(e,a,v);
   }

  forall(e,del) G.hide_edge(e);

  assert(G.degree(b) == 0);
}



void construct_contract_graph(GRAPH<node,int>& G, const graph& H, 
                                                  const edge_array<int>& weight)
{
  node_array<node> v_in_G(H); 
  node v;
  forall_nodes(v,H) v_in_G[v] = G.new_node(v);
  edge e;
  forall_edges(e,H)
  { node x = v_in_G[source(e)];
    node y = v_in_G[target(e)];
    edge ee = G.new_edge(x,y,0);
    G[ee] = weight[e];
   }
}



st_node construct_flow_graph(st_graph& G, orig_array& orig, 
                                          cap_array& cap, 
                                          const graph& H, 
                                          node s,
                                          const edge_array<int>& weight,
                                          node_array<st_node>& V)
{
  V.init(H,0);

  int n = H.number_of_nodes();
  int m = H.number_of_edges();

  G.start_construction(n+1,2*m+1);

  st_node s0 = G.new_node();
  orig[s0] = 0;

  node v;
  forall_nodes(v,H) 
  { st_node u =  G.new_node();
    V[v] = u;
    orig[u] = v;
   }

  st_edge x = G.new_edge(s0,V[s]);
  cap[x] = 0;
  
  forall_nodes(v,H)
  { edge e;
    //forall_inout_edges(e,v)
    forall_out_edges(e,v)
    { node w = opposite(v,e);
      st_edge x = G.new_edge(V[v],V[w]);
      cap[x] = weight[e];
     }
   }

  G.finish_construction();

  return s0;
}




int MIN_CUT_MF(const graph& G_orig, const edge_array<int>& weight_orig, 
                                    list<node>& C, bool use_heuristic)
{
  double frac = 0.50;
  int mf_count = 0;
  int rebuild_count = 0;

  GRAPH<node,int> G;
  construct_contract_graph(G,G_orig,weight_orig);
  edge_array<int>& weight = G.edge_data();

  st_graph F;
  orig_array V_orig(F);
  cap_array  cap(F);
  flow_array flow(F);

  node_array<st_node> V(G,0);
  node_array<edge> s_edge(G,0);

  int best_value = MAXINT;
  node s = min_weight_node(G,weight,best_value);
  node s_orig = G[s];

  best_value++;

  st_node s0 = construct_flow_graph(F,V_orig,cap,G,s,weight,V);
  st_edge e0 = F.first_out_edge(s0);
  int m0 = G.number_of_edges();

  MaxFlow mf;
  node t_best = 0;

  node t;
  forall_nodes(t,G)
  { 
    if (t == s || G.degree(t) == 0) continue;

    cap[e0] = best_value;
    int f = mf.mincut_value(F,s0,V[t],cap,flow,best_value);
    mf_count++;

    bool new_best_value = false;
    if (f < best_value) 
    { best_value = f;
      new_best_value = true;
      t_best = G[t];
     }

    if (use_heuristic)
    { list<edge> L;
      contract(G,s,t,weight,s_edge,L,best_value);

      while (!L.empty())
      { edge e = L.pop();
        node x = G.source(e);
        node y = G.target(e);
        if (y == s) swap(x,y);
        contract(G,x,y,weight,s_edge,L,best_value);
       }
  
      if (new_best_value) 
      { list<edge> L;
        edge e;
        forall_edges(e,G)
          if (weight[e] >= best_value) L.append(e);
  
        while (!L.empty())
        { edge e = L.pop();
          node x = G.source(e);
          node y = G.target(e);
          if (y == s) swap(x,y);
          contract(G,x,y,weight,s_edge,L,best_value);
         }
       }
     }

     if (G.number_of_edges() == 0) break;

     if (G.number_of_edges() < frac*m0)
     { //cout << string("rebuild: mf = %d   f = %d   |E| = %d   |F| = %d", 
       //                mf_count, best_value, G.number_of_edges(), 
       //                F.number_of_edges()) << endl;
       s0 = construct_flow_graph(F,V_orig,cap,G,s,weight,V);
       e0 = F.first_out_edge(s0);
       m0 = G.number_of_edges();
       rebuild_count++;
      }

  }

  assert(t_best != 0);

  construct_flow_graph(F,V_orig,cap,G_orig,s_orig,weight_orig,V);
  int f = mf.run(F,V[s_orig],V[t_best],cap,flow);
  assert(f == best_value);
  
  list<st_node> cut;
  mf.compute_min_st_cut(F,V[s_orig],V[t_best],cap,flow,cut);

  C.clear();
  st_node v;
  forall(v,cut) C.append(V_orig[v]);

  return best_value; 
}

LEDA_END_NAMESPACE

#endif
