/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  random_graph_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:21 $


#include <LEDA/graph/graph_gen.h>
#include <assert.h>
#include <LEDA/graph/node_map2.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;



  
int main(){

IO_interface I("Random Graph Generation");

I.write_demo("This demo illustrates the speed of random graph generation.");

I.write_demo("We ask for integers n and m and then generate four \
random graph with n nodes and m edges. In the first graph we allow \
parallel edges, anti-parallel edges, and self-loops, in the second \
graph we forbid parallel edges, in the third graph we forbid parallel \
edges and self-loops and in the fourth graph we forbid parallel and \
anti-parallel edges and self-loops.");

int n = I.read_int("n = ",100000);
int m = I.read_int("m = ",1000000);

graph G;

float T = used_time(); float TU;

//random_graph(G,n,m,false,false,false);
random_graph(G,n,m);

TU = used_time(T);
I.write_table("",TU);
I.write_demo("random graph, time = ",TU);

random_simple_graph(G,n,m);

TU = used_time(T);
I.write_table(" & ",TU);
I.write_demo("random simple graph, time = ",TU);

assert(Is_Simple(G));

TU = used_time(T);
I.write_demo("time to check simplicity, time = ",TU);

random_simple_loopfree_graph(G,n,m);

TU = used_time(T);
I.write_table(" & ",TU);
I.write_demo("random simple loopfree graph, time = ",TU);

assert(Is_Simple_Loopfree(G));

TU = used_time(T);
I.write_demo("time to check simplicity and looplessness, time = ",TU);

random_simple_undirected_graph(G,n,m);

TU = used_time(T);
I.write_table(" & ",TU, " \\\\ ");
I.write_demo("random simple undirected graph, time = ",TU);

assert(Is_Simple_Undirected(G));

TU = used_time(T);
I.write_demo("time to check, time = ",TU);

return 0;
}

