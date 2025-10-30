/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  avl_tree1.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_AVL_TREE1_H
#define LEDA_AVL_TREE1_H

//------------------------------------------------------------------------------
//
// avl_tree1:
//
//        node oriented AVL trees (derived from bin_tree1)
//
// Stefan N"aher (2023)
//
//------------------------------------------------------------------------------



#include <LEDA/system/basic.h>
#include <LEDA/core/impl/bin_tree1.h>

#include <assert.h>

LEDA_BEGIN_NAMESPACE
 
typedef bin_tree1_node* avl_tree1_item;

 
class __exportC avl_tree1 : public bin_tree1
{ 
  // balance of node v with right (left) subtree R (L)
  // bal(v) =  height(R) - height(L) in [-1 ... +1]
  //
  // all created nodes have balance 0

  int         tree_type() const { return LEDA_AVL_TREE1; }
  const char* tree_name() const { return "AVL Tree"; }

  int root_balance() { return 0; }
  int leaf_balance() { return 0; }
  int node_balance() { return 0; }

  void insert_rebal(avl_tree1_item);

  void del_rebal(bin_tree1_node*, int);
  void del_rebal_recursive(bin_tree1_node*, int);

  int check_balance(avl_tree1_item p, bool b=false);

public:

  avl_tree1() {}
 ~avl_tree1() {}

  avl_tree1(const avl_tree1& T) : bin_tree1(T) {}

  avl_tree1& operator=(const avl_tree1& T) 
  { bin_tree1::operator=(T); return *this; }


};


LEDA_END_NAMESPACE

#endif


