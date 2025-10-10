/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _bin_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
//
//  bin_tree: base class for all binary tree types in LEDA
//
//  leaf oriented & doubly linked 
//
//  S. N"aher (1993)
//
//  - set operations: union & intersection  (1995)
//------------------------------------------------------------------------------

#include <LEDA/core/impl/bin_tree.h>

LEDA_BEGIN_NAMESPACE

bin_tree_node* bin_tree::lookup(GenPtr x) const
{ bin_tree_node* p = locate(x);
  if (p && cmp(p->k,x) == 0) return p;
  return 0;
}

void  bin_tree::change_inf(bin_tree_node* p,GenPtr y) 
{ clear_inf(p->i);
  copy_inf(y);
  p->i = y; 
 }


//------------------------------------------------------------------------------
// locate(x) : returns pointer to leaf with successor or predessor key of x
//------------------------------------------------------------------------------

bin_tree_node* bin_tree::locate(GenPtr x) const
{ return  (count > 0) ? search(x) : 0; }

bin_tree_node* bin_tree::locate(GenPtr x, bool& found) const
{ bin_tree_node* p = locate(x);
  found =  (p && cmp(p->k,x) == 0);
  return p;
}


//------------------------------------------------------------------------------
// locate_succ(x) : returns pointer to leaf with minimal key >= x
//             nil if tree empty
//------------------------------------------------------------------------------

bin_tree_node* bin_tree::locate_succ(GenPtr x) const
{ if (count==0) return 0;
  bin_tree_node* p =  search(x);
  return (cmp(x,p->k) > 0) ? succ(p) : p ;
 }



//------------------------------------------------------------------------------
// locate_pred(x) : returns pointer to leaf with maximal key <= x
//                  nil if tree empty
//------------------------------------------------------------------------------

bin_tree_node* bin_tree::locate_pred(GenPtr x) const 
{ if (count==0) return 0;
  bin_tree_node* p = search(x);
  return (cmp(x,p->k) < 0) ? pred(p) : p ;
 }



//------------------------------------------------------------------------------
// search(x) : returns pointer to leaf with minimal key >= x
//------------------------------------------------------------------------------

bin_tree_node* bin_tree::search(GenPtr x) const
{ bin_tree_node* p = get_root();

  switch (key_type_id()) {

   case INT_TYPE_ID: {
        int ix =  LEDA_ACCESS(int,x);
        while (p->is_node())
           p = p->child[ (ix <= LEDA_ACCESS(int,p->k)) ? left : right ];
        break;
     }

   case DOUBLE_TYPE_ID: {
        double dx =  LEDA_ACCESS(double,x);
        while (p->is_node())
           p = p->child[ (dx <= LEDA_ACCESS(double,p->k)) ? left : right ];
        break;
      }

   default: {
        while (p->is_node())
           p = p->child[ (cmp(x,p->k) <= 0) ? left : right ];
        break;
      }
   }

  return p;
}


//------------------------------------------------------------------------------
// insert(x,y,ii):
// inserts new leaf with key x and inf y, sets inf of new inner node to ii
// returns pointer to the new leaf
//------------------------------------------------------------------------------

bin_tree_node* bin_tree::insert( GenPtr x, GenPtr y, GenPtr ii)
{  
  // key x, inf y, iinf ii

  bin_tree_node* p;

   if (count==0)  // tree is empty 
     { copy_key(x);
       copy_inf(y);
       p = new bin_tree_node(x,y,leaf_balance());
       p->corr = &ROOT;
       ROOT.i = ii;
       set_min(p);
       set_root(p);
       p->parent = &ROOT;
       p->child[right] = p;
       p->child[left] = p;
       count = 1;
       last_inner_node = 0;
     }
   else
     { p = search(x);
       p = insert_at_item(p,x,y,ii);
      }

   return p ;
 }



