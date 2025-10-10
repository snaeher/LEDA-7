/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  bin_tree1.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_BIN_TREE1_H
#define LEDA_BIN_TREE1_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 450980
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
//
// bin_tree1  
//
//     base class for all node oriented binary trees in LEDA
//
// Stefan N"aher (2023)
//
//------------------------------------------------------------------------------


#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

 
class bin_tree1;
class bin_tree1_node;

typedef bin_tree1_node* bin_tree1_item;

/*
enum { LEDA_BIN_TREE=0, LEDA_AVL_TREE=1, LEDA_BB_TREE=2, LEDA_RB_TREE=3, 
       LEDA_RS_TREE=4, LEDA_NUM_TREES=5 };
*/

enum { LEDA_BIN_TREE1=0, LEDA_AVL_TREE1=1 };

//------------------------------------------------------------------------------
// class bin_tree1_node 
//------------------------------------------------------------------------------

class __exportC bin_tree1_node
{  
   friend class __exportC bin_tree1;
   friend class __exportC avl_tree1;


   GenPtr   k;              // key
   GenPtr   i;              // info

   bin_tree1_item child[2]; // node: left and right child
   bin_tree1_item parent;   // pointer to parent (used temporarily)
   int balance;              // rebalancing data
 

   bin_tree1_node(GenPtr key=0, GenPtr inf=0, int b=0)
   { k = key;
     i = inf; 
     parent = 0;
     balance = b;
    }         

protected:

   void set_bal(int x) { balance = x; }

public:

   bool is_leaf() { return (child[0] == 0 && child[1] == 0); }

   int    get_bal() { return balance; }
   GenPtr get_key() { return k; }
   GenPtr get_inf() { return i; }

   LEDA_MEMORY(bin_tree1_node)

};


 
//------------------------------------------------------------------------------
// class bin_tree1
//------------------------------------------------------------------------------

class __exportC bin_tree1
{ 
  protected:

  enum { left=0, right=1 };

  bin_tree1_node ROOT;  // "super root" to avoid special cases in rotations
                        // ROOT.child[left]  points to real root node
                        // ROOT.child[right] points to leftmost leaf (minimum)

/*
  bin_tree1_item get_min()  const { return ROOT.child[right]; }
  void set_min(bin_tree1_item p)  { ROOT.child[right] = p; }
*/

  void set_root(bin_tree1_item p) { ROOT.child[left] = p;  }


  int count;            

  // functions depending on used rebalancing method
  // will be defined in derived classes (rb_tree, avl_tree, ...)

  virtual int leaf_balance() { return 0; }  // default balance value for leaves
  virtual int node_balance() { return 0; }  // inner nodes
  virtual int root_balance() { return 0; }  // root node

  virtual void insert_rebal(bin_tree1_item)   {}

  virtual void del_rebal(bin_tree1_item) {}

  virtual void del_rebal(bin_tree1_item, int) {}


protected:

  void rotation(bin_tree1_item, bin_tree1_item, int);
  void double_rotation(bin_tree1_item, bin_tree1_item, bin_tree1_item, int);

  void del_tree(bin_tree1_item);

  bin_tree1_item search(GenPtr, bin_tree1_item&) const;

  bin_tree1_item copy_tree(bin_tree1_item) const;

  bin_tree1_item succ_or_pred(bin_tree1_item p, int dir) const;


  // functions depending on actual key type
  // will be defined in dictionary and sortseq templates

  virtual int  cmp(GenPtr x, GenPtr y) const 
                                        { return LEDA_COMPARE(long,x,y); }

  virtual int  key_type_id()      const { return UNKNOWN_TYPE_ID; }
  virtual bool key_def_order()    const { return false; }

  virtual void clear_key(GenPtr&) const { }
  virtual void clear_inf(GenPtr&) const { }
  virtual void copy_key(GenPtr&)  const { }
  virtual void copy_inf(GenPtr&)  const { }
  virtual void print_key(GenPtr)  const { }
  virtual void print_inf(GenPtr)  const { }

public:

  typedef bin_tree1_item item;

  void* owner; // pointer to data type object (e.g. sortseq<K,I>)

  virtual int         tree_type() const { return LEDA_BIN_TREE1; }
  virtual const char* tree_name() const { return "Binary Tree";  }


  const GenPtr&  key(bin_tree1_item p)  const { return  p->k; }
  GenPtr&        inf(bin_tree1_item p)  const { return  p->i; }

  void change_inf(bin_tree1_item p, GenPtr y) 
  { clear_inf(p->i);
    copy_inf(y);
    p->i = y; 
   }

  bin_tree1_item lookup(GenPtr x) const;

  bin_tree1_item insert(GenPtr,GenPtr);

  void del(GenPtr);
  void del_item(bin_tree1_item it) { del(it->k); }

  void clear() 
  { del_tree(get_root());
    set_root(nil);
    count = 0;
   }

  int  size()   const { return count; } 
  bool empty()  const { return count == 0 ; }

  bin_tree1_item min() const;
  bin_tree1_item max() const;


  // construction, assignment, destruction

  bin_tree1() { 
    ROOT.child[left] = 0;
    ROOT.child[right] = 0;
    ROOT.parent = 0;
    ROOT.balance = 0;
    count = 0; 
  }

  bin_tree1(const bin_tree1& T) { 
    ROOT.child[left] = 0;
    ROOT.child[right] = 0;
    ROOT.parent = 0;
    ROOT.balance = 0;
    set_root(copy_tree(T.get_root())); 
    count = T.count;
  }

  virtual ~bin_tree1() { clear(); }


  // iteration

  bin_tree1_item succ(bin_tree1_item p) const { return succ_or_pred(p,right);}
  bin_tree1_item pred(bin_tree1_item p) const { return succ_or_pred(p,left); }


  bin_tree1_item first_item()  const { return min(); }
  bin_tree1_item last_item()   const { return max(); }

  bin_tree1_item next_item(bin_tree1_item p) const { return p ? succ(p) : 0; }
  bin_tree1_item pred_item(bin_tree1_item p) const { return p ? pred(p) : 0; }

  bin_tree1_item stl_next_item(bin_tree1_item p) const 
  { return p ? succ(p) : first_item(); }

  bin_tree1_item stl_pred_item(bin_tree1_item p) const 
  { return p ? pred(p) : last_item(); }



  // drawing and animation

  bin_tree1_item l_child(bin_tree1_item p) const
  { return p->child[left]; }

  bin_tree1_item r_child(bin_tree1_item p) const
  { return p->child[right]; }

  bin_tree1_item parent(bin_tree1_item p)  const
  { return  p->parent; }

  bool is_root(bin_tree1_item p) { return p == ROOT.child[left]; }

  bin_tree1_item get_root() const { return ROOT.child[left]; }

  bool is_leaf(bin_tree1_item p) { 
    return p->child[left] == 0 && p->child[right] == 0;  
  }

  int get_bal(bin_tree1_item p) { return p->balance; }


};



LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 350980
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


#endif
