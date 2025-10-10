/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main()
{
  GRAPH<rat_point,int> DT;

  list<rat_point> L; 
  lattice_points(25,100,L);

  DELAUNAY_TRIANG(L,DT);

  GraphWin gw(DT);

  node v;
  forall_nodes(v,DT) 
  { rat_point p = DT[v];
    gw.set_position(v,p.to_point());
    gw.set_label_type(v,no_label);
    gw.set_width(v,8);
    gw.set_height(v,8);
  }

  edge e;
  forall_edges(e,DT)
  { switch (DT[e]) {
     case DIAGRAM_EDGE:     gw.set_color(e,green2);
                            break;
     case NON_DIAGRAM_EDGE: gw.set_color(e,yellow);
                            break;
     case HULL_EDGE:        gw.set_color(e,red);
                            break;
    }
  }

  gw.display();
  gw.zoom_graph();
  gw.edit();
  return 0;
}
