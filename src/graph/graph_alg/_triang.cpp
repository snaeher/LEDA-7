/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

list<edge> TRIANGULATE_PLANAR_MAP(graph& G)
{ return G.triangulate_map(); }


LEDA_END_NAMESPACE
