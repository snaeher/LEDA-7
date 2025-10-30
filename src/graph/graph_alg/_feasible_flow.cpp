/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _feasible_flow.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>


LEDA_BEGIN_NAMESPACE

typedef int NT;


bool FEASIBLE_FLOW(const graph& G, const node_array<NT>& supply,
                                   const edge_array<NT>& cap,
                                   edge_array<NT>& flow)
{
  node_array<node> V_copy(G);

  GRAPH<node,edge> GG;

  NT total_supply = 0;

  node v;
  forall_nodes(v,G) V_copy[v] = GG.new_node(v);

  edge e;
  forall_edges(e,G) 
  { node a = G.source(e);
    node b = G.target(e);
    GG.new_edge(V_copy[a],V_copy[b],e);
    assert(cap[e] >= 0);
   }

  node s = GG.new_node(0);
  node t = GG.new_node(0);

  forall_nodes(v,G)
  { NT b = supply[v];
    if (b > 0) 
    { GG.new_edge(s,V_copy[v],0);
      total_supply += b;
     }
    if (b < 0) GG.new_edge(V_copy[v],t,0);
   }

  edge_array<NT> cap_GG(GG);
  edge_array<NT> flow_GG(GG);

  forall_edges(e,GG)
  { node a = GG.source(e);
    node b = GG.target(e);
    if (a == s)
       cap_GG[e] = supply[GG[b]];
    else
      if (b == t)
          cap_GG[e] = -supply[GG[a]];
      else
           cap_GG[e] = cap[GG[e]];
     assert(cap_GG[e] >= 0);
   }

   NT fmax = MAX_FLOW(GG,s,t,cap_GG,flow_GG);

  forall_edges(e,GG) 
  { edge x = GG[e];
    if (x) flow[x] = flow_GG[e];
   }
  
  return (fmax == total_supply);
}


bool FEASIBLE_FLOW(const graph& G, const node_array<NT>& supply,
                                   const edge_array<NT>& lcap,
                                   const edge_array<NT>& ucap,
                                   edge_array<NT>&       flow)

{
/*
  edge_array<NT> cap(ucap);
  node_array<NT> sup(supply);
*/
  edge_array<NT> cap(G);
  node_array<NT> sup(G);

  node v;
  forall_nodes(v,G) sup[v] = supply[v];

  edge e;
  forall_edges(e,G) cap[e] = ucap[e];

  forall_edges(e,G)
  { int lc = lcap[e];
    if (lc == 0) continue;
    node v = G.source(e);
    node w = G.target(e);
    cap[e] -= lc;
    sup[v] -= lc;
    sup[w] += lc;
  }

  bool feasible = FEASIBLE_FLOW(G,sup,cap,flow);

  forall_edges(e,G) flow[e] += lcap[e];

  return feasible;
}



#if defined (OLD_VERSION)

#include <LEDA/graph/templates/feasible_flow.h>


bool FEASIBLE_FLOW_OLD(graph& G, const node_array<NT>& supply,
                             const edge_array<NT>& cap,
                             edge_array<NT>&       flow)
{

  typedef node_array<int>    dist_array;
  typedef node_array<node>   succ_array;

  feasible_flow<NT, graph, succ_array, dist_array> ff;

  bool feasible = ff.run(G,supply,cap,flow);

  string msg;

  if (feasible && !ff.check(G,supply,cap,flow,msg))
  { LEDA_EXCEPTION(0,string("ff.check failed: ") + msg);
    cout << endl;
    node v;
    forall_nodes(v,G)
    { G.print_node(v);
      cout << string(" (%d): ",supply[v]);
      edge e;
      forall_out_edges(e,v)
      { G.print_edge(e);
        cout << string(" (%d)",flow[e]);
      }
      cout << endl;
    }
   }

  return feasible;
}


bool FEASIBLE_FLOW_OLD(graph& G, const node_array<NT>& supply,
                             const edge_array<NT>& lcap,
                             const edge_array<NT>& ucap,
                             edge_array<NT>&       flow)
{

  typedef node_array<int>    dist_array;
  typedef node_array<node>   succ_array;

  feasible_flow<NT, graph, succ_array, dist_array> ff;

  bool feasible = ff.run(G,supply,lcap,ucap,flow);

  string msg;

  if (feasible && !ff.check(G,supply,ucap,flow,msg))
    { LEDA_EXCEPTION(0,string("ff.check failed: ") + msg);
      cout << endl;
      node v;
      forall_nodes(v,G)
      { G.print_node(v);
        cout << string(" (%d): ",supply[v]);
        edge e;
        forall_out_edges(e,v)
        { G.print_edge(e);
          cout << string(" (%d)",flow[e]);
        }
        cout << endl;
      }
    }

  return feasible;
}

#endif

LEDA_END_NAMESPACE
