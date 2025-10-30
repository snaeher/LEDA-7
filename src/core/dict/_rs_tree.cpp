/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rs_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/rs_tree.h>

//----------------------------------------------------------------------------
//  rs_tree:
//
//  rebalancing of randomized search trees
//
//  S. N"aher (1993)
//----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

void rs_tree::insert_rebal(rs_tree_item v)
{
  rs_tree_item u = v->parent;

  int prio = v->get_bal();

  ROOT.set_bal(prio);

  if (prio < u->get_bal())
  {
    int dir = (v == u->child[left]) ? left : right;

    while (prio < u->get_bal())          	     /* rotate v up        */
    {                                                /*       p            */
      bin_tree_node* p = u->parent;                  /*       |            */
      bin_tree_node* r = v->child[1-dir];            /*       |            */
      u->child[dir] = r;                             /*       u            */
      v->child[1-dir] = u;                           /*      / \           */
      u->parent = v;                                 /*     /   \          */
      r->parent = u;                                 /*    *     v (prio)  */
      propagate_modification(5,v,u);                 /*         / \        */
      propagate_modification(4,u,r);                 /*        /   \       */
                                                     /*       r     *      */
      dir = (u == p->child[left]) ? left : right;
      u = p;
     }
  
    // link to parent
  
    u->child[dir] = v;
    v->parent = u;

    if (u != &ROOT) 
       propagate_modification(6,u,v);
  }

}

void rs_tree::del_rebal(rs_tree_item, rs_tree_item) { }

LEDA_END_NAMESPACE
