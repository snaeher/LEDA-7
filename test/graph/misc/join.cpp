/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  join.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main()
{
   graph G1,G2;

   test_graph(G1);
   test_graph(G2);

   G1.print("G1");
   G2.print("G2");

   G1.hide_node(G1.first_node());
   G2.hide_node(G2.first_node());

   G1.print("G1");
   G2.print("G2");

   G1.join(G2);
   G1.print("G1");
   G2.print("G2");

   G1.restore_all_nodes();
   G1.restore_all_edges();
   G1.print();

   cout << endl;

   return 0;
}

