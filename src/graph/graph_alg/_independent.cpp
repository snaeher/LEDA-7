/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _independent.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/array.h>

LEDA_BEGIN_NAMESPACE

void INDEPENDENT_SET(const graph& G, list<node>& I)
{ 
  I.clear();

  GRAPH<node,edge> H;
  CopyGraph(H,G);


  // remove self-loops
  edge e;
  forall_edges(e,H)
    if (H.source(e) == H.target(e) ) H.del_edge(e);

  // remove high degree nodes
  node v;
  forall_nodes(v,H) 
     if (H.degree(v) >= 10) H.del_node(v);
  

  array<list<node> > LD(10);
  forall_nodes(v,H) LD[H.degree(v)].append(v);

  node_array<bool> removed(H,false);

  while (H.number_of_nodes() > 0)
  { node v = 0;
    int  i = 0;
    while (i < 10)
    { if (! LD[i].empty())
      { v = LD[i].pop();
        if (!removed[v]) break;
       }
      else i++;
    }

    I.append(H[v]); 
    list<node> affected_nodes;

    forall_inout_edges(e,v) 
    { node w = H.opposite(v,e);
      edge f;
      forall_inout_edges(f,w) 
        affected_nodes.append(H.opposite(w,f)); 
      if (!removed[w]) H.del_node(w); 
      removed[w] = true;
    }

    if (!removed[v]) H.del_node(v); 
    removed[v] = true;

    forall(v,affected_nodes) 
      if (!removed[v]) LD[H.degree(v)].append(v);
  }
}

LEDA_END_NAMESPACE
