/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_max_flow.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/max_flow.h>
#include <LEDA/core/queue.h>
#include <assert.h>

using namespace leda;


static GRAPH<int,int> G;

static node src = 0;
static node snk = 0;

static int min_cap = 0;
static int max_cap = 12;

void display_max_flow(GraphWin& gw)
{ 
  if (G.number_of_nodes() < 2) return;

  gw.set_flush(false);

  if (src == 0) src = G.first_node();
  if (snk == 0) snk = G.succ_node(G.first_node());

  node s = src;
  node t = snk;

  edge_array<int> f(G,0);

  MAX_FLOW(G,s,t,G.edge_data(),f);
  
  //assert(CHECK_MAX_FLOW(G,s,t,G.edge_data(),f));

  gw.set_width(s,24); 
  gw.set_shape(s,square_node);
  gw.set_label(s,"S");

  gw.set_width(t,24);
  gw.set_color(t,blue2);
  gw.set_shape(t,square_node);
  gw.set_label(t,"T");

  node_array<bool> reached(G,false);
  queue<node> Q;
  
  Q.append(s); reached[s] = true;
  while ( !Q.empty() )
  { node v = Q.pop(); 
    edge e;
    forall_out_edges(e,v) 
    { node w = G.target(e);
      if ( f[e] < G[e] && !reached[w] ) 
      { reached[w] = true; Q.append(w); }
    }
    forall_in_edges(e,v) 
    { node w = G.source(e);
      if ( f[e] > 0 && !reached[w] ) 
      { reached[w] = true; Q.append(w); }
    }
  }
  

  node v;
  forall_nodes(v,G)
  { if (v == t) continue;
    gw.set_color(v,(reached[v]? red : grey2));
   }


  edge e;
  forall_edges(e,G)
  { gw.set_color(e,black);
    if (reached[G.source(e)] != reached[G.target(e)]) gw.set_color(e,green2);
    gw.set_width(e,f[e] == 0 ? 1 : f[e]);
    gw.set_style(e,f[e] > 0 ? solid_edge : dashed_edge);
    gw.set_user_label(e,string("%d",f[e]) + "/" + string("%d",G[e]));
  }
  

  gw.redraw();
  
}

void init_handler(GraphWin& gw)
{ edge e;
  forall_edges(e,G) 
  { G[e] = rand_int(min_cap+3,max_cap-3);
    gw.set_slider_value(e,(G[e] - min_cap)/((double) (max_cap - min_cap)));
  }
  src = G.first_node();
  snk = G.last_node();
  display_max_flow(gw);
}

void new_edge_handler(GraphWin& gw, edge e)  
{ G[e] = rand_int(min_cap+3,max_cap-3);
  gw.set_slider_value(e,(G[e] - min_cap)/((double) (max_cap - min_cap)));
  display_max_flow(gw); 
}



void new_node_handler(GraphWin& gw ,node)   { display_max_flow(gw); }
void del_node_handler(GraphWin& gw)         { display_max_flow(gw); }

bool pre_del_node_handler(GraphWin& gw, node v)
{ if (v == src) src = 0;
  if (v == snk) snk = 0;
  return true;
 }

void del_edge_handler(GraphWin& gw)        { display_max_flow(gw); }


void edge_slider_handler(GraphWin& gw, edge e, double f)
{ G[e] = int(min_cap + (max_cap - min_cap) * f);
  display_max_flow(gw); 
}

void end_edge_slider_handler(GraphWin& gw, edge e, double f)
{ display_max_flow(gw); }




int main()
{ 
 
  GraphWin gw(G,"Maximum Flow");

  gw.add_help_text("gw_max_flow");
  gw.display();

  gw.display_help_text("gw_max_flow");

  gw.set_action(A_LEFT|A_IMMEDIATE|A_EDGE,0);
  gw.set_action(A_LEFT|A_DRAG|A_EDGE,0); // prevent adding edge bends
  gw.set_action(A_LEFT|A_EDGE,0);


  //gw.set_init_graph_handler(del_edge_handler);
  gw.set_init_graph_handler(init_handler);

  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);

  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(pre_del_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(true);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_edge_label_type(user_label);
  gw.set_node_width(12);
#ifdef BOOK
  gw.set_node_color(white);
#endif
  gw.set_edge_slider_handler(edge_slider_handler);
  gw.set_end_edge_slider_handler(end_edge_slider_handler);


  gw.edit();

  return 0;
}

