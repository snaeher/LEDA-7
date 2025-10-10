/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  _f_heap.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:17 $


#include <LEDA/system/basic.h>
#include "f_heap.h"

using namespace leda;


f_heap::f_heap()
{ number_of_nodes = 0;
  power = 1;
  logp = 0;
  minptr = nil;
  node_list = nil;
} 


f_heap_item f_heap::find_min()         const { return minptr; }
int         f_heap::size()             const 
            { return number_of_nodes; }

bool        f_heap::empty()            const 
            { return number_of_nodes == 0; } 

GenPtr      f_heap::key(f_heap_item x) const { return x->key; }
GenPtr      f_heap::inf(f_heap_item x) const { return x->inf; }

void f_heap::change_inf(f_heap_item x, GenPtr i)
{ clear_inf(x->inf);
  copy_inf(i);
  x->inf = i;
}


f_heap_item f_heap::insert(GenPtr k, GenPtr i) 
{                            
  k = copy_key(k);
  i = copy_inf(i);

  f_heap_item new_item = new f_heap_node(k,i,node_list);

  if ( number_of_nodes == 0 )
  { // insertion into empty queue 
    minptr = new_item;
    // build trivial circular list
    new_item->right = new_item; 
    new_item->left = new_item;
    // power and logp have already the correct value
  }
  else                                    
  { // insertion into non-empty queue; 
    // we first add to the list of roots 
    new_item->left = minptr;      
    new_item->right = minptr->right;
    minptr->right->left = new_item;
    minptr->right = new_item;
    if ( cmp(k,minptr->key) < 0 ) // new minimum
       minptr = new_item;
    if ( number_of_nodes >= power) 
    { // log number_of_nodes grows by one
      power = power * 2;
      logp = logp + 1;
    }
  }

  number_of_nodes++;

  return new_item;
} 


void f_heap::del_min()
{ // removes the item pointed to by minptr

  if ( minptr == nil ) 
    error_handler(1,"f_heap: deletion from empty heap"); 

  number_of_nodes--;

  if ( number_of_nodes==0 ) 
  { // removal of the only node
    // power and logp do not have to be changed.
    clear_key(minptr->key);
    clear_inf(minptr->inf);
    delete minptr;
    minptr = nil;
    node_list = nil;
    return;
   }

  /* removal from a queue with more than one item. */

  
  if ( 2 * number_of_nodes <= power )
  { power = power / 2;
    logp = logp - 1;
  }

  f_heap_item r1 = minptr->right;
  f_heap_item r2 = minptr->child;

  if ( r2 )
  { // minptr has children 
    while ( r2->parent )       
    { //  visit them all and make them roots 
      r2->parent = nil;
      r2->marked = false;
      r2 = r2->right;
    }    

    // combine the lists, i.e. cut r2's list between r2 and its left
    // neighbor and splice r2 to minptr and its left neighbor to r1 

    r2->left->right = r1;
    r1->left = r2->left;
    minptr->right = r2;
    r2->left = minptr;
  }
;
  
  f_heap_item rank_array[12*sizeof(int)];
  for (int i = (int)1.5*logp; i >= 0; i--) rank_array[i] = nil;

  f_heap_item new_min = minptr->right;
  f_heap_item current = new_min; 

  while (current != minptr)  
  { // old min is used as a sentinel
    r1 = current;
    int rank = r1->rank;
    // it's important to advance current already here
    current = current->right;  

    while (r2 = rank_array[rank])
    { rank_array[rank] = nil;
      // link combines trees r1 and r2 into a tree of rank one higher
      r1 = link(r1,r2); 
      rank++;
    }
    rank_array[rank] = r1;
    if ( cmp(r1->inf,new_min->inf) <= 0 ) new_min = r1;
  }
;
  
  minptr->left->right = minptr->right;
  minptr->right->left = minptr->left;
    
  clear_key(minptr->key);
  clear_inf(minptr->inf);
    
  r1 = minptr->pred;
  r2 = minptr->next;
  if (r2) r2->pred = r1;
  if (r1) r1->next = r2; else node_list = r2;
   
  delete minptr;

  minptr = new_min;
;
}


