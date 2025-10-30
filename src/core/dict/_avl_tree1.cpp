/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _avl_tree1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/avl_tree1.h>


//----------------------------------------------------------------------------
//  avl_tree1:
//
//  rebalancing routines for node oriented AVL trees
//
//----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE


#include <LEDA/system/basic.h>
#include <LEDA/core/impl/avl_tree1.h>

#include <assert.h>


int avl_tree1::check_balance(avl_tree1_item p, bool b) 
{
  if (p == nil) return 0;

  int h_left = check_balance(p->child[left],b);
  int h_right = check_balance(p->child[right],b);

  if (p->balance != (h_right - h_left)) {
     cout << string("[%d]  bal = %d h_left = %d h_right = %d",
     p->k, p->balance,h_left,h_right) << endl;
  }

  if (b) {
    assert(p->balance == (h_right - h_left));
    assert(p->balance >= -1 && p->balance <= +1);
  }

  return 1 + std::max(h_left,h_right);
}


void avl_tree1::insert_rebal(avl_tree1_item v)
{
  // rebalance the AVL tree after an insertion

  // preconditions 
  // -- subtree T rooted at v is an AVL Tree (with correct balance values)
  // -- the total height of T has been increased by one (by an insertion)

  assert(v->balance >= -1 && v->balance <= +1);

  while (v != get_root())
  {
    // Check whether T is the left or right subtree of
    // its parent node u (dir = left or right)
    // and adjust the balance of u accordingly.

    avl_tree1_item u = v->parent;
    int dir = (v == u->child[left]) ? left : right;

  //int b = u->balance + (dir == left) ? -1 : +1; // brackets required ?

    int b = u->balance + ((dir == left) ? -1 : +1);

    u->balance = b;

    if (b == 0)  
    { // total height has not changed  (b == -1 or b == +1 before)
      // stop rebalancing
      break;   
     }

    if (b == -1 || b == +1) 
    { // total height has increased (b == 0 before the insertion)
      // continue rebalancing at parent node u
      v = u;
      continue;
     }

    // u is unbalanced (b = -2 or b = + 2)
    // one rotation/double-rotation rebalances the tree
    // such that the total height does not change

    int d = (b < 0) ? -1 : +1;

    avl_tree1_item w = u->child[dir];

    if (w->balance == d)
    { rotation(u,w,dir);
      u->balance = 0;
      w->balance = 0;
     }
    else
    { avl_tree1_item x = w->child[1-dir];
      double_rotation(u,w,x,dir);

      if (x->balance == d)
         u->balance = -d;
      else
         u->balance = 0;

      if (x->balance == -d)
         w->balance = d;
      else
         w->balance = 0;

      x->balance = 0;

     }

    break; // stop

  }

}



void avl_tree1::del_rebal(avl_tree1_item u, int dir)
{
  // rebalance the AVL tree after a deletion

  // Preconditions :
  // 1. dir = 0 (left) or 1 (right)
  // 2. subtree T rooted at u->child[dir] is a (possibly empty) AVL Tree 
  // 3. the total height of T has been decreased by one (by a deletion)

  while (u != &ROOT) 
  {
    // The height of the subtree rooted at u->child[dir] has been 
    // decreased by one - adjust the balance of u accordingly.
  
    int b = u->balance + ((dir == left) ? +1 : -1);
  
    u->balance = b;
  
    if (b == 1 || b == -1)
    { // b == 0 before the delete operation
      // balance ok & total height unchanged
      // stop rebalancing
      break;
    }
  
    avl_tree1_item p = u->parent;
  
    if (b == 0) {
      // balance ok but total height decreased by one
      // continue rebalancing with parent p of u
      dir = (u == p->child[left]) ? left : right;
      u = p;
      continue;
    }
  
    // u is out of balance (-2 or + 2)
    
    int d = (b < 0) ? -1 : +1;
  
    avl_tree1_item w = u->child[1-dir];
  
    if (d * w->balance >= 0)
    { 
      rotation(u,w,1-dir);
  
      if (w->balance == 0)
      { u->balance = d;
        w->balance = -d;
        // stop rebalancing (total height unchanged)
        break;
       }
      else
      { u->balance = 0;
        w->balance = 0;
        // continue rebalancing at parent p of w
        dir = (w == p->child[left]) ? left : right;
        u = p;
       }
     }
    else
    { avl_tree1_item x = w->child[dir];
  
      double_rotation(u,w,x,1-dir);
  
      if (x->balance == d)
         u->balance = -d;
      else
         u->balance = 0;
  
      if (x->balance == -d)
         w->balance = d;
      else
         w->balance = 0;
  
      x->balance = 0;
  
      // continue rebalancing at parent p of x
      dir = (x == p->child[left]) ? left : right;
      u = p;
     }
  }
  
}


void avl_tree1::del_rebal_recursive(avl_tree1_item u, int dir)
{
  // a recursive procedure to rebalance the AVL tree after a deletion

  // Preconditions :
  // 1. dir = 0 (left) or 1 (right)
  // 2. subtree T rooted at u->child[dir] is a (possibly empty) AVL Tree 
  // 3. the total height of T has been decreased by one (by a deletion)

  if (u == &ROOT) return;


  // The height of the subtree rooted at u->child[dir] has been 
  // decreased by one - adjust the balance of u accordingly.

  int b = u->balance + ((dir == left) ? +1 : -1);

  u->balance = b;

  if (b == 1 || b == -1)
  { // b == 0 before the delete operation
    // balance ok & total height unchanged
    // stop rebalancing
    return;
  }

  avl_tree1_item p = u->parent;

  if (b == 0) {
    // balance ok but total height decreased by one
    // continue rebalancing with parent node
    del_rebal(p, (u == p->child[left]) ? left : right);
    return;
  }

  // u is out of balance (-2 or + 2)
  
  int d = (b < 0) ? -1 : +1;

  avl_tree1_item w = u->child[1-dir];

  if (d * w->balance >= 0)
  { 
    rotation(u,w,1-dir);

    if (w->balance == 0)
    { u->balance = d;
      w->balance = -d;
      // stop rebalancing (total height unchanged)
     }
    else
    { u->balance = 0;
      w->balance = 0;
      // continue rebalancing at parent p of w
      del_rebal(p,(w == p->child[left]) ? left : right);
     }
   }
  else
  { avl_tree1_item x = w->child[dir];

    double_rotation(u,w,x,1-dir);

    if (x->balance == d)
       u->balance = -d;
    else
       u->balance = 0;

    if (x->balance == -d)
       w->balance = d;
    else
       w->balance = 0;

    x->balance = 0;

    // continue rebalancing at parent p of x
    del_rebal(p,(x == p->child[left]) ? left : right);
   }
}


LEDA_END_NAMESPACE
