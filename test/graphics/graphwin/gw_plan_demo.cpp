/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_plan_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;


void highlight(GraphWin& GW, list<node> V, list<edge> E, node_array<int>& kind)
{
  const graph& G = GW.get_graph();

  bool flush0 = GW.set_flush(false);

  node v;
  forall_nodes(v,G) {
    switch (kind[v]) {

      case  0: GW.set_color(v,grey1);
               GW.set_border_color(v,grey1);
               GW.set_label_color(v,grey2);
               break;
        
      case  2: GW.set_color(v,grey1);
               GW.set_label_type(v,no_label);
               GW.set_width(v,8);
               GW.set_height(v,8);
               break;

      case  3:
      case  4: GW.set_shape(v,rectangle_node);
               GW.set_color(v,red);
               break;

      case -3: GW.set_shape(v,rectangle_node);
               GW.set_color(v,blue2);
               break;
    }
  }


  edge e;
  forall_edges(e,G) GW.set_color(e,grey1);
        
  forall(e,E)
  { GW.set_color(e,black);
    GW.set_width(e,2);
   }

 GW.redraw();

 GW.set_flush(flush0);
}


char* about_str = "\
 \\bf\\blue Planarity Test Demo \\rm\\black\\c4 \
 This demo illustrates planarity testing and straight-line embedding using \
 the LEDA \\bf\\blue GraphWin \\rm\\black data type. You can generate a graph \
 either interactively by simply drawing it into the window or by using one \
 of the graph generators from the menu. The graph is displayed and (after\
 clicking the done button) tested for planarity. If it is planar a \
 straight-line planar drawing is produced, otherwise, a \
 \\bf\\blue Kuratowski \\rm\\black subdivison is shown.";


void about(GraphWin& gw)
{ panel P("Planarity Test Demo");
  P.text_item(about_str);
  P.button("ok");
  gw.open_panel(P);
}



int main() 
{

  graph G;

  GraphWin GW(G,"Planarity Test Demo");

  GW.display();

  int h_menu = GW.get_menu("Help");

  GW.add_simple_call(about,"About Planarity Demo",h_menu);

  about(GW);

  while (GW.edit()) 
  { 
    if (PLANAR(G)) 
      { if (G.number_of_nodes() < 3) continue;
        node_array<double> xcoord(G);
        node_array<double> ycoord(G);
        STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);
        GW.adjust_coords_to_win(xcoord,ycoord);
        GW.set_layout(xcoord,ycoord);
      }
    else 
     { 
       list<node> V;
       list<edge> E;
       node_array<int> kind(G);
       KURATOWSKI(G,V,E,kind);
  
       GW.save_all_attributes();

       highlight(GW,V,E,kind);

       GW.wait("\\bf This Graph is not planar. I show you a\
                \\blue Kuratowski Subdivision \\rm\\black\\n\
                (click done to continue)");

       GW.restore_all_attributes();
      }

  }

  return 0;
}
