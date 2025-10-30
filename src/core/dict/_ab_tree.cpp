/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _ab_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/ab_tree.h>

#define delete_node(p)\
std_memory.deallocate_bytes(p,node_bytes(p->size))

LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------
// constructors
//------------------------------------------------------------------

atomic_counter ab_tree_node::id_count(0);
 

inline int node_bytes(int sz)
{ return int(sizeof(ab_tree_node)) + (sz-1)*int(sizeof(ab_tree_elem)); }
 
inline ab_tree_node* new_ab_tree_node(int d, int h, ab_tree_node* f, int sz)
{ ab_tree_node* q=(ab_tree_node*)std_memory.allocate_bytes(node_bytes(sz)); 
  q->last = q->arr + d - 1;
  q->height=h; 
  q->size = sz;
  q->father=f; 
  q->id = ab_tree_node::id_count++;
  return q;
}



ab_tree::ab_tree(int A, int B)
{ root=maximum=minimum=0;
  count=0;
  height=-1;
  if( A >= 2 && B >= 2*A-1 )
   { a=A;
     b=B;
    }
  else LEDA_EXCEPTION(1,"ab_tree: illegal arguments (a,b) in tree constructor");
  owner = 0;
}


ab_tree::ab_tree(const ab_tree& T)
{
  if (T.root==0) 
    { root=maximum=minimum=0;
      height=-1;
      count=0;
     }
  else 
   { ab_tree_node* p=0;
     root = T.copy_ab_tree(T.root,p,T.b);
     maximum=p;
     maximum->succ() = 0;
     p=root;
     while (p->height) p=p->arr[0].child;
     minimum=p;
     height=T.height;
     count=T.count;
    }
  a=T.a;
  b=T.b;
  owner = 0;
}

//-----------------------------------------------------------------
// operators
//-----------------------------------------------------------------

 ab_tree& ab_tree::operator=(const ab_tree& T)
{ if (this == &T) return *this;

  clear();

  if (T.root!=0) 
  { ab_tree_node* p = 0;
    root=copy_ab_tree(T.root,p,T.b);
    maximum=p;
    maximum->succ() = 0;
    p=root;
    while (p->height) p=p->arr[0].child;
    minimum=p;
    height=T.height;
    count=T.count;
   }

  a=T.a;
  b=T.b;

  return *this;
}


//-----------------------------------------------------------------
// member functions
//-----------------------------------------------------------------

ab_tree_elem* ab_tree::index(ab_tree_node* v, ab_tree_node* u)
{ ab_tree_elem* p = u->arr;
  while (p->child != v) p++;
  return p;
}


ab_tree_elem* ab_tree::same(ab_tree_node* v)
{ 
  if (v->succ() == 0) return 0;

  ab_tree_node* u = v->father;
  GenPtr key = v->key();

  while (v == u->last->child)  
  { v = u; 
    u = v->father;
   }

  ab_tree_elem* p = u->arr;
  while (p->key != key) p++;

  return p;
}


void ab_tree::flip_leaves(ab_tree_node* v, ab_tree_node* w, ab_tree_node* f)
{ 
  // flip leaves v and w
  // precond: w = succ(v) && f = parent(v) = parent(w)

  ab_tree_elem* p1 = f->arr; 
  while (p1->child != v) p1++;

  ab_tree_elem* p2 = p1+1;

  ab_tree_elem* u2 = nil;

  if (p2 == f->last) u2 = same(w);

  p1->child = w;
  p1->key = w->key();

  p2->child = v;
  if (u2)
    u2->key = v->key();
  else
    p2->key = v->key();

  ab_tree_node* pred = v->pred();
  ab_tree_node* succ = w->succ();

  w->succ() = v;
  w->pred() = pred;
  if (pred) pred->succ() = w;

  v->pred() = w;
  v->succ() = succ;
  if (succ) succ->pred() = v;

  if (v==minimum) minimum = w;
  if (w==maximum) maximum = v;

}

  


