/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mf0.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/graph/node_list.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/system/stream.h>

using namespace leda;

using std::ifstream;
using std::cout;
using std::cin;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  GRAPH<int,int> G(2,1);

  edge_array<int>& cap = G.edge_data();

  edge_array<int> flow;
  flow.use_edge_data(G,0);

  node s,t;

  cout << endl;

  if (argc < 2) 
    { cout << "reading DIMACS maxflow problem from std input ... ";
      cout << flush;
      Read_Dimacs_MF(cin,G,s,t,cap);
      cout << endl;
     }
  else
    { cout << "reading DIMACS maxflow problem from " << argv[1] << " ... ";
      cout <<flush;
      ifstream in(argv[1]);
      Read_Dimacs_MF(in,G,s,t,cap);
     }

  cout << endl;

//int pushes,relabels,updates,gap_nodes,inspects;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;

  cout << "MAX_FLOW   " << flush;
  float T  = used_time();

//int f = MAX_FLOW(G,s,t,cap,flow,pushes,inspects,relabels,updates,gap_nodes) ;
  int f = MAX_FLOW(G,s,t,cap,flow) ;

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec  f = %d",used_time(T),f) << endl;
  cout << endl;
/*
  cout << "     ";
  cout << string("pushes:%d  relabels:%d  updates:%d  insp:%d  gap nodes:%d",
                  pushes, relabels, updates, inspects, gap_nodes) << endl;
  cout << endl;
*/

  CHECK_MAX_FLOW(G,s,t,cap,flow);

  return 0;
}
