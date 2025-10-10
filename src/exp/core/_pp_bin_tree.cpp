/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _pp_bin_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
//
//  pp_btree: base class for persistent binary tree types in LEDA
//
//  leaf oriented & doubly linked 
//
//  Sven Thiel (1998); ephemeral version: S. Naeher (1993)
//------------------------------------------------------------------------------


#include <LEDA/core/impl/pp_bin_tree.h>

//------------------------------------------------------------------------------
// special functions for persistence:
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

void pp_btree::copy_info(GenPtr& p, InfoIndex idx, Anchor& a)
{
  pp_btree_node& n = (pp_btree_node&) a;

  switch (idx) {
  case PP_BTREE_KEY: 
          if (n.is_leaf()) 
	  {
	    copy_key(p); 
	  }

	  break;
  case PP_BTREE_INF: 
          if (n.is_leaf()) 
	  {
	    copy_inf(p);
	  }
	  else 
	  {
	    copy_iinf(p); 
	  }

	  break;
  }
}

void pp_btree::clear_info(GenPtr& p, InfoIndex idx, Anchor& a)
{
  pp_btree_node& n = (pp_btree_node&) a;

  switch (idx) {
  case PP_BTREE_KEY: 
          if (n.is_leaf()) 
	  {
	    clear_key(p); 
	  }

	  break;
  case PP_BTREE_INF: 
          if (n.is_leaf()) 
	  {
	    clear_inf(p);
	  }
	  else 
	  {
	    clear_iinf(p);
	  }

	  break;
  }
}

//------------------------------------------------------------------------------

pp_btree_node* pp_btree::lookup(GenPtr x) const
{ pp_btree_node* p = locate(x);
  if (p && cmp(p->k,x) == 0) return p;
  return 0;
}

void  pp_btree::change_inf(pp_btree_node* p,GenPtr y) 
{
  copy_inf(y);
  p->i = y; 
 }

//------------------------------------------------------------------------------
// locate(x) : returns pointer to leaf with successor or predessor key of x
//------------------------------------------------------------------------------

pp_btree_node* pp_btree::locate(GenPtr x) const
{ return  (size() > 0) ? search(x) : 0; }

//------------------------------------------------------------------------------
// locate_succ(x) : returns pointer to leaf with minimal key >= x
//             nil if tree empty
//------------------------------------------------------------------------------

pp_btree_node* pp_btree::locate_succ(GenPtr x) const
{ if (size()==0) return 0;
  pp_btree_node* p =  search(x);
  return (cmp(x,p->k) > 0) ? succ(p) : p ;
 }

//------------------------------------------------------------------------------
// locate_pred(x) : returns pointer to leaf with maximal key <= x
//                  nil if tree empty
//------------------------------------------------------------------------------

pp_btree_node* pp_btree::locate_pred(GenPtr x) const 
{ if (size()==0) return 0;
  pp_btree_node* p = search(x);
  return (cmp(x,p->k) < 0) ? pred(p) : p ;
 }

//------------------------------------------------------------------------------
// search(x) : returns pointer to leaf with minimal key >= x
//------------------------------------------------------------------------------

pp_btree_node* pp_btree::search(GenPtr x) const
{ const pp_btree_node* p = root(); 

  switch (type_id()) {

   case INT_TYPE_ID: {
        int ix =  LEDA_ACCESS(int,x);
        while (p->is_node())
           p = p->child( (ix <= LEDA_CONST_ACCESS(int,p->k)) ? left:right);
        break;
     }

   case DOUBLE_TYPE_ID: {
        double dx =  LEDA_ACCESS(double,x);
        while (p->is_node())
           p = p->child( (dx <= LEDA_CONST_ACCESS(double,p->k)) ? left:right);
        break;
      }

   default: {
        while (p->is_node())
           p = p->child( (cmp(x,p->k) <= 0) ? left : right );
        break;
      }
   }

  return __const_cast(pp_btree_node*)(p);
}

