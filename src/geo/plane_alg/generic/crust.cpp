/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  crust.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE


void  CRUST(const list<POINT>& L0, GRAPH<POINT,int>& G) 
{
  list <POINT> L = L0;

  map<POINT,bool> voronoi_vertex(false);

  GRAPH<CIRCLE,POINT> VD;

  VORONOI(L,VD);

// add Voronoi vertices and mark them

  node v;
  forall_nodes(v,VD)
  { if (VD.outdeg(v) < 2) continue; 
    POINT p = VD[v].center();
    voronoi_vertex[p] = true;
    L.append(p);
   }

  DELAUNAY_TRIANG(L,G);

  list<node> vlist;
  forall_nodes(v,G)
     if (voronoi_vertex[G[v]]) vlist.append(v);

  G.del_nodes(vlist);

  edge e;
  forall_edges(e,G) G[e] = DIAGRAM_EDGE;
}

LEDA_END_NAMESPACE