f_heap_item f_heap::link(f_heap_item r1, f_heap_item r2)   
{ 
  // r1 and r2 are roots of equal rank, both different from minptr;
  // the two trees are combined and the resulting tree is returned. 

  f_heap_item h1;
  f_heap_item h2;

  if (cmp(r1->inf,r2->inf) <= 0) 
   { // r2 becomes a child of r1
     h1 = r1; 
     h2 = r2;
   }
   else 
   { // r1 becomes a child of r2
     h1 = r2; 
     h2 = r1;
   }

  // we now make h2 a child of h1. We first remove h2 from 
  // the list of roots. 

  h2->left->right = h2->right;
  h2->right->left = h2->left;

  /* we next add h2 into the circular list of children of h1 */

  if ( h1->child == nil ) 
  { // h1 has no children yet; so we make h2 its only child 
    h1->child = h2;
    h2->left = h2;
    h2->right = h2;
  }
  else
  { // add h2 to the list of children of h1
    h2->left = h1->child;
    h2->right = h1->child->right;
    h1->child->right->left = h2;
    h1->child->right = h2;
  }

  h2->parent = h1;
  h1->rank++;

  return h1;
}


f_heap_item f_heap::cut(f_heap_item x)
{ 
  // turns the non-root x into a root and returns its old parent

  f_heap_item y = x->parent;

  if ( y->rank == 1 ) // only child
     y->child = nil;
  else
  { /* y has more than one child. We first make sure that its
       childptr does not point to x and then delete x from the 
       list of children */
    if ( y->child == x ) y->child = x->right;
    x->left->right = x->right;
    x->right->left = x->left;
  } 

  y->rank--;
  x->parent = nil;
  x->marked = false;

  // add to circular list of roots
  x->left = minptr;
  x->right = minptr->right;
  minptr->right->left = x;
  minptr->right = x;

  return y;
}


void f_heap::decrease_key(f_heap_item v, GenPtr newkey)
{ 
  /* changes the key of f_heap_item v to newkey; 
     newkey must be no larger than the old key; 
     if newkey is no larger than the minimum key 
     then v becomes the target of the minptr 
   */
  
  if (cmp(newkey,v->key) > 0)
    error_handler(1,"f_heap: key too large in decrease_key.");
  
  // change v's key
  clear_key(v->key);
  v->key = copy_key(newkey);
  
  if ( v->parent )   
  { f_heap_item x = cut(v);          // make v a root
    while (x->marked) x = cut(x);    // a marked f_heap_node 
                                     // is a non-root
    if (x->parent) x->marked = true; // mark x if it not a root
   }

  // update minptr (if necessary)
  if (cmp(newkey,minptr->key) <= 0) minptr = v;   
}


void   f_heap::clear()  { while (number_of_nodes > 0) del_min(); }

f_heap::~f_heap() { clear(); }


void f_heap::del_item(f_heap_item x) 
{ decrease_key(x,minptr->key);     // the minptr now points to x
  del_min();
}


f_heap& f_heap::operator=(const f_heap& H)
{ if (this != &H)
  { clear();
    for (f_heap_item p = H.first_item(); p; p = H.next_item(p)) 
     insert(p->key,p->inf);  
   }
  return *this;
 }


f_heap_item f_heap::first_item()             const 
            { return node_list; }

f_heap_node* f_heap::next_item(f_heap_node* p) const 
            { return p ? p->next : 0; }


f_heap::f_heap(const f_heap& H)
{ number_of_nodes = H.size();
  minptr = nil;
  node_list = nil;

  f_heap_item first_node = nil;

  for(f_heap_item p = H.first_item(); p; p = H.next_item(p))
   { GenPtr k = H.copy_key(p->key);    
     GenPtr i = H.copy_inf(p->inf);
     f_heap_item q = new f_heap_node(k,i,node_list);  
     q->right = node_list->next;
     if (node_list->next) node_list->next->left = q;
     if (minptr == nil) { minptr = q; first_node = q;}
     else if ( H.cmp(k,minptr->key) < 0 ) minptr = q;
    }
   first_node->right = node_list;
   node_list->left = first_node;
  
}
