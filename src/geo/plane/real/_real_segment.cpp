/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_segment.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/real_segment.h>
#include <LEDA/geo/rat_segment.h>
#include <ctype.h>


//------------------------------------------------------------------------------
// real segments 
//
// by S. Thiel (2003)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

atomic_counter real_segment_rep::id_counter(0);

real_segment_rep::real_segment_rep(const real_point& p, const real_point& q) : start(p), end(q)
{ dx  = q.xcoord() - p.xcoord(); 
  dy  = q.ycoord() - p.ycoord(); 
  id  = id_counter++; 
}


real_segment::real_segment()
{ PTR = new real_segment_rep(real_point(0,0), real_point(0,0)); }

real_segment::real_segment(const real_point& x, const real_point& y) 
{ PTR = new real_segment_rep(x,y); }

real_segment::real_segment(const real_point& x, const real_vector& v) 
{ PTR = new real_segment_rep(x,x+v); }

real_segment::real_segment(real x1, real y1, real x2, real y2) 
{ PTR = new real_segment_rep(real_point(x1,y1), real_point(x2,y2)); }

real_segment::real_segment(const rat_segment& s)
{ PTR = new real_segment_rep(real_point(s.source()), real_point(s.target())); }

/*
real_segment::real_segment(const real_point& p, double alpha, double length)
{ real_point q = p.translate_by_angle(alpha,length);
  PTR = new real_segment_rep(p,q); 
 }
*/

/*
real_segment real_segment::translate_by_angle(double alpha, double d) const
{ real_point p = ptr()->start.translate_by_angle(alpha,d);
  real_point q = ptr()->end.translate_by_angle(alpha,d);
  return real_segment(p,q);
 }
*/

real_segment real_segment::translate(real dx, real dy) const
{ real_point p = ptr()->start.translate(dx,dy);
  real_point q = ptr()->end.translate(dx,dy);
  return real_segment(p,q);
 }


real_segment real_segment::translate(const real_vector& v) const
{ real_point p = ptr()->start.translate(v);
  real_point q = ptr()->end.translate(v);
  return real_segment(p,q);
 }

/*
real_segment real_segment::rotate(double alpha) const
{ real_point p = start();
  real_point q = end();
  return real_segment(p,q.rotate(p,alpha));
}

real_segment real_segment::rotate(const real_point& origin, double alpha) const
{  real_point p = start().rotate(origin,alpha);
   real_point q = end().rotate(origin,alpha);
   return real_segment(p,q);
}
*/

real_segment real_segment::rotate90(const real_point& q, int i) const
{  return real_segment(start().rotate90(q,i),end().rotate90(q,i)); }

real_segment real_segment::rotate90(int i) const
{ return rotate90(start(),i); }


real_segment real_segment::reflect(const real_point& p, const real_point& q) const
{ return real_segment(start().reflect(p,q),end().reflect(p,q)); }

real_segment real_segment::reflect(const real_point& p) const
{ return real_segment(start().reflect(p),end().reflect(p)); }



real real_segment::slope() const 
{ real dx = ptr()->dx;
  real dy = ptr()->dy;
#ifndef LEDA_CHECKING_OFF
  if (dx == 0) 
	  LEDA_EXCEPTION(1, "real_segment::slope(): vertical segment");
#endif
  return dy/dx; 
}



real real_segment::sqr_length() const
{ real dx = ptr()->dx;
  real dy = ptr()->dy;
  return (dx*dx+dy*dy);
 }

real real_segment::length() const { return sqrt(sqr_length()); }

/*
double real_segment::angle() const
{ double dx = ptr()->dx;
  double dy = ptr()->dy;
  if (dx == 0 && dy == 0)  return 0;
  double a = atan2(dy,dx); 
  if (a < 0) a += 2*LEDA_PI;
  return a;
}


double real_segment::angle(const real_segment& s) const
{ double dx1 =   ptr()->dx;
  double dy1 =   ptr()->dy;
  double dx2 = s.ptr()->dx;
  double dy2 = s.ptr()->dy;
  
  double norm  = (dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2);

  if (norm == 0) return 0;

  double cosfi = (dx1*dx2+dy1*dy2) / sqrt(norm);

  if (cosfi >=  1.0 ) return 0;
  if (cosfi <= -1.0 ) return LEDA_PI;
  
  double fi=acos(cosfi);

  if (dx1*dy2 < dy1*dx2) fi = -fi;

  if (fi < 0) fi += 2*LEDA_PI;

  return fi;
}
*/

real  real_segment::y_proj(real x)  const
{ return  ycoord1() - slope() * (xcoord1() - x); }

real  real_segment::x_proj(real y)  const
{ if (is_vertical())  
       return  xcoord1();
  else
       return  xcoord1() - (ycoord1() - y)/slope(); 
}

real real_segment::y_abs() const 
{/* if (ptr()->dx == 0) 
     return -MAXDOUBLE;
  else*/
     return ycoord1() - slope()*xcoord1();
 }


bool real_segment::contains(const real_point& p) const
{ real_point a = source();
  real_point b = target();
  if (a == b) return a == p;
  return a.orientation(b,p) == 0 && (b-a)*(p-a) >=0 && (a-b)*(p-b) >= 0;
 }
 
 


