/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _dfsnum.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Depth First Search Numbering 
//                                                                              
// S. N"aher (1989)
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

static int count1;
static int count2;

static void dfs(node v, node_array<int>& dfsnum, 
                        node_array<int>& compnum,
                        list<edge>& T )
{ dfsnum[v] = ++count1;

  edge e;
  forall_adj_edges(e,v) 
    { node w = target(e);
      if (dfsnum[w] == 0) 
       { T.append(e);
         dfs(w,dfsnum,compnum,T);
        }
     }

  compnum[v] = ++count2;
} 


list<edge> DFS_NUM(const graph& G, node_array<int>& dfsnum, 
                                   node_array<int>& compnum)
{ 
  list<edge> T;
  count1 = 0;
  count2 = 0;

  dfsnum.init(G,0);

  node v;
  forall_nodes(v,G) 
    if (dfsnum[v] == 0) 
      dfs(v,dfsnum,compnum,T);

  return T;
}

LEDA_END_NAMESPACE
