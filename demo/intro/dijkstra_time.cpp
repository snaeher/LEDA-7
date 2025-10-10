/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  dijkstra_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




#include <LEDA/graph/graph.h>
#include <LEDA/graph/node_pq.h>

using namespace leda;

using std::cout;
using std::endl;

void DIJKSTRA(const graph &G, node s, 
              const edge_array<double>& cost,
              node_array<double>& dist)
{ node_pq<double> PQ(G);
  node v; edge e;

  forall_nodes(v,G)
  { if (v == s) dist[v] = 0; else dist[v] = MAXDOUBLE;
    PQ.insert(v,dist[v]);
  }

  while ( !PQ.empty() )
  { node u = PQ.del_min();
    forall_out_edges(e,u)
      { v = target(e);
        double c = dist[u] + cost[e];
        if ( c < dist[v] ) 
        {  PQ.decrease_p(v,c);  dist[v] = c;  }
      }
  }
}


int main()
{ int n = read_int("number of nodes = ");
  int m = read_int("number of edges = ");
  graph G;
  random_graph(G,n,m);
  edge_array<double> cost(G);
  node_array<double> dist(G);

  edge e; forall_edges(e,G) cost[e] = ((double) rand_int(0,100));
 
  float T = used_time();
  DIJKSTRA(G,G.first_node(),cost,dist);
  cout << "\n\nThe shortest path computation took " << 
          used_time(T) << " seconds.\n\n";

  return 0;
}