void ab_tree::exchange_leaves(ab_tree_node* v, ab_tree_node* w)
{ // exchange leaves v and w

  GenPtr k1 = v->key(); 
  GenPtr k2 = w->key(); 

  ab_tree_node* f1 = v->father;
  ab_tree_node* f2 = w->father;

  ab_tree_elem* p1 = f1->arr; 
  while (p1->child != v) p1++;

  ab_tree_elem* p2 = f2->arr;
  while (p2->child != w) p2++;

  ab_tree_elem* u1 = same(v);
  ab_tree_elem* u2 = same(w);

  if (u1) u1->key = k2;
  if (u2) u2->key = k1;

  p1->child = w;
  w->father = f1;

  p2->child = v;
  v->father = f2;

  ab_tree_node* pred1 = v->pred();
  ab_tree_node* succ1 = v->succ();

  ab_tree_node* pred2 = w->pred();
  ab_tree_node* succ2 = w->succ();

  w->pred() = pred1;
  if (pred1) pred1->succ() = w;
  if (succ1!=w) 
  { w->succ() = succ1;
    succ1->pred() = w;
   }

  if (pred2==v) pred2 = w;

  v->pred() = pred2;
  v->succ() = succ2;
  pred2->succ() = v;
  if (succ2) succ2->pred() = v;

  // minimum & maximum:

  if (v==minimum) minimum = w;
  if (w==maximum) maximum = v;

}


void ab_tree::reverse_items(ab_tree_node* v, ab_tree_node* w)
{ // reverse sequence of leaves: v, ..., w

  if (v==w) return;

  if (w == succ(v))
  { ab_tree_node* f1 = v->father;
    ab_tree_node* f2 = w->father;
    if (f1 == f2) 
    { flip_leaves(v,w,f1);
      return;
     }
   }

  for(;;)
  { exchange_leaves(v,w);
    ab_tree_node* u = pred(v);
    if (u == w) break;
    v = succ(w);
    if (v==u) break;
    w = u;
   }
}


/*
void ab_tree::reverse_items(ab_tree_node* p, ab_tree_node* q)
{ // reverse sequence of leaves: p... q

  cout << "reverse:" << endl;
  print_key(p->key()); cout << endl;
  print_key(q->key()); cout << endl;
  cout << endl;
  
  while (p != q)
  { ab_tree_node* r = p;
    p = succ(p);
    remove_item(r);
    insert_item_at_item(r,q,1);
   }

   int n = 0;
   ab_tree_node* x;
   forall_items(x,*this)
   { n++;
     ab_tree_node* s = succ(x);
     if (s == 0) continue;
     if (pred(s) != x) LEDA_EXCEPTION(1,"succ/pred");
    }
   if (n != count) LEDA_EXCEPTION(1,"count");

 }
*/



void ab_tree::clear()
{ if (root!=0) del_tree(root);
  maximum=minimum=root=0;
  count = 0;
  height=-1;
 }


void ab_tree::del(GenPtr key)
{ if (!root) return;
  bool found;
  ab_tree_node* w=locate(key,found);
  if (found)  del_item(w);
}


ab_tree_elem*  ab_tree::expand(ab_tree_node* v, ab_tree_node* w, 
                                 ab_tree_node* u, int dir)
{
   // expand v by inserting an additional child u to the left (dir==0)
   // or right (dir==1) of w; returns the position of the new child

   ab_tree_elem* p = v->last;
   ab_tree_elem* q = p+1;

   v->last = q;

   if (dir == 0)
      do *q-- = *p--; while (q->child != w);
   else
      while (p->child != w) *q-- = *p--;

   q->child = u;

   return q;
}                     


/*
ab_tree_elem*  ab_tree::expand_left(ab_tree_node* v, ab_tree_node* w, 
                                                        ab_tree_node* u)
{ ab_tree_elem* p = v->last;
  ab_tree_elem* q = p+1;
  v->last = q;
  do *q-- = *p--; while (q->child != w);
  q->child = u;
  return q;
}                     


ab_tree_elem*  ab_tree::expand_right(ab_tree_node* v, ab_tree_node* w, 
                                                         ab_tree_node* u)
{ ab_tree_elem* p = v->last;
  ab_tree_elem* q = p+1;
  v->last = q;
  while (p->child != w) *q-- = *p--;
  q->child = u;
  return q;
}                     
*/



