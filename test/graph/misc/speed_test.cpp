/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  speed_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/ugraph.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




int main(int argc, char** argv)
{
  GRAPH<int,int> G0;

  float T = used_time();

  int n;
  int m;

  if (argc > 1)  // file argument
    { cout << "read  graph          " << flush;
      G0.read(argv[1]);
      cout << string("%6.2f sec\n",used_time(T));
      n = G0.number_of_nodes();
      m = G0.number_of_edges();
     }
  else
    { n = read_int("|V| = ");
      m = read_int("|E| = ");
      G0.set_node_bound(n);
      G0.set_edge_bound(m);
      rand_int.set_seed(n*m);
      T = used_time();
      cout << "build graph          " << flush;
      random_graph(G0,n,m);
Delete_Loops(G0);
      cout << string("%6.2f sec\n",used_time(T));
      G0.write("graph.1");
     }

  GRAPH<int,int> G1 = G0;


cout << "n = " << G1.number_of_nodes() << endl;
cout << "m = " << G1.number_of_edges() << endl;
cout << endl;

  random_source ran(10); // random 10 bit numbers
  ran.set_seed(n*m);


  float T0 = used_time();

  T = T0;

  cout << "build ugraph         " << flush;
  UGRAPH<int,int> U = G1;
  cout << string("%6.2f sec\n",used_time(T));

  cout << "build bi-graph       " << flush;
  GRAPH<int,int> G2;
  G2.set_node_bound(n);
  G2.set_edge_bound(m);
  list<node> A;
  list<node> B;
  random_bigraph(G2,n/2,n/2,m,A,B);
  cout << string("%6.2f sec\n",used_time(T));

  node s = G1.first_node();
  node t = G1.last_node();

  cout << "node/edge arrays     " << flush;

cout << endl;

  edge_array<int>     cost1(G1);
  edge_array<double>  cost2(G1);
  node_array<int>     dist1(G1);
  node_array<double>  dist2(G1);
  node_array<edge>    pred(G1);
  edge_array<int>     flow1(G1);
  edge_array<double>  flow2(G1);

  node_array<int>  ord(G1);
  node_array<int>  compnum(G1);
  node_array<bool> reached(G1,false);
  node_array<int>  dfs_num(G1);
  node_array<int>  comp_num(G1);
  node_array<int>  layer(G1,-1);

cout << endl;

  edge_array<int>    cost3(G2);
  edge_array<double> cost4(G2);

  node_array<int>  compnum1(U);

  cout << string("%6.2f sec\n",used_time(T));


  edge e;

  cout << "assign edge labels   " << flush;
  forall_edges(e,G1) cost2[e] = cost1[e] = G1[e] = ran();
  forall_edges(e,G2) cost4[e] = cost3[e] = G2[e] = ran();
  cout << string("%6.2f sec\n",used_time(T));
  cout << endl;

  cout << "TOPSORT              " << flush;
  TOPSORT(G1,ord); 
  cout << string("%6.2f sec\n",used_time(T));

  cout << "DFS                  " << flush;
  DFS(G1,s,reached);
  cout << string("%6.2f sec\n",used_time(T));

  cout << "DFS_NUM              " << flush;
  DFS_NUM(G1,dfs_num,comp_num);
  cout << string("%6.2f sec\n",used_time(T));

  cout << "BFS                  " << flush;
  BFS(G1,G1.first_node(),layer);
  cout << string("%6.2f sec\n",used_time(T));

  cout << "COMPONENTS           " << flush;
  int c = COMPONENTS(G1,compnum);
  cout << string("%6.2f sec  c = %d",used_time(T),c) << endl;

/*
  cout << "COMPONENTS1          " << flush;
  c = COMPONENTS1(G1,compnum);
  cout << string("%6.2f sec  c = %d",used_time(T),c) << endl;
*/

  cout << "STRONG_COMPONENTS    " << flush;
  c = STRONG_COMPONENTS(G1,compnum);
  cout << string("%6.2f sec  c = %d",used_time(T),c) << endl;

  cout << "SPANNING_TREE        " << flush;
  list<edge> EL = SPANNING_TREE(G1);
  cout << string("%6.2f sec |T| = %d",used_time(T),EL.length()) << endl;

  cout << "MS_TREE<int>         " << flush;
  EL = MIN_SPANNING_TREE(G1,cost1);
  c = 0;
  forall(e,EL) c += cost1[e]; 
  cout << string("%6.2f sec |T| = %d  W(T) = %d",used_time(T),EL.length(),c);
  cout << endl;


  cout << "MS_TREE<int>         " << flush;
  edge_array<int>& e_cost = G1.edge_data();
  EL = MIN_SPANNING_TREE(G1,e_cost);
  c = 0;
  forall(e,EL) c += cost1[e]; 
  cout << string("%6.2f sec |T| = %d  W(T) = %d",used_time(T),EL.length(),c);
  cout << endl;

  cout << "MS_TREE<double>      " << flush;
  EL = MIN_SPANNING_TREE(G1,cost2);
  double c2 = 0;
  forall(e,EL) c2 += cost2[e]; 
  cout << string("%6.2f sec |T| = %d  W(T) = %.2f",used_time(T),EL.length(),c2);
  cout << endl;

  cout << "DIJKSTRA <int>       " << flush;
  DIJKSTRA(G1,s,cost1,dist1,pred);
  cout << string("%6.2f sec  d  = %d",used_time(T),dist1[t]) << endl;

  cout << "DIJKSTRA <int>       " << flush;
  DIJKSTRA(G1,s,e_cost,G1.node_data(),pred);
  cout << string("%6.2f sec  d  = %d",used_time(T),G1[t]) << endl;

  cout << "DIJKSTRA <double>    " << flush;
  DIJKSTRA(G1,s,cost2,dist2,pred);
  cout << string("%6.2f sec  d  = %.2f",used_time(T),dist2[t]) << endl;

  cout << "BELLMAN_FORD<int>    " << flush;
  BELLMAN_FORD(G1,s,cost1,dist1,pred);
  cout << string("%6.2f sec  d  = %d",used_time(T),dist1[t]) << endl;

  cout << "BELLMAN_FORD<double> " << flush;
  BELLMAN_FORD(G1,s,cost2,dist2,pred);
  cout << string("%6.2f sec  d  = %.2f",used_time(T),dist2[t]) << endl;

  if (G1.number_of_nodes() < 50)
  { node_matrix<int> M(G1);
    cout << "ALL PAIRS<int>       " << flush;
    ALL_PAIRS_SHORTEST_PATHS(G1,cost1,M);
    cout << string("%6.2f sec\n",used_time(T));
   }

  cout << "MAX FLOW<int>        " << flush;
  int f1 = MAX_FLOW(G1,s,t,cost1,flow1) ;
  cout << string("%6.2f sec  f  = %d",used_time(T),f1) << endl;

  cout << "MAX FLOW<double>     " << flush;
  double f2 = MAX_FLOW(G1,s,t,cost2,flow2) ;
  cout << string("%6.2f sec  f  = %.2f",used_time(T),f2) << endl;

  T = used_time();

/*
  cout << "MC  MATCHING         " << flush;
  list<edge> M1 = MAX_CARD_MATCHING(G1);
  cout << string("%6.2f sec |M| = %d",used_time(T),M1.length()) << endl;


  list<edge> R = Make_Bidirected(G1);
  GRAPH<int,int> G4 = G1;
  edge y;
  forall(y,R) G1.del_edge(y);
  cout << "STATIC MC  MATCHING  " << flush;
  list<edge> M5 = STATIC_MAX_CARD_MATCHING(G4);
  cout << string("%6.2f sec |M| = %d",used_time(T),M5.length()) << endl;
*/


  cout << "MCB MATCHING         " << flush;
  list<edge> M2 = MAX_CARD_BIPARTITE_MATCHING(G2,A,B);
  cout << string("%6.2f sec |M| = %d",used_time(T), M2.length()) << endl;

  cout << "MWB MATCHING<int>    " << flush;
  list<edge> M3 = MAX_WEIGHT_BIPARTITE_MATCHING(G2,A,B,cost3);
  int W1 = 0;
  forall(e,M3) W1 += cost3[e];
  cout << string("%6.2f sec |M| = %d  W(M) = %d",used_time(T), M3.length(),W1);
  cout << endl;


  cout << "MWB MATCHING<double> " << flush;
  list<edge> M4 = MAX_WEIGHT_BIPARTITE_MATCHING(G2,A,B,cost4);
  double W2 = 0;
  forall(e,M4) W2 += cost4[e];
  cout << string("%6.2f sec |M| = %d  W(M) = %.2f",used_time(T),M4.length(),W2);
  cout << endl;


  
  
  if (G1.number_of_nodes() < 500)
  { cout << "TRANSITIVE_CLOSURE   " << flush;
    graph clos = TRANSITIVE_CLOSURE(G1);
    cout << string("%6.2f sec |E| = %d",used_time(T),clos.number_of_edges()) << endl;
  
   }
  
  cout << endl;
  cout << "TOTAL TIME           ";
  cout << string("%6.2f sec\n",used_time(T0));

  return 0;
}
