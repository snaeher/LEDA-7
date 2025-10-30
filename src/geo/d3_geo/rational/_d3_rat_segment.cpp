/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_rat_segment.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_rat_segment.h>
#include <LEDA/geo/d3_rat_line.h>
#include <LEDA/geo/d3_rat_plane.h>

//----------------------------------------------
// d3_rat_segment (1999)
//----------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter d3_rat_segment_rep::id_counter(0);

d3_rat_segment_rep::d3_rat_segment_rep(const d3_rat_point& p1,const d3_rat_point& p2) : a(p1),b(p2)
{
 id=++id_counter;
}

d3_rat_segment::d3_rat_segment(const d3_rat_point& p1,const d3_rat_point& p2)
{
 PTR = new d3_rat_segment_rep(p1,p2);
}

d3_rat_segment::d3_rat_segment(const list<d3_rat_point>& L)
{
 if (L.size() < 2) LEDA_EXCEPTION(1,"d3_rat_segment: constructor: list has fewer than 2 elements.");
 d3_rat_point p1,p2;
 p1= L[L[0]]; p2=L[L[1]];
 PTR = new d3_rat_segment_rep(p1,p2);
}

d3_rat_segment::d3_rat_segment()
{
 d3_rat_point p1(0,0,0,1),p2(1,0,0,1);

 PTR = new d3_rat_segment_rep(p1,p2);
}

bool d3_rat_segment::only_one_common(const d3_rat_point& p1, const d3_rat_point& p2) const
// p1 and p2 have to be coplanar to the points of the segment...
{
  if (p1==point1()){
   if (compare(p1,point2()) != compare(p1,p2)) return true; else return false;
  }
  if (p1==point2()){
   if (compare(p1,point1()) != compare(p1,p2)) return true; else return false;
  }
  if (p2==point1()){
   if (compare(p2,point2()) != compare(p2,p1)) return true; else return false;
  }
  if (p2==point2()){
   if (compare(p2,point1()) != compare(p2,p1)) return true; else return false;
  }
  return false;
}

d3_segment d3_rat_segment::to_d3_segment() const
{
 d3_rat_point p1,p2;
 p1=source(); p2=target();
 return d3_segment(p1.to_d3_point(),p2.to_d3_point());
}

d3_segment d3_rat_segment::to_float() const
{
 d3_rat_point p1,p2;
 p1=source(); p2=target();
 return d3_segment(p1.to_d3_point(),p2.to_d3_point());
}

bool d3_rat_segment::contains(const d3_rat_point& p) const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 if (! collinear(p,p1,p2)) return false;

 //collinear case
 if (in_xyz(p)) return true;

 return false;
}


bool d3_rat_segment::in_xyz(const d3_rat_point& p) const
{
/*
 if ( ( p.xcoord() >= point1().xcoord() && p.xcoord() <= point2().xcoord() ) ||
      ( p.xcoord() >= point2().xcoord() && p.xcoord() <= point1().xcoord() )); else return false;

 if ( ( p.ycoord() >= point1().ycoord() && p.ycoord() <= point2().ycoord() ) ||
      ( p.ycoord() >= point2().ycoord() && p.ycoord() <= point1().ycoord() )); else return false;

 if ( ( p.zcoord() >= point1().zcoord() && p.zcoord() <= point2().zcoord() ) ||
      ( p.zcoord() >= point2().zcoord() && p.zcoord() <= point1().zcoord() )); else return false;
*/
 d3_rat_point a=point1();
 d3_rat_point b=point2(); 
 int w1,w2;
 
 w1 = d3_rat_point::cmp_x(p,a); w2 = d3_rat_point::cmp_x(p,b);
 if ( ( w1 >= 0 && w2 <= 0 ) || ( w1 <= 0 && w2 >= 0  )); else return false;

 w1 = d3_rat_point::cmp_y(p,a); w2 = d3_rat_point::cmp_y(p,b); 
 if ( ( w1 >= 0 && w2 <= 0 ) || ( w1 <= 0 && w2 >= 0  )); else return false;

 w1 = d3_rat_point::cmp_z(p,a); w2 = d3_rat_point::cmp_z(p,b);
 if ( ( w1 >= 0 && w2 <= 0 ) || ( w1 <= 0 && w2 >= 0  )); else return false;      


 return true;
}


rat_segment d3_rat_segment::project_xy() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();

 return rat_segment(p1.project_xy(),p2.project_xy());
}

rat_segment d3_rat_segment::project_xz() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();

 return rat_segment(p1.project_xz(),p2.project_xz());
}

