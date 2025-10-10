/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ray.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/ray.h>

//------------------------------------------------------------------------------
// rays 
//
// by S. Naeher (1995,1996)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

ray::ray()
{ PTR = new ray_rep; }

ray::ray(const segment& s) 
{ PTR = new ray_rep(s); }

ray::ray(const point& x, const point& y)    
{ PTR = new ray_rep(segment(x,y)); }

ray::ray(const point& p, const vector& v) 
{ PTR = new ray_rep(segment(p,v)); }

ray::ray(const point& p, double alpha) 
{ PTR = new ray_rep(segment(p,alpha,1)); }
  


bool ray::contains(const point& p) const
{ if (orientation(ptr()->seg,p) == 0) 
  { // p lies on line
    segment s = ptr()->seg.rotate90();
    return (orientation(s,p) <= 0);
   }
  return false;
}


bool ray::contains(const segment& s) const
{ return contains(s.start()) && contains(s.end()); }


bool ray::intersection(const ray& r, point& inter) const
{ if ( ! ptr()->seg.intersection_of_lines(r.ptr()->seg,inter) ) return false;
  return orientation(ptr()->seg.rotate90(),inter)   <= 0 &&
         orientation(r.ptr()->seg.rotate90(),inter) <= 0;
}


bool ray::intersection(const segment& s, point& inter) const
{ if ( ! ptr()->seg.intersection_of_lines(s,inter) ) return false;
  return orientation(ptr()->seg.rotate90(),inter) <= 0 &&
         orientation(s.rotate90(),inter)          <= 0 &&
         orientation(s.reverse().rotate90(),inter)<= 0;
}


bool ray::operator==(const ray& g) const 
{ segment s1 = ptr()->seg;
  segment s2 = g.ptr()->seg;
  return (s1.source() == s2.source()) && contains(s2); 
}


istream& operator>>(istream& in, ray& r)  
{ segment s; 
  in >> s; 
  r = ray(s); 
  return in; 
 }

LEDA_END_NAMESPACE 
