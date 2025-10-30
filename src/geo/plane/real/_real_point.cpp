/*******************************************************************************
+
+  LEDA 7.2.2
+
+
+  real_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
*******************************************************************************/

#include <LEDA/geo/real_point.h>
#include <LEDA/geo/rat_point.h>
#include <ctype.h>

//------------------------------------------------------------------------------
// real_points
//
// by C. Uhrig (2003)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter real_point_rep::id_counter(0);

real_point_rep::real_point_rep(real a, real b)
{ x = a;
  y = b;
  id  = id_counter++;
}

real_point::real_point(const point& p, int prec)
{ 
  double x = p.xcoord();
  double y = p.ycoord();
  if ( prec <= 0 )
  { PTR = new real_point_rep(x,y);
    return;
  }
  int exp; double fraction;
  fraction = frexp(x,&exp);
  double xc = truncate(fraction,exp + prec);

  fraction = frexp(y,&exp);
  double yc = truncate(fraction,exp + prec);

  PTR = new real_point_rep(xc,yc);
}

real_point::real_point(const rat_point& p)
{
#if LEDA_REAL_VERSION >= 200
  PTR = new real_point_rep(real(p.xcoord()), real(p.ycoord()));
#else
  real X(p.X());
  real Y(p.Y());
  real W(p.W());

  PTR = new real_point_rep(X/W, Y/W);
#endif
}

point real_point::to_point() const
{
  return point(ptr()->x.to_close_double(), ptr()->y.to_close_double());
}

/*
double real_point::angle(const real_point& q, const real_point& r) const
{
  real dx1 = q.ptr()->x - ptr()->x;
  real dy1 = q.ptr()->y - ptr()->y;
  real dx2 = r.ptr()->x - ptr()->x;
  real dy2 = r.ptr()->y - ptr()->y;

  if ((dx1 == 0 && dy1 == 0) || (dx2 == 0 && dy2 == 0))
     LEDA_EXCEPTION(1,"real_point::angle:  zero vector input.");

  real norm  = (dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2);

  real cosfi = (dx1*dx2+dy1*dy2) / sqrt(norm);
  double cosfi_d = cosfi.to_close_double();

  if (cosfi_d >=  1.0 ) return 0;
  if (cosfi_d <= -1.0 ) return LEDA_PI;

  double fi = acos(cosfi_d);

  if (dx1*dy2 < dy1*dx2) fi = -fi;

  if (fi < 0) fi += 2*LEDA_PI;

  return fi;
}
*/


// Rotations

real_point real_point::rotate90(const real_point& p, int i) const
{
  int r = i % 4;

  if (r == 0) return *this;
  if (r < 0) r += 4; // on some platforms the result of modulo (i.e. %) may be negative

  real px = p.xcoord();
  real py = p.ycoord();
  real dx = xcoord() - px;
  real dy = ycoord() - py;

  real_point q;
  switch (r) {
  case 1:  q = real_point(px-dy,py+dx);
           break;
  case 2:  q = real_point(px-dx,py-dy);
           break;
  case 3:  q = real_point(px+dy,py-dx);
           break;
  }
  return q;
}

real_point real_point::rotate90(int i) const
{ return rotate90(real_point(0,0),i); }

/*
real_point real_point::rotate(const real_point& origin, double fi) const
{ real cx = origin.xcoord();
  real cy = origin.ycoord();
  real sinfi = sin(fi);
  real cosfi = cos(fi);
  real dx = xcoord() - cx;
  real dy = ycoord() - cy;
  return real_point(cx+dx*cosfi-dy*sinfi,cy+dx*sinfi+dy*cosfi);
}


real_point real_point::rotate(double fi) const
{ double sinfi = sin(fi);
  double cosfi = cos(fi);
  real x = xcoord();
  real y = ycoord();
  return real_point(x*cosfi-y*sinfi,x*sinfi+y*cosfi);
}
*/

real_point real_point::reflect(const real_point& p, const real_point& q) const
{ // reflect point across line through p and q

  real px = p.xcoord();
  real py = p.ycoord();

  real x1 = xcoord()   - px;
  real y1 = ycoord()   - py;
  real x2 = q.xcoord() - px;
  real y2 = q.ycoord() - py;

  real L = (x1*x1 + y1*y1) * (x2*x2 + y2*y2);

  real cosfi = (x1*x2 + y1*y2);
  real sinfi = (x1*y2 - x2*y1);
  real cos2 = (cosfi*cosfi - sinfi*sinfi)/L;
  real sin2 = 2*cosfi*sinfi/L;

  return real_point(px + x1*cos2-y1*sin2, py + x1*sin2+y1*cos2);
}


real_point real_point::reflect(const real_point& q) const
{ // reflect point across point q
  return real_point(2*q.xcoord()-xcoord(), 2*q.ycoord()-ycoord());
 }



// Translations

real_point real_point::translate(real dx, real dy) const
{ return real_point(xcoord()+dx,ycoord()+dy); }


/*
real_point real_point::translate_by_angle(double phi, double d) const
{ double dx = cos(phi) * d;
  double dy = sin(phi) * d;
  if (fabs(dx) < 1e-12) dx = 0;
  if (fabs(dy) < 1e-12) dy = 0;
  return real_point(xcoord()+dx,ycoord()+dy);
 }
*/

