/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_ray.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/real_ray.h>
#include <LEDA/geo/rat_ray.h>

//------------------------------------------------------------------------------
// real rays 
//
// by S. Thiel (2003)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

real_ray::real_ray()
{ PTR = new real_ray_rep; }

real_ray::real_ray(const real_segment& s) 
{ PTR = new real_ray_rep(s); }

real_ray::real_ray(const real_point& x, const real_point& y)    
{ PTR = new real_ray_rep(real_segment(x,y)); }

real_ray::real_ray(const real_point& p, const real_vector& v) 
{ PTR = new real_ray_rep(real_segment(p,v)); }

real_ray::real_ray(const rat_ray& r1) 
{ PTR = new real_ray_rep(real_segment(real_point(r1.source()), 
                                      real_point(r1.point2()))); }


bool real_ray::contains(const real_point& p) const
{ if (orientation(ptr()->seg,p) == 0) 
  { // p lies on line
    real_segment s = ptr()->seg.rotate90();
    return (orientation(s,p) <= 0);
   }
  return false;
}


bool real_ray::contains(const real_segment& s) const
{ return contains(s.start()) && contains(s.end()); }


bool real_ray::intersection(const real_ray& r, real_point& inter) const
{ if ( ! ptr()->seg.intersection_of_lines(r.ptr()->seg,inter) ) return false;
  return orientation(ptr()->seg.rotate90(),inter)   <= 0 &&
         orientation(r.ptr()->seg.rotate90(),inter) <= 0;
}


bool real_ray::intersection(const real_segment& s, real_point& inter) const
{ if ( ! ptr()->seg.intersection_of_lines(s,inter) ) return false;
  return orientation(ptr()->seg.rotate90(),inter) <= 0 &&
         orientation(s.rotate90(),inter)          <= 0 &&
         orientation(s.reverse().rotate90(),inter)<= 0;
}


bool real_ray::operator==(const real_ray& g) const 
{ real_segment s1 = ptr()->seg;
  real_segment s2 = g.ptr()->seg;
  return (s1.source() == s2.source()) && contains(s2); 
}


istream& operator>>(istream& in, real_ray& r)  
{ real_segment s; 
  in >> s; 
  r = real_ray(s); 
  return in; 
 }

LEDA_END_NAMESPACE 
