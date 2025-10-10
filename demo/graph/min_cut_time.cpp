/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  min_cut_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:21 $


#include <LEDA/graph/min_cut.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

graph G;

IO_interface I("Mincut");

I.write_demo("We illustrate the running time of the min-cut algorithm. \
We ask you for integers n and m, construct a random graph with n nodes \
and m edges and random edge weights in the range [7,20], and then \
determine a minimum cut in the graph. The running time without and with \
a heuristic is given. ");

int n = I.read_int("n = ",1000);
int m = I.read_int("m = ",5000);



int i = 1 ,j = 1;

/*
for (j = 1; j <= 4; j++) I.write_table(" & \\multicolumn{2}{c}{",j*m,"}");
 I.write_table("\\\\ \\hline");

for (j = 1; j <= 4; j++) I.write_table(" & \\multicolumn{1}{c}{NOH} & \\multicolumn{1}{c}{WH}");
I.write_table("\\\\ \\hline \\hline");
*/

//for (i = 1; i <= 5; i++)
{ I.write_table("\n", i*n);
  
//for (j = 1; j <= 4; j++)
{ 
  
  //I.write_demo("n = ",i*n); I.write_demo("m = ",j*m);
  random_simple_undirected_graph(G,i*n,j*m);

  edge_array<int> weight(G);

  edge e;
  forall_edges(e,G) weight[e] = rand_int(7,20);

  float T = used_time(); float TU;

  list<node> Cf,Ct;
  int cf = MIN_CUT(G,weight,Cf,false);

  TU = used_time(T);
  I.write_demo("time without heuristic = ",TU);
  I.write_table(" & ",TU);

  int ct = MIN_CUT(G,weight,Ct,true);

  TU = used_time(T);
  I.write_demo("time with heuristic = ",TU);
  I.write_table(" & ",TU);

  if ( ct != cf )
  { 
    error_handler(1,"error in heuristic");
  }

}

I.write_table("\\\\ \\hline");
//I.stop();

}
I.write_table(" \\hline");
return 0;
}


