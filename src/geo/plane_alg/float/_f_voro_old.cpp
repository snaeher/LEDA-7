/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _f_voro_old.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define NO_RAT_ALGORITHMS
#include <LEDA/geo/plane_alg.h>

LEDA_BEGIN_NAMESPACE

// old style voronoi diagrams (for backward compatibility)

void VORONOI(const list<point>& L, double R, GRAPH<point,point>& VD)
{
  GRAPH<circle,point> VD0;

  VORONOI(L,VD0);

  node_array<node> corr(VD0);

  node v;
  forall_nodes(v,VD0)
  { circle C = VD0[v];
    point c;
    if (C.orientation() != 0)
      c = C.center();
    else
    { point a = C.point1();
      point b = C.point3();
      vector v = (b-a).rotate90();
      c = center(a,b) + R*v.norm();
     }
    corr[v] = VD.new_node(c);
   }

  forall_nodes(v,VD0)
  { edge e;
    forall_adj_edges(e,v)
    { node x = corr[v];
      node y = corr[target(e)];
      VD.new_edge(x,y,VD0[e]);
     }
   }
}

LEDA_END_NAMESPACE
