/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_handler.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/b_stack.h>

using namespace leda;

static void scc_dfs(const graph& G, node v, node_array<int>& compnum,
                                            b_stack<node>& unfinished,
                                            b_stack<int>& roots,
                                            int& count1, int& count2 )
{ int cv = --count1;
  compnum[v] = cv;
  unfinished.push(v);
  roots.push(cv);

  edge e;
  forall_out_edges(e,v)
  { node w =G.target(e);
    int cw = compnum[w];
    if (cw == -1)
      scc_dfs(G,w,compnum,unfinished,roots,count1,count2);
    else
      if (cw < -1)
         while (roots.top() < cw)  roots.pop();
   }

  if (roots.top() == cv)
  { node u;
    do { u = unfinished.pop(); // u is a node of the scc with root v
         compnum[u] = count2;
        } while (v != u);
    roots.pop();
    count2++;
   }
}


int STRONG_COMPONENTS(const graph& G, node_array<int>& compnum)
{
  int n = G.number_of_nodes();

  b_stack<int>  roots(n);
  b_stack<node> unfinished(n);

  int count1 = -1;
  int count2 = 0;

  node v;
  forall_nodes(v,G) compnum[v] = -1;

  forall_nodes(v,G)
    if (compnum[v] == -1)
       scc_dfs(G,v,compnum,unfinished,roots,count1,count2);

  return count2;
}



void run_and_display(GraphWin& gw)
{ 
  graph& G = gw.get_graph();
  node_array<int> comp_num(G);
  STRONG_COMPONENTS(G,comp_num);
  node v;
  forall_nodes(v,G) 
    gw.set_color(v,color(comp_num[v]));
 }

void new_node_handler(GraphWin& gw, node){ run_and_display(gw); }
void new_edge_handler(GraphWin& gw, edge){ run_and_display(gw); }
void del_edge_handler(GraphWin& gw)      { run_and_display(gw); }
void del_node_handler(GraphWin& gw)      { run_and_display(gw); }
void init_graph_handler(GraphWin& gw)    { run_and_display(gw); }


int main()
{
  GraphWin gw;

  gw.set_init_graph_handler(init_graph_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.display();
  gw.edit();

  return 0;
}

