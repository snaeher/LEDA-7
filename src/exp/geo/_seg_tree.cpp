/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _seg_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// Rev 1.12->1.13: removed comments from Segment_Tree::??rot routines

// ------------------------------------------------------------------
//
// full dynamic Segment Trees
//
// Michael Wenzel     (1990)
//
// Implementation as described in
// Kurt Mehlhorn: Data Structures and Algorithms 3
//
// ------------------------------------------------------------------


#include <LEDA/geo/seg_tree.h>


LEDA_BEGIN_NAMESPACE

enum left_or_right {st_left = 0, st_right = 1};

#define forall_seg_nd_tr_items(a,b) \
for ((b).init_iterator(); (a=(b).move_iterator()) != 0; )

#undef TEST
#undef DUMP

#ifdef TEST
#define DPRINT(x) cout << x
#else
#define DPRINT(x)
#endif

#ifdef DUMP
#define DDUMP(x) cout << x
#else
#define DDUMP(x)
#endif

ostream& operator<<(ostream& s, h_segment& h) 
{ s << "(" << h._x0 << "," << h._x1 << ";" << h._y << ")";
  return s;
}

//-------------------------------------------------------------------
// member functions
//-------------------------------------------------------------------

/// seg_node_tree /////////////////////////////////////////////////////////////

int seg_node_tree::cmp(GenPtr hs_p1,GenPtr hs_p2) const
{ return father->seg_cmp((h_segment_p)hs_p1,(h_segment_p)hs_p2); }

// ------------------------------------------------------------
// locate
// returns item s.th. key(item)->_y >= y and
//                    key(item)->_y <= key(it)->_y for all it
//                                                 with key(it)->_y >= y
//                    pred(item) == nil or key(pred(item))->_y < y
//         nil, if no such item exists (i.e. y > key(it)->_y for all it)

seg_nd_tr_item seg_node_tree::locate_y(GenPtr y) const
{
  if (root == 0) return 0;

  seg_nd_tr_item current = root;
  while (!current->blatt()) {
    if ( father->cmp_dim2(y, key(current)->_y) <=0 )
	  current=current->sohn[left];
    else 
	  current=current->sohn[right];
  }

  // NOTE: if y == key(current)->_y it might be wrong to go to the left son
  // (The leaves in the left tree might contain only keys less than y whereas
  // some of the leaves in the right subtree might contain keys equal to y.
  // This is because for a node n in the right subtree key(current) < key(n) 
  // may hold even if key(current)->_y == key(n)->_y (due to the x-coords).
  // If we take the wrong way, we will end up with the maximum(=rightmost) 
  // leaf l1 in the left subtree while the correct leaf would be the 
  // minimum(=leftmost) leaf l2 in the right subtree, and we have 
  // l2 == succ(l1). So this case can be detected easily.)

  int c = father->cmp_dim2(y, key(current)->_y);
  if (c > 0) return nil; // y is bigger than all y-coords in the tree

  seg_nd_tr_item succ_current = succ(current);
  if (c == 0 || succ_current == nil) return current;

  // now we know: key(current)->_y < y, i.e. succ_current might be the correct item
  return ( father->cmp_dim2(y, key(succ_current)->_y) == 0) ? succ_current : current;
}

// ------------------------------------------------------------
// query
// returns a List of all segments hsp with y0 <= y(hsp) <= y1

list<h_segment_p> seg_node_tree::query(GenPtr y0, GenPtr y1) const
{ 
  DPRINT(" query in nodelist\n");

  list<seg_tree_item> L;

  seg_nd_tr_item it = locate_y(y0);
    // we have: key(pred(it))->_y < y0 <= key(it)->_y

  while ( it && father->cmp_dim2(key(it)->_y, y1) <=0 ) {
    L.append(key(it));
    it = succ(it);
  }

  return L;
}


//-------------------------------------------------------------------
// all_items
// returns all items in the nodelist

list<seg_nd_tr_item> seg_node_tree::all_items() const
{
  list<seg_nd_tr_item> L;

  seg_nd_tr_item z;
  forall_seg_nd_tr_items(z,*this)
    L.append(z);
  
  return L;
}



