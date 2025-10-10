/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  map_io.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_GMAP_IO

#include <LEDA/graph/graph.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main()
{
  graph G;
  random_graph(G,10,10);
  node_array<int>    A(G);
  edge_array<string> B(G);
 
  int i = 0;
  node v;
  forall_nodes(v,G) A[v] = i++;
 
  int j = 0;
  edge e;
  forall_edges(e,G) B[e] = string("label_%d",j++);


  // write graph and arrays to file
 
  ofstream out("ggg");
  G.add_io_map(A);
  G.add_io_map(B);
  G.write(out);
  out.close();


  // read graph and arrays from file
 
  graph G1;
  node_array<int>    A1;
  edge_array<string> B1;
 
  ifstream in("ggg");
  G1.add_io_map(A1);
  G1.add_io_map(B1);
  G1.read(in);
 
  forall_nodes(v,G1) cout << A1[v] << " ";
  cout << endl;
  cout << endl;
 
  forall_edges(e,G1) cout << B1[e] << " ";
  cout << endl;
  cout << endl;

  // read graph without arrays
  ifstream in1("ggg");
  G.clear_io_maps();
  G.read(in1);
  G.print();
 
  return 0;
}

