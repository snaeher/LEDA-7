/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  git_example.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


/********************************************************************
*                                                                   *
*        Graphiterator Example Program  git_example.c               *
*                                                                   *
*              Marco Nissen (marco@mpi-sb.mpg.de)                   *
*                                                                   *
*        Last update 2.7.1998 (Marco Nissen)                        *
*                                                                   *
********************************************************************/
#if defined(__HP_aCC) || defined(mips)
#include<LEDA/internal/std/iostream.h>

int main()
{
    cout << "example does not work with this compiler\n";
    return 0;
}

#else

#define GITSETWK
 
#include <LEDA/graph/graph.h>
#include <LEDA/core/set.h>
#include <LEDA/graph/graph_iterator.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


int main()
{
	// graph preparation
	graph G;
	node v1=G.new_node();
	node v2=G.new_node();
	node v3=G.new_node();
	/*edge e1=*/ G.new_edge(v1,v2);
	/*edge e2=*/ G.new_edge(v2,v3);
	
	// now we traverse the graph
	
	// at first the nodes
	
	NodeIt it(G);
	while (it.valid()) {
		cout << "Now node " << index(it++.get_node()) << endl;
	}
	
	for (it.reset(); it.valid(); it++) {
		cout << "Now node " << index(it.get_node()) << endl;
	}
	
	it.reset();
	while (it.valid()) {
		AdjIt ai(G,it);
		cout << "Entering node " << index(ai.get_node()) << endl;
		while (ai.valid()) {
			cout << "Now adjacent node " << index(ai++.curr_adj().get_node()) << endl;
		}
		it++;
	}
	return 0;
}

#endif