GenPtr  ab_tree::shrink(ab_tree_node* v, ab_tree_elem* pos)
{ GenPtr k = pos->key;
  ab_tree_elem* stop = v->last--;
  while(pos < stop)
  { *pos = *(pos+1);
    pos++;
   }
  return k;
}                     


                     

void ab_tree::split_node(ab_tree_node* v)
{
 /* adding a child increases the degree of v by 1. If v->p<=b after
    adding the new leave, then we are done . Otherwise we have to 
    split v. Splitting can propagate ==> Loop 
    (changes same links between nodes) 
  */

  while (v->last == v->arr+b) 
  {
    if (v == root)  
    { root=new_ab_tree_node(1,v->height+1,0,b+1);
      height++;
      root->arr[0].child=v;
      v->father=root;
     }

    ab_tree_node* y = v->father;

    // u <-- new right brother of v
    ab_tree_node* u = new_ab_tree_node(0,v->height,y,b+1);
    ab_tree_elem* u_pos = expand(y,v,u,1);
    ab_tree_elem* v_pos = u_pos - 1;

    int down = (b+1)/2;
    int up   = b+1-down;

    u_pos->key = v_pos->key;
    v_pos->key = v->arr[down-1].key;


    // split v, i.e. take the rightmost (b+1)/2 children and keys
    // away from v and incorporate them into u and store key v->k[(b+1)/2]
    // in y ( = father()(v))  between the pointers to v and u i.e. at position
    // index of v

    ab_tree_elem*  p      = u->arr;
    ab_tree_elem*  p_stop = p + up;
    ab_tree_elem*  q      = v->arr + down;

    while (p < p_stop)
    { q->child->father = u;
      *p++ = *q++;
     }

    v->last = v->arr + down - 1;
    u->last = u->arr + up - 1;

    v = y;
  }
}


ab_tree_node* ab_tree::insert(GenPtr key, GenPtr inf)
{
 if (root==0) 
 { root=new_ab_tree_node(0,0,0,2);
   copy_key(key);
   copy_inf(inf);
   root->key()  = key;
   root->inf()  = inf;
   root->pred() = 0;
   root->succ() = 0;
   height=0;
   maximum=minimum=root;
   count++;
   return root;
 }


 bool found;
 ab_tree_node* p = locate(key,found);

 if (found)
 { clear_inf(p->inf());
   copy_inf(inf);
   p->inf() = inf; 
   return p;
  }

 ab_tree_node* v;

 if (p==nil)
    v = insert_at_item(maximum,key,inf,1); // new maximum
 else
    v = insert_at_item(p,key,inf,0);

 return v;

}

ab_tree_node* ab_tree::insert_at_item(ab_tree_node* w, GenPtr key, GenPtr inf,
                                                                   int dir)
{ copy_key(key);
  copy_inf(inf);
  ab_tree_node* u = new_ab_tree_node(0,0,0,2);
  u->key() = key;
  u->inf() = inf;
  return insert_item_at_item(u,w,dir);
}



ab_tree_node* ab_tree::insert_item_at_item(ab_tree_node* u, ab_tree_node* w,
                                                                   int dir)
{ 
  // insert leaf u left (dir=0) or right (dir=1) of leaf w

   count++;

   if (w == root) 
   { ab_tree_node* v = new_ab_tree_node(2,1,0,b+1);
     u->father = v;
     u->pred() = 0;
     u->succ() = 0;

     w->father = v;

     ab_tree_node* l = (dir == 0) ? u : w;
     ab_tree_node* r = (dir == 0) ? w : u;

     v->arr[0].child  = l;
     v->arr[1].child  = r;
     v->arr[0].key    = l->arr[0].key;
     l->succ()  = r;
     r->pred()  = l;

     root    = v;
     height  = 1;
     minimum = l;
     maximum = r;

     return u;
   }

   ab_tree_node* v = w->father;

   // new son u left/right of w

   u->father = v;

   ab_tree_elem* u_pos = expand(v,w,u,dir);       

   if (dir == 0)
   { /*        v
             / | \
              (u) w  
      */

     ab_tree_node* pred = w->pred();

     u->succ() = w;
     u->pred() = pred;
     w->pred() = u;

     u_pos->key = u->key();

     if (pred == nil) // w minimum
        minimum = u;
     else 
        pred->succ() = u;

    }
   else 
    { /*       v
             / | \
            w (u)     
       */

     ab_tree_node* succ = w->succ();

     u->pred() = w;
     u->succ() = succ;
     w->succ() = u;

     if (succ == nil) // w maximum  
       { maximum=u;
         (u_pos-1)->key = w->arr[0].key;
        }
      else
       { u_pos->key = u->key();
         succ->pred() = u;
        }

    }

    if (v->last >= v->arr + b) split_node(v);

    return u;
}


