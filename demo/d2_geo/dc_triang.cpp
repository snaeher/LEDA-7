/*******************************************************************************
+
+  LEDA 7.2.2  
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


#include <LEDA/graphics/window.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/tuple.h>
#include <LEDA/geo/point.h>
#include <LEDA/system/assert.h>
#include <LEDA/core/quicksort.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
using std::ostream;
using std::istream;
#endif



bool less_xy(const point& p, const point& q)
{ return (point::cmp_xy(p,q) < 0); } 

bool less_yx(const point& p, const point& q)
{ return (point::cmp_yx(p,q) < 0); } 



template<class E, class LessThen>
void QUICKSORT(E* l, E* r,LessThen lt)
{ if (lt(*r,*l)) leda::swap(*l,*r);
  if (r == l+1) return;
  E* k = l + (r-l)/2;
  if (lt(*k,*l)) 
     leda::swap(*l,*k);
  else
     if (lt(*r,*k)) leda::swap(*k,*r);
  if (r == l+2) return;
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
  if (j > l+1) QUICKSORT(l,j-1,lt);
  if (r > j+1) QUICKSORT(j+1,r,lt);
}


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

point p1;
point p2;
point p3;

Triangle() {}
Triangle(const point& a, const point& b, const point c) : p1(a), p2(b), p3(c) {}
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

  node  new_triangle(point a, point b , point c) 
  { 
    node v = new_node(Triangle(a,b,c)); 

    new_edge(v,v); // edge 0
    new_edge(v,v); // edge 1
    new_edge(v,v); // edge 2

    return v;
   }

  edge get_edge(node v, int i) const 
  { if (i == 0) return first_adj_edge(v);
    if (i == 1) return adj_succ(first_adj_edge(v));
    if (i == 2) return last_adj_edge(v);
    return 0;
   }

  node hull_succ(node v)  const { return target(get_edge(v,2)); }
  node hull_pred(node v)  const { return target(get_edge(v,1)); }

  void set_target(edge e, node v) { move_edge(e,source(e),v); }

  void set_target(node u, int i, node v) { set_target(get_edge(u,i),v); }
  node get_target(node u, int i) const   { return target(get_edge(u,i)); }


  point node_position(node v)
  { point p = inf(v).p1;
    point q = inf(v).p2;
    point r = inf(v).p3;
    point pos;
    if (orientation(p,q,r) != 0)
      { double x = (p.xcoord() + q.xcoord() + r.xcoord())/3;
        double y = (p.ycoord() + q.ycoord() + r.ycoord())/3;
        pos = point(x,y);
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
    { point p = T[v].p1;
      point q = T[v].p2;
      point r = T[v].p3;
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

};


static window W;

node TRIANG_MERGE(triangulation& T, node vl, node vr, node v_last, int i_last)
{

  // preconditions:
  // T[vl].p1 rightmost point in left part
  // T[vr].p3 leftmost point in right part

  // assert(point::cmp_xy(T[vl].p1,T[vr].p3) < 0);  // T[vl].p1 < T[vr].p3

  int count1 = 0;
  int count2 = 0;

  do {

    point p = T[vr].p3;
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

      T.draw(W,vl,vr);
     }

  
    point q = T[vl].p1;
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

      T.draw(W,vl,vr);
     }

   } while (count1 > 0 || count2 > 0);


  // construct tangent

  node u = T.new_triangle(T[vr].p3,T[vl].p1,T[vl].p1);

  T.set_target(u,1,vr);
  T.set_target(u,2,vl);

  T.set_target(u,0,v_last);
  T.set_target(v_last,i_last,u);

  T.set_target(vl,1,u);
  T.set_target(vr,2,u);

T.draw(W,vl,vr);

  return u;
}



void MERGE(node vl1, node vr1, node vl2, node vr2, triangulation& T, node& vl, node& vr)
{

  node pred1 = T.hull_pred(vr1);
  node pred2 = T.hull_pred(vl2);

  node v = T.new_triangle(point(),point(),point());
  int i = 1;

  node ut = TRIANG_MERGE(T,vr1,pred2,v,i);

  node u = T.get_target(v,1);

  if (T.get_target(u,0) == v) i = 0;
  if (T.get_target(u,1) == v) i = 1;
  if (T.get_target(u,2) == v) i = 2;

  node lt = TRIANG_MERGE(T,vl2,pred1,u,i);

  T.del_node(v);

  if (T[ut].p1 == T[vr2].p1) vr2 = ut; 
  if (T[lt].p1 == T[vl1].p1) vl1 = lt; 

  vl = vl1;
  vr = vr2;
}


void TRIANG_2(point p, point q, triangulation& T, node& vl, node& vr)
{
  if (point::cmp_xy(q,p) < 0) leda::swap(p,q);
  node a = T.new_triangle(p,q,q);
  node b = T.new_triangle(q,p,p);
  for(int i=0; i<3; i++)
  { T.set_target(a,i,b);
    T.set_target(b,i,a);
   }
  vl = a;
  vr = b;
}


void TRIANG_3(point p, point q, point r, triangulation& T, node& vl, node& vr)
{
  if (point::cmp_xy(q,p) < 0) leda::swap(p,q);
  if (point::cmp_xy(r,p) < 0) leda::swap(p,r);
  if (point::cmp_xy(r,q) < 0) leda::swap(q,r);

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




void TRIANG(point* l, point* r, triangulation& T, node& vl, node& vr)
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

  //point* m = (l+r)/2;

  //point*  m = PARTITION(l,r,less_xy);
  point*  m = PARTITION(l,r,less_yx);


  node vl1, vr1, vl2, vr2;


  TRIANG(l,m,T,vl1,vr1);
  T.draw(W,vl1,vr1);

  TRIANG(m+1,r,T,vl2,vr2);
  T.draw(W,vl2,vr2);

  MERGE(vl1,vr1,vl2,vr2,T,vl,vr);
}


int main()
{
   point A[512];

/*
   A[0] = point(0,0);
   A[1] = point(0,2);
   A[2] = point(2,1);
   A[3] = point(4,1);
   A[4] = point(6,0);
   A[5] = point(6,2);
*/

/*
   A[0] = point(0,2);
   A[1] = point(1,0);
   A[2] = point(2,2);
   A[3] = point(4,2);
   A[4] = point(5,0);
   A[5] = point(6,2);
*/


   triangulation T;
   node vl,vr;


   W.init(-10,10,-10);
   W.set_node_width(3);
   W.set_grid_dist(1);
   W.set_show_coordinates(true);
   W.set_grid_style(line_grid);
   W.set_point_style(disc_point);
   W.display(window::center,window::center);

   int n = 0;
   point p;
   while (W >> p) 
   { W << p;
     A[n++] = p;
    }

   //QUICKSORT_C(&A[0],&A[n-1]);

   TRIANG(A,A+n-1,T,vl,vr);

   T.draw(W,vl,vr);

   return 0;
}




  
  
