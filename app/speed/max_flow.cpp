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
#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>
#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>
#include <LEDA/graph/node_map.h>

#define MF_STATIC_GRAPH
#include <LEDA/graph/templates/max_flow.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;
using std::ofstream;



int main(int argc, char** argv)
{

typedef max_flow<int>::node_data nd_type;
typedef max_flow<int>::edge_data ed_type;

const int ns = sizeof(nd_type)/4;
const int es = sizeof(ed_type)/4;

typedef static_fgraph<opposite_graph,data_slots<ns>,data_slots<es+2> > st_graph;
typedef st_graph::node node;
//typedef st_graph::edge edge;

typedef node_slot1<nd_type,st_graph,0> node_data_array;
typedef edge_slot1<ed_type,st_graph,0> edge_data_array;


  st_graph G;
  node s=0;
  node t=0;

  edge_slot<int,st_graph,es>    cap(G);
  edge_slot<int,st_graph,es+1>  flow(G);


  cout << endl;
  cout << "reading DIMACS maxflow problem from std input ... ";
  cout << flush;

  dimacs_mf D(cin);
  D.translate(G,s,t,cap);


  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << endl;
  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;
  cout << "node slots: " << ns << endl;
  cout << "edge slots: " << es << endl;
  cout << endl;


  max_flow<int, st_graph, node_data_array, edge_data_array> mf;

  if (argc >= 2) 
  { if (string(argv[1]) == "-i") 
       mf.set_run_phase0(false);
    else
       if (string(argv[1]) == "-s") 
          rand_int.set_seed(atoi(argv[2]));
       else
          mf.set_update_f(atof(argv[1]));
   }

  // mf.set_run_phase0(false);

  //edge_slot_c<100,st_graph> cap100;

  mf.run(G,s,t,cap,flow);


  cout << endl;
  cout << string("time: %5.2f  f = %d  ", mf.cpu_time(), mf.flow());
  cout << string("pushes:%8d  ", mf.pushes(-1));
  cout << string("relabels:%8d  ", mf.relabels(-1));
  cout << string("updates:%3d",  mf.updates(-1));
  cout << endl;

  mf.statistics(cout);


  string msg;
  if (!mf.check(G,s,t,cap,flow, msg))
  { cerr << msg << endl;
    ofstream os("err.dimacs");
    write_dimacs_mf(os,G,s,t,cap);
    return 1;
   }

  return 0;
}

