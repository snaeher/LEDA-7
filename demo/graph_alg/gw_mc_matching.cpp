/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_mc_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/mc_matching.h>
#include <LEDA/core/array.h>

using namespace leda;

static color colors[] = { yellow, red, blue, green, pink };


string mes;
void display_general_matching(GraphWin& gw)
{ graph& G = gw.get_graph();
  node v; edge e;

  list<edge> M;
  node_array<int> OSC(G);
  M = MAX_CARD_MATCHING(G,OSC);
  
  CHECK_MAX_CARD_MATCHING(G,M,OSC);

  gw.set_node_label_type(user_label);


  forall_nodes(v,G) 
  { //gw.set_color(v,OSC[v]);
    int i = OSC[v];
    gw.set_color(v,colors[i]);
    gw.set_user_label(v,string("%d",i));
  }

          
  forall_edges(e,G) 
  { gw.set_color(e,black);
    gw.set_width(e,1);
  }
  forall(e,M) 
  { gw.set_color(e,blue);
    gw.set_width(e,3); 
  }
  

  int n = leda_max(2,G.number_of_nodes());
  int K = 1;
  array<int> count(n);
  int i;
  for (i = 0; i < n; i++) count[i] = 0;

  forall_nodes(v,G) 
  { count[OSC[v]]++;
    if (OSC[v] > K) K = OSC[v];
  }

string mes ="An optimal matching is shown in bold.\\5 \
The node labels form a proof of optimality.\\3 "; 
if (count[1] > 0) 
mes += "There are " +  string("%d",count[1]) + " nodes labeled one and hence any matching can have at most " +  string("%d",count[1]) + " edges incident to a node labeled one. \\blue We have " + string("%d",count[1]) + ".\\3 \\black ";
for (i = 2; i <= K; i++)
mes += "There are " +  string("%d",count[i]) + " nodes labeled " + string("%d",i) + " and hence any matching can have at most " +  string("%d",count[i]/2) + " edges where both endpoints are labeled " + string("%d",i) + ".\\blue We have " + string("%d",count[i]/2) + ".\\3 \\black ";

mes += "Every edge is incident to a node labeled one";
if (K > 1) mes += " or connects two nodes labeled i for some i greater or equal to two";
mes += ". Thus the matching is \\blue optimal.";



  gw.message(mes);

}

void about_max_cardinality_matching(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P;
 
  //P.set_panel_bg_color(win_p->mono() ? white : ivory);

P.text_item("This demo illustrates the maximum cardinality matching problem.");
P.text_item("");P.text_item("");
P.text_item("");
P.text_item("Please edit a graph.");
P.text_item("");
P.text_item("");
P.text_item("");
P.text_item("After every edit operation the edges of a maximum matching are shown in bold");
P.text_item("and a proof of optimality is given.");
P.text_item("");
P.text_item("");
P.text_item("");

P.text_item("");
P.text_item("Consult the LEDA book to find out how the program works.");
P.text_item("");




  P.button("OK");
  W.disable_panel();
  P.open(W);
  W.enable_panel();
}

void new_edge_handler(GraphWin& gw, edge e){ display_general_matching(gw); }
void del_edge_handler(GraphWin& gw)        { display_general_matching(gw); }
void new_node_handler(GraphWin& gw,node)   { display_general_matching(gw); }
void del_node_handler(GraphWin& gw)        { display_general_matching(gw); }


int main()
{ GraphWin gw("Maximum Cardinality Matching");

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_max_cardinality_matching, "About Matchings",h_menu);

  about_max_cardinality_matching(gw);

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
/*
  gw.set_node_width(20);
*/


  gw.edit();

  return 0;
}