//------------------------------------------------------------------------------
// insert(x,y,ii):
// inserts new leaf with key x and inf y, sets inf of new inner node to ii
// returns pointer to the new leaf
//------------------------------------------------------------------------------

pp_btree_node* pp_btree::insert(GenPtr x, GenPtr y, GenPtr ii)
{  
  // key x, inf y, iinf ii

  pp_btree_node* p;

   if (size()==0)  // tree is empty 
     { copy_key(x);
       copy_inf(y);
       p = new pp_btree_node(*this,x,y,leaf_balance());
       p->corr = ROOT;
       ROOT->i = ii;
       min_ptr() = p;
       root() = p;
       p->parent = ROOT;
       p->right = p;
       p->left = p;
       pers_data->count = 1;
     }
   else
     { p = search(x);
       p = insert_at_item(p,x,y,ii);
      }

   return p ;
}

pp_btree_node* pp_btree::insert_at_item(pp_btree_node* p, GenPtr x, GenPtr y, 
                                                                    GenPtr ii)
{
   bool insert_left;  //  true <==> insert new leaf q to the left of p

   copy_inf(y);
                   
   int c = cmp(x,p->k);

   if ( c == 0 )
   {
     p->i = y;
     return p;
    }

   insert_left = (c < 0);
   copy_key(x);

   int b = (size()==1) ? root_balance() : node_balance();

   pers_data->count = size()+1;

   pp_btree_node* q = new pp_btree_node(*this,x,y,leaf_balance()); // new leaf
   pp_btree_node* r = new pp_btree_node(*this,b); // new inner node
   r->corr = nil;

   // insertion of q, adjusting key & inf of corresponding inner nodes,
   // double-chaining with neighbors, checking min pointer, ...

   if (insert_left) // insert q left of p
      { q->corr = r;
        r->k = q->k;
        r->i = ii;
        r->left = q;
        r->right= p;
        pp_btree_node* pl = p->left;
        q->left  = pl;
        q->right = p;
        pl->right = q;
        p->left  = q;
        if ( p == min_ptr() ) min_ptr() = q;     
       }
    else         // insert q right of p
      { pp_btree_node* pc = p->corr; 
        p->corr = r;
        q->corr = pc;
        r->k = p->k;
        r->i = pc->i;
        pc->i = ii;
        r->left = p;
        r->right= q;
        pp_btree_node* pr = p->right;
        q->left  = p;
        q->right = pr;
        p->right = q;
        pr->left = q;
       }

   pp_btree_node* u = p->parent; 
   q->parent = r;
   p->parent = r;
   r->parent = u;
   if (p == u->left)
      u->left = r;
   else
      u->right = r;

    propagate_modification(1,r,p);
    propagate_modification(2,r,q);

    if (r != root()) insert_rebal(r);

    return q;
}

//------------------------------------------------------------------------------
// del(x) 
// removes leaf with key x from the tree
// overwrites possible copy of x in an inner node (if key type is not integer)
//------------------------------------------------------------------------------

void pp_btree::del(GenPtr x)
{
  if (size() == 0) return;  // tree is empty

  pp_btree_item v = search(x);

  if ( cmp(v->k,x) == 0 ) del_item(v);
 }

void pp_btree::del_item(pp_btree_item v)
{
  //unsigned cur_stamp = v->get_current_stamp();
  
  pp_btree_item w;
  pp_btree_item p = v->parent;
  pp_btree_item u = v->corr;

  // overwrite copy of key in corresponding inner node u by its predecessor,
  // but keep its information (not necessary in the case that v is a left child)

  if (v != p->left) 
  { pp_btree_node* pred =  v->left;
    u->k = pred->k;
    //clear_iinf(pred->corr->i);
    pred->corr = u; // pred bleibt Blatt
  }

  pers_data->count = size()-1;
 
  if (size() == 0)      // tree is now empty
  { root() = min_ptr() = nil;
    p_delete(v);
    return;
   } 

  pp_btree_node* pred = v->left;
  pp_btree_node* succ = v->right;

  // link neighbors
  pred->right = succ; 
  succ->left = pred;

  // adjust min pointer
  if ( v == min_ptr() ) min_ptr() = succ;

  // replace p by sibling w of v

  u = p->parent;
  w = p->left;
  if (v == w) w =  p->right;
  w->parent = u;
  if (p == u->left)
     u->left = w;
  else
     u->right = w;

                                /*     u             u            u  */
                                /*     |             |            |  */
                                /*     p     or      p    --->    w  */
                                /*    / \           / \              */
                                /*   v   w         w   v             */

  propagate_modification(3,u,w);

  // rebalance tree, if necessary

  if (w != root()) del_rebal(w,p);

  p_delete(v);
  p_delete(p);
}

