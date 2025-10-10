/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mcft1.c
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
using std::ifstream;
using std::cout;
using std::flush;
using std::endl;



int main(int argc, char** argv)
{
  graph G(6,5);
  int n,m;

  if (argc == 1)
  { n = read_int("n = ");
    m = read_int("m = ");
    //rand_int.set_seed(m+17*n);
    random_graph(G,n,m);
    Delete_Loops(G);
    Make_Simple(G);
   }

  int U = 1000;
  int Cmin = 1;
  int Cmax = 1000;

  edge_array<int> cap;
  assert(cap.use_edge_data(G,0));

  edge_array<int> cost;
  assert(cost.use_edge_data(G,0));

  edge_array<int> lcap;
  assert(lcap.use_edge_data(G,0));

  edge_array<int> flow;
  assert(flow.use_edge_data(G,0));

  node_array<int> supply;
  assert(supply.use_node_data(G,0));

  node s,t;


  if (argc > 1)
  { ifstream in(argv[1]);
    Read_Dimacs_MCF(in,G,supply,lcap,cap,cost);
   }
  else
  { s = G.first_node();
    t = G.last_node();
  
    edge e;
    forall_edges(e,G) cap[e]  = rand_int(U/2,U);
    forall_edges(e,G) cost[e] = rand_int(Cmin,Cmax);
  
    edge_array<int> flow0(G,0);
    MAX_FLOW(G,s,t,cap,flow0) ;

    forall_edges(e,G) lcap[e]  = flow0[e]/2;
  
    forall_out_edges(e,s) 
    { supply[target(e)] += flow0[e];
      lcap[e] = 0;
     }

    forall_in_edges(e,t) 
    { supply[source(e)]  -= flow0[e];
      lcap[e] = 0;
     }

    ofstream out("mcflow.dim");
    Write_Dimacs_MCF(out,G,supply,lcap,cap,cost);
  }


  
  float T = used_time();

{
  cout << "capacity scaling : " << flush;
  bool f = MCF_CAPACITY_SCALING(G,lcap,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %d",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}


{
  cout << "primal dual :      " << flush;
  bool f = MCF_PRIMAL_DUAL(G,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %d",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}



  return 0;
}

