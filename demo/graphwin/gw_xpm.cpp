/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_xpm.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>

using namespace leda;

int main() 
{
  graph G;

  node_array<double> xcoord;
  node_array<double> ycoord;
  grid_graph(G,xcoord,ycoord,6);

  GraphWin gw(G);

  gw.set_node_shape(rectangle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_label_font(bold_font,8);
  gw.set_node_label_pos(northwest_pos);
  gw.set_node_color(grey1);

  gw.set_position(xcoord,ycoord);
  gw.place_into_win();
  gw.display();

  string xpm_dir = "../../incl/LEDA/graphics/pixmaps/button32";

  list<string> xpm_files = get_files(xpm_dir);

  if (!xpm_files.empty())
  { list_item it = xpm_files.first();
    node v;
    forall_nodes(v,G)
    { string s = xpm_files[it];
      gw.set_label(v,s.del(".xpm")+"@"+s);
      it = xpm_files.cyclic_succ(it);
     }
  }

  gw.message("");

  gw.place_into_win();
  gw.edit();

  return 0;
}

