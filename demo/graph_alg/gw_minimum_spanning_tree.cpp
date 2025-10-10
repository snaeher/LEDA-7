/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_minimum_spanning_tree.c
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
int min_cost,max_cost;

void display_minimum_spanning_tree(GraphWin& gw)
{ 
  gw.set_flush(false);
  
  list<edge> M = MIN_SPANNING_TREE(G,G.edge_data());
 
  edge e; 
  forall_edges(e,G) 
  { gw.set_width(e,1); 
    gw.set_color(e,black); 
  }
  
  forall(e,M)
  { gw.set_width(e,4); gw.set_color(e,blue); }
  gw.redraw();

}

void init_handler(GraphWin& gw)
{ edge e;
  forall_edges(e,G) 
  { G[e] = rand_int(min_cost,max_cost);
    gw.set_slider_value(e,G[e]/((double) max_cost));
  }
  display_minimum_spanning_tree(gw);
}


void new_edge_handler(GraphWin& gw, edge e)  
{ G[e] = rand_int(min_cost,max_cost);
  gw.set_slider_value(e,G[e]/((double) max_cost));
  display_minimum_spanning_tree(gw); 
}

void del_edge_handler(GraphWin& gw)     { display_minimum_spanning_tree(gw); }
void new_node_handler(GraphWin& gw,node){ display_minimum_spanning_tree(gw); }
void del_node_handler(GraphWin& gw)     { display_minimum_spanning_tree(gw); }

void edge_slider_handler(GraphWin& gw, edge e, double f)
{ G[e] = int(max_cost * f);
  display_minimum_spanning_tree(gw); 
}

void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ display_minimum_spanning_tree(gw); }

void about_minspantree(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(2*W.width()/3,-1);
 
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
  gw.display();

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_minspantree, "About Minimum Spanning Trees",h_menu);

  about_minspantree(gw);

  min_cost = 0;
  max_cost = 9;

  gw.set_action(A_LEFT|A_IMMEDIATE|A_EDGE,0);
  gw.set_action(A_LEFT|A_DRAG|A_EDGE,0); // prevent adding edge bends
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
  gw.set_flush(false);
#ifdef BOOK
  gw.set_node_color(white);
#endif

  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);


  gw.edit();

  return 0;
}
