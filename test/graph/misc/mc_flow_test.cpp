/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mc_flow_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/system/assert.h>

using namespace leda;

using std::ofstream;
using std::cout;
using std::flush;
using std::endl;



int main()
{
  graph G;

  test_graph(G);
  cout << endl;

  cout << "|V| = " << G.number_of_nodes() << endl;
  cout << "|E| = " << G.number_of_edges() << endl;
  cout << endl;

  cout << "capacities in [0 .. U]" << endl;
  int U = read_int("U = ");
  cout << endl;

  cout << "edge costs in [Cmin .. Cmax]" << endl;
  int Cmin = read_int("Cmin = ");
  int Cmax = read_int("Cmax = ");
  cout << endl;

  edge_array<int>  cap(G,0);
  edge_array<int>  flow(G,0);
  edge_array<int>  cost(G,0);

  node s = G.first_node();
  node t = G.last_node();

  edge e;
  forall_edges(e,G) cap[e]  = rand_int(0,U);
  forall_edges(e,G) cost[e] = rand_int(Cmin,Cmax);

  int fmax = MAX_FLOW(G,s,t,cap,flow) ;

  node_array<int> supply(G,0);
  supply[s] = fmax;
  supply[t] = -fmax;

  edge_array<int> lcap(G,0);

  ofstream out("mcflow.dim");
  Write_Dimacs_MCF(out,G,supply,lcap,cap,cost);
  
  float T = used_time();

  cout << "capacity scaling:      " << flush;
  bool f2 = MCF_CAPACITY_SCALING(G,lcap,cap,cost,supply,flow) ;
  double c2 = 0;
  forall_edges(e,G) c2 += cost[e]*flow[e];
  if (f2)
    cout << string("time: %6.2f sec c = %.2f",used_time(T),c2) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;

  cout << "cost scaling old:      " << flush;
  bool f3 = MCF_COST_SCALING_OLD(G,lcap,cap,cost,supply,flow) ;
  double c3 = 0;
  forall_edges(e,G) c3 += cost[e]*flow[e];
  if (f3)
    cout << string("time: %6.2f sec c = %.2f",used_time(T),c3) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;


  cout << "cost scaling:          " << flush;
  bool f4 = MCF_COST_SCALING(G,lcap,cap,cost,supply,flow) ;
  double c4 = 0;
  forall_edges(e,G) c4 += cost[e]*flow[e];
  if (f4)
    cout << string("time: %6.2f sec c = %.2f",used_time(T),c4) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;

  cout << endl;
  
  return 0;
}