bin_tree_node* bin_tree::insert_at_item(bin_tree_node* p, GenPtr x, GenPtr y, 
                                                                    GenPtr ii)
{

   bool insert_left;  //  true <==> insert new leaf q to the left of p

   copy_inf(y);

   int c = cmp(x,p->k);

   if ( c == 0 )
   { clear_inf(p->i);
     p->i = y;
     return p;
    }

   insert_left = (c < 0);
   copy_key(x);

   int b = (count==1) ? root_balance() : node_balance();

   count++;

   bin_tree_node* q = new bin_tree_node(x,y,leaf_balance()); // new leaf
   bin_tree_node* r = new bin_tree_node(0,0,b);              // new inner node
   bin_tree_node* u = p->parent; 

   q->parent = r;
   p->parent = r;
   r->parent = u;
   r->corr = nil;

   if (p == u->child[left])
      u->child[left] = r;
   else
      u->child[right] = r;

   // insertion of q, adjusting key & inf of corresponding inner nodes,
   // double-chaining with neighbors, checking min pointer, ...

   if (insert_left) // insert q left of p
      { r->k = x;
        q->corr = r;
        r->i = ii;
        r->child[left] = q;
        r->child[right]= p;
        u = p->child[left];
        q->child[left]  = u;
        q->child[right] = p;
        u->child[right] = q;
        p->child[left]  = q;
        if ( p == get_min() ) set_min(q);     
       }
    else         // insert q right of p
      { bin_tree_node* pc = p->corr; 
        r->k = p->k;
        r->i = pc->i;
        pc->i = ii;
        q->corr = pc;
        p->corr = r;
        r->child[left] = p;
        r->child[right]= q;
        u = p->child[right];
        q->child[left]  = p;
        q->child[right] = u;
        u->child[left] = q;
        p->child[right] = q;
       }

    propagate_modification(1,r,p);
    propagate_modification(2,r,q);

    if (r != get_root()) insert_rebal(r);

    last_inner_node = r; 

    return q;
}



//------------------------------------------------------------------------------
// del(x) 
// removes leaf with key x from the tree
// overwrites possible copy of x in an inner node (if key type is not integer)
//------------------------------------------------------------------------------

void bin_tree::del(GenPtr x)
{
  if (count == 0) return;  // tree is empty

  bin_tree_item v = search(x);

  if ( cmp(v->k,x) == 0 ) del_item(v);

 }


void bin_tree::del_item(bin_tree_item v)
{
  bin_tree_item w;
  bin_tree_item p = v->parent;
  bin_tree_item u = v->corr;

  clear_key(v->k);
  clear_inf(v->i);

  // overwrite copy of key in corresponding inner node u by its predecessor,
  // but keep its information (not necessary in the case that v is a left child)

  if (v != p->child[left]) 
  { bin_tree_node* pred =  v->child[left];
    u->k = pred->k;
    clear_iinf(pred->corr->i);
    pred->corr = u;
  }
  else
    clear_iinf(u->i);

  count--;
 
  if (count == 0)      // tree is now empty
  { set_root(nil);
    set_min(nil);
    delete v;
    return;
   } 


  bin_tree_node* pred = v->child[left];
  bin_tree_node* succ = v->child[right];

  // link neighbors
  pred->child[right] = succ; 
  succ->child[left] = pred;

  // adjust min pointer
  if ( v == get_min() ) set_min(succ);


  // replace p by sibling w of v

  u = p->parent;
  w = p->child[left];
  if (v == w) w =  p->child[right];
  w->parent = u;
  if (p == u->child[left])
     u->child[left] = w;
  else
     u->child[right] = w;

                                /*     u             u            u   */
                                /*     |             |            |   */
                                /*     p     or      p    --->    w   */
                                /*    / \           / \               */
                                /*   v   w         w   v              */

  propagate_modification(3,u,w);

  // rebalance tree, if necessary

  if (w != get_root()) del_rebal(w,p);

  delete v;
  delete p;

}


//------------------------------------------------------------------
// concatenate
//------------------------------------------------------------------

