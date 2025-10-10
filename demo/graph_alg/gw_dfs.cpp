/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_dfs.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graphics/graphwin.h>


using namespace leda;


void display_scc(GraphWin& gw)
{ graph& G = gw.get_graph();
  node_array<int> dfsnum(G);
  node_array<int> compnum(G);
  list<edge> T = DFS_NUM(G,dfsnum,compnum);
  node v;

  forall_nodes(v,G) 
  { string a = string("%d",dfsnum[v]);
    string b = string("%d",compnum[v]);
    gw.set_user_label(v,a + " : " + b);
  }

  edge e;
  forall_edges(e,G)
  { gw.set_width(e,2);
    node v = G.source(e);
    node w = G.target(e);
    if ( dfsnum[v] < dfsnum[w] && compnum[v] > compnum[w] )
      { // tree edge or forward edge 
        //gw.set_style(e,solid_edge); 
        gw.set_color(e,orange);
      }
    if ( dfsnum[v] >= dfsnum[w] && compnum[v] <= compnum[w] )
      { // backward edge 
        //gw.set_style(e,dashed_edge); 
        gw.set_color(e,blue2);
      }
    if ( dfsnum[v] > dfsnum[w] && compnum[v] > compnum[w] )
      { // cross 
        //gw.set_style(e,dotted_edge); 
        gw.set_color(e,cyan);
      }
  
  }
  forall(e,T) gw.set_color(e,black);
  gw.redraw();
}

void new_edge_handler(GraphWin& gw, edge)  { display_scc(gw); }
void del_edge_handler(GraphWin& gw)        { display_scc(gw); }
void new_node_handler(GraphWin& gw, node)  { display_scc(gw); }
void del_node_handler(GraphWin& gw)        { display_scc(gw); }

void about_dfs(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(2*W.width()/3,-1);
 
  //P.set_panel_bg_color(win_p->mono() ? white : ivory);
 

  P.text_item("");
  P.text_item("DFS illustrates depth-first search. \
               Please enter a directed graph G. \
               After every update of the graph the result of \
               a dfs is shown.");
  P.text_item("");
  P.text_item("Within each nodes the dfs-number and the \
               completion number are shown.");
  P.text_item("");
  P.text_item("Tree edges are shown as heavy solid edges.");
  P.text_item("");
  P.text_item("Forward edges are shown as thin solid edges.");
  P.text_item("");
  P.text_item("Cross edges are shown as dotted edges.");
  P.text_item("");
  P.text_item("Backward edges are shown as dashed edges.");
  P.text_item("");

  P.button("OK");

  W.disable_panel();
  P.open(W);
  W.enable_panel();
}


int main()
{
  GraphWin gw("Depth-First Search");

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.display();

  int h_menu = gw.get_menu("Help");
  gw_add_simple_call(gw,about_dfs, "About DFS",h_menu);

  about_dfs(gw);


  gw.set_directed(true);
  gw.set_node_shape(rectangle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_width(60);
#ifdef BOOK
  gw.set_node_color(white);
#endif

  
  gw.edit();

  return 0;
}

