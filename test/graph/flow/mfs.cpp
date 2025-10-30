/*******************************************************************************
+
+  LEDA 7.2.2  
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

#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot.h>

#include "incl/max_flow.h"


using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;


int main(int argc, char** argv)
{
  typedef static_graph<opposite_graph,data_slots<3>,data_slots<2> > st_graph;
  typedef st_graph::node node;
  typedef st_graph::edge edge;


  st_graph G;
  node s=0;
  node t=0;

  edge_slot<int,st_graph,0>   cap(G);
  edge_slot<int,st_graph,1>  flow(G);

  typedef node_slot1<node,st_graph,0> succ_array;
  typedef node_slot1<int, st_graph,1> excess_array;
  typedef node_slot1<int, st_graph,2> dist_array;

/*
  edge_array<int,st_graph>   cap(G);
  edge_array<int,st_graph>  flow(G);

  typedef node_array<node,st_graph> succ_array;
  typedef node_array<int, st_graph> excess_array;
  typedef node_array<int, st_graph> dist_array;
*/


  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);

/*
  flow.init(G);
*/

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;
  cout << string("bw = %.0f",G.bandwidth()) << endl;
  cout << endl;

/*
  G.permute_in_edges();
  cout << string("bw = %.0f",G.bandwidth()) << endl;
  cout << endl;
*/


  cout << "MAX_FLOW    " << flush;

  max_flow<int,st_graph,succ_array,excess_array,dist_array> mf;

  if (argc >= 2 && string(argv[1]) != "-p")
     mf.set_heuristic(atof(argv[1]));

  mf.run(G,s,t,cap,flow);

  cout << endl;
  cout << string("%5.2f s  f = %d  ", mf.cpu_time(), mf.flow());
  cout << string("pushes:%8d  ", mf.pushes(0));
  cout << string("relabels:%8d  ", mf.relabels(0));
  cout << string("updates:%3d",  mf.updates(0));
  cout << endl;

  mf.statistics(cout);

  //mf.print(G,s,t,cap,flow);

  string msg;
  if (!mf.check(G,s,t,cap,flow, msg))
  { cerr << msg << endl;
    ofstream os("err.dimacs");
    write_dimacs_mf(os,G,s,t,cap);
    return 1;
   }

  if (argc >= 2 && string(argv[1]) == "-p")
      write_dimacs_mf(cout,G,s,t,cap);

  return 0;
}




