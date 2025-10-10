/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  flow_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/templates/max_flow.h>

#include <LEDA/graph/dimacs.h>
#include <LEDA/system/stream.h>
#include <assert.h>

using namespace leda;

using std::ofstream;
using std::cout;
using std::flush;
using std::endl;



/*
int Max_Flow(const graph& G, node s, node t,
             const edge_array<int>& cap, edge_array<int>& flow,
             int& pushes, int& inspects, int& relabels, int& updates,
             int& gap_nodes)
{

   max_flow<int,graph> mf;
   mf(G,s,t,cap,flow);

   pushes = mf.pushes();
   inspects = mf.inspections();
   relabels = mf.relabels();
   updates = mf.updates();
   gap_nodes = mf.gap_nodes();

   return mf.flow();
}
*/



int main() 
{
//int node_slots = 2;
  int node_slots = 3;
  int edge_slots = 1;

  GRAPH<int,int> G(node_slots,edge_slots);

  node s,t;

  int n = read_int("|V| = ");
  int m = read_int("|E| = ");

  edge_array<int>& cap = G.edge_data();

  for(int j=0; j<4; j++)
  { cout << endl;
    switch (j)
    { 
      case 0: cout << "Random: Random Networks     " << flush;
              max_flow_gen_rand(G,s,t,n,m);
              break;

      case 1: cout << "CG1: Cherkassky/Goldberg 1  " << flush;
              max_flow_gen_CG1(G,s,t,n);
              break;

      case 2: cout << "CG2: Cherkassky/Goldberg 2  " << flush;
              max_flow_gen_CG2(G,s,t,n);
              break;

      case 3: cout << "AMO: Ahuja/Magnanti/Orlin   " << flush;
              max_flow_gen_AMO(G,s,t,n);
              break;
     }


    edge_array<int> flow;
    flow.use_edge_data(G,0);
  
    int n = G.number_of_nodes();
    int m = G.number_of_edges();
    
    cout << "  (" << n << " nodes  " << m << " edges)" << endl;
    cout << endl;
  //cout << string("Saving problem to flow%d.dim (DIMACS format) ... ",j);
  //cout << flush;
    ofstream out(string("flow%d.dim",j));
    Write_Dimacs_MF(out,G,s,t,cap);
  //cout << endl;
    
    
    float T;
    int f1,f2;

    cout << "MAX_FLOW_T  " << flush;
    T = used_time();
    f1 = MAX_FLOW_T(G,s,t,cap,flow) ;
    cout << string("time: %6.2f sec  f = %d",used_time(T),f1) << endl;

    CHECK_MAX_FLOW(G,s,t,cap,flow);

  
    cout << "MAX_FLOW    " << flush;
    T = used_time();
    f2 = MAX_FLOW(G,s,t,cap,flow) ;
    cout << string("time: %6.2f sec  f = %d",used_time(T),f2) << endl;
    cout << endl;

    assert(f1 == f2);

    CHECK_MAX_FLOW(G,s,t,cap,flow);

 }
    
 return 0;
}
