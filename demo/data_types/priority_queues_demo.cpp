/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  priority_queues_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:19 $



#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>

#include <LEDA/core/p_queue.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/array.h>
#include <LEDA/core/IO_interface.h>

/*
#include <LEDA/core/hollow_heap1.h>
#include <LEDA/core/hollow_heap2.h>
#include <LEDA/core/hollow_heap3.h>
*/

using namespace leda;



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
          PQ.decrease_p(I[v],c);
        dist[v] = c;
      }                                                                 
    }
    //PQ.del_item(it);
  }
}


/*
void dijkstra_hollow1(graph& G, node s, const edge_array<int>& cost, 
              node_array<int>& dist)
{ 
  typedef hollow_heap1<int,node>::item pq_item;

  hollow_heap1<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
    dist[v] = MAXINT;

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

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
          PQ.decrease_p(I[v],c);
        dist[v] = c;
      }                                                                 
    }
    //PQ.del_item(it);
  }
}


void dijkstra_hollow2(graph& G, node s, const edge_array<int>& cost, 
              node_array<int>& dist)
{ 
  typedef hollow_heap2<int,node>::item pq_item;

  hollow_heap2<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
    dist[v] = MAXINT;

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

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
          PQ.decrease_p(I[v],c);
        dist[v] = c;
      }                                                                 
    }
    //PQ.del_item(it);
  }
}

void dijkstra_hollow3(graph& G, node s, const edge_array<int>& cost, 
              node_array<int>& dist)
{ 
  typedef hollow_heap3<int,node>::item pq_item;

  hollow_heap3<int,node> PQ;

  node_array<pq_item> I(G);
  node v;
                                                                               
  forall_nodes(v,G)
    dist[v] = MAXINT;

  dist[s] = 0;
  I[s] = PQ.insert(0,s);

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
          PQ.decrease_p(I[v],c);
        dist[v] = c;
      }                                                                 
    }
    //PQ.del_item(it);
  }
}


*/



int DIJKSTRA_GEN(GRAPH<int,int>& G, int n, int m, int c = 0)
{ G.clear();
  
  array<node> V(n);
  int i;
  for (i = 0; i < n; i++) V[i] = G.new_node(i);

  stack<edge> S;
  int m1 = m - (n - 1);
  i = 0;
  int j = i + 2;
  while (m1 > 0)
  { if (j < n )
    { S.push(G.new_edge(V[i],V[j])); m1--; j++; }
    else
    { i++; j = i + 2;
      if (j == n) 
      error_handler(1,"DIJKSTRA_GEN: m can be at most n*(n-1)/2"); 
    }
  }
  edge e = S.pop();
  int last_c = G[e] = c + 1;
  while (!S.empty())
  { e = S.pop();
    int j = G[G.target(e)];
    if (j == n-1)
      last_c = G[e] = last_c + c + 1;
    else
      last_c = G[e] = last_c + 1;
  }

 for (i = 0; i < n-1; i++) G.new_edge(V[i], V[i+1], c);

 return last_c;
}


#include <LEDA/core/impl/k_heap.h>
#include <LEDA/core/impl/bin_heap.h>
/*
#include <LEDA/core/impl/m_heap.h>
#include <LEDA/core/impl/p_heap.h>
#include <LEDA/core/impl/list_pq.h>
*/
#include <LEDA/core/impl/r_heap.h>


int main()
{
  GRAPH<int,int> G;
  IO_interface I("Priority Queues and Dijkstras Algorithm");

I.write_demo("This demo compares the speed of our priority queue \
implementations.");
I.write_demo("We run Dijkstras shortest path algorithm with \
different priority queues on a graph with n nodes and m edges.");
I.write_demo(" The graph is either a random graph or a worst \
 case graph constructed by \
the program DIJKSTRA_GEN. For random graph the edge costs are random \
integers and for the worst case graph the edge costs are chosen by \
DIJKSTRA_GEN starting from a seed c.");

  for(;;)
  {

  int n = I.read_int("# nodes = ");  if (n==0) break;
  int m = I.read_int("# edges = ");
  int r = I.read_int("random graph = 0, worst case = 1: ");
  int M;  int c;
  G.clear();

  if (r == 0)
  { 
    random_graph(G,n,m);
    M = I.read_int("max edge cost + 1 = ");
    if (M < 1) M = 1;
  }
  else
    { c = I.read_int("constant c for DIJKSTRA_GEN = ");
      if (c < 0) c = 0;
      M = 1 + DIJKSTRA_GEN(G,n,m,c);
    }

  edge_array<int>  cost(G);
  node_array<int>  dist0(G);
  node_array<int>  dist(G);
  node_array<edge> pred(G);
  node s = G.first_node();
  edge e; 
  
  if (r == 0)
  { 
    forall_edges(e,G) G[e] = cost[e] = rand_int(0,M-1);
  }


  string choice[8];

  choice[0] = "LEDA default";

  choice[1] = "f_heap";
/*
  //choice[1] = "k_heap";
  //choice[2] = "m_heap";
  //choice[3] = "list_pq";
  choice[2] = "hollow_pq1";
  choice[3] = "hollow_pq2";
  choice[4] = "hollow_pq3";
  choice[5] = "p_heap";
*/
  choice[6] = "r_heap";
  choice[7] = "bin_heap";


  for(int i = 0; i < 8; i++)
  { 
    //if ( i == 3 && m > 20000 ) continue;
    //if ( i == 3) continue;

    float T  = used_time();

    switch (i) {

    case 0: {
              DIJKSTRA(G,s,cost,dist);
              break;
             }

    case 1: { p_queue<int,node> PQ;
              dijkstra(G,s,cost,dist,PQ);
              break;
             }
/*
    case 1: { p_queue<int,node,k_heap> PQ(n);
              dijkstra(G,s,cost,dist,PQ);
              break;
             }
    case 2: { p_queue<int,node,m_heap> PQ(M);
              dijkstra(G,s,cost,dist,PQ);
              break;
             }
*/
/*
    case 2: { 
              dijkstra_hollow1(G,s,cost,dist);
              break;
             }
    case 3: { //p_queue<int,node,list_pq> PQ;
              dijkstra_hollow2(G,s,cost,dist);
              break;
             }

    case 4: { //p_queue<int,node,list_pq> PQ;
              dijkstra_hollow3(G,s,cost,dist);
              break;
             }
    case 5: { p_queue<int,node,p_heap> PQ;
              dijkstra(G,s,cost,dist,PQ);
              break;
             }
*/

    case 6: { p_queue<int,node,r_heap> PQ(M);
              dijkstra(G,s,cost,dist,PQ);
              break;
             }
    case 7: { p_queue<int,node,bin_heap> PQ(n);
              dijkstra(G,s,cost,dist,PQ);
              break;
             }

    }

    I.write_demo(choice[i] + ": used time = ",used_time(T));

  }

 I.stop();

 }

 return 0;
}


