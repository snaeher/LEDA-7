/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dc_triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


/*
#include <LEDA/graphics/window.h>
*/

#include <LEDA/graph/graph.h>
#include <LEDA/core/tuple.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/geo/random_rat_point.h>

#include <LEDA/system/assert.h>

using namespace leda;

using std::cout;
using std::endl;
using std::ostream;
using std::istream;



typedef rat_point POINT;



inline int left_bend(const POINT& p, 
                     const GRAPH<POINT,int>& G, 
                     const edge& e)   
{ return (orientation(p,G[source(e)],G[target(e)]) > 0); }


edge triang_points(const POINT* l, POINT* r,
                        GRAPH<POINT,int>& G)
{ 
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

  assert(j > l); 
  assert(j < r-1);

  return j;
}




class Triangle {

public:

POINT p1;
POINT p2;
POINT p3;

edge inc[3];

Triangle() { inc[0] = inc[1] = inc[2] = (edge)15; }

Triangle(const POINT& a, const POINT& b, const POINT c) : p1(a), p2(b), p3(c) 
{}
};


//LEDA_BEGIN_NAMESPACE

ostream& operator<<(ostream& out, const Triangle& t)
{ return out << t.p1 << " " << t.p2 << " " << t.p3; }

istream& operator>>(istream& in, Triangle& t)
{ return in >> t.p1 >> t.p2 >> t.p3; }

//LEDA_END_NAMESPACE





class triangulation : public GRAPH<Triangle,int> 
{
  public:

  ~triangulation() {}

  node  new_triangle(const POINT& a, const POINT& b , const POINT& c,
                     node u0, node u1, node u2) 
  { Triangle t(a,b,c); 
    node v = new_node(t); 
    operator[](v).inc[0] = new_edge(v,u0); // edge 0
    operator[](v).inc[1] = new_edge(v,u1); // edge 1
    operator[](v).inc[2] = new_edge(v,u2); // edge 2
    return v;
   }

  node  new_triangle(const POINT& a, const POINT& b , const POINT& c) 
  { Triangle t(a,b,c); 
    node v = new_node(t); 
    operator[](v).inc[0] = new_edge(v,v); // edge 0
    operator[](v).inc[1] = new_edge(v,v); // edge 1
    operator[](v).inc[2] = new_edge(v,v); // edge 2
    return v;
   }

  edge get_edge(node v, int i) const  { return inf(v).inc[i]; }
/*
  { if (i == 0) return first_adj_edge(v);
    if (i == 1) return adj_succ(first_adj_edge(v));
    if (i == 2) return last_adj_edge(v);
    return 0;
   }
*/

  node hull_succ(node v)  const { return target(get_edge(v,2)); }
  node hull_pred(node v)  const { return target(get_edge(v,1)); }

  void set_target(edge e, node v) { move_edge(e,source(e),v); }

  void set_target(node u, int i, node v) { set_target(get_edge(u,i),v); }
  node get_target(node u, int i) const   { return target(get_edge(u,i)); }


/*
  POINT node_position(node v)
  { POINT p = inf(v).p1;
    POINT q = inf(v).p2;
    POINT r = inf(v).p3;
    POINT pos;
    if (orientation(p,q,r) != 0)
      { double x = (p.xcoord() + q.xcoord() + r.xcoord())/3;
        double y = (p.ycoord() + q.ycoord() + r.ycoord())/3;
        pos = POINT(x,y);
       }
    else
      { q = center(p,r);
        p = p.rotate90(q);
        pos = q + 0.5*(p-q).norm();
       }
    return pos;
  }

  void draw(window& W,node vl, node vr)
  { triangulation& T = *this;
    W.clear();
    node v;
    forall_nodes(v,T)  
    { POINT p = T[v].p1;
      POINT q = T[v].p2;
      POINT r = T[v].p3;
      W.draw_triangle(p,q,r,black);
      if (v == vl || v == vr)
         W.draw_filled_node(node_position(v),green);
      else
         W.draw_filled_node(node_position(v),red);
     }

    edge e;
    forall_edges(e,T)
    { node v = T.source(e);
      node w = T.target(e);
      W.draw_edge_arrow(node_position(v),node_position(w),blue);
     }
   W.read_mouse();
  }
*/

};


//static window W;

