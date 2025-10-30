/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _kuratowski.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

static void dfs(const graph& G, node v, edge_array<bool>& marked,
                                        node_array<int>&  deg)
{ edge e;
  forall_inout_edges(e,v)
    if (marked[e])
    { marked[e] = false;
      node w = G.opposite(v,e);
      if (deg[w] == 2) dfs(G,w,marked,deg);
      else deg[w] = -3;
     }
}


int KURATOWSKI(graph& G, list<node>& V, list<edge>& E, node_array<int>& deg)
{ 
  // deg[v] = 0     not in Kuratowski Subdivison
  //          2     subdivison point
  //          3(-3) sides L(R) of K33
  //          4     nodes of K5

  if (PLANAR(G,E,false)) return 0;

  edge_array<bool> marked(G,false);
  deg.init(G,0);

  edge e;
  forall(e,E) 
  { marked[e] = true;
    if (++deg[source(e)] == 1) V.append(source(e));
    if (++deg[target(e)] == 1) V.append(target(e));
   }

  node v;
  forall(v,V)
    if (deg[v] == 3) 
    { dfs(G,v,marked,deg);
      return 1;
     }

  return 2;
}



/*
static void dfs(const graph& G, node v, node_array<bool>& visited)
{ edge e;
  forall_inout_edges(e,v)
  { node w = G.opposite(v,e);
    if (!visited[w]) 
    { visited[w] = true;
      if (G.degree(w) == 2) dfs(G,w,visited);
     }
  }
}


static void bfs(const graph& G, node s, node_array<edge>& pred)
{ list<node> Q(s);
  pred[s] = G.first_edge(); //dummy
  for (list_item it = Q.first(); it; it = Q.succ(it))
  { node v = Q[it];
    edge e;
    forall_inout_edges(e,v)
    { node w = G.opposite(v,e);
      if (pred[w] == nil) 
      { Q.append(w); 
        pred[w] = e;
       }
     }
   }
}
*/

LEDA_END_NAMESPACE
