/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gg.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  graph G;

  if (argc < 2)
  { cerr << "usage: gg n" << endl;
    return 1;
   }

  int n = atoi(argv[1]);

  rand_int.set_seed(n);
  grid_graph(G,n);

  edge_array<int> cap(G,0);

  node s = G.first_node();
  node t = G.last_node();

  edge e;
  forall_edges(e,G) cap[e] = rand_int(1,1000);

  Write_Dimacs_MF(cout,G,s,t,cap);

  return 0;
}

