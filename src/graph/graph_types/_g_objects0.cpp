/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_objects0.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>


//------------------------------------------------------------------------------
// nodes and edges
//
// by S. Naeher  (1995)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

node_struct::node_struct(GenPtr inf, int id) 
{ ID = id; 
  Data[0] = inf; 
  succ_link = nil;
  for(int j=0; j<2; j++)
  { FirstAdjEdge[j]= nil;
    LastAdjEdge[j] = nil;
    AdjLength[j] = 0;
  }
}

edge_struct::edge_struct(node v, node w, GenPtr i, int id)
{ SuccAdjEdge[0] = nil;
  SuccAdjEdge[1] = nil;
  PredAdjEdge[0] = nil;
  PredAdjEdge[1] = nil;
  ID = id;
  term[0] = v;
  term[1] = w;
  rev = nil;
  Data[0] = i;
}


void node_struct::clear_adj_lists()
{ FirstAdjEdge[0] = nil;
  FirstAdjEdge[1] = nil;
  LastAdjEdge[0] = nil;
  LastAdjEdge[1] = nil;
  AdjLength[0] = 0;
  AdjLength[1] = 0;
}


void node_struct::append_adj_edge(edge e,int i, int chain_e)
{ // append e to adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining of e

  edge last = LastAdjEdge[i];

  e->SuccAdjEdge[chain_e] = nil;

  if (last == 0) // empty list
  { FirstAdjEdge[i] = e;
    e->PredAdjEdge[chain_e] = nil;
   }
  else
  { e->PredAdjEdge[chain_e] = last;
    if (source(last) == target(last))  // loop
       last->SuccAdjEdge[chain_e] = e;
    else
       last->SuccAdjEdge[(this==source(last)) ? 0:1] = e;
   }

  LastAdjEdge[i] = e;
  AdjLength[i]++;
 }


/*
void node_struct::push_adj_edge(edge e,int i, int chain_e)
{ // insert e at the beginning of adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining of e

  edge first = FirstAdjEdge[i];

  e->PredAdjEdge[chain_e] = nil;

  if (first == 0) // empty list
  { LastAdjEdge[i] = e;
    e->SuccAdjEdge[chain_e] = nil;
   }
  else
  { first->PredAdjEdge[chain_e] = e;
    if (source(e) == target(e)) // loop
      e->SuccAdjEdge[chain_e] = first;
    else
      e->SuccAdjEdge[(this==source(e)) ? 0:1] = first;
   }

  FirstAdjEdge[i] = e;
  AdjLength[i]++;
 }
*/

   

void node_struct::push_adj_edge(edge e, int i, int chain_e)
{ edge e1 = FirstAdjEdge[i];
  insert_adj_edge(e,e1,i,chain_e,1);
 }


void node_struct::insert_adj_edge(edge e, edge e1, int i, int chain_e, int dir)
{ 
  // insert e after (dir==0) or before (dir!=0) e1 into adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining


  if (e1 == nil) 
  { append_adj_edge(e,i,chain_e);
    //push_adj_edge(e,i,chain_e);
    return;
   }


  edge e2;       // successor (dir==0) or predecessor (dir!=0) of e1

  int  chain_e1; // chaining used for e1


  if (source(e1) == target(e1)) // e1 is a self-loop
      chain_e1 = chain_e;
  else
      chain_e1 = (this == source(e1)) ? 0 : 1;

  if (dir == 0)
  { e2 = e1->SuccAdjEdge[chain_e1];
    e->PredAdjEdge[chain_e] = e1;
    e->SuccAdjEdge[chain_e] = e2;
    e1->SuccAdjEdge[chain_e1] = e;
    if (e2 == nil) 
       LastAdjEdge[i] = e;
    else
     { if (source(e2) == target(e2)) //loop
          e2->PredAdjEdge[chain_e] = e;
       else
          e2->PredAdjEdge[(this==source(e2)) ? 0:1] = e;
      }
   }
  else
  { e2 = e1->PredAdjEdge[chain_e1];
    e->SuccAdjEdge[chain_e] = e1;
    e->PredAdjEdge[chain_e] = e2;
    e1->PredAdjEdge[chain_e1] = e;
    if (e2 == nil) 
       FirstAdjEdge[i] = e;
    else
     { if (source(e2) == target(e2)) //loop
          e2->SuccAdjEdge[chain_e] = e;
       else
          e2->SuccAdjEdge[(this==source(e2)) ? 0:1] = e;
      }
   }

   AdjLength[i]++;
}

