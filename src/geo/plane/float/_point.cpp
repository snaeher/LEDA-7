/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/segment.h>

//------------------------------------------------------------------------------
// points 
//
// by S. Naeher (1995)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

atomic_counter point_rep::id_counter(0);

point_rep::point_rep(double a, double b) 
{ x = a; 
  y = b; 
  id  = ++id_counter; 
}


point::point()                            { PTR = new point_rep(0,0); }
point::point(double x, double y)          { PTR = new point_rep(x,y); }
point::point(double x, double y, double w){ PTR = new point_rep(x/w,y/w); }
point::point(vector v)                    { PTR = new point_rep(v[0], v[1]); }


point::point(const point& p, int prec)
{ 
  double x = p.xcoord();
  double y = p.ycoord();
  if ( prec <= 0 )
  { PTR = new point_rep(x,y);
    return;
  }
  int exp; double fraction;
  fraction = frexp(x,&exp);
  double xc = truncate(fraction,exp + prec);

  fraction = frexp(y,&exp);
  double yc = truncate(fraction,exp + prec);

  PTR = new point_rep(xc,yc);
}



double point::angle(const point& q, const point& r) const
{
  double dx1 = q.ptr()->x - ptr()->x; 
  double dy1 = q.ptr()->y - ptr()->y; 
  double dx2 = r.ptr()->x - ptr()->x; 
  double dy2 = r.ptr()->y - ptr()->y; 

  if ((dx1 == 0 && dy1 == 0) || (dx2 == 0 && dy2 == 0)) 
     LEDA_EXCEPTION(1,"point::angle:  zero vector input.");
  
  double norm  = (dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2);

  double cosfi = (dx1*dx2+dy1*dy2) / sqrt(norm);

  if (cosfi >=  1.0 ) return 0;
  if (cosfi <= -1.0 ) return LEDA_PI;
  
  double fi = acos(cosfi);

  if (dx1*dy2 < dy1*dx2) fi = -fi;

  if (fi < 0) fi += 2*LEDA_PI;

  return fi;
}
  


// Rotations 

point point::rotate90(const point& p, int i) const
{ 
  int r = i % 4;

  if (r == 0) return *this;
  if (r < 0) r += 4; // on some platforms the result of modulo (i.e. %) may be negative

  double px = p.xcoord();
  double py = p.ycoord();
  double dx = xcoord() - px;
  double dy = ycoord() - py;

  point q;
  switch (r) {
  case 1:  q = point(px-dy,py+dx);
           break;
  case 2:  q = point(px-dx,py-dy);
           break;
  case 3:  q = point(px+dy,py-dx);
           break;
  }
  return q;
}

point point::rotate90(int i) const
{ return rotate90(point(0,0),i); }


point point::rotate(const point& origin, double fi) const
{ double cx = origin.xcoord();
  double cy = origin.ycoord();
  double sinfi = sin(fi);
  double cosfi = cos(fi);
  double dx = xcoord() - cx;
  double dy = ycoord() - cy;
  return point(cx+dx*cosfi-dy*sinfi,cy+dx*sinfi+dy*cosfi);
}


point point::rotate(double fi) const
{ double sinfi = sin(fi);
  double cosfi = cos(fi);
  double x = xcoord();
  double y = ycoord();
  return point(x*cosfi-y*sinfi,x*sinfi+y*cosfi);
}


point point::reflect(const point& p, const point& q) const
{ // reflect point across line through p and q

  double px = p.xcoord();
  double py = p.ycoord();

  double x1 = xcoord()   - px;
  double y1 = ycoord()   - py;
  double x2 = q.xcoord() - px;
  double y2 = q.ycoord() - py;

  double L = (x1*x1 + y1*y1) * (x2*x2 + y2*y2);

  double cosfi = (x1*x2 + y1*y2);
  double sinfi = (x1*y2 - x2*y1); 
  double cos2 = (cosfi*cosfi - sinfi*sinfi)/L;
  double sin2 = 2*cosfi*sinfi/L;

  return point(px + x1*cos2-y1*sin2, py + x1*sin2+y1*cos2);
}


point point::reflect(const point& q) const
{ // reflect point across point q
  return point(2*q.xcoord()-xcoord(), 2*q.ycoord()-ycoord());
 }
  


// Translations

point point::translate(double dx, double dy) const
{ return point(xcoord()+dx,ycoord()+dy); }


point point::translate_by_angle(double phi, double d) const
{ double dx = cos(phi) * d;
  double dy = sin(phi) * d;
  if (fabs(dx) < 1e-12) dx = 0; 
  if (fabs(dy) < 1e-12) dy = 0; 
  return point(xcoord()+dx,ycoord()+dy);
 }

point point::translate(const vector& v) const 
{ return point(xcoord()+v[0],ycoord()+v[1]); }


// Distances

double point::sqr_dist(const point& p)  const
{ double dx = p.ptr()->x - ptr()->x; 
  double dy = p.ptr()->y - ptr()->y;
  return dx*dx + dy*dy;
 }

int point::cmp_dist(const point& q, const point& r) const
{ 
  return compare(sqr_dist(q),sqr_dist(r));
}




double point::xdist(const point& q) const 
{ return fabs(xcoord() - q.xcoord()); }

double point::ydist(const point& q) const 
{ return fabs(ycoord() - q.ycoord()); }

