/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_min_spanning_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/map.h>

using namespace leda;

GRAPH<int,int> G;

map<edge,int> c(-1000);

int min_c,max_c;

void display_minimum_spanning_tree(GraphWin& gw)
{ 
  bool flush = gw.set_flush(false);

  edge e; 

  gw.message("\\bf\\blue Computing Minimum Spanning Tree");
  
  list<edge> M = MIN_SPANNING_TREE(G,G.edge_data());
 
  forall_edges(e,G) 
  { gw.set_width(e,1); 
    gw.set_color(e,black); 
  }
  
  forall(e,M)
  { gw.set_width(e,4); gw.set_color(e,blue); }

  gw.set_flush(flush);

  gw.message("");
  gw.redraw();
}

void init_handler(GraphWin& gw)
{ edge e;
  forall_edges(e,G) 
  { G[e] = rand_int(min_c,max_c);
    gw.set_slider_value(e,double(G[e]-min_c)/(max_c-min_c));
  }
  display_minimum_spanning_tree(gw);
}


void new_edge_handler(GraphWin& gw, edge e)  
{ G[e] = rand_int(min_c,max_c);
  gw.set_slider_value(e,G[e]/((double) max_c));
  display_minimum_spanning_tree(gw); 
}

void del_edge_handler(GraphWin& gw)     { display_minimum_spanning_tree(gw); }
void new_node_handler(GraphWin& gw,node){ display_minimum_spanning_tree(gw); }
void del_node_handler(GraphWin& gw)     { display_minimum_spanning_tree(gw); }

void edge_slider_handler(GraphWin& gw, edge e, double f)
{ G[e] = int(max_c * f);
  //display_minimum_spanning_tree(gw); 
}

void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ display_minimum_spanning_tree(gw); }

void about_minspantree(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P;
 
  //P.set_panel_bg_color(win_p->mono() ? white : ivory);

P.text_item("This demo illustrates the minimum spanning tree algorithm.");
P.text_item("");
P.text_item("Edge costs will be random integers in the range [0,9].");
P.text_item("");
P.text_item("You may edit a graph in the graphics window.");
P.text_item("");
P.text_item("After every edit operation a minimum spanning tree will be shown.");

  P.text_item("");


  P.button("OK");
  W.disable_panel();
  P.open(W);
  W.enable_panel();
}

int main()
{ 
  GraphWin gw(G,"Minimum Spanning Trees");

  min_c = 0;
  max_c = 32;

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw.add_simple_call(about_minspantree,"About Minimum Spanning Trees",h_menu);

  about_minspantree(gw);


  gw.set_action(A_LEFT|A_IMMEDIATE|A_EDGE,gw_slide_edge);
  gw.set_action(A_LEFT|A_DRAG|A_EDGE,gw_slide_edge);
  gw.set_action(A_LEFT|A_EDGE,0);

  gw.set_init_graph_handler(init_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_edge_label_type(data_label);
  gw.set_node_width(20);
  gw.set_node_height(20);

#ifdef BOOK
  gw.set_node_color(white);
#endif

  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);

  gw.edit();

  return 0;
}