/// Segment_Tree //////////////////////////////////////////////////////////////

Segment_Tree::Segment_Tree() : r(this)  {} 
Segment_Tree::~Segment_Tree() { clear(root); }

int Segment_Tree::seg_cmp(h_segment_p p, h_segment_p q)
{ int a;
  if ((a = cmp_dim2(p->y(), q->y())) != 0)  return a;
  if ((a = cmp_dim1(p->x0(),q->x0())) != 0) return a;
  return cmp_dim1(p->x1(), q->x1());
}

// ------------------------------------------------------------
// empty
// returns true, iff for all seg_nd_tr_items i in nodelist(it):
//                 key(it) is not a start- or an end-coordinate of i

bool Segment_Tree::empty(bb1_item it)
{
  DPRINT(" empty of "<<(int)key(it)<<"\n");

  seg_node_list nodelist = info(it);

  seg_nd_tr_item i;
  forall_seg_nd_tr_items(i,*nodelist)
    if ( (cmp(key(it),x0(i))==0) || (cmp(key(it),x1(i))==0) )
	  { nodelist->lbreak(); return false; }

  DPRINT("nodelist empty \n");
  return true;
}   

// ------------------------------------------------------------
// lrot() , rrot() , ldrot() , rdrot()
// Rotationen am Knoten p

void Segment_Tree::lrot(bb1_item p, bb1_item q)
{
  bb1_item h = p->sohn[st_right];

  DDUMP("lrot "<< (int)key(p)) ; 
    if (q) { DDUMP(" Vater " << (int)key(q)); }
  DDUMP("\n");

  p->sohn[st_right] = h->sohn[st_left];
  h->sohn[st_left] = p;
  if (!q)
    root=h;
  else
  { if (cmp(key(h),key(q))>0)
      q->sohn[st_right]=h;
    else
      q->sohn[st_left]=h;
  }
  p->gr=p->sohn[st_left]->groesse()+p->sohn[st_right]->groesse();
  h->gr=p->groesse()+h->sohn[st_right]->groesse();

						// update nodelists
  bb1_item re=p->sohn[st_right];
  bb1_item s=p->sohn[st_left];

  seg_node_list help = info(h);
  info(h) = info(p);

  info(p) = new seg_node_tree(this);

  seg_nd_tr_item i;
  forall_seg_nd_tr_items(i,*(info(re)))
    if (   ( (!re->blatt()) || ((!start_coord(re,i)) && (!end_coord(re,i))) )
        && ( (!s->blatt())  || ((!start_coord(s,i))  && (!end_coord(s,i)))  ) 
        && (info(s)->member(r.key(i))) )
    {
      info(p)->insert(r.key(i));
    }

  forall_seg_nd_tr_items(i,*(info(p)))
  { info(re)->del(r.key(i));
    info(s)->del(r.key(i));
  }

  forall_seg_nd_tr_items(i,*help)
  { info(re)->insert(r.key(i));
    info(h->sohn[st_right])->insert(r.key(i));
  } 

  delete help;
}

void Segment_Tree::rrot(bb1_item p, bb1_item q)
{
  bb1_item h = p->sohn[st_left];

  DDUMP("rrot "<< (int)key(p)) ; 
    if (q) { DDUMP(" Vater " << (int)key(q)); }
  DDUMP("\n");

  p->sohn[st_left] = h->sohn[st_right];
  h->sohn[st_right] = p;
  if (!q) root=h;
  else
  { if (cmp(key(h),key(q))>0)
      q->sohn[st_right] = h;
    else
      q->sohn[st_left] = h;
  }
  p->gr=p->sohn[st_left]->groesse()+p->sohn[st_right]->groesse();
  h->gr=p->groesse()+h->sohn[st_left]->groesse();

						// update nodelists
  bb1_item re=p->sohn[st_right];
  bb1_item s=p->sohn[st_left];

  seg_node_list help = info(h);
  info(h) = info(p);

  info(p) = new seg_node_tree(this);
 
  seg_nd_tr_item i;
  forall_seg_nd_tr_items(i,*(info(s)))
    if (   ( (!s->blatt())  || ((!start_coord(s,i))&&(!end_coord(s,i)))   )
        && ( (!re->blatt()) || ((!start_coord(re,i))&&(!end_coord(re,i))) ) 
        && (info(re)->member(r.key(i))) )
    {
      info(p)->insert(r.key(i));
    }

  forall_seg_nd_tr_items(i,*(info(p)))
  { info(re)->del(r.key(i));
    info(s)->del(r.key(i));
  }

  forall_seg_nd_tr_items(i,*help)
  { info(s)->insert(r.key(i));
    info(h->sohn[st_left])->insert(r.key(i));
  } 

  delete help;
}

