/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _BF_GEN.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/array.h>
#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

void BF_GEN(GRAPH<int,int>& G, int n, int m,
                   bool non_negative)
{ G.clear();
 
  int K = 1; while ( (K+1)*(K+1) <= m/2 ) K++;
  int l = 0; int L = 1;
  while ( 2*L <= n/2 ) {l++; L = 2*L; }

  array<node> V(n);
  int i, j;
  for (i = 0; i < n; i++) V[i] = G.new_node(i);

  for (i = L - 1; i < L - 1 + K; i++)
    for (j = L - 1; j < L - 1 + K; j++)
      G.new_edge(V[i], V[j], 0);

  for (i = 0; i < L - 1; i++) G.new_edge(V[i], V[i+1], 0);

  G.new_edge(V[0],V[L-1],(non_negative? L-1-1  :-1));

  int powj = 1;
  for (j = 0; j < l-1; j++)
  { int x = L - L/powj;
    int y = L - L/(2*powj);
    G.new_edge(V[x],V[y],  (non_negative? y-x-1 :   -1));
    G.new_edge(V[y],V[L-1],(non_negative? L-1-y-1 : -1));
    powj *= 2;
  }
}

LEDA_END_NAMESPACE
