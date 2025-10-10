/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_static.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/static_graph.h>

LEDA_BEGIN_NAMESPACE

static_graph::static_graph() : V(0), E(0),num_V(0),num_E(0) { }


static_graph::static_graph(const graph& G)
{
   num_V = G.number_of_nodes();
   num_E = G.number_of_edges();

   V = new static_node[num_V+1];
   V_stop = V+num_V;

   E = new static_edge[num_E+1];
   E_stop = E+num_E;

   static_node* pv = V;
   static_edge* pe = E;

   n_vec.init(G,nil);
   e_vec.init(G,nil);

   node_struct* v; 
   forall_nodes(v,G) n_vec[v] = pv++;

   forall_nodes(v,G)
   { static_node* pv = n_vec[v];
     pv->adj_start = pe;
     edge_struct* e;
     forall_adj_edges(e,v)
     { pe->target = n_vec[G.target(e)];
       e_vec[e] = pe++;
      }
     pv->adj_stop = pe;
   }
}




list_item static_graph::register_map(graph_map<static_graph>*, int& i, bool)
{ i = -1; return nil; }

void static_graph::unregister_map(graph_map<static_graph>*) {}

LEDA_END_NAMESPACE
