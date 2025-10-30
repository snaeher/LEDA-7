/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_point.h>

//------------------------------------------------------------------------------
// d3_points 
//
// last modified 02/11 (rotate)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE


atomic_counter d3_point_rep::id_counter(0); 

d3_point_rep::d3_point_rep(double a, double b, double c) 
{ x = a; 
  y = b; 
  z = c; 
  id  = ++id_counter; 
}


d3_point::d3_point() { PTR = new d3_point_rep; }

d3_point::d3_point(double x, double y, double z) 
{ PTR = new d3_point_rep(x,y,z); }

d3_point::d3_point(double x, double y, double z,double w)
{ PTR = new d3_point_rep(x/w,y/w,z/w); }

d3_point::d3_point(vector v) 
{ PTR = new d3_point_rep(v[0], v[1], v[2]); }

// Projections

point d3_point::project_xy() const { return point(xcoord(),ycoord()); }
point d3_point::project_yz() const { return point(ycoord(),zcoord()); }
point d3_point::project_xz() const { return point(xcoord(),zcoord()); }


// Translations

d3_point d3_point::translate(double dx, double dy, double dz) const
{ return d3_point(xcoord()+dx,ycoord()+dy,zcoord()+dz); }


d3_point d3_point::translate(const vector& v) const 
{ return translate(v[0],v[1],v[2]); }


// Distances

double d3_point::sqr_dist(const d3_point& p)  const
{ double dx = p.ptr()->x - ptr()->x; 
  double dy = p.ptr()->y - ptr()->y;
  double dz = p.ptr()->z - ptr()->z;
  return dx*dx + dy*dy + dz*dz;
 }

double d3_point::xdist(const d3_point& q) const 
{ return fabs(xcoord() - q.xcoord()); }

double d3_point::ydist(const d3_point& q) const 
{ return fabs(ycoord() - q.ycoord()); }

double d3_point::zdist(const d3_point& q) const 
{ return fabs(zcoord() - q.zcoord()); }


d3_point center(const d3_point& a, const d3_point& b)
{ double x = (a.xcoord()+b.xcoord())/2;
  double y = (a.ycoord()+b.ycoord())/2;
  double z = (a.zcoord()+b.zcoord())/2;
  return d3_point(x,y,z);
 } 


d3_point d3_point::reflect(const d3_point& q) const
{ // reflect point across point q
  return d3_point(2*q.xcoord()-xcoord(), 2*q.ycoord()-ycoord(),
                                         2*q.zcoord()-zcoord());
 }


d3_point  d3_point::reflect(const d3_point& a, const d3_point& b,
                                               const d3_point& c) const
{  
  // reflect point across plane through a, b, and c

  double x1 = b.xcoord() - a.xcoord();
  double y1 = b.ycoord() - a.ycoord();
  double z1 = b.zcoord() - a.zcoord();

  double x2 = c.xcoord() - a.xcoord();
  double y2 = c.ycoord() - a.ycoord();
  double z2 = c.zcoord() - a.zcoord();

  double x3 = xcoord() - a.xcoord();
  double y3 = ycoord() - a.ycoord();
  double z3 = zcoord() - a.zcoord();

  double x = (z1*y2-y1*z2);
  double y = (x1*z2-z1*x2);
  double z = (y1*x2-x1*y2);

  if (x == 0 && y == 0 && z == 0)
      LEDA_EXCEPTION(1,"d3_point::reflect(a,b,c): a,b,c are coplanar");


  double f = -2*(x*x3+y*y3+z*z3)/(x*x+y*y+z*z);

  return translate(f*x,f*y,f*z);
}


d3_point d3_point::rotate_around_axis(int axis, double phi) const
{
  // axis: 0 --> x,  1 --> y, 2 --> z

 if (axis < 0 || axis > 2)
 { LEDA_EXCEPTION(1,"d3_point::rotate: illegal axis.");
   return *this;
  }

  double x = xcoord();
  double y = ycoord();
  double z = zcoord();

  double sin_phi = sin(phi);
  double cos_phi = cos(phi);

  double x1=0, y1=0, z1=0;

  if (axis == 0)
  { x1 = x;
    y1 = cos_phi * y - sin_phi * z;
    z1 = sin_phi * y + cos_phi * z;
   }

  if (axis == 1)
  { x1 =  cos_phi * x - sin_phi * z;
    y1 =  y;
    z1 =  sin_phi * x + cos_phi * z;
   }

  if (axis == 2)
  { x1 = cos_phi * x - sin_phi * y;
    y1 = sin_phi * x + cos_phi * y;
    z1 = z;
   }

  return d3_point(x1,y1,z1);
}


