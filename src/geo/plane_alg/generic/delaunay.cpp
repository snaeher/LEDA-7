/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



LEDA_BEGIN_NAMESPACE

void DELAUNAY_TRIANG(const list<POINT>& L, GRAPH<POINT,int>& G)
{ DELAUNAY_DWYER(L,G); }

void F_DELAUNAY_TRIANG(const list<POINT>& L, GRAPH<POINT,int>& G)
{ F_DELAUNAY_FLIP(L,G); }


void DELAUNAY_DIAGRAM(const list<POINT>& L, GRAPH<POINT,int>& DD)
{ 
  DELAUNAY_TRIANG(L,DD);
  edge e;
  forall_edges(e,DD)
      if (DD[e] == NON_DIAGRAM_DART) DD.del_edge(e);
}


void F_DELAUNAY_DIAGRAM(const list<POINT>& L, GRAPH<POINT,int>& DD)
{ 
  F_DELAUNAY_TRIANG(L,DD);
  edge e;
  forall_edges(e,DD)
      if (DD[e] == NON_DIAGRAM_DART) DD.del_edge(e);
}

LEDA_END_NAMESPACE
