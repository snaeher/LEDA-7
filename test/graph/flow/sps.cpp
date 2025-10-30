/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sps.c
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
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>

#define DIJKSTRA_DIRECTED
#include <LEDA/graph/templates/dijkstra.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph,data_slots<2>,data_slots<1> >  st_graph;
  
  typedef st_graph::node node;
  typedef st_graph::edge edge;

  st_graph G;
  node s = 0;

  edge_slot<int,st_graph,0>  cost;
  node_slot<int,st_graph,0>  dist;
  node_slot<edge,st_graph,1> pred;
  
  cout << endl;
  cout << "s_graph<opposite_graph,3,1>" << endl;
  cout << "reading DIMACS shortest path problem from std input ... ";
  cout << flush;
  
  float T = used_time();
  dimacs_sp DSP;
  DSP.read(cin);
  DSP.translate(G,s,cost);
  cout << string("(%.2f sec)",used_time(T)) << endl;
  
  cout << endl;
  
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << endl;
  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;

  node v;
  forall_nodes(v,G) pred[v] = nil;
 
  cout << "DIJKSTRA    " << flush;
  T = used_time();

  dijkstra<int,st_graph> D;
  D.run(G,s,cost,dist,pred);
  
  double d = 0;
  
  forall_nodes(v,G) d += dist[v];
  
  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec   d = %.0f",used_time(T),d) << endl;
  cout << endl;

  return 0;
}




