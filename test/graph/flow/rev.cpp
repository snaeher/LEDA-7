/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  rev.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>

#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>



using namespace leda;

using std::cout;
using std::cin;

int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph,data_slots<0>,data_slots<1> > st_graph;
  typedef st_graph::node node;

  st_graph G;
  node s=0;
  node t=0;
  edge_slot<int,st_graph,0>   cap(G);

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  write_dimacs_mf(cout,G,s,t,cap);

  return 0;
}




