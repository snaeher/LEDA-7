/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  embed_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




int main()
{

GRAPH<int,int> G;

int n = read_int("n = ");

random_planar_graph(G,n);

list<edge> inserted_edges;

Make_Biconnected(G,inserted_edges);
Make_Bidirected(G,inserted_edges);

if ( ! PLANAR(G,true) )
 { cout << "Graph is not planar\n";
   return 1;
  }

node_array<int> x(G);
node_array<int> y(G);

float T = used_time();

cout << "OLD EMBEDDING       ";
cout.flush();
STRAIGHT_LINE_EMBEDDING(G,x,y);
cout << string("%6.2f sec  \n",used_time(T));

cout << "NEW EMBEDDING       ";
cout.flush();
STRAIGHT_LINE_EMBEDDING2(G,x,y);
cout << string("%6.2f sec  \n",used_time(T));

/*
node v;
forall_nodes(v,G) cout << string("x = %2d    y = %2d\n",x[v],y[v]);
*/

return 0;

}
