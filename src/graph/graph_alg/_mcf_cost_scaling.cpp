/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mcf_cost_scaling.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

#if (defined(__SUNPRO_CC) && __SUNPRO_CC <= 0x550) || (defined(_MSC_VER) && _MSC_VER < 1300) || defined(__BORLANDC__) || defined(__mipspro__) || defined(__DMC__) || defined(__HP_aCC)

#include <LEDA/graph/templates/mcf_cost_scaling.h>

LEDA_BEGIN_NAMESPACE

typedef int NT;

bool MCF_COST_SCALING(const graph& G, const edge_array<NT>& lcap,
                                      const edge_array<NT>& ucap,
                                      const edge_array<NT>& cost,
                                      const node_array<NT>& supply,
                                      edge_array<NT>&       flow)
{
  typedef mcf_cost_scaling<int,double,double>::node_data nd_type;
  typedef mcf_cost_scaling<int,double,double>::edge_data ed_type;

  typedef node_array<nd_type> node_data_array;
  typedef edge_array<ed_type> edge_data_array;

  mcf_cost_scaling<int,double,double, graph, node_data_array, 
                                             edge_data_array> mcf;

  bool feasible = mcf.run(G,lcap,ucap,cost,supply,flow) ;

  if (feasible) 
  { string msg;
    if (!mcf.check(G,lcap,ucap,cost,supply,flow,msg)) LEDA_EXCEPTION(1,msg);
  }

  return feasible;
}


LEDA_END_NAMESPACE

#else

#include <LEDA/graph/graph_alg.h>

#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>
#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>

#define MCF_STATIC_GRAPH
#include <LEDA/graph/templates/mcf_cost_scaling.h>

LEDA_BEGIN_NAMESPACE

bool MCF_COST_SCALING(const graph& _gr, const edge_array<int>& _lcap,
                                        const edge_array<int>& _ucap,
                                        const edge_array<int>& _cost,
                                        const node_array<int>& _supply,
                                        edge_array<int>& _flow)
{
  //if (!FEASIBLE_FLOW(_gr,_supply,_lcap,_ucap,_flow)) return false;

  typedef mcf_cost_scaling<int,double,double>::node_data nd_type;
  typedef mcf_cost_scaling<int,double,double>::edge_data ed_type;

  const int ns = sizeof(nd_type)/4;
  const int es = sizeof(ed_type)/4;

  typedef 
     static_fgraph<opposite_graph,data_slots<ns+1>,data_slots<es+5> > st_graph;
  typedef st_graph::node st_node;
  typedef st_graph::edge st_edge;

  typedef node_slot1<nd_type,st_graph,0> node_data_array;
  typedef edge_slot1<ed_type,st_graph,0> edge_data_array;

  st_graph G;
  node_slot1<int,st_graph,ns+0>  supply(G);

  edge_slot1<int,st_graph,es+0> flow(G);
  edge_slot1<int,st_graph,es+1> lcap(G);
  edge_slot1<int,st_graph,es+2> ucap(G);
  edge_slot1<int,st_graph,es+3> cost(G);

  edge_slot1<edge,st_graph,es+4> e_orig(G);

  int n = _gr.number_of_nodes();
  int m = _gr.number_of_edges();

  G.start_construction(n,m);

  node_array<st_node> V(_gr);

  node vv;
  forall_nodes(vv,_gr) 
  { st_node v = G.new_node();
    supply[v] = _supply[vv];
    V[vv] = v;
   }

  forall_nodes(vv,_gr) 
  { st_node v = V[vv];
    edge x = _gr.first_out_edge(vv);
    while (x)
    { st_node w = V[_gr.target(x)];
      st_edge e = G.new_edge(v,w);
      e_orig[e] = x;
      x = _gr.next_out_edge(x);
     }
   }

  G.finish_construction();

  st_node v;
  forall_nodes(v,G)
  { st_edge e;
    forall_out_edges(e,v)
    { edge x = e_orig[e];
      cost[e] = _cost[x];
      ucap[e] = _ucap[x];
      lcap[e] = _lcap[x];
     }
   }


  mcf_cost_scaling<int,double,double,st_graph,node_data_array,
                                              edge_data_array>  mcf;

  
  bool feasible = mcf.run(G,lcap,ucap,cost,supply,flow);

  if (feasible) 
  { 
/*
    string msg;
    if (!mcf.check(G,lcap,ucap,cost,supply,flow,msg)) LEDA_EXCEPTION(1,msg);
*/

    st_node v;
    forall_nodes(v,G)
    { st_edge e;
      forall_out_edges(e,v) 
      { edge x = e_orig[e];
        _flow[x] = flow[e];
       }
     }
   }

  return feasible;
}

LEDA_END_NAMESPACE

#endif


LEDA_BEGIN_NAMESPACE

int MCMF_COST_SCALING(const graph& G, node s, node t, 
                                      const edge_array<int>& cap, 
                                      const edge_array<int>& cost, 
                                      edge_array<int>& flow)
{ edge_array<int> lcap(G,0);
  node_array<int> supply(G,0);
  int f = MAX_FLOW(G,s,t,cap,flow);
  supply[s] =  f;
  supply[t] = -f;
  MCF_COST_SCALING(G,lcap,cap,cost,supply,flow); 
  return f;
 }

LEDA_END_NAMESPACE