ab_tree_node* ab_tree::insert_at_item(ab_tree_node* w, GenPtr key, GenPtr inf)
{ 
   copy_inf(inf);

   int c = cmp(key,w->key());

   if (c == 0)
   { clear_inf(w->inf());
     w->inf() = inf; 
     return w;
    }

/*
   if ( w!=minimum && cmp(w->pred()->key(),key) > 0)
    { cout << "INSERT_AT: WRONG POSITION\n";
      cout << "insert:   key = "; print_key(key); cout << "\n";
      if (w!=maximum) 
      { cout << "succ-pos: key = "; print_key(w->succ()->key()); cout << endl; }
      cout << "position: key = "; print_key(w->key()); cout << "\n";
      cout << "pred-pos: key = "; print_key(w->pred()->key()); cout << "\n";
      LEDA_EXCEPTION(1,"ab_tree::insert_at : wrong position "); 
     }

   if ( w!=maximum && cmp(w->succ()->key(),key) < 0)
    { cout << "INSERT_AT: WRONG POSITION\n";
      cout << "insert:   key = "; print_key(key); cout << "\n";
      cout << "succ-pos: key = "; print_key(w->succ()->key()); cout << "\n";
      cout << "position: key = "; print_key(w->key()); cout << "\n";
      if (w!=minimum)
      { cout << "pred-pos: key = "; print_key(w->pred()->key()); cout << endl; }
      LEDA_EXCEPTION(1,"ab_tree::insert_at : wrong position "); 
     }
*/

   if (c < 0)
      return insert_at_item(w,key,inf,0);
   else
      return insert_at_item(w,key,inf,1);
}



ab_tree_node* ab_tree::locate(GenPtr key, bool& found) const
{
  /* computes the leaf v with key(v) minimal such that key(v) >= key
     (nil) if key is larger than the current maximum
     we search down the tree starting at the root r until we reach 
     a leave. In each node v we use the sequence k[1](v),..k[v->p-1](v) 
     to guide the search.
  */

#define LOCATE_BODY(ktype)\
  ktype k = LEDA_ACCESS(ktype,key);\
  if (k <= LEDA_ACCESS(ktype,maximum->key()))\
  { while (v->height > 0)\
    { ab_tree_elem* p = v->arr;\
      (GenPtr&)(v->last->key) = key;\
      while (k > LEDA_ACCESS(ktype,p->key)) p++;\
      v = p->child;\
     }\
    if (k == LEDA_ACCESS(ktype,v->key())) found = true;\
  } else v = nil;


  ab_tree_node* v = root;
  found = false;

  if (v == nil) return nil;

  if (key_def_order()) {

    switch (key_type_id()) {
  
      case INT_TYPE_ID:   { LOCATE_BODY(int);
                            return v;
                           }
  
      case FLOAT_TYPE_ID: { LOCATE_BODY(float);
                            return v;
                           }
  
      case DOUBLE_TYPE_ID:{ LOCATE_BODY(double);
                            return v;
                           }
      }
  }

  // generic search (using "cmp")

  int c = cmp(key,maximum->key());
  if (c > 0)  return nil;
  if (c == 0)
  { found = true;
    return maximum;
   }

  while (v->height > 0)
  { ab_tree_elem* p = v->arr;
    (GenPtr&)(v->last->key) = key;
    while (cmp(key,p->key) > 0) p++;
    v = p->child;
   }

  if (cmp(key,v->key()) == 0) found = true;

  return v;
}


ab_tree_node* ab_tree::locate_succ(GenPtr key) const
{ bool found;
  return locate(key,found);
 }

