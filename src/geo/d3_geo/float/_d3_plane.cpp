/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_plane.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_plane.h>
#include <LEDA/system/assert.h>

//------------------------------------------------------------------------------
// d3_plane 
//
// M. Baesken & S. Naeher
//
// last modified May 1999
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE



atomic_counter d3_plane_rep::id_counter(0); 


d3_plane_rep::d3_plane_rep(const d3_point& p,
                           const d3_point& q, const d3_point& r, 
                           const double& x, const double& y, const double& z)
: a(p), b(q), c(r), nx(x), ny(y), nz(z), nd(-x*p.X()-y*p.Y()-z*p.Z())

{ id  = ++id_counter; }

d3_plane::d3_plane() 
{ PTR = new d3_plane_rep(d3_point(0,0,0), d3_point(0,1,0), d3_point(1,0,0),
                         0,0,1); 
 }

d3_plane::d3_plane(const d3_point& a, const d3_point& b, const d3_point& c)
{ double X1 = b.X() - a.X();
  double Y1 = b.Y() - a.Y();
  double Z1 = b.Z() - a.Z();
  double X2 = c.X() - a.X();
  double Y2 = c.Y() - a.Y();
  double Z2 = c.Z() - a.Z();
  double nx = Z1*Y2 - Y1*Z2;
  double ny = X1*Z2 - Z1*X2;
  double nz = Y1*X2 - X1*Y2;
  
  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_plane::d3_plane(): cannot construct plane.");
  PTR = new d3_plane_rep(a,b,c,nx,ny,nz); 
}


d3_plane::d3_plane(const d3_point& p, const vector& n) 
{ 
  double nx = n[0];
  double ny = n[1];
  double nz = n[2];

  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_plane::d3_plane(): cannot construct plane.");

  vector v1;
  if (nx == 0) v1 = vector(1,0,0);
  else if (ny == 0) v1 = vector(0,1,0);
  else if (nz == 0) v1 = vector(0,0,1);
  else v1 = vector(-ny,nx,0);

  vector v2 = vector(ny*v1[2]-nz*v1[1],
                     nz*v1[0]-nx*v1[2],
                     nx*v1[1]-ny*v1[0]);

  PTR = new d3_plane_rep(p,p+v1,p+v2,nx,ny,nz); 
}

d3_plane::d3_plane(const d3_point& p, const d3_point& p2) 
{ 
  double nx = p2.xcoord() - p.xcoord();
  double ny = p2.ycoord() - p.ycoord();
  double nz = p2.zcoord() - p.zcoord();

  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_plane::d3_plane(): cannot construct plane.");

  vector v1;
  if (nx == 0) v1 = vector(1,0,0);
  else if (ny == 0) v1 = vector(0,1,0);
  else if (nz == 0) v1 = vector(0,0,1);
  else v1 = vector(-ny,nx,0);

  vector v2 = vector(ny*v1[2]-nz*v1[1],
                     nz*v1[0]-nx*v1[2],
                     nx*v1[1]-ny*v1[0]);

  PTR = new d3_plane_rep(p,p+v1,p+v2,nx,ny,nz); 
}


int d3_plane::intersection(const d3_point p1, const d3_point p2, 
                                                      d3_point& q) const
{ 
 // returns
 // 0: line through p1,p2 parallel
 // 1: line through p1,p2 intersects plane in a single point q
 // 2: line is in the plane

  d3_point pp1 = p1;
  d3_point pp2 = p2;

  vector vec1 = normal_project(pp1);
  vector vec2 = normal_project(pp2);

  if (vec1 == vec2) 
  { if (vec1 == vector(0,0,0))
       return 2;
    else
       return 0;
   }

  double d1 = vec1.sqr_length();
  double d2 = vec2.sqr_length();


  if (d1 == 0)
  { q = pp1;
    return 1;
   }

  if (d2 == 0)
  { q = pp2;
    return 1;
   }

  if (d1 < d2)
  { leda_swap(pp1,pp2);
    leda_swap(vec1,vec2);
   }

  vec1 = vec1 - vec2;

  int i = 0;
  while (i < 3 && vec1[i] == 0) i++;

  assert(i < 3);

  double f1 = vec1[i];
  double f2 = vec2[i];

  vector trans = pp2.to_vector() - pp1.to_vector();

  q = pp2 + (f2*trans)/f1;

  return 1; 
}

 

int d3_plane::intersection(const d3_plane& Q, d3_point& i1, d3_point& i2) const
{ 
  // returns:
  // 0: planes pl1 and pl2 do not intersect in a line (are parallel)
  // 1: intersect in the line through i1 and i2
  // 2: are equal

  d3_point a = point1();
  d3_point b = point2();
  d3_point c = point3();

  int w1 = Q.intersection(a,b,i1);
  int w2 = Q.intersection(a,c,i2);

  if (w1 == 0 && w2 == 0) return 0; // parallel

  if (w1 == 2 || w2 == 2)
  { i1 = a;
    if (w1 == 2 && w2 == 2) return 2; // same plane
    if (w1 == 2) i2 = b;
    if (w2 == 2) i2 = c;
  }

  if (w1 == 0 && w2 == 1) i1 = b + i2.to_vector() - a.to_vector();
  if (w2 == 0 && w1 == 1) i2 = c + i1.to_vector() - a.to_vector();

  if (w1 == 1 && w2 == 1) Q.intersection(b,c,i2);

  return 1;
}




