#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
/*
#include <LEDA/graph/node_pq22.h>
*/
#include <LEDA/graph/node_pq2k.h>
#include <limits>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


typedef int NT;

bool AP_BELLMAN_FORD(const graph& G, const edge_array<NT>& cost,
                                     node_array<NT>& dist,
                                     node_array<int>& count)
{ 
  int n = G.number_of_nodes();

  node_list Q;

  node v;
  forall_nodes(v,G) 
  { Q.append(v);
    count[v] = 0;
   }

  while(! Q.empty() )
  { node u = Q.pop();

    if (++count[u] > n) return false;   // negative cycle

    NT du = dist[u];

    edge e;
    forall_adj_edges(e,u) 
    { v = G.opposite(u,e);              // makes it work for undirected graph
      NT c = du + cost[e];
      if (c < dist[v]) 
      { dist[v] = c; 
        if (!Q.member(v)) Q.append(v);
       }
     } 
   }
  return true;
}



template <class NT, class graph_t   = graph,
                    class node_pq_t = node_pq22<NT,graph_t,4> >
class dijkstra1
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  
  float T;

  node opposite(const graph_t& G, edge e, node s)
  { if (graph::category() == bidirectional_graph_category || 
        graph::category() == directed_graph_category)
      return G.target(e);
    else
      return G.opposite(e,s);
  }
  
  public:
        
  template <class cost_array, class dist_array>
  void run(const graph_t& G, node s, const cost_array& cost, dist_array& dist)
  {
    T = used_time();

    int n = G.number_of_nodes();
    int m = G.number_of_edges();

    node_pq_t PQ(n+m); 

    NT max_dist = std::numeric_limits<NT>::max();

    node v;
    forall_nodes(v,G) dist[v] = max_dist;

    dist[s] = 0;
    PQ.insert(s,0);

    while (!PQ.empty())
    { NT du;
      node u = PQ.del_min(du,dist);  

      if (du != dist[u]) continue;
  
      edge e;
      forall_adj_edges(e,u)
      { node v = opposite(G,e,u);
        NT c = du + cost[e]; 
        if (c < dist[v]) 
        { PQ.insert(v,c); 
          dist[v] = c; 
         }
      }                                                                  
    }
    
    T = used_time(T);
  }  

};



bool AP_SHORTEST_PATHS(graph& G, const edge_array<NT>& cost, 
                                       node_matrix<NT>& DIST)
{ 
  edge e;
  node v,w;

  NT C = 0;
  forall_edges(e,G)
  { NT ce = cost[e];
    C += (ce > 0) ? ce : -ce;
   }

  node_array<NT> dist1(G);
  edge_array<NT> cost1(G);
  node_array<edge> pred(G);
  node_array<int> count(G);

  forall_nodes(v,G) dist1[v] = C;

  if (!AP_BELLMAN_FORD(G,cost,dist1,count)) return false;

  forall_edges(e,G) cost1[e] = dist1[source(e)] + cost[e] - dist1[target(e)];

  forall_nodes(v,G) DIJKSTRA(G,v,cost1,DIST[v],pred);

  forall_nodes(v,G)
  { NT dv = dist1[v];
    forall_nodes(w,G) DIST(v,w) += (dist1[w] - dv);
   }

 return true;
}

/*
bool AP_SHORTEST_PATHS1(graph& G, const edge_array<NT>& cost, 
                                       node_matrix<NT>& DIST)
{ 
  edge e;
  node v,w;

  NT C = 0;
  forall_edges(e,G)
  { NT ce = cost[e];
    C += (ce > 0) ? ce : -ce;
   }

  node_array<NT> dist1(G);
  edge_array<NT> cost1(G);
  node_array<int> count(G);

  forall_nodes(v,G) dist1[v] = C;

  if (!AP_BELLMAN_FORD(G,cost,dist1,count)) return false;

  forall_edges(e,G) cost1[e] = dist1[source(e)] + cost[e] - dist1[target(e)];

  forall_nodes(v,G) DIJKSTRA1(G,v,cost1,DIST[v]);

  forall_nodes(v,G)
  { NT dv = dist1[v];
    forall_nodes(w,G) DIST(v,w) += (dist1[w] - dv);
   }

 return true;
}
*/