bool real_segment::intersection(const real_segment& t) const
{ // decides whether this and |t| intersect 

  if (   is_trivial() ) return t.contains(  source());
  if ( t.is_trivial() ) return   contains(t.source());

  int o1 = orientation(t.start());
  int o2 = orientation(t.end());
  int o3 = t.orientation(start());
  int o4 = t.orientation(end());

  if ( o1 == 0 && o2 == 0 )
     return ( t.contains(  source()) || t.contains(  target()) ||
                contains(t.source()) ||   contains(t.target())    );

  return o1 != o2 && o3 != o4;
}
 
  
bool real_segment::intersection(const real_segment& s, real_point& inter) const
{ 
  if (!intersection(s)) return false;

  real_point a = source();
  real_point b = target();
  real_point c = s.source();
  real_point d = s.target();

  if (is_trivial())
  { bool C = s.contains(a);
    if (C) inter = a;
    return C;
   }

  if ( s.is_trivial() )
  { bool C = contains(c);
    if (C) inter = c;
    return C;
   }


  if (start() == s.start() || start() == s.end())
  { inter = start();
    return true;
   }

  if (end() == s.start() || end() == s.end())
  { inter = end();
    return true;
   }


  real cx,cy;

  if (is_vertical())
     cx = xcoord1();
  else
     if (s.is_vertical())
        cx = s.xcoord1();
     else
        cx = (s.y_abs()-y_abs())/(slope()-s.slope());

  if (is_vertical())
     cy = s.slope() * cx + s.y_abs();
  else
     cy = slope() * cx + y_abs();

  inter = real_point(cx,cy);

  return true;
}


bool real_segment::intersection_of_lines(const real_segment& s, real_point& inter) const
{ 
  if (dx() * s.dy() == s.dx() * dy()) return false; 
    // supporting lines are parallel

  if (start() == s.start() || start() == s.end())
  { inter = start(); return true; }

  if (end() == s.start() || end() == s.end())
  { inter = end(); return true; }

  if (is_vertical()) {
	  // => s is not vertical!
	  real cx = xcoord1();
	  inter = real_point(cx, s.y_proj(cx));
	  return true;
  }
  else if (s.is_vertical()) {
	  // => this is not vertical!
	  real cx = s.xcoord1();
	  inter = real_point(cx, y_proj(cx));
	  return true;
  }

  real sl1 = dy() / dx(), sl2 = s.dy() / s.dx();
  real y_abs1 = ycoord1() - xcoord1() * sl1, y_abs2 = s.ycoord1() - s.xcoord1() * sl2;

  real cx = (y_abs2 - y_abs1) / (sl1 - sl2);
  real cy = sl1 * cx + y_abs1;

  inter = real_point(cx,cy);

  return true;
}


real_segment real_segment::perpendicular(const real_point& q) const
{ real_point p = source();
  real_point r;
  intersection_of_lines(translate(q-p).rotate90(),r);
  return real_segment(q,r);
 }


real real_segment::distance(const real_point& p) const
{ real_point a = source();
  real_point b = target();
  if (a.orientation(b.rotate90(a),p) > 0) return a.distance(p);
  if (b.orientation(a.rotate90(b),p) > 0) return b.distance(p);
  real d = dx()*(a.ycoord()-p.ycoord()) - dy()*(a.xcoord()-p.xcoord());
  d /= length();
  return  (d < 0) ? -d : d;
}

real real_segment::sqr_dist(const real_point& p) const
{ real_point a = source();
  real_point b = target();
  if (a.orientation(b.rotate90(a),p) > 0) return a.sqr_dist(p);
  if (b.orientation(a.rotate90(b),p) > 0) return b.sqr_dist(p);
  real d = dx()*(a.ycoord()-p.ycoord()) - dy()*(a.xcoord()-p.xcoord());
  return (d*d)/sqr_length();
}



istream& operator>>(istream& in, real_segment& s) 
{ // syntax: {[} p {===} q {]}

  real_point p,q; 
  char c;

  do in.get(c); while (isspace(c));
  if (c != '[') in.putback(c);

  in >> p;

  do in.get(c); while (isspace(c));
  while (c== '=') in.get(c);
  while (isspace(c)) in.get(c);
  in.putback(c);

  in >> q; 

  do in.get(c); while (c == ' ');
  if (c != ']') in.putback(c);

  s = real_segment(p,q); 
  return in; 

 } 


 
int cmp_segments_at_xcoord(const real_segment& s1, const real_segment& s2, const real_point& p)
{
  real s1yd = s2.dx()*(s1.dy()*(p.xcoord() - s1.xcoord1()) + 
			 s1.dx()*s1.ycoord1());

  real s2yd = s1.dx()*(s2.dy()*(p.xcoord() - s2.xcoord1()) + 
			 s2.dx()*s2.ycoord1());

  if( s1yd > s2yd ) return +1;
  if( s1yd < s2yd ) return -1;
  return 0;
}


LEDA_END_NAMESPACE 
