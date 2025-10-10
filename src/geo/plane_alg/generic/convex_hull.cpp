/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  convex_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <assert.h>

LEDA_BEGIN_NAMESPACE

namespace {
class point_smaller {
public:
bool operator()(const POINT& a, const POINT& b) const
 { return POINT::cmp_xy(a,b) < 0; }
};


class point_smaller_F {
public:
 bool operator()(const POINT& a, const POINT& b) const
 { double d = a.XD()*b.WD() - b.XD()*a.WD();
   if (d == 0) d = a.YD()*b.WD() - b.YD()*a.WD();
   return d < 0;
 }
};
}

list<POINT>  CONVEX_HULL_S0(const list<POINT>& L0)
{ list<POINT> CH;
  
  if (L0.empty() ) return CH;

  list<POINT> L = L0;
  L.sort(); 

  POINT last_p;
  CH.append(last_p = L.pop());

  while ( !L.empty() && last_p == L.head() ) L.pop();

  if ( L.empty() ) return CH;

  list_item last_vertex = CH.append(last_p = L.pop());
     
  
  POINT p; 
  forall(p,L) 
  { if ( p == last_p ) continue; // duplicate point
    last_p = p;
    if (CH.length() == 2 && collinear(CH.head(),CH.tail(),p))
     { CH[last_vertex] = p; continue; }
    
    // the interesting case 

    // compute down_item 

    list_item down_item = last_vertex; 
    list_item pred_item = CH.cyclic_pred(down_item);
    while (!right_turn(p,CH[down_item],CH[pred_item]))
    { down_item = pred_item;
      pred_item = CH.cyclic_pred(down_item); 
     } 

    // compute up_item 

    list_item up_item = last_vertex; 
    list_item succ_item = CH.cyclic_succ(up_item);
    while (!left_turn(p,CH[up_item],CH[succ_item]))
    { up_item = succ_item;
      succ_item = CH.cyclic_succ(up_item);
     }

    // update hull 

    for(list_item it = CH.cyclic_succ(down_item); it != up_item; 
                                            it = CH.cyclic_succ(down_item))
      CH.del_item(it);
          
    last_vertex = CH.insert(p,down_item,leda::after); 
  }
 
  return CH;
}


list<POINT>  CONVEX_HULL_S(const list<POINT>& L)
{ list<POINT> CH;
  int n = L.length();
  if (n == 0) return CH;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;

  A.sort();
  //quicksort(A.first_item(), A.last_item(), point_smaller(), point_smaller_F());

  POINT last_p = A[0];
  CH.append(last_p);
  i = 1;
  while (i < n && last_p == A[i]) i++;

  if (i==n) return CH;

  last_p = A[i++];
  list_item last_vertex = CH.append(last_p);
     
  while (i < n)
  { POINT p  = A[i++];
    if ( p == last_p ) continue; // duplicate point
    last_p = p;
    if (CH.length() == 2 && collinear(CH.head(),CH.tail(),p))
     { CH[last_vertex] = p; continue; }
    
    // compute down_item 

    list_item down_item = last_vertex; 
    list_item pred_item = CH.cyclic_pred(down_item);
    while (!right_turn(p,CH[down_item],CH[pred_item]))
    { down_item = pred_item;
      pred_item = CH.cyclic_pred(down_item); 
     } 

    // compute up_item 

    list_item up_item = last_vertex; 
    list_item succ_item = CH.cyclic_succ(up_item);
    while (!left_turn(p,CH[up_item],CH[succ_item]))
    { up_item = succ_item;
      succ_item = CH.cyclic_succ(up_item);
     }

    // update hull 

    for(list_item it = CH.cyclic_succ(down_item); it != up_item; 
                                            it = CH.cyclic_succ(down_item))
      CH.del_item(it);
          
    last_vertex = CH.insert(p,down_item,leda::after); 
  }
 
  return CH;
}


namespace {
class ch_edge {

public:

POINT   source;
POINT   target;
ch_edge* succ;
ch_edge* pred;
ch_edge* link;
bool     outside;

ch_edge(const POINT& a, const POINT& b, ch_edge*& p) : source(a), target(b) 
{ outside = true; 
  link = p;
  p = this;
}

~ch_edge() {}

 LEDA_MEMORY(ch_edge)

};
}

