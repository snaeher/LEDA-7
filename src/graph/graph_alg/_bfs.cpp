/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _bfs.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// Breadth First Search
//                                                                              
// S. N"aher (1995)
//------------------------------------------------------------------------------



#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

list<node> BFS(const graph&, node s, node_array<int>& dist)
{ 
  // performs a breadth first seach on the subgraph induced
  // by all nodes v with dist[v] < 0

  list<node> Q(s);
  dist[s] = 0;
  for (list_item it = Q.first(); it; it = Q.succ(it))
  { node v = Q[it];
    node w;
    forall_adj_nodes(w,v)
      if (dist[w] < 0) 
        { Q.append(w); 
          dist[w] = dist[v]+1;
         }
   }
  return Q;
}



list<node> BFS(const graph& G, node s, node_array<int>& dist, 
                                       node_array<edge>& pred)
{ list<node> Q(s);
  dist[s] = 0;
  pred[s] = nil;
  for (list_item it = Q.first(); it; it = Q.succ(it))
  { node v = Q[it];
    edge e;
    forall_adj_edges(e,v)
    { node w = G.opposite(v,e);
      if (dist[w] < 0) 
      { Q.append(w); 
        dist[w] = dist[v]+1;
        pred[w] = e;
       }
     }
   }
  return Q;
}

LEDA_END_NAMESPACE