//------------------------------------------------------------------
// concatenate
//------------------------------------------------------------------

pp_btree& pp_btree::conc(pp_btree&) 
{ LEDA_EXCEPTION(1,"sorry, pp_btree::conc not implemented"); 
  return *this;
 }

//------------------------------------------------------------------
// split at item
//------------------------------------------------------------------

void pp_btree::split_at_item(pp_btree_node*,pp_btree&,pp_btree&) 
{ LEDA_EXCEPTION(1,"sorry, pp_btree::split_at_item not implemented"); }

//------------------------------------------------------------------
// reverse items
//------------------------------------------------------------------

void pp_btree::reverse_items(pp_btree_node* v, pp_btree_node* w)
{
  pp_btree_node* l = v;
  pp_btree_node* r = w;

  while (l != r && r->right != l) 
  {
    pp_btree_node* pl = l->parent;
    pp_btree_node* pr = r->parent;
    pp_btree_node* cl = l->corr;
    pp_btree_node* cr = r->corr;

    // exchange l and r

   if (pl == pr)
     { pl->left = r;
       pl->right = l;
      }
   else
     { if (l == pl->left)
          pl->left = r;
       else
          pl->right = r;
    
       r->parent = pl;
    
       if (r == pr->left)
          pr->left = l;
       else
          pr->right = l;
    
       l->parent = pr;
      }

   // update corresponding inner nodes

   l->corr = cr; // l bleibt Blatt
   cr->k = l->k;

   r->corr = cl; // r bleibt Blatt
   cl->k = r->k;

   l = l->right; 
   r = r->left;
  }

  // reverse chaining of leaves v...w

  if (size() > 2)
  { l = v->left;
    r = w->right;
  
    r->left = v;
  
    pp_btree_node* p = v; 
  
    while(p != w)
    { pp_btree_node* q = p->right;
      p->left = q;
      p = q;
     }
  
    w->left = l;
  
    p = r;
  
    while ( p != l )
    { pp_btree_node* q = p->left;
      q->right = p;
      p = q;
     }
   }

  // adjust min pointer
  
  if (v == min_ptr()) min_ptr() = w;
 }

//------------------------------------------------------------------
// copy_tree(p) makes a copy of tree with root p and returns a pointer to the
// root of the copy. pre is last created leaf ( leaves are created from left 
// to right).
//------------------------------------------------------------------

pp_btree_node* 
pp_btree::copy_tree(pp_btree_node* p, pp_btree_node*& pre, pp_btree& base)const

{
  pp_btree_node* q = new pp_btree_node(base, p->bal);

  if ( p->is_node() )  // internal node: copy subtrees 
  { 
    q->corr = nil;
    q->left = copy_tree(p->left, pre, base);
    pre->corr = q;
    q->k = pre->k;
    q->right = copy_tree(p->right, pre, base);
    q->left->parent = q;
    q->right->parent = q;
  }
  else   //leaf: chaining with last created leaf "pre"
  {
    q->corr = q;
    q->k = p->k; q->i = p->i;
    if (pre) pre->right = q; 
    q->left = pre;
    pre = q;
   }

  return q;
}

//------------------------------------------------------------------
// clear
//------------------------------------------------------------------

void pp_btree::clear() 
{
   if ( root() )
   { del_tree(root());
     root() = min_ptr() = 0;
     pers_data->count = 0;
   }
}

