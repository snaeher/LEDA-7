#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_pq22.h>

#include <LEDA/exp/core/f2_heap.h>
#include <LEDA/exp/core/f3_heap.h>

#include <LEDA/core/p_queue.h>
#include <LEDA/core/impl/f_heap.h>

#include <LEDA/system/assert.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



void dijkstra1(const graph& G, 
              node s, 
              const edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred)
{

  p_queue<int,node,f_heap> PQ;

  typedef p_queue<int,node,f_heap>::item item;

  node_array<item> Item(G,0);

  node v;
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  dist[s] = 0;
  Item[s] = PQ.insert(0,s);

  while (! PQ.empty())
  { item it = PQ.find_min();
    node u = PQ.inf(it);
    PQ.del_min();
    int du = dist[u];
    edge e;
    forall_out_edges(e,u)
    { node v = G.target(e);
      int d = du + cost[e];
      if (d < dist[v]) 
      { if (dist[v] == MAXINT)
           Item[v] = PQ.insert(d,v);
        else
           PQ.decrease_p(Item[v],d);
        dist[v] = d;
        pred[v] = e;
       }                                                                 
     }
   }
}



void dijkstra2(const graph& G, 
              node s, 
              const edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred)
{
  f2_heap<node,int> PQ;

  node v;
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  int n = G.number_of_nodes();

  dist[s] = 0;
  PQ.insert(s,0);

  while (!PQ.empty() && n > 0)
  { 
    f2_heap<node,int>::item it = PQ.find_min();
    node u = PQ.inf(it);
    int du = PQ.prio(it);
    PQ.del_min();

    if (du > dist[u]) continue;

    n--;

    edge e;
    forall_out_edges(e,u)
    { node v = G.target(e);
      int d = du + cost[e];
      if (d < dist[v]) 
      { PQ.insert(v,d);
        dist[v] = d;
        pred[v] = e;
       }                                                                 
     }
   }
}


void dijkstra3(const graph& G, 
              node s, 
              const edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred,
              float delta)
{

  int n = G.number_of_nodes();

  int k = int((1+delta)*n);

  f3_heap<node,int> PQ(k);

  node v;
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }


  dist[s] = 0;
  PQ.insert(s,0);

  while (!PQ.empty())
  { 
    float ratio = PQ.ratio();

    f3_heap<node,int>::item it = PQ.find_min();
    node u = PQ.inf(it);
    int du = PQ.prio(it);

    if (du > dist[u]) 
    { PQ.del_min(false);
      continue;
     }

    if (PQ.free_nodes() < G.outdeg(u) || (delta > 0 && ratio > delta))
    {  float t = used_time();
       PQ.clear();
       node v;
       forall_nodes(v,G) { 
          //if (dist[v] < MAXINT && dist[v] >= 0) 
          if (dist[v] < MAXINT && dist[v] >= du) PQ.insert(v,dist[v]);
       }
/*
       cout << string("rebuild: r = %.2f/%.2f t = %.2f",
                                ratio, PQ.ratio(), used_time(t)) << endl;
*/
       continue;
    }

    PQ.del_min(true);

    //dist[u] = -(du+1);

    edge e;
    forall_out_edges(e,u)
    { node v = G.target(e);
      int dv = dist[v];
      int d = du + cost[e];
      if (d < dv) 
      { if (dv == MAXINT)
           PQ.insert(v,d);
        else
           PQ.decrease_p(v,d);
        dist[v] = d;
        pred[v] = e;
       }                                                                 
     }
   }

/*
  forall_nodes(v,G)
    if (dist[v] < 0) dist[v] = -(dist[v]+1);
*/
}



int main(int argc, char** argv)
{
  graph G;

  int n = 0;
  int m = 0;
  float delta = 0;

  if (argc > 1) n = atoi(argv[1]);
  if (argc > 2) m = atoi(argv[2]);
  if (argc > 3) delta = (float)atof(argv[3]);

  if (n == 0) n = read_int("# nodes = ");
  if (m == 0) m = read_int("# edges = ");

  cout << endl;
  cout << "n = " << n <<endl;
  cout << "m = " << m <<endl;
  cout << "d = " << delta <<endl;

  random_graph(G,n,m);

  edge_array<int>  cost(G);
  node_array<int>  dist0(G);
  node_array<int>  dist(G);
  node_array<edge> pred(G);

  //int M = read_int("max edge cost = ");

  int M = n;

  node s = G.first_node();

  edge e;
  forall_edges(e,G) cost[e] = rand_int(0,M);

  float T;
  cout << "DIJKSTRA:  ";
  cout.flush();
  T  = used_time();
  DIJKSTRA(G,s,cost,dist0,pred);
  cout << string(" %6.2f sec\n",used_time(T));
  cout << endl;

  cout << "DIJKSTRA1: ";
  cout.flush();
  T  = used_time();
  dijkstra1(G,s,cost,dist,pred);
  cout << string(" %6.2f sec\n",used_time(T));
  cout << endl;

  cout << "DIJKSTRA2: ";
  cout.flush();
  T  = used_time();
  dijkstra2(G,s,cost,dist,pred);
  cout << string(" %6.2f sec\n",used_time(T));
  cout << endl;

  cout << "DIJKSTRA3: ";
  cout.flush();
  T  = used_time();
  dijkstra3(G,s,cost,dist,pred,delta);
  cout << string(" %6.2f sec\n",used_time(T));
  cout << endl;


  node v;
  forall_nodes(v,G) assert(dist[v] == dist0[v]);

 return 0;
}
