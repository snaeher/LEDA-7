/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mfs.c
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

#include <LEDA/graph/templates/feasible_flow1.h>


using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;

int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph,data_slots<2>,data_slots<2> > st_graph;
  typedef st_graph::node node;
  typedef st_graph::edge edge;


  st_graph G;
  node s=0;
  node t=0;

  edge_slot<int,st_graph,0>   cap(G);
  edge_slot<int,st_graph,1>  flow(G);

  typedef node_slot<node,st_graph,0> succ_array;
  typedef node_slot<int, st_graph,1> dist_array;


  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;


  cout << "MAX_FLOW    " << flush;

  feasible_flow<int,st_graph,succ_array,dist_array> ff;

  ff.maxflow(G,s,t,cap,flow);

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec  f = %d", ff.cpu_time(), ff.flow()) << endl;

  ff.statistics(cout);

  return 0;
}




