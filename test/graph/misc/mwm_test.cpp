/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mwm_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/array.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



template <class NT>

void test_mwm(const graph& G, const edge_array<NT>& weight, 
                            bool perfect, bool max, bool check, int heur)
{
  list<edge> M;

  cout << endl;
  cout <<  "computing matching ... " << flush;

  float T = used_time();

  if (perfect) 
    if (max)
      M = MAX_WEIGHT_PERFECT_MATCHING(G, weight, check, heur);
    else 
      M = MIN_WEIGHT_PERFECT_MATCHING(G, weight, check, heur);
  else 
    if (max)
      M = MAX_WEIGHT_MATCHING(G, weight, check, heur);
    else
      error_handler(1,"non-perfect minimum-weight matching not supported!");      
  NT W = 0;
  edge e;
  forall(e,M) W += weight[e];

  cout << endl;
  cout << string("|M| = %d  W = %d   time = %.2f sec", M.length(),
                                                       W, used_time(T));
  cout << endl;
}



int main() 
{

  int heur = 2;      
  bool max = true; 
  bool perfect = false; 
  bool check = true;

  int n = read_int("n = ");
  int m = read_int("m = ");
  int low  = read_int("low  = ");
  int high = read_int("high = ");

  //ugraph G;
  graph G;
  random_graph(G,n,m,1,1,1);

  edge_array<int>    weight1(G);
  edge_array<double> weight2(G);

  edge e;
  forall_edges(e, G) weight2[e] = weight1[e] = 4*rand_int(low/4, high/4);


  test_mwm(G,weight1,perfect,max,check,heur);
  //test_mwm(G,weight1,true,false,check,heur);
  //test_mwm(G,weight2,perfect,max,check,heur);

  G.make_undirected();
  test_mwm(G,weight1,perfect,true,check,heur);
  test_mwm(G,weight1,true,false,check,heur);


  return 0;
}


