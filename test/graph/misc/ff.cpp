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



bool feasible_flow(graph& G, node_array<int>& supply, 
                             edge_array<int>& lcap,
                             edge_array<int>& ucap,
                             edge_array<int>& flow)
{


  node_map<int> excess(G,0);
  edge_map<int> cap(G,0);

  node v;
  forall_nodes(v,G) excess[v] = supply[v];

  edge e;
  forall_edges(e,G)
  { int lc = lcap[e];
    node v = G.source(e);
    node w = G.target(e);
    cap[e] = ucap[e] - lc;
    excess[v] -= lc;
    excess[w] += lc;
  }

  node s = G.new_node();
  node t = G.new_node();

  forall_nodes(v,G)
  { if (v == s || v == t) continue;
    int ev = excess[v];
    if (ev > 0) 
    { edge x = G.new_edge(s,v);
      cap[x] = ev;
      excess[s] += ev;
     }
    if (ev < 0) 
    { edge y = G.new_edge(v,t);
      cap[y] = -ev;
      excess[t] += ev;
     }
   }

   if (excess[s] + excess[t] != 0) return false;

   edge_array<int> flow_max(G,0);
   int f = MAX_FLOW(G,s,t,cap,flow_max);

   bool feasible = (f == excess[s]);

   G.del_node(s);
   G.del_node(t);

   if (feasible)
   { edge e;
     forall_edges(e,G) flow[e] = flow_max[e] + lcap[e];
    }

   return feasible;
}

void check_ff(const graph& G, const node_array<int>& supply,
                              const edge_array<int>& lcap,
                              const edge_array<int>& ucap,
                              const edge_array<int>& flow)
{
  node_array<int> excess(G);

  node v;
  forall_nodes(v,G) excess[v] = supply[v];

  edge e;
  forall_edges(e,G)
  { assert(flow[e] >= lcap[e] && flow[e] <= ucap[e]);
    node v = G.source(e);
    node w = G.target(e);
    excess[v] -= flow[e];
    excess[w] += flow[e];
  }

  forall_nodes(v,G) assert(excess[v] == 0);
}


int main(int argc, char** argv)
{
  graph G(5,4);
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

  edge_array<int> lcap(G,0);
  //assert(lcap.use_edge_data(G,0));

  edge_array<int> flow;
  assert(flow.use_edge_data(G,0));

  node_array<int> supply(G,0);
  //assert(supply.use_node_data(G,0));

  node s,t;


  if (argc > 1)
  { ifstream in(argv[1]);
    Read_Dimacs_MCF(in,G,supply,lcap,cap,cost);
   }
  else
  { s = G.first_node();
    t = G.last_node();
  
    edge e;
    forall_edges(e,G)  cap[e]  = rand_int(U/2,U);
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

  
{
  float T = used_time();

  cout << "feasible_flow :        " << flush;
  bool f = feasible_flow(G,supply,lcap,cap,flow) ;

  if (f) check_ff(G,supply,lcap,cap,flow);

  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}


{
  
  float T = used_time();

  cout << "FEASIBLE_FLOW :        " << flush;
  bool f = FEASIBLE_FLOW(G,supply,lcap,cap,flow) ;

  if (f) check_ff(G,supply,lcap,cap,flow);

  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}

/*
{
  
  float T = used_time();

  cout << "FEASIBLE_FLOW_OLD :    " << flush;
  bool f = FEASIBLE_FLOW_OLD(G,supply,lcap,cap,flow) ;

  if (f) check_ff(G,supply,lcap,cap,flow);

  double c = 0;
  edge e;
  forall_edges(e,G) c += cost[e]*flow[e];
  if (f)
    cout << string("time: %6.2f sec c = %f",used_time(T),c) << endl;
  else
    cout << string("time: %6.2f sec. Not feasible.",used_time(T)) << endl;
}
*/

  return 0;
}

