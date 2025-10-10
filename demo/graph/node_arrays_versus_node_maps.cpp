/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  node_arrays_versus_node_maps.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:21 $


#include <stdio.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/array.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph_gen.h>
#include <LEDA/core/IO_interface.h>  

using namespace leda;




int main(){


IO_interface I("Node Arrays versus Node Maps");

I.write_demo("The demo illustrates the relative speed of node arrays and \
node maps.");

I.write_demo("We ask you for an integer n and create a \
graph with n nodes and no edge. We also create a node array and a node map \
for this graph. As an alternative we create a graph with one node data slot.");

I.write_demo("We perform two tests. In the first test \
we iterate over the nodes in their natural \
order and in the second test we iterate over the nodes in random order. \
For each node we access its entry in the node array \
and node map, respectively. \
We perform R repetitions."); 

I.write_demo("We suggest to start with n about 100000"); 

int n = I.read_int("n = ",1000000);
int R = I.read_int("R = ",10);


graph G; graph G1(1,0); node v; int j;
random_graph(G,n,0); random_graph(G1,n,0); 

float T = used_time();
float TA, TB, TM, TAP, TBP, TMP;

{ node_array<int> A(G,0);
  for ( j = 0; j < R; j++ )
    forall_nodes(v,G) A[v]++;
  TA = used_time(T);
}


{ node_array<int> A;
  A.use_node_data(G1,0);
  for ( j = 0; j < R; j++ )
    forall_nodes(v,G1) A[v]++;
  TB = used_time(T);
}

{ node_map<int> A(G,0);
  for ( j = 0; j < R; j++ )  
    forall_nodes(v,G) A[v]++;  
  TM = used_time(T);
}


array<node> perm(n); array<node> perm1(n);
int i = 0;
forall_nodes(v,G) perm[i++] = v;
i = 0;
forall_nodes(v,G1) perm1[i++] = v;
perm.permute(); perm1.permute();

used_time(T);

{ node_array<int> A(G,0);
  for ( j = 0; j < R; j++ )  
    for(i = 0; i < n; i++) A[perm[i]]++;
  TAP = used_time(T);
}

{ node_array<int> A;
  A.use_node_data(G1,0);
  for ( j = 0; j < R; j++ )  
    for(i = 0; i < n; i++) A[perm1[i]]++;
  TBP = used_time(T);
}

{ node_map<int> A(G,0);
  for ( j = 0; j < R; j++ )  
    for(i = 0; i < n; i++) A[perm[i]]++;
  TMP = used_time(T);
}



I.write_table("",TA);
I.write_table(" & ", TB);
I.write_table(" & ", TM);
I.write_table(" & ", TAP);
I.write_table(" & ", TBP);
I.write_table(" & ", TMP);

I.write_demo("Time for node array, natural order = ", TA);
I.write_demo("Time for node data, natural order = ", TB);
I.write_demo("Time for node map, natural order = ", TM);
I.write_demo("Time for node array, random order = ", TAP);
I.write_demo("Time for node data, random order = ", TBP);
I.write_demo("Time for node map, random order = ", TMP);


 

return 0;
}


