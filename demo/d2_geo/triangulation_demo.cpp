/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  triangulation_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/geo_alg.h>
#include <LEDA/graphics/window.h>

#include <LEDA/geo/rat_kernel_names.h>

using namespace leda;



window W("TRIANGULATION BY SWEEP");
window WDD("DELAUNAY TRIANGULATION");
int kind;
list<POINT> L;
GRAPH<POINT,int> G,DD;

int main()
{
 

   W.display(); WDD.display(window::min,window::max);
   W.display_help_text("triangulation_demo");

   W.set_line_width(2);  WDD.set_line_width(2);
   W.set_node_width(4);  WDD.set_node_width(4);

   POINT p; point q;  

   while( W >> q)
   { L.append(POINT(q));
       
     TRIANGULATE_POINTS(L,G);
     DELAUNAY_TRIANG(L,DD);

     node v; edge e; 
     W.clear(); WDD.clear();

     forall_nodes(v,G) W.draw_filled_node(G[v].to_point());
  
     forall_edges(e,G) 
     { node v = G.source(e), w = G.target(e);
       W.draw_segment(G[v].to_point(),G[w].to_point());
     }

     

     forall_nodes(v,DD) WDD.draw_filled_node(DD[v].to_point());
  
     forall_edges(e,DD) 
     { node v = DD.source(e), w = DD.target(e);
       WDD.draw_segment(DD[v].to_point(),DD[w].to_point());
     }

  }
  return 0;
}


