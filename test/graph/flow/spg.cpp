/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  spg.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>

#include <LEDA/graph/templates/dijkstra.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  graph G(3,2);
  node s = 0;

  edge_slot<int,graph,0>  cost;
  node_slot<int,graph,1>  dist;
  node_slot<edge,graph,2> pred;
  
  cout << endl;
  cout << "leda::graph(3,2)" << endl;
  cout << "reading DIMACS shortest path problem from std input ... ";
  cout << flush;
  
  dimacs_sp DSP(cin);
  DSP.translate(G,s,cost);
  
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
  float T = used_time();
  
  dijkstra<int,graph> D;
  D.run(G,s,cost,dist,pred);
  
  double d = 0;
  
  forall_nodes(v,G) d += dist[v];
  
  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec %f |DIST|",used_time(T),d) << endl;
  cout << endl;

  return 0;
}




