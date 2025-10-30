/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_mcmflow.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;

static edge_map<int> cap;
static edge_map<int> cost;


void run_mcm_flow(GraphWin& gw)
{ bool flush = gw.set_flush(false);

  graph& G = gw.get_graph();
  node   s = G.first_node();
  node   t = G.last_node();

  gw.message("\\bf Computing MinCostMaxFlow");

  edge_array<int> flow(G);

  //int F = MIN_COST_MAX_FLOW(G,s,t,cap,cost,flow);
  int F = gw_min_cost_max_flow(gw,s,t,cap,cost,flow);
  int C = 0;

  // sum up total cost and indicate flow[e] by the width of e
  edge e;
  forall_edges(e,G) 
  { C += flow[e]*cost[e];
    gw.set_label_color(e,black);
    gw.set_label(e,string("%d",flow[e]));

    //gw.set_width(e,1+int((flow[e]+4)/5.0));
    gw.set_width(e,2+int(flow[e]/6.0));

    if (flow[e] == 0)
      gw.set_color(e,grey2); // 0-flow edges are faded to grey
    else
      gw.set_color(e,black);
   }
  gw.set_flush(flush);
  gw.message(string("\\bf Flow: %d  \\bf Cost: %d",F,C));
  gw.redraw();
}



void init_edge(GraphWin& gw, edge e)
{ // init capacity and cost to a random value
  cap[e] = rand_int(10,50);
  cost[e] = rand_int(10,75);
  // set sliders accordingly
  gw.set_slider_value(e,cap[e]/100.0,0);  // slider zero
  gw.set_slider_value(e,cost[e]/100.0,1); // slider one
}

void init_handler(GraphWin& gw)  
{ edge e;
  forall_edges(e,gw.get_graph()) init_edge(gw,e);
  run_mcm_flow(gw); 
}

void new_edge_handler(GraphWin& gw, edge e)  
{ init_edge(gw,e);
  run_mcm_flow(gw); 
}



// capacity sliders

void start_cap_slider_handler(GraphWin& gw, edge, double)
{ gw.message("\\bf\\blue Change Edge Capacity"); }

void cap_slider_handler(GraphWin& gw,edge e, double f) 
{ cap[e] = int(100*f); 
  gw.set_label_color(e,blue);
  gw.set_label(e,string("cap = %d",cap[e]));
}

void end_cap_slider_handler(GraphWin& gw, edge, double)
{ run_mcm_flow(gw); }


// cost sliders

void start_cost_slider_handler(GraphWin& gw, edge, double)
{ gw.message("\\bf\\red Change Edge Cost"); }

void cost_slider_handler(GraphWin& gw, edge e, double f) 
{ cost[e] = int(100*f); 
  gw.set_label_color(e,red);
  gw.set_label(e,string("cost = %d",cost[e]));
}

void end_cost_slider_handler(GraphWin& gw, edge, double)
{ run_mcm_flow(gw); }


int main() 
{
  // construct a (grid) graph

  graph G;
  node_array<double> xcoord;
  node_array<double> ycoord;
  grid_graph(G,xcoord,ycoord,5);

  // initialize cap and cost maps 
  cap.init(G);
  cost.init(G);


  GraphWin gw(G, "Min Cost Max Flow");

  // disable edge bends
  gw.set_action(A_LEFT | A_DRAG | A_EDGE , NULL);
  
  gw.set_init_graph_handler(init_handler);
  gw.set_undo_graph_handler(run_mcm_flow);
  gw.set_del_edge_handler(run_mcm_flow);
  gw.set_del_node_handler(run_mcm_flow);
  gw.set_new_edge_handler(new_edge_handler);


  gw.set_start_edge_slider_handler(start_cap_slider_handler,0);
  gw.set_edge_slider_handler(cap_slider_handler,0);
  gw.set_end_edge_slider_handler(end_cap_slider_handler,0);
  gw.set_edge_slider_color(blue,0);


  gw.set_start_edge_slider_handler(start_cost_slider_handler,1);
  gw.set_edge_slider_handler(cost_slider_handler,1);
  gw.set_end_edge_slider_handler(end_cost_slider_handler,1);
  gw.set_edge_slider_color(red,1);

  
  gw.set_node_color(yellow);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(no_label);
  gw.set_node_width(16);
  gw.set_node_height(16);

  gw.set_edge_direction(directed_edge);

  node s = G.first_node();
  gw.set_shape(s,rectangle_node);
  gw.set_width(s,32);
  gw.set_height(s,32);
  gw.set_color(s,blue2);
  gw.set_label(s,"s");

  node t = G.last_node();
  gw.set_shape(t,rectangle_node);
  gw.set_width(t,32);
  gw.set_height(t,32);
  gw.set_color(t,green2);
  gw.set_label(t,"t");

  // open window
  gw.display();

  //adjust layout
  gw.set_flush(false);
  gw.adjust_coords_to_win(xcoord,ycoord);
  gw.set_layout(xcoord,ycoord);
  gw.set_flush(true);
  gw.redraw();

  gw.display_help_text("gw_mcmflow");

  gw.edit();

  return 0;
}
