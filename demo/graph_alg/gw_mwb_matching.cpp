/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_mwb_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <assert.h>
#include <LEDA/graph/mwb_matching.h>
#include <LEDA/graph/templates/mwb_matching.h>
#include <LEDA/core/map.h>

using namespace leda;


enum {MAX_WEIGHT, MAX_ASSIGN, MIN_ASSIGN, MAX_WEIGHT_MAX_CARD, GEO};
int which = MAX_WEIGHT;

graph G;

const int min_weight = -4;
const int max_weight = 8;
map<edge, int> weight(min_weight - 1);
map<node, int> col(0);
int next_color = 1;

void display_mwb_matching(GraphWin& gw)
{ gw.set_flush(false);
  window& W = gw.get_window();
  W.clear();

  edge e; node v;
  
  edge_array<int> w(G);

  forall_edges(e,G)
    if ( which == GEO )
    { point p = gw.get_position(G.source(e));
      point q = gw.get_position(G.target(e));
      w[e] = (int) p.distance(q);
    }
    else
    { if ( weight[e] == min_weight - 1 ) weight[e] = rand_int(min_weight,max_weight);
      w[e] = weight[e];
    }
    
  list<edge> M;
  node_array<int> pot(G);

  list<node> A,B;
  forall_nodes(v,G)
  { if ( col[v] == 1 ) A.append(v); else B.append(v);
    gw.set_color(v,col[v] == 1? red : green);
  }    
  
  if (which == MAX_WEIGHT || which == GEO )
  { M = MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,w,pot);
    //assert(CHECK_MWBM_T(G,w,M,pot));
  }

  if (which == MAX_WEIGHT_MAX_CARD)
  { M = MWMCB_MATCHING_T(G,A,B,w,pot);
  }

  
  if (which == MAX_ASSIGN)
  { M = MAX_CARD_BIPARTITE_MATCHING(G,A,B);
    if ( A.size() != B.size() || A.size() != M.size() )
    { gw.message("graph has no perfect matching");
      leda_wait(1.0);
      gw.del_message();
      return;
    }
    M = MAX_WEIGHT_ASSIGNMENT_T(G,A,B,w,pot);
    //CHECK_MAX_WEIGHT_ASSIGNMENT_T(G,w,M,pot);
  }
  if (which == MIN_ASSIGN )
  { M = MAX_CARD_BIPARTITE_MATCHING(G,A,B);
    if ( A.size() != B.size() || A.size() != M.size() )
    { gw.message("graph has no perfect matching");
      leda_wait(1.0);
      gw.del_message();
      return;
    }
    M = MIN_WEIGHT_ASSIGNMENT_T(G,A,B,w,pot);
    //CHECK_MIN_WEIGHT_ASSIGNMENT_T(G,w,M,pot);
  }
  
 
  forall_edges(e,G) 
  { gw.set_color(e,black);
    gw.set_width(e,1);
    gw.set_user_label(e,string("%d",w[e]));
  }
  forall(e,M) 
  { gw.set_color(e,blue);
    gw.set_width(e,3); 
  }

  forall_nodes(v,G)  gw.set_user_label(v,string("%d",pot[v]));

  gw.redraw();
  
}

void init_handler(GraphWin& gw)
{ node v; edge e;
  forall_edges(e,G) 
  { weight[e]= rand_int(min_weight+1,max_weight-1);
    gw.set_slider_value(e,(weight[e] - min_weight)/((double) (max_weight - min_weight)));
  }
  forall_nodes(v,G)
  { col[v] = next_color; 
    next_color = - next_color;
  }
  display_mwb_matching(gw);
}

void new_node_handler(GraphWin& gw,node v)
{ col[v] = next_color; 
  next_color = -next_color;
  display_mwb_matching(gw);
}


void new_edge_handler(GraphWin& gw, edge e)  
{ node v = G.source(e);
  node w = G.target(e);

  if ( col[v] == col[w] && G.degree(v) == 1 ) col[v] = -col[v];
  if ( col[v] == col[w] && G.degree(w) == 1 ) col[w] = -col[w];

  if ( col[v] == -1) G.rev_edge(e);
  // all edges go from +1 to -1

  if ( col[v] == col[w] )
  { gw.message("Illegal Edge: destroys bipartiteness. I will delete it.");
    G.del_edge(e);
    leda_wait(1);
    display_mwb_matching(gw); 
    gw.del_message();
    return;
  }

  weight[e] = rand_int(min_weight+1,max_weight-1);
  gw.set_slider_value(e,(weight[e] - min_weight)/((double) (max_weight - min_weight)));
  display_mwb_matching(gw); 
}



void del_edge_handler(GraphWin& gw)        { display_mwb_matching(gw); }

void del_node_handler(GraphWin& gw)        { display_mwb_matching(gw); }


void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ 
  weight[e] = int(min_weight + (max_weight - min_weight) * f);

  display_mwb_matching(gw); 
}

void edge_slider_handler(GraphWin& gw, edge e, double f)
{ 
  weight[e] = int(min_weight + (max_weight - min_weight) * f);
  gw.set_user_label(e,string("%d",weight[e]));
  gw.redraw();
  //display_mwb_matching(gw); 
}

void end_move_node_handler(GraphWin& gw, node v)
{ 
  display_mwb_matching(gw); 
}



void select_geo(GraphWin& gw){ which = GEO;  display_mwb_matching(gw);  }
void select_mw(GraphWin& gw){ which = MAX_WEIGHT;  display_mwb_matching(gw);  }
void select_mwmc(GraphWin& gw){ which = MAX_WEIGHT_MAX_CARD;  display_mwb_matching(gw);  }
void select_max_ass(GraphWin& gw){ which = MAX_ASSIGN;  display_mwb_matching(gw);  }
void select_min_ass(GraphWin& gw){ which = MIN_ASSIGN;  display_mwb_matching(gw);  }





int main()
{ 
 
  GraphWin gw(G,"Maximum Weight Bipartite Matching");

  gw.add_help_text("gw_mwb_matching");

  int my_menu = gw.add_menu("Variants"); 

  gw.add_simple_call(select_mw,"maximum weight matching",my_menu);
  gw.add_simple_call(select_mwmc,"maximum weight max cardinality matching",my_menu);
  gw.add_simple_call(select_max_ass,"maximum weight assignment",my_menu);
  gw.add_simple_call(select_min_ass,"minimum weight assignment",my_menu); 
  gw.add_simple_call(select_geo,"maximum weight geo matching",my_menu);

 

  gw.display();
  gw.display_help_text("gw_mwb_matching");

  gw.set_action(A_LEFT|A_IMMEDIATE|A_EDGE,gw_slide_edge);
  gw.set_action(A_LEFT|A_DRAG|A_EDGE,gw_slide_edge);
  gw.set_action(A_LEFT|A_EDGE,0);


  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_edge_label_type(user_label);
  gw.set_node_width(20);
#ifdef BOOK
  gw.set_node_color(white);
#endif
  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);
  gw.set_end_move_node_handler(end_move_node_handler);


  gw.edit();

  return 0;
}

