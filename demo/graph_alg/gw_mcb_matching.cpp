/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_mcb_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/ugraph.h>

using namespace leda;


enum {CARD,WEIGHT};
int which;

void select_card(){ which = CARD; }
void select_weight(){ which = WEIGHT; }

void gw_call(GraphWin& gw, void (*select)())
{ select(); }


void display_mcb_matching(GraphWin& gw)
{ graph& G = gw.get_graph();

  edge e; node v;
  list<edge> M;
  node_array<bool> NC(G);
  M = MAX_CARD_BIPARTITE_MATCHING(G,NC);
  
  CHECK_MCB(G,M,NC);

    forall_nodes(v,G) 
     if ( NC[v]) gw.set_color(v,blue);
     else gw.set_color(v,white);
    
      
    forall_edges(e,G) 
    { gw.set_color(e,black);
      gw.set_width(e,1);
    }
    forall(e,M) 
    { gw.set_color(e,blue);
      gw.set_width(e,3); 
    }
  
}

void new_edge_handler(GraphWin& gw, edge e)  
{ //c[e] = rand_int(min,max);
  display_mcb_matching(gw); 
}
void del_edge_handler(GraphWin& gw)        { display_mcb_matching(gw); }
void new_node_handler(GraphWin& gw,node)   { display_mcb_matching(gw); }
void del_node_handler(GraphWin& gw)        { display_mcb_matching(gw); }


int main()
{ 
 
  GraphWin gw("Bipartite Cardinality Matching");;
  gw.display();

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_width(20);

  
  gw.edit();

  return 0;
}

