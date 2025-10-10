/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  p_queue_test.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:16 $


#define f_heap f_heap_book

#line 3463 "impl.lw"
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


#line 3492 "impl.lw"
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


#line 3746 "impl.lw"
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


#line 3791 "impl.lw"
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

  
#line 3838 "impl.lw"
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

#line 3812 "impl.lw"
                                        ;
  
#line 3953 "impl.lw"
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

#line 3813 "impl.lw"
                                                         ;
  
#line 3981 "impl.lw"
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

#line 3814 "impl.lw"
                        ;
}


#line 3870 "impl.lw"
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


#line 4003 "impl.lw"
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


#line 4043 "impl.lw"
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


#line 4077 "impl.lw"
void   f_heap::clear()  { while (number_of_nodes > 0) del_min(); }

f_heap::~f_heap() { clear(); }


#line 4090 "impl.lw"
void f_heap::del_item(f_heap_item x) 
{ decrease_key(x,minptr->key);     // the minptr now points to x
  del_min();
}


#line 4104 "impl.lw"
f_heap& f_heap::operator=(const f_heap& H)
{ if (this != &H)
  { clear();
    for (f_heap_item p = H.first_item(); p; p = H.next_item(p)) 
     insert(p->key,p->inf);  
   }
  return *this;
 }


#line 4125 "impl.lw"
f_heap_item f_heap::first_item()             const 
            { return node_list; }

f_heap_node* f_heap::next_item(f_heap_node* p) const 
            { return p ? p->next : 0; }


#line 4146 "impl.lw"
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

#line 3176 "impl.lw"
#define PRIO_IMPL f_heap

typedef PRIO_IMPL::item pq_item;

template<class P, class I> 
class p_queue: private PRIO_IMPL
{
  int    key_type_id()           const { return LEDA_TYPE_ID(P); }

  int    cmp(GenPtr x, GenPtr y) const 
         { return LEDA_COMPARE(P,x,y); }

  void   clear_key(GenPtr& x)    const { LEDA_CLEAR(P,x); }

  void   clear_inf(GenPtr& x)    const { LEDA_CLEAR(I,x); }

  GenPtr copy_key(GenPtr& x)     const { return LEDA_COPY(P,x); }

  GenPtr copy_inf(GenPtr& x)     const { return LEDA_COPY(I,x); }

public:
  p_queue()  {}
  p_queue(const p_queue<P,I>& Q):PRIO_IMPL(Q) {}
 ~p_queue()  { PRIO_IMPL::clear(); }
 
  p_queue<P,I>& operator=(const p_queue<P,I>& Q) 
  { PRIO_IMPL::operator=(Q); return *this; }
  
  P       prio(pq_item it) const 
          { return LEDA_CONST_ACCESS(P,PRIO_IMPL::key(it)); }
  I       inf(pq_item it)  const 
          { return LEDA_CONST_ACCESS(I,PRIO_IMPL::inf(it)); }
  pq_item find_min()       const { return PRIO_IMPL::find_min();}
  void    del_min()              { PRIO_IMPL::del_min(); }
  void    del_item(pq_item it)   { PRIO_IMPL::del_item(it); }
  
  pq_item insert(const P& x, const I& i) 
  { return PRIO_IMPL::insert(leda_cast(x),leda_cast(i)); }
  
  void    change_inf(pq_item it, const I& i) 
  { PRIO_IMPL::change_inf(it,leda_cast(i)); }
  
  void decrease_p(pq_item it, const P& x)
  { PRIO_IMPL::decrease_key(it,leda_cast(x)); }
  
  int  size()  const { return PRIO_IMPL::size(); }
  bool empty() const { return (size()==0) ? true : false; }
  void clear()       { PRIO_IMPL::clear(); }
  
  pq_item first_item()          const 
          { return PRIO_IMPL::first_item(); }
  pq_item next_item(pq_item it) const 
          { return PRIO_IMPL::next_item(it); }
  
};


#line 4175 "impl.lw"
int main(){
 
p_queue<int,int> P;

for (int i = 0; i < 10; i++)
  { P.insert(10-i,i); }

while ( !P.empty() )
  { pq_item min_it = P.find_min();
    cout << P.prio(min_it);
    P.del_min();
  }

 return 0;
}


