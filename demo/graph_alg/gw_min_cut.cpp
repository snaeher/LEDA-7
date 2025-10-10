/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_min_cut.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/min_cut.h>

using namespace leda;

 
const color cap_c = blue;
const color cost_c = red;

//static GRAPH<int,int> G;
static graph G;

static edge_map<int> Gcost(G);


void run_min_cut(GraphWin& gw)
{ 
  bool flush = gw.set_flush(false);

  gw.message("\\bf Computing Minimum Cut");

  edge_array<int> cost(G);

  node v; edge e;
  forall_edges(e,G) cost[e] = Gcost[e];

  list<node> C;
  int F = MIN_CUT(G,cost,C,true);

  node_array<bool> in_C(G,false);
  forall(v,C) in_C[v] = true;
  
  forall_edges(e,G) 
  { 
    gw.set_label(e,string("%d",cost[e]));
    node v = G.source(e), w = G.target(e);
    if ( in_C[v] != in_C[w] )
      gw.set_color(e,red);
    else
      gw.set_color(e,black);
   }

  forall_nodes(v,G) 
  { gw.set_color(v, in_C[v] ? blue : black );
    gw.set_shape(v, in_C[v] ? circle_node : square_node );
  }

  gw.set_flush(flush);

  gw.message(string("\\bf Cut Value: %d ",F));
  gw.redraw();
}


void init_edge(GraphWin& gw, edge e)
{ 
  Gcost[e] = rand_int(10,75);
  gw.set_slider_value(e,Gcost[e]/100.0,1);
}

void init_handler(GraphWin& gw)  
{ edge e;
  forall_edges(e,G) init_edge(gw,e);
  run_min_cut(gw); 
 }

void new_edge_handler(GraphWin& gw, edge e)  
{ init_edge(gw,e);
  run_min_cut(gw); 
 }


// capacity sliders

/*
void start_cap_slider_handler(GraphWin& gw, edge, double)
{ gw.message("\\bf\\blue Change Edge Capacity"); }

void cap_slider_handler(GraphWin& gw,edge e, double f) 
{ Gcap[e] = int(100*f); 
  gw.set_label_color(e,cap_c);
  gw.set_label(e,string("cap = %d",Gcap[e]));
}

void end_cap_slider_handler(GraphWin& gw, edge, double)
{ run_min_cut(gw); }
*/



// cost sliders

void start_cost_slider_handler(GraphWin& gw, edge, double)
{ gw.message("\\bf\\red Change Edge Cost"); }

void cost_slider_handler(GraphWin& gw, edge e, double f) 
{ Gcost[e] = int(100*f); 
  gw.set_label_color(e,cost_c);
  gw.set_label(e,string("cost = %d",Gcost[e]));
}

void end_cost_slider_handler(GraphWin& gw, edge, double)
{ run_min_cut(gw); }



int main() 
{
  //node_array<double> xcoord;
  //node_array<double> ycoord;

  //grid_graph(G,xcoord,ycoord,5);

  GraphWin gw(G,"Minimum Cut");

  gw.add_help_text("gw_min_cut");
  gw.display();

  gw.display_help_text("gw_min_cut");

  // disable edge bends

  gw.set_action(A_LEFT | A_DRAG | A_EDGE , NULL);
  gw.win_init(0,200,0);

  // set handlers

  gw.set_init_graph_handler(init_handler);

  gw.set_del_edge_handler(run_min_cut);
  gw.set_del_node_handler(run_min_cut);
  gw.set_new_edge_handler(new_edge_handler);

  //gw.set_start_edge_slider_handler(start_cap_slider_handler,0);
  //gw.set_edge_slider_handler(cap_slider_handler,0);
  //gw.set_end_edge_slider_handler(end_cap_slider_handler,0);
  //gw.set_edge_slider_color(cap_c,0);

  gw.set_start_edge_slider_handler(start_cost_slider_handler,1);
  gw.set_edge_slider_handler(cost_slider_handler,1);
  gw.set_end_edge_slider_handler(end_cost_slider_handler,1);
  gw.set_edge_slider_color(cost_c,1);


  gw.set_node_shape(rectangle_node);
  gw.set_directed(false);

  //gw.set_position(xcoord,ycoord);
  //gw.place_into_win();


  gw.edit();

  return 0;
}

