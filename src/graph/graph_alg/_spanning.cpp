/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _spanning.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Spanning Trees 
//                                                                              
// S. N"aher (1989)
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_partition.h>


LEDA_BEGIN_NAMESPACE

list<edge> SPANNING_TREE(const graph& G)
{ 
  list<edge>     EL;
  node_partition P(G);

  edge e;
  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if (! P.same_block(v,w))
    { EL.append(e);
      P.union_blocks(v,w);
     }
   }

  return EL;
}

void SPANNING_TREE1(graph& G)
{ 
  
  node_partition P(G);

  edge e;
  forall_edges(e,G)
  { 
    node v = G.source(e);
    node w = G.target(e);
    if (! P.same_block(v,w))
    { 
      P.union_blocks(v,w);
    }
    else
	{
	  G.del_edge(e);
	}
  }
}

LEDA_END_NAMESPACE

