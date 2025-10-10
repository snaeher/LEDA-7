/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  testall.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_set.h>
#include <LEDA/graph/node_slist.h>
#include <LEDA/graph/node_map2.h>

#include <LEDA/system/stream.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main(int argc, char** argv)
{

  GRAPH<string, list<int> > G;

  G.set_node_bound(100);
  G.set_edge_bound(200);

  if (argc > 1)  // file argument
     G.read(argv[1]);
  else
   { random_graph(G,12,30);
     int count = 0;

     node v;
     forall_nodes(v,G)
         G[v] = string("This is {node with index %d|.",count++);
   
     edge e;
     forall_edges(e,G)
     { G[e].clear();
       G[e].append(rand_int(1,100));
       G[e].append(rand_int(1,100));
       G[e].append(rand_int(1,100));
       G[e].append(rand_int(1,100));
      }

     cout << count << endl;

     G.print();

     G.write("graph.lgr");

    }


{
  node_set S(G);
  node v;
  forall_nodes(v,G) S.insert(v);
  forall(v,S) { G.print_node(v); cout << endl; }
  cout << endl;
}


{
  node_slist L(G);
  node v;
  forall_nodes(v,G) L.append(v);
  forall(v,L) { G.print_node(v); cout << endl; }
  cout << endl;
}

{
  node_map2<int> M(G,0);
  edge e;
  forall_edges(e,G)
     M(source(e),target(e))++;

 }
     
{
  node_array<string> NA = G.node_data();
  node v;
  forall_nodes(v,G) cout << NA[v] << endl;
  v = G.first_node();
  NA[v] = "This is node 99999";
  cout << endl;
}
  
  G.print();
  cout << endl;

  edge_array<int>  cost(G);
  node_array<int>  dist(G);
  node_array<edge> pred(G);

  node_array<int>  ord(G);
  node_array<int>  compnum(G);
  edge_array<int>  flow(G) ;
  node_array<bool> reached(G,false);
  node_array<int>  dfs_num(G);
  node_array<int>  comp_num(G);
  node_array<int>  layer(G,-1);

  node_matrix<int> M(G);

  list<node> nl;
  list<edge> el;
  node v,w,s,t;
  edge e;


/*
  UGRAPH<string, list<int> > U = G;
  node_array<int> compnum1(U);
*/

  random_source ran(0,1000);

  forall_edges(e,G) cost[e] = ran();

  

  cout << "TOPSORT:\n";
  if (TOPSORT(G,ord)) 
     cout << "graph is acyclic\n";
  else 
     cout << "graph is cyclic\n";
  cout << endl;

  cout << endl;
  cout << "DFS:\n";
  cout << endl;
  nl = DFS(G,G.choose_node(),reached);
  cout << "DFS:\n";
  forall(v,nl) { G.print_node(v); cout << endl; }
  cout << endl;

  cout << endl;
  cout << "DFS_NUM:\n";
  DFS_NUM(G,dfs_num,comp_num);
  forall_nodes(v,G) 
  { G.print_node(v);
    cout << string("  dfsnum = %2d  compnum = %2d \n",dfs_num[v],comp_num[v]);
   }
  cout << endl;

  cout << endl;
  cout << "BFS:\n";
  nl = BFS(G,G.first_node(),layer);
  forall_nodes(v,G)
  { G.print_node(v);
    cout << string("  layer = %2d\n",layer[v]);
   }
  cout << endl;


  cout << endl;
  cout << "COMPONENTS:\n";
  COMPONENTS(G,compnum);
  forall_nodes(v,G)
  { G.print_node(v);
    cout << string("  compnum = %2d \n",compnum[v]);
   }
  cout << endl;

/*
  cout << endl;
  cout << "COMPONENTS1:\n";
  COMPONENTS1(G,compnum);
  forall_nodes(v,G)
  { G.print_node(v);
    cout << string("  compnum = %2d \n",compnum[v]);
   }
  cout << endl;
*/


#if GRAPH_REP != 2
  cout << endl;
  cout << "TRANSITIVE_CLOSURE:\n";
  graph G1 = TRANSITIVE_CLOSURE(G);
  G1.print("Graph G1 = transitive closure of G");
  cout << endl;
#endif


  cout << endl;
  cout << "SPANNING_TREE: \n";
  el = SPANNING_TREE(G);
  forall(e,el) 
    { G.print_edge(e);;
      cout << endl;
     }
  cout << endl;


  cout << "MIN_SPANNING_TREE: \n";
  el = MIN_SPANNING_TREE(G,cost);
  forall(e,el) 
  { G.print_edge(e);;
    cout << endl;
   }


  cout << endl;
  cout << "STRONG_COMPONENTS:\n";
  STRONG_COMPONENTS(G,compnum);
  forall_nodes(v,G) 
  { G.print_node(v);
    cout << string("  compnum = %d\n",compnum[v]);
   }
  cout << endl;


  s = G.first_node();

  float T = used_time();

  cout << endl;
  cout << "DIJKSTRA <int>      ";
  cout.flush();
  DIJKSTRA(G,s,cost,dist,pred);
  cout << string("%6.2f sec  \n",used_time(T));
  cout << endl;

  cout << "BELLMAN_FORD <int>  ";
  cout.flush();
  BELLMAN_FORD(G,s,cost,dist,pred);
  cout << string("%6.2f sec  \n",used_time(T));
  cout << endl;

  cout << "ALL PAIRS SHORTEST PATHS <int> ";
  cout.flush();
  ALL_PAIRS_SHORTEST_PATHS(G,cost,M);
  cout << string("%.2f sec\n",used_time(T));
  forall_nodes(v,G)
  { forall_nodes(w,G) cout << string("%7d ",M(v,w));
    cout << endl;
   }
  cout << endl;


  cout << "MAX_FLOW<int>:  ";
  cout.flush();
  s = G.first_node();
  t = G.last_node();
  int val = MAX_FLOW(G,s,t,cost,flow) ;
  cout << string("total flow = %d \n",val);
  cout << endl;





  edge_array<double>  cost1(G);
  node_array<double>  dist1(G);
  edge_array<double>  flow1(G) ;
  node_matrix<double> M1(G);

  forall_edges(e,G)  cost1[e] = cost[e];


  used_time(T);
  cout << "DIJKSTRA <double>     ";
  cout.flush();
  DIJKSTRA(G,s,cost1,dist1,pred);
  cout << string("%6.2f sec  \n",used_time(T));
  cout << endl;


  cout << "BELLMAN_FORD <double> ";
  cout.flush();
  BELLMAN_FORD(G,s,cost1,dist1,pred);
  cout << string("%6.2f sec  \n",used_time(T));
  cout << endl;


  cout << "ALL PAIRS SHORTEST PATHS <double>  ";
  cout.flush();
  ALL_PAIRS_SHORTEST_PATHS(G,cost1,M1);
  cout << string("%.2f sec\n",used_time(T));
  cout << endl;

  cout << "MAX_FLOW<double>: ";
  cout.flush();
  double val1 = MAX_FLOW(G,s,t,cost1,flow1) ;
  cout << string("total flow = %f \n",val1);
  cout << endl;



  G.write("graph.ggg");

  Delete_Loops(G);

  if (PLANAR(G)) 
    { cout << "G is planar\n";
      //cout << "STRAIGHT_LINE_EMBEDDING: \n";
      //node_array<int>   xcoord(G);
      //node_array<int>   ycoord(G);
      //STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);
      //forall_nodes(v,G) 
      //{ G.print_node(v);
      //  cout << string("  x = %3d   y = %3d\n",xcoord[v],ycoord[v]);
      // }
      // cout << endl;
     }
  else 
   { cout << "Graph is not planar." << endl;
     cout << "I compute a a Kuratowsky Subgraph ..." << endl;
     list<edge> L;
     edge e;

     assert(!PLANAR(G,L));  

     cout << "|L| = " << L.size() << endl; 

     forall(e,L) 
     { G.print_edge(e);
       cout << endl;
      }
     cout << endl;

    }

  return 0;
}