void Segment_Tree::ldrot(bb1_item p, bb1_item q)
{
  bb1_item h = p->sohn[st_right];

  DDUMP("ldrot "<< (int)key(p)) ; 
    if (q) { DDUMP(" Vater " << (int)key(q)); }
  DDUMP("\n");

  rrot(h,p);
  lrot(p,q);
}

void Segment_Tree::rdrot(bb1_item p, bb1_item q)

{
  bb1_item h = p->sohn[st_left];

  DDUMP("rdrot "<< (int)key(p)) ; 
    if (q) { DDUMP(" Vater " << (int)key(q)); }
  DDUMP("\n");

  lrot(h,p);
  rrot(p,q);
}
 

//-------------------------------------------------------------------
// insert
// insert a segment into a Segment_tree:             
// - insert x-coordinates in main tree (bb-alpha) 
//          and rotate (if necessary)
// - insert segment on nodes 
//          immediately below the paths to x-coordinates
// - insert segment into the tree of all segments
//
// precond: x0 <= x1

seg_tree_item Segment_Tree::insert(GenPtr x0, GenPtr x1, GenPtr y, GenPtr inf)

{
  DPRINT(" insert segment " << (int)x0 << " " << (int)x1 << " " << (int)y << " in main tree \n" );

  seg_nd_tr_item inserted;

  if (!(cmp(x0,x1)))                         // empty segment
    return 0;

  // info is replaced in any case
  copy_info(inf);

  h_segment_p new_seg = new h_segment(x0,x1,y,inf);

  if ((inserted=r.lookup(new_seg)) != 0) 
  { delete new_seg;
    clear_info(r.info(inserted));
    r.info(inserted)=inf;
    return to_seg_tree_item(inserted);
  }

  // we copy the dimensions only if a new segment is inserted
  copy_dim1(x0); new_seg->x0() = x0;
  copy_dim1(x1); new_seg->x1() = x1;
  copy_dim2(y);  new_seg->y()  = y;

  bb1_item t,father,p;
  bb1_item start,end;
  seg_node_list h;

// insert start_coordinate

  if ((start=bb1_tree::lookup(x0)) == 0)        // new coordinate
  { h = new seg_node_tree(this);
    start = sinsert(x0,h);
    t = start;
  
    if (!st.empty())
    { father = st.pop();         // pop father of leaf and set nodelist
      h=new seg_node_tree(this);
      info(father) = h;
      p = succ(t); 
      if (p)
      { list<seg_nd_tr_item> L = info(p)->all_items();
        seg_nd_tr_item j;
        forall(j,L)
        { DDUMP("Item "<<*(r.key(j))<<" in Knoten "<<(int)key(p)<<"\n");
          if (end_coord(p,j))
            info(t)->insert(r.key(j));
          else if (!start_coord(p,j))
          { info(father)->insert(r.key(j));
            info(p)->del(r.key(j));
		  }
        }
      }
    }
    
    // rebalance
    while (!st.empty())
    { t=st.pop();
      father = st.empty() ? 0 : st.top();
      t->gr++;  
      float i = t->bal();

      DDUMP("rebal cur="<<(int)key(t)<<" groesse= "<<t->groesse()<<" bal= "<<i<< "in main tree\n");

      if (i < alpha)
        if (t->sohn[st_right]->bal()<=d) lrot(t,father);
        else ldrot(t,father);
      else if (i>1-alpha) 
      { if (t->sohn[st_left]->bal() > d ) rrot(t,father);
        else rdrot(t,father);
       }
    }
  }              // start coordinate inserted



// insert end_coordinate

  if ((end=bb1_tree::lookup(x1)) == 0)   // new coordinate
  { h = new seg_node_tree(this);
    end = sinsert(x1,h);
    t=end;

    if (!st.empty())
    { father = st.pop();         // pop father of leaf and set nodelist
      h=new seg_node_tree(this);
      info(father) = h;
      p = succ(t); 
      if (p)
      { list<seg_nd_tr_item> L = info(p)->all_items();
        seg_nd_tr_item j;
        forall(j,L)
        if (end_coord(p,j))
          info(t)->insert(r.key(j));
        else if (!start_coord(p,j))
        { info(father)->insert(r.key(j));
          info(p)->del(r.key(j));
        }
      }
    }

    // rebalance
    while (!st.empty())
    { t=st.pop();
      father = st.empty() ? 0 : st.top();
      t->gr++;  
      float i = t->bal();

      DDUMP("rebal cur="<<(int)key(t)<<" groesse= "<<t->groesse()<<" bal= "<<i<< "in main tree\n");

      if (i < alpha)
        if (t->sohn[st_right]->bal()<=d) lrot(t,father);
        else ldrot(t,father);
      else if (i>1-alpha) 
      { if (t->sohn[st_left]->bal() > d ) rrot(t,father);
        else rdrot(t,father);
       }
    }
  } // end coordinate inserted


// insert segment into nodelists of leaves of coordinates

  info(start)->insert(new_seg);  
  info(end)->insert(new_seg);

  p=t=root;
  GenPtr x2,x3;

  // same path
  while (p==t)  // start and end coordinate assigned to different leaves
  { x2=key(p);
    DDUMP(" same path " << (int)x0 << " " << (int)x1 << " " << (int)x2 << "\n");
    if ( cmp(x0,x2)<=0 ) p=p->sohn[st_left];
      else p=p->sohn[st_right];
    if ( cmp(x1,x2)<=0 ) t=t->sohn[st_left];
      else t=t->sohn[st_right];
  }

  // now paths to lower and upper part
  while (!p->blatt())                // follow lower path
  { 
    x2=key(p);
    DDUMP(" lower path " << (int)x0 << " " << (int)x2 << "\n");
    if ( cmp(x0,x2)>0 ) 
      p=p->sohn[st_right];
    else
    { info(p->sohn[st_right])->insert(new_seg);   // insertion into nodelist
      p=p->sohn[st_left];
    } 
  }

  while (!t->blatt())               // follow upper path
  { 
    x3=key(t);
    DDUMP(" upper path " << (int)x1 << " " << (int)x3 << "\n");
    if ( cmp(x1,x3)<=0 ) 
      t=t->sohn[st_left];
    else
    { info(t->sohn[st_left])->insert(new_seg);  // insertion into nodelist
      t=t->sohn[st_right];
    } 
  }

#ifdef DUMP
  print_tree();
#endif

  return to_seg_tree_item(r.insert(new_seg)); 
}


