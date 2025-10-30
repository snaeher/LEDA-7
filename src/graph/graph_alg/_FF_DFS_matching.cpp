/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _FF_DFS_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

static int number_of_augmentations;

static bool find_aug_path_by_dfs(graph& G, edge f, 
             node_array<bool>& free, node_array<int>& mark)
{ node w = G.target(f);
  mark[w] = number_of_augmentations;
  if (free[w]) 
  { free[w] = false;
    G.rev_edge(f);
    return true;
  }
  
  edge e;
  forall_adj_edges(e,w)
  { node z = G.target(e);
    if ( mark[z] == number_of_augmentations ) continue;
    if ( find_aug_path_by_dfs(G,e,free,mark))
    { G.rev_edge(f); 
      return true; 
    }
  }
  return false;
} 


list<edge> MAX_CARD_BIPARTITE_MATCHING_FF_DFS(graph& G, 
                                   const list<node>& A, 
                                   const list<node>& B,
                                  node_array<bool>& NC)
{ node v; edge e;
  node_array<bool> free(G,true);
  node_array<int> mark(G,-1);

  // check that all edges are directed from A to B
  forall(v,B) assert(G.outdeg(v) == 0);

  
  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if ( free[v] && free[w] )
    { free[v] = free[w] = false;
      G.rev_edge(e);
    }
  }


  number_of_augmentations = 0;
  forall(v,A)
  { if ( !free[v] ) continue;
    edge f; 
    forall_adj_edges(f,v)
    { if (find_aug_path_by_dfs(G,f,free,mark)) 
      { free[v] = false;
        number_of_augmentations++ ;
        break;
      }
    }
  }

  
  list<edge> result;

  forall(v,B) 
   forall_adj_edges(e,v) result.append(e);

  forall_nodes(v,G) NC[v] = false;
  node_array<bool> reachable(G,false);
  forall(v,A)
    if (free[v]) DFS(G,v,reachable);
  forall(e,result)
    if ( reachable[G.source(e)] )
      NC[G.source(e)] = true;
    else
      NC[G.target(e)] = true;

  forall(e,result) G.rev_edge(e);

  return result;

}  

LEDA_END_NAMESPACE
