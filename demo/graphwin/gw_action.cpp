/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_action.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include<LEDA/graphics/graphwin.h>

using namespace leda;

void change_node_color(GraphWin& gw, const point&)
{ node v  = gw.get_edit_node();
  int i = color::index(gw.get_color(v));
  gw.set_color(v,color::get((i+1)%17));
 }

void change_edge_color(GraphWin& gw, const point&)
{ edge e  = gw.get_edit_edge();
  int i = color::index(gw.get_color(e));
  gw.set_color(e,color::get((i+1)%17));
 }


void center_node(GraphWin& gw, const point& p)
{ node v  = gw.get_edit_node();
  gw.set_position(v,p);
 }

void delete_node(GraphWin& gw, const point&)
{ node v  = gw.get_edit_node();
  gw.del_node(v);
 }

void zoom_up(GraphWin& gw, const point&)   { gw.zoom(1.5); }
void zoom_down(GraphWin& gw, const point&) { gw.zoom(0.5); }


int main()
{
  GraphWin gw;
 
  gw.set_action(A_LEFT | A_NODE | A_CTRL,  change_node_color);
  gw.set_action(A_LEFT | A_EDGE | A_CTRL,  change_edge_color);
  gw.set_action(A_LEFT | A_NODE | A_SHIFT, center_node);
  gw.set_action(A_RIGHT| /* A_IMMEDIATE | */ A_NODE, delete_node);

  gw.set_action(A_LEFT | A_CTRL, zoom_up);
  gw.set_action(A_RIGHT| A_CTRL, zoom_down);

  gw.display(window::center,window::center);
  gw.edit();

  return 0;
}

