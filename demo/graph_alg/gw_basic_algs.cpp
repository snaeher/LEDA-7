/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_basic_algs.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph_alg.h>
#include <LEDA/graphwin.h>
#include <LEDA/array.h>

using namespace leda;

enum {COMP,BICOMP,TRICOMP};
int which;

string mes;
void display_basic_graph_algorithms(GraphWin& gw)
{ 
  bool flush = gw.set_flush(false);

  graph& G = gw.get_graph();
  node v; edge e;

  switch( which ){

  case COMP: { node_array<int> compnum(G);
               COMPONENTS(G,compnum);

               forall_nodes(v,G) 
               { gw.set_color(v,compnum[v]);
                 gw.set_user_label(v,string("%d",compnum[v]));
                 gw.set_label_type(v,user_label);
               }

          
               forall_edges(e,G) 
               { gw.set_color(e,black);
                 gw.set_width(e,1);
                 gw.set_label_type(e,no_label);
               }
               gw.message("Nodes in the same component have the same number.");

               break;

              }

  case BICOMP: { edge_array<int> compnum(G);
                 BICONNECTED_COMPONENTS(G,compnum);

                 forall_nodes(v,G) 
                 { gw.set_color(v,black);
                   int color = -1;
                   forall_inout_edges(e,v)
                   { if (color == -1) color = compnum[e];
                     if (color != compnum[e]) gw.set_color(v,red);
                   }
                 gw.set_label_type(v,no_label);

                 }

          
               forall_edges(e,G) 
               { gw.set_color(e,black);
                 gw.set_width(e,1);
                 gw.set_user_label(e,string("%d",compnum[e]));
                 gw.set_label_type(e,user_label);
               }
               gw.message("Edges in the same biconnected component have the same number and articulation points are shown in red. ");

               break;

              }
case TRICOMP: { node s1 = nil; node s2 = nil;
                if ( Is_Triconnected(G,s1,s2) )
                  gw.message("G is triconnected.");
                else
                  gw.message("G is not triconnected.\\5\
 The nodes shown in red from a spit pair.\\3\
 If no node is shown in red then the graph is not even connected and \\3\
 if only one node is shown in red then the graph is not biconnected and \
 the red node is an articulation point.");
 
                 forall_nodes(v,G) 
                 { gw.set_color(v,black);
                   if ( v == s1 || v == s2) gw.set_color(v,red);
                   gw.set_label_type(v,no_label);
                 }

          
               forall_edges(e,G) 
               { gw.set_color(e,black);
                 gw.set_width(e,1);
                 gw.set_label_type(e,no_label);
               }

               break;

              }
  }

  gw.set_flush(flush);
  gw.redraw();
}


void about_basic_graph_algorithms(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(2*W.width()/3,-1);
 
  //P.set_panel_bg_color(win_p->mono() ? white : ivory);

P.text_item("This demo illustrates some basic graph algorithms.");
P.text_item("");
P.text_item("Edit a graph");
P.text_item("");
P.text_item("and select a graph property. ");
P.text_item("");
P.text_item("The graph property is recomputed after every edit operation.");
P.text_item("");

  P.button("OK");
  W.disable_panel();
  P.open(W);
  W.enable_panel();
}


void select_comp(GraphWin& gw)
{ which = COMP; display_basic_graph_algorithms(gw); }

void select_bicomp(GraphWin& gw)
{ which = BICOMP; display_basic_graph_algorithms(gw); }

void select_tricomp(GraphWin& gw)
{ which = TRICOMP; display_basic_graph_algorithms(gw) ;}



void new_edge_handler(GraphWin& gw, edge e)
{ display_basic_graph_algorithms(gw); }

void del_edge_handler(GraphWin& gw)        
{ display_basic_graph_algorithms(gw); }

void new_node_handler(GraphWin& gw,node)   
{ display_basic_graph_algorithms(gw); }

void del_node_handler(GraphWin& gw)        
{ display_basic_graph_algorithms(gw); }


int main()
{ 
  GraphWin gw(580,620,"Basic Graph Algorithms");


  gw.add_simple_call(select_comp,"connected components");
  gw.add_simple_call(select_bicomp,"biconnected components");
  gw.add_simple_call(select_tricomp,"triconnected components");

  gw.display();
  about_basic_graph_algorithms(gw);


  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_basic_graph_algorithms, "About Basic Graph Algorithms",h_menu);

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);
  gw.set_node_shape(circle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_width(20);
  gw.set_node_height(20);

#ifdef BOOK
  gw.set_node_color(white);
#endif

  gw.edit();

  return 0;
}


