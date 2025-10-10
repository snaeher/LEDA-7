/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_objects1.c
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
  for(int j=0; j<2; j++)
  { FirstAdjEdge[j]= nil;
    //LastAdjEdge[j] = nil;
    //AdjLength[j] = 0;
  }
}

edge_struct::edge_struct(node v, node w, GenPtr i, int id)
{ SuccAdjEdge[0] = nil;
  SuccAdjEdge[1] = nil;
  ID = id;
  term[0] = v;
  term[1] = w;
  Data[0] = i;
}


void node_struct::clear_adj_lists()
{ FirstAdjEdge[0] = nil;
  FirstAdjEdge[1] = nil;
  //LastAdjEdge[0] = nil;
  //LastAdjEdge[1] = nil;
  //AdjLength[0] = 0;
  //AdjLength[1] = 0;
}


void node_struct::append_adj_edge(edge e,int i, int chain_e)
{ // append e to adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining of e

  not_implemented("node_struct::append_adj_edge(e,i,chain_e)");

/*
  edge last = LastAdjEdge[i];

  e->SuccAdjEdge[chain_e] = nil;

  if (last == 0) // empty list
    FirstAdjEdge[i] = e;
  else
  { if (source(last) == target(last))  // loop
       last->SuccAdjEdge[chain_e] = e;
    else
       last->SuccAdjEdge[(this==source(last)) ? 0:1] = e;
   }

  LastAdjEdge[i] = e;
  //AdjLength[i]++;
*/
}

   

void node_struct::push_adj_edge(edge e, int i, int chain_e)
{ edge e1 = FirstAdjEdge[i];
  FirstAdjEdge[i] = e;
  e->SuccAdjEdge[chain_e] = e1;
  //AdjLength[i]++;
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

  //AdjLength[i] +=  v->AdjLength[j];

  v->FirstAdjEdge[j] = 0;
  //v->AdjLength[j] = 0;
}
   



// expensive on singly linked lists

void node_struct::insert_adj_edge(edge e, edge e1, int i, int chain_e, int dir)
{ 
  // insert e after (dir==0) or before (dir!=0) e1 into adj_list[i]
  // use succ/PredAdjEdge[chain_e] pointers for chaining

  if (dir != 0) not_implemented("insert_adj_edge before given edge");


  if (e1 == nil) 
  { //append_adj_edge(e,i,chain_e);
    push_adj_edge(e,i,chain_e);
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

//if (e2 == nil) LastAdjEdge[i] = e;

  //AdjLength[i]++;
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

//if (e == LastAdjEdge[i]) LastAdjEdge[i] = e_pred;
  if (e == FirstAdjEdge[i]) FirstAdjEdge[i] = e_succ;;

  //AdjLength[i]--;
}


void node_struct::insert_adj_list(int i, edge e1, node v, int j)
{ not_implemented("insert_adj_list"); }




/*
void graph_obj_list_base::insert(graph_object* x, graph_object* y) 
{ // insert x after y
  x->obj_list_succ = y->obj_list_succ;
  y->obj_list_succ = x;
  if (y == obj_list_tail) obj_list_tail = x;
  obj_list_sz++;
 }

void graph_obj_list_base::remove(graph_object* obj)
{ graph_object* x = obj_list_head;
  graph_object* obj_succ = obj->obj_list_succ;
  graph_object* obj_pred = nil;
  while (x != obj)
  { obj_pred = x;
    x = x->obj_list_succ;
   }

  if (obj_pred) obj_pred->obj_list_succ = obj_succ;

  if (obj == obj_list_tail) obj_list_tail = obj_pred;
  if (obj == obj_list_head) obj_list_head = obj_succ;

  obj_list_sz--;
}
*/

LEDA_END_NAMESPACE


