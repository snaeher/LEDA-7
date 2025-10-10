/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  list_impl.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:15 $


#include "list_impl.h"

list_impl::list_impl() : hd(0), sz(0) {}

list_impl::~list_impl() { clear(); }

void* list_impl::head() const 
{ return  hd->entry; }

void list_impl::push(void* x) 
{ hd = new list_impl_elem(x,hd); 
  sz++;
}

void* list_impl::pop() 
{ void* x =  hd->entry;
  list_impl_elem* p = hd;
  hd = p->succ;
  delete p;
  sz--;
  return x;
}

void list_impl::clear() 
{ while (hd) pop(); }

int  list_impl::size() const
{ return sz; }

