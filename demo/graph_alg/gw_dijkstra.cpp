/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_dijkstra.c
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


GRAPH<int,int> G;

void run_dijkstra(GraphWin& gw)
{ 
  bool flush = gw.set_flush(false);

  node s = G.first_node();

  if (s == nil) return; //empty graph

  node_array<edge> pred(G);


  gw.message("\\bf Computing Shortest Paths");

  DIJKSTRA(G,s,G.edge_data(),G.node_data(),pred);

  gw.set_edge_color(grey2);
  gw.set_edge_width(1);

  node v;
  forall_nodes(v,G) 
  { edge e = pred[v];
    if (e == nil) 
      gw.set_color(v,blue); // root
    else
    { gw.set_color(e,blue);
      gw.set_width(e,3);
     }
   }
  gw.set_flush(flush);
  gw.message("");
  gw.redraw();
}


void init_edge(GraphWin& gw, edge e)
{ G[e] = rand_int(0,99);
  gw.set_slider_value(e,G[e]/100.0,0);
}

void init_handler(GraphWin& gw)  
{ edge e;
  forall_edges(e,G) init_edge(gw,e);
  run_dijkstra(gw); 
 }

void new_edge_handler(GraphWin& gw, edge e)  
{ init_edge(gw,e);
  run_dijkstra(gw); 
 }

void edge_slider_handler(GraphWin& gw,edge e, double f) 
{ G[e] = int(100*f); }

void end_edge_slider_handler(GraphWin& gw, edge, double)
{ run_dijkstra(gw); }


int main() 
{
  node_array<double> xcoord;
  node_array<double> ycoord;

  grid_graph(G,xcoord,ycoord,5);

  GraphWin gw(G,"Dijkstra Demo");

  gw.set_node_shape(rectangle_node);
  gw.set_node_width(60);
  gw.set_node_height(60);
  gw.set_directed(true);

  gw.set_edge_label_type(data_label);
  gw.set_node_label_type(data_label);

  gw.set_init_graph_handler(init_handler);
  gw.set_undo_graph_handler(run_dijkstra);
  gw.set_del_edge_handler(run_dijkstra);
  gw.set_del_node_handler(run_dijkstra);
  gw.set_new_edge_handler(new_edge_handler);

  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);

  gw.set_position(xcoord,ycoord);
  gw.place_into_win();

  gw.display();
  gw.edit();

  return 0;
}

