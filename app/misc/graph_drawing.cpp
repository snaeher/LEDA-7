/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw.c
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

static void exit_handler(window* wp) {
  exit(0);
}


int main()
{
  bool interactive = getenv("LEDA_OPEN_MAXIMIZED")==0;

  int n = 10;
  int m = 25;

  GraphWin gw("GraphWin");

  gw.set_animation_steps(120);
  gw.set_show_status(false);

  if (!interactive) gw.set_frameless(true);

  gw.display();

  graph& G = gw.get_graph();
  window& W = gw.get_window();

  W.set_window_close_handler(exit_handler);

  while (gw.get_edit_result() == -1)
  { gw.clear_graph(); 
    gw.set_node_label_type(index_label);
    gw.set_node_shape(circle_node);
    gw.set_edge_direction(undirected_edge);

    random_planar_graph(G,n,m);
    gw.update_graph();

    gw.set_flush(false);
    gw_random_embed(gw);
    gw.set_flush(true);

    gw_spring_embed(gw);
    sleep(0.5);

    //gw.set_node_label_type(no_label);
    gw_ortho_embed(gw);
    sleep(0.5);

    gw_visrep_embed(gw);

    if (interactive)
      gw.edit();
    else
      W.read_mouse();
  }

  return 0;
}

