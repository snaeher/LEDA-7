/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  c_obj_list.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_C_OBJ_LIST_H
#define LEDA_C_OBJ_LIST_H

#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
//
//  c_obj_list  (doubly linked circular lists of obj_links)
//
//  each "obj_link" may be present in at most one list 
//  
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// class obj_link (base class for list items)
//------------------------------------------------------------------------------

class __exportC obj_link {

protected:

  obj_link* succ_link;
  obj_link* pred_link;

public:

void del_item() 
{ pred_link->succ_link = succ_link; 
  succ_link->pred_link = pred_link; 
 }

// obj_link() { succ = nil; }

  obj_link* succ_item() { return succ_link; }
  obj_link* pred_item() { return pred_link; }

  friend class __exportC obj_list;
  friend class __exportC c_obj_list;
  friend class __exportC sc_obj_list;
};



//------------------------------------------------------------------------------
//
// c_obj_list (doubly linked circular object list)
//
// simple and efficient implementation (no counter, iterator, sorting, etc.)
// removed items are assigned a nil succ pointer 
// member(x) <==>  x->succ != nil
//
//------------------------------------------------------------------------------

class __exportC c_obj_list : public obj_link {

// the list head is an obj_link, namely the predecessor of the first 
// and the successor of the last element

public:

bool empty()  const { return succ_link == (obj_link*)this; }

obj_link* first()      const { return (succ_link==(obj_link*)this) ? nil : succ_link; }
obj_link* last()       const { return (pred_link==(obj_link*)this) ? nil : pred_link; }
obj_link* first_item() const { return first(); }
obj_link* last_item()  const { return last(); }

obj_link* succ(obj_link* p) const 
{ return (p->succ_link==(obj_link*)this)? nil : p->succ_link;}

obj_link* pred(obj_link* p) const 
{ return (p->pred_link==(obj_link*)this)? nil : p->pred_link;}

obj_link* cyclic_succ(obj_link* p) const 
{ return (p->succ_link==(obj_link*)this) ? succ_link : p->succ_link; }

obj_link* cyclic_pred(obj_link* p) const 
{ return (p->pred_link==(obj_link*)this) ? pred_link : p->pred_link; }


obj_link* next_item(obj_link* p) const { return p ? succ(p) : 0; }


 void insert(obj_link* n, obj_link* p) 
 { // insert n insert after p
   obj_link* s=p->succ_link;
   n->pred_link = p;
   n->succ_link = s;
   p->succ_link = n;
   s->pred_link = n;
  }

 obj_link* del(obj_link* x)
 { obj_link*  p = x->pred_link;
   obj_link*  s = x->succ_link;
   p->succ_link = s;
   s->pred_link = p;
   x->succ_link = nil;
   return x;
  }

 obj_link* del(obj_link* x, obj_link* y)
 { obj_link*  p = x->pred_link;
   obj_link*  s = y->succ_link;
   p->succ_link = s;
   s->pred_link = p;
   return x;
  }

 void move(obj_link* x, obj_link* y)
 { obj_link* px = x->pred_link;
   obj_link* sx = y->succ_link;
   obj_link* sy = y->succ_link;
   px->succ_link = sx;
   sx->pred_link = px;
   x->pred_link = y;
   x->succ_link = sy;
   y->succ_link = x;
   sy->pred_link = x;
  }


 bool member(obj_link* x) { return x->succ_link != nil; }

 void push(obj_link* p)   { insert(p,this); }
 void append(obj_link* p) { insert(p,pred_link); }

 obj_link* pop()      { return del(succ_link); }
 obj_link* pop_back() { return del(pred_link); }

 void clear() 
 { while(succ_link != this) 
   { obj_link* p = succ_link;
     succ_link = p->succ_link;
     p->succ_link = nil;
    }
   pred_link = this; 
  }

 void init() { succ_link = pred_link = this; }

// constructors & destructors

 c_obj_list()  { succ_link = pred_link = this; }
~c_obj_list()  { clear(); }

};


LEDA_END_NAMESPACE

#endif

