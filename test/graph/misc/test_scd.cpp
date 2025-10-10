/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  test_scd.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/node_array.h>
#include <LEDA/graph/edge_array.h>

#include <LEDA/graph/graph.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main(int argv,char** argc)
{     	    		 	  
// typedef scd_graph<3> graph_t;  
// typedef scd_graph<3,scd_in_edge,scd_in_node> graph_t;

  typedef scd_graph<3, scd_data_edge<1>, scd_data_node<1> > Graph;
  
  typedef Graph::node node;
  typedef Graph::edge edge;
 
  Graph G;
   
  G.init(4);
   
  node v = G.new_node();
  node w = G.new_node();
  node u = G.new_node();   
  node z = G.new_node();

  G.new_edge(v,w);
  G.new_edge(v,u);
  G.new_edge(v,z);
  
  G.new_edge(w,v);
  G.new_edge(w,u);
  G.new_edge(w,z);
  
  G.new_edge(u,v);
  G.new_edge(u,w);
  G.new_edge(u,z);
  
  G.new_edge(z,v);
  G.new_edge(z,w);
  G.new_edge(z,u);

  G.finish_construction();
 
  cout << "sizeof(edge_t) " << sizeof(Graph::edge_t) << endl;
  cout << "sizeof(node_t) " << sizeof(Graph::node_t) << endl;
  cout << endl;

  G.print();
 
  cout << "in edges" << endl;
  forall_nodes(v,G)
  {
    cout << string("[%d] : ", G.index(v));
  
    edge e;
    forall_in_arcs(e,v,G)
      cout << string("[%d]-->[%d]", G.index(G.source(e)), G.index(v));

    cout << endl;
  }
 

  edge_array<int,Graph> A;
  
  A.use_edge_data(G,0);

  edge e;
  forall_edges(e,G)
  {
    A[e] = G.index(e);
    cout << string("A[e] : %d\n", A[e]);
  }


  node_array<int,Graph> B;
  
  B.use_node_data(G,0);

  forall_nodes(v,G)
  {
    B[v] = G.index(v);
    cout << string("B[v] : %d\n", B[v]);
  }

  return 0;
}

