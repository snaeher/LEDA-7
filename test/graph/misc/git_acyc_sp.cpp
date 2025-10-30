/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  git_acyc_sp.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


/********************************************************************
*                                                                   *
*        Graphiterator Example Program  git_acyc_sp.c               *
*                                                                   *
*              Marco Nissen (marco@mpi-sb.mpg.de)                   *
*                                                                   *
*        Last update 2.7.1998 (Marco Nissen)                        *
*                                                                   *
********************************************************************/
 

#include <LEDA/graph/graph_iterator.h>
#include <LEDA/system/stream.h>
#include <math.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/panel.h>
#include <time.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




#define GRAPHADD(x,y)                   \
if (!M.defined(index(x)*n*10+index(y))) \
{ G.new_edge(x,y);                      \
		mm++;                   \
M[index(x)*n*10+index(y)]=true; }

void
random_acyclic_graph (graph & G, int n, int m)
{
	node V[10000];
	G.clear ();
	for (int i = 0; i < n; i++)
		V[i] = G.new_node ();
	h_array < int, bool > M;
	int mm = 0;
	node n1 = nil, n2 = nil;
	int counter = 0;
	while (mm < m) {
		n1 = n2;
		counter = 0;
		while (n1 == n2 && counter < 10000) {
			int a = rand_int (0, n - 1);
			int b = rand_int (0, n - 1);
			n1 = n2;
			if (a < b) {
				n1 = V[a];
				n2 = V[b];
				counter++;
			}
		}
		if (counter < 10000)
			GRAPHADD (n1, n2);
	}
}


int
main ()
{
	// graph preparation
	graph G;
	edge e;
	node v;
	panel P ("Acyclic Shortest Paths Demo");
	P.text_item ("This program demonstrates how to use modified algorithms for graph with special properties.");
	P.text_item ("");
	P.text_item ("For example, if you know that a graph is acyclic, you could sort the graph in topological order and then update all distances to satisfy the equation 'd(j)<=d(i)+c(i,j)' where (i,j) is an edge in the graph and 'd' distance array and 'c' cost of edges.");
	P.text_item ("");
	P.text_item ("That is exactly what the algorithm does, but it does not explicitly compute a topological ordering or reorders the graph structure. Instead of that it uses topological iterators and the examines the distance relation.");
	P.open ();
	G.clear ();
	GraphWin GW (G);
#ifndef _MSC_VER
	GW.set_directed (true);
#endif
	
	while (1) {
		panel P2 ("Demonstration of Graph Iterators");
		P2.text_item ("The next window shows a graph editor window (GraphWin)");
		P2.text_item ("and you can enter sample graph.");
		P2.text_item ("For this demonstration you should enter an acyclic graph.");
		P2.text_item ("If you don't know what an acyclic graph is or if you don't");
		P2.text_item ("want to enter the graph by yourself, select 'generate'.");
		P2.text_item ("");
		int sc = P2.button ("continue");
		int sg = P2.button ("generate");
		int action2 = P2.open ();
		if (action2 == sg) {
			G.clear ();
			panel P2 ("Size of acyclic graph");
			P2.text_item ("Please enter the size of the generated acyclic graph.");
			int n = 10, m = 20;
			P2.int_item ("Number of nodes", n);
			P2.int_item ("Number of edges", m);
			P2.open ();
			random_acyclic_graph (G, n, m);
		}
		GW.update_graph();
		GW.open ();
		
		
#ifndef _MSC_VER
		node_array < int >ord (G);
              
		bool acyc = TOPSORT (G, ord);
		if (acyc == true)
			break;
		else {
			panel P ("Graph not Acyclic");
			P.text_item ("Unfortunately the graph that you built was not acyclic. Please repeat the graph creation process.");
		}
#else
break;
#endif

	}
	
	edge_array < int >cost (G);
	node_array < int >dist (G), dist2(G);
	node_array < edge > pred(G), pred2 (G);
	node_array_da < int >ddist (dist);
	node_array_da < edge > dpred (pred);
	edge_array_da < int >dcost (cost);
	
		forall_nodes (v, G) {
			pred[v] = nil;
			dist[v] = 0;
			dist2[v] = 0;
			GW.set_color (v, green);
		}
		forall_edges (e, G) {
			double a = GW.get_position (source (e)).xcoord ();
			double b = GW.get_position (source (e)).ycoord ();
			double c = GW.get_position (target (e)).xcoord ();
			double d = GW.get_position (target (e)).ycoord ();
			double f = sqrt ((a - c) * (a - c) + (b - d) * (b - d));
			cost[e] = (int) (10 * f);
			GW.set_color (e, black);
		}
		GW.redraw ();
		
	panel P3("Start of Algorithm");
	P3.text_item("Now I will start the computation.");
	P3.text_item("");
	P3.text_item("All results will be printed on standard output. Please verify that the results from both algorithms are equal for equal nodes. ");
	P3.button("end");
	P3.open();
	
	// now we traverse the graph
	// we simply preselect the first node of the graph for beginning of traversal ..
	
	NodeIt nit (G);
	while (nit.valid ()) {
		set (ddist, nit, ddist.value_max);
		++nit;
	}
	nit.update (G.first_node ());
	set (ddist, nit, ddist.value_null);
	
	TOPO_It it (G);
	
	InAdjIt in (G);
	while (it.valid ()) {
		in.update (it.get_node ());
		while (in.valid ()) {
			if (get(ddist,in.curr_adj())!=ddist.value_max &&
					get (ddist, in.curr_adj ()) + get (dcost, in) < get (ddist, in)) {
					set (ddist, in, get (ddist, in.curr_adj ()) + get (dcost, in));
				set (dpred, in, in.get_edge ());
			}
			++in;
		}
		cout << "Alternative Algorithm : Distance for node " << index (it.get_node ()) << " is " << get (ddist, it) << endl;
		++it;
	}
	
	DIJKSTRA_It < int >dit (G, G.first_node (), dist2, cost);
	
	while (dit.valid ()) {
		++dit;
	}
	
	it.reset();
	while (it.valid()) {
		cout << "'Traditional Dijkstra': Distance for node " << index (it.get_node()) << " is " << dist2[it.get_node()] << endl;
	++it; }
 return 0;
}
