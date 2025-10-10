/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mcf_cap_scaling.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graph/graph_alg.h>
#include <assert.h>

#if (defined(__SUNPRO_CC) && __SUNPRO_CC <= 0x550) || (defined(_MSC_VER) && _MSC_VER < 1300) || defined(__BORLANDC__) || defined(__mipspro__) || defined(__DMC__) || defined(__HP_aCC)

#include <LEDA/graph/templates/mcf_cap_scaling.h>

LEDA_BEGIN_NAMESPACE

bool MCF_CAPACITY_SCALING(const graph& G, const edge_array<int>& lcap,
                                          const edge_array<int>& ucap,
                                          const edge_array<int>& cost,
                                          const node_array<int>& supply,
                                          edge_array<int>& flow)

{
  typedef graph::incident_edge incident_edge;
  typedef node_array<int,graph>  pot_array;
  typedef node_array<int,graph>  excess_array;
  typedef node_array<int,graph>  dist_array;
  typedef node_array<incident_edge,graph> pred_array;
  typedef node_array<int,graph>  mark_array;

  mcf_cap_scaling<int,graph, pot_array,excess_array, dist_array,
                                       pred_array,mark_array> mcf;

  bool f = mcf.run(G,lcap,ucap,cost,supply,flow,1) ;

  if (f)
  { string msg;
    if (!mcf.check(G,lcap,ucap,cost,supply,flow,msg)) error_handler(1,msg);
   }

  return f;
}

LEDA_END_NAMESPACE

#else

#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>

#define MCF_STATIC_GRAPH
#include <LEDA/graph/templates/mcf_cap_scaling.h>

LEDA_BEGIN_NAMESPACE

bool MCF_CAPACITY_SCALING(const graph& GG, const edge_array<int>& _lcap,
                                           const edge_array<int>& _ucap,
                                           const edge_array<int>& _cost,
                                           const node_array<int>& _supply,
                                           edge_array<int>& _flow)
{
  typedef static_fgraph<opposite_graph,data_slots<5>,data_slots<4> > st_graph;
  typedef st_graph::node st_node;
  typedef st_graph::edge st_edge;
  typedef st_graph::incident_edge incident_edge;

  typedef node_slot<int,st_graph,0>      pot_array;
  typedef node_slot<int,st_graph,1>      excess_array;
  typedef node_slot<int,st_graph,2>      dist_array;
  typedef node_slot<incident_edge,st_graph,3>  pred_array; 
  typedef node_slot<int,st_graph,4>      mark_array; 

  st_graph G;

  edge_slot<edge,st_graph,0> e_orig(G);
  edge_slot<int,st_graph,1> cap(G);
  edge_slot<int,st_graph,2> flow(G);
  edge_slot<int,st_graph,3> cost(G);

  excess_array excess(G);

  edge x;
  forall_edges(x,GG)
    if (_cost[x] < 0) ((graph&)GG).rev_edge(x);


  int n = GG.number_of_nodes();
  int m = GG.number_of_edges();

  G.start_construction(n,m);

  node_array<st_node> V(GG);

  node vv;
  forall_nodes(vv,GG) 
  { st_node v = G.new_node();
    excess[v] = _supply[vv];
    V[vv] = v;
   }

  forall_nodes(vv,GG) 
  { st_node v = V[vv];
    edge x = GG.first_out_edge(vv);
    while (x)
    { st_node w = V[GG.target(x)];
      st_edge e = G.new_edge(v,w);
      e_orig[e] = x;
      x = GG.next_out_edge(x);
     }
   }

  G.finish_construction();

  forall_edges(x,GG)
    if (_cost[x] < 0) ((graph&)GG).rev_edge(x);

  st_node v;
  forall_nodes(v,G)
  { st_edge e;
    forall_out_edges(e,v)
    { st_node w = G.opposite(e,v);
      edge x = e_orig[e];
      int lc = _lcap[x];
      int ce = _cost[x];

      cap[e] = _ucap[x] -lc;

      if (ce < 0) 
      { lc = -(lc+cap[e]);
        ce = -ce;
       }

      excess[v] -= lc;
      excess[w] += lc;
      cost[e] = ce;
     }
   }


  mcf_cap_scaling<int,st_graph,pot_array,excess_array, dist_array,
                                                       pred_array,
                                                       mark_array> mcf;

  bool feasible =  mcf.run(G,cap,cost,excess,flow,4);


  if (feasible)
  { st_node v;
    forall_nodes(v,G)
    { st_edge e;
      forall_out_edges(e,v) 
      { edge x = e_orig[e];
        int fe = flow[e];
        if (_cost[x] < 0) fe = cap[e] - flow[e];
        _flow[x] = fe + _lcap[x];
       }
     }
   }
  return feasible;
}
LEDA_END_NAMESPACE

#endif 



LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// Variants
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// without lower capacity bounds
//------------------------------------------------------------------------------


bool MCF_CAPACITY_SCALING(const graph& G, const edge_array<int>& cap,
                                          const edge_array<int>& cost,
                                          const node_array<int>& supply,
                                          edge_array<int>& flow)
{ edge_array<int> lcap(G,0);
  return MCF_CAPACITY_SCALING(G,lcap,cap,cost,supply,flow); 
 }
   



//------------------------------------------------------------------------------
//  min cost maxflow
//------------------------------------------------------------------------------

int MCMF_CAPACITY_SCALING(const graph& G, node s, node t,
                                                  const edge_array<int>& cap,
                                                  const edge_array<int>& cost,
                                                  edge_array<int>& flow)
{ node_array<int> supply(G,0);
  int f = MAX_FLOW(G,s,t,cap,flow);
  supply[s] =  f;
  supply[t] = -f;
  assert(MCF_CAPACITY_SCALING(G,cap,cost,supply,flow));
  return f;
 }


LEDA_END_NAMESPACE

