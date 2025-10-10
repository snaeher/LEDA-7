#include <LEDA/graph/graph.h>
#include <LEDA/core/p_queue.h>

#include <LEDA/core/impl/f_heap.h>
#include <LEDA/core/impl/bin_heap.h>
/*
#include <LEDA/core/impl/p_heap.h>
*/

using namespace leda;

using std::cout;
using std::endl;



template<class impl>
void dijkstra(graph& G, node s, const edge_array<int>& cost, 
              node_array<int>& dist, p_queue<int,node,impl>& PQ)
{ 
  typedef typename p_queue<int,node,impl>::item pq_item;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
    dist[v] = MAXINT;

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

  while (! PQ.empty())
  { pq_item it = PQ.find_min();
    node u = PQ.inf(it);
    int du = dist[u];
    edge e;
    forall_adj_edges(e,u)
    { v = G.target(e);
      int c = du + cost[e];
      if (c < dist[v])
      { if (dist[v] == MAXINT)
          I[v] = PQ.insert(c,v);
        else
          PQ.decrease_p(I[v],c);
        dist[v] = c;
      }                                                                 
    }
    PQ.del_item(it);
  }
}


// cmp class with counter

class cmp_count : public leda_cmp_base<int> {

  mutable int count;

public:

  cmp_count() : count(0) {}

  int operator()(const int& x, const int& y) const
  { count++;
    return leda::compare(x,y);
   }

  int get_count() { return count; }

  void reset() { count = 0; }

};





int main()
{

  cout << endl;
  int n = read_int("# nodes = ");  
  int m = read_int("# edges = ");
  cout << endl;

  graph G;
  random_graph(G,n,m);

  edge_array<int>  cost(G);
  node_array<int>  dist(G);

  node s = G.first_node();

  edge e; 
  forall_edges(e,G) cost[e] = rand_int(0,100000);

  cmp_count cmp;

  float T  = used_time();

  p_queue<int,node,f_heap> PQ1(cmp);
  dijkstra(G,s,cost,dist,PQ1);
  cout << string("f_heap:   comparisons: %d   time: %.2f sec", 
                            cmp.get_count(), used_time(T));
  cout << endl;

  cmp.reset();
  p_queue<int,node,bin_heap> PQ2(cmp);
  dijkstra(G,s,cost,dist,PQ2);
  cout << string("bin_heap: comparisons: %d   time: %.2f sec", 
                            cmp.get_count(), used_time(T));
  cout << endl;

/*
  cmp.reset();
  p_queue<int,node,p_heap> PQ3(cmp);
  dijkstra(G,s,cost,dist,PQ3);
  cout << string("p_heap:   comparisons: %d   time: %.2f sec", 
                            cmp.get_count(), used_time(T));
  cout << endl;
*/


  return 0;
}