list<POINT>  CONVEX_HULL_IC(const list<POINT>& L)
{ 
  if (L.length() < 2) return L;

  list<POINT> CH;

  POINT a = L.head();
  POINT b = L.tail();
  POINT c, p;

  if ( a == b )
  { forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }

  int orient = 0; 
  forall(c,L)
    if ( (orient = orientation(a,b,c)) != 0 ) break;

  if ( orient == 0 )
  { // all points are collinear 
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if ( orient < 0 ) leda_swap(b,c);

  
  ch_edge* last_edge = NULL;

  ch_edge* T[3];

  T[0] = new ch_edge(a,b,last_edge);
  T[1] = new ch_edge(b,c,last_edge);
  T[2] = new ch_edge(c,a,last_edge);

  int i;
  for(i = 0; i < 2; i++)  T[i]->succ = T[i+1];
  T[2]->succ = T[0];

  for(i = 1; i < 3; i++)  T[i]->pred = T[i-1];
  T[0]->pred = T[2];

  forall(p,L)
    {   
      int i = 0;
      while (i < 3 && !right_turn(T[i]->source,T[i]->target,p) ) i++;
      if (i == 3) 
      { // p inside initial triangle
        continue; 
      }

      ch_edge* e = T[i];

      while (! e->outside)
      { ch_edge* r0 = e->pred;
        if ( right_turn(r0->source,r0->target,p) ) e = r0;
        else { ch_edge* r1 = e->succ;
              if ( right_turn(r1->source,r1->target,p) ) e = r1;
              else { e = nil; break; }
            }
      }

      if (e == nil) continue;  // p inside current hull

      
      // compute "upper" tangent (p,high->source)

      ch_edge* high = e->succ;
      while (orientation(high->source,high->target,p) <= 0) 
        high = high->succ;

      // compute "lower" tangent (p,low->target)

      ch_edge* low = e->pred;
      while (orientation(low->source,low->target,p) <= 0) 
        low = low->pred;

      e = low->succ;  // e = successor of low edge

      // add new tangents between low and high

      ch_edge* e_l = new ch_edge(low->target,p,last_edge);
      ch_edge* e_h = new ch_edge(p,high->source,last_edge);

      e_h->succ = high;
      e_l->pred = low;
      high->pred = e_l->succ = e_h;
      low->succ  = e_h->pred = e_l;

      // mark edges between low and high as "inside" 
      // and define refinements

      while (e != high)
      { ch_edge* q = e->succ;
        e->pred = e_l;
        e->succ = e_h;
        e->outside = false;
        e = q;
      }

 }

  ch_edge* l_edge = last_edge;

  CH.append(l_edge->source);
  for(ch_edge* e = l_edge->succ; e != l_edge; e = e->succ) 
     CH.append(e->source);

  // clean up 

  while (l_edge)
  { ch_edge* e = l_edge;
    l_edge = l_edge->link;
    delete e;
  }

  return CH;
}


namespace {
class ch_edge3 {

public:

POINT   source;
POINT   target;
ch_edge3* succ;
ch_edge3* pred;
ch_edge3* link;

ch_edge3(const POINT& a, ch_edge3*& p): source(a), target(a)
{ link = p;
  p = this;
}

ch_edge3(const POINT& a, const POINT& b, ch_edge3*& p) : source(a), target(b) 
{ link = p;
  p = this;
}

bool outside() const { return succ->pred == this; }
bool inside() const { return succ->pred != this; }

bool right_turn(const POINT& p) const 
{ return leda::right_turn(source,target,p); }

bool left_turn(const POINT& p) const 
{ return leda::left_turn(source,target,p); }

LEDA_MEMORY(ch_edge3)
};
}

list<POINT>  CONVEX_HULL_ICF(const list<POINT>& L)
{ 
  if (L.length() < 2) return L;

  list<POINT> CH;

  POINT a = L.head();
  POINT b = L.tail();
  POINT p;


/*
  forall(p,L)
  { if (POINT::cmp_xy(p,a) < 0) a = p;
    if (POINT::cmp_xy(p,b) > 0) b = p;
   }
*/

  if ( a == b )
  { forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }


  //double A = 0;
  POINT c = a;

  forall(p,L)
    if (orientation(a,b,p) != 0 ) { c = p; break; }
/*
  { double ar = area(a.to_float(),b.to_float(),p.to_float());
    if (ar > A) { A = ar; c = p; }
   }
*/

  if (c == a )
  { // all points are collinear 
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if (orientation(a,b,c) < 0 ) leda_swap(b,c);

  ch_edge3* last_edge = NULL;

  ch_edge3* T[3];

  T[0] = new ch_edge3(a,b,last_edge);
  T[1] = new ch_edge3(b,c,last_edge);
  T[2] = new ch_edge3(c,a,last_edge);

  int i;
  for(i = 0; i < 2; i++)  T[i]->succ = T[i+1];
  T[2]->succ = T[0];

  for(i = 1; i < 3; i++)  T[i]->pred = T[i-1];
  T[0]->pred = T[2];

  forall(p,L)
    {
      int i = 0;
      while (i < 3 && !(T[i]->right_turn(p)) ) i++;

      if (i == 3)  continue; // p inside initial triangle

/*
      int j = (i>0) ? i-1 : 2;
      if (T[j]->right_turn(p)) i = j;

      assert(T[i]->right_turn(p));
      int k = (i>0) ? i-1 : 2;
      assert(!(T[k]->right_turn(p)));
*/

      ch_edge3* e = T[i];


#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 1;

      while (e->inside())
      { ch_edge3* r = e->pred;
        if (r->right_turn(p))
          e = r;
        else
         { ch_edge3* s = e->succ;
           if (s->right_turn(p))
             e = s;
           else
             break;
          }
       }

POINT::float_computation_only = 0;

      if (!(e->right_turn(p))) e = T[i];

#endif


      while (e->inside())
      { ch_edge3* r = e->pred;
        if (r->right_turn(p))
          e = r;
        else
         { ch_edge3* s = e->succ;
           if (s->right_turn(p))
             e = s;
           else
             break;
          }
       }

      if (e->inside()) continue;  // p inside current hull

/*
      assert(e->right_turn(p)); 
      assert(!(e->pred->right_turn(p)));
*/

      // compute upper and lower tangents

      ch_edge3* e_l = new ch_edge3(p,last_edge);
      ch_edge3* e_h = new ch_edge3(p,last_edge);
      e_l->succ = e_h;
      e_h->pred = e_l;

      ch_edge3* high = e->succ;
      ch_edge3* low = e->pred;

      e->pred = e_l;
      e->succ = e_h;

      while (!high->left_turn(p)) 
      { ch_edge3* s = high->succ;
        high->pred = e_l;
        high->succ = e_h;
        high = s;
       }
      e_h->target = high->source;
      e_h->succ = high;
      high->pred = e_h;

      while (!low->left_turn(p))
      { ch_edge3* q = low->pred;
        low->pred = e_l;
        low->succ = e_h;
        low = q;
       }
      e_l->source = low->target;
      e_l->pred = low;
      low->succ  = e_l;

 }

  // construct hull (CH)
  ch_edge3* e = last_edge;
  do { CH.append(e->source);
       e = e->succ;
     } while (e != last_edge);


  // clean up
  while (last_edge)
  { ch_edge3* e = last_edge;
    last_edge = last_edge->link;
    delete e;
  }

  return CH;
}

         


list<POINT> CONVEX_HULL_IC(const array<POINT>& A)
{ 
  list<POINT> CH;
  int n = A.size();
  if (n == 0) return CH;

  POINT a = A[0];

  int i = 1;
  while (i<n && a == A[i]) i++;

  if (i == n)
  { // all points are equal
    CH.append(a);
    return CH; 
  } 


  POINT b = A[i++];

  assert(a != b);

  int orient = 0; 
  while (i < n)
  { orient = orientation(a,b,A[i]);
    if (orient != 0) break;
    i++;
   }

  if (orient == 0)
  { // all points are collinear 
    for(int i=0; i<n; i++)
    { if (compare(A[i],a) < 0 ) a = A[i];
      if (compare(A[i],b) > 0 ) b = A[i];
    }
    CH.append(a); 
    CH.append(b);
    return CH;
  }

  POINT c = A[i++];   // a, b, and c are not collinear

  if (orient < 0) leda_swap(b,c);

  
  ch_edge* last_edge = NULL;

  ch_edge* T[3];

  T[0] = new ch_edge(a,b,last_edge);
  T[1] = new ch_edge(b,c,last_edge);
  T[2] = new ch_edge(c,a,last_edge);

  for(int j = 0; j < 3; j++)  
  { T[j]->succ = T[(j+1)%3];
    T[j]->pred = T[(j+2)%3];
   }


  while (i < n)
  { 
    POINT p = A[i++];

    int j = 0;
    while (j < 3 && !right_turn(T[j]->source,T[j]->target,p)) j++;
    if (j == 3) 
    { // p inside initial triangle
      continue; 
    }

    ch_edge* e = T[j];

    while (! e->outside)
    { ch_edge* r0 = e->pred;
      if ( right_turn(r0->source,r0->target,p) ) e = r0;
      else { ch_edge* r1 = e->succ;
            if ( right_turn(r1->source,r1->target,p) ) e = r1;
            else { e = nil; break; }
          }
    }

    if (e == nil) continue;  // p inside current hull

    
    // compute "upper" tangent (p,high->source)

    ch_edge* high = e->succ;
    while (orientation(high->source,high->target,p) <= 0) high = high->succ;

    // compute "lower" tangent (p,low->target)

    ch_edge* low = e->pred;
    while (orientation(low->source,low->target,p) <= 0) low = low->pred;

    e = low->succ;  // e = successor of low edge

    // add new tangents between low and high

    ch_edge* e_l = new ch_edge(low->target,p,last_edge);
    ch_edge* e_h = new ch_edge(p,high->source,last_edge);

    e_h->succ = high;
    e_l->pred = low;
    high->pred = e_l->succ = e_h;
    low->succ  = e_h->pred = e_l;

    // mark edges between low and high as "inside" 
    // and define refinements

    while (e != high)
    { ch_edge* q = e->succ;
      e->pred = e_l;
      e->succ = e_h;
      e->outside = false;
      e = q;
    }

 }
  
  ch_edge* l_edge = last_edge;

  CH.append(l_edge->source);
  for(ch_edge* e = l_edge->succ; e != l_edge; e = e->succ) 
     CH.append(e->source);

  // clean up 

  while (l_edge)
  { ch_edge* e = l_edge;
    l_edge = l_edge->link;
    delete e;
  }

  return CH;
}


namespace {
class ch_edge1 {

POINT src;
POINT tgt;

public:

ch_edge1* succ;
ch_edge1* pred;


 bool outside() const { return pred->succ == this; }
 bool inside() const { return pred->succ != this; }

 const POINT& source() const { return src; }
 const POINT& target() const { return tgt; }

 int orientation(const POINT& p) const { return src.orientation(tgt,p); }
 
 ch_edge1(const POINT& a, const POINT& b) : src(a),tgt(b) {}

LEDA_MEMORY(ch_edge1)

};
}

list<POINT>  CONVEX_HULL_IC1(const list<POINT>& L)
{ 
  int n = L.length();

  if (n < 2) return L;

  list<POINT> CH;

  POINT a = L.head();
  POINT b = L.tail();
  POINT c, p;

  if ( a == b )
  { forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }

  int orient = 0; 
  forall(c,L)
    if ( (orient = orientation(a,b,c)) != 0 ) break;

  if ( orient == 0 )
  { // all points are collinear 
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if ( orient < 0 ) leda_swap(b,c);


  ch_edge1* T = (ch_edge1*) new char[2*n*sizeof(ch_edge1)];
  ch_edge1* Tp = T;

  new(Tp++) ch_edge1(a,b);
  new(Tp++) ch_edge1(b,c);
  new(Tp++) ch_edge1(c,a);

  int i;
  for(i = 0; i < 2; i++)  T[i].succ = T + (i+1);
  T[2].succ = T;

  for(i = 1; i < 3; i++)  T[i].pred = T + (i-1);
  T[0].pred = T+2;

  forall(p,L)
    {   
      int i = 0;
      while (i < 3 && T[i].orientation(p) >= 0) i++;
      if (i == 3) 
      { // p inside initial triangle
        continue; 
      }

      ch_edge1* e = &T[i];

      while (e->inside())
      { ch_edge1* r0 = e->pred;
        if ( r0->orientation(p) < 0) e = r0;
        else { ch_edge1* r1 = e->succ;
              if ( r1->orientation(p) < 0) e = r1;
              else { e = nil; break; }
            }
      }

      if (e == nil) continue;  // p inside current hull

      
      // compute "upper" tangent (p,high->source)

      ch_edge1* high = e->succ;
      while (high->orientation(p) <= 0) high = high->succ;

      // compute "lower" tangent (p,low->target)

      ch_edge1* low = e->pred;
      while (low->orientation(p) <= 0) low = low->pred;

      e = low->succ;  // e = successor of low edge

      // add new tangents between low and high

      ch_edge1* e_l = new(Tp++) ch_edge1(low->target(),p);
      ch_edge1* e_h = new(Tp++) ch_edge1(p,high->source());

      e_h->succ = high;
      e_l->pred = low;
      high->pred = e_l->succ = e_h;
      low->succ  = e_h->pred = e_l;

      // (mark edges between low and high as "inside") 
      // and define refinements

      while (e != high)
      { ch_edge1* q = e->succ;
        e->pred = e_l;
        e->succ = e_h;
        e = q;
      }

 }
  
  ch_edge1* l_edge = Tp-1;

  CH.append(l_edge->source());

  for(ch_edge1* e = l_edge->succ; e != l_edge; e = e->succ) 
     CH.append(e->source());

  // clean up 

  while (--Tp >= T) Tp->~ch_edge1();

  delete[] ((char*)T);

  return CH;
}



namespace {
class ch_edge2 {

POINT src;
POINT tgt;

#if KERNEL == RAT_KERNEL
double aycx;
double axcy;
double axcw;
double cxaw;
double aycw;
double cyaw;
#endif



public:

ch_edge2* succ;
ch_edge2* pred;


 bool outside() const { return pred->succ == this; }
 bool inside() const { return pred->succ != this; }

 POINT source() const { return src; }
 POINT target() const { return tgt; }

#if KERNEL == RAT_KERNEL
 int orientation(const POINT& p) const 
 { return -src.orientation(p,tgt,aycx,axcy,axcw,cxaw,aycw,cyaw); }
#else
 int orientation(const POINT& p) const 
 { return -src.orientation(p,tgt); }
#endif

 bool right_turn(const POINT& p) const { return orientation(p) < 0; }
 bool left_turn(const POINT& p) const  { return orientation(p) > 0; }

/*
 bool right_turn(const POINT& p) const { return leda::right_turn(src,tgt,p); }
 bool left_turn(const POINT& p) const { return leda::left_turn(src,tgt,p); }
*/

 ch_edge2() {}

 void set_points(const POINT& a, const POINT& c) {
   src = a;
   tgt = c;

#if KERNEL == RAT_KERNEL
   aycx = a.YD()*c.XD();
   axcy = a.XD()*c.YD();
   axcw = a.XD()*c.WD();
   cxaw = c.XD()*a.WD();
   aycw = a.YD()*c.WD();
   cyaw = c.YD()*a.WD();
#endif
 }


 ch_edge2(const POINT& a, const POINT& c) : src(a),tgt(c) 
{
#if KERNEL == RAT_KERNEL
   aycx = a.YD()*c.XD();
   axcy = a.XD()*c.YD();
   axcw = a.XD()*c.WD();
   cxaw = c.XD()*a.WD();
   aycw = a.YD()*c.WD();
   cyaw = c.YD()*a.WD();
#endif
 }

LEDA_MEMORY(ch_edge2)

};
}


list<POINT>  CONVEX_HULL_IC2(const list<POINT>& L)
{ 
  int n = L.length();

  if (n < 2) return L;

  list<POINT> CH;

  POINT a = L.head();
  POINT b = L.tail();
  POINT c, p;

  if ( a == b )
  { forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }

  int orient = 0; 
  forall(c,L)
    if ( (orient = orientation(a,b,c)) != 0 ) break;

  if ( orient == 0 )
  { // all points are collinear 
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if ( orient < 0 ) leda_swap(b,c);


  ch_edge2* T = (ch_edge2*) new char[2*n*sizeof(ch_edge2)];
  ch_edge2* Tp = T;

  new(Tp++) ch_edge2(a,b);
  new(Tp++) ch_edge2(b,c);
  new(Tp++) ch_edge2(c,a);

  int i;
  for(i = 0; i < 2; i++)  T[i].succ = T + (i+1);
  T[2].succ = T;

  for(i = 1; i < 3; i++)  T[i].pred = T + (i-1);
  T[0].pred = T+2;

  forall(p,L)
    {   
      int i = 0;
      while (i < 3 && !T[i].right_turn(p)) i++;
      if (i == 3) 
      { // p inside initial triangle
        continue; 
      }

/*
      int j = i-1;
      if (j < 0) j = 2;
      if (T[j].right_turn(p)) i = j;

      j = i-1;
      if (j < 0) j = 2;
      assert(T[j].left_turn(p));
*/


      ch_edge2* e = &T[i];

      while (e->inside())
      { ch_edge2* r0 = e->pred;
      //if (!r0->left_turn(p)) e = r0;
        if ( r0->right_turn(p)) e = r0;
        else { ch_edge2* r1 = e->succ;
              if ( r1->right_turn(p)) e = r1;
              else { e = nil; break; }
            }
      }

      if (e == nil) continue;  // p inside current hull

      
      // compute "upper" tangent (p,high->source)

      ch_edge2* high = e->succ;
      while (!high->left_turn(p)) high = high->succ;

      // compute "lower" tangent (p,low->target)

      ch_edge2* low = e->pred;
      //assert(low->left_turn(p));
      while (!low->left_turn(p)) low = low->pred;

      e = low->succ;  // e = successor of low edge

      // add new tangents between low and high

      ch_edge2* e_l = new(Tp++) ch_edge2(low->target(),p);
      ch_edge2* e_h = new(Tp++) ch_edge2(p,high->source());

      e_h->succ = high;
      e_l->pred = low;
      high->pred = e_l->succ = e_h;
      low->succ  = e_h->pred = e_l;

      // (mark edges between low and high as "inside") 
      // and define refinements

      while (e != high)
      { ch_edge2* q = e->succ;
        e->pred = e_l;
        e->succ = e_h;
        e = q;
      }

 }
  
  ch_edge2* l_edge = Tp-1;

  CH.append(l_edge->source());

  for(ch_edge2* e = l_edge->succ; e != l_edge; e = e->succ) 
     CH.append(e->source());

  // clean up 

  while (--Tp >= T) Tp->~ch_edge2();

  delete[] ((char*)T);

  return CH;
}







namespace {
class ch_edge4 {

POINT src;
POINT tgt;

public:

 ch_edge4* succ;
 ch_edge4* pred;

 bool outside() const { return pred->succ == this; }
 bool inside() const { return pred->succ != this; }

 const POINT& source() const { return src; }
 const POINT& target() const { return tgt; }

 bool right_turn(const POINT& p) const { return leda::right_turn(src,tgt,p); }
 bool left_turn(const POINT& p)  const { return leda::left_turn(src,tgt,p); }

 ch_edge4(const POINT& a, const POINT& c) : src(a),tgt(c) {}

 LEDA_MEMORY(ch_edge4)

};
}



list<POINT>  CONVEX_HULL_ICF2(const list<POINT>& L)
{ 
  int n = L.length();

  if (n < 2) return L;

  list<POINT> CH;

  POINT a = L.head();
  POINT b = L.tail();
  POINT c;

  if ( a == b )
  { POINT p;
    forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }

  int orient = 0; 
  forall(c,L)
    if ( (orient = orientation(a,b,c)) != 0 ) break;

  if ( orient == 0 )
  { // all points are collinear 
    POINT p;
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if ( orient < 0 ) leda_swap(b,c);


  ch_edge4* T = (ch_edge4*)new char[2*n*sizeof(ch_edge4)];;
  ch_edge4* Tp = T;

  new(Tp++) ch_edge4(a,b);
  new(Tp++) ch_edge4(b,c);
  new(Tp++) ch_edge4(c,a);

  int i;
  for(i = 0; i < 2; i++)  T[i].succ = T + (i+1);
  T[2].succ = T;

  for(i = 1; i < 3; i++)  T[i].pred = T + (i-1);
  T[0].pred = T+2;

int err_count1 = 0;
int err_count2 = 0;

  POINT p;
  forall(p,L)
    {   
      int i = 0;
      while (i < 3 && !T[i].right_turn(p)) i++;

      if (i == 3)  continue;  // p inside initial triangle

      ch_edge4* e = &T[i];

      while (e->pred->right_turn(p)) e = e->pred;


      bool err = false;

#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 1;

      ch_edge4* start_e = e;

      while (e->inside())
      { ch_edge4* r = e->pred;
        if (r->right_turn(p))
          e = r;
        else
         { ch_edge4* s = e->succ;
           if (s->right_turn(p))
             e = s;
           else
             break;
          }
       }

POINT::float_computation_only = 0;

      if (!e->right_turn(p)) { err = true; e = start_e; }

      if (err) err_count1++; 

#endif

      while (e->inside())
      { ch_edge4* r = e->pred;
        if (r->right_turn(p))
          e = r;
        else
         { ch_edge4* s = e->succ;
           if (s->right_turn(p))
             e = s;
           else
             break;
          }
        if (!err) { err_count2++; err = true; }
       }

      if (e->inside()) continue;  // p inside current hull


      // compute upper and lower tangents

      ch_edge4* high = e->succ;
      while (!high->left_turn(p)) high = high->succ;

      ch_edge4* low = e->pred;
      while (!low->left_turn(p)) low = low->pred;

      e = low->succ;  // e = successor of low edge


      // add new tangents between low and high

      ch_edge4* e_l = new(Tp++) ch_edge4(low->target(),p);
      ch_edge4* e_h = new(Tp++) ch_edge4(p,high->source());

      e_h->succ = high;
      e_l->pred = low;
      high->pred = e_l->succ = e_h;
      low->succ  = e_h->pred = e_l;

      // define refinements
      while (e != high)
      { ch_edge4* q = e->succ;
        e->pred = e_l;
        e->succ = e_h;
        e = q;
      }
 }

cout << endl;
cout << "err1 = " << err_count1 << "  err2 = " << err_count2 << endl;
cout << endl;

  // construct hull (CH)

  ch_edge4* e = Tp-1;
  do { CH.append(e->source());
       e = e->succ;
     } while (e != Tp-1);


  // clean up
  while (--Tp >= T) Tp->~ch_edge4();

  delete[] ((char*)T);

  return CH;
}










list<POINT>  CONVEX_HULL_RIC(const list<POINT>& L)  
{ list<POINT> L1 = L;
  L1.permute();
  return CONVEX_HULL_IC(L1);
}


list<POINT>  CONVEX_HULL_RIC1(const list<POINT>& L)  
{ int n = L.length();
  array<POINT> A(n); //L.build_random_array();
  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;
  A.permute();
  return CONVEX_HULL_IC(A);
}



list<POINT>  CONVEX_HULL(const list<POINT>& L)
{ return CONVEX_HULL_RIC(L); }

POLYGON CONVEX_HULL_POLY(const list<POINT>& L)
{ list<POINT> P = CONVEX_HULL(L);
  return POLYGON(P);
}


// Graham Scan (sn august 2002)

list<POINT> UPPER_CONVEX_HULL(const list<POINT>& LL)
{ list<POINT> L = LL;
  L.sort();
  L.unique();
  if (L.length() <= 2) return L;

  POINT p_min = L.front(); // leftmost point
  POINT p_max = L.back();  // rightmost point

  list<POINT> hull;
  hull.append(p_max); // use rightmost point as sentinel
  hull.append(p_min); // first hull point

  // goto first point p above (p_min,p_max)
  while (!L.empty() && !left_turn(p_min,p_max,L.front())) L.pop();
  if (L.empty()) return hull;

  POINT p = L.pop();  // second (potential) hull point 
  POINT q;
  forall(q,L)
  { while (!right_turn(hull.back(),p,q)) p = hull.pop_back();
    hull.append(p);
    p = q;
  }

  hull.append(p); // add last hull point
  hull.pop();     // remove sentinel
  return hull;
}


list<POINT> CONVEX_HULL_S_OLD(const list<POINT>& LL)
{ list<POINT> L = LL;
  if (L.length() <= 1) return L;

  L.sort();

  list<POINT> hull;

  POINT p = L.pop();

  POINT q;
  forall(q,L)
  { POINT r = p;
    while (!hull.empty() && !right_turn(hull.front(),r,q)) r = hull.pop();
    hull.push(r);
    while (!hull.empty() && !left_turn(hull.back(),p,q)) p = hull.pop_back();
    hull.append(p);
    p = q;
  }

  hull.push(p);

  return hull;
}


list<POINT> CONVEX_HULL_S1(const list<POINT>& L)
{ 
  int n = L.length();
  if (n <= 1) return L;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;
  A.sort();

  list<POINT> hull(A[0]);

  for(int i=2; i<n; i++)
  { POINT q = A[i];
    p = A[i-1];
    while (!hull.empty() && !right_turn(hull.front(),p,q)) p = hull.pop();
    hull.push(p);
    p = A[i-1];
    while (!hull.empty() && !left_turn(hull.back(),p,q)) p = hull.pop_back();
    hull.append(p);
    assert(hull.length() == 1 || p != hull.front());
  }

  if (A[n-1] != A[0]) hull.push(A[n-1]);

  return hull;
}


list<POINT> CONVEX_HULL_S2(const list<POINT>& L)
{ 
  int n = L.length();
  if (n <= 1) return L;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;
  A.sort();

  POINT pmin = A[0];
  POINT pmax = A[n-1];

  list<POINT> hull;

  POINT last_u = pmin;
  POINT last_d = pmin;

  for(int i=1; i<n-1; i++)
  { POINT q = A[i];
    if (left_turn(last_u,pmax,q))
    { // upper hull candidate
      while (!hull.empty() && !right_turn(hull.front(),last_u,q)) 
           last_u = hull.pop();
      hull.push(last_u);
      last_u = q;
      continue;
    }

    if (right_turn(last_d,pmax,q))
    { // lower hull candidate
      while (!hull.empty() && !left_turn(hull.back(),last_d,q)) 
           last_d= hull.pop_back();
      hull.append(last_d);
      last_d= q;
    }
  }

  if (last_u != pmin) hull.push(last_u);
  if (last_d != pmin) hull.append(last_d);

  hull.push(pmax);

  return hull;
}


list<POINT> CONVEX_HULL_S3(const list<POINT>& L)
{ 
  int n = L.length();
  if (n <= 1) return L;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;
  A.sort();

  POINT pmin = A[0];
  POINT pmax = A[n-1];

  list<POINT> hull; 

  list_item sentinel = hull.push(pmax);

  POINT first_u = pmin;
  POINT last_d = pmin;

  for(int i=1; i<n-1; i++)
  { POINT q = A[i];

    if (right_turn(pmax,first_u,q))
    { // upper hull candidate
      while (!right_turn(hull.front(),first_u,q)) first_u = hull.pop();
      hull.push(first_u);
      first_u = q;
      continue;
    }

    if (left_turn(pmax,last_d,q))
    { // lower hull candidate
      while (!left_turn(hull.back(),last_d,q)) last_d= hull.pop_back();
      hull.append(last_d);
      last_d= q;
    }
  }

  hull.push(first_u);
  hull.append(last_d);
  hull.del_item(hull.succ(sentinel));
  hull.move_to_front(sentinel);

  return hull;
}





list<POINT> CONVEX_HULL_S3F(const list<POINT>& L)
{ 
  int n = L.length();
  if (n <= 1) return L;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;

  quicksort(A.first_item(), A.last_item(), point_smaller(), point_smaller_F());

  POINT pmin = A[0];
  POINT pmax = A[n-1];

  list<POINT> hull; 

  list_item sentinel = hull.push(pmax);

  POINT first_u = pmin;
  POINT last_d = pmin;

  for(int i=1; i<n-1; i++)
  { POINT q = A[i];

    if (right_turn(pmax,first_u,q))
    { // upper hull candidate
      while (!right_turn(hull.front(),first_u,q)) first_u = hull.pop();
      hull.push(first_u);
      first_u = q;
      continue;
    }

    if (left_turn(pmax,last_d,q))
    { // lower hull candidate
      while (!left_turn(hull.back(),last_d,q)) last_d= hull.pop_back();
      hull.append(last_d);
      last_d= q;
    }
  }

  hull.push(first_u);
  hull.append(last_d);
  hull.del_item(hull.succ(sentinel));
  hull.move_to_front(sentinel);

  return hull;
}


list<POINT> CONVEX_HULL_S4F(const list<POINT>& L)
{ 
  int n = L.length();
  if (n <= 1) return L;

  array<POINT> A(n);

  int i = 0;
  POINT p;
  forall(p,L) A[i++] = p;

  quicksort(A.first_item(), A.last_item(), point_smaller(), point_smaller_F());


  POINT pmax = A[n-1];
  POINT pmin = A[0];

  list<POINT> hull; 

  list_item sentinel = hull.push(pmax);

  POINT first_u = pmin;
  POINT last_d = pmin;


  for(int i=1; i<n-1; i++)
  { POINT q = A[i];

    if (right_turn(pmax,first_u,q))
    { // upper hull candidate
#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 1;
#endif
      while (!right_turn(hull.front(),first_u,q)) first_u = hull.pop();
#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 0;
#endif
      hull.push(first_u);
      first_u = q;
      continue;
    }

    if (left_turn(pmax,last_d,q))
    { // lower hull candidate
#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 1;
#endif
      while (!left_turn(hull.back(),last_d,q)) last_d= hull.pop_back();
#if KERNEL == RAT_KERNEL
POINT::float_computation_only = 0;
#endif
      hull.append(last_d);
      last_d= q;
    }
  }

  hull.push(first_u);
  hull.append(last_d);
  hull.del_item(hull.succ(sentinel));
  hull.move_to_front(sentinel);

  return hull;
}






list<POINT> LOWER_CONVEX_HULL(const list<POINT>& L)
{ 
  list<POINT> H = CONVEX_HULL(L);
  H.sort();

  if (H.size() > 2)
  { POINT a = H.head();
    POINT b = H.tail();
    list_item it;
    forall_items(it,H)
      if (orientation(a,b,H[it]) > 0) H.del_item(it);
  }

  return H;
}


LEDA_END_NAMESPACE
