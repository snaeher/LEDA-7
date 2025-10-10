/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _dfs.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// Depth First Search 
//------------------------------------------------------------------------------

#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

static void dfs(node s, node_array<bool>& reached, list<node>& L)
{
   L.append(s);
   reached[s] = true; 
   node v;
   forall_adj_nodes(v,s)
      if ( !reached[v] ) dfs(v,reached,L);
 }


list<node> DFS(const graph&, node v, node_array<bool>& reached)
{ 
  list<node>    L;
  dfs(v,reached,L);
  return L;
} 

LEDA_END_NAMESPACE
