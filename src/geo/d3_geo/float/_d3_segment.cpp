/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_segment.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//D3-segment-class
#include <LEDA/geo/d3_segment.h>
#include <LEDA/geo/d3_line.h>
#include <LEDA/geo/d3_plane.h>

//----------------------------------------------
// d3_segment (1999)
//----------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter d3_segment_rep::id_counter(0);

d3_segment_rep::d3_segment_rep(const d3_point& p1,const d3_point& p2) : a(p1),b(p2)
{ id=++id_counter; }

d3_segment::d3_segment(const d3_point& p1,const d3_point& p2)
{
 PTR = new d3_segment_rep(p1,p2);
}

d3_segment::d3_segment(const list<d3_point>& L)
{
 if (L.size() < 2) LEDA_EXCEPTION(1,"d3_segment: constructor: list has fewer than 2 elements.");
 d3_point p1,p2;
 p1= L[L[0]]; p2=L[L[1]];
 PTR = new d3_segment_rep(p1,p2);
}

d3_segment::d3_segment()
{
 d3_point p1(0,0,0),p2(1,0,0);

 PTR = new d3_segment_rep(p1,p2);
}

bool d3_segment::only_one_common(const d3_point& p1, const d3_point& p2) const
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

d3_segment d3_segment::to_d3_segment() const
{
 return *this;
}

d3_segment d3_segment::to_float() const
{
 return *this;
}

bool d3_segment::contains(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();
 if (! collinear(p,p1,p2)) return false;

 //collinear case
 if (in_xyz(p)) return true;

 return false;
}

bool d3_segment::in_xyz(const d3_point& p) const
{
 if ( ( p.xcoord() >= point1().xcoord() && p.xcoord() <= point2().xcoord() ) ||
      ( p.xcoord() >= point2().xcoord() && p.xcoord() <= point1().xcoord() )) ; else return false;

 if ( ( p.ycoord() >= point1().ycoord() && p.ycoord() <= point2().ycoord() ) ||
      ( p.ycoord() >= point2().ycoord() && p.ycoord() <= point1().ycoord() )) ; else return false;

 if ( ( p.zcoord() >= point1().zcoord() && p.zcoord() <= point2().zcoord() ) ||
      ( p.zcoord() >= point2().zcoord() && p.zcoord() <= point1().zcoord() )) ; else return false;

 return true;
}

segment d3_segment::project_xy() const
{
 d3_point p1=point1();
 d3_point p2=point2();

 return segment(p1.project_xy(),p2.project_xy());
}

segment d3_segment::project_xz() const
{
 d3_point p1=point1();
 d3_point p2=point2();

 return segment(p1.project_xz(),p2.project_xz());
}

segment d3_segment::project_yz() const
{
 d3_point p1=point1();
 d3_point p2=point2();

 return segment(p1.project_yz(),p2.project_yz());
}

d3_segment d3_segment::project(const d3_point& pt, const d3_point& qt, const d3_point& vt) const
{
 d3_plane p(pt,qt,vt);
 vector v1=p.normal_project(point1());
 vector v2=p.normal_project(point2());

 return d3_segment(point1()+v1,point2()+v2);
}

d3_segment d3_segment::reflect(const d3_point& pt, const d3_point& qt, const d3_point& vt) const
{
 d3_plane p(pt,qt,vt);
 return d3_segment(p.reflect_point(point1()),p.reflect_point(point2()));
}

d3_segment d3_segment::reflect(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();

 return d3_segment(p1.reflect(p),p2.reflect(p)); 
}

d3_segment d3_segment::reverse() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 
 return d3_segment(p2,p1);
}

bool overlap(const segment& s1,const segment& s2)
{
  if (s1.contains(s2.source()) || s1.contains(s2.target()) ||
      s2.contains(s1.source()) || s2.contains(s1.target())) return true;
  else return false;
}

bool d3_segment::project_2d_intersection(const d3_segment& t) const
{
 segment s1,s2;
 bool b1,b2,b3;

 // xy -plane ...
 s1=project_xy();s2=t.project_xy();
 b1= s1.intersection(s2);
 if (!b1 && overlap(s1,s2) ) b1=true;
 if (! b1) return false;

 // xz -plane ... 
 s1=project_xz();s2=t.project_xz();
 b2= s1.intersection(s2);
 if (!b2 && overlap(s1,s2) ) b2=true;
 if (! b2) return false; 
 
 s1=project_yz();s2=t.project_yz();
 b3= s1.intersection(s2);
 if (!b3 && overlap(s1,s2) ) b3=true;

 if (b1 && b2 && b3) return true; 
 else return false;
}

bool d3_segment::intersection(const d3_segment& t) const
{
 if (t.is_trivial() ) return contains(t.point1());
 if (is_trivial() ) return t.contains(point1());

 d3_line l(t);
 d3_point p;
 bool b= l.intersection(*this,p);

 if (b && project_2d_intersection(t)) return true;
 if (! b && collinear(point1(),point2(),t.point1()) && collinear(point1(),point2(),t.point2()))
 {
   d3_point p1= point1(), p2=point2();
   d3_point tp1=t.point1(), tp2=t.point2();

   if (tp1==p1 || tp1==p2 || tp2==p1 || tp2==p2)
   {
     if (only_one_common(tp1,tp2)) return true;
     else return false;
   }
 }

 return false;
}

bool d3_segment::intersection(const d3_segment& t,d3_point& p) const
{
 if (! project_2d_intersection(t)) return false;
 
 if (t.is_trivial() ) { 
   bool b=contains(t.point1());
   if (b) { p=t.point1(); return true; }
   else return false;
 }
 if (is_trivial() ) {
   bool b=t.contains(point1());
   if (b) { p=point1(); return true; }
   else return false;
 }

 d3_line l(t);
 bool b= l.intersection(d3_line(*this),p);
 
 //cout << b << " result of line intersection!\n";

 if (b) return true;
 if (! b && collinear(point1(),point2(),t.point1()) && collinear(point1(),point2(),t.point2()))
 {
   d3_point p1= point1(), p2=point2();
   d3_point tp1=t.point1(), tp2=t.point2();

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

bool d3_segment::intersection_of_lines(const d3_segment& t,d3_point& p) const
{
 d3_line l(t);
 d3_line l2(*this);

 return l.intersection(l2,p);
}

double d3_segment::sqr_length() const
{
 d3_point p1=point1();

 return p1.sqr_dist(point2());
}

double d3_segment::length() const
{
 d3_point p1=point1();

 return p1.distance(point2());
}

d3_segment d3_segment::translate(const vector& v) const
// precondition v.dim==3
{
   d3_point a=point1();
   d3_point b=point2();
 
   a=a.translate(v); b=b.translate(v);
   return d3_segment(a,b); 
}

d3_segment d3_segment::translate(double wx,double wy,double wz) const
{
   d3_point a=point1();
   d3_point b=point2();

   a=a.translate(wx,wy,wz); b=b.translate(wx,wy,wz);
   return d3_segment(a,b);
}

ostream& operator<<(ostream& s, const d3_segment& T){
 d3_point a,b;
 a=T.point1(); b=T.point2();
 s << a;
 s << b;
 return s;
}

istream& operator>>(istream& in, d3_segment& T)
{
 d3_point a,b;
 in >> a;
 in >> b; 
 
 T=d3_segment(a,b);

 return in;
}

LEDA_END_NAMESPACE



