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
  \\bf Orthogonal Drawing of Planar Graphs\\tf\\c12\
  This program demonstrates the algorithm of \\blue\\bf Tamassia\\tf\\black\
  that computes an \\bf\\blue orthogonal drawing\\tf\\black with a minimal\
  number of bends for planar input graphs.\\15\
  Edit or create a random graph and click done. If the graph is planar an\
  orthogonal drawing will be shown. Otherwise you can ask for a proof\
  of non-planarity.");

  P.button("ok");

  gw.open_panel(P);
}


int main()
{
  GraphWin gw("Orthogonal Drawing");

  gw.set_node_shape(rectangle_node);
  gw.set_edge_direction(undirected_edge);

  gw.display();

  int help_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about,"About Ortho Demo",help_menu);

  about(gw);

  gw.message("Please construct or create a planar graph\
             and click \\blue done\\black.");

  while(gw.edit()) {
    gw.get_graph().permute_edges();
    gw_ortho_embed(gw);
  }
  
  return 0;
}
