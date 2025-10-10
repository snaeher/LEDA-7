/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _components.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Connected Components
//                                                                              
// S. N"aher (1989)
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/stack.h>

LEDA_BEGIN_NAMESPACE


static void dfs(const graph& G, node v, node_array<int>& compnum, int count)
{ 
  stack<node>  S;
  S.push(v);

  compnum[v] = count;

  while (!S.empty())
  { v = S.pop(); 

    edge e;

    //forall_adj_undirected_edges(e,v) // treats G like an undirected graph
    list<edge> L = G.adj_edges(v);
    if (G.is_directed()) {
       forall_in_edges(e,v) L.append(e);
    }

    forall(e,L) 
    { node w = G.opposite(v,e);
      if (compnum[w] == -1)
      { compnum[w] = count;
        S.push(w);
       }
     }


   }
 
} 


int COMPONENTS(const graph& G, node_array<int>& compnum)
{ // computes the connected components of the underlying undirected graph
  // compnum[v] = i  iff v in component i
  // number of components is returned

  node v;
  forall_nodes(v,G) compnum[v] = -1;

  int count = 0;
  forall_nodes(v,G) 
    if (compnum[v] == -1) dfs(G,v,compnum,count++);

  return count;
}



/*
int COMPONENTS1(const graph& G, node_array<int>& compnum)
{ 
  // an alternative implementation using node partitions (union-find)

  node_partition P(G);
  edge e;
  node v;

  forall_nodes(v,G) compnum[v] = -1;

  forall_edges(e,G) P.union_blocks(source(e),target(e));

  int count = 0;
  forall_nodes(v,G) 
  { node w = P.find(v);
    if (compnum[w]==-1) compnum[w] = count++;
    compnum[v] = compnum[w];
   }

  return count;
}
*/


LEDA_END_NAMESPACE