bool AP_SHORTEST_PATHS1(graph& G, const edge_array<NT>& cost, 
                                       node_matrix<NT>& DIST)
{ 
  NT C = 0;
  int n_edges = 0;

  edge e;
  forall_edges(e,G)
  { NT ce = cost[e];
    if (ce < 0) { n_edges++; ce = -ce; }
    C += ce;
   }

  dijkstra1<int> D;

/*
  if (n_edges == 0)
  { node v;
    forall_nodes(v,G) D.run(G,v,cost,DIST[v]);
    return true;
   }
*/

  node_array<NT> dist1(G);
  edge_array<NT> cost1(G);
  node_array<int> count(G);

  node v;
  forall_nodes(v,G) dist1[v] = C;
  if (!AP_BELLMAN_FORD(G,cost,dist1,count)) return false;

  forall_edges(e,G) 
  { NT d = dist1[source(e)] + cost[e] - dist1[target(e)];
    cost1[e] = d;
   }

  forall_nodes(v,G) D.run(G,v,cost1,DIST[v]);

  forall_nodes(v,G)
  { NT dv = dist1[v];
    node w;
    forall_nodes(w,G) DIST(v,w) += (dist1[w] - dv);
   }

 return true;
}


bool AP_SHORTEST_PATHS2(graph& G, const edge_array<NT>& cost, 
                                       node_matrix<NT>& DIST)
{ 
  dijkstra1<int> D;

  node_array<NT> dist1(G);
  edge_array<NT> cost1(G);
  node_array<int> count(G);

  node v;
  forall_nodes(v,G) dist1[v] = 0;
  if (!AP_BELLMAN_FORD(G,cost,dist1,count)) return false;

  edge e;
  forall_edges(e,G) 
  { NT d = dist1[source(e)] + cost[e] - dist1[target(e)];
    cost1[e] = d;
   }

  forall_nodes(v,G) D.run(G,v,cost1,DIST[v]);

  forall_nodes(v,G)
  { NT dv = dist1[v];
    node w;
    forall_nodes(w,G) DIST(v,w) += (dist1[w] - dv);
   }

 return true;
}




int main()
{

GRAPH<int,int> G;
node v,w;
edge e;

int n = read_int("# nodes = ");
int m = read_int("# edges = ");

random_graph(G,n,m);


edge_array<int>  cost(G);
node_matrix<int> DIST1(G);
node_matrix<int> DIST2(G);

int a = read_int("a = ");
int b = read_int("b = ");

forall_edges(e,G) cost[e] = rand_int(a,b);

float T = used_time();

  cout << "ALL PAIRS SHORTEST PATHS <int> ";
  cout.flush();
  if (!ALL_PAIRS_SHORTEST_PATHS(G,cost,DIST1)) cout << "negative cycle" << endl;
  cout << string("%5.2f sec\n",used_time(T));

  cout << "AP SHORTEST PATHS <int>        ";
  cout.flush();
  if (!AP_SHORTEST_PATHS(G,cost,DIST2)) cout << "negative cycle" << endl;
  cout << string("%5.2f sec\n",used_time(T));

  cout << "AP SHORTEST PATHS1<int>        ";
  cout.flush();
  if (!AP_SHORTEST_PATHS1(G,cost,DIST2)) cout << "negative cycle" << endl;
  cout << string("%5.2f sec\n",used_time(T));

  cout << "AP SHORTEST PATHS2<int>        ";
  cout.flush();
  if (!AP_SHORTEST_PATHS2(G,cost,DIST2)) cout << "negative cycle" << endl;
  cout << string("%5.2f sec\n",used_time(T));

  int count = 0;
  forall_nodes(v,G)
   forall_nodes(w,G) 
     if (DIST1(v,w) != DIST2(v,w)) count++;

  cout << "count = " << count << endl;

  return 0;
}
