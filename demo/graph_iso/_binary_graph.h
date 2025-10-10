/*******************************************************************************
+
+  _binary_graph.h
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  Helper class to read in a directed unlabeled graph from the 
+  "Graph Database CD" by SIVALab (http://amalfi.dis.unina.it).
+  The file format is described at 
+  http://amalfi.dis.unina.it/graph/db/doc/graphdbat-2.html#ss2.1
+
*******************************************************************************/

#include <LEDA/core/string.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <iostream>


using namespace leda;

using std::ifstream;
using std::ofstream;

class binary_graph
{
public:
	//Fill the given graph with the nodes and edges read from the file
	static void read_binary_leda_graph(const string& file_name, 
                                           graph& g, node_map<int>& numbering);

	//Fill the given graph (parametrized with string for both nodes 
        //and edges) with the nodes and edges read from the file
	static void read_binary_leda_string_graph(const string& file_name,
                                                  GRAPH<string, string>& g, 
                                                  node_map<int>& numbering);

	//Save the given graph to a file
	static void write_binary_leda_graph(const string& file_name, 
                                            const graph& g, 
                                            node_map<int>& numbering);

private:
	//read a word (2 bytes, little endian)
	static unsigned short read_word(ifstream& in);
	//write a word (2 bytes, little endian)
	static void write_word(ofstream& in, unsigned short w);
};

