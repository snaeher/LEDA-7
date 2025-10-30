/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _max_flow_gen.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/max_flow.h>

LEDA_BEGIN_NAMESPACE

void max_flow_gen_rand(GRAPH<int,int>& G, node& s, node& t, int n, int m)
{ G.clear();

  G.set_node_bound(n);
  G.set_edge_bound(m+2*n);

  random_graph(G,n,m);

  s = G.first_node(); t = G.last_node();

  node v; edge e;
  forall_nodes(v,G) { G.new_edge(s,v); G.new_edge(v,t); }

  forall_edges(e,G) 
  { if (G.source(e) != s) 
      G[e] = rand_int(1,10);
    else 
      G[e] = rand_int(2,11);
  }
}


void max_flow_gen_CG1(GRAPH<int,int>& G, node& s, node& t, int n)
{ 
  G.clear();

  if (n < 1) 
    LEDA_EXCEPTION(1,"max_flow_gen_CG1: n must be at least one");

  G.set_node_bound(2*n);
  G.set_edge_bound(3*n+1);
  
  array<node> V(2*n);
  int i;
  for(i = 0; i < 2*n; i++) V[i] = G.new_node(); 
  s = V[0]; t = V[2*n - 1];

  node v = V[n];

  for (i = 0; i < n; i++)
  { G.new_edge(V[i],V[i + 1], n - i);
    G.new_edge(V[i],v, 1);
  }
 
  G.new_edge(V[n - 1],V[2*n - 1], 1);
  G.new_edge(V[n - 1],V[n], 1);

  for (i = n; i <= 2*n - 2 ; i++ ) G.new_edge(V[i],V[i + 1],n);
}

void max_flow_gen_CG2(GRAPH<int,int>& G, node& s, node& t, int n)
{ G.clear();

  if (n < 1) 
    LEDA_EXCEPTION(1,"max_flow_gen_CG2: n must be at least one");

  G.set_node_bound(2*n);
  G.set_edge_bound(3*n-1);
  
  array<node> V(2*n);
  int i;
  for(i = 0; i < 2*n; i++) V[i] = G.new_node(); 
  s = V[0]; t = V[2*n-1];

  for (i = 0; i < n; i++ ) G.new_edge(V[i],V[2*n - 1 - i], 1);

  for (i = 0; i <= n - 1; i++ ) G.new_edge(V[i],V[i + 1], 2*n);

  for (i = n; i <= 2*n - 2; i++ ) G.new_edge(V[i],V[i + 1], n);
}


void max_flow_gen_AMO(GRAPH<int,int>& G, node& s, node& t, int n)
{ G.clear();

  if (n < 1) 
    LEDA_EXCEPTION(1,"max_flow_gen_AMO: n must be at least one");

  G.set_node_bound(n+1);
  G.set_edge_bound(2*n-2);
  
  
  array<node> V(n);
  s = G.new_node();
  int i;
  for(i = 0; i < n; i++) V[i] = G.new_node(); 
  t = G.last_node();

  for (i = n - 2; i >= 0; i-- )
  { G.new_edge(s,V[i], 10000);
    G.new_edge(V[i],V[i + 1], 1);
  }
}

LEDA_END_NAMESPACE
