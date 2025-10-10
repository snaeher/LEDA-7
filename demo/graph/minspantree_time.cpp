/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  minspantree_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:22 $


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

int n,m;
IO_interface R("Minimum Spanning Tree Running Times");
#ifdef BOOK
for (n = 25000; n<= 100000; n = 2*n)
{ m = 10*n;
  R.write_table("\n",n); R.write_table(" & ",m);
float sum = 0;
for (int j = 0; j < 10; j++)
{ 
#else
R.write_demo("This demo illustrates the speed of our \
minimum spanning tree algorithm.");
n = R.read_int("number of nodes = ");
m = R.read_int("number of edges = ");
R.write_demo("I am generating a random graph with random \
edge weights in 0 to 100000.");
#endif
GRAPH<int,int> G;
random_graph(G,n,m);
edge e;
forall_edges(e,G) G[e] = rand_int(0,100000);

#ifndef BOOK
R.write_demo("and now compute a minimum spanning tree.");
#endif
float T = used_time();
MIN_SPANNING_TREE(G,G.edge_data());
#ifdef BOOK
sum += used_time(T);
}
R.write_table(" & ",sum/10," \\\\ ");
}
#else
R.write_demo("This took ",used_time(T)," seconds.\n\n");
#endif

return 0;
}