rat_segment d3_rat_segment::project_yz() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();

 return rat_segment(p1.project_yz(),p2.project_yz());
}

d3_rat_segment d3_rat_segment::project(const d3_rat_point& p, const d3_rat_point& q, const d3_rat_point& v) const
{
 d3_rat_plane pl(p,q,v);

 rat_vector v1=pl.normal_project(point1());
 rat_vector v2=pl.normal_project(point2());

 return d3_rat_segment(point1()+v1,point2()+v2);
}

d3_rat_segment d3_rat_segment::reflect(const d3_rat_point& p, const d3_rat_point& q, const d3_rat_point& v) const
{
 d3_rat_plane pl(p,q,v);
 return d3_rat_segment(pl.reflect_point(point1()),pl.reflect_point(point2()));
}

d3_rat_segment d3_rat_segment::reflect(const d3_rat_point& p) const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();

 return d3_rat_segment(p1.reflect(p),p2.reflect(p)); 
}

d3_rat_segment d3_rat_segment::reverse() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 
 return d3_rat_segment(p2,p1);
}

bool d3_rat_segment::intersection(const d3_rat_segment& t) const
{
 if (t.is_trivial() ) return contains(t.point1());
 if (is_trivial() ) return t.contains(point1());

 d3_rat_line l(t);

 d3_rat_point p;
 bool b= l.intersection(*this,p);

 if (b && t.in_xyz(p)) return true;
 if (! b && collinear(point1(),point2(),t.point1()) && collinear(point1(),point2(),t.point2()))
 {
   d3_rat_point p1= point1(), p2=point2();
   d3_rat_point tp1=t.point1(), tp2=t.point2();

   if (tp1==p1 || tp1==p2 || tp2==p1 || tp2==p2)
   {
     if (only_one_common(tp1,tp2)) return true; 
     else return false;
   }
 }

 return false;
}

bool d3_rat_segment::intersection(const d3_rat_segment& t,d3_rat_point& p) const
{
 if (t.is_trivial() ) {  
   bool b=contains(t.point1());
   if (b) {p=t.point1(); return true;}
   else return false;
 }
 if (is_trivial() ) {
   bool b=t.contains(point1());
   if (b) {p=point1(); return true;}
   else return false;
 }

 d3_rat_line l(t);
 //cout << "line:" << l << "\n";
 bool b= l.intersection(*this,p);

 if (b && t.in_xyz(p)) return true;
 if (! b && collinear(point1(),point2(),t.point1()) && collinear(point1(),point2(),t.point2()))
 {
   d3_rat_point p1= point1(), p2=point2();
   d3_rat_point tp1=t.point1(), tp2=t.point2();

   if (tp1==p1 || tp1==p2 || tp2==p1 || tp2==p2)
   {
     if (only_one_common(tp1,tp2)) {
       if (tp1==p1 || tp2==p1) p=p1;
       else p=p2; 
       return true;
     }
     else return false;
   }
 } 

 return false;
}

bool d3_rat_segment::intersection_of_lines(const d3_rat_segment& t,d3_rat_point& p) const
{
 d3_rat_line l(t);
 d3_rat_line l2(*this);

 return l.intersection(l2,p);
}

rational d3_rat_segment::sqr_length() const
{
 d3_rat_point p1=point1();

 return p1.sqr_dist(point2());
}

d3_rat_segment d3_rat_segment::translate(const rat_vector& v) const
// precondition v.dim==3
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();
 
   a=a.translate(v); b=b.translate(v);
   return d3_rat_segment(a,b); 
}

d3_rat_segment d3_rat_segment::translate(rational wx,rational wy,rational wz) const
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();

   a=a.translate(wx,wy,wz); b=b.translate(wx,wy,wz);
   return d3_rat_segment(a,b);
}

d3_rat_segment d3_rat_segment::translate(integer wx,integer wy,integer wz,integer ww) const
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();

   a=a.translate(wx,wy,wz,ww); b=b.translate(wx,wy,wz,ww);
   return d3_rat_segment(a,b);
}

ostream& operator<<(ostream& s, const d3_rat_segment& T){
 d3_rat_point a,b;
 a=T.point1(); b=T.point2();
 s << a;
 s << b;
 return s;
}

istream& operator>>(istream& in, d3_rat_segment& T)
{
 d3_rat_point a,b;
 in >> a;
 in >> b; 
 
 T=d3_rat_segment(a,b);

 return in;
}

LEDA_END_NAMESPACE