//-------------------------------------------------------------------
// delete
// delete a segment in a Segment_tree:
// - delete segment out of the tree of all segments
// - delete segment on nodes 
//          immediately below the paths to x-coordinates
// - delete x-coordinates in main tree (bb-alpha) 
//          and rotate (if necessary)
//
// precond: x0 <= x1

void Segment_Tree::del(GenPtr x0, GenPtr x1, GenPtr y)

{
  DPRINT(" delete segment " << (int)x0 << " " << (int)x1 << " " << (int)y << " in main tree\n");

  if (!(cmp(x0,x1)))                         // empty segment
    return ;

  bb1_item p,q,s,t,father;
  seg_nd_tr_item z;
  seg_node_list help;
  h_segment_p deleted;
  h_segment_p del_seg = new h_segment(x0,x1,y);

  if ((t=r.lookup(del_seg)) == 0)          // segment not in tree
  { delete del_seg;
    DDUMP("delete: segment not in tree\n");
    return;
  }

  deleted = r.key(t);
  r.del(del_seg);                      // delete in tree of all segments

  s=t=root;
  GenPtr x2,x3;

// delete segment in nodelists

  while ((s==t)&&(!s->blatt()))       // same path
  { x2=key(s);
    x3=key(t);
    DDUMP(" same path " << (int)x2 << " groesse " << s->groesse() << "\n");
    if ( cmp(x0,x2)<=0 ) s=s->sohn[st_left];
      else s=s->sohn[st_right];
    if ( cmp(x1,x3)<=0 ) t=t->sohn[st_left];
      else t=t->sohn[st_right];
  }
				     // now paths to lower and upper part
  while (!s->blatt())                // follow lower path
  { 
    x2=key(s);
    DDUMP(" lower path " << (int)x2 << " groesse " << s->groesse() << "\n");
    if ( cmp(x0,x2)>0 ) 
      s=s->sohn[st_right];
    else
    { info(s->sohn[st_right])->del(del_seg);   // delete out of nodelist
      s=s->sohn[st_left];
    } 
  }
  info(s)->del(del_seg);

  while (!t->blatt())               // follow upper path
  { 
    x3=key(t);
    DDUMP(" upper path " << (int)x3 << " groesse " << t->groesse() << "\n");
    if ( cmp(x1,x3)<=0 ) 
      t=t->sohn[st_left];
    else
    { info(t->sohn[st_left])->del(del_seg);   // delete out of nodelist
      t=t->sohn[st_right];
    } 
  }
  info(t)->del(del_seg);
				    // delete in main tree if necessary

  if(empty(s))                      // delete item of start coordinate
  {
    sdel(x0);
    if (!st.empty())                // father exists 
    { q = st.pop();                 // pop father of leaf and set nodelist

      if (!st.empty())
      { p = st.top();
        if (cmp(key(q),key(p))<=0)  // left son deleted
          p = p->sohn[st_left];
        else                        // right son deleted
          p = p->sohn[st_right];
      } 
      else                          // root deleted 
        p = root;

                                          // set nodelist
      help = info(p);
      info(p) = info(q);

      if (p->blatt())
        forall_seg_nd_tr_items(z,*help)
          if ((start_coord(p,z))||(end_coord(p,z)))
            info(p)->insert(r.key(z));

      delete help;
      delete q;
    }

    delete info(s);
    delete s;

    // rebalance
    while (!st.empty())
    { p=st.pop();
      father = st.empty() ? 0 : st.top();
      p->gr--;  
      float i = p->bal();

      DDUMP("rebal cur="<<(int)key(p)<<" groesse= "<<p->groesse()<<" bal= "<<i<< "in main tree \n");

      if (i < alpha)
        if (p->sohn[st_right]->bal()<=d) lrot(p,father);
        else ldrot(p,father);
      else if (i>1-alpha) 
      { if (p->sohn[st_left]->bal() > d ) rrot(p,father);
  	    else rdrot(p,father);
       }
    }
  }

  if(empty(t))                      // delete item of end coordinate
  {
    sdel(x1);
    if (!st.empty())                // father exists 
    { q = st.pop();                 // pop father of leaf and set nodelist

      if (!st.empty())
      { p = st.top();
        if (cmp(key(q),key(p))<=0)  // left son deleted
          p = p->sohn[st_left];
        else                        // right son deleted
          p = p->sohn[st_right];
      } 
      else                          // root deleted 
        p = root;
                                          // set nodelist
      help = info(p);
      info(p) = info(q);

      if (p->blatt())
        forall_seg_nd_tr_items(z,*help)
          if ((start_coord(p,z))||(end_coord(p,z)))
            info(p)->insert(r.key(z));

      delete help;
      delete q;
    }

    delete info(t);
    delete t;

    // rebalance
    while (!st.empty())
    { p=st.pop();
      father = st.empty() ? 0 : st.top();
      p->gr--;  
      float i = p->bal();

      DDUMP("rebal cur="<<(int)key(p)<<" groesse= "<<p->groesse()<<" bal= "<<i<< "in main tree \n");

      if (i < alpha)
        if (p->sohn[st_right]->bal()<=d) lrot(p,father);
        else ldrot(p,father);
      else if (i>1-alpha) 
      { if (p->sohn[st_left]->bal() > d ) rrot(p,father);
  	  else rdrot(p,father);
       }
    }
  }

#ifdef DUMP
  print_tree();
#endif


  // We do a lazy deletion of coord1 entries because they may still be in use
  // in the bb1_tree *this (as keys of nodes).
  // The actual deletion is deferred until Segment_Tree::clear() is called.
  deleted_coord1.append(this->x0(deleted)); // clear_dim1(this->x0(deleted));
  deleted_coord1.append(this->x1(deleted)); // clear_dim1(this->x1(deleted));
  clear_dim2(this->y(deleted));
  clear_info(inf(deleted));

  delete del_seg;
  delete deleted;

  if (empty()) clear_tree(); // in order to force deferred deletions ...
}

