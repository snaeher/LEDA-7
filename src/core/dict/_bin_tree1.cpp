/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _bin_tree1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
//
//  bin_tree1: base class for node oriented binary tree types in LEDA
//
//
//  S. N"aher (2023)
//
//------------------------------------------------------------------------------

#include <LEDA/core/impl/bin_tree1.h>

#include <assert.h>

LEDA_BEGIN_NAMESPACE


#define SEARCH(T) {                        \
const T& xx = LEDA_CONST_ACCESS(T,x);      \
while (q) {                                \
  q->parent = p;                           \
  const T& k = LEDA_CONST_ACCESS(T,q->k);  \
  if (xx == k) break;                      \
  p = q;                                   \
  q = q->child[ (xx < k) ? left : right ]; \
}}


bin_tree1_item bin_tree1::search(GenPtr x, bin_tree1_item& last) const
{ 
  // search for key x
  // returns node q with q->k = x and sets last = q->parent, if x in T.
  // returns null and sets last = last node on search path to x, otherwise.
  // updates parent pointers for all nodes on the search path


  bin_tree1_item p = (bin_tree1_item)&ROOT;
  bin_tree1_item q = p->child[left]; // get_root()

  int type_id = key_type_id();
  if (!key_def_order()) type_id = -1;

  switch (type_id) {
    case INT_TYPE_ID:    SEARCH(int);
                         break;
    case FLOAT_TYPE_ID:  SEARCH(float);
                         break;
    case DOUBLE_TYPE_ID: SEARCH(double);
                         break;
    default: while (q) 
             { q->parent = p;
               int c = cmp(x,q->k);
               if (c == 0) break;
               p = q;
               q = q->child[(c < 0) ? left : right];
              }
             break;
   }

  last = p;
  return q;
}


bin_tree1_item bin_tree1::lookup(GenPtr x) const
{ bin_tree1_item p;
  return  search(x,p);
}


bin_tree1_item bin_tree1::insert(GenPtr x, GenPtr y)
{ // inserts a new leaf with key x and inf y
  // and returns pointer to this leaf

  if (count==0)  // tree is empty 
  { copy_key(x);
    copy_inf(y);
    bin_tree1_item q = new bin_tree1_node(x,y,leaf_balance());
    q->child[right] = 0;
    q->child[left] = 0;
    set_root(q);
    count = 1;
    return q;
  }

  bin_tree1_item p; // parent
  bin_tree1_item q = search(x,p);

  if (q)
  { // x in T: overwrite info
    clear_inf(p->i);
    copy_inf(y);
    p->i = y;
    return q;
   }

  copy_key(x);
  copy_inf(y);
  count++;

  // create new leaf node q with parent p

  q = new bin_tree1_node(x,y,leaf_balance());
  q->child[right] = 0;
  q->child[left] = 0;

  if (cmp(x,p->k) < 0) 
    p->child[left] = q;
  else
    p->child[right]= q;

  q->parent = p;

  insert_rebal(q);

  return q;
}



//------------------------------------------------------------------------------
// del(x) 
// removes node with key x from the tree
//------------------------------------------------------------------------------

void bin_tree1::del(GenPtr x)
{
  bin_tree1_item p; // parent
  bin_tree1_item q = search(x,p);

  if (q == nil) {
    // x not found
    return;
  }

  clear_key(q->k);
  clear_inf(q->i);

  if (--count == 0)      
  { // tree is now empty
    delete q;
    set_root(nil);
    return;
   } 

  int dir = (q == p->child[left]) ? left : right; // q == p->child[dir] 

  bin_tree1_item q_left = q->child[left];
  bin_tree1_item q_right = q->child[right];

  if (q_left == nil && q_right == nil)
  { 
    // q has no children (leaf) 

    p->child[dir] = 0;

/*
    cout << endl;
    cout << "DELETE: CASE 0" << endl;
    cout << string("q = [%d]", q->k) << endl;
    cout << string("p = [%d]", p->k) << endl;
    cout << endl;
*/

    del_rebal(p,dir);

    delete q;
    return;
  }


  if (q_left == nil || q_right == nil)
  { // q has one child

    if (q_right) 
      p->child[dir] = q_right;
    else
      p->child[dir] = q_left;
/*
    cout << endl;
    cout << "DELETE: CASE 1" << endl;
    cout << string("q = [%d]", q->k) << endl;
    cout << endl;
*/

    del_rebal(p,dir);

    delete q;
    return;
  }

  // q has two children
  // find node u with maximal key in left subtree rooted at q

  bin_tree1_item u = q_left;
  u->parent = q;
  while (u->child[right]) { 
    bin_tree1_item next = u->child[right];
    next->parent = u;
    u = next;
  }

  bin_tree1_item u_parent = u->parent;

  // u has no right child 

  if (u == q_left) 
  { 
/*
    cout << endl;
    cout << "DELETE: CASE 2a" << endl;
    cout << string("q = [%d]", q->k) << endl;
    cout << string("p = [%d]", p->k) << endl;
    cout << string("u = [%d]", u->k) << endl;
*/
    p->child[dir] = u;
    u->parent = p;
    u->child[right] = q_right;
    u->balance = q->balance;

    del_rebal(u,left);

    delete q;
    return;
  }

/*
  cout << endl;
  cout << "DELETE: CASE 2b" << endl;
  cout << string("q = [%d]", q->k) << endl;
  cout << string("p = [%d]", p->k) << endl;
  cout << string("u = [%d]", u->k) << endl;
  cout << endl;
*/
  
  // u is right child of u_parent and has at most one (left) child 
  // disconnect u and move it to the position of q (replace q by u)

  u_parent->child[right] = u->child[left];

  if (u->child[left]) u->child[left]->parent = u_parent;

  p->child[dir] = u;
  u->parent = p;

  u->child[right] = q_right;
  q_right->parent = u;

  u->child[left]  = q_left;
  q_left->parent = u;

  u->balance = q->balance;

  del_rebal(u_parent,right);

  delete q;
}


