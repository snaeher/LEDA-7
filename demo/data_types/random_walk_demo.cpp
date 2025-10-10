/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  random_walk_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:19 $

#include <LEDA/graph.h>
#include <LEDA/string.h>
#include <LEDA/markov_chain.h>
#include <LEDA/IO_interface.h>

using namespace leda;


int main(){
graph G;
node v0 = G.new_node();
node v1 = G.new_node();
edge e00 = G.new_edge(v0,v0); edge e01 = G.new_edge(v0,v1);
edge e10 = G.new_edge(v1,v0); edge e11 = G.new_edge(v1,v1);

edge_array<int> weight(G);
weight[e00] = 2; weight[e01] = 1;
weight[e10] = 1; weight[e11] = 1;

#ifdef BOOK
string sh = "$n$ "; string s0 = "$v_0$ "; string s1 = "$v_1$ ";

for (int n = 1; n <= 10000000; n = 10 * n)
{ markov_chain M(G,weight);
  M.step(n);
  sh = sh + " & " + string("%d",n);
  s0 = s0 + " & " + string("%d",M.number_of_visits(v0));
  s1 = s1 + " & " + string("%d",M.number_of_visits(v1));
}

cout << sh << " \\\\ \\hline \\hline \n" 
     << s0 << " \\\\ \\hline \n " << s1 << "\\\\ \\hline \\hline \n";
#else
IO_interface I("A Random Walk in a Graph");
I.write_demo("We perform a random walk on the graph shown \
in the section of random number generation of the LEDA book. \
You are asked to input the number of steps you want to perform \
and the program reports the number of visits to each of the nodes.");

while( true )
{ int N = I.read_int("number of steps = ");

  dynamic_markov_chain M(G,weight);
  M.step(N);

  I.write_demo("# of visits of v0 = ",M.number_of_visits(v0));
  I.write_demo("# of visits of v1 = ",M.number_of_visits(v1));
  print_statistics();
}
#endif
return 0;
}

