/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _segment.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/line.h>


//------------------------------------------------------------------------------
// segments 
//
// by S. Naeher (1995)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

atomic_counter segment_rep::id_counter(0);

segment_rep::segment_rep(const point& p, const point& q) : start(p), end(q)
{ dx  = q.xcoord() - p.xcoord(); 
  dy  = q.ycoord() - p.ycoord(); 
  id  = ++id_counter; 
}


segment::segment()
{ PTR = new segment_rep(point(0,0), point(0,0)); }

segment::segment(const point& x, const point& y) 
{ PTR = new segment_rep(x,y); }

segment::segment(const point& x, const vector& v) 
{ PTR = new segment_rep(x,x+v); }

segment::segment(double x1, double y1, double x2, double y2) 
{ PTR = new segment_rep(point(x1,y1), point(x2,y2)); }

segment::segment(const point& p, double alpha, double length)
{ point q = p.translate_by_angle(alpha,length);
  PTR = new segment_rep(p,q); 
 }
  

segment segment::translate_by_angle(double alpha, double d) const
{ point p = ptr()->start.translate_by_angle(alpha,d);
  point q = ptr()->end.translate_by_angle(alpha,d);
  return segment(p,q);
 }


segment segment::translate(double dx, double dy) const
{ point p = ptr()->start.translate(dx,dy);
  point q = ptr()->end.translate(dx,dy);
  return segment(p,q);
 }


segment segment::translate(const vector& v) const
{ point p = ptr()->start.translate(v);
  point q = ptr()->end.translate(v);
  return segment(p,q);
 }


segment segment::rotate(double alpha) const
{ point p = start();
  point q = end();
  return segment(p,q.rotate(p,alpha));
}

segment segment::rotate(const point& origin, double alpha) const
{  point p = start().rotate(origin,alpha);
   point q = end().rotate(origin,alpha);
   return segment(p,q);
}


segment segment::rotate90(const point& q, int i) const
{  return segment(start().rotate90(q,i),end().rotate90(q,i)); }

segment segment::rotate90(int i) const
{ return rotate90(start(),i); }


segment segment::reflect(const point& p, const point& q) const
{ return segment(start().reflect(p,q),end().reflect(p,q)); }

segment segment::reflect(const point& p) const
{ return segment(start().reflect(p),end().reflect(p)); }



double segment::slope() const 
{ double dx = ptr()->dx;
  double dy = ptr()->dy;
  return (dx == 0) ? MAXDOUBLE : dy/dx; 
}



double segment::sqr_length() const
{ double dx = ptr()->dx;
  double dy = ptr()->dy;
  return (dx*dx+dy*dy);
 }

double segment::length() const { return sqrt(sqr_length()); }


double segment::angle() const
{ double dx = ptr()->dx;
  double dy = ptr()->dy;
  if (dx == 0 && dy == 0)  return 0;
  double a = atan2(dy,dx); 
  if (a < 0) a += 2*LEDA_PI;
  return a;
}


double segment::angle(const segment& s) const
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


double  segment::y_proj(double x)  const
{ return  ycoord1() - slope() * (xcoord1() - x); }

double  segment::x_proj(double y)  const
{ if (is_vertical())  
       return  xcoord1();
  else
       return  xcoord1() - (ycoord1() - y)/slope(); 
}

double segment::y_abs() const 
{ if (ptr()->dx == 0) 
     return -MAXDOUBLE;
  else
     return ycoord1() - slope()*xcoord1();
 }


bool segment::contains(const point& p) const
{ point a = source();
  point b = target();
  if (a == b) return a == p;
  return a.orientation(b,p) == 0 && (b-a)*(p-a) >=0 && (a-b)*(p-b) >= 0;
 }
 
 


bool segment::intersection(const segment& t) const
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
 
  
bool segment::intersection(const segment& s, point& inter) const
{ 
  if (!intersection(s)) return false;

  // here we know that the segments intersect

  point a = source();
  point b = target();
  point c = s.source();
  point d = s.target();

  if (is_trivial())     { inter = a; return true; }
  if ( s.is_trivial() ) { inter = c; return true; }

  if (a == c || a == d) { inter = a; return true; }
  if (b == c || b == d) { inter = b; return true; }


  // new: sn 06/2009
  // use code similar as for rat_point 

  double w  =  dy()*s.dx()-s.dy()*dx();
  double c1 = b.xcoord()*a.ycoord() - a.xcoord()*b.ycoord();
  double c2 = d.xcoord()*c.ycoord() - c.xcoord()*d.ycoord();
  inter = point((c2*dx()-c1*s.dx())/w, (c2*dy()-c1*s.dy())/w);

  return true;

}


bool segment::intersection_of_lines(const segment& s, point& inter) const
{ 

  // new: sn 06/2009
  // use code similar as for rat_point 

  // 02/2010: check equality of endpoints for better robustness (sn)


  double w = dy()*s.dx() - dx()*s.dy();

  if (w == 0) //same slope
    return false;

  point a = source();
  point b = target();
  point c = s.source();
  point d = s.target();

  if (a == c || a == d) { inter = a; return true; }
  if (b == c || b == d) { inter = b; return true; }


  double c1 = xcoord2()*ycoord1() - xcoord1()*ycoord2();
  double c2 = s.xcoord2()*s.ycoord1() - s.xcoord1()*s.ycoord2();

  inter = point((c2*dx()-c1*s.dx())/w, (c2*dy()-c1*s.dy())/w);

  return true;

}


segment segment::perpendicular(const point& q) const
{ point p = source();
  point r;
  intersection_of_lines(translate(q-p).rotate90(),r);
  return segment(q,r);
 }


double segment::distance(const point& p) const
{ point a = source();
  point b = target();
  if (a.orientation(b.rotate90(a),p) > 0) return a.distance(p);
  if (b.orientation(a.rotate90(b),p) > 0) return b.distance(p);
  double d = dx()*(a.ycoord()-p.ycoord()) - dy()*(a.xcoord()-p.xcoord());
  d /= length();
  return  (d < 0) ? -d : d;
}

double segment::sqr_dist(const point& p) const
{ point a = source();
  point b = target();
  if (a.orientation(b.rotate90(a),p) > 0) return a.sqr_dist(p);
  if (b.orientation(a.rotate90(b),p) > 0) return b.sqr_dist(p);
  double d = dx()*(a.ycoord()-p.ycoord()) - dy()*(a.xcoord()-p.xcoord());
  return (d*d)/sqr_length();
}



istream& operator>>(istream& in, segment& s) 
{ // syntax: {[} p {===} q {]}

  point p,q; 
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

  s = segment(p,q); 
  return in; 

 } 


 
int cmp_segments_at_xcoord(const segment& s1, const segment& s2, const point& p)
{
  double s1yd = s2.dx()*(s1.dy()*(p.xcoord() - s1.xcoord1()) + 
			 s1.dx()*s1.ycoord1());

  double s2yd = s1.dx()*(s2.dy()*(p.xcoord() - s2.xcoord1()) + 
			 s2.dx()*s2.ycoord1());

  if( s1yd > s2yd ) return +1;
  if( s1yd < s2yd ) return -1;
  return 0;
}


LEDA_END_NAMESPACE 
