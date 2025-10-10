/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mcf_cost_scaling.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

class opposite_graph {};

#include <LEDA/graph/templates/mcf_cost_scaling.h>


LEDA_BEGIN_NAMESPACE

typedef int NT;

bool MCF_COST_SCALING_OLD(graph& G, const edge_array<NT>& lcap,
                                    const edge_array<NT>& ucap,
                                    const edge_array<NT>& cost,
                                    const node_array<NT>& supply,
                                    edge_array<NT>&       flow)
{
  typedef mcf_cost_scaling<int,double,double>::node_data nd_type;
  typedef mcf_cost_scaling<int,double,double>::edge_data ed_type;

  typedef node_array<nd_type> node_data_array;
  typedef edge_array<ed_type> edge_data_array;

  mcf_cost_scaling<int,double,double, graph, node_data_array, 
                                             edge_data_array> mcf;

  bool feasible = mcf.run(G,lcap,ucap,cost,supply,flow) ;

  if (feasible) 
  { string msg;
    if (!mcf.check(G,lcap,ucap,cost,supply,flow,msg)) LEDA_EXCEPTION(1,msg);
  }

  return feasible;
}




NT MCMF_COST_SCALING_OLD(graph& G, node s, node t, 
                                     const edge_array<NT>& cap, 
                                     const edge_array<NT>& cost, 
                                     edge_array<NT>& flow)
{ edge_array<NT> lcap(G,0);
  node_array<NT> supply(G,0);
  NT f = MAX_FLOW(G,s,t,cap,flow);
  supply[s] =  f;
  supply[t] = -f;
  MCF_COST_SCALING_OLD(G,lcap,cap,cost,supply,flow); 
  return f;
 }

LEDA_END_NAMESPACE
