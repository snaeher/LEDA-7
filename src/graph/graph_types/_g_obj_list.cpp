/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_obj_list.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_obj_list.h>

LEDA_BEGIN_NAMESPACE

void graph_obj_list_base::clear()
{ obj_list_head = 0; 
  obj_list_tail = 0; 
  obj_list_sz   = 0;
}

void graph_obj_list_base::push(graph_object* e)
{ 
  e->obj_list_pred = 0;

  if (obj_list_sz > 0)
     obj_list_head->obj_list_pred = e;
  else
     obj_list_tail = e;

  e->obj_list_succ = obj_list_head;
  obj_list_head = e;

  obj_list_sz++;
}



void graph_obj_list_base::append(graph_object* e)
{ 
  e->obj_list_succ = 0;

  if (obj_list_sz > 0)
     obj_list_tail->obj_list_succ = e;
  else
     obj_list_head = e;

  e->obj_list_pred = obj_list_tail;
  obj_list_tail = e;

  obj_list_sz++;
}


void graph_obj_list_base::insert(graph_object* e, graph_object* s)
{ 
  if (s == nil)
  { append(e);
    return;
   }

  graph_object* p = s->obj_list_pred;

  e->obj_list_succ = s;
  s->obj_list_pred = e;

  e->obj_list_pred = p;

  if (p)
     p->obj_list_succ= e;
  else
     obj_list_head = e;

  obj_list_sz++;
}



graph_object* graph_obj_list_base::pop()
{ graph_object* e = obj_list_head;
  if (e)
  { graph_object* s = e->obj_list_succ;
    obj_list_head = s;
    if (s)
       s->obj_list_pred = 0;
    else
       obj_list_tail = 0;
    obj_list_sz--;
   }
  return e;
}


void graph_obj_list_base::remove(graph_object* e)
{ 
  graph_object* s = e->obj_list_succ;
  graph_object* p = e->obj_list_pred;

  if (s)
    { e->obj_list_succ = s->obj_list_succ;
      s->obj_list_pred = p; }
  else
      obj_list_tail = p;

  if (p)
    { e->obj_list_pred = p->obj_list_pred;
      p->obj_list_succ = s; }
  else
      obj_list_head = s;

  obj_list_sz--;
}


void graph_obj_list_base::conc(graph_obj_list_base& L)
{ if (L.obj_list_sz ==0) return; 

  if (obj_list_sz > 0)
     obj_list_tail->obj_list_succ = L.obj_list_head;
  else
     obj_list_head = L.obj_list_head;
  L.obj_list_head->obj_list_pred = obj_list_tail;
  obj_list_tail = L.obj_list_tail;
  obj_list_sz += L.obj_list_sz;
  L.clear();
}

LEDA_END_NAMESPACE