d3_point d3_point::rotate_around_vector(const vector& u, double phi) const
{
  double x = xcoord();
  double y = ycoord();
  double z = zcoord();

  double ux  = u[0];
  double uy  = u[1];
  double uz  = u[2];

  double ux2 = ux * ux;
  double uy2 = uy * uy;
  double uz2 = uz * uz;

  double uxy = ux * uy;
  double uyz = uy * uz;
  double uxz = ux * uz;

  double sin_phi = sin(phi);
  double cos_phi = cos(phi);

  double x1 = x * (cos_phi + ux2 * (1-cos_phi)) +
              y * (uxy*(1-cos_phi) - uz*sin_phi) +
              z * (uxz*(1-cos_phi) + uy*sin_phi);

  double y1 = x * (uxy*(1-cos_phi) + uz*sin_phi) +
              y * (cos_phi + uy2 * (1-cos_phi)) +
              z * (uyz*(1-cos_phi) - ux*sin_phi);


  double z1 = x * (uxz*(1-cos_phi) - uy*sin_phi) +
              y * (uyz*(1-cos_phi) + ux*sin_phi) +
              z * (cos_phi + uz2 * (1-cos_phi));
  
  return d3_point(x1,y1,z1);
}



d3_point d3_point::cartesian_to_polar() const
{
  double x =  xcoord();
  double y = -ycoord();
  double z =  zcoord();

  double r = hypot(x,y);

  double a = (fabs(y) < 1e-12)  ? LEDA_PI/2 : atan(x/y);
  double b = atan(z/r);

  if (y < 0)
  {
    if (a <= 0)
      a += LEDA_PI;
    else
      a -= LEDA_PI;
  }

  double lon = 180*a/LEDA_PI;
  double lat = 180*b/LEDA_PI;
  double radius = distance();

  return d3_point(lon,lat,radius);
}


d3_point d3_point::polar_to_cartesian() const
{ 
  double lon = xcoord();
  double lat = ycoord();
  double radius = zcoord();

  double a = LEDA_PI*lon/180;
  double b = LEDA_PI*lat/180;

  double d = cos(b) * radius;
  double x = sin(a) * d;
  double y = cos(a) * d;
  double z = sin(b) * radius;

  return d3_point(x,-y,z);
}


  


double  d3_point::distance(const d3_point& q) const 
{ return sqrt(sqr_dist(q)); }


bool d3_point::operator==(const d3_point& p) const 
{ return ptr()->x == p.ptr()->x && 
         ptr()->y == p.ptr()->y && 
         ptr()->z == p.ptr()->z; 
}

   

int side_of_sphere(const d3_point& a, const d3_point& b, const d3_point& c, 
                                                         const d3_point& d,
                                                         const d3_point& e)

{  double ax = a.xcoord();
   double ay = a.ycoord();
   double az = a.zcoord();

   double bx = b.xcoord() - ax;
   double by = b.ycoord() - ay;
   double bz = b.zcoord() - az;
   double bw = bx*bx + by*by + bz*bz;

   double cx = c.xcoord() - ax;
   double cy = c.ycoord() - ay;
   double cz = c.zcoord() - az;
   double cw = cx*cx + cy*cy + cz*cz;

   double dx = d.xcoord() - ax;
   double dy = d.ycoord() - ay;
   double dz = d.zcoord() - az;
   double dw = dx*dx + dy*dy + dz*dz;

   double ex = e.xcoord() - ax;
   double ey = e.ycoord() - ay;
   double ez = e.zcoord() - az;
   double ew = ex*ex + ey*ey + ez*ez;

   return  orientation(d3_point(bx/bw,by/bw,bz/bw),
                       d3_point(cx/cw,cy/cw,cz/cw),
                       d3_point(dx/dw,dy/dw,dz/dw),
                       d3_point(ex/ew,ey/ew,ez/ew));
 }


int region_of_sphere(const d3_point& a, const d3_point& b, const d3_point& c, 
                                                           const d3_point& d,
                                                           const d3_point& e)
{ int orient = orientation(a,b,c,d);
  if (orient == 0)
     LEDA_EXCEPTION(1,"region_of_sphere: degenerate sphere.");
  return orient * side_of_sphere(a,b,c,d,e); 
 }



