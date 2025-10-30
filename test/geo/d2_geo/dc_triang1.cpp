/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dc_triang1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/geo/random_rat_point.h>
#include <LEDA/core/quicksort.h>
#include <LEDA/system/assert.h>



using namespace leda;

using std::cout;
using std::endl;
using std::ostream;
using std::istream;



typedef rat_point POINT;


edge triang_points(POINT* l, POINT* r, GRAPH<POINT,int>& G)
{ 

 QUICKSORT_C(l,r);

#define left_bend(p,G,e) (orientation(p,G[source(e)],G[target(e)]) > 0)

  G.clear();
  if (l > r) return nil;

  // initizialize G with a single edge starting at the first point

  POINT last_p = *l++;               // last visited point
  node  last_v = G.new_node(last_p); // last inserted node

  while (l <= r && last_p == *l) l++;

  if (l <= r)
  { last_p = *l++;
    node v = G.new_node(last_p);
    edge x = G.new_edge(last_v,v,0);
    edge y = G.new_edge(v,last_v,0);
    G.set_reversal(x,y);
    last_v = v;
   }

  
  while (l <= r)
  { 
    POINT p = *l++;

    if (p == last_p) continue; 

    edge e =  G.last_adj_edge(last_v);

    last_v = G.new_node(p);
    last_p = p;

    // walk up to upper tangent
    do e = G.face_cycle_pred(e); while (left_bend(p,G,e));
    // now e = e_up

    // walk down to lower tangent and triangulate
    do { edge succ_e = G.face_cycle_succ(e);
         edge x = G.new_edge(succ_e,last_v,leda::after,0);
         edge y = G.new_edge(last_v,source(succ_e),0);
         G.set_reversal(x,y);
         e = succ_e;
       } while (left_bend(p,G,e));
  }

  return G.last_edge();
}






bool less_xy(const POINT& p, const POINT& q)
{ return (POINT::cmp_xy(p,q) < 0); } 

bool less_yx(const POINT& p, const POINT& q)
{ return (POINT::cmp_yx(p,q) < 0); } 


template<class E, class LessThen>
E* PARTITION(E* l, E* r, LessThen lt)
{ 
  assert(r-l >= 3);

  E* k = l + (r-l)/2;
  E* k1 = k+1;

  if (lt(*r,*l)) leda_swap(*l,*r);

  if (lt(*k,*l)) 
    leda::swap(*l,*k);
  else 
    if (lt(*r,*k)) leda::swap(*k,*r);

  if (lt(*k1,*k))
  { leda::swap(*k,*k1);
    if (lt(*k,*l)) leda::swap(*l,*k);
   }


  E* i = l+1;
  E* j = r;
  leda::swap(*i,*k); 
  k = i;
  const E& s = *i;
  for(;;)
  { while (lt(*++i,s));
    while (lt(s,*--j));
    if (i<j) leda::swap(*i,*j);
    else break;
   }
  leda::swap(*k,*j); 

  return j;
}




class Triangle {

POINT p1;
POINT p2;
POINT p3;

Triangle* inc[3];

friend class triangulation;

friend ostream& operator<<(ostream& out, const Triangle& t)
{ return out << t.p1 << " " << t.p2 << " " << t.p3; }

friend istream& operator>>(istream& in, Triangle& t)
{ return in >> t.p1 >> t.p2 >> t.p3; }

};

typedef Triangle* triangle;






class triangulation
{
  triangle first;
  triangle current;

  public:

   triangulation(int sz) { current = first = new Triangle[sz];}

  ~triangulation() { delete[] first; }


  triangle  new_triangle(const POINT& a, const POINT& b , const POINT& c) 
  { current->p1 = a;
    current->p2 = b;
    current->p3 = c;
    return current++;
   }

  POINT& point1(triangle v) { return v->p1; }
  POINT& point2(triangle v) { return v->p2; }
  POINT& point3(triangle v) { return v->p3; }

  void set_target(triangle u, int i, triangle v) { u->inc[i] = v; }
  triangle get_target(triangle u, int i) const   { return u->inc[i]; }

  triangle hull_succ(triangle v)  const { return get_target(v,2); }
  triangle hull_pred(triangle v)  const { return get_target(v,1); }

};





triangle TRIANG_MERGE(triangulation& T, triangle vl, triangle vr, triangle v_last, int i_last)
{

  // preconditions:
  // T.point1(vl) rightmost POINT in left part
  // T.point3(vr) leftmost POINT in right part


  int count1 = 0;
  int count2 = 0;

  do {

    POINT p = T.point3(vr);
    triangle  v_succ = T.hull_succ(vl);

    count1 = 0;
    while (orientation(p,T.point1(vl),T.point1(v_succ)) < 0)
    { 
      if (count1++ == 0)
      { T.set_target(vl,1,v_last);
        T.set_target(v_last,i_last,vl);
       }

      v_last = vl;
      i_last = 2;

      T.point3(vl) = p;
      vl = v_succ;
      v_succ = T.hull_succ(vl);
     }

  
    POINT q = T.point1(vl);
    triangle  v_pred = T.hull_pred(vr);

    count2 = 0;
    while (orientation(q,T.point3(vr),T.point3(v_pred)) > 0)
    { 
      if (count2++ == 0)
      { T.set_target(vr,2,v_last);
        T.set_target(v_last,i_last,vr);
       }

      v_last = vr;
      i_last = 1;

      T.point3(vr) = q;
      vr = v_pred; 
      v_pred = T.hull_pred(vr);

     }

   } while (count1 > 0 || count2 > 0);


  // construct tangent

  triangle u = T.new_triangle(T.point3(vr),T.point1(vl),T.point1(vl));

  T.set_target(u,0,v_last);
  T.set_target(u,1,vr);
  T.set_target(u,2,vl);

  T.set_target(v_last,i_last,u);

  T.set_target(vl,1,u);
  T.set_target(vr,2,u);

  return u;
}



