/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  compact_versus_noncompact_representation.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:24 $


#include <LEDA/core/IO_interface.h>
#include <LEDA/graph/graph_gen.h>

using namespace leda;


int main(){

IO_interface R("compact versus noncompact storage of graph");

R.write_demo("This demo illustrates the influence of caching on the \
running time of graph algorithms.");

R.write_demo("We generate a random graph in two ways.");

R.write_demo("The first method allocates all edges in an adjacency \
list consecutively and the second does not. Thus walking along \
an adjacency list is more costly for the second method (as the \
chances of a cache miss are higher).");

int n,m;

n = R.read_int("n = ");
m = R.read_int("m = ");

#ifdef BOOK
n = 100000;
m = 1000000;
R.write_table("",n); R.write_table(" & ",m);
#endif 

graph G; node v; edge e; 

int count; float T; float UT;

R.write_demo("Generating a random graph: compact representation");

random_graph_compact(G,n,m,false,false,false);

T = used_time();
count = 0;
forall_nodes(v,G)
  forall_adj_edges(e,v) count++;

UT = used_time(T);
R.write_demo("Used time with compact storage representation = ", UT);

R.write_table(" & ",UT);

R.write_demo("Generating a random graph: noncompact representation");

random_graph_noncompact(G,n,m);

T = used_time();
forall_nodes(v,G)
  forall_adj_edges(e,v) count++;

UT = used_time(T);
R.write_demo("Used time with noncompact storage representation = ", UT);
R.write_table(" & ",UT);

return count;

}


