/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mcb_matching.cpp
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
  graph G;
  list<node> A,B;

  bool heuristic = false;
  
  //test_bigraph(G,A,B);

  cout << endl;
  cout << "random bipartite graph" << endl;
  cout << endl;

  int a = read_int("|A| = ");
  int b = read_int("|B| = ");
  int m = read_int("|E| = ");

/*
  // random bipartite graphs with 10 % density
  int a = 10000;
  int b = 10000;
  int m = (a*b)/10;
*/


  cout << endl;
  cout << string("random graph: |A| = %d  |B| = %d  |E| = %d",a,b,m) << endl;
  cout << endl;

  random_bigraph(G,a,b,m,A,B);
  
  node_array<bool> NC(G,false);

  double T = cpu_time();

  cout << "MAX_CARD_BIPARTITE MATCHING              " << flush;
  list<edge> M0 = MAX_CARD_BIPARTITE_MATCHING(G,A,B,NC);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M0.length());

  
  cout << "MAX_CARD_BIPARTITE MATCHING_HK           " << flush;
  list<edge> M = MAX_CARD_BIPARTITE_MATCHING_HK(G,A,B,NC,heuristic);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M.length());


  cout << "MAX_CARD_BIPARTITE MATCHING_HK1          " << flush;
  list<edge> M1 = MAX_CARD_BIPARTITE_MATCHING_HK1(G,A,B,NC,heuristic);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M1.length());

  assert(M1.length() == M.length());


  cout << "MAX_CARD_BIPARTITE MATCHING_HK2          " << flush;
  list<edge> M2 = MAX_CARD_BIPARTITE_MATCHING_HK2(G,A,B,NC,heuristic);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M2.length());
  
  assert(M2.length() == M.length());

  
  cout << "MAX_CARD_BIPARTITE MATCHING_ABMP         " << flush;
  list<edge> M3 = MAX_CARD_BIPARTITE_MATCHING_ABMP(G,A,B,NC,heuristic);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M3.length());

  assert(M3.length() == M.length());
  
  
/*
  cout << "MAX_CARD_BIPARTITE MATCHING_FF           " << flush;
  list<edge> M4 = MAX_CARD_BIPARTITE_MATCHING_FF(G,A,B,NC,heuristic);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M4.length());

  assert(M4.length() == M.length());
*/
  
  
  cout << "MAX_CARD_MATCHING_GABOW                  " << flush;
  list<edge> M5 = MAX_CARD_MATCHING(G);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M5.length());
  cout << endl;

  assert(M5.length() == M.length());

  cout << "MAX_CARD_MATCHING_EDMONDS                " << flush;
  list<edge> M6 = MAX_CARD_MATCHING(G);
  cout << string("%5.2f sec    |M| = %d\n",cpu_time(T), M6.length());
  cout << endl;

  assert(M6.length() == M.length());
  
  
  return 0;
}
