#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/stack.h>

using namespace leda;

void scc_dfs(const graph& G, node v, node_array<int>& compnum,
                                     node_array<int>& dfsnum,
                                     stack<node>& unfinished,
                                     stack<node>& roots,
                                     node_array<bool>& in_unfinished,
                                     int& count1, int& count2 )

{
  node w;

  dfsnum[v] = ++count1;
  unfinished.push(v);
  in_unfinished[v] = true;
  roots.push(v);

  edge e;
  forall_out_edges(e,v)
  { w = G.target(e);

    if (dfsnum[w]==-1) 
       scc_dfs(G,w,compnum,dfsnum,unfinished,roots,in_unfinished,count1,count2);
      else 
       if (in_unfinished[w])
        while (dfsnum[roots.top()] > dfsnum[w])  roots.pop();
   }

  if (v == roots.top()) 
   { do { w=unfinished.pop();
          in_unfinished[w] = false;
          /* w is an element of the scc with root v */
          compnum[w] = count2;
         } while (v!=w);
     roots.pop(); 
     count2++;
    }
}


int STRONG_COMPONENTS(const graph& G, node_array<int>& compnum)
{
  // int STRONG_COMPONENTS(graph& G, node_array<int>& compnum)
  // computes strong connected components (scc) of digraph G
  // returns m = number of scc 
  // returns in node_array<int> compnum for each node an integer with
  // compnum[v] = compnum[w] iff v and w belong to the same scc
  // 0 <= compnum[v] <= m-1 for all nodes v

  stack<node>   roots;
  stack<node>   unfinished;
  node_array<int> dfsnum(G,-1);
  node_array<bool> in_unfinished(G,false);

  int count1 = 0; 
  int count2 = 0;

  node v;

  forall_nodes(v,G) 
      if (dfsnum[v] == -1) 
       scc_dfs(G,v,compnum,dfsnum,unfinished,roots,in_unfinished,count1,count2);

  return count2;
}

void display_scc(GraphWin& gw)
{ graph& G = gw.get_graph();
  node_array<int> comp_num(G);
  STRONG_COMPONENTS(G,comp_num);
  node v;
  gw.set_node_label_type(user_label);
  forall_nodes(v,G) 
  { gw.set_color(v,comp_num[v]%16);
    gw.set_user_label(v,string("%d",comp_num[v]));
  }
}


void new_edge_handler(GraphWin& gw, edge)  { display_scc(gw); }
void del_edge_handler(GraphWin& gw)        { display_scc(gw); }
void new_node_handler(GraphWin& gw, node)  { display_scc(gw); }
void del_node_handler(GraphWin& gw)        { display_scc(gw); }


int main()
{
  GraphWin gw("Strongly Connected Components");

  gw.set_init_graph_handler(del_edge_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(true);

  gw.display();

  gw.edit();

  return 0;
}

