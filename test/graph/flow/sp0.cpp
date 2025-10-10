/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  sp0.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/templates/dijkstra.h>
#include <LEDA/graph/dimacs.h>

using namespace leda;

using std::ifstream;
using std::cout;
using std::cin;
using std::flush;
using std::endl;


int main(int argc, char** argv)
{
  GRAPH<int,int> G(1,1);

  edge_array<int>& cost = G.edge_data();

  node_array<int>& dist = G.node_data();

  node_array<edge> pred;
  pred.use_node_data(G,0);

  node s;

  cout << endl;

  if (argc < 2) 
    { cout << "reading DIMACS shortest paths problem from std input ... ";
      cout << flush;
      Read_Dimacs_SP(cin,G,s,cost);
      cout << endl;
     }
  else
    { cout << "reading DIMACS shortest paths problem from " << argv[1] << " ... ";
      cout <<flush;
      ifstream in(argv[1]);
      Read_Dimacs_SP(in,G,s,cost);
     }

  cout << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cout << string("%6d nodes",n) << endl;
  cout << string("%6d edges",m) << endl;
  cout << endl;

  cout << "DIJKSTRA   " << flush;
  float T  = used_time();

  DIJKSTRA_T(G,s,cost,dist,pred);

  cout << endl;
  cout << "     ";
  cout << string("time: %6.2f sec",used_time(T)) << endl;
  cout << endl;

  return 0;
}