real_point real_point::translate(const real_vector& v) const
{ return real_point(xcoord()+v[0],ycoord()+v[1]); }


// Distances

real real_point::sqr_dist(const real_point& p)  const
{ real dx = p.ptr()->x - ptr()->x;
  real dy = p.ptr()->y - ptr()->y;
  return dx*dx + dy*dy;
 }

int real_point::cmp_dist(const real_point& q, const real_point& r) const
{
  return compare(sqr_dist(q),sqr_dist(r));
}




real real_point::xdist(const real_point& q) const
{ real x = xcoord() - q.xcoord();
  if (x >= 0) return x;
  else
  return -x;}

real real_point::ydist(const real_point& q) const
{ real y = ycoord() - q.ycoord();
  if (y >= 0) return y;
  else
  return -y;}

real  real_point::distance(const real_point& q) const
{ return sqrt(sqr_dist(q)); }


bool real_point::operator==(const real_point& p) const
{ return (ptr()->x == p.ptr()->x) && (ptr()->y == p.ptr()->y); }

int side_of_halfspace(const real_point& a,
                      const real_point& b,
                      const real_point& c)
{
  real s = (b - a) * (c - a);
  return s.sign();
}



int side_of_circle(const real_point& a, const real_point& b, const real_point& c,
                                                   const real_point& d)
{  // comments indicate bit lengths of values if coordinates have
   // at most L bits.
   real ax = a.xcoord();    // L bits
   real ay = a.ycoord();

   real bx = b.xcoord() - ax;  // L + 1 bits
   real by = b.ycoord() - ay;
   real bw = bx*bx + by*by;    // 2L + 3 bits

   real cx = c.xcoord() - ax;  // L + 1 bits
   real cy = c.ycoord() - ay;
   real cw = cx*cx + cy*cy;    // 2L + 3 bits

   real D1 = cy*bw - by*cw;  // 2L + 3 + L + 1 + 1 = 3L + 5 bits
   real D2 = bx*cw - cx*bw;  // 3L + 5 bits
   real D3 = by*cx - bx*cy;  // 2L + 3

   real dx = d.xcoord() - ax;  // L + 1 bits
   real dy = d.ycoord() - ay;

   real D  = D1*dx  + D2*dy + D3*(dx*dx + dy*dy);
                               // 3L + 5 + L + 1 + 2 = 4L + 8 bits

   return D.sign();
}

static int sign(const int& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}

int compare_by_angle(const real_point& p1, const real_point& p2,
                     const real_point& p3, const real_point& p4)
{
  if (p1 == p2) return (p3 == p4) ? 0 : -1;

  if (p3 == p4) return 1;

  // both vectors are non-zero ...
  real x1 = p2.xcoord() - p1.xcoord();
  real y1 = p2.ycoord() - p1.ycoord();
  real x2 = p4.xcoord() - p3.xcoord();
  real y2 = p4.ycoord() - p3.ycoord();

  int sy1 = sign(y1);
  int sy2 = sign(y2);

  int upper1 = (sy1 != 0) ? sy1 : sign(x1);
  int upper2 = (sy2 != 0) ? sy2 : sign(x2);

  if (upper1 == upper2)
    return sign(x2*y1 - x1*y2);
  else
    return sign(upper2 - upper1);
}


bool affinely_independent(const array<real_point>& A)
{ if (A.size() == 1) return true;
  if (A.size() == 2 && A[0] != A[1]) return true;
  if (A.size() == 3 && orientation(A[0],A[1],A[2]) != 0) return true;
  return false;
}

bool contained_in_simplex(const array<real_point>& A, const real_point& p)
{ if (A.size() == 1 && A[0] == p) return true;
  if (A.size() == 2) {
    real_point a = A[0];
    real_point b = A[1];
    if (a == b) LEDA_EXCEPTION(1,"cis: equal points");
    if (compare(a,b) > 0) leda_swap(a,b);
    return compare(a,p) <= 0 && compare(p,b) <= 0;
  }
 if (A.size() == 3) {
    real_point a = A[0];
    real_point b = A[1];
    real_point c = A[2];
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

bool contained_in_affine_hull(const array<real_point>& A, const real_point& p)
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





istream& operator>>(istream& in, real_point& p)
{ // syntax: {(} x {,} y {)}

  real x,y;
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

  p = real_point(x,y);
  return in;

 }

int real_point::cmp_xy(const real_point& a, const real_point& b)
{
#if !defined(__BORLANDC__)
  int r = compare(a.xcoord(),b.xcoord());
  return (r!=0) ? r : compare(a.ycoord(),b.ycoord());
#else
  // Borland does not destroy the local vars correctly, if we use the code above!
  if (identical(a,b)) return 0;
  int r = 0;
  {
    real ax = a.xcoord();
    real bx = b.xcoord();
    r = compare(ax, bx);
  }
  if (r == 0)
  {
    real ay = a.ycoord();
    real by = b.ycoord();
    r = compare(ay, by);
  }
  return r;
#endif
}

int real_point::cmp_yx(const real_point& a, const real_point& b)
{ int r = compare(a.ycoord(),b.ycoord());
  return (r!=0) ? r : compare(a.xcoord(),b.xcoord());
 }


LEDA_END_NAMESPACE