ab_tree_node* ab_tree::locate_pred(GenPtr key) const
{ bool found;
  ab_tree_node* v = locate(key,found);
  if (v==0) return maximum;
  if (found) return v;
  return v->pred();
 }


ab_tree_node* ab_tree::lookup(GenPtr k) const 
{ bool found;
  ab_tree_node* p = locate(k,found);
  if (!found) p = 0;
  return p;
 }



void ab_tree::fuse(ab_tree_node* v,ab_tree_node* y)
{

// fuse v and y, i.e. make all sons of y to sons of v and move all
// keys from y to v and delete node y; also move one key (the key
// between the pointers to y and v) from z to v; (note that this will
// shrink z, i.e. decrease the arity of z by one)  

   ab_tree_node* z=v->father;

   ab_tree_elem* v_pos = z->arr; 
   while (v_pos->child != v) v_pos++;

   ab_tree_elem* y_pos = v_pos + 1;

   ab_tree_elem* vp = v->last;

   vp->key = v_pos->key;

   v_pos->key = shrink(z,y_pos);

   ab_tree_elem* yp = y->arr;
   ab_tree_elem* yl = y->last;
   vp++;

   while (yp <= yl)
   { yp->child->father = v;
     *vp++ = *yp++;
    }

   v->last = vp - 1;

   delete_node(y);
   
}



void ab_tree::share(ab_tree_node* v,ab_tree_node* y,int direct)
{

// assume that y is the right brother of v;
// take the leftmost son away from y and make it an additional(right-
// most) son of v; also move one key( the key between the pointers
// to v and y) from z down to v and replace it by the leftmost
// key of y;  the other case is symmetric
// let z be the fatherof v

     ab_tree_node* z=v->father;


     if (direct==1)  
     { 
       // y is right sibling of v

       ab_tree_elem* v_pos = z->arr; 
       while (v_pos->child != v) v_pos++;

       ab_tree_elem*  yp = y->arr;

       (v->last+1)->child = yp->child;
       (v->last+1)->child->father=v;
       v->last->key = v_pos->key;

       v_pos->key = shrink(y,yp);
       
       v->last++;    
     }
     else            
     { // y is left sibling of v

       ab_tree_elem* y_pos = z->arr; 
       while (y_pos->child != y) y_pos++;

       ab_tree_elem* vstop = v->arr;

       for(ab_tree_elem* vp = v->last+1; vp > vstop; vp--) *vp = *(vp-1);

       v->arr[0].child = y->last->child;
       v->arr[0].child->father = v;
       v->arr[0].key = y_pos->key;

       v->last++;
       y->last--;

       y_pos->key=y->last->key;

      }
}




void ab_tree::del_item(ab_tree_node* w)
{ remove_item(w);
  clear_key(w->key());
  clear_inf(w->inf());
  delete_node(w); 
}



void ab_tree::remove_item(ab_tree_node* w)
{
/* we remove leave w with parent v
   we shrink v by deleting leave w and one of the keys in the 
   adjacent to the pointer to w 
   (if w is the i-th son of v then we delete k[i](v) if i<v->p
   k[i-1](v) if i=v->p  ).
   m.w. if i=v->p we overwrite the inner node 
   in which key w->k[1] is stored with k[i-1](v)
   and then delete k[i-1](v)
 */

  if (w==nil) LEDA_EXCEPTION(1,"ab_tree: nil item in del_item");

  count--;

  if (count == 0)
  { maximum=minimum=root=0; 
    height=-1; 
    return;
   }

  ab_tree_node* succ = w->succ();
  ab_tree_node* pred = w->pred();

  if (pred) 
     pred->succ() = succ;
  else 
     minimum = succ;

  if (succ) 
     succ->pred() = pred;
  else  
     maximum = pred;


  ab_tree_node* v = w->father;

  ab_tree_elem* w_pos = v->arr; 
  while (w_pos->child != w) w_pos++;

  if (w_pos == v->last) 
  { //overwrite copy in inner node u
    ab_tree_elem* u = same(w);
    if (u) u->key=pred->arr[0].key;  
    v->last--;
   }
  else
   shrink(v,w_pos);


  while ( v->last < v->arr+a-1 ) // rebalancing
  { 
    if (v == root)
    { if (v->last == v->arr)
      { ab_tree_node* z = v;
        root = v->arr[0].child;
        root->father = 0;
        height--;
        delete_node(z);  
       }
      break;
     }

    ab_tree_node* z = v->father;

    ab_tree_elem* v_pos = z->arr; 
    while (v_pos->child != v) v_pos++;

    ab_tree_node* x = nil;
    ab_tree_node* y = nil;

    if (v_pos > z->arr)   x = (v_pos-1)->child; // left sibling
    if (v_pos < z->last)  y = (v_pos+1)->child; // right sibling


    if (y && y->last >= y->arr+a) 
    { share(v,y,1);
      break;
     }

    if (x && x->last >= x->arr+a) 
    { share(v,x,0);
      break;
     }

    if (x) 
       fuse(x,v); 
    else  
       fuse(v,y); 

    v = z;       
  }

}


