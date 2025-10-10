/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  t_list_test.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:17 $



#include "t_list.h"
#define NULL 0

template <class E> t_list<E>::t_list()
{ hd = NULL;
  sz = 0; 
 }

template <class E> const E& t_list<E>::head() const
{ return hd->entry; }

template <class E> void t_list<E>::push(const E& x)
{ hd = new list_elem(x,hd); 
  sz++;
 }

template <class E> void t_list<E>::pop(E& y)
{ y = hd->entry;
  list_elem* p = hd;
  hd = p->succ;
  delete p;
  sz--;
}


int main()
{ t_list<int> L;
  L.push(0);
  return 0;
}

