/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  graph_write.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main()
{
  GRAPH<GRAPH<float,float>, list<int> > G;

  test_graph(G);

  //int  i = 0;
  node v; 

  forall_nodes(v,G) 
  { node x = G[v].new_node(17.5); 
    node y = G[v].new_node(13.7); 
    G[v].new_edge(x,y,0.1234);
   }

  //i = 0;
  edge e;
  forall_edges(e,G) 
    for(int j=0; j< 4; j++) G[e].append(j);

  G.print();

  G.write(read_string("write graph to file: "));

  return 0;
}
