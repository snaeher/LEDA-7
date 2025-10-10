/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_visrep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/graphwin.h>
#include<LEDA/graph/graph_alg.h>

using namespace leda;


void about(GraphWin& gw)
{ 
  window& W = gw.get_window();

  panel P(3*W.width()/4,-1);

  P.text_item("\
  \\bf Visbility Representations of Planar Graphs\\tf\\c12\
  This program demonstrates that any \\bf\\blue planar graph\\tf\\black can\
  be drawn as a \\bf\\blue visibility diagram\\black\\tf such that every node\
  is represented by a horizontal bar (rectangle) and all edges are drawn\
  by vertical line segments connecting nodes that can see each other.\\15\
  Edit a graph or create a (random) planar graph and click done. If the\
  the graph is planar a visbility reprentation will be computed and\
  shown. In the other case you can ask for a proof of non-planarity.");

  P.button("ok");

  gw.open_panel(P);
}


int main()
{
  GraphWin gw;
  gw.set_node_shape(rectangle_node);
  gw.set_edge_direction(undirected_edge);

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about,"About Tutte Demo",h_menu);

  about(gw);

  gw.message("Please construct or create a (planar) graph\
             and click \\blue done\\black.");

  while(gw.edit())
  {
    gw.set_node_shape(rectangle_node);
    gw.set_edge_direction(undirected_edge);

    graph& G = gw.get_graph();

    G.permute_edges();

    if (!Is_Planar(G)) {
      panel P;
      P.text_item("");
      P.text_item("\\bf This graph is \\red not planar\\black .");
      P.button("ok",0);
      P.button("proof",1);
      if (gw.open_panel(P)) {
       // show a kuratowski subdivision
       gw_test_planar(gw);
      }
      continue;
    }


    node_array<double> x_pos(G);
    node_array<double> y_pos(G);
    node_array<double> x_rad(G);
    node_array<double> y_rad(G);
    edge_array<double> x_sanch(G);
    edge_array<double> y_sanch(G);
    edge_array<double> x_tanch(G);
    edge_array<double> y_tanch(G);

    if (!VISIBILITY_REPRESENTATION(G,x_pos,y_pos,x_rad,y_rad,x_sanch,y_sanch,
                                                             x_tanch,y_tanch))
       continue;

    edge_array<list<double> > x_bends(G);
    edge_array<list<double> > y_bends(G);

    double dx,dy,f;
    gw.fill_win_params(x_pos,y_pos,x_rad,y_rad,x_bends,y_bends,dx,dy,f,f);
    gw.transform_layout(x_pos,y_pos,x_rad,y_rad,x_bends,y_bends,dx,dy,f,f);
    gw.set_layout(x_pos,y_pos,x_rad,y_rad,x_bends,y_bends,
                                          x_sanch,y_sanch,x_tanch,y_tanch);
  }
  
  return 0;
}
