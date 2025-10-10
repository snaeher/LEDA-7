/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dc_triang2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/geo/random_point.h>
#include <LEDA/core/sort.h>
#include <LEDA/system/assert.h>

using std::cout;
using std::endl;
using std::flush;
using std::ostream;
using std::istream;


/*
LEDA_BEGIN_NAMESPACE

typedef rat_point POINT;

inline bool operator<(const POINT& p, const POINT& q)
{ return POINT::cmp_xy(p,q) < 0; }

inline void swap(POINT& p, POINT& q) { swap(*(void**)&p,*(void**)&q); }

LEDA_END_NAMESPACE
*/


/*
LEDA_BEGIN_NAMESPACE

typedef point POINT;

inline bool operator<(const POINT& p, const POINT& q)
{ return POINT::cmp_xy(p,q) < 0; }

LEDA_END_NAMESPACE
*/



using namespace leda;


class POINT {

static unsigned long count;

/*
double x,y;
unsigned long id;
*/
float x,y;

// void* rat_p;

public:

bool id;

//friend unsigned long ID_Number(const POINT& p) { return p.id; }

double xcoord() const { return x; }
double ycoord() const { return y; }

POINT() : x(0), y(0), id(0) 
{ //id = count++;
  //rat_p = new rat_point(0,0,1); 
}

POINT(const point& p) : x(p.to_float().xcoord()), y(p.to_float().ycoord()) , id(0)
{ //id = count++;
  //rat_p = new rat_point(p); 
 }

void mark() { id  = true; }
void unmark() { id  = false; }
bool is_marked() { return id; }

~POINT() {}


friend bool operator==(const POINT& a, const POINT& b)
{  return a.x == b.x && a.y == b.y; }

friend bool operator<(const POINT& a, const POINT& b)
{  return a.x < b.x || (a.x == b.x && a.y < b.y); }

friend bool left_turn(const POINT& a, const POINT& b, const POINT& c)
{ double d1 = (a.xcoord() - b.xcoord()) * (a.ycoord() - c.ycoord());
  double d2 = (a.ycoord() - b.ycoord()) * (a.xcoord() - c.xcoord());
  return d1 > d2;
}

friend bool right_turn(const POINT& a, const POINT& b, const POINT& c)
{ double d1 = (a.xcoord() - b.xcoord()) * (a.ycoord() - c.ycoord());
  double d2 = (a.ycoord() - b.ycoord()) * (a.xcoord() - c.xcoord());
  return d1 < d2 ;
}

friend bool collinear(const POINT& a, const POINT& b, const POINT& c)
{ double d1 = (a.xcoord() - b.xcoord()) * (a.ycoord() - c.ycoord());
  double d2 = (a.ycoord() - b.ycoord()) * (a.xcoord() - c.xcoord());
  return d1 == d2 ;
}

friend int orientation(const POINT& a, const POINT& b, const POINT& c)
{ double d1 = (a.xcoord() - b.xcoord()) * (a.ycoord() - c.ycoord());
  double d2 = (a.ycoord() - b.ycoord()) * (a.xcoord() - c.xcoord());
  return compare(d1,d2);
}

friend ostream& operator<<(ostream& out, const POINT& p)
{ return out << p.x << " " << p.y; }

friend istream& operator>>(istream& in, POINT& p)
{ return in >> p.x >> p.y; }

};


unsigned long POINT::count = 0;



