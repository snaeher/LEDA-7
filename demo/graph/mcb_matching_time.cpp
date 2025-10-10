/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  mcb_matching_time.c
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
#include <LEDA/core/IO_interface.h>

#include <assert.h>
#include <math.h>

using namespace leda;

IO_interface I("Maximum Cardinality Matching: A Comparison");

int N = 40000;


int main()
{
I.write_demo("We compare the running time of different bipartite matching algorithms on random group graph (see the LEDA book for a definition).");
#ifdef BOOK
for (int n = N; n <= 2*N; n = 2*n)
for (int m = 2*n; m <= 4*n; m = m + n)
for (int k = 1; k <= 10000; k = 100*k)
#else
int n = I.read_int("n = ");
int m = I.read_int("m = ");
int k = I.read_int("k = ");
#endif
  { graph G;
    list<node> A,B;
    random_bigraph(G,n,n,m,A,B,k);

    I.write_table("\n", n/10000); I.write_table(" & ",m/10000);
    I.write_table(" & ",k);

    edge_array<int>  edge_number(G);
    edge e; int i = 0;
    forall_edges(e,G) edge_number[e] = i++;
    G.sort_edges(edge_number);
    node_array<bool> NC(G);

    float T = used_time(); float UT;
    list<edge> M;

    
    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,false,true);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford-Fulkerson (BFS) without heuristic   ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,true,true);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Ford-Fulkerson (BFS) with    heuristic ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 


    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_HK(G,A,B,NC,false);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Hopcroft-Karp without heuristic   ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_HK(G,A,B,NC);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Hopcroft-Karp with    heuristic   ",UT);

    CHECK_MCB(G,M,NC);
    G.sort_edges(edge_number);  

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_ABMP(G,A,B,NC,false);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Alt-Blum-Mehlhorn-Paul without heuristic",UT);

    CHECK_MCB(G,M,NC); 

    used_time(T);
    M = MAX_CARD_BIPARTITE_MATCHING_ABMP(G,A,B,NC);

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("Alt-Blum-Mehlhorn-Paul with heuristic",UT);

    CHECK_MCB(G,M,NC); 

    used_time(T);
    CHECK_MCB(G,M,NC); 

    UT = used_time(T);
    I.write_table(" & ",UT);
    I.write_demo("time for check",UT);

    I.write_table(" \\\\ \\hline ");

  }
return 0;
}

