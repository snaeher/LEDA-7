/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  mcb_dfs_vs_bfs.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:25 $


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_gen.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/IO_interface.h>

#include <assert.h>
#include <math.h>

using namespace leda;

IO_interface I("Maximum Cardinality Matching: Depth-First vs Breadth-First Search");

int N = 10000;


int main()
{

I.write_demo("This demo illustrates that breadth-first search is the \
superior search method in bipartite matching algorithms. We use random \
group graph (see the LEDA book for a definition).");

#ifdef BOOK
int n = N;
for (int m = 1.5*n; m <= 4*n; m = m + n)
for (int k = 1; k <= 10000; k = 10*k)
#else
int n = I.read_int("n = ");
int m = I.read_int("m = ");
int k = I.read_int("k = ");
#endif
  { graph G;
    list<node> A,B;

   I.write_table("\n", n); I.write_table(" & ",m);

    random_bigraph(G,n,n,m,A,B,k);
    edge_array<int>  edge_number(G);
    edge e; int i = 0;
    forall_edges(e,G) edge_number[e] = i++;
    G.sort_edges(edge_number);
    node_array<bool> NC(G);

    float T = used_time(); float UT;
    list<edge> M;
    
    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,false,false);
    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford Fulkerson with DFS, no heuristic ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,true,false);
    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford Fulkerson with DFS and heuristic ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,false,true);
    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford Fulkerson with BFS, no heuristic",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,true,true);
    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford Fulkerson with BFS and heuristic ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    I.write_table(" \\\\ \\hline ");

  }
return 0;
}