bin_tree& bin_tree::conc(bin_tree&, int) 
{ LEDA_EXCEPTION(1,"sorry, bin_tree::conc not implemented"); 
  return *this;
 }

//------------------------------------------------------------------
// split at item
//------------------------------------------------------------------

void bin_tree::split_at_item(bin_tree_node*,bin_tree&,bin_tree&, int) 
{ LEDA_EXCEPTION(1,"sorry, bin_tree::split_at_item not implemented"); }


//------------------------------------------------------------------
// reverse items
//------------------------------------------------------------------

void bin_tree::reverse_items(bin_tree_node* v, bin_tree_node* w)
{
  bin_tree_node* l = v;
  bin_tree_node* r = w;

  while (l != r && r->child[right] != l) 
  {
    bin_tree_node* pl = l->parent;
    bin_tree_node* pr = r->parent;
    bin_tree_node* cl = l->corr;
    bin_tree_node* cr = r->corr;


    // exchange l and r

   if (pl == pr)
     { pl->child[left] = r;
       pl->child[right] = l;
      }
   else
     { if (l == pl->child[left])
          pl->child[left] = r;
       else
          pl->child[right] = r;
    
       r->parent = pl;
    
       if (r == pr->child[left])
          pr->child[left] = l;
       else
          pr->child[right] = l;
    
       l->parent = pr;
      }
  

   // update corresponding inner nodes

   l->corr = cr;
   cr->k = l->k;

   r->corr = cl;
   cl->k = r->k;

   l = l->child[right]; 
   r = r->child[left];

  }

  // reverse chaining of leaves v...w

  if (count > 2)
  { l = v->child[left];
    r = w->child[right];
  
    r->child[left] = v;
  
    bin_tree_node* p = v; 
  
    while(p != w)
    { bin_tree_node* q = p->child[right];
      p->child[left] = q;
      p = q;
     }
  
    w->child[left] = l;
  
    p = r;
  
    while ( p != l )
    { bin_tree_node* q = p->child[left];
      q->child[right] = p;
      p = q;
     }
   }
  

  // adjust min pointer
  
  if (v == get_min()) set_min(w);

 }


//------------------------------------------------------------------
// operator=
//------------------------------------------------------------------

bin_tree& bin_tree::operator=(const bin_tree& t)
{
   if (this == &t) return *this;

   clear();

   if ( t.get_root() )   
   { bin_tree_node* pre = 0;
     set_root(t.copy_tree(t.get_root(),pre));
     get_root()->parent = &ROOT;
     pre->corr = &ROOT;
     ROOT.i = t.ROOT.i;
     set_min(get_root());
     while (get_min()->child[left]) set_min(get_min()->child[left]);
     get_min()->child[left] = pre;
     pre->child[right] = get_min();
     count = t.count;
   }

   return *this; 
}

//------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------

  bin_tree::bin_tree(const bin_tree& t)
  { set_root(nil);
    set_min(nil);
    count = t.count; 
    if ( t.get_root() ) 
    { bin_tree_node* pre = 0;
      set_root(t.copy_tree(t.get_root(),pre));
      set_min(get_root());
      get_root()->parent = &ROOT;
      pre->corr = &ROOT;
      while (get_min()->child[left]) set_min(get_min()->child[left]);
      get_min()->child[left] = pre;
      pre->child[right] = get_min();
     }
  }


//------------------------------------------------------------------
// copy_tree(p) makes a copy of tree with root p and returns a pointer to the
// root of the copy. pre is last created leaf ( leaves are created from left 
// to right).
//------------------------------------------------------------------

bin_tree_node* bin_tree::copy_tree( bin_tree_node* p, bin_tree_item& pre) const
{
  bin_tree_node* q = new bin_tree_node(p);

  q->corr = nil;

  if ( p->is_node() )  // internal node: copy subtrees 
  { q->child[left] = copy_tree(p->child[left],pre);
    pre->corr = q;
    q->k = pre->k;
    q->child[right] = copy_tree(p->child[right],pre);
    q->child[left]->parent = q;
    q->child[right]->parent = q;
  }
  else   //leaf: chaining with last created leaf "pre"
  { copy_key(q->k);
    copy_inf(q->i);
    if (pre) pre->child[right] = q; 
    q->child[left] = pre;
    pre = q;

   }

  return q;
}