void ab_tree::pr_ab_tree(ab_tree_node* localroot,int blancs) const

{ 
  if (localroot==0)
   { for(int j=1;j<=blancs;j++) cout<<" ";
     cout << "NIL\n";
     return;
    }
  
  if (localroot->height == 0) 
   { for(int j=1;j<=blancs;j++) cout<<" ";
     print_key(localroot->arr[0].key); 
     cout << "\n";
    }

   else
    { for(ab_tree_elem* p = localroot->arr; p <= localroot->last; p++)
      { pr_ab_tree(p->child,blancs+10);
        for(int j=1;j<=blancs;j++) cout<<" ";
        print_key(p->key); 
        cout << endl;
       }
      pr_ab_tree(localroot->last->child,blancs+10);
    }
} 
 
ab_tree_node* ab_tree::copy_ab_tree(ab_tree_node* localroot,
                                    ab_tree_node*& last_leaf,int b0) const
{ 
  ab_tree_node* r;

  int deg = int(localroot->last - localroot->arr) + 1;

  if (localroot->height == 0)   //leaf
   { r=new_ab_tree_node(0,0,0,2); 

     r->key() = localroot->key();
     r->inf() = localroot->inf();

     copy_key(r->key());
     copy_inf(r->inf());

     r->pred()=last_leaf;
     if (last_leaf) last_leaf->succ() = r;
     last_leaf = r;               

    }
  else
   { r=new_ab_tree_node(deg,localroot->height,0,b0+1); 
     for(int i=0; i < deg-1; i++)
     { r->arr[i].child=copy_ab_tree(localroot->arr[i].child,last_leaf,b0);
       r->arr[i].child->father=r;
       r->arr[i].key=localroot->arr[i].key;
      }

     r->last->child = copy_ab_tree(localroot->last->child,last_leaf,b0);
     r->last->child->father=r;
   }

  return r;
}
        
void ab_tree::del_tree(ab_tree_node* localroot)
{ 
  if (localroot->height > 0)
   { ab_tree_elem* last = localroot->last;
     for(ab_tree_elem* p=localroot->arr; p<=last; p++) del_tree(p->child);
    }
  else // leaf
  { clear_key(localroot->key());
    clear_inf(localroot->inf());
   }

  delete_node(localroot);
}

void ab_tree::change_inf(ab_tree_node* p, GenPtr x) 
{ clear_inf(p->inf());
  copy_inf(x);
  p->inf() = x;
 }


void ab_tree::decrease_key(ab_tree_node* p, GenPtr k) 
{ GenPtr i = p->inf();
  copy_key(i);
  del_item(p);
  insert(k,i);
  clear_key(i);
 }


void ab_tree::conc(ab_tree&, int)
{ LEDA_EXCEPTION(1,"sorry, not implemented: ab_tree::conc(ab_tree)\n"); }

void ab_tree::split_at_item(ab_tree_node*,ab_tree&,ab_tree&, int)
{ LEDA_EXCEPTION(1,"sorry, not implemented: skiplist::split_at_item\n"); }



#if defined(CONCATENATE_AND_SPLIT)

ab_tree& ab_tree::conc(ab_tree& s2)