bool contained_in_simplex(const d3_point& a, const d3_point& b,
                                             const d3_point& c,
                                             const d3_point& d,
                                             const d3_point& x)
{ int orient1 = orientation(a,b,c,d);
  if (orient1 == 0)
    LEDA_EXCEPTION(1,"contained_in_simplex: degenerate simplex.");
  if (orientation(a,b,c,x) == -orient1) return false;
  if (orientation(d,c,b,x) == -orient1) return false;
  if (orientation(d,b,a,x) == -orient1) return false;
  if (orientation(d,a,c,x) == -orient1) return false;
  return true;
}


bool contained_in_simplex(const array<d3_point>& A, const d3_point& x)
{ int l = A.low();
  int d = A.high() - l + 1;

  switch (d) {

  case 4: return contained_in_simplex(A[l],A[l+1],A[l+2],A[l+3],x); 


  case 3: { const d3_point& a = A[l];
            const d3_point& b = A[l+1];
            const d3_point& c = A[l+2];
            d3_point d = point_on_positive_side(a,b,c);
            if (orientation(a,b,c,x) != 0) return false;
            return contained_in_simplex(a,b,c,d,x);
           }


  case 2: { d3_point a = A[l];
            d3_point b = A[l+1];
            if ( !collinear(a,b,x) ) return false;
            if (compare(a,b) < 0)
              return compare(a,x) <= 0 && compare(x,b) <= 0;
            else
              return compare(b,x) <= 0 && compare(x,a) <= 0;
           }

  case 1:  return A[l] == x;

  default: LEDA_EXCEPTION(1,"contained_in_simplex: more than d+1 simplex points.");
           break;
  }

  return false;

}



static int compute_base(list<d3_point>& L, array<d3_point>& base)
{
  // precond: L is sorted

  d3_point A = L.pop();
  base[0] = A;
  while(!L.empty() && L.head() == A) L.pop();

  if (L.empty()) return 0;
      
  d3_point B = L.pop();
  base[1] = B;
  while(!L.empty() && collinear(A,B,L.head())) L.pop();

  if (L.empty()) return 1;

  d3_point C = L.pop();
  base[2] = C;
  while(!L.empty() && orientation(A,B,C,L.head()) == 0) L.pop();

  if (L.empty()) return 2;

  d3_point D = L.pop();
  base[3] = D;
  return 3;
}
  

int affine_rank(const list<d3_point>& L)
{ list<d3_point> L1 = L;
  L1.sort();
  array<d3_point> base(4);
  return compute_base(L1,base);
}

int affine_rank(const array<d3_point>& A)
{ list<d3_point> L;
  d3_point p;
  forall(p,A) L.append(p);
  L.sort();
  array<d3_point> base(4);
  return compute_base(L,base);
}


bool affinely_independent(const list<d3_point>& L)
{ return affine_rank(L) == L.size()-1; }

bool affinely_independent(const array<d3_point>& A)
{ return affine_rank(A) == A.size()-1; }



bool contained_in_affine_hull(const list<d3_point>& L, const d3_point& x)
{ list<d3_point> L1 = L;
  L1.sort();
  array<d3_point> base(4);
  int d = compute_base(L1,base);
  switch (d) {
    case 0: return base[0] == x;
    case 1: return collinear(base[0],base[1],x);
    case 2: return orientation(base[0],base[1],base[2],x) == 0;
    case 3: return true;
  }
  LEDA_EXCEPTION(1,"contained_in_affine_hull: internal error.");
  return false;
}

bool contained_in_affine_hull(const array<d3_point>& A, const d3_point& x)
{ list<d3_point> L;
  d3_point p;
  forall(p,A) L.append(p);
  return contained_in_affine_hull(L,x);
}




ostream& operator<<(ostream& out, const d3_point& p)
{ out << "(" << p.xcoord() << "," << p.ycoord() << "," << p.zcoord() << ")";
  return out;
 } 

istream& operator>>(istream& in, d3_point& p) 
{ // syntax: {(} x {,} y {,} z{)}

  double x,y,z; 
  char c;

  do in.get(c); while (in && isspace(c));

  if (!in) return in;

  if (c != '(') in.putback(c);

  in >> x;

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> y; 

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> z; 

  do in.get(c); while (c == ' ');
  if (c != ')') in.putback(c);

  p = d3_point(x,y,z); 
  return in; 

 } 


int d3_point::cmp_xyz(const d3_point& a, const d3_point& b)
{ int d = compare(a.xcoord(),b.xcoord());
  if (d) return d;
  d = compare(a.ycoord(),b.ycoord());
  if (d) return d;
  d = compare(a.zcoord(),b.zcoord());
  return d;
 }


int d3_point::cmp_x(const d3_point& a, const d3_point& b)
{ return compare(a.xcoord(),b.xcoord()); }

