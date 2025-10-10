/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  ffs.c
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

#include <LEDA/graph/templates/feasible_flow.h>

#include <LEDA/system/assert.h>


using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;

int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph,data_slots<4>,data_slots<4> > st_graph;
  typedef st_graph::node node;
  typedef st_graph::edge edge;


  st_graph G;

  edge_slot<int,st_graph,0>  ucap(G);
  edge_slot<int,st_graph,1>  lcap(G);
  edge_slot<int,st_graph,2>  cost(G);
  edge_slot<int,st_graph,3>  flow(G);

  node_slot<int,st_graph,0>  supply(G);

  typedef node_slot<node,st_graph,1> succ_array;
  typedef node_slot<int, st_graph,2> dist_array;

  cout << endl;
  cout << "reading DIMACS min-cost-flow problem from std input ... ";
  cout << flush;

  dimacs_mcf D(cin);
  D.translate(G,supply,lcap,ucap,cost);

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  int stotal = 0;
  int scount = 0;

  int dtotal = 0;
  int dcount = 0;

  node v;
  forall_nodes(v,G)
  {  if (supply[v] > 0) { scount++; stotal += supply[v]; }
     if (supply[v] < 0) { dcount++; dtotal += supply[v]; }
   }


  cout << string("%8d nodes",n) << endl;
  cout << string("%8d edges",m) << endl;
  cout << string("%8d supply (+%d)",scount,stotal) << endl;
  cout << string("%8d demand (%d)",dcount,dtotal) << endl;
  cout << endl;

  cout << "FEASIBLE FLOW:   " << flush;

  feasible_flow<int,st_graph,succ_array,dist_array> ff;


  bool feasible = ff.run(G,supply,lcap,ucap,flow);

  if (!feasible)
      cout << string("infeasible       time: %.2f sec", ff.cpu_time()) << endl;
  else
   { double C = 0;

     edge e;
     forall_edges(e,G) C += double(cost[e])*flow[e];
   
     cout << string("total cost: %.2f  time: %.2f sec", C,ff.cpu_time());
     cout << endl;
   
     ff.statistics(cout);
   
     string msg;
     if (!ff.check(G,supply,ucap,flow,msg))
     { cerr << msg << endl;
       return 1;
      }
   }
   
  return 0;
}