{ 
  if ((a!=s2.a)||(b!=s2.b)) 
     LEDA_EXCEPTION(1,"ab_tree: incompatible trees in concatenate operation");

  if (s2.root==0) return *this;

  if (root==0) 
  { root=s2.root;
    maximum=s2.maximum;
    minimum=s2.minimum;
    height=s2.height;
    count =s2.count;
   }
  else
  { if (cmp(maximum->k[1],s2.minimum->k[1])>=0) 
                    LEDA_EXCEPTION(1,"ab_tree: join(S,T) : max(S)>=min(T)"); 

    concat(*this,s2,maximum,maximum->k[1]);

    // link leaves 
    maximum->succ()=s2.minimum;       
    s2.minimum->pred()=maximum;

    maximum=s2.maximum;              
   }

  s2.root=0;
  s2.maximum=0;
  s2.minimum=0;
  s2.height=-1;

  return *this;
}


/*---------------------------------------------------------------------
  global functions
----------------------------------------------------------------------*/

void concat(ab_tree& s1,ab_tree& s2,ab_tree_node* current,GenPtr cur_key)
{ 
  // Result in s1

  ab_tree_node* v=s1.root;
  ab_tree_node* w=s2.root;
  int h1=v->height;     
  int h2=w->height;
  int i;

  if(h1==h2)
     { ab_tree_node* z=new_ab_tree_node(2,h1+1,0,s1.b);
       z->son[1]=v;
       z->son[2]=w; 
       z->k[1]=cur_key;
       z->son[1]->father=z; 
       z->son[2]->father=z;
       s1.height++;
       s1.root=z;
    }
  else { if (h1>h2)
         {
            for(i=1;i<h1-h2;i++,v=v->son[v->p]);  
            v->son[v->p+1]=w;
            v->son[v->p+1]->father=v;
            v->k[v->p]=cur_key;
 	    v->p++;
	    if (v->p==s1.b+1)  {s1.split_node(v);  };
        }
        else /* h1<h2 */
        {
	   for(i=1;i<=h2-h1-1;i++,w=w->son[1]);
           for(i=w->p;i>1;i--)
            { w->son[i+1]=w->son[i];
              w->son[i+1]->father=w;
              w->k[i]=w->k[i-1];
            };
           w->p++;
           w->son[2]=w->son[1];
           w->son[2]->father=w;
           w->son[1]=v;
           w->son[1]->father=w;
           w->k[1]=cur_key;
           if (w->p==s2.b+1) {s2.split_node(w);};
	   s1.root =  s2.root;
	   s1.height =  s2.height;
        }
      }

  /* maximum/minimum are now undefined  */

}