void MERGE(triangle vl1, triangle vr1, triangle vl2, triangle vr2, triangulation& T, triangle& vl, triangle& vr)
{

  triangle pred1 = T.hull_pred(vr1);
  triangle pred2 = T.hull_pred(vl2);

  Triangle v;

  int i = 1;

  triangle ut = TRIANG_MERGE(T,vr1,pred2,&v,i);

  triangle u = T.get_target(&v,1);

  if (T.get_target(u,0) == &v) i = 0;
  if (T.get_target(u,1) == &v) i = 1;
  if (T.get_target(u,2) == &v) i = 2;

  triangle lt = TRIANG_MERGE(T,vl2,pred1,u,i);

  if (T.point1(ut) == T.point1(vr2)) vr2 = ut; 
  if (T.point1(lt) == T.point1(vl1)) vl1 = lt; 

  vl = vl1;
  vr = vr2;
}


void TRIANG_2(POINT p, POINT q, triangulation& T, triangle& vl, triangle& vr)
{
  if (POINT::cmp_xy(q,p) < 0) leda::swap(p,q);
  triangle a = T.new_triangle(p,q,q);
  triangle b = T.new_triangle(q,p,p);
  for(int i=0; i<3; i++)
  { T.set_target(a,i,b);
    T.set_target(b,i,a);
   }
  vl = a;
  vr = b;
}


void TRIANG_3(POINT p, POINT q, POINT r, triangulation& T, triangle& vl, triangle& vr)
{
  if (POINT::cmp_xy(q,p) < 0) leda::swap(p,q);
  if (POINT::cmp_xy(r,p) < 0) leda::swap(p,r);
  if (POINT::cmp_xy(r,q) < 0) leda::swap(q,r);

  if (orientation(p,q,r) == 0) 
  { triangle  a = T.new_triangle(p,q,q);
    triangle  b = T.new_triangle(q,r,r);
    triangle  c = T.new_triangle(r,q,q);
    triangle  d = T.new_triangle(q,p,p);

    T.set_target(a,0,d);
    T.set_target(a,1,d);
    T.set_target(a,2,b);
  
    T.set_target(b,0,c);
    T.set_target(b,1,a);
    T.set_target(b,2,c);

    T.set_target(c,0,b);
    T.set_target(c,1,b);
    T.set_target(c,2,d);

    T.set_target(d,0,a);
    T.set_target(d,1,c);
    T.set_target(d,2,a);

    vl = a;
    vr = c;

    return;
  }


  bool swapped = false;

  if (orientation(p,q,r) > 0) 
  { leda::swap(q,r);
    swapped = true;
   }

  triangle  a = T.new_triangle(p,r,r);
  triangle  b = T.new_triangle(r,q,q);
  triangle  c = T.new_triangle(q,p,p);
  triangle  d = T.new_triangle(p,q,r);

  vl = a;
  vr =  (swapped) ? c : b;

  T.set_target(d,0,a);
  T.set_target(d,1,b);
  T.set_target(d,2,c);

  T.set_target(a,0,d);
  T.set_target(a,1,c);
  T.set_target(a,2,b);

  T.set_target(b,0,d);
  T.set_target(b,1,a);
  T.set_target(b,2,c);

  T.set_target(c,0,d);
  T.set_target(c,1,b);
  T.set_target(c,2,a);

}




void TRIANG(POINT* l, POINT* r, triangulation& T, triangle& vl, triangle& vr)
{

  int len = r-l+1;

  //cout << "len = " << len << endl;

  assert(len > 1);

  if (len == 2) 
  { TRIANG_2(*l,*r,T,vl,vr);
    return;
   }

  if (len == 3) 
  { TRIANG_3(*l,*(l+1),*(l+2),T,vl,vr);
    return;
   }

  //POINT* m = l + (len-1)/2;

  POINT*  m = PARTITION(l,r,less_xy);


  triangle vl1, vr1, vl2, vr2;


  TRIANG(l,m,T,vl1,vr1);

  TRIANG(m+1,r,T,vl2,vr2);

  MERGE(vl1,vr1,vl2,vr2,T,vl,vr);
}


int main()
{
  list<POINT>  L;
  int N = read_int("N = ");
  random_points_in_disc(N,10000,L);

  cout << endl;

  cout << "Sort & Unique: " << flush;
  float t1 = used_time();
  L.sort();
  L.unique();
  L.permute();
  cout << string(" %5.2f sec",used_time(t1)) << endl;
  cout << endl;

  int n = L.length();
  int i = 0;

  POINT* A = new POINT[n];
  POINT p;
  forall(p,L) A[i++] = p;


  GRAPH<POINT,int> T0;
  cout << "triang_points: " << flush;
  float t2 = used_time();
  triang_points(A,A+n-1,T0);
  cout << string(" %5.2f sec",used_time(t2)) << flush;
  cout << endl;


  i=0;
  forall(p,L) A[i++] = p;

  triangulation T(2*n);
  triangle vl,vr;

  cout << "triang_dual:   " << flush;
  float t3 = used_time();
   TRIANG(A,A+n-1,T,vl,vr);
  cout << string(" %5.2f sec",used_time(t3)) << flush;
  cout << endl;

  delete[] A;

  return 0;
}


