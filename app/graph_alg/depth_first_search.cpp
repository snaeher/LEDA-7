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

string msg = " \
Nodes are labeled with\\blue\\bf dfs\\black\\rm and \
\\blue\\bf completion\\black\\rm numbers.\\n \
Edges are drawn with different colors.\\10 \
\\black\\bf  --- Tree-Edges\\rm\\black\\n \
\\blue2\\bf  --- Forward-Edges\\rm\\black\\n \
\\red\\bf    --- Backward-Edges\\rm\\black\\n  \
\\green2\\bf --- Cross-Edges\\rm\\black\\n ";

void about_dfs(GraphWin& gw) 
{ window& W = gw.get_window();
 
  panel P(3*W.width()/4,-1);
 
  P.text_item("\\bf Depth First Search\\rm\\10");
  P.text_item("This demo illustrates depth-first search on a directed graph. ");
  P.text_item("Please enter a graph - after every update operation ");
  P.text_item("the result of DFS is shown.\\15 ");
  P.text_item(msg);

  P.button("continue");

  gw.open_panel(P);
}



void display_dfs(GraphWin& gw)
{
  gw.message(msg);

  graph& G = gw.get_graph();
  node_array<int> dfsnum(G);
  node_array<int> compnum(G);

  list<edge> T = DFS_NUM(G,dfsnum,compnum);

  node v;
  forall_nodes(v,G) 
  { string a = string("%d",dfsnum[v]);
    string b = string("%d",compnum[v]);
    gw.set_user_label(v,a + " | " + b);
  }

  edge e;
  forall_edges(e,G)
  { gw.set_width(e,2);
    node v = G.source(e);
    node w = G.target(e);
    if ( dfsnum[v] < dfsnum[w] && compnum[v] > compnum[w] )
      { // tree edge or forward edge 
        //gw.set_style(e,solid_edge); 
        gw.set_color(e,blue2);
      }
    if ( dfsnum[v] >= dfsnum[w] && compnum[v] <= compnum[w] )
      { // backward edge 
        //gw.set_style(e,dashed_edge); 
        gw.set_color(e,red);
      }
    if ( dfsnum[v] > dfsnum[w] && compnum[v] > compnum[w] )
      { // cross edge 
        //gw.set_style(e,dotted_edge); 
        gw.set_color(e,green2);
      }
  
  }

  // tree edges
  forall(e,T) {
    gw.set_color(e,black);
    gw.set_width(e,3);
  }

  gw.redraw();
}

void new_edge_handler(GraphWin& gw, edge)  { display_dfs(gw); }
void del_edge_handler(GraphWin& gw)        { display_dfs(gw); }
void new_node_handler(GraphWin& gw, node)  { display_dfs(gw); }
void del_node_handler(GraphWin& gw)        { display_dfs(gw); }

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

  cout << "h_menu = " << h_menu << endl;

  if (h_menu != -1)
    gw_add_simple_call(gw,about_dfs, "About DFS",h_menu);

  about_dfs(gw);

  gw.set_directed(true);

  gw.set_node_shape(rectangle_node);
  gw.set_node_label_type(user_label);
  gw.set_node_label_color(blue);

  gw.set_node_width(124);
  gw.set_node_height(62);


  gw.message("Please edit a graph.");

  gw.edit();

  return 0;
}

