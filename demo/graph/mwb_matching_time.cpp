/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  mwb_matching_time.c
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
#include <LEDA/graph/templates/mwb_matching.h>

#include <LEDA/core/IO_interface.h>
#include <LEDA/core/random.h>

#include <assert.h>

using namespace leda;

using std::cout;
using std::endl;



int main()
{ GRAPH<int,int> G;
 list<node> A,B;
/*
 int N = 20000;  
*/
 IO_interface I("Weighted Bipartite Matching");

 I.write_demo("We illustrate the speed of our bipartite weighted matching algorithms."); 
 I.write_demo("Please start with n and m below ten thousand.");

 cout.precision(4);
 int R = 1;  float T0,T1,T2,TC,TCARD ; T0 = T1 = T2 = TC = TCARD = 0;
 string s;
#ifdef BOOK
 R = 10;
 for (int C = 1; C <= 3; C++)
 for (int n = N; n <= 2*N; n = 2*n )
 for (int m = 2*n; m <= 4*n; m = m + n )
 { T0 = T1 = T2 = TC = TCARD = 0;
 for (int r = 0; r < R; r++)  // do R repetitions
#else
 int n = I.read_int("n = ");
 int m = I.read_int("m = ");
 int C = I.read_int("1 for uniform weights, \n2 for random weights in [1..1000], \n3 for random weights in [10000.. 10005]\n");
#endif
 { random_bigraph(G,n,n,m,A,B);
   edge e;
   switch(C)
   { case 1: { s = "U"; forall_edges(e,G) G[e] = 1; break; }
     case 2: { s = "R"; forall_edges(e,G) G[e] = rand_int(1,1000); break; }
     default: { s = "L"; forall_edges(e,G) G[e] = rand_int(10000,10005); break; }
   }
  
   edge_array<int> edge_number(G);
   { edge e;
     int i = 0;
     forall_edges(e,G) edge_number[e] = i++;
   }
   G.sort_edges(edge_number); 
   list<edge> M;

  
   I.write_demo("weights = "+ s);

   float T = used_time();
      
   //int which_heuristic = 0;   // todo  das ist globale Variable von MAX_WE

   M = MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,G.edge_data(),G.node_data());
   T0 += used_time(T);

   I.write_demo("time with no heuristic =      ",T0);
   assert(CHECK_MWBM_T(G,G.edge_data(),M,G.node_data()));
   G.sort_edges(edge_number); 
   used_time(T);

   //which_heuristic = 1;
   M = MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,G.edge_data(),G.node_data());
   T1 +=used_time(T);

   I.write_demo("time with simple heuristic =      ",T1);

   assert(CHECK_MWBM_T(G,G.edge_data(),M,G.node_data()));
   G.sort_edges(edge_number); 
   used_time(T);

   //which_heuristic = 2;
  
   M = MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,G.edge_data(),G.node_data());
   T2 += used_time(T);
 
   I.write_demo("time with refined heuristic =      ",T2);
   assert(CHECK_MWBM_T(G,G.edge_data(),M,G.node_data()));
   G.sort_edges(edge_number); 
   used_time(T);



   assert(CHECK_MWBM_T(G,G.edge_data(),M,G.node_data()));
   TC += used_time(T);
  
   I.write_demo("check =      ",TC/R);  

   node_array<bool> NC(G);
   used_time(T);
   M = MAX_CARD_BIPARTITE_MATCHING(G,A,B,NC); TCARD += used_time(T);
  
   I.write_demo("time for max cardinality =      ",TCARD);
   
 }
#ifdef BOOK
  I.write_table("\n " + s);
  I.write_table(" & ",n);
  I.write_table(" & ",m);
  I.write_table(" & ", T0/R);   
  I.write_table(" & ", T1/R);  
  I.write_table(" & ", T2/R);  
  I.write_table(" & ", TC/R);  
  I.write_table(" & ", TCARD/R," \\\\ \\hline");
 }
#endif
return 0;
}

