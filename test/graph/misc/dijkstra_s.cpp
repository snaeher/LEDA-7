/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dijkstra_s.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph_alg.h>
//#include <LEDA/graph/s_graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/system/assert.h>
#include <LEDA/internal/std/fstream.h>

#include <LEDA/core/impl/m_heap.h>
#include <LEDA/core/impl/f_heap.h>

using namespace leda;

using std::cout;
using std::cin;
using std::flush;
using std::endl;




template <class NT, class graph_t, class npqueue>
inline
void MY_DIJKSTRA(const graph_t& G, typename graph_t::node s, 
                   const edge_array<NT,graph_t>& cost,
                   node_array<NT,graph_t>& dist,
                   npqueue& PQ)
{ 
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;

  PQ.insert(s,0);

  node v;
  forall_nodes(v,G) dist[v] = MAXINT; 

  dist[s] = 0;

  while (!PQ.empty())
  { node u = PQ.find_min();
    //node u = PQ.del_min();
    NT du = dist[u];
    edge e;
    forall_adj_edges(e,u)                                                    
    { //v = G.opposite(u,e);
      v = G.target(e);
      NT& dv = dist[v];
      NT c = du + cost[e]; 
      if (c < dv)
      { if (dv == MAXINT) 
           PQ.insert(v,c);
        else 
           PQ.decrease_p(v,c); 
        dv = c; 
      }                                                                  
    } 
    PQ.del(u);
  }
}


int main()
{
  GRAPH<int,int> G(1,0);
  edge_array<int>&  cost = G.edge_data();
  node_array<int>&  dist = G.node_data();
  node s;

  cerr << endl;
  cerr << "Reading DIMACS Shortest Paths Problem from Std Input ..." << flush;
  Read_Dimacs_SP(cin,G,s,cost);
  cerr << endl;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cerr << "|V| = " << n << endl;
  cerr << "|E| = " << m << endl;
  cerr <<endl;

  edge e;
  //forall_edges(e,G) cost[e] = rand_int(0,999);


  node_pq<int,graph,bin_heap> PQ(G);

  float T;

  cout << endl;
  cout << "DIJKSTRA(graph):                 " << flush;
  T  = used_time();
  MY_DIJKSTRA((graph&)G,s,cost,dist,PQ);
  cout << string(" %6.2f sec",used_time(T)) << endl;

  typedef s_graph<basic_graph,2,1> static_graph;

  static_graph G2(n,m);

  node_array<static_graph::node> sn(G);
  edge_array<static_graph::edge> se(G);

  node v;
  forall_nodes(v,G) sn[v] = G2.new_node();

  forall_nodes(v,G) 
  { edge e;
    forall_adj_edges(e,v) 
       se[e] = G2.new_edge(sn[v],sn[target(e)]);
  }

  G2.finish_construction();


  static_graph::node s2 = sn[s];

  edge_array<int,static_graph >  cost2;
  assert(cost2.use_edge_data(G2));

  node_array<int,static_graph >  dist2;
  assert(dist2.use_node_data(G2));

  forall_edges(e,G) cost2[se[e]] = cost[e];


{
  node_pq<int,static_graph,bin_heap>   PQ(G2,1000);
  cout << "DIJKSTRA(static_graph/bin_heap): " << flush;
  T  = used_time();
  MY_DIJKSTRA(G2,s2,cost2,dist2,PQ);
  cout << string(" %6.2f sec",used_time(T)) << endl;
  node v;
  forall_nodes(v,G) 
    assert(dist[v] == dist2[sn[v]]);
}

{
  node_pq<int,static_graph,f_heap>   PQ(G2);

  cout << "DIJKSTRA(static_graph/f_heap):   " << flush;
  T  = used_time();
  MY_DIJKSTRA(G2,s2,cost2,dist2,PQ);
  cout << string(" %6.2f sec",used_time(T)) << endl;
  node v;
  forall_nodes(v,G) 
    assert(dist[v] == dist2[sn[v]]);
}

/*
{
  node_pq<int,static_graph,m_heap>   PQ(G2,1000);

  cout << "DIJKSTRA(static_graph/m_heap):   " << flush;
  T  = used_time();
  MY_DIJKSTRA(G2,s2,cost2,dist2,PQ);
  cout << string(" %6.2f sec",used_time(T)) << endl;
  node v;
  forall_nodes(v,G) 
    assert(dist[v] == dist2[sn[v]]);
}
*/


  cout << endl;
  return 0;
}
