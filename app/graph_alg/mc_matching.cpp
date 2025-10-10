/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mc_matching.c
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

static int cnum = 6; // number of colors

static color  colors[] = { ivory, red, yellow, orange, pink, violet, cyan};
static string cnames[] = { "ivory","red","yellow","orange","pink","violet","cyan" };



void display_mc_matching(GraphWin& gw)
{ 
  graph& G = gw.get_graph();

  node_array<int> OSC(G);
  list<edge> M = MAX_CARD_MATCHING(G,OSC);
  
  CHECK_MAX_CARD_MATCHING(G,M,OSC);

  gw.set_flush(false);

  gw.set_node_label_type(user_label);

  node v;
  forall_nodes(v,G) 
  { int i = OSC[v];
    gw.set_shape(v,circle_node);
    gw.set_color(v,colors[i%cnum]);
    gw.set_label(v,string("%d",i));
  }

  edge e;
  forall_edges(e,G) 
  { gw.set_color(e,grey3);
    gw.set_width(e,1);
  }

  forall(e,M) 
  { gw.set_color(e,blue);
    gw.set_width(e,3); 
/*
    gw.set_shape(G.source(e),rectangle_node);
    gw.set_shape(G.target(e),rectangle_node);
*/
  }

  gw.set_flush(true);
  gw.redraw();

  if (M.empty()) {
    gw.message("Empty Matching.");
    return;
  }


  int n = max(2,G.number_of_nodes());
  int K = 1;

  array<int> count(n);
  for (int i = 0; i < n; i++) count[i] = 0;

  forall_nodes(v,G) 
  { count[OSC[v]]++;
    if (OSC[v] > K) K = OSC[v];
   }


  string msg ="A maximum matching is shown in bold. ";
  msg += "The node labels give a proof of optimality."; 
  msg += "\\5 ";

  if (count[1] == 1) 
  { msg += "There is one (" + cnames[1] + ") node labeled\\bf 1\\rm and ";
    msg += "hence any matching can have at most one edge incident to a node ";
    msg += "labeled\\bf 1\\rm .\\blue We have one such edge\\black ";
    msg += ".\\5 ";
   }

  if (count[1] > 1)
  { string x("%d",count[1]);

    msg += "There are " + x + " (" + cnames[1] + ") nodes labeled\\bf 1\\rm ";
    msg += "and hence any matching can have at most " + x + " edges incident ";
    msg += "to a node labeled\\bf 1\\rm .";
    msg += "\\blue We have " + x + " such edge"; 
    if (count[1] > 1) msg += "s";
    msg += ".\\black\\10 ";
  }

  for (int i = 2; i <= K; i++) 
  { 
    string x_nodes = string("%d ",count[i]) + "(" + cnames[i%cnum] + ") node";
    if (count[i] > 1) x_nodes += "s";

    string x_edges = string("%d edge",count[i]/2);
    if (count[i]/2 > 1) x_edges += "s";

    string label_i = string("\\bf %d\\rm ", i);

    msg += "There are " +  x_nodes + " labeled " + label_i;
    msg += "and hence any matching can have at most " + x_edges + " ";
    msg += "where both endpoints are labeled " + label_i + ". ";
    msg += "\\blue We have " + x_edges + " of this kind\\black .";
    msg += "\\10 ";
  }

  if (count[1] == 0) {
    // there are no nodes labeled "1" 
    msg += "Every edge connects two (" + cnames[2] + ") ";
    msg += "nodes labeled\\bf 2\\rm ";
  }
  else
  { msg += "Every edge is incident to a (" + cnames[1] + ") ";
    msg += "node labeled \\bf 1\\rm ";
    if (K > 1) {
      msg += "or it connects two nodes labeled\\bf i\\rm for some i >= 2";
    }
  }

  msg += ".\\10 ";
  msg += "\\bf\\blue That's why the matching is optimal\\rm\\black .";

  gw.message(msg);
}


void about_mc_matching(GraphWin& gw) 
{ 
  int w = 2*gw.get_window().width()/3;

  panel P(w,-1);

  P.text_item("\\bf Maximum Cardinality Matching\\rm");
  P.text_item("");
  P.text_item("This demo illustrates the\\blue maximum cardinality matching");
  P.text_item("\\black problem.");
  P.text_item("Please edit a graph - after each edit operation the edges ");
  P.text_item("of a maximum matching are shown in bold and a proof of ");
  P.text_item("optimality is given.");
  P.text_item("");
  P.text_item("Consult the LEDA book to find out how the program works.");

  P.button("continue");

  gw.open_panel(P);
}

void new_edge_handler(GraphWin& gw, edge e){ display_mc_matching(gw); }
void del_edge_handler(GraphWin& gw)        { display_mc_matching(gw); }
void new_node_handler(GraphWin& gw,node)   { display_mc_matching(gw); }
void del_node_handler(GraphWin& gw)        { display_mc_matching(gw); }
void undo_graph_handler(GraphWin& gw)      { display_mc_matching(gw); }


int main()
{ 
  GraphWin gw("Maximum Cardinality Matching");

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);
  gw.set_undo_graph_handler(undo_graph_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);

/*
  gw.set_node_width(64);
  gw.set_node_height(64);
*/

  gw.display();

  int h_menu = gw.get_menu("Help");

  if (h_menu != -1)
    gw_add_simple_call(gw,about_mc_matching,"About Matchings",h_menu);

  about_mc_matching(gw);

  gw.message("Please edit a graph.");

  gw.edit();

  return 0;
}


