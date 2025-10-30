/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _m_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/m_heap.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

m_heap::m_heap(int m)
{ if (m<=0) LEDA_EXCEPTION(1,string("m_heap constructor: illegal size = %d",m));
  M = m;
  T = new m_heap_elem[m];
  T_last = T+m-1;
  count = 0;
  min_ptr = T;
  min_key = 0;
  for (m_heap_item p = T; p <= T_last; p++) 
  { p->inf = this;
    p->succ = p+1;
    p->pred = p-1;
   }
  T->pred = T_last;
  T_last->succ = T;
}


void m_heap::clear()
{ count = 0;
  min_ptr = T;
  min_key = 0;
  for (m_heap_item p = T; p <= T_last; p++) 
  { p->inf = this;
    p->succ = p+1;
    p->pred = p-1;
   }
  T->pred = T_last;
  T_last->succ = T;
}


m_heap_item m_heap::first_item() const
{ return 0;
 }
  
m_heap_item m_heap::next_item(m_heap_item) const
{ return 0; }


void m_heap::insert(m_heap_item it, m_heap_elem& p)
{ m_heap_item s = p.succ; 
  it->succ = s;
  it->pred = &p;
  p.succ = it;
  s->pred = it;
}

void m_heap::remove(m_heap_item it)
{ assert(it->inf != this);
  m_heap_item s = it->succ; 
  m_heap_item p = it->pred; 
  p->succ = s;
  s->pred = p;
}



m_heap_item m_heap::insert(GenPtr kk, GenPtr inf)   // int key
{ 
  int key = LEDA_ACCESS(int,kk);

  int k = key-min_key; 

  if (count > 0 && (k < 0 || k >= M)) 
  LEDA_EXCEPTION(1,string("m_heap insert: illegal key %d %d",key,k));

  m_heap_item it = new m_heap_elem;
  copy_inf(inf);
  it->inf = inf;

  if (count == 0)
  { min_ptr = T;
    min_key = key;
    count = 1;
    insert(it,T[0]); 
    return it;
   }

  count++;
  insert(it,T[((min_ptr-T) + k) % M]); 

  return it;
}


m_heap_item m_heap::find_min() const 
{ assert(count > 0);
  return (count > 0) ? min_ptr->succ : 0; }


GenPtr m_heap::del_min()
{ 
  if (count == 0)  LEDA_EXCEPTION(1,"m_heap del_min: empty heap");

  m_heap_item p = find_min();
  GenPtr inf = p->inf;
  remove(p);
  delete p;

  if (--count > 0)
  { for(p = min_ptr; p->succ->inf == (GenPtr)this; p = p->succ) min_key++;
    min_ptr = p;
  }


  return inf;
}

void m_heap::del_item(m_heap_item it) 
{ if (it == min_ptr->succ) 
     del_min();
  else
   { GenPtr inf = it->inf;
     clear_inf(inf);
     remove(it);
     delete it;
     count--;
    }
}
 

void m_heap::change_key(m_heap_item it, GenPtr kk)  
{ int key = LEDA_ACCESS(int,kk);
  remove(it);
  int k = key - min_key;
  if (k<0 || k>=M) 
    LEDA_EXCEPTION(1,string("m_heap change key: illegal key %d\n",key));
  insert(it,T[((min_ptr-T) + k) % M]); 
}


void m_heap::print() const { }

LEDA_END_NAMESPACE
