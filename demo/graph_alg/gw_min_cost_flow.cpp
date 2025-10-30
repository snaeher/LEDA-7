/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_min_cost_flow.c
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


const color cap_c = blue;
const color cost_c = red;


static GRAPH<int,int> G;

static edge_map<int> Gcap(G);
static edge_map<int> Gcost(G);


void run_mcm_flow(GraphWin& gw)
{ 
  bool flush = gw.set_flush(false);

  node s = G.first_node();
  node t = G.last_node();

  if (s == t) return;

  gw.message("\\bf Computing MinCostMaxFlow");

  edge_array<int> flow(G);
  edge_array<int> cap(G);
  edge_array<int> cost(G);

  edge e;
  forall_edges(e,G) cap[e]  = Gcap[e];
  forall_edges(e,G) cost[e] = Gcost[e];

  int F = MIN_COST_MAX_FLOW(G,s,t,cap,cost,flow);
  int C = 0;

  forall_edges(e,G) 
  { C += flow[e]*cost[e];
    gw.set_label_color(e,black);
    gw.set_label(e,string("%d",flow[e]));
    gw.set_width(e,1+int((flow[e]+4)/5.0));
    if (flow[e] == 0)
      gw.set_color(e,grey2);
    else
      gw.set_color(e,black);
   }

  node v;
  forall_nodes(v,G) 
  { gw.set_color(v,yellow);
    gw.set_shape(v,circle_node);
    gw.set_label(v,"");
    gw.set_width(v,14);
    gw.set_height(v,14);
   }

  gw.set_width(s,22);
  gw.set_height(s,22);
  gw.set_color(s,cyan);
  gw.set_label(s,"S");

  gw.set_width(t,22);
  gw.set_height(t,22);
  gw.set_color(t,cyan);
  gw.set_label(t,"T");

  gw.set_flush(flush);

  gw.message(string("\\bf Total Flow: %d ~~~~~\\red Total Cost: %d",F,C));
  gw.redraw();
}


void init_edge(GraphWin& gw, edge e)
{ Gcap[e] = rand_int(10,50);
  Gcost[e] = rand_int(10,75);
  gw.set_slider_value(e,Gcap[e]/100.0,0);
  gw.set_slider_value(e,Gcost[e]/100.0,1);
}

void init_handler(GraphWin& gw)  
{ edge e;
  forall_edges(e,G) init_edge(gw,e);
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
{ Gcap[e] = int(100*f); 
  gw.set_label_color(e,cap_c);
  gw.set_label(e,string("cap = %d",Gcap[e]));
}

void end_cap_slider_handler(GraphWin& gw, edge, double)
{ run_mcm_flow(gw); }



// cost sliders

void start_cost_slider_handler(GraphWin& gw, edge, double)
{ gw.message("\\bf\\red Change Edge Cost"); }

void cost_slider_handler(GraphWin& gw, edge e, double f) 
{ Gcost[e] = int(100*f); 
  gw.set_label_color(e,cost_c);
  gw.set_label(e,string("cost = %d",Gcost[e]));
}

void end_cost_slider_handler(GraphWin& gw, edge, double)
{ run_mcm_flow(gw); }



int main() 
{
  node_array<double> xcoord;
  node_array<double> ycoord;

  grid_graph(G,xcoord,ycoord,5);

  GraphWin gw(G,"Min Cost Max Flow");

  // disable edge bends

  gw.set_action(A_LEFT | A_DRAG | A_EDGE , NULL);
  gw.win_init(0,200,0);

  // set handlers

  gw.set_init_graph_handler(init_handler);

  gw.set_del_edge_handler(run_mcm_flow);
  gw.set_del_node_handler(run_mcm_flow);
  gw.set_new_edge_handler(new_edge_handler);

  gw.set_start_edge_slider_handler(start_cap_slider_handler,0);
  gw.set_edge_slider_handler(cap_slider_handler,0);
  gw.set_end_edge_slider_handler(end_cap_slider_handler,0);
  gw.set_edge_slider_color(cap_c,0);

  gw.set_start_edge_slider_handler(start_cost_slider_handler,1);
  gw.set_edge_slider_handler(cost_slider_handler,1);
  gw.set_end_edge_slider_handler(end_cost_slider_handler,1);
  gw.set_edge_slider_color(cost_c,1);


  gw.set_node_shape(rectangle_node);
  gw.set_directed(true);

  gw.set_position(xcoord,ycoord);
  gw.place_into_win();

  gw.display();
  gw.edit();

  return 0;
}

