/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_objects2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

node_struct::node_struct(GenPtr inf, int id) 
{ ID = id; 
  Data[0] = inf; 
  FirstAdjEdge[0]= nil;
  FirstAdjEdge[1]= nil;
}

edge_struct::edge_struct(node v, node w, GenPtr i, int id)
{ ID = id;
  Data[0] = i;
  SuccAdjEdge[0] = nil;
  SuccAdjEdge[1] = nil;
  term[0] = v;
  term[1] = w;
}

edge_struct::edge_struct(GenPtr i, int id)
{ ID = id;
  Data[0] = i;
  SuccAdjEdge[0] = nil;
  SuccAdjEdge[1] = nil;
}


void node_struct::clear_adj_lists()
{ FirstAdjEdge[0] = nil;
  FirstAdjEdge[1] = nil;
}


   

void node_struct::push_adj_edge(edge e, int i, int chain_e)
{ edge e1 = FirstAdjEdge[i];
  FirstAdjEdge[i] = e;
  e->SuccAdjEdge[chain_e] = e1;
 }



// expensive (without last-pointer)

void node_struct::append_adj_list(int i, node v, int j)
{ // append adj_list[j] of v to adj_list[i] of this node
  // use succ[i] and pred[j] pointers for chaining

  edge first= v->FirstAdjEdge[j];

  if (first == nil) return;

  edge last = FirstAdjEdge[i];

  if (last)
  { while (last->SuccAdjEdge[i]) last = last->SuccAdjEdge[i];
    last->SuccAdjEdge[i] = first;
   }
  else
    FirstAdjEdge[i] = first;

  v->FirstAdjEdge[j] = 0;
}
   




// expensive (singly linked lists)

void node_struct::insert_adj_edge(edge e, edge e1, int i, int chain_e, int dir)
{ 
  // insert e after (dir==0) or before (dir!=0) e1 into adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining

  if (dir != 0) not_implemented("insert_adj_edge before given edge");


  if (e1 == nil) 
  { push_adj_edge(e,i,chain_e);
    return;
   }

  int  chain_e1; // chaining used for e1

  if (source(e1) == target(e1)) // e1 is a self-loop
      chain_e1 = chain_e;
  else
      chain_e1 = (this == source(e1)) ? 0 : 1;

  edge e2 = e1->SuccAdjEdge[chain_e1];
  e->SuccAdjEdge[chain_e] = e2;
  e1->SuccAdjEdge[chain_e1] = e;
}


void node_struct::del_adj_edge(edge e, int i, int chain_e)
{ edge x = FirstAdjEdge[i];
  edge e_succ = e->SuccAdjEdge[chain_e];
  edge e_pred = nil;
  while (x != e)
  { e_pred = x;
    x = x->SuccAdjEdge[chain_e];
   }

  if (e_pred) e_pred->SuccAdjEdge[chain_e] = e_succ;

  if (e == FirstAdjEdge[i]) FirstAdjEdge[i] = e_succ;;
}


// not implemented

void node_struct::append_adj_edge(edge e,int i, int chain_e)
{ not_implemented("node_struct::append_adj_edge(e,i,chain_e)"); }


void node_struct::insert_adj_list(int i, edge e1, node v, int j)
{ not_implemented("insert_adj_list"); }

LEDA_END_NAMESPACE
