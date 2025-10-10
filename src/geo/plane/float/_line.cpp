/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _line.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/line.h>

//------------------------------------------------------------------------------
// lines 
//
// by S. Naeher (1995)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

line::line()
{ PTR = new line_rep; }

line::line(const segment& s) 
{ PTR = new line_rep(s); }

line::line(const ray& r) 
{ PTR = new line_rep(r.ptr()->seg); }

line::line(const point& x, const point& y)    
{ PTR = new line_rep(segment(x,y)); }

line::line(const point& p, const vector& v) 
{ PTR = new line_rep(segment(p,v)); }

line::line(const point& p, double alpha) 
{ PTR = new line_rep(segment(p,alpha,1)); }
  


bool line::contains(const point& p) const
{ return orientation(p) == 0; }

bool line::contains(const segment& s) const
{ return contains(s.start()) && contains(s.end()); }


bool line::intersection(const line& s, point& inter) const
{ return ptr()->seg.intersection_of_lines(s.ptr()->seg,inter) ; }



bool line::intersection(const segment& s, point& inter) const
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


bool line::intersection(const segment& s) const
{
  int orient1 = orientation(s.start());
  int orient2 = orientation(s.end());
  
  if ((orient1 != orient2) || (orient1 == 0 && orient2 == 0)) return true;
  return false;  
}


segment line::perpendicular(const point& q) const
{ return ptr()->seg.perpendicular(q); }



point line::dual() const
{
  if (is_vertical())
    LEDA_EXCEPTION(1,"line::dual(): line is vertical.");

  return point(-slope(),y_proj(0));
}


double line::sqr_dist(const point& q) const
{ segment s = ptr()->seg.perpendicular(q); 
  return s.sqr_length();
}

double line::distance(const point& q) const 
{ return sqrt(sqr_dist(q)); }


bool line::clip(point p, point q, segment& s) const
{ 
  const line& l = *this;
  point a = l.point1();
  point b = l.point2();
  if ( compare(a,b) > 0 ) leda_swap(a,b);

  if ( compare(p,q) > 0 ) leda_swap(p,q);

  point luc(p.xcoord(),q.ycoord());  // left upper corner
  point rlc(q.xcoord(),p.ycoord());  // right lower corner
  
  line left  (p, luc);
  line right (q, rlc);
  line bottom(p, rlc);
  line top   (q, luc);

  point p_left,p_right,p_bottom, p_top;

  l.intersection(left,p_left);
  l.intersection(right,p_right);
  l.intersection(bottom,p_bottom);
  l.intersection(top,p_top);


  if ( point::cmp_y(a,b) == 0 )
  { // horizontal
    s = segment(p_left,p_right);
    return a.orientation(b,p) != a.orientation(b,q);
  }
  if ( point::cmp_x(a,b) == 0 )
  { // vertical 
    s = segment(p_bottom,p_top); 
    return a.orientation(b,p) != a.orientation(b,q);
  }
  if ( point::cmp_y(a,b) < 0 )
  { // positive slope
    point c = compare(p_left,p_bottom) < 0 ? p_bottom : p_left;
    point d = compare(p_right,p_top) < 0   ? p_right : p_top;
    s = segment(c,d);
    return a.orientation(b,luc) != a.orientation(b,rlc);
  }
  // negative slope
  point c = compare(p_left,p_top) < 0    ? p_top : p_left;
  point d = compare(p_right,p_bottom) <0 ? p_right : p_bottom;
  s = segment(c,d);
  return a.orientation(b,p) != a.orientation(b,q);
}
    




line p_bisector(const point& p, const point& q)
{ segment s(center(p,q),q);
  return s.rotate90();
}

istream& operator>>(istream& in, line& l)  
{ segment s; 
  in >> s; 
  l = line(s); 
  return in; 
 }

LEDA_END_NAMESPACE 