void node_struct::append_adj_list(int i, node v, int j)
{ // append adj_list[j] of v to adj_list[i] of this node
  // use succ[i] and pred[j] pointers for chaining

  edge first= v->FirstAdjEdge[j];

  if (first == nil) return;

  if (LastAdjEdge[i])
    LastAdjEdge[i]->SuccAdjEdge[i] = first;
  else
    FirstAdjEdge[i] = first;

  v->FirstAdjEdge[j]->PredAdjEdge[j] = LastAdjEdge[i];
  LastAdjEdge[i] = v->LastAdjEdge[j];

  AdjLength[i] +=  v->AdjLength[j];

  v->FirstAdjEdge[j] = 0;
  v->LastAdjEdge[j] = 0;
  v->AdjLength[j] = 0;
}
   

void node_struct::insert_adj_list(int i, edge e1, node v, int j)
{ 
  // insert adj_list[j] of v after e1 into adj_list[i] of this node
  // use succ[i] and pred[j] pointers for chaining

  if (e1 == nil) 
  { append_adj_list(i,v,j);
    return;
   }

  edge first= v->FirstAdjEdge[j];
  edge last = v->LastAdjEdge[j];

  edge e1_succ = e1->SuccAdjEdge[i];

  e1->SuccAdjEdge[i] = first;
  first->PredAdjEdge[j] = e1; 
  last->SuccAdjEdge[i] = e1_succ; 
  e1_succ->PredAdjEdge[j] = last;

  AdjLength[i] +=  v->AdjLength[j];

  v->FirstAdjEdge[j] = 0;
  v->LastAdjEdge[j] = 0;
  v->AdjLength[j] = 0;
}


  
  
void node_struct::del_adj_edge(edge e, int i, int chain_e)
{ 
  // remove e from adj_list[i] 
  // with respect to succ/PredAdjEdge[chain_e] pointers

  edge e_succ = e->SuccAdjEdge[chain_e];
  edge e_pred = e->PredAdjEdge[chain_e];

  if (e_succ) 
      if (source(e_succ) == target(e_succ)) // loop
         e_succ->PredAdjEdge[chain_e] = e_pred;
      else
         e_succ->PredAdjEdge[(this==source(e_succ)) ? 0:1 ] = e_pred;
  else
      LastAdjEdge[i] = e_pred;

  if (e_pred) 
      if (source(e_pred) == target(e_pred)) // loop
         e_pred->SuccAdjEdge[chain_e] = e_succ;
      else
         e_pred->SuccAdjEdge[(this==source(e_pred)) ? 0:1 ] = e_succ;
  else
      FirstAdjEdge[i] = e_succ;

  AdjLength[i]--;
 }


// moved from graph_objects.h (for testing if undirected) s.n.


edge_struct* node_struct::first_out_edge() const
{ 
  if (owner()->is_undirected())
     LEDA_EXCEPTION(1,"forall_out_edges: graph must be directed.");

   return first_adj_edge(0); 
}


edge_struct* node_struct::first_in_edge() const
{ 
  if (owner()->is_undirected())
     LEDA_EXCEPTION(1,"forall_in_edges: graph must be directed.");

  return first_adj_edge(1); 
}


bool First_Inout_Edge(edge_struct*& e, node_struct* v)
{
  if (v->owner()->is_undirected())
     LEDA_EXCEPTION(1,"forall_inout_edges: graph must be directed.");

  e = v->first_adj_edge(0); // first_out_edge
  if (e) return true;
  e = v->first_adj_edge(1); // first_in_edge
  return false;
}


LEDA_END_NAMESPACE
