/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mf_bin.c
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

#include <LEDA/graph/templates/max_flow.h>
#include <LEDA/graph/templates/feasible_flow.h>
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
  typedef static_graph<opposite_graph,data_slots<4>,data_slots<2> > st_graph;
  typedef st_graph::node node;
  typedef st_graph::edge edge;


  st_graph G;
  node s=0;
  node t=0;

  edge_slot<int,st_graph,0>   cap(G);
  edge_slot<int,st_graph,1>  flow(G);

  typedef node_slot<node,st_graph,0> succ_array;
  typedef node_slot<int, st_graph,1> excess_array;
  typedef node_slot<int, st_graph,2> dist_array;


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


  cout << "MAX_FLOW       " << flush;

  max_flow<int,st_graph,succ_array,excess_array,dist_array> mf;

  mf.run(G,s,t,cap,flow);

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec  f = %d", mf.cpu_time(), mf.flow()) << endl;
  mf.statistics(cout);

  cout << endl;

/*

{

  float T = used_time();

  int F_s = 0;
  int F_t = 0;
  int FF = 0;

  edge e;
  forall_out_edges(e,s) F_s += cap[e];
  forall_in_edges(e,t) F_t += cap[e];

  node_slot<int,3,st_graph> supply(G,0);

  int Fmin = 0;
  int Fmax = min(F_s,F_t);

  while (Fmin <= Fmax) {

  int F = (Fmin+Fmax)/2;

cout << "F = " << F <<endl;

  supply[s] =  F;
  supply[t] = -F;

  feasible_flow<int,st_graph,succ_array,excess_array,dist_array> ff;

  bool feasible = ff.run(G,supply,cap,flow);

  cout << string("F = %8d  feasible = %d  time: %6.2f sec", F,feasible,ff.cpu_time()) << endl;

  if (feasible) 
     { FF = F;
       Fmin = F+1;
      }
  else
     Fmax = F-1;

}

  cout << endl;
  cout << string("flow = %d  time: %6.2f sec", FF, used_time(T)) << endl;
  cout << endl;
}
*/


{

  float T = used_time();

  int F_s = 0;
  int F_t = 0;
  int FF = 0;

  edge e;
  forall_out_edges(e,s) F_s += cap[e];
  forall_in_edges(e,t) F_t += cap[e];

  int Fmin = 0;
  int Fmax = min(F_s,F_t);

  while (Fmin <= Fmax) {

  int F = (Fmin+Fmax)/2;

  feasible_flow1<int,st_graph,succ_array,dist_array> ff;

  bool feasible = ff.run(G,s,t,F,cap,flow);

  cout << string("F = %8d  feasible = %d  time: %6.2f sec", F,feasible,ff.cpu_time()) << endl;

  ff.statistics(cout);
  cout << endl;

  if (feasible) 
     { FF = F;
       Fmin = F+1;
      }
  else
     Fmax = F-1;

}

  cout << endl;
  cout << string("flow = %d  time: %6.2f sec", FF, used_time(T)) << endl;
  cout << endl;
}


  return 0;
}

