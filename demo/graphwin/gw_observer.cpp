/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_observer.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/gw_observer.h>

using namespace leda;

static gw_observer* obs1;
static gw_observer* obs2;
static gw_observer* obs3;

bool turn_off_events(GraphWin& gw)
{ obs1->set_online(false);
  obs2->set_online(false);
  obs3->set_online(false);
  return true;
 }

void turn_on_events(GraphWin& gw)
{ obs1->set_online(true);
  obs2->set_online(true);
  obs3->set_online(true);
  obs1->update_layout(gw);
  obs2->update_layout(gw);
  obs3->update_layout(gw);
}


int main() 
{
  int win_w = 502;
  int win_h = 358;

  GraphWin gw(win_w,win_h);
  gw.set_init_graph_handler(turn_off_events);
  gw.set_init_graph_handler(turn_on_events);
  gw.set_edge_direction(undirected_edge);
  gw.display(window::min,window::min);


  gw_observer observ1(gw,win_w,win_h,"Spring Embedder");
  observ1.set_draw_func(gw_spring_embed);
  observ1.set_edge_direction(undirected_edge);
  observ1.display(window::max,window::min);

  gw_observer observ2(gw,win_w,win_h,"Orthogonal Draw");
  observ2.set_draw_func(gw_ortho_embed);
  observ2.set_edge_direction(undirected_edge);
  observ2.display(window::min,window::max);

  gw_observer observ3(gw,win_w,win_h,"Visibility Repesentation");
  observ3.set_draw_func(gw_visrep_embed);
  observ3.set_edge_direction(undirected_edge);
  observ3.display(window::max,window::max);

  obs1 = &observ1;
  obs2 = &observ2;
  obs3 = &observ3;

  gw.edit();

  return 0;
}

