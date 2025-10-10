/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _topsort.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




//------------------------------------------------------------------------------
// Topological Sorting 
//                                                                              
// S. N"aher (1989)
//------------------------------------------------------------------------------



#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/b_queue.h>


LEDA_BEGIN_NAMESPACE

bool TOPSORT(const graph& G, node_array<int>& ord)
{ 
  int n = G.number_of_nodes();

  node_array<int> INDEG(G,0);
  b_queue<node> ZEROINDEG(n);

  int count=0;
  node v,w;

  forall_nodes(v,G) 
  { int d = G.indeg(v); 
    INDEG[v] = d;
    if (d == 0) ZEROINDEG.append(v); 
   }

  while (!ZEROINDEG.empty())
   { v = ZEROINDEG.pop();
     ord[v] = ++count;
     forall_adj_nodes(w,v) 
        if (--INDEG[w]==0) ZEROINDEG.append(w);
    }
  
  return count == n; 
}
     

bool TOPSORT(const graph& G, list<node>& L)
{ 
  // computes list L of nodes in topological order

  node_array<int> INDEG(G,0);

  L.clear();

  node v,w;

  forall_nodes(v,G) 
  { int d = G.indeg(v); 
    INDEG[v] = d;
    if (d == 0) L.append(v); 
   }

  for(list_item it = L.first(); it != 0; it = L.succ(it))
  { v = L[it];
    forall_adj_nodes(w,v) 
        if (--INDEG[w]==0) L.append(w);
    }
  
  return L.length() == G.number_of_nodes(); 
}
     



// TOPSORT1 rearrange nodes and edges using bucket sort

bool TOPSORT1(graph& G)
{ 
  if (G.number_of_nodes()==0 || G.number_of_edges()==0) return true;

  node_array<int> node_ord(G);
  edge_array<int> edge_ord(G);

  if (TOPSORT(G,node_ord))
   { edge e;
     forall_edges(e,G) edge_ord[e] = node_ord[target(e)];
     G.bucket_sort_nodes(node_ord);
     G.bucket_sort_edges(edge_ord);
     return true;
    }

  return false;
}
 
LEDA_END_NAMESPACE
    