//-------------------------------------------------------------------
// query
// returns all items it in the Segment_tree with
//     x0(it) <= x <= x1(it) and y0 <= y(it) <= y1
//    
// by:
// - look for x in the main tree   
//   
// - for all nodes on the path perform a query(y0,y1) on nodelists
//
// precond: y0 <= y1

list<seg_tree_item> Segment_Tree::query(GenPtr x, GenPtr y0, GenPtr y1) const

{ 
  DPRINT("query in main tree " << (int)x << " " << (int)y0 << " " << (int)y1 << "\n");

  bb1_item it;
  seg_tree_item z;
  list<seg_tree_item> L,l;

  search(x);
  if (st.empty()) return L;

  if (cmp(x,key(st.top()))==0)             // x-coordinate in tree
    while (!st.empty())
    { it = st.pop();
      l = info(it)->query(y0,y1);
      L.conc(l);
    }

  else                                     // x-coordinate not in tree
  {
     if ((cmp(x,key(bb1_tree::min()))<0) || (cmp(x,key(bb1_tree::max()))>0))
       return L;
   
     while (!st.empty())
     { it = st.pop();
       l = info(it)->query(y0,y1);
       forall(z,l)
         if ((cmp(x,x0(z))>=0) && (cmp(x,x1(z))<=0))
           L.append(z);
       l.clear();
     }
  }

  return L;
}

