/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  graph_io.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// test output in LEDA format and GML format

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_gen.h>
#include <LEDA/numbers/integer.h>
#include <LEDA/system/file.h>
#include <LEDA/core/array.h>

using namespace leda;

using std::cout;
using std::endl;


#undef leda_graph

typedef GRAPH<integer, double> Graph;
typedef Graph::node_value_type node_t;
typedef Graph::edge_value_type edge_t;

bool compare_graphs(Graph& G1, Graph& G2)
{
	if (G1.number_of_nodes() != G2.number_of_nodes()) return false;
	if (G1.number_of_edges() != G2.number_of_edges()) return false;

      //int n = G1.number_of_nodes(); 
        int m = G1.number_of_edges();

	edge e;

	array<edge> edge_in_G1(m);
	forall_edges(e, G1) {
		int i = int(G1[e]);
		edge_in_G1[i] = e;
	}

	array<edge> edge_in_G2(m);
	forall_edges(e, G2) {
		int i = int(G2[e]);
		edge_in_G2[i] = e;
	}

	for (int i = 0; i < m; ++i) {
		edge e1 = edge_in_G1[i], e2 = edge_in_G2[i];
		if (G1[source(e1)] != G2[source(e2)]) return false;
		if (G1[target(e1)] != G2[target(e2)]) return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	Graph G;
	random_graph(G, 10, 50);

	node n;
	forall_nodes(n, G) G[n] = node_t(index(n));
	edge e;
	forall_edges(e, G) G[e] = edge_t(index(e));

	string leda_file = "graph_io.led", gml_file = "graph_io.gml";

	G.write(leda_file);
	G.write_gml(gml_file);

	Graph leda_graph;
	leda_graph.read(leda_file);

	Graph gml_graph;
	gml_graph.read_gml(gml_file);

	delete_file(leda_file); delete_file(gml_file);

	if (! compare_graphs(G, leda_graph)) {
		cout << "i/o error with leda format" << endl;
		cout << "original\n" << G << endl;
		cout << "leda\n" << leda_graph << endl;
		return 1;
	}
	if (! compare_graphs(G, gml_graph)) {
		cout << "i/o error with gml format" << endl;
		cout << "original\n" << G << endl;
		cout << "gml\n" << gml_graph << endl;
		return 2;
	}

	return 0;
}