//------------------------------------------------------------------
// del_tree(p) : deletes subtree rooted at node p
//------------------------------------------------------------------

void pp_btree::del_tree(pp_btree_node* p)
{
  if ( p->is_node() )
  { del_tree(p->left);
    del_tree(p->right);
   }

  p_delete(p);
}

//----------------------------------------------------------------------------
// set operations
//----------------------------------------------------------------------------

void pp_btree::add(const pp_btree& T)
{ pp_btree_node* p;
  forall_items(p,T) insert(p->k,p->i);
 }

void pp_btree::intersect_with(const pp_btree& T)
{ pp_btree_node* p = first_item();
  while(p)
  { pp_btree_node* q = next_item(p);
    if (T.lookup(p->k) == nil) del_item(p);
    p = q;
   }
}

void pp_btree::subtract(const pp_btree& T)
{ pp_btree_node* p;
  forall_items(p,T) del(p->k);
 }

bool pp_btree::contains(const pp_btree& T) const
{ pp_btree_node* p;
  forall_items(p,T) 
       if (lookup(p->k) == nil) return false;
  return true;
 }

//----------------------------------------------------------------------------
// printing and drawing trees
//----------------------------------------------------------------------------

void pp_btree::print() const
{ cout << "size = " << size() << endl;
  if ( root() ) print_tree(root(),1);
  cout << endl;
}

void pp_btree::print_tree(pp_btree_node* p,int h) const
{  
  if ( p->is_node() ) print_tree(p->right,h+1);

  for( int j=1; j <= h ; j++ ) cout << "     ";
  
  print_key(key(p));
  cout << " bal=" << p->get_bal();
  
  if ( p->is_leaf() )
    { cout << " [" << p << "] ";  
      cout << " succ[" << (GenPtr)p->right << "] ";
      cout << " pred[" << (GenPtr)p->left << "] ";
      cout << endl;
    }
  else cout << "\n";
  
  if ( p->is_node() ) print_tree(p->left,h+1);
}

void pp_btree::draw(PP_DRAW_BIN_NODE_FCT draw_node,
                    PP_DRAW_BIN_NODE_FCT draw_leaf,
                    PP_DRAW_BIN_EDGE_FCT draw_edge, 
                    double x1, double x2, double y, double ydist) const
{ 
  // draw a picture of the tree using the functions
  // draw_node(x,y,k,b)   draws node with key k and balance b at (x,y)
  // draw_leaf(x,y,k,b)   draws leaf with key k and balance b at (x,y)
  // draw_edge(x,y,x',y') draws an edge from (x,y) to (x',y')

  draw(draw_node,draw_leaf,draw_edge,root(),x1,x2,y,ydist,0); 
}

void pp_btree::draw(PP_DRAW_BIN_NODE_FCT draw_node,
                    PP_DRAW_BIN_NODE_FCT draw_leaf,
                    PP_DRAW_BIN_EDGE_FCT draw_edge, 
                    pp_btree_node* r, 
                    double x1, double x2, double y, 
                    double ydist, double last_x) const
{ 
  // draw subtree rooted at r

  double x = (x1+x2)/2;

  if (r==nil) return;

  if (last_x != 0) draw_edge(last_x,y+ydist,x,y);

  if (r->is_node()) 
     { draw_node(x,y,r->k,r->get_bal());
       draw(draw_node,draw_leaf,draw_edge, r->left,x1,x,y-ydist,ydist,x);
       draw(draw_node,draw_leaf,draw_edge, r->right,x,x2,y-ydist,ydist,x);
      }
  else
     draw_leaf(x,y,r->k,r->get_bal());
}

int pp_btree::depth(const pp_btree_node* r) const
{ 
  if (r == nil) return 0;
  if (r->is_leaf()) return 1;
  int l_depth = depth(r->child(left));
  int r_depth = depth(r->child(right));
  return 1 + (l_depth >= r_depth ? l_depth : r_depth);
}

LEDA_END_NAMESPACE