//------------------------------------------------------------------
// clear
//------------------------------------------------------------------

void bin_tree::clear() 
{
   if ( get_root() )
   { del_tree(get_root());
     set_root(nil);
     set_min(nil);
     count = 0;
   }
}

//------------------------------------------------------------------
// del_tree(p) : deletes subtree rooted at node p
//------------------------------------------------------------------

void bin_tree::del_tree(bin_tree_node* p)
{
  if ( p->is_node() )
  { del_tree(p->child[left]);
    del_tree(p->child[right]);
   }
  else
  { clear_key(p->k);
    clear_inf(p->i);
    clear_iinf(p->corr->i);
   }

  delete p;
}


//----------------------------------------------------------------------------
// set operations
//----------------------------------------------------------------------------

void bin_tree::add(const bin_tree& T)
{ bin_tree_node* p;
  forall_items(p,T) insert(p->k,p->i);
 }

void bin_tree::intersect_with(const bin_tree& T)
{ bin_tree_node* p = first_item();
  while(p)
  { bin_tree_node* q = next_item(p);
    if (T.lookup(p->k) == nil) del_item(p);
    p = q;
   }
}

void bin_tree::subtract(const bin_tree& T)
{ bin_tree_node* p;
  forall_items(p,T) del(p->k);
 }

bool bin_tree::contains(const bin_tree& T) const
{ bin_tree_node* p;
  forall_items(p,T) 
       if (lookup(p->k) == nil) return false;
  return true;
 }



//----------------------------------------------------------------------------
// printing and drawing trees
//----------------------------------------------------------------------------

void bin_tree::print() const
{ cout << "size = " << size() << endl;
  if ( get_root() ) print_tree(get_root(),1);
  cout << endl;
}


void bin_tree::print_tree(bin_tree_node* p,int h) const
{  
  if ( p->is_node() ) print_tree(p->child[right],h+1);

 for( int j=1; j <= h ; j++ ) cout << "     ";

 print_key(key(p));
 cout << " bal=" << p->get_bal();

 if ( p->is_leaf() )
 { cout << " [" << p << "] ";  
   cout << " succ[" << p->child[right] << "] ";  
   cout << " pred[" << p->child[left] << "] "; 
   cout << endl;
 }
 else cout << "\n";

 if ( p->is_node() ) print_tree(p->child[left],h+1);
}

        

void bin_tree::draw(DRAW_BIN_NODE_FCT draw_node,
                    DRAW_BIN_EDGE_FCT draw_edge, 
                    double x1, double x2, double y, double ydist)
{ 
  // draw a picture of the tree using the functions
  // draw_node(x,y,k,b)   draws node with key k and balance b at (x,y)
  // draw_edge(x,y,x',y') draws an edge from (x,y) to (x',y')

  draw(draw_node,draw_edge,get_root(),x1,x2,y,ydist,0); 
 }



void bin_tree::draw(DRAW_BIN_NODE_FCT draw_node,
                    DRAW_BIN_EDGE_FCT draw_edge, 
                    bin_tree_node* r, 
                    double x1, double x2, double y, 
                    double ydist, double last_x)
{ 
  // draw subtree rooted at r

  double x = (x1+x2)/2;

  if (r==nil) return;

  if (last_x != 0) draw_edge(last_x,y+ydist,x,y);

  draw_node(r,x,y);

  if (r->is_node()) 
  { draw(draw_node,draw_edge, r->child[0],x1,x,y-ydist,ydist,x);
    draw(draw_node,draw_edge, r->child[1],x,x2,y-ydist,ydist,x);
   }
}

LEDA_END_NAMESPACE
