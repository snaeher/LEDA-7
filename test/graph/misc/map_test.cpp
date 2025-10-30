/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  map_test.c
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



void clear_array(graph& G, node_array<int>& A)
{ node v;
  forall_nodes(v,G) A[v] = 0;
}

void clear_array(graph& G, edge_array<int>& B)
{ edge e;
  forall_edges(e,G) B[e] = 0;
}


int main()
{

GRAPH<int,int> G;


node_map<int> A(G);
edge_map<int> B(G);

test_graph(G);

STRONG_COMPONENTS(G,A);

node v;
forall_nodes(v,G) cout << A[v] << endl;

node a = G.first_node();
node b = G.last_node();
G.new_edge(a,b);

clear_array(G,B);

return 0;
}
