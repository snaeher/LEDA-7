/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/pixmaps/texture/space.xpm>
#include <LEDA/graphics/pixmaps/tintin.xpm>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cin;
using std::endl;
#endif


int main() 
{
  graph G;

  node_array<double> xcoord;
  node_array<double> ycoord;
  grid_graph(G,xcoord,ycoord,4);

  GraphWin gw(G);

  gw.set_d3_look(true);

/*
  gw.set_bg_xpm(space_xpm);
*/

  gw.set_node_shape(rectangle_node);

  //gw.set_node_label_type(no_label);
  gw.set_node_label_type(gw_label_type(user_label|index_label));
  gw.set_node_label_pos(north_pos);

  gw.set_node_color(grey2);

  char* tintin = gw.get_window().create_pixrect(tintin_xpm);
  gw.set_node_pixmap(tintin);

  gw.set_edge_color(violet);
  gw.set_edge_width(2);

  gw.set_position(xcoord,ycoord);
  gw.place_into_win();

  gw.display();
  gw.edit();

  return 0;
}

