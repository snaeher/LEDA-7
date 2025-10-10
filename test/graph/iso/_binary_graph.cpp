/*******************************************************************************
+
+  _binary_graph.cpp
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

#include "_binary_graph.h"

using std::ios;
using std::cerr;
using std::endl;

unsigned short binary_graph::read_word(ifstream& in)
{
	unsigned char b1, b2;
    
	b1 = in.get(); /* Least-significant Byte */
	b2 = in.get(); /* Most-significant Byte */

	return b1 | (b2 << 8);
}

void binary_graph::read_binary_leda_graph(const leda::string& file_name, graph& g, node_map<int>& numbering)
{
	ifstream in(file_name, ios::in | ios::binary);

	if(!in.good())
	{
		cerr << "Could not open file " << file_name << endl;
		return;
	}

	int num_nodes = read_word(in);
	node* mapping = new node[num_nodes];

	for(int i = 0; i < num_nodes; i++)
	{
		mapping[i] = g.new_node();
		numbering[mapping[i]] = i;
	}

	for(int i = 0; i < num_nodes; i++)
	{
		node n = mapping[i];
		mapping[i] = n;
		int num_edges = read_word(in);

		for(int j = 0; j < num_edges; j++)
		{
			int target = read_word(in);
			if(target >= num_nodes)
			{
				cerr << "Error reading file at node " << i << ", edge to " << j << endl;
				break;
			}
			g.new_edge(n, mapping[target]);
		}
	}

	delete[] mapping;
}

void binary_graph::read_binary_leda_string_graph(const leda::string& file_name, GRAPH<leda::string, leda::string>& g, node_map<int>& numbering)
{
	ifstream in(file_name, ios::in | ios::binary);
	if(!in.good())
	{
		cerr << "Could not open file " << file_name << endl;
		return;
	}

	int num_nodes = read_word(in);
	node* mapping = new node[num_nodes];

	for(int i = 0; i < num_nodes; i++)
	{
		mapping[i] = g.new_node("");
		numbering[mapping[i]] = i;
	}

	for(int i = 0; i < num_nodes; i++)
	{
		node n = mapping[i];
		mapping[i] = n;
		int num_edges = read_word(in);

		for(int j = 0; j < num_edges; j++)
		{
			int target = read_word(in);
			if(target >= num_nodes)
			{
				cerr << "Error reading file at node " << i << ", edge to " << j << endl;
				break;
			}
			g.new_edge(n, mapping[target], "");
		}
	}

	delete[] mapping;
}

void binary_graph::write_word(ofstream& out, unsigned short w)
{
	unsigned char b1 = w & 0xFF, b2 = w >> 8;

	out.put(b1);
	out.put(b2);
}

void binary_graph::write_binary_leda_graph(const leda::string& file_name, const graph& g, node_map<int>& numbering)
{
	ofstream out(file_name, ios::out | ios::binary);

	if(!out.good())
	{
		cerr << "Could not write to file " << file_name << endl;
		return;
	}

	write_word(out, g.number_of_nodes());

	node n;
	forall_nodes(n, g)
	{
		write_word(out, g.outdeg(n));
		for(edge e = g.first_out_edge(n); e != nil; e = g.next_out_edge(e))
			write_word(out, numbering[g.target(e)]);
	}
}