//-------------------------------------------------------------------
// x_infinity_query
// returns a List of all items it with y0 <= y(it) <= y1
//
// precond: y0 <= y1

list<seg_tree_item> Segment_Tree::x_infinity_query(GenPtr y0, GenPtr y1) const
{
  return r.query(y0,y1);
}

//-------------------------------------------------------------------
// y_infinity_query
// returns a List of all items it with x0(it) <= x <= x1(it)

list<seg_tree_item> Segment_Tree::y_infinity_query(GenPtr x) const
{
  bb1_item it;
  seg_nd_tr_item z;
  list<seg_tree_item> L;

  search(x);
  if (st.empty()) return L;

  if (cmp(x,key(st.top()))==0)             // x-coordinate in tree
    while (!st.empty())
    { it = st.pop();
      forall_seg_nd_tr_items(z,*(info(it)))
      L.append(to_seg_tree_item(z));
    }

  else                                     // x-coordinate not in tree
  {
     if ((cmp(x,key(bb1_tree::min()))<0) || (cmp(x,key(bb1_tree::max()))>0))
       return L;
   
     list<seg_tree_item> L1;
     while (!st.empty())
     { it = st.pop();
       forall_seg_nd_tr_items(z,*(info(it)))
       if ((cmp(x,x0(z))>=0) && (cmp(x,x1(z))<=0))
         L.append(to_seg_tree_item(z));
     }
  }

  return L;
}


