/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _list_pq.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/list_pq.h>

LEDA_BEGIN_NAMESPACE

list_pq::list_pq(const list_pq&) { /* copy constructor */}

list_pq& list_pq::operator=(const list_pq&) { /* assignment */ return *this; }


list_pq_item list_pq::insert(GenPtr k,GenPtr i) 
{ copy_key(k);
  copy_inf(i); 
  list_pq_item p = new list_pq_elem(k,i,nil,head);
  if (head) head->pred = p;
  head = p;
  count++;
  return p; 
}

list_pq_item list_pq::find_min() const  
{ list_pq_item p = head;
  list_pq_item m = head;
  if (key_type_id() == INT_TYPE_ID)
     while (p)
     { if (LEDA_ACCESS(int,p->key) < LEDA_ACCESS(int,m->key)) m = p;
       p = p->succ;
      }
  else
     while (p)
     { if (cmp(p->key,m->key) < 0) m = p;
       p = p->succ;
      }
  return m;
 }


void list_pq::del_item(list_pq_item it)     
{ list_pq_item p = it->pred;
  list_pq_item s = it->succ;
  if (p) p->succ = s;
  else   head = s;
  if (s) s->pred = p;
  count--;
 }


void list_pq::clear()   
{ while (head)
  { list_pq_item p = head->succ;
    clear_key(head->key);
    clear_inf(head->inf);
    delete head;
    head = p;
   }
  count = 0;
 }

LEDA_END_NAMESPACE
