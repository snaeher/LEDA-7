/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_line.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/real_line.h>
#include <LEDA/geo/rat_line.h>

//------------------------------------------------------------------------------
// real lines 
//
// by S. Thiel (2003)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

real_line::real_line()
{ PTR = new real_line_rep; }

real_line::real_line(const real_segment& s) 
{ PTR = new real_line_rep(s); }

real_line::real_line(const real_ray& r) 
{ PTR = new real_line_rep(r.ptr()->seg); }

real_line::real_line(const real_point& x, const real_point& y)    
{ PTR = new real_line_rep(real_segment(x,y)); }

real_line::real_line(const real_point& p, const real_vector& v) 
{ PTR = new real_line_rep(real_segment(p,v)); }

real_line::real_line(const rat_line& l)
{ PTR = new real_line_rep(real_segment(l.seg())); }



bool real_line::contains(const real_point& p) const
{ return orientation(p) == 0; }

bool real_line::contains(const real_segment& s) const
{ return contains(s.start()) && contains(s.end()); }


bool real_line::intersection(const real_line& s, real_point& inter) const
{ return ptr()->seg.intersection_of_lines(s.ptr()->seg,inter) ; }



bool real_line::intersection(const real_segment& s, real_point& inter) const
{ 
  int orient1 = orientation(s.start());
  int orient2 = orientation(s.end());

  if (orient1 == 0 && orient2 == 0)
  { inter = s.start();
    return s.is_trivial();
   }

  if (orient1*orient2 > 0) return false;

  ptr()->seg.intersection_of_lines(s,inter);
  return true;
}


bool real_line::intersection(const real_segment& s) const
{
  int orient1 = orientation(s.start());
  int orient2 = orientation(s.end());
  
  if ((orient1 != orient2) || (orient1 == 0 && orient2 == 0)) return true;
  return false;  
}


real_segment real_line::perpendicular(const real_point& q) const
{ return ptr()->seg.perpendicular(q); }



real_point real_line::dual() const
{
  if (is_vertical())
    LEDA_EXCEPTION(1,"real_line::dual(): line is vertical.");

  return real_point(-slope(),y_proj(0));
}




real real_line::sqr_dist(const real_point& q) const
{ real_segment s = ptr()->seg.perpendicular(q); 
  return s.sqr_length();
}

real real_line::distance(const real_point& q) const 
{ return sqrt(sqr_dist(q)); }


real_line p_bisector(const real_point& p, const real_point& q)
{ real_segment s(center(p,q),q);
  return s.rotate90();
}

istream& operator>>(istream& in, real_line& l)  
{ real_segment s; 
  in >> s; 
  l = real_line(s); 
  return in; 
 }

LEDA_END_NAMESPACE 
