/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  graph_read.c
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
  GRAPH<GRAPH<float,float>, list<int> > G;

  int n = G.read(read_string("read graph from file: "));

  cout << "n = " << n << "\n";

  G.print();

  return 0;
}