double  point::distance(const point& q) const 
{ return sqrt(sqr_dist(q)); }


bool point::operator==(const point& p) const 
{ return (ptr()->x == p.ptr()->x) && (ptr()->y == p.ptr()->y); }

int side_of_halfspace(const point& a, 
                      const point& b, 
                      const point& c)
{ 
  double s = (b - a) * (c - a);
  if ( s != 0 ) return ( s > 0 ? 1 : -1 );
  return 0;
}

   

int side_of_circle(const point& a, const point& b, const point& c, 
                                                   const point& d)
{  // comments indicate bit lengths of values if coordinates have 
   // at most L bits. 
   double ax = a.xcoord();    // L bits
   double ay = a.ycoord();

   double bx = b.xcoord() - ax;  // L + 1 bits
   double by = b.ycoord() - ay;
   double bw = bx*bx + by*by;    // 2L + 3 bits

   double cx = c.xcoord() - ax;  // L + 1 bits
   double cy = c.ycoord() - ay;
   double cw = cx*cx + cy*cy;    // 2L + 3 bits

   double D1 = cy*bw - by*cw;  // 2L + 3 + L + 1 + 1 = 3L + 5 bits
   double D2 = bx*cw - cx*bw;  // 3L + 5 bits
   double D3 = by*cx - bx*cy;  // 2L + 3

   double dx = d.xcoord() - ax;  // L + 1 bits
   double dy = d.ycoord() - ay;

   double D  = D1*dx  + D2*dy + D3*(dx*dx + dy*dy);
                               // 3L + 5 + L + 1 + 2 = 4L + 8 bits

   if (D != 0) 
      return (D > 0) ? 1 : -1;
   else
      return 0;
}

static int sign(const int& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}

static int sign(const double& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}

int compare_by_angle(const point& p1, const point& p2, 
                     const point& p3, const point& p4)
{ 
  if (p1 == p2) return (p3 == p4) ? 0 : -1;

  if (p3 == p4) return 1;

  // both vectors are non-zero ...
  double x1 = p2.xcoord() - p1.xcoord();
  double y1 = p2.ycoord() - p1.ycoord();
  double x2 = p4.xcoord() - p3.xcoord();
  double y2 = p4.ycoord() - p3.ycoord();  
  
  int sy1 = sign(y1); 
  int sy2 = sign(y2);

  int upper1 = (sy1 != 0) ? sy1 : sign(x1);
  int upper2 = (sy2 != 0) ? sy2 : sign(x2);

  if (upper1 == upper2) 
    return sign(x2*y1 - x1*y2);
  else
    return sign(upper2 - upper1);
}


bool affinely_independent(const array<point>& A)
{ if (A.size() == 1) return true;
  if (A.size() == 2 && A[0] != A[1]) return true;
  if (A.size() == 3 && orientation(A[0],A[1],A[2]) != 0) return true;
  return false; 
}

bool contained_in_simplex(const array<point>& A, const point& p)
{ if (A.size() == 1 && A[0] == p) return true;
  if (A.size() == 2) {
    point a = A[0];
    point b = A[1];
    if (a == b) LEDA_EXCEPTION(1,"cis: equal points");
    if (compare(a,b) > 0) leda_swap(a,b);
    return compare(a,p) <= 0 && compare(p,b) <= 0;
  }
 if (A.size() == 3) {
    point a = A[0];
    point b = A[1];
    point c = A[2];
   int orient = orientation(a,b,c);
   if (orient == 0) 
      LEDA_EXCEPTION(1,"cis: equal points or no simplex");
   if (orient < 0) leda_swap(b,c);
   return (orientation(a,b,p)>=0 &&
           orientation(b,c,p)>=0 &&
           orientation(c,a,p)>=0);
 }
 return false;
}

bool contained_in_affine_hull(const array<point>& A, const point& p)
{ if (A.size() == 1 && A[0] == p) return true;
  if (A.size() == 2) {
    if (A[0]==A[1]) return (A[0] == p);
    return orientation(A[0],A[1],p) == 0;
  }
  if (A.size() == 3) {
    if (orientation(A[0],A[1],A[2]) != 0)
      return true;
    else
      return (orientation(A[0],A[1],p) == 0);
  }
  return false;
}





istream& operator>>(istream& in, point& p) 
{ // syntax: {(} x {,} y {)}

  double x,y; 
  char c;

  do in.get(c); while (in && isspace(c));

  if (!in) return in;

  if (c != '(') in.putback(c);

  in >> x;

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> y; 

  do in.get(c); while (c == ' ');
  if (c != ')') in.putback(c);

  p = point(x,y); 
  return in; 

 } 


int point::cmp_xy(const point& a, const point& b)
{ int r = compare(a.xcoord(),b.xcoord());
  return (r!=0) ? r : compare(a.ycoord(),b.ycoord());
 }

int point::cmp_x(const point& a, const point& b)
{ return compare(a.xcoord(),b.xcoord()); }



int point::cmp_yx(const point& a, const point& b)
{ int r = compare(a.ycoord(),b.ycoord());
  return (r!=0) ? r : compare(a.xcoord(),b.xcoord());
 }

int point::cmp_y(const point& a, const point& b)
{ return compare(a.ycoord(),b.ycoord()); }


LEDA_END_NAMESPACE 