edge triang_points(POINT* l, POINT* r, GRAPH<POINT,int>& G)
{ 

  leda::quicksort(l,r);

#define left_bend(p,G,e) (left_turn(p,G[source(e)],G[target(e)]))

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





class Triangle {

POINT p[3];
Triangle* inc[3];

friend class triangulation;

friend ostream& operator<<(ostream& out, const Triangle& t)
{ return out << t.p[0] << " " << t.p[1] << " " << t.p[2]; }

friend istream& operator>>(istream& in, Triangle& t)
{ return in >> t.p[0] >> t.p[1] >> t.p[2]; }

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
  { current->p[0] = a;
    current->p[1] = b;
    current->p[2] = c;
    return current++;
   }

  const POINT& get_point(triangle v, int i) const { return v->p[i]; }
  triangle     get_target(triangle u, int i) const { return u->inc[i]; }

  void set_point(triangle v, int i, const POINT& q) { v->p[i] = q; }
  void set_target(triangle u, int i, triangle v) { u->inc[i] = v; }

  triangle hull_succ(triangle v)  const { return get_target(v,2); }
  triangle hull_pred(triangle v)  const { return get_target(v,1); }

};





triangle TRIANG_MERGE(triangulation& T, triangle vl, triangle vr, 
                                        triangle& v0, int& i0)
{
  // preconditions:
  // T.get_point(vl,0) rightmost POINT in left part
  // T.get_point(vr,1) leftmost POINT in right part

  triangle v_last = v0;
  int i_last = i0;

  v0 = 0;

  int count = 0;

  do {

    count = 0;

    POINT p = T.get_point(vr,1);
    triangle  v_succ = T.hull_succ(vl);

    while (right_turn(p,T.get_point(vl,0),T.get_point(v_succ,0)))
    { 
      count++;

      if (v0 == 0)
      { v0 = vl;
        i0 = 2;
       }

      if (v_last)
      { T.set_target(vl,1,v_last);
        T.set_target(v_last,i_last,vl);
       }

      T.set_point(vl,2,p);

      v_last = vl;
      i_last = 2;
      vl = v_succ;
      v_succ = T.hull_succ(vl);
     }

  
    POINT q = T.get_point(vl,0);
    triangle  v_pred = T.hull_pred(vr);

    while (left_turn(q,T.get_point(vr,1),T.get_point(v_pred,1)))
    { 
      count++;

      if (v0 == 0)
      { v0 = vl;
        i0 = 1;
       }

      if (v_last)
      { T.set_target(vr,2,v_last);
        T.set_target(v_last,i_last,vr);
       }

      T.set_point(vr,2,q);

      v_last = vr;
      vr = v_pred; 
      v_pred = T.hull_pred(vr);
      i_last = 1;
     }

   } while (count > 0);


  // construct tangent

  triangle u = T.new_triangle(T.get_point(vr,1), T.get_point(vl,0),
                                                 T.get_point(vl,0));
  if (v0 == 0)
  { v0 = u;
    i0 = 0;
   }

  T.set_target(u,1,vr);
  T.set_target(u,2,vl);

  if (v_last)
  { T.set_target(u,0,v_last);
    T.set_target(v_last,i_last,u);
   }

  T.set_target(vl,1,u);
  T.set_target(vr,2,u);

  return u;
}



void MERGE(triangle vl1, triangle vr1, triangle vl2, triangle vr2, 
           triangulation& T, triangle& vl, triangle& vr)
{
  triangle pred1 = T.hull_pred(vr1);
  triangle pred2 = T.hull_pred(vl2);

  triangle v = 0;
  int i = 1;

  triangle ut = TRIANG_MERGE(T,vr1,pred2,v,i);
  triangle lt = TRIANG_MERGE(T,vl2,pred1,v,i);

  if (T.get_point(ut,0) == T.get_point(vr2,0)) vr2 = ut; 
  if (T.get_point(lt,0) == T.get_point(vl1,0)) vl1 = lt; 

  vl = vl1;
  vr = vr2;
}



void TRIANG_2(POINT* first, triangulation& T, triangle& vl, triangle& vr)
{
  POINT p = *first;
  POINT q = *(first+1);

  if (q < p) leda::swap(p,q);
  triangle a = T.new_triangle(p,q,q);
  triangle b = T.new_triangle(q,p,p);
  for(int i=0; i<3; i++)
  { T.set_target(a,i,b);
    T.set_target(b,i,a);
   }
  vl = a;
  vr = b;
}


void TRIANG_3(POINT* first, triangulation& T, triangle& vl, triangle& vr)
{
  POINT p = *first;
  POINT q = *(first+1);
  POINT r = *(first+2);

  if (q < p) leda::swap(p,q);
  if (r < p) leda::swap(p,r);
  if (r < q) leda::swap(q,r);

  if (collinear(p,q,r) == 0) 
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

  if (left_turn(p,q,r)) 
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
  int diff = r-l;

  if (diff == 1) 
    TRIANG_2(l,T,vl,vr);
  else
    if (diff == 2) 
      TRIANG_3(l,T,vl,vr);
    else
      { POINT* m = l + diff/2;
        triangle vl1, vr1, vl2, vr2;
        TRIANG(l,m,T,vl1,vr1);
        TRIANG(m+1,r,T,vl2,vr2);
        MERGE(vl1,vr1,vl2,vr2,T,vl,vr);
      }
}




void TRIANG_P(POINT* l, POINT* r, triangulation& T, triangle& vl, triangle& vr)
{
  int diff = r-l;

  if (diff == 1) 
    TRIANG_2(l,T,vl,vr);
  else
    if (diff == 2) 
      TRIANG_3(l,T,vl,vr);
    else
      { POINT*  m = l + diff/2;
        leda::select1(m);
        triangle vl1, vr1, vl2, vr2;
        TRIANG_P(l,m,T,vl1,vr1);
        TRIANG_P(m+1,r,T,vl2,vr2);
        MERGE(vl1,vr1,vl2,vr2,T,vl,vr);
      }
}



int main()
{
  cout << sizeof(POINT) << endl;
  cout << endl;

  list<point>  L;
  int N = read_int("N = ");

  rand_int.set_seed(17*N);

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

  POINT* A = new POINT[n+2];
  POINT p;
  forall(p,L) A[i++] = p;


  GRAPH<POINT,int> T0;
  cout << "triang_points: " << flush;
  float t2 = used_time();
  triang_points(A,A+n-1,T0);
  cout << string(" %5.2f sec",used_time(t2)) << flush;
  cout << endl;


{
  int i=0;
  forall(p,L) A[i++] = p;

  triangulation T(2*n);
  triangle vl,vr;

  cout << "triang_dual:   " << flush;
  float t3 = used_time();
  leda::quicksort(A,A+n-1);
  //cout << string(" %5.2f sec",used_time(t3)) << flush;
  //cout << endl;
  TRIANG(A,A+n-1,T,vl,vr);
  cout << string(" %5.2f sec",used_time(t3)) << flush;
  cout << endl;
}


{
  int i=1;
  forall(p,L) A[i++] = p;

  A[0].id = true;
  A[n].id = true;

  triangulation T(2*n);
  triangle vl,vr;

  cout << "triang_dual:   " << flush;
  float t4 = used_time();
  TRIANG_P(&A[1],&A[n],T,vl,vr);
  cout << string(" %5.2f sec",used_time(t4)) << flush;
  cout << endl;

}


  delete[] A;

  return 0;
}


