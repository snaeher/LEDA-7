/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  closure_test.c
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

/*

// Example from Mehlhorn volume II, page 11

node n1,n2,n3,n4,n5,n6,n7;

n1 = G.new_node(1);
n2 = G.new_node(2);
n3 = G.new_node(3);
n4 = G.new_node(4);
n5 = G.new_node(5);
n6 = G.new_node(6);
n7 = G.new_node(7);

G.new_edge(n1,n3);
G.new_edge(n2,n4);
G.new_edge(n3,n5);
G.new_edge(n4,n5);
G.new_edge(n5,n6);
G.new_edge(n5,n7);

G.print();

*/

test_graph(G);

G.write("trans.ggg");

cout << "TRANSITIVE_CLOSURE   ";
cout.flush();
float T = used_time();
graph G1 = TRANSITIVE_CLOSURE(G);
cout << string("    %6.2f sec\n",used_time(T));
cout << endl;
if (Yes("ausgabe?")) G1.print();
G.clear();
G1.clear();

return 0;

}
