/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_menu.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;

static void dfs(node v, node_array<int>& dfsnum, node_array<int>& compnum,
                        int& count1, int& count2)
{ dfsnum[v] = ++count1;
  edge e;
  forall_adj_edges(e,v) 
  { node w = target(e);
    if (dfsnum[w] == 0) dfs(w,dfsnum,compnum,count1, count2);
   }
  compnum[v] = ++count2;
} 

void dfs_numbering(const graph& G, node_array<int>& dfsnum, 
                            node_array<int>& compnum)
{ int count1 = 0;
  int count2 = 0;
  dfsnum.init(G,0);
  node v;
  forall_nodes(v,G) 
    if (dfsnum[v] == 0) 
      dfs(v,dfsnum,compnum,count1, count2);
}





// a caller for component algorithms

void call_comp(GraphWin& gw, 
               int (*comp)(const graph& G, 
                           node_array<int>& compnum) )
{ graph& G = gw.get_graph();
  node_array<int> compnum(G);
  comp(G,compnum);
  node v;
  forall_nodes(v,G) 
  { int i = compnum[v];
    gw.set_label(v,string("%d",i));
    gw.set_color(v,(color)(i%16));
   }
  if (gw.get_flush() == false) gw.redraw();
}


void call_comp1(GraphWin& gw, 
               int (*comp)(GraphWin& gw, 
                           node_array<int>& compnum) )
{ graph& G = gw.get_graph();
  node_array<int> compnum(G);
  comp(gw,compnum);
  node v;
  forall_nodes(v,G) 
  { int i = compnum[v];
    gw.set_label(v,string("%d",i));
    gw.set_color(v,(color)(i%16));
   }
  if (gw.get_flush() == false) gw.redraw();
}




static void dfs_num(GraphWin& gw) 
{ graph& G = gw.get_graph();
  node_array<int> dfsnum(G);
  node_array<int> compnum(G);

  dfs_numbering(G,dfsnum,compnum);

  node v;
  forall_nodes(v,G) 
      gw.set_label(v,string("%d|%d",dfsnum[v],compnum[v]));
  
  if (gw.get_flush() == false) gw.redraw();
}


void span_tree(GraphWin& gw) 
{ //graph& G = gw.get_graph();
  //list<edge> L = SPANNING_TREE(G);
  list<edge> L = gw_spanning_tree(gw);
  gw.set_color(L,red);
  gw.set_width(L,2);
  if (gw.get_flush() == false) gw.redraw();
}


int main() 
{
  GraphWin gw;

  // we delete some of the standard menus
  gw.set_default_menu(M_COMPLETE 
                      & ~M_LAYOUT &  ~M_SETTINGS & ~M_HELP & ~M_WINDOW);

  // add two simple function calls
  gw.add_simple_call(dfs_num,   "dfsnum");
  gw.add_simple_call(span_tree, "spanning");

  // a member call
  gw.add_member_call(&GraphWin::reset,"reset");

  // and a menu with three non-simple functions using 
  // a common call function

  int menu1 = gw.add_menu("components");

  gw_add_call(gw,::gw_components,       call_comp1, 
                                   "simply connected",  menu1);
  gw_add_call(gw,::gw_strong_components,call_comp1, 
                                   "strongly connected",menu1);

  gw_add_call(gw,COMPONENTS,       call_comp, 
                                   "simply connected1",  menu1);
  gw_add_call(gw,STRONG_COMPONENTS,call_comp, 
                                   "strongly connected1",menu1);

  gw.display();
  gw.edit();

  return 0;
}
