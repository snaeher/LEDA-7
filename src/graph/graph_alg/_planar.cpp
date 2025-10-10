/*******************************************************************************
+
+  LEDA 5.2  
+
+
+  _planar.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE


bool PLANAR(graph& G, bool embed)
{ //return BL_PLANAR(G,embed);  
  return HT_PLANAR(G,embed);
}

bool PLANAR(graph& G, list<edge>& el, bool embed)
{ //return BL_PLANAR(G,el,embed); 
  return HT_PLANAR(G,el,embed); 
}


LEDA_END_NAMESPACE