node TRIANG_MERGE(triangulation& T, node vl, node vr, node v_last, int i_last)
{

  // preconditions:
  // T[vl].p1 rightmost POINT in left part
  // T[vr].p3 leftmost POINT in right part

  // assert(POINT::cmp_xy(T[vl].p1,T[vr].p3) < 0);  // T[vl].p1 < T[vr].p3

  int count1 = 0;
  int count2 = 0;

  do {

    POINT p = T[vr].p3;
    node  v_succ = T.hull_succ(vl);

    count1 = 0;
    while (orientation(p,T[vl].p1,T[v_succ].p1) < 0)
    { 
      if (count1++ == 0)
      { T.set_target(vl,1,v_last);
        T.set_target(v_last,i_last,vl);
       }

      v_last = vl;
      i_last = 2;

      T[vl].p3 = p;
      vl = v_succ;
      v_succ = T.hull_succ(vl);

      //T.draw(W,vl,vr);
     }

  
    POINT q = T[vl].p1;
    node  v_pred = T.hull_pred(vr);

    count2 = 0;
    while (orientation(q,T[vr].p3,T[v_pred].p3) > 0)
    { 
      if (count2++ == 0)
      { T.set_target(vr,2,v_last);
        T.set_target(v_last,i_last,vr);
       }

      v_last = vr;
      i_last = 1;

      T[vr].p3 = q;
      vr = v_pred; 
      v_pred = T.hull_pred(vr);

      //T.draw(W,vl,vr);
     }

   } while (count1 > 0 || count2 > 0);


  // construct tangent

  node u = T.new_triangle(T[vr].p3,T[vl].p1,T[vl].p1,
                          v_last,vr,vl);

/*
  T.set_target(u,0,v_last);
  T.set_target(u,1,vr);
  T.set_target(u,2,vl);
*/

  T.set_target(v_last,i_last,u);

  T.set_target(vl,1,u);
  T.set_target(vr,2,u);

  //T.draw(W,vl,vr);

  return u;
}



void MERGE(node vl1, node vr1, node vl2, node vr2, triangulation& T, node& vl, node& vr)
{

  node pred1 = T.hull_pred(vr1);
  node pred2 = T.hull_pred(vl2);

  node v = T.new_triangle(POINT(),POINT(),POINT());
  int i = 1;

  node ut = TRIANG_MERGE(T,vr1,pred2,v,i);

  node u = T.get_target(v,1);

  i = 0;
  edge x;
  forall_adj_edges(x,u)
  { if (T.target(x) == v) break;
    i++;
   }

  node lt = TRIANG_MERGE(T,vl2,pred1,u,i);

  T.del_node(v);

  if (T[ut].p1 == T[vr2].p1) vr2 = ut; 
  if (T[lt].p1 == T[vl1].p1) vl1 = lt; 

  vl = vl1;
  vr = vr2;
}


void TRIANG_2(POINT p, POINT q, triangulation& T, node& vl, node& vr)
{
  if (POINT::cmp_xy(q,p) < 0) leda::swap(p,q);
  node a = T.new_triangle(p,q,q);
  node b = T.new_triangle(q,p,p);
  for(int i=0; i<3; i++)
  { T.set_target(a,i,b);
    T.set_target(b,i,a);
   }
  vl = a;
  vr = b;
}


void TRIANG_3(POINT p, POINT q, POINT r, triangulation& T, node& vl, node& vr)
{
  if (POINT::cmp_xy(q,p) < 0) leda::swap(p,q);
  if (POINT::cmp_xy(r,p) < 0) leda::swap(p,r);
  if (POINT::cmp_xy(r,q) < 0) leda::swap(q,r);

  if (orientation(p,q,r) == 0) 
  { node  a = T.new_triangle(p,q,q);
    node  b = T.new_triangle(q,r,r);
    node  c = T.new_triangle(r,q,q);
    node  d = T.new_triangle(q,p,p);

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

  node  a = T.new_triangle(p,r,r);
  node  b = T.new_triangle(r,q,q);
  node  c = T.new_triangle(q,p,p);
  node  d = T.new_triangle(p,q,r);

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




void TRIANG(POINT* l, POINT* r, triangulation& T, node& vl, node& vr)
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

  POINT* m = l + (len-1)/2;

  //POINT*  m = PARTITION(l,r,less_xy);


  node vl1, vr1, vl2, vr2;


  TRIANG(l,m,T,vl1,vr1);
  //T.draw(W,vl1,vr1);

  TRIANG(m+1,r,T,vl2,vr2);
  //T.draw(W,vl2,vr2);

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
  cout << string(" %5.2f sec",used_time(t1)) << endl;
  cout << endl;

  int n = L.length();

  POINT* A = new POINT[n];
  for(int i=0; i<n; i++) A[i] = L.pop();


  GRAPH<POINT,int> T0;
  cout << "triang_points: " << flush;
  float t2 = used_time();
  triang_points(A,A+n-1,T0);
  cout << string(" %5.2f sec",used_time(t2)) << flush;
  cout << endl;


  //L.permute();


  triangulation T;
  node vl,vr;

  cout << "triang_dual:   " << flush;
  float t3 = used_time();
   TRIANG(A,A+n-1,T,vl,vr);
  cout << string(" %5.2f sec",used_time(t3)) << flush;
  cout << endl;

  delete[] A;

  return 0;
}


