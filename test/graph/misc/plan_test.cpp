/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  plan_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main()
{
  graph G;
  
  int n = read_int("n = ");

  bool embed = Yes("embed (y/n): ");
  int  i = 0;


  for(;;)
  {
   random_graph(G,n,rand_int(n/3,n));

   if (embed)
   { Make_Biconnected(G);
     Make_Bidirected(G);
    }

   list<edge> el = G.all_edges();

   float t;
   float T;

   cout << i++ << endl;
   cout << "Hocroft/Tarjan:  " << flush;
   T = used_time();
   bool  p0 = HT_PLANAR(G,embed);  
   t = used_time(T);
   cout << ( p0 ? "    planar" : "non-planar");
   cout << string("%5.2f ",t) << endl;
 
   G.sort_edges(el);
 
   cout << "Booth/Luecker:   " << flush;
   T = used_time();
   bool  p1 = BL_PLANAR(G,embed);  
   t = used_time(T);
   cout << ( p1 ? "    planar" : "non-planar");
   cout << string("%5.2f ",t) << endl;
 
   if (p1 != p0 ) 
   { G.write("plan_test.lgr");
     error_handler(1,"plan_test: saved graph in `plan_test.lgr'");
    }
 
   cout << endl;

 }

  return 0;
}
