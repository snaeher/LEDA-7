/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_min_ratio_cycle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/map.h>
#include <LEDA/internal/std/iostream.h>
#include <stdio.h>

using namespace leda;

map<edge,int> c(0);
map<edge,int> p(0);

int min_profit,max_profit;
string mes;

void display_minimum_ratio_cycle(GraphWin& gw)
{ graph& G = gw.get_graph();
  edge_array<int> profit(G);
  edge_array<int> cost(G);

  edge e;
  forall_edges(e,G) 
    { if ( p[e] < min_profit) 
      { p[e] = rand_int(min_profit,max_profit); c[e] = p[e] - 1; }
      profit[e] = p[e]; cost[e] = c[e];
    }

  list<edge> C_opt; rational lambda_star;

  node_array<int> comp_num(G);
  if ( STRONG_COMPONENTS(G,comp_num) == G.number_of_nodes() )
   mes = "There a no cycles yet\\5 ";
  else
  { mes = "Optimal cycle is shown in bold\\5 ";
    lambda_star = MINIMUM_RATIO_CYCLE(G,profit,cost,C_opt);
  }
  
 
  forall_edges(e,G) 
  { gw.set_width(e,1);
    gw.set_user_label(e,string("%d",cost[e]) + "/" + string("%d",profit[e]));
  }
  mes += "Labels are cost/profit\n";
  int Psum = 0; int Csum = 0;
  forall(e,C_opt) 
    { gw.set_width(e,3); Psum += profit[e]; Csum += cost[e]; }
  mes += "\\5 total cost   = "+ string("%d",Csum);
  mes += "\\3 total profit = "+ string("%d",Psum); 

  gw.message(mes);

}

void new_edge_handler(GraphWin& gw, edge e)  
{ p[e] = rand_int(min_profit,max_profit);
  c[e] = p[e] - 1;
  display_minimum_ratio_cycle(gw); 
}
void del_edge_handler(GraphWin& gw)        { display_minimum_ratio_cycle(gw); }
void new_node_handler(GraphWin& gw,node)   { display_minimum_ratio_cycle(gw); }
void del_node_handler(GraphWin& gw)        { display_minimum_ratio_cycle(gw); }

void about_minimum_ratio_cycle(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(2*W.width()/3,-1);
 
  //P.set_panel_bg_color(win_p->mono() ? white : ivory);

P.text_item("This demo illustrates the minimum ratio cycle problem");
P.text_item("");
P.text_item("You will be asked to input two integers min and max.");
P.text_item("Edge profits will be random integers in the range [min,max].");
P.text_item("");
P.text_item("You may then edit a graph in the graphics window.");
P.text_item("Edge costs are one less than profits.");
P.text_item("There should be no cycle of cost zero or less with");
P.text_item("respect to either cost or profit. We therefore require min > 1");
P.text_item("");
P.text_item("For a cycle C, lambda(C) is defined as the ratio of the");
P.text_item("total cost of the cycle and the total profit of the cycle.");
P.text_item("");
P.text_item("The program computes the cycle that minimizes this ratio.");
P.text_item("");
P.text_item("See the LEDA book to find out how the program works");
P.text_item("");

  P.text_item("");


  P.button("OK");
  W.disable_panel();
  P.open(W);
  W.enable_panel();
}

int main()
{ 
  GraphWin gw("Minimum Ratio Cycles");

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw.add_simple_call(about_minimum_ratio_cycle, "About Minimum Ratio Cycles",h_menu);

  about_minimum_ratio_cycle(gw);
  window& W = gw.get_window();
  min_profit = leda_max(2,W.read_int("minimum cost of an edge (at least two) = "));
  max_profit = leda_max(min_profit,W.read_int("maximum cost of an edge (at least minimum) = "));

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(true);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_width(20);
#ifdef BOOK
  gw.set_node_color(white);
#endif

  gw.edit();

  return 0;
}