int d3_point::cmp_y(const d3_point& a, const d3_point& b)
{ return compare(a.ycoord(),b.ycoord()); }

int d3_point::cmp_z(const d3_point& a, const d3_point& b)
{ return compare(a.zcoord(),b.zcoord()); }



double volume(const d3_point& a, const d3_point& b, const d3_point& c,
                                                    const d3_point& d)
{ double AX = a.xcoord();
  double AY = a.ycoord();
  double AZ = a.zcoord();
  double BX = b.xcoord();
  double BY = b.ycoord();
  double BZ = b.zcoord();
  double CX = c.xcoord();
  double CY = c.ycoord();
  double CZ = c.zcoord();
  double DX = d.xcoord();
  double DY = d.ycoord();
  double DZ = d.zcoord();
   
  double D =   AZ*BY*CX - AY*BZ*CX - AZ*BX*CY + AX*BZ*CY 
             + AY*BX*CZ - AX*BY*CZ - AZ*BY*DX + AY*BZ*DX 
             + AZ*CY*DX - BZ*CY*DX - AY*CZ*DX + BY*CZ*DX 
             + AZ*BX*DY - AX*BZ*DY - AZ*CX*DY + BZ*CX*DY 
             + AX*CZ*DY - BX*CZ*DY - AY*BX*DZ + AX*BY*DZ 
             + AY*CX*DZ - BY*CX*DZ - AX*CY*DZ + BX*CY*DZ;

  return D/6;
}



int orientation(const d3_point& a, const d3_point& b, const d3_point& c,
                                                      const d3_point& d)
{ // sign of (d-z) * cross_prod(c-a,b-a)  

  double x1 = c.xcoord() - a.xcoord();
  double y1 = c.ycoord() - a.ycoord();
  double z1 = c.zcoord() - a.zcoord();

  double x2 = b.xcoord() - a.xcoord();
  double y2 = b.ycoord() - a.ycoord();
  double z2 = b.zcoord() - a.zcoord();

  double x3 = d.xcoord() - a.xcoord();
  double y3 = d.ycoord() - a.ycoord();
  double z3 = d.zcoord() - a.zcoord();
    
  double D = (z1*y2-y1*z2)*x3 + (x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3;

  if (D > 0) return +1;
  if (D < 0) return -1;
  return 0;
}



bool collinear(const d3_point& a, const d3_point& b, const d3_point& c)
{ // cross product(b-a, c-a) == 0
  double X1 = b.xcoord() - a.xcoord();
  double Y1 = b.ycoord() - a.ycoord();
  double Z1 = b.zcoord() - a.zcoord();
  double X2 = c.xcoord() - a.xcoord();
  double Y2 = c.ycoord() - a.ycoord();
  double Z2 = c.zcoord() - a.zcoord();

  return  Z1*Y2 == Y1*Z2 &&
          X1*Z2 == Z1*X2 &&
          Y1*X2 == X1*Y2;
 }




inline int d2_orientation(double ax,double ay, double bx,double by,
                                               double cx,double cy)
{ double E = (ax-bx) * (ay-cy) - (ay-by) * (ax-cx);
  if (E > 0) return  1;
  else if (E < 0) return -1;
  else return  0;
}
 

int orientation_xy(const d3_point& a, const d3_point& b, const d3_point& c)
{ return d2_orientation(a.xcoord(),a.ycoord(),b.xcoord(),b.ycoord(), 
                                              c.xcoord(),c.ycoord()); }

int orientation_yz(const d3_point& a, const d3_point& b, const d3_point& c)
{ return d2_orientation(a.ycoord(),a.zcoord(),b.ycoord(),b.zcoord(), 
                                              c.ycoord(),c.zcoord()); }

int orientation_xz(const d3_point& a, const d3_point& b, const d3_point& c)
{ return d2_orientation(a.xcoord(),a.zcoord(),b.xcoord(),b.zcoord(), 
                                              c.xcoord(),c.zcoord()); }



d3_point point_on_positive_side(const d3_point& a, const d3_point& b, 
                                                   const d3_point& c)
{ 
  double x1 = c.xcoord() - a.xcoord();
  double y1 = c.ycoord() - a.ycoord();
  double z1 = c.zcoord() - a.zcoord();

  double x2 = b.xcoord() - a.xcoord();
  double y2 = b.ycoord() - a.ycoord();
  double z2 = b.zcoord() - a.zcoord();

  double cx = z1*y2-y1*z2;
  double cy = x1*z2-z1*x2; 
  double cz = y1*x2-x1*y2;

  return a.translate(cx,cy,cz);
}




LEDA_END_NAMESPACE
