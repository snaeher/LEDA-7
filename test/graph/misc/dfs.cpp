#include <LEDA/graph/graph.h>
#include <LEDA/core/b_stack.h>

using namespace leda;
using std::cout;
using std::endl;


void DFS(const graph& G, node_array<int>& dfsnum,
                         node_array<int>& compnum)
                                 
{ 
  int n = G.number_of_nodes();
  b_stack<node> S(n);

  node_array<edge> current(G);

  int dfs_count = 0;
  int comp_count = 0;

  node u;
  forall_nodes(u,G) 
  { dfsnum[u] = 0;
    compnum[u] = 0;
    current[u] = G.first_out_edge(u);
   }

  forall_nodes(u,G)
  { 
    if (dfsnum[u] != 0) continue;  // visited before

    S.push(u);
    dfsnum[u] = ++dfs_count;

    while (!S.empty())
    { node v = S.top();
      edge e = current[v];

      if (e == 0)         
      { // all edges scanned: finish v
        compnum[v] = ++comp_count;
        S.pop();
        continue;
       }

      current[v] = G.next_out_edge(e);

      node w  = G.target(e);

      if (dfsnum[w] == 0) // tree edge
      { S.push(w);
        dfsnum[w] = ++dfs_count;
       }
//    else
//      if (compnum[w] > 0) 
//        { // w completed
//          if (dfsnum[w] < dfsnum[v])
//            // forward edge
//          else
//            // cross edge
//         }
//      else
//         // w not completed: back edge

    } // while stack not empty 

  } // forall_nodes

}




int main() {

  graph G;

  test_graph(G);

  node_array<int> dfsnum(G);
  node_array<int> compnum(G);

  DFS(G,dfsnum,compnum);

  return 0;
}

