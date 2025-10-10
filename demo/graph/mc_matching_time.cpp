/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  mc_matching_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:21 $


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/mc_matching.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

IO_interface I("Maximum Cardinality Matching in General Graphs");

I.write_demo("This demo illustrates the speed of the cardinality matching \
algorithm for general graph. We ask you to input n and m, \
construct a random graph with n nodes and m edges, \
and construct a maximum matching.");

int n = I.read_int("n = ",10000);
int m = I.read_int("m = ",10000);

graph G;

float T = used_time();
random_graph(G,n,m);

I.write_demo("used time for constructing the graph = ",used_time(T));

node_array<int> OSC(G);
list<edge> MK = MAX_CARD_MATCHING(G,OSC);

I.write_demo("used time for constructing the matching = ",used_time(T));
  
CHECK_MAX_CARD_MATCHING(G,MK,OSC);
I.write_demo("used time for checking the matching = ",used_time(T));
  
MAX_CARD_MATCHING(G,OSC,1);

I.write_demo("used time for constructing the matching \
(with heuristic to construct an initial matching) = ",used_time(T));

return 0;
} 


