/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mcft.c
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
  graph G;

  int n,m;

  if (argc == 1)
  { n = read_int("n = ");
    m = read_int("m = ");
    //rand_int.set_seed(m+17*n);
    random_graph(G,n,m);
    Delete_Loops(G);
    Make_Simple(G);
   }

  edge_array<int> cap(G,0);
  edge_array<int> cost(G,0);
  edge_array<int> lcap(G,0);
  node_array<int> supply(G,0);



  if (argc > 1)
  { ifstream in(argv[1]);
    Read_Dimacs_MCF(in,G,supply,lcap,cap,cost);
   }
  else
  { cout << "Capacities in [0 .. U]" << endl;
    int U = read_int("U = ");
    cout << endl;

    cout << "Edge costs in [Cmin .. Cmax]" << endl;
    int Cmin = read_int("Cmin = ");
    int Cmax = read_int("Cmax = ");
    cout << endl;

    node s = G.first_node();
    node t = G.last_node();
  
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


  edge_array<int> flow(G,0);
  //assert(flow.use_edge_data(G,0));


  
  float T = used_time();

{ cout << "FEASIBLE_FLOW :        " << flush;
  bool f = FEASIBLE_FLOW(G,supply,lcap,cap,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}

{
  cout << "MIN_COST_FLOW :        " << flush;
  bool f = MIN_COST_FLOW(G,lcap,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}


{
  cout << "COST_SCALING :         " << flush;
  bool f = MCF_COST_SCALING(G,lcap,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}


{
  cout << "COST_SCALING_OLD :     " << flush;
  bool f = MCF_COST_SCALING_OLD(G,lcap,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}

{ cout << "CAPACITY_SCALING :     " << flush;
  bool f = MCF_CAPACITY_SCALING(G,lcap,cap,cost,supply,flow) ;
  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}

  return 0;
}

