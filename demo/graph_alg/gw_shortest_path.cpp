/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_shortest_path.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/map.h>

using namespace leda;


GRAPH<int,int> G;

int min_cost = -5;
int max_cost = 9;

void display_shortest_path(GraphWin& gw)
{ gw.set_flush(false);
  node_array<int> dist(G);
  node_array<edge> pred(G);

  node s = G.first_node();

  gw.set_shape(s,square_node);
  
  SHORTEST_PATH(G,s,G.edge_data(),dist,pred);
  node_array<int> type = CHECK_SP(G,G.first_node(),G.edge_data(),dist,pred);


  node v;
  forall_nodes(v,G) 
  { if (type[v] > 0 )
    { gw.set_color(v,black);
      gw.set_user_label(v,"+oo");
    }
    if (type[v] == 0 )
    { gw.set_color(v,(v == s)? blue : blue2);
      gw.set_user_label(v,string("%d",dist[v]));
    }
    if (type[v] < 0 )
    { gw.set_color(v,yellow);
      gw.set_user_label(v,"-oo");
    }
    
  }

  edge e; 
  forall_edges(e,G) 
  { node v = G.target(e);
    if ( e == pred[v] )
    { gw.set_width(e,3); 
      if (type[v] == 0  ) gw.set_color(e,blue);
      if (type[v] == -1 ) gw.set_color(e,violet);
      if (type[v] == -2 ) gw.set_color(e,red);
    }
    else
    { gw.set_width(e,1);
      
    }
    gw.set_user_label(e,string("%d",G[e]));
  }

  gw.redraw();

}

void init_handler(GraphWin& gw)
{ edge e;
  forall_edges(e,G) 
  { G[e] = rand_int(min_cost+1,max_cost-1);
    gw.set_slider_value(e,(G[e] - min_cost)/((double) (max_cost - min_cost)));
  }
  display_shortest_path(gw);
}


void new_edge_handler(GraphWin& gw, edge e)  
{ G[e] = rand_int(min_cost+1,max_cost-1);
  gw.set_slider_value(e,(G[e] - min_cost)/((double) (max_cost - min_cost)));
  display_shortest_path(gw); 
}


void del_edge_handler(GraphWin& gw)        { display_shortest_path(gw); }
void new_node_handler(GraphWin& gw,node)   { display_shortest_path(gw); }
void del_node_handler(GraphWin& gw)        { display_shortest_path(gw); }

void edge_slider_handler(GraphWin& gw, edge e, double f)
{ G[e] = int(min_cost + (max_cost - min_cost) * f);
  display_shortest_path(gw); 
}

void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ G[e] = int(min_cost + (max_cost - min_cost) * f);
  display_shortest_path(gw); 
}


void about_shortpath(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(3*W.width()/4,-1);

P.text_item("\\bf Shortest Paths\\rm");
P.text_item("");
P.text_item("This demo illustrates the output convention of shortest-paths ");
P.text_item("algorithms.");
P.text_item("");
P.text_item("You can construct and edit a graph in the graphics window.");
P.text_item("The first node entered is the source node of the shortest-path ");
P.text_item("problem. Every edge has a slider that can be used to adjust ");
P.text_item("the cost value of the edge. The solution of the shortest path ");
P.text_item("computation is updated after each edit operation.");
P.text_item("");
P.text_item("Unreachable nodes are labeled by +infinity.");
P.text_item(" ");
P.text_item("Reachable nodes with finite distance are labeled with their distance.");
P.text_item("");
P.text_item("Reachable nodes for which there are paths with");
P.text_item("arbitrary negative length are labeled with -infinity.");
P.text_item("");
P.text_item("For each reachable node one of the incoming edges is highlighted.");
P.text_item("For each node with finite distance this is the");
P.text_item("last edge on a shortest path to the node.");
P.text_item("For the nodes with distance minus infinity these edges form");
P.text_item("negative cycles plus paths emanating from these cycles."); 

P.button("continue");

  gw.open_panel(P);
}

int main()
{ 
  GraphWin gw(G,"Shortest Paths");

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_shortpath, "About Shortest Paths",h_menu);

  about_shortpath(gw);

  gw.set_action(A_LEFT|A_IMMEDIATE|A_EDGE,0);
  gw.set_action(A_LEFT|A_DRAG|A_EDGE,0); // prevent adding edge bends
  gw.set_action(A_LEFT|A_EDGE,0);


  gw.set_init_graph_handler(init_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(true);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);

  //gw.set_node_width(20);

  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);


  gw.edit();

  return 0;
}

