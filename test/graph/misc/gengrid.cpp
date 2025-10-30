/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gengrid.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/ugraph.h>
#include <LEDA/system/stream.h>
#include <LEDA/graph/dimacs.h>

using namespace leda;


using std::cout;
using std::flush;
using std::endl;
using std::ofstream;



int main(){

GRAPH<int,int> G;

int N = read_int("N = ");

grid_graph(G,N);

edge e;
forall_edges(e,G) G[e] = rand_int(0,100);

node v;
forall_nodes(v,G) G[v] = index(v);

G.write(string("grid%d.graph",N));


ofstream dimacs(string("grid%d.dimacs",N));
Write_Dimacs_SP(dimacs,G,G.first_node(),G.edge_data());


// write Lauther's format:

file_ostream lauther(string("grid%d",N));

lauther << index(G.first_node());
lauther << " ";
lauther << index(G.last_node());
lauther << endl;

forall_nodes(v,G)
{ forall_adj_edges(e,v)
  { lauther << index(v) << " ";
    lauther << index(target(e)) << " "; 
    lauther << G[e];
    lauther << endl;
   }
 }

 return 0;
}
