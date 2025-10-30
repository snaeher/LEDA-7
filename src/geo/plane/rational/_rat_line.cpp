/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rat_line.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/






#include <LEDA/geo/rat_line.h>

//------------------------------------------------------------------------------
// rat_lines 
//
// S. Naeher (1996)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


rat_line::rat_line()
{ PTR = new rat_line_rep; }

rat_line::rat_line(const rat_segment& s) 
{ PTR = new rat_line_rep(s); 
  if (s.is_trivial())
    LEDA_EXCEPTION(1,"rat_line: cannot construct line from trivial segment");
 }

rat_line::rat_line(const rat_point& p, const rat_vector& v) 
{ rat_segment s(p,v); 
  PTR = new rat_line_rep(s); 
  if (s.is_trivial())
    LEDA_EXCEPTION(1,"rat_line: cannot construct line from trivial segment");
}

rat_line::rat_line(const rat_ray& r) 
{ PTR = new rat_line_rep(r.ptr()->seg); }

rat_line::rat_line(const rat_point& x, const rat_point& y)    
{ rat_segment s(x,y); 
  PTR = new rat_line_rep(s); 
  if (s.is_trivial())
    LEDA_EXCEPTION(1,"rat_line: cannot construct line from trivial segment");
}


line rat_line::to_line() const
{ return line(ptr()->seg.to_segment()); }


bool rat_line::contains(const rat_point& p) const
{ return orientation(p) == 0; }

bool rat_line::contains(const rat_segment& s) const
{ return contains(s.start()) && contains(s.end()); }


ostream& operator<<(ostream& out, const rat_line& l) 
{ return out << l.seg(); }

istream& operator>>(istream& in, rat_line& l)  
{ rat_segment s; 
  in >> s; 
  l = rat_line(s); 
  return in; 
 }


bool rat_line::intersection(const rat_line& s, rat_point& inter) const
{ return ptr()->seg.intersection_of_lines(s.ptr()->seg,inter) ; }



bool rat_line::intersection(const rat_segment& s, rat_point& inter) const
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


bool rat_line::intersection(const rat_segment& s) const
{
  int orient1 = orientation(s.start());
  int orient2 = orientation(s.end());
  
  if ((orient1 != orient2) || (orient1 == 0 && orient2 == 0)) return true;
  return false;
}


rat_segment rat_line::perpendicular(const rat_point& q) const
{ return ptr()->seg.perpendicular(q); }



rat_point rat_line::dual() const
{
 if (is_vertical())
   LEDA_EXCEPTION(1,"rat_line::dual(): line is vertical.");

 rat_point p1 = point1();
 rat_point p2 = point2();

 integer x1 = p1.X();
 integer y1 = p1.Y();
 integer w1 = p1.W();

 integer x2 = p2.X();
 integer y2 = p2.Y();
 integer w2 = p2.W();

 integer aa = y2*w1 - y1*w2;
 integer aw = x2*w1 - x1*w2;
 integer bb = y2*aw - aa*x2;

 return rat_point(-aa*w2,bb,aw*w2);
}



rational rat_line::sqr_dist(const rat_point& q) const
{ rat_segment s = ptr()->seg.perpendicular(q); 
  return s.sqr_length();
}



bool rat_line::clip(rat_point p, rat_point q, rat_segment& s) const
{ 
  const rat_line& l = *this;
  rat_point a = l.point1();
  rat_point b = l.point2();
  if ( compare(a,b) > 0 ) leda_swap(a,b);

  if ( compare(p,q) > 0 ) leda_swap(p,q);

  rat_point luc(p.xcoord(),q.ycoord());  // left upper corner
  rat_point rlc(q.xcoord(),p.ycoord());  // right lower corner
  
  rat_line left  (p, luc);
  rat_line right (q, rlc);
  rat_line bottom(p, rlc);
  rat_line top   (q, luc);

  rat_point p_left,p_right,p_bottom, p_top;

  l.intersection(left,p_left);
  l.intersection(right,p_right);
  l.intersection(bottom,p_bottom);
  l.intersection(top,p_top);


  if ( rat_point::cmp_y(a,b) == 0 )
  { // horizontal
    s = rat_segment(p_left,p_right);
    return a.orientation(b,p) != a.orientation(b,q);
  }
  if ( rat_point::cmp_x(a,b) == 0 )
  { // vertical 
    s = rat_segment(p_bottom,p_top); 
    return a.orientation(b,p) != a.orientation(b,q);
  }
  if ( rat_point::cmp_y(a,b) < 0 )
  { // positive slope
    rat_point c = compare(p_left,p_bottom) < 0 ? p_bottom : p_left;
    rat_point d = compare(p_right,p_top) < 0   ? p_right : p_top;
    s = rat_segment(c,d);
    return a.orientation(b,luc) != a.orientation(b,rlc);
  }
  // negative slope
  rat_point c = compare(p_left,p_top) < 0    ? p_top : p_left;
  rat_point d = compare(p_right,p_bottom) <0 ? p_right : p_bottom;
  s = rat_segment(c,d);
  return a.orientation(b,p) != a.orientation(b,q);
}
    



rat_line p_bisector(const rat_point& p, const rat_point& q)
{ rat_segment s(center(p,q),q);
  return s.rotate90();
}


LEDA_END_NAMESPACE 
