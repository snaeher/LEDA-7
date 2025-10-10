/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mfg.c
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
#include <LEDA/system/assert.h>

#include "incl/max_flow.h"


using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  graph G;
  node s=0;
  node t=0;

  edge_array<int> cap(G,0);
  edge_array<int> flow(G,0);

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


  cout << "MAX_FLOW    " << flush;

  //max_flow<int,graph,succ_array,excess_array,dist_array>  mf;
  max_flow<int,graph>  mf;

  if (argc >= 2)  
     mf.set_heuristic(atof(argv[1]));

  mf.run(G,s,t,cap,flow);

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec  f = %d", mf.cpu_time(), mf.flow()) << endl;

  mf.statistics(cout);

  string msg;
  if (!mf.check(G,s,t,cap,flow,msg))
  { cerr << msg << endl;
    return 1;
   }

  return 0;
}




