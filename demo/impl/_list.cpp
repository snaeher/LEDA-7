/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  _list.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:17 $


#include "_list.h"
#define NULL 0

int list::head() const
{ return hd->entry; }

void list::push(int x)
{ hd = new list_elem(x,hd); 
  sz++;
 }

void list::pop(int& y)
{ y = hd->entry;
  list_elem* p = hd;
  hd = p->succ;
  delete p;
  sz--;
 }

list::list()
{ // construct an empty list
  hd = NULL;
  sz = 0; 
 }

list::list(const list& L)
{ // construct a copy of L
  hd = NULL;
  sz = L.sz; 
  if (sz > 0)
  { hd = new list_elem(L.hd->entry,0); // first element
    list_elem* q = hd;
    // subsequent elements
    for (list_elem* p = L.hd->succ; p != NULL; p = p->succ)
    { q->succ = new list_elem(p->entry,NULL);
      q = q->succ;
     }
   }
 }

list::~list()
{ // destroy the list
  while (hd)
  { list_elem* p = hd->succ;
    delete hd;
    hd = p;
   }
 }
