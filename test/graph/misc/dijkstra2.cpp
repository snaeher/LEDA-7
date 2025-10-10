#define LEDA_CHECKING_OFF

#include <LEDA/core/array.h>

#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/node_slot.h>
#include <LEDA/graph/edge_slot.h>

#define DIJKSTRA_DIRECTED
#include <LEDA/graph/templates/dijkstra.h>

/*
#include <LEDA/core/f3_heap.h>
*/
#include <LEDA/core/impl/f_heap.h>

#include <limits>
#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


template <class NT, class graph_t>
class dijkstra0
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  
  float T;
  int   rebuilds;

 node opposite(const graph_t& G, edge e, node s)
  { if (graph_t::category() == bidirectional_graph_category ||
        graph_t::category() == directed_graph_category)
      return G.target(e);
    else
      return G.opposite(e,s);
  }
  
  public:

  dijkstra0() 
  { T = 0; 
    rebuilds = 0; 
   }

  float cpu_time() const { return T; }                                       
  int   num_rebuild() const { return rebuilds; }                                       
template<class  cost_array, class dist_array, class pred_array>
void run(const graph_t& G, node s, const cost_array& cost, 
                                         dist_array& dist,
                                         pred_array& pred,
                                         float delta)

{
  T = used_time();

  NT max_dist = std::numeric_limits<NT>::max();

  node_pq<int,graph_t,f_heap> PQ(G);

  node v;
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = max_dist;
   }


  dist[s] = 0;
  PQ.insert(s,0);

  while (!PQ.empty())
  { 
    node u = PQ.del_min();
    int du = dist[u];

    edge e;
    forall_out_edges(e,u)
    { node v = opposite(G,e,u);
      int dv = dist[v];
      int d = du + cost[e];
      if (d < dv) 
      { if (dv == max_dist)
           PQ.insert(v,d);
        else
           PQ.decrease_p(v,d);
        dist[v] = d;
        pred[v] = e;
       }                                                                 
     }
   }

  T = used_time(T);
}

};


#if 0


template <class NT, class graph_t, class pqueue_t>
class dijkstra1
{
  typedef typename graph_t::node node;
  typedef typename graph_t::edge edge;
  
  float T;
  int   rebuilds;
  int   del_min_count0;
  int   del_min_count1;

 node opposite(const graph_t& G, edge e, node s)
  { if (graph_t::category() == bidirectional_graph_category ||
        graph_t::category() == directed_graph_category)
      return G.target(e);
    else
      return G.opposite(e,s);
  }
  
  public:

  dijkstra1() 
  { T = 0; 
    rebuilds = 0; 
    del_min_count0 = 0;
    del_min_count1 = 0;
   }

  float cpu_time() const { return T; }                                       
  int   num_rebuild() const { return rebuilds; }                                       
  int   num_del_min0() const { return del_min_count0; } 
  int   num_del_min1() const { return del_min_count1; }
 
template<class  cost_array, class dist_array, class pred_array>
void run(const graph_t& G, node s, const cost_array& cost, 
                                         dist_array& dist,
                                         pred_array& pred,
                                         float delta)

{
  T = used_time();

  NT max_dist = std::numeric_limits<NT>::max();

  int n = G.number_of_nodes();
  int k = int((1+delta)*n);

  f3_heap<node,int> PQ(k);

  node v;
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = max_dist;
   }


  dist[s] = 0;
  PQ.insert(s,0);

  while (!PQ.empty())
  { 
    typename pqueue_t::item it = PQ.find_min();

    node u = PQ.inf(it);
    int du = PQ.prio(it);

    if (du > dist[u]) 
    { del_min_count0++;
      PQ.del_min(false);
      continue;
     }

    if (PQ.free_nodes() < G.outdeg(u) || (delta > 0 && PQ.ratio() > delta))
    {  PQ.clear();
       node v;
       forall_nodes(v,G) { 
          if (dist[v] < max_dist && dist[v] >= du) PQ.insert(v,dist[v]);
       }
       rebuilds++;
       continue;
    }

    del_min_count1++;
    PQ.del_min(true);

    edge e;
    forall_out_edges(e,u)
    { node v = opposite(G,e,u);
      int dv = dist[v];
      int d = du + cost[e];
      if (d < dv) 
      { if (dv == max_dist)
           PQ.insert(v,d);
        else
           PQ.decrease_p(v,d);
        dist[v] = d;
        pred[v] = e;
       }                                                                 
     }
   }

  T = used_time(T);
}

};

#endif



int main(int argc, char** argv)
{
  //typedef static_graph<opposite_graph,data_slots<2>,data_slots<1> >  st_graph;
  typedef static_graph<directed_graph,data_slots<2>,data_slots<1> >  st_graph;
  typedef st_graph::node st_node;
  typedef st_graph::edge st_edge;

  st_graph G;
  st_node s = 0;
  edge_slot<int,st_graph,0>  cost;
  node_slot<int,st_graph,0>  dist;
  node_slot<st_edge,st_graph,1> pred;


    if (st_graph::category() == bidirectional_graph_category)
     cout << "bidirectional_graph" <<endl;
    if (st_graph::category() == directed_graph_category)
     cout << "directed_graph" <<endl;
    if (st_graph::category() == opposite_graph_category)
     cout << "opposite_graph" <<endl;
    if (st_graph::category() == leda_graph_category)
     cout << "leda_graph" <<endl;


  int n = 0;
  int m = 0;
  //int delta = 0;
  //float d = 0;

  float d = 0;

  if (argc > 1) n = atoi(argv[1]);
  if (argc > 2) m = atoi(argv[2]);
  //if (argc > 3) delta = (float)atof(argv[3]);

  if (n == 0) n = read_int("# nodes = ");
  if (m == 0) m = read_int("# edges = ");
  if (d == 0) d = read_real("d = ");

  cout << endl;
  cout << "n = " << n <<endl;
  cout << "m = " << m <<endl;
  cout << "d = " << d <<endl;

  array<st_node> V(n);
  G.start_construction(n,m);

  for(int i=0; i<n; i++) V[i] = G.new_node();

  for(int i=0; i<n; i++) 
  { st_node x = V[i];
    for(int j=0; j<m/n; j++) 
    { st_node y = V[rand_int(0,n-1)];
      G.new_edge(x,y);
     }
  }

  G.finish_construction();

  s = G.first_node();

  st_edge x;
  forall_edges(x,G) cost[x] = rand_int(0,n);


  dijkstra<int,st_graph> D;

  cout << "DIJKSTRA:  ";
  cout.flush();
  D.run(G,s,cost,dist,pred);
  cout << string(" %6.2f sec",D.cpu_time());
  cout << endl;

  dijkstra0<int,st_graph> D0;

  cout << "DIJKSTRA0: ";
  cout.flush();
  D0.run(G,s,cost,dist,pred,d);
  cout << string(" %6.2f sec",D0.cpu_time());
  cout << endl;



/*
  dijkstra1<int,st_graph,f3_heap<st_node,int> > D1;

  cout << "DIJKSTRA1: ";
  cout.flush();
  D1.run(G,s,cost,dist,pred,d);
  cout << string(" %6.2f sec  rebuild: %d  del_min: %d/%d",
                   D1.cpu_time(), D1.num_rebuild(), 
                   D1.num_del_min0(), D1.num_del_min1());
  cout << endl;
*/

  return 0;
}
