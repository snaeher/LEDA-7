/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mcfs.c
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

#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>

#define MCF_STATIC_GRAPH
#include <LEDA/graph/templates/mcf_cap_scaling.h>


using namespace leda;

using std::cin;
using std::cout;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  //bool do_check = false;
  bool dimacs_statistics = false;


  typedef static_fgraph<opposite_graph,data_slots<5>,data_slots<3> > st_graph;
  //typedef st_graph::node node;
  typedef st_graph::edge edge;
  typedef st_graph::incident_edge incident_edge;


  st_graph G;

  node_array<int,st_graph>  supply(G);
  edge_array<int,st_graph>  lcap(G);

  edge_slot<int,st_graph,0>  ucap(G);
  edge_slot<int,st_graph,1>  cost(G);
  edge_slot<int,st_graph,2>  flow(G);

/*
  typedef edge_slot<int,st_graph,2> cap_array;
*/

  typedef node_slot<int,st_graph,0>  pot_array;
  typedef node_slot<int,st_graph,1>  excess_array;
  typedef node_slot<int,st_graph,2>  dist_array;
  typedef node_slot<incident_edge,st_graph,3> pred_array;
//typedef node_slot<edge,st_graph,3> pred_array;
  typedef node_slot<int,st_graph,4>  mark_array;

  //excess_array excess;


  cout << endl;
  cout << "reading DIMACS min-cost-flow problem from std input ... ";
  cout << flush;

  dimacs_mcf D(cin);
  D.translate(G,supply,lcap,ucap,cost);

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;

  mcf_cap_scaling<int,st_graph, pot_array,excess_array, dist_array,
                                    pred_array,mark_array> mcf;


  int i = 1;
  while (i < argc)
  { string s = argv[i];
    if (s[0] != '-') break;
    if (s == "-l") mcf.set_write_log(true);
    //if (s == "-c") do_check = true;
    if (s == "-s") dimacs_statistics = true;
    i++;
  }

  if (dimacs_statistics)
  { D.print_statistics(cout);
    cout << endl;
   }



//for(int i = 1; i<=1024; i*=2)
{
  int i = 1;

  bool f = mcf.run(G,lcap,ucap,cost,supply,flow,i) ;

  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];

  if (f)
   cout << string("time: %6.2f sec c = %f  i = %3d",mcf.cpu_time(),c,i) << endl;
  else
   cout << string("time: %6.2f sec. Not feasible.",mcf.cpu_time()) << endl;

  string msg;
  if (!mcf.check(G,lcap,ucap,cost,supply,flow,msg))
    error_handler(1,msg);
}

}