bin_tree1_item bin_tree1::min() const
{ bin_tree1_item q = get_root();
  if (q) {
    while (q->child[left]) q = q->child[left];
  }
  return q;
}

bin_tree1_item bin_tree1::max() const
{ bin_tree1_item q = get_root();
  if (q) {
    while (q->child[right]) q = q->child[right];
  }
  return q;
}

bin_tree1_item bin_tree1::succ_or_pred(bin_tree1_item p, int dir) const
{ // dir in { left, right }
  // if p has a child[dir] q find min/max in subtree rooted at q

  bin_tree1_item q = p->child[dir];
  if (q) {
    while (q->child[left]) q = q->child[1-dir];
    return q;
  }

  // p has no child[dir] 
  // find lowest left or right turn on search path from root to p

  bin_tree1_item lturn = 0;
  bin_tree1_item rturn = 0;

  GenPtr x = p->k;

  for(bin_tree1_item q = get_root(); q != p; ) 
  { int c = cmp(x,q->k);
    if (c < 0) 
    { lturn = q;
      q = q->child[left];
     }
    else
    { rturn = q;
      q = q->child[right];
     }
  }

  return (dir == right) ? lturn : rturn;
}



//------------------------------------------------------------------
// copy_tree(p) makes a copy of tree with root p and returns a pointer to the
// root of the copy. 
//------------------------------------------------------------------

bin_tree1_item bin_tree1::copy_tree(bin_tree1_item p) const
{
  if (p == nil) return nil;

  bin_tree1_item q = new bin_tree1_node(p->k,p->i,p->balance);
  copy_key(q->k);
  copy_inf(q->i);

  q->child[left]  = copy_tree(p->child[left]);
  q->child[right] = copy_tree(p->child[right]);

  return q;
}

//------------------------------------------------------------------
// del_tree(p) : deletes subtree rooted at node p
//------------------------------------------------------------------

void bin_tree1::del_tree(bin_tree1_item p)
{
  if (p == nil) return;

  del_tree(p->child[left]);
  del_tree(p->child[right]);

  clear_key(p->k);
  clear_inf(p->i);
  delete p;
}


//------------------------------------------------------------------
// rotations
//------------------------------------------------------------------


void bin_tree1::rotation(bin_tree1_item p,bin_tree1_item q, int dir)
{ bin_tree1_item r = q->child[1-dir];
  bin_tree1_item x = p->parent;

/*
cout << string("SINGLE ROTATION(%d) p = [%d] (%d) q = [%d] (%d)",
                dir,p->k,p->balance,q->k,q->balance) << endl; 
*/

//assert(p->child[dir] == q);

  p->child[dir] = r;
  //if (r) r->parent = p;

  q->child[1-dir] = p;
  //if (p) p->parent = q;

  if (p == x->child[left])
     x->child[left] = q;
  else
     x->child[right] = q;

  q->parent = x;
}


void bin_tree1::double_rotation(bin_tree1_item p, bin_tree1_item q, 
                                       bin_tree1_item r, int dir1)
{ int dir2 = 1-dir1;
  bin_tree1_item s = r->child[dir1];
  bin_tree1_item t = r->child[dir2];

  bin_tree1_item x = p->parent;

/*
cout << string("DOUBLE ROTATION(%d) p = [%d] (%d) q = [%d] (%d) r = [%d] (%d)",
                dir1,p->k,p->balance,q->k,q->balance,r->k,r->balance) << endl; 
*/

  p->child[dir1] = t;
  //if (t) t->parent = p;

  q->child[dir2] = s;
  //if (s) s->parent = q;

  r->child[dir1] = q;
  //if (q) q->parent = r;

  r->child[dir2] = p;
  //if (p) p->parent = r;

  if (p == x->child[left])
     x->child[left] = r;
  else
     x->child[right] = r;

  r->parent = x;
}



LEDA_END_NAMESPACE
