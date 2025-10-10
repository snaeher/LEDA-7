/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  short.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




int main()
{

GRAPH<int,int> G0;
node s,v,w;
edge e;

int n = read_int("# nodes = ");
int m = read_int("# edges = ");

random_graph(G0,n,m);

GRAPH<int,int> G = G0;

edge_array<int>  cost1(G,0);
node_array<int>  dist1(G,0);
node_array<int>  dist2(G,0);
node_array<edge> pred(G,nil);


int a = read_int("a = ");
int b = read_int("b = ");

forall_edges(e,G) cost1[e] = G[e] = rand_int(a,b);

s = G.first_node();

float T = used_time();

cout << "DIJKSTRA <int>      ";
cout.flush();
DIJKSTRA(G,s,cost1,dist1,pred);
cout << string("%6.2f sec  \n",used_time(T));

cout << "BELLMAN_FORD <int>  ";
cout.flush();
BELLMAN_FORD(G,s,cost1,dist2,pred);
cout << string("%6.2f sec  \n",used_time(T));

if (Yes("output	 ? "))
 { forall_nodes(v,G)
   { G.print_node(v);
    cout << string(" %6d %6d\n",dist1[v],dist2[v]);
    }
   cout << endl;
  }


if (Yes("all pairs shortest paths <int> ? "))
{ 
  node_matrix<int> Mi(G);

  used_time(T);
  cout << "ALL PAIRS SHORTEST PATHS <int> ";
  cout.flush();
  ALL_PAIRS_SHORTEST_PATHS(G,cost1,Mi);
  cout << string("%.2f sec\n",used_time(T));
  
  if (Yes("output ? "))
  { node v;
    forall_nodes(v,G)
     { forall_nodes(w,G) cout << string("%6d ",Mi(v,w));
       cout << endl;
       }
   }
  cout << endl;
}


edge_array<double> cost2(G,0);
node_array<double> dist3(G,0);
node_array<double> dist4(G,0);

forall_edges(e,G) cost2[e] = cost1[e];

cout << "DIJKSTRA <double>     ";
cout.flush();
DIJKSTRA(G,s,cost2,dist3,pred);
cout << string("%6.2f sec  \n",used_time(T));


cout << "BELLMAN_FORD <double> ";
cout.flush();
BELLMAN_FORD(G,s,cost2,dist4,pred);
cout << string("%6.2f sec  \n",used_time(T));


if (Yes("output ? "))
 { forall_nodes(v,G)
   { G.print_node(v);
     cout << "  " << dist3[v] << "  " << dist4[v];
     cout << endl;
    }
   cout << endl;
  }

if (Yes("all pairs shortest paths <double> ? "))
{ 
  node_matrix<double> Mr(G);

  used_time(T);
  cout << "ALL PAIRS SHORTEST PATHS <double>  ";
  cout.flush();
  ALL_PAIRS_SHORTEST_PATHS(G,cost2,Mr);
  cout << string("%.2f sec\n",used_time(T));
  
  if (Yes("output ? "))
  { node v;
    forall_nodes(v,G)
    { forall_nodes(w,G) cout << string("%6.2f ",Mr(v,w));
      cout << endl;
     }
   }
  cout << endl;

 }

  return 0;
}
