/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mcf_primal_dual.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/system/basic.h>

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/core/b_stack.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

typedef int num_type;

bool MCF_PRIMAL_DUAL(graph& G0, const edge_array<num_type>& cap,
                                const edge_array<num_type>& cost,
                                const node_array<num_type>& supply,
                                edge_array<num_type>&       flow)
{

  GRAPH<node,edge> G;

  CopyGraph(G,G0);

  list<edge> E = G.all_edges();

  edge e;

  forall(e,E)
  { node u = source(e);
    node v = target(e);
    edge r = G.new_edge(v,u,G[e]);
    G.set_reversal(e,r);
  }

  node s = G.new_node();
  node t = G.new_node();

  node v;
  forall_nodes(v, G) 
  { if (v == t || v == s) continue;
    num_type b = supply[G[v]];
    if ( b > 0) G.new_edge(s,v,0);
    if ( b < 0) G.new_edge(v,t,0);
  }

  list<edge> E1 = G.all_edges();

  edge_array<num_type> r_cap(G,0);
  edge_array<num_type> r_cost(G,0);


  forall(e,E) r_cap[e] = cap[G[e]];

  num_type excess_s = 0;

  forall_out_edges(e,s) 
  { num_type b =  supply[G[target(e)]];
    r_cap[e] =  b;
    excess_s += b;
   }

  forall_in_edges(e,t) 
    r_cap[e] = -supply[G[source(e)]];


  forall(e,E) 
  { edge r = G.reversal(e);
    num_type c = cost[G[e]];
    r_cost[e] =  c;
    r_cost[r] = -c;
    assert(r_cap[r] == 0);
  }


  for(;;)
  { 
    edge e;
    forall_edges(e,G) 
       if (r_cap[e] == 0) G.hide_edge(e);
       //else assert(r_cost[e] >= 0);

    node_array<num_type> r_dist(G,0);
    DIJKSTRA(G,s,r_cost,r_dist);

    forall(e,E1) 
    { node u = source(e);
      node v = target(e);
      num_type rc = r_cost[e] + r_dist[u] - r_dist[v];
      r_cost[e] = rc;
      if (rc != 0) G.hide_edge(e);
     }

    edge_array<num_type> r_flow(G,0);
    num_type mf = MAX_FLOW(G,s,t,r_cap,r_flow);

    excess_s -= mf;

    if (mf == 0) break;

    forall_edges(e,G)
    { num_type f = r_flow[e];
      r_cap[e] -= f;
      if (source(e) != s && target(e) != t) r_cap[G.reversal(e)] += f;
     }

    G.restore_all_edges();
  } 

  assert(excess_s == 0);

  forall(e,E) 
  { edge x = G[e];
    flow[x] = cap[x] - r_cap[e];
    assert(flow[x] >= 0);
    assert(flow[x] <= cap[x]);
   }

  forall_nodes(v,G0)
  { num_type b = 0;
    edge e;
    forall_out_edges(e,v) b += flow[e];
    forall_in_edges(e,v) b -= flow[e];
    assert(supply[v] == b);
   }

    
  return excess_s == 0;
}



#define RCOST(e) \
(G[e] ?   (cost[G[e]] - r_pot[source(e)] + r_pot[target(e)]) : \
         (-cost[G[G.reversal(e)]] - r_pot[source(e)] + r_pot[target(e)]))

#define RCAP(e) \
(G[e] ? (cap[G[e]] - flow[G[e]]) : flow[G[G.reversal(e)]])



bool MCF_PRIMAL_DUAL1(graph& G0, node s0, node t0, num_type supply,
                                 const edge_array<num_type>& cap,
                                 const edge_array<num_type>& cost,
                                 edge_array<num_type>& flow)
{

  flow.init(G0,0);

  GRAPH<node,edge> G;

  CopyGraph(G,G0);

  node s = G.first_node();
  while (G[s] != s0) s = G.succ_node(s); 

  node t = G.first_node();
  while (G[t] != t0) t = G.succ_node(t); 


  list<edge> E = G.all_edges();

  edge e;

  forall(e,E)
  { node u = source(e);
    node v = target(e);
    edge r = G.new_edge(v,u,0);
    G.set_reversal(e,r);
  }

  num_type excess_s = supply;


  node_array<num_type> r_pot(G,0);

  for(;;)
  { 
    edge e;
    forall_edges(e,G) 
       if (RCAP(e) == 0) G.hide_edge(e);
       //else assert(RCOST(e) >= 0);


    { edge_array<num_type> r_cost(G,0);
      node_array<num_type> r_dist(G,0);

      edge e;
      forall_edges(e,G) r_cost[e] = RCOST(e);
        
      DIJKSTRA(G,s,r_cost,r_dist);
      node v;
      forall_nodes(v,G) r_pot[v] -= r_dist[v];

      //forall_edges(e,G) assert(RCOST(e) >= 0); 
     }


    forall_edges(e,G) 
      if (RCOST(e) != 0) G.hide_edge(e);


    edge_array<num_type> r_flow(G,0);

    { edge_array<num_type> r_cap(G,0);
      edge e;
      forall_edges(e,G) r_cap[e] = RCAP(e);
      num_type mf = MAX_FLOW(G,s,t,r_cap,r_flow);
      excess_s -= mf;
      if (mf == 0) break;
     }

    forall_edges(e,G)
    { num_type f = r_flow[e];
      edge x = G[e];
      if (x) flow[x] += f;
      else   flow[G[G.reversal(e)]] -= f;
    }

    G.restore_all_edges();
  } 

  return excess_s == 0;
}






LEDA_END_NAMESPACE
