/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_mw_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h> 
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/mw_matching.h>
#include <LEDA/core/array.h>

using namespace leda;


static map<edge,int> weight(-1);

static int min_weight = 1;
static int max_weight = 10;
static graph G;
static string mes;

void display_general_matching(GraphWin& gw)
{ gw.set_flush(false);
  //graph& G = gw.get_graph();

  list<edge> M;  
  edge_array<int> w(G);
  //array<int> u;
  //array<pair<int,int> > BT;
  //array<int> b;

  edge e;
  forall_edges(e,G) w[e] = 4*weight[e];  //s.n. (weights must be multiple of 4)
      
  M = MAX_WEIGHT_MATCHING(G,w);
  
  //CHECK_MAX_WEIGHT_MATCHING(G,M,u,BT,b);

  gw.set_node_label_type(user_label);

  /*
  forall_nodes(v,G) 
  { gw.set_color(v,OSC[v]);
    gw.set_user_label(v,string("%d",OSC[v]));
  }
  */
          
  forall_edges(e,G) 
  { gw.set_color(e,black);
    gw.set_width(e,1);
    gw.set_user_label(e,string("%d",w[e]));
  }
  forall(e,M) 
  { gw.set_color(e,blue);
    gw.set_width(e,3); 
  }
  gw.redraw();
}


void init_handler(GraphWin& gw)
{ //graph& G = gw.get_graph();
  edge e;
  forall_edges(e,G) 
  { weight[e]= rand_int(min_weight+1,max_weight-1);
    gw.set_slider_value(e,(weight[e] - min_weight)/((double) (max_weight - min_weight)));
  }
  
  display_general_matching(gw);  
}

void new_edge_handler(GraphWin& gw, edge e)  
{
  weight[e] = rand_int(min_weight+1,max_weight-1);
  gw.set_slider_value(e,(weight[e] - min_weight)/((double) (max_weight - min_weight)));
  display_general_matching(gw); 
}

void del_edge_handler(GraphWin& gw)        { display_general_matching(gw); }

void del_node_handler(GraphWin& gw)        { display_general_matching(gw); }

void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ 
  weight[e] = int(min_weight + (max_weight - min_weight) * f);
  display_general_matching(gw); 
}

void start_edge_slider_handler(GraphWin& gw, edge e, double f)
{ 
  weight[e] = int(min_weight + (max_weight - min_weight) * f);
  //display_general_matching(gw); 
}

void edge_slider_handler(GraphWin& gw, edge e, double f)
{ 
  weight[e] = int(min_weight + (max_weight - min_weight) * f);
  //display_general_matching(gw); 
}

int main()
{ GraphWin gw(G,"Maximum Weight Matching");

  gw.add_help_text("gw_mw_matching");
  gw.display();

  gw.display_help_text("gw_mw_matching");

  // disable edge bends

  gw.set_action(A_LEFT | A_DRAG | A_EDGE , NULL);
  //gw.win_init(0,200,0);

  // set handlers, copied from gw_min_cost_flow

  gw.set_init_graph_handler(init_handler);

  gw.set_del_edge_handler(del_edge_handler);
  gw.set_del_node_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);

  gw.set_start_edge_slider_handler(start_edge_slider_handler);
  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_edge_label_type(user_label);
  gw.set_node_width(20);

  gw.edit();

  return 0;
}

