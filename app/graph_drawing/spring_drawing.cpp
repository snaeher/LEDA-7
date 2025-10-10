/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_spring.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graph/graph_alg.h>
#include<LEDA/graphics/graphwin.h>


using namespace leda;



void fix_node(GraphWin& gw, const point&)
{ // position no used
  node v = gw.get_found_node();
  if (gw.get_color(v) != red) 
     gw.set_color(v,red);
  else
     gw.set_color(v,yellow);
 }


void update_drawing(GraphWin& gw, int steps=500)
{ const graph& G = gw.get_graph();

  double xleft   = gw.get_xmin();
  double xright  = gw.get_xmax();
  double ybottom = gw.get_ymin();
  double ytop    = gw.get_ymax();

  double x0,y0,x1,y1;
  gw.get_bounding_box(x0,y0,x1,y1);

  if (x0 < xleft) xleft = x0;
  if (x1 > xright) xright = x1;
  if (y0 < ybottom) ybottom = y0;
  if (y1 > ytop) ytop = y1;


  node_array<double> xpos(G);
  node_array<double> ypos(G);

  node_array<double> xrad(G);
  node_array<double> yrad(G);

  node v;
  forall_nodes(v,G) {
    point p = gw.get_position(v);
    xpos[v] = p.xcoord();
    ypos[v] = p.ycoord();
    xrad[v] = gw.get_radius1(v);
    yrad[v] = gw.get_radius2(v);
  }

  //list<node> fixed = gw.get_selected_nodes();

  list<node> fixed;
  forall_nodes(v,G)
      if (gw.get_color(v) == red) fixed.append(v);
 
  SPRING_EMBEDDING(G,fixed,xpos,ypos,xrad,yrad,xleft,xright,ybottom,ytop,steps);
  //SPRING_EMBEDDING(G,xpos,ypos,xrad,yrad,steps);
  gw.set_position(xpos,ypos);
}




void move_node_handler(GraphWin& GW, node v) { update_drawing(GW); }
void move_node_handler1(GraphWin& GW, node v) { update_drawing(GW,4); }
void init_graph_handler(GraphWin& GW)        { update_drawing(GW); }
void del_node_handler(GraphWin& GW)          { update_drawing(GW); }
void new_edge_handler(GraphWin& GW, edge e)  { update_drawing(GW); }
void del_edge_handler(GraphWin& GW)          { update_drawing(GW); }


void about(GraphWin& GW)
{ 
  window& W = GW.get_window();

  panel P(3*W.width()/4,-1);

  P.text_item("\
  \\bf\\blue Spring Embedding Demo \\black\\tf\\c10 \
  This program demonstrates SPRING EMBEDDING of graphs.\
  Edit a graph and select some nodes by clicking with the left\
  mouse button while simultaneously holding down the CTRL key.\
  The selected nodes will become red and be fixed.\\12\
  Now, move the red nodes or change the graph. You get the\
  best results for sparse planar graph.");

  P.button("ok");

  GW.open_panel(P);
}



int main()
{
  GraphWin GW("Spring Embedding");

  GW.set_action(A_LEFT|A_NODE|A_CTRL,fix_node);

  //GW.set_move_node_handler(move_node_handler1);
  GW.set_init_graph_handler(init_graph_handler);
  GW.set_end_move_node_handler(move_node_handler);
  GW.set_del_node_handler(del_node_handler);
  GW.set_new_edge_handler(new_edge_handler);
  GW.set_del_edge_handler(del_edge_handler);


  GW.set_node_shape(rectangle_node);
  GW.set_node_radius1(20);
  GW.set_node_radius2(15);

  GW.display(window::center,window::center);

  int h_menu = GW.get_menu("Help");
  gw_add_simple_call(GW,about,"About Spring Embed",h_menu);

  about(GW);
  GW.edit();

  return 0;
}
