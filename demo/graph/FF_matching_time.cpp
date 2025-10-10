/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  FF_matching_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:22 $


#include <LEDA/graph/graph_alg.h>
#include <assert.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


static bool use_heuristic = false;


static bool find_aug_path_by_dfs(graph& G, edge f, 
                     node_array<bool>& free, node_array<bool>& reached)
{ node w = G.target(f);
  reached[w] = true;
  if (free[w]) 
  { free[w] = false;
    G.rev_edge(f);
    return true;
  }
  
  edge e;
  forall_adj_edges(e,w)
  { node z = G.target(e);
    if ( reached[z] ) continue;
    if ( find_aug_path_by_dfs(G,e,free,reached) )
    { G.rev_edge(f);
      return true; 
    }
  }
  return false;
} 


list<edge> MAX_CARD_BIPARTITE_MATCHING_FFBT(graph& G, 
                                const list<node>& A, 
                                const list<node>& B,
                               node_array<bool>& NC)
{ node v; edge e;
  node_array<bool> free(G,true);

if (use_heuristic)
{  
   forall_edges(e,G)
   { node v = G.source(e);
     node w = G.target(e);
     if ( free[v] && free[w] )
     { free[v] = free[w] = false;
       G.rev_edge(e);
     }
   }
 }
  
  forall(v,A)
  { if (!free[v]) continue;
    edge f; 
    node_array<bool> reached(G,false);
    forall_adj_edges(f,v)
    { if (find_aug_path_by_dfs(G,f,free,reached)) 
      { free[v] = false;
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


list<edge> MAX_CARD_BIPARTITE_MATCHING_FF_DFST(graph& G, 
                              const list<node>& A, const list<node>& B,
                              node_array<bool>& NC)
{ node v; edge e;
  node_array<bool> free(G,true);
  node_array<int> mark(G,-1);

if (use_heuristic)
{  
   forall_edges(e,G)
   { node v = G.source(e);
     node w = G.target(e);
     if ( free[v] && free[w] )
     { free[v] = free[w] = false;
       G.rev_edge(e);
     }
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




int main(){

IO_interface I("Maximum Cardinality Matching: Ford and Fulkerson");

I.write_demo("A comparison of the basic and the refined version of the \
Ford and Fulkerson Matching Algorithm.");


int N = I.read_int("N = ",1000);
 
for (int n = N; n < 10*N; n = 2*n)
for (int m = 2*n; m < 10*n; m = 2*m)
{ graph G;
  list<node> A,B;
  random_bigraph(G,n,n,m,A,B);
  list<edge> M;

  I.write_table("\n", n); I.write_table(" & ",m);
  I.write_demo("n = ",n); I.write_demo("m = ",m);

  node_array<bool> NC(G);

/*
  float FFB, FFBW, FF, FFW;
*/

  float T = used_time(); float UT;
 
  use_heuristic = false;

  
  M = MAX_CARD_BIPARTITE_MATCHING_FFBT(G,A,B,NC);
  UT = used_time(T);
  I.write_table(" & ",UT);
  I.write_demo("time for basic algorithm without heuristic =",UT);
  CHECK_MCB(G,M,NC); 
     
  use_heuristic = true;
  used_time(T);
  M = MAX_CARD_BIPARTITE_MATCHING_FFBT(G,A,B,NC);
    UT = used_time(T);
  I.write_table(" & ",UT);
  I.write_demo("time for basic algorithm with heuristic =",UT);
  CHECK_MCB(G,M,NC); 

  use_heuristic = false;

  used_time(T);
  M = MAX_CARD_BIPARTITE_MATCHING_FF_DFST(G,A,B,NC);
  UT = used_time(T);
  I.write_table(" & ",UT);
  I.write_demo("time for refined algorithm without heuristic =",UT);

  CHECK_MCB(G,M,NC);  
     
  use_heuristic = true;
  used_time(T);
  M = MAX_CARD_BIPARTITE_MATCHING_FF_DFST(G,A,B,NC);
  UT = used_time(T);
  I.write_table(" & ",UT);
  I.write_demo("time for basic algorithm with heuristic =",UT);

  CHECK_MCB(G,M,NC); 
  UT = used_time(T);
  I.write_table(" & ",UT," \\\\ \\hline ");
  I.write_demo("time for check =",UT); 

  I.stop(); 
}
return 0;
}
 

