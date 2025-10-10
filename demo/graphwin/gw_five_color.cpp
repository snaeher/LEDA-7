/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_five_color.c
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

int main() {

  graph G;

  GraphWin GW(G,"Five Coloring Demo");

  GW.set_node_label_type(no_label);
  GW.set_node_color(grey1);
  GW.set_flush(false);

  GW.display();

  GW.message("\\bf\\blue Please construct a planar graph and press done.");
  leda_wait(1.75);
  GW.message("");

  while (GW.edit()) {

    Delete_Loops(G);
    Make_Simple(G);
    GW.update_graph();

/*
    if (!Is_Planar(G)) 
    { GW.message("\\bf Graph must be planar.");
      leda_wait(1.5);
      GW.del_message();
      continue;
     }
*/

     node_array<int> col(G,0);

     FIVE_COLOR(G,col);

     node v;
     forall_nodes(v,G) GW.set_color(v,color::get(col[v]+2));
     GW.redraw();

  }

  return 0;
}
