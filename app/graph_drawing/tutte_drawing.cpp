/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_tutte.c
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


// ------------------------------------------------------------- //
//                    GraphWin interface                         //
// ------------------------------------------------------------- //



void fix_node(GraphWin& gw, const point&)
{ // position no used
  node v = gw.get_found_node();
  if (gw.get_color(v) != red) 
     gw.set_color(v,red);
  else
     gw.set_color(v,gw.get_node_color());
 }


void update_drawing(GraphWin& GW)
{ // redraw the graph according to the positions of the red nodes

  const graph& G = GW.get_graph();

  // get fixed nodes
  list<node> fixed_nodes;

  node v;
  forall_nodes(v,G) 
     if(GW.get_color(v) == red) fixed_nodes.append(v);

  node_array<double> xpos(G);
  node_array<double> ypos(G);

  forall(v,fixed_nodes) 
  { point p = GW.get_position(v);
    xpos[v] = p.xcoord();
    ypos[v] = p.ycoord();
   }

  if (TUTTE_EMBEDDING(G,fixed_nodes,xpos,ypos)) GW.set_layout(xpos,ypos);
}


// --------------------- handler ------------------------------ //


void move_node_handler(GraphWin& GW, node v) { 
  if (GW.get_color(v) == red) update_drawing(GW);
}

void del_node_handler(GraphWin& GW)         { update_drawing(GW); }
void new_edge_handler(GraphWin& GW, edge e) { update_drawing(GW); }
void del_edge_handler(GraphWin& GW)         { update_drawing(GW); }


void about(GraphWin& GW)
{ 
  window& W = GW.get_window();

  panel P(3*W.width()/4,-1);

  P.text_item("\
  \\bf\\blue Tutte Drawing Demo \\black\\tf \\c4\
  This program demonstrates the \\bf Tutte Drawing \\tf\
  procedure. Some designated nodes of the graph have fixed positions,\
  and all other nodes are drawn in the center of gravity\
  of their neighbors.\
  Edit a graph and select some nodes by clicking with the left\
  mouse button while simultaneously holding down the CTRL key.\
  The selected nodes will become red and be the fixed nodes as\
  described above. Now, move the red nodes or change the graph.\\4 \
  You get the best results for planar triangulated graphs.\
  ");

  P.button("ok");

  GW.open_panel(P);
}


int main()
{
  GraphWin GW("Tutte Demo");

  GW.set_edge_direction(undirected_edge);

  GW.set_action(A_LEFT|A_NODE|A_CTRL, fix_node);
  GW.add_node_menu("fix node",fix_node);

  GW.set_animation_steps(0);


  GW.set_move_node_handler(move_node_handler);
  //GW.set_end_move_node_handler(move_node_handler);

  GW.set_del_node_handler(del_node_handler);
  GW.set_new_edge_handler(new_edge_handler);
  GW.set_del_edge_handler(del_edge_handler);

  GW.display();

  int h_menu = GW.get_menu("Help");
  gw_add_simple_call(GW,about,"About Tutte Demo",h_menu);

  about(GW);
  GW.edit();

  return 0;
}
