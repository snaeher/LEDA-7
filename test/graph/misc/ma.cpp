/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  ma.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




#include <LEDA/graph/node_map.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




int main()
{
	graph G;
	node_map<int> M(G,0);

	node v = G.new_node();
	M[v] = 1;

	for (int i = 1; i <= 20; i++) {
		node w = G.new_node();
		M[w] = M[v]++;
		v = w;
	}

	forall_nodes(v,G)
		cout << M[v] << " ";

	cout << endl;

	return 0;
}
