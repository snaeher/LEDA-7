/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dijkstra.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph_alg.h>

#include <LEDA/core/p_queue.h>

#include <LEDA/core/impl/k_heap.h>
#include <LEDA/core/impl/bin_heap.h>
#include <LEDA/core/impl/f_heap.h>

/*
#include <LEDA/core/impl/m_heap.h>
#include <LEDA/core/impl/p_heap.h>
#include <LEDA/core/impl/r_heap.h>
#include <LEDA/core/impl/list_pq.h>
*/

/*
#include <LEDA/core/hollow_heap1.h>
#include <LEDA/core/hollow_heap2.h>
#include <LEDA/core/hollow_heap3.h>
*/

#include <LEDA/system/assert.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



template<class pq_impl>
void dijkstra(graph& G, 
              node s, 
              edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred,
              p_queue<int,node,pq_impl>& PQ)
{

  typedef typename p_queue<int,node,pq_impl>::item pq_item;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

//int decr_count = 0;

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
          { PQ.decrease_p(I[v],c);
            //decr_count++;
           }
        dist[v] = c;
        pred[v] = e;
       }                                                                 
     }
    PQ.del_item(it);
   }

//cout << "decrease: " << decr_count << endl;
}

/*
void dijkstra_hollow1(graph& G, 
              node s, 
              edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred)
{
  typedef typename hollow_heap1<int,node>::item pq_item;

  hollow_heap1<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

//int decr_count = 0;

  while (! PQ.empty())
  { pq_item it = PQ.find_min();
    node u = PQ.inf(it);
    PQ.del_min();

    int du = dist[u];
    edge e;
    forall_adj_edges(e,u)
    { v = G.target(e);
      int c = du + cost[e];
      if (c < dist[v])
      { if (dist[v] == MAXINT)
          I[v] = PQ.insert(c,v);
        else
          { PQ.decrease_p(I[v],c);
            //decr_count++;
           }
        dist[v] = c;
        pred[v] = e;
       }                                                                 
     }
   }

//cout << "decrease: " << decr_count << endl;
}


void dijkstra_hollow2(graph& G, 
              node s, 
              edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred)
{
  typedef typename hollow_heap2<int,node>::item pq_item;

  hollow_heap2<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

//int decr_count = 0;

  while (! PQ.empty())
  { pq_item it = PQ.find_min();
    node u = PQ.inf(it);
    PQ.del_min();

    int du = dist[u];
    edge e;
    forall_adj_edges(e,u)
    { v = G.target(e);
      int c = du + cost[e];
      if (c < dist[v])
      { if (dist[v] == MAXINT)
          I[v] = PQ.insert(c,v);
        else
          { PQ.decrease_p(I[v],c);
            //decr_count++;
           }
        dist[v] = c;
        pred[v] = e;
       }                                                                 
     }
   }

//cout << "decrease: " << decr_count << endl;
}


void dijkstra_hollow3(graph& G, 
              node s, 
              edge_array<int>&  cost, 
              node_array<int>&  dist,
              node_array<edge>& pred)
{
  typedef typename hollow_heap3<int,node>::item pq_item;

  hollow_heap3<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
  { pred[v] = nil;
    dist[v] = MAXINT;
   }

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

//int decr_count = 0;

  while (! PQ.empty())
  { pq_item it = PQ.find_min();
    node u = PQ.inf(it);
    PQ.del_min();

    int du = dist[u];
    edge e;
    forall_adj_edges(e,u)
    { v = G.target(e);
      int c = du + cost[e];
      if (c < dist[v])
      { if (dist[v] == MAXINT)
          I[v] = PQ.insert(c,v);
        else
          { PQ.decrease_p(I[v],c);
            //decr_count++;
           }
        dist[v] = c;
        pred[v] = e;
       }                                                                 
     }
   }

//cout << "decrease: " << decr_count << endl;
}


*/




int main()
{
  GRAPH<int,int> G;

  bool go_on = true;
  while(go_on)
  {

  int n = read_int("# nodes = ");
  int m = read_int("# edges = ");

  if (n==0) break;

  random_graph(G,n,m);

  edge_array<int>  cost(G);
  node_array<int>  dist0(G);
  node_array<int>  dist(G);
  node_array<edge> pred(G);

  int M = read_int("max edge cost = ");

  node s = G.choose_node();

  edge e;
  forall_edges(e,G) G[e] = cost[e] = rand_int(0,M);

  float T  = used_time();
  cout << "DIJKSTRA: ";
  cout.flush();
  DIJKSTRA(G,s,cost,dist0,pred);
  cout << string(" %6.2f sec\n",used_time(T));
  cout << endl;

  string choice = 
    "0:f_heap 1:hollow1 2:hollow2 3:hollow3 4:p_heap 5:r_heap 6:bin_heap --> ";

  for(;;)
  { int i = read_int(choice);

    if (i>6) { if (i>7) go_on = false; break; }

    float T  = used_time();

    switch (i) {

    case 0: { p_queue<int,node,f_heap> PQ;
              dijkstra(G,s,cost,dist,pred,PQ);
              break;
             }

/*
    case 1: { //p_queue<int,node,k_heap> PQ(n);
              dijkstra_hollow1(G,s,cost,dist,pred);
              break;
             }

    case 2: { //p_queue<int,node,m_heap> PQ(M);
              dijkstra_hollow2(G,s,cost,dist,pred);
              break;
             }

    case 3: { //p_queue<int,node,list_pq> PQ;
              dijkstra_hollow3(G,s,cost,dist,pred);
              break;
             }

    case 4: { p_queue<int,node,p_heap> PQ;
              dijkstra(G,s,cost,dist,pred,PQ);
              break;
             }

    case 5: { p_queue<int,node,r_heap> PQ(M);
              dijkstra(G,s,cost,dist,pred,PQ);
              break;
             }
*/

    case 6: { p_queue<int,node,bin_heap> PQ(n);
              dijkstra(G,s,cost,dist,pred,PQ);
              break;
             }

    }

    cout << string("time: %6.2f sec\n",used_time(T));
    cout << endl;

    node v;
    forall_nodes(v,G)
      assert(dist[v] == MAXINT || dist[v] == dist0[v]);

   }
 }

 return 0;
}
