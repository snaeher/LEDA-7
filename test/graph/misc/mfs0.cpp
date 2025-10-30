/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mfs0.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/max_flow.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;




int main(int argc, char** argv)
{
  GRAPH<int,int> G(3,1);
  node s=0;
  node t=0;

  edge_array<int>&  cap = G.edge_data();
  //edge_array<int>  cap;
  //assert(cap.use_edge_data(G));

  edge_array<int>  flow;
  assert(flow.use_edge_data(G));


  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  read_dimacs_mf(cin,(graph&)G,s,t,cap);
  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;

  float T = used_time();

  cout << "MAX_FLOW    " << flush;

  int f = MAX_FLOW(G,s,t,cap,flow);

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec  f = %d", used_time(T),f) << endl;

  return 0;
}




