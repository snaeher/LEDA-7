/*******************************************************************************
+
+  LEDA 7.2  
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

#include <LEDA/graph/templates/max_flow.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;




int main(int argc, char** argv)
{
  graph G;
  node s=0;
  node t=0;
  edge_array<int>     cap_i(G);

  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  read_dimacs_mf(cin,G,s,t,cap_i);
  cout << endl;

  edge_array<double>  cap_d(G);

  edge e;
  forall_edges(e,G) cap_d[e] = cap_i[e];

  edge_array<int>     flow_i(G);
  edge_array<double>  flow_d(G);


  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;


{
  float T = used_time();
  cout << "MAX_FLOW_T(int):    " << flush;
  int f = MAX_FLOW_T(G,s,t,cap_i,flow_i);
  cout << string("%6.2f sec  f = %d", used_time(T), f) << endl;
}
{
  float T = used_time();
  cout << "MAX_FLOW(int):      " << flush;
  int f = MAX_FLOW(G,s,t,cap_i,flow_i);
  cout << string("%6.2f sec  f = %d", used_time(T), f) << endl;
}
  cout << endl;
{
  float T = used_time();
  cout << "MAX_FLOW_T(double): " << flush;
  double f = MAX_FLOW_T(G,s,t,cap_d,flow_d);
  cout << string("%6.2f sec  f = %.2f", used_time(T), f) << endl;
}
{
  float T = used_time();
  cout << "MAX_FLOW(double):   " << flush;
  double f = MAX_FLOW(G,s,t,cap_d,flow_d);
  cout << string("%6.2f sec  f = %.2f", used_time(T), f) << endl;
}
  cout << endl;

  return 0;
}




