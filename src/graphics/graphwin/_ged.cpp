/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _ged.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graph_edit.h>
#include <LEDA/graphics/graphwin.h>

LEDA_BEGIN_NAMESPACE

void graph_edit(window& W, GRAPH<point,int>& G, bool dir, bool)
{ GraphWin gw(G,W);
  gw.set_directed(dir);
  gw.set_position(G.node_data());
  gw.open();
  node v;
  forall_nodes(v,G) G[v] = gw.get_position(v);
 }

LEDA_END_NAMESPACE