d3_plane d3_plane::translate(double dx, double dy, double dz) const
{ d3_point p = point1().translate(dx,dy,dz);
  return d3_plane(p,normal());
}


d3_plane d3_plane::translate(const vector& v) const 
{ d3_point p = point1().translate(v);
  return d3_plane(p,normal());
}


// Distances

d3_plane d3_plane::reflect(const d3_point& q) const
{ // reflect plane across point q
  d3_point a = point1();
  d3_point b = a.translate(normal());
  d3_point a1 = a.reflect(q);
  d3_point b1 = b.reflect(q);
  return d3_plane(a1,b1-a1);
 }


vector d3_plane::normal_project(const d3_point& p) const
{ vector v = p - point1();
  double nx = ptr()->nx;
  double ny = ptr()->ny;
  double nz = ptr()->nz;

  double W = nx*nx+ny*ny+nz*nz;
  double A = -(nx*v.hcoord(0)+ny*v.hcoord(1)+nz*v.hcoord(2))/W;
  return vector(A*nx,A*ny,A*nz);
}

d3_point d3_plane::reflect_point(const d3_point& p) const
{ return p.translate(2*normal_project(p)); } 


d3_plane  d3_plane::reflect(const d3_plane& Q) const
{ // reflect plane across plane q
  d3_point a = point1();
  d3_point b = a.translate(normal());
  d3_point a1 = Q.reflect_point(a);
  d3_point b1 = Q.reflect_point(b);
  return d3_plane(a1,b1-a1);
 }

/*
double d3_plane::sqr_dist(const d3_point& p)  const
{ vector vec = normal_project(p);
  return vec.sqr_length();
 }
*/

// ---------------------------------------------------------- 
// new implementation of squared distance ...
 
double d3_plane::sqr_dist(const d3_point& p)  const
{ 
  double A = ptr()->nx;
  double B = ptr()->ny;
  double C = ptr()->nz;
  double D = ptr()->nd;
  
  double sq_nom = A*p.X() + B*p.Y() + C*p.Z() + D;
  sq_nom = sq_nom * sq_nom;
  
  double sq_denom = A*A + B*B + C*C;
  
  return sq_nom/sq_denom;
} 

int d3_plane::cmp_distances(const d3_point& p1, const d3_point& p2) const
{
  double A = ptr()->nx;
  double B = ptr()->ny;
  double C = ptr()->nz;
  double D = ptr()->nd;
  
  // compute squared nominators
  double sq_nom_p1 = A*p1.X() + B*p1.Y() + C*p1.Z() + D;
  sq_nom_p1 = sq_nom_p1 * sq_nom_p1;
  
  double sq_nom_p2 = A*p2.X() + B*p2.Y() + C*p2.Z() + D;
  sq_nom_p2 = sq_nom_p2 * sq_nom_p2;
  
  if (sq_nom_p1 > sq_nom_p2)  return 1;
  if (sq_nom_p1 == sq_nom_p2) return 0;
  return -1;
}

// ----------------------------------------------------------

double d3_plane::distance(const d3_point& p) const 
{ return sqrt(sqr_dist(p)); }


bool d3_plane::parallel(const d3_plane& Q) const
{ double nx1 = ptr()->nx;
  double ny1 = ptr()->ny;
  double nz1 = ptr()->nz;
  double nx2 = Q.ptr()->nx;
  double ny2 = Q.ptr()->ny;
  double nz2 = Q.ptr()->nz;
  return nz1*ny2 == ny1*nz2 && nx1*nz2 == nz1*nx2 && ny1*nx2 == nx1*ny2;
}

/*
int d3_plane::side_of(const d3_point& q) const
{ 
  d3_point a = point1();

  double qx = q.X() - a.X();
  double qy = q.Y() - a.Y();
  double qz = q.Z() - a.Z();

  double nx = ptr()->nx;
  double ny = ptr()->ny;
  double nz = ptr()->nz;

  double D = nx*qx + ny*qy + nz*qz;

  if (D > 0) return +1;
  if (D < 0) return -1;
  return 0;
}
*/
// -----------------------------------------------
// other implementation ...

int d3_plane::side_of(const d3_point& q) const
{ 
  double nx = ptr()->nx;
  double ny = ptr()->ny;
  double nz = ptr()->nz;
  double nd = ptr()->nd;

  double val = nx*q.X() + ny*q.Y() + nz*q.Z() + nd;

  if (val > 0) return -1;
  if (val < 0) return +1;
  return 0;
}

// -----------------------------------------------


d3_plane d3_plane::to_d3_plane() const
{ return d3_plane(point1(),normal()); }


bool d3_plane::operator==(const d3_plane& P) const 
{ return parallel(P) && contains(P.point1()); }

   
ostream& operator<<(ostream& out, const d3_plane& P)
{ cout << P.point1() << " " << P.normal(); return out; } 

istream& operator>>(istream& in, d3_plane& P) 
{ d3_point p;
  vector n(3);
  cin >> p >> n;
  P = d3_plane(p,n);
  return in; 
} 


LEDA_END_NAMESPACE
