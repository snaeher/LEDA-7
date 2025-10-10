/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_ray.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_ray.h>
#include <LEDA/geo/d3_plane.h>
#include <LEDA/geo/d3_line.h>

//----------------------------------------------
// d3_ray (1999)
//----------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter d3_ray_rep::id_counter(0);
   
d3_ray_rep::d3_ray_rep(const d3_segment& s2) : seg(s2)
{
 if (s2.is_trivial()) LEDA_EXCEPTION(1,"d3_ray_rep: constructor got trivial segment");
 id=++id_counter;
}

d3_ray::d3_ray(const d3_point& p1,const d3_point& p2)
{
 PTR = new d3_ray_rep(d3_segment(p1,p2));
}

d3_ray::d3_ray(const list<d3_point>& L)
{
 if (L.size() < 2) LEDA_EXCEPTION(1,"d3_ray: constructor: list has fewer than 2 elements.");
 d3_point p1,p2;
 p1= L[L[0]]; p2=L[L[1]];
 PTR = new d3_ray_rep(d3_segment(p1,p2));
}

d3_ray::d3_ray(const d3_segment& s)
{
 PTR = new d3_ray_rep(s);
}

d3_ray::d3_ray()
{
 PTR = new d3_ray_rep(d3_segment(d3_point(0,0,0),d3_point(1,1,1)));
}

bool d3_ray::contains(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();
 if (! collinear(p,p1,p2)) return false;
 
 // collinear ...
 if (in_xyz(p)) return true;
 return false; 
}

bool d3_ray::contains(const d3_segment& s) const
{
 d3_point p1 = s.source();
 d3_point p2 = s.target();
 
 if (contains(p1) && contains(p2)) return true;
 else return false;
}

bool d3_ray::in_xyz(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();
 
 int c1,c2,c3;
 int d1,d2,d3;
 
 c1 = compare(p1.xcoord(),p2.xcoord());
 d1 = compare(p1.xcoord(),p.xcoord());
 if (d1==0 || c1==d1) ; else return false;
 
 c2 = compare(p1.ycoord(),p2.ycoord());
 d2 = compare(p1.ycoord(),p.ycoord());
 if (d2==0 || c2==d2) ; else return false;
 
 c3 = compare(p1.zcoord(),p2.zcoord());
 d3 = compare(p1.zcoord(),p.zcoord());
 if (d3==0 || c3==d3) ; else return false; 
 
 return true;
}

d3_point d3_ray::source() const
{
 d3_segment s= ptr()->seg;
 return s.point1();
}

d3_point d3_ray::point1() const
{
 d3_segment s= ptr()->seg;
 return s.point1();
}

d3_point d3_ray::point2() const
{
 d3_segment s= ptr()->seg;
 return s.point2();
}

d3_segment d3_ray::seg() const
{
 d3_segment s= ptr()->seg;
 return s;
}
 
bool d3_ray::x_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.xcoord() == p2.xcoord());
}

bool d3_ray::y_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.ycoord() == p2.ycoord());
}

bool d3_ray::z_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.zcoord() == p2.zcoord());
}

bool d3_ray::xy_equal() const
{ return (x_equal() && y_equal()); }

bool d3_ray::xz_equal() const
{  return (x_equal() && z_equal()); }

bool d3_ray::yz_equal() const
{ return (y_equal() && z_equal()); }

bool d3_ray::project_xy(ray& m) const 
{
 d3_segment ds= seg();
 segment spr= ds.project_xy();
 if (spr.is_trivial()) return false;
 else { m=ray(spr); return true; }
}

bool d3_ray::intersection(const d3_segment& s, d3_point& inter) const
{
  d3_line l1(s);
  d3_line l2(point1(),point2());
  d3_point pt;
  
  bool i = l1.intersection(l2,pt);
  if (! i) return false;
  
  if (s.in_xyz(pt) && in_xyz(pt)) { inter=pt; return true; }
  else return false;  
}
  
bool d3_ray::intersection(const d3_ray& r, d3_point& inter) const
{
  d3_line l1(r.point1(),r.point2());
  d3_line l2(point1(),point2());
  d3_point pt;
  
  bool i = l1.intersection(l2,pt);
  if (! i) return false;
  
  if (r.in_xyz(pt) && in_xyz(pt)) { inter= pt; return true; }

  else return false;
}

bool d3_ray::project_xz(ray& m) const 
{
 d3_segment ds= seg();
 segment spr= ds.project_xz();
 if (spr.is_trivial()) return false;
 else { m=ray(spr); return true; }
}

bool d3_ray::project_yz(ray& m) const 
{
 d3_segment ds= seg();
 segment spr= ds.project_yz();
 if (spr.is_trivial()) return false;
 else { m=ray(spr); return true; }
}

bool d3_ray::project(const d3_point& pt, const d3_point& qt, const d3_point& vt, d3_ray& m) const
{
 d3_plane p(pt,qt,vt);
 vector v1=p.normal_project(point1());
 vector v2=p.normal_project(point2());
 d3_point pn1= point1()+v1, pn2= point2()+v2;
 if (pn1==pn2) return false;
 m=d3_ray(pn1,pn2);
 return true;
}

d3_ray d3_ray::reverse() const
{
  d3_point p1 = point1();
  d3_point p2 = p1 + (p1 - point2());
  
  return d3_ray(p1,p2);
}

d3_ray d3_ray::translate(const vector& v) const
{
  d3_point p1 = point1();
  d3_point p2 = point2();
  
  return d3_ray(p1.translate(v),p2.translate(v));
}

d3_ray d3_ray::translate(double dx, double dy, double dz) const
{
  d3_point p1 = point1();
  d3_point p2 = point2();
  
  return d3_ray(p1.translate(dx,dy,dz),p2.translate(dx,dy,dz));
}

d3_ray d3_ray::reflect(const d3_point& pt, const d3_point& qt, const d3_point& vt) const
{
 d3_plane p(pt,qt,vt);
 return d3_ray(p.reflect_point(point1()),p.reflect_point(point2()));
}

d3_ray d3_ray::reflect(const d3_point& p) const
{
 d3_segment ds= seg().reflect(p);
 return d3_ray(ds); 
}

vector d3_ray::to_vector() const
{
 d3_segment seg= ptr()->seg;
 return seg.to_vector();
}

 
LEDA_END_NAMESPACE 
 
 
 