void ab_tree::split_at_item(ab_tree_node* w,ab_tree& L,ab_tree& R)
  {
    if(((a!=L.a)||(a!=R.a))||((b!=L.b)||(b!=R.b)))
       LEDA_EXCEPTION(1,"ab_tree: incompatible trees in split operation");
    
    /* initialisation   */
    L.root=L.minimum=L.maximum=0;L.height=-1;
    R.root=R.minimum=R.maximum=0;R.height=-1;

    if(root==0) return;

    if (w==0) 
    { R.root = root;
      R.height = height;
      R.maximum = maximum;
      R.minimum = minimum;
      R.count = count;
      root = 0;
      height = -1;
      maximum = 0;
      minimum = 0;
      count = 0;
      return;
     }

    if (w==maximum) 
    { L.root = root;
      L.height = height;
      L.maximum = maximum;
      L.minimum = minimum;
      L.count = count;
      root = 0;
      height = -1;
      maximum = 0;
      minimum = 0;
      count = 0;
      return;
     }

    ab_tree_node* l;
    ab_tree_node* r;    // pointers to the roots of the left and right subtree


    /* parameters for concat  */

    ab_tree_node* current_l=0 ;
    GenPtr           current_l_key=0;

    ab_tree_node* current_r=0;  
    GenPtr           current_r_key=0;

    int i;


    /* w is a pointer to the leave y  */
    ab_tree_node* v;

    /* store leaf to split at         */
    ab_tree_node* leaf=w;


     l = w;
     r = 0;

      do{
         v = w->father;

         //int w_pos = index(w,v);

         ab_tree_elem* w_pos = v->arr; 
         while (w_pos->child != w) w_pos++;

       /* now we have construct the  left and right subtrees and the pointers
          to the roots  --> we must construct two trees with these roots*/

        if ((L.root==0)&&(l!=0))  { L.root=l;
			            L.height=l->height; 
			            L.root->father=0;
			          }
        else { if ((L.root!=0)&&(l!=0))
                 {  ab_tree L1(L.a,L.b);
	            L1.root=l;
                    L1.height=l->height;
                    L1.root->father=0;
	            concat(L1,L,current_l,current_l_key);
	            L.root  = L1.root;
                    L.height= L1.height;
                    L.count = L1.count;

                    L1.root=0;
	         }
             }
       if ((R.root==0)&&(r!=0))  {R.root=r;
		        	  R.height=r->height;
			          R.root->father=0;
                                  R.root->p=r->p;
			         }
       else { if ((R.root!=0)&&(r!=0))
                { ab_tree R1(R.a,R.b);
	  	  R1.root=r;
		  R1.height=r->height;
                  R1.root->father=0;
                  R1.root->p=r->p;
		  concat(R,R1,current_r,current_r_key);
                  R1.root=0;
	        }
            }

        if (v!=0)
        {
         if (w_pos==1)     /* w is leftmost son of v */
         { l=0;
           r=v;
           int pos;
           current_r=same(v,pos);
           current_r_key=v->k[1];
	   for(i=2;i<r->p;i++) 
            {  r->son[i-1]=r->son[i];
 	       r->k[i-1]=r->k[i];
    	    }
           r->son[r->p-1]=r->son[r->p];    /* last son */
           r->son[r->p]=0;
           r->p--; 
           r->k[r->p]=0;
           if (r->p==1) r=r->son[1];
         } 
         else {if ( w_pos==v->p )
                 {  r=0;
                    l=v;
		    l->son[l->p]=0;  /* last son */
		    l->p--;
		    current_l=same_leaf(l,w_pos-1);
		    current_l_key=current_l->k[1];
                    l->k[l->p]=0;
                    if (l->p==1) l=l->son[1];
		} 
               else  /* if w is not the leftmost or rightmost son of v*/
               {  
                 r=v;
                 l=new_ab_tree_node(w_pos-1,v->height,0,R.b);
		 current_l=same_leaf(v,w_pos-1);
 		 current_l_key=current_l->k[1];
		 current_r=same_leaf(v,w_pos);
		 current_r_key=current_r->k[1];
		 // current_r=(v->k[w_pos])-1;   ERROR: liefert neuen Schluessel ;
                 for(i=1;i<w_pos-1;i++)
   		  {
		   l->son[i]=v->son[i];
                   l->son[i]->father=l;
		   l->k[i]=v->k[i];
		  };
		 l->son[w_pos-1]=v->son[w_pos-1];
                 l->son[w_pos-1]->father=l;

                 r->son[w_pos] = 0;   // changed

    		 for (i=1;i<r->p-w_pos;i++)
		  {
		   r->son[i]=r->son[i+w_pos];
                   r->son[i+w_pos]=0;
                   r->son[i]->father=r;
		   r->k[i]=r->k[i+w_pos];
                   r->k[i+w_pos]=0;
		  };
	         r->son[r->p-w_pos]=r->son[r->p];  /* last son */
                 r->son[r->p]=0;
                 r->son[r->p-w_pos]->father=r;
		 r->p=r->p-w_pos;
                 if (l->p==1) l=l->son[1];
                 if (r->p==1) r=r->son[1];
                }
               }
              }

 /* initialisation for the next iteration  */
  w=v;
 }
 while (w!=0);


 /* unlink leaves    m.w.         */
 leaf->succ()->pred()=0;
 leaf->succ()=0;

 /* redefine maximum and minimum  */
 L.minimum=minimum;
 ab_tree_node* help=L.root;
 while (help->p) help=help->son[help->p];
 L.maximum=help;
 help=R.root;
 while (help->pred()!=0) help=help->pred();
 R.minimum=help;
 R.maximum=maximum;

 maximum=minimum=root=l=r=0;
 height=-1; 
 count = 0;

 delete_node(l);
 delete_node(r);

}


#endif

LEDA_END_NAMESPACE
