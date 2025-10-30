/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _independent_set.c
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
{ I.clear();

  GRAPH<node,edge> H;
  CopyGraph(H,G);

  node v; edge e;
  list<edge> E = H.all_edges();
  forall(e,E)
  { if (H.source(e) == H.target(e) ) H.del_edge(e); }
 
  H.make_map(E); // E is a dummy argument

  list<node> HD; // high degree nodes
  forall_nodes(v,H) if (H.degree(v) >= 10) HD.append(v);
  
  forall(v,HD) H.del_node(v);

  array<list<node> > LD(10);
  forall_nodes(v,H) LD[H.degree(v)].append(v);
  node_array<bool> node_of_H(H,true);

  while (H.number_of_nodes() > 0)
  { int i = 0;
    while (i < 10)
    { if ( LD[i].empty() ) { i++; continue; }
      v = LD[i].pop();
      if ( node_of_H[v] ) break;
    }

    I.append(H[v]); 
    list<node> affected_nodes;

    forall_inout_edges(e,v) 
    { node w = H.opposite(v,e);
      edge f;
      forall_inout_edges(f,w) 
        affected_nodes.append(H.opposite(w,f)); 
      H.del_node(w); node_of_H[w] = false;
    }
    H.del_node(v); node_of_H[v] = false;

    forall(v,affected_nodes) 
      if ( node_of_H[v] ) LD[H.degree(v)].append(v);
  }
}

LEDA_END_NAMESPACE
