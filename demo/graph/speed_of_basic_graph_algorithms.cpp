/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  speed_of_basic_graph_algorithms.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:21 $


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

IO_interface R("Speed of Basic Graph Algorithms");

R.write_demo("This demo shows the speed of basic graph algorithms.");

R.write_demo(" You are asked to input the number of nodes and edges. \
We will construct \
a random graph of the specified size, run some basic graph algorithms on \
it, and give the running times for each one of them. ");

int n,m;

n = R.read_int("n = ");
m = R.read_int("m = ");

#ifdef BOOK
for (n = 1000; n <= 10000; n = 10*n)
{ m = 10*n;
  R.write_table("\n ",n); // R.write_table(" & ",m);
#endif 

graph G; 

float T = used_time(); float UT;

R.write_demo("Generating a random graph: ");

random_graph(G,n,m);

UT = used_time(T);

R.write_demo("This took ", UT," seconds.");

R.write_table(" & ",UT);

R.write_demo("Deleting Self-Loops: ");
T = used_time();

Delete_Loops(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds."); 
R.write_table(" & ",UT);


R.write_demo("Testing Connectedness: ");
T = used_time();

Is_Connected(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");
R.write_table(" & ",UT);

R.write_demo("Testing Biconnectedness: ");
T = used_time();

Is_Biconnected(G);

float BT = UT = used_time(T);
R.write_demo("This took ", UT," seconds.");
R.write_table(" & ",UT);

R.write_demo("Testing Simplicity: ");
T = used_time();

Is_Simple(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");R.stop();
R.write_table(" & ",UT);

R.write_demo("Testing Bidirectedness: ");
T = used_time();

Is_Bidirected(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");
R.write_table(" & ",UT);

R.write_demo("Testing Acyclicity: ");
T = used_time();

Is_Acyclic(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");
R.write_table(" & ",UT);

R.write_demo("Testing Bipartiteness: ");
T = used_time();

Is_Bipartite(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");
R.write_table(" & ",UT);


R.write_demo("Testing Planarity: ");
T = used_time();

Is_Planar(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");R.stop();
//R.write_table(" & ",UT);


R.write_demo("Testing Triconnectedness. Be patient, the worst case \
running time of this algorithm is \
n times the running time of the biconnectedness algorithm. \
The worst case is assumed if the input graph is triconnected. You \
may have to wait up to ",BT*G.number_of_nodes()," seconds: ");
T = used_time();

Is_Triconnected(G);

UT = used_time(T);
R.write_demo("This took ", UT," seconds.");R.stop();
R.write_table(" & ",UT, "\\\\ \\hline ");
if (n == 10000) R.write_table("\\hline");

#ifdef BOOK
}
#endif
  return 0;
}











