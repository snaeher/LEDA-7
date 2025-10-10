/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  make_simple.c
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




void Make_Simple_Undirected(graph& G)
{
  Make_Simple(G);
  Delete_Loops(G);

  edge_array<edge> rev(G);
  Is_Bidirected(G,rev);

  edge e;
  forall_edges(e,G) 
  { edge r = rev[e];
    if (r == nil) continue;
    G.del_edge(e);
    rev[r] = nil;
   }
 
  G.make_undirected();
}



main()
{
  graph G;
  test_graph(G);

  graph G1 = G;
  graph G2 = G;

  Make_Simple_Undirected(G1);
  G1.print();

  G2.make_undirected();
  Make_Simple(G2);
  G2.print();
}
