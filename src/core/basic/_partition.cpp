/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _partition.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
//
// partitions
//
// implementation: union find with weighted union rule and path compression
//
// S. N.  (1989)
//------------------------------------------------------------------------------

#include <LEDA/core/partition.h>

LEDA_BEGIN_NAMESPACE

void partition::clear()
{ // free all used items
  partition_item p = used_items; 
  while (used_items)
  { p = used_items;
    used_items = used_items->next;
    delete p;
   }
  block_count = 0;
 }
  


partition_item partition::find(partition_item y)  const
{ // find with path compression

  partition_item x = y->father;

  if (x==0) return y;

  partition_item root = y;

  while (root->father) root = root->father;

  while (x!=root)
  { y->father = root;
    y = x;
    x = y->father;
   }
  return root;
 }

void partition::union_blocks(partition_item a, partition_item b)
{ // weighted union

  a = find(a);
  b = find(b);

  if (a==b) return;

  block_count--;

  if (a->size > b->size)
       { b->father = a;
         a->size += b->size; }
  else { a->father = b;
         b->size += a->size; }

 }

void partition::reset(partition_item p)
{ 
  if (p->father) {
    p->father = nil; 
    block_count++;
  }
  p->size = 1;
}

void partition::split(const list<partition_item>& L)
{ partition_item p;
  forall(p,L) reset(p);
 }
 
LEDA_END_NAMESPACE