//-------------------------------------------------------------------
// query_sorted
// returns all items it in the Segment_tree with
//     x0(it) <= x <= x1(it) and y0 <= y(it) <= y1
//    
// items in the returned list are sorted by y(it) (according to cmp_dim2)
//
// precond: y0 <= y1

class Adaptor_cmp_dim2_to_leda_cmp : public leda_cmp_base<seg_tree_item> {
public:
  Adaptor_cmp_dim2_to_leda_cmp(const Segment_Tree& t) : tree(t) {}

  virtual int operator()(const seg_tree_item& it1, const seg_tree_item& it2) const
  { return tree.cmp_dim2(tree.y(it1), tree.y(it2)); }

private:
  const Segment_Tree& tree;
};

list<seg_tree_item> Segment_Tree::query_sorted(GenPtr x, GenPtr y0, GenPtr y1) const
{
  list<seg_tree_item> L = query(x, y0, y1);
  Adaptor_cmp_dim2_to_leda_cmp cmp(*this);
  L.sort(cmp);
  return L;
}

//-------------------------------------------------------------------
// y_infinity_query_sorted
// returns a List of all items it with x0(it) <= x <= x1(it)
// items in the returned list are sorted by y(it) (according to cmp_dim2)

list<seg_tree_item> Segment_Tree::y_infinity_query_sorted(GenPtr x) const
{
  list<seg_tree_item> L = y_infinity_query(x);
  Adaptor_cmp_dim2_to_leda_cmp cmp(*this);
  L.sort(cmp);
  return L;
}

//-------------------------------------------------------------------
// all_items
// returns all items in the tree

list<seg_tree_item> Segment_Tree::all_items() const
{
  list<seg_tree_item> L;

  seg_nd_tr_item z;
  forall_seg_nd_tr_items(z,r)
    L.append(to_seg_tree_item(z));
  
  return L;
}


//-------------------------------------------------------------------
// lookup
// returns a seg_tree_item it with key(it) = (x0,x1,y) if there is one
//
// precond: x0 <= x1

seg_tree_item Segment_Tree::lookup(GenPtr x0, GenPtr x1, GenPtr y) const
{ 
  DPRINT(" lookup in main tree " << (int)x0 << " " << (int)x1 << " " << (int)y << "\n");

  h_segment_p z = new h_segment(x0,x1,y);
  seg_tree_item it = to_seg_tree_item_safe(r.lookup(z));
  delete z;

  return it;
}


//-------------------------------------------------------------------
// clear_tree
// delete all Items and Tree of Items

void Segment_Tree::clear_tree()
{
  if (root) {
    clear(root);

    seg_nd_tr_item z;
    forall_seg_nd_tr_items(z,r)
    { 
      h_segment_p q=r.key(z);
      clear_dim1(x0(q));
      clear_dim1(x1(q));  
      clear_dim2(y(q));
      clear_info(inf(q));
      delete q;
    }
    r.clear();
  }

  // Now we perform the deletion of coord1 entries (cf. Segment_Tree::del(...)).
  GenPtr x;
  forall(x, deleted_coord1) clear_dim1(x);
  deleted_coord1.clear();

  first = iterator = 0;
  anzahl = 0;
}

//-------------------------------------------------------------------
// clear
// delete nodelists and nodes


void Segment_Tree::clear(bb1_item& it)
{
  if (it == 0) return;
  
  if(!it->blatt())
  { clear(it->sohn[st_left]);
    clear(it->sohn[st_right]);
  }

  delete info(it);
  delete it;
  it = 0;

}

//-------------------------------------------------------------------
// print     
// prints the tree with nodelists

void Segment_Tree::print(bb1_item it,string s)
{
  if (!it) return;

  if (!it->blatt())
    print(it->sohn[st_left],s + string("     "));

  cout<< s << key(it) << "\n";
  cout<< s ; 

  seg_nd_tr_item i;
  forall_seg_nd_tr_items(i,*info(it))
    cout << "[" << r.key(i) << "]:" << *(r.key(i)) << " ";
  cout << endl;

  if (!it->blatt())
    print(it->sohn[st_right],s + string("     "));
}

LEDA_END_NAMESPACE
