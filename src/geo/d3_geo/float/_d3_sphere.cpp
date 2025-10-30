/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_sphere.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_sphere.h>

//------------------------------------------------------------------------------
// d3_sphere 
//
// last modifid: 07/99
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE


static d3_point intersect(const vector& n1, const vector& n2, const vector& n3,
                          double D1, double D2, double D3);

atomic_counter d3_sphere_rep::id_counter(0);

d3_sphere_rep::d3_sphere_rep(const d3_point& p1, const d3_point& p2,
                             const d3_point& p3, const d3_point& p4) 
                             : a(p1),b(p2),c(p3),d(p4)
{ id=++id_counter; }


d3_sphere::d3_sphere()
{ d3_point p1(1,0,0), p2(0,0,0), p3(0,1,0), p4(0,0,1);
  PTR = new d3_sphere_rep(p1,p2,p3,p4);
 }


d3_sphere::d3_sphere(const d3_point& p1, const d3_point& p2,
                     const d3_point& p3, const d3_point& p4)
{ PTR = new d3_sphere_rep(p1,p2,p3,p4); }



bool d3_sphere::contains(const d3_point& p) const 
{ 
  return on_sphere(point1(),point2(),point3(),point4(),p); 
}


bool d3_sphere::inside(const d3_point& p) const 
{
  return inside_sphere(point1(),point2(),point3(),point4(),p);
}
 

bool d3_sphere::outside(const d3_point& p) const {
  return outside_sphere(point1(),point2(),point3(),point4(),p);
}


d3_point d3_sphere::center() const
{
  if (is_degenerate())
     LEDA_EXCEPTION(1,"d3_sphere::center: degenerate sphere.");

  d3_plane pl1(point1(),point2(),point4());
  d3_plane pl2(point1(),point3(),point4());
  d3_plane pl3(point1(),point2(),point3());

  d3_point m1 = midpoint(point1(),point2()); 
  d3_point m2 = midpoint(point1(),point3()); 
  d3_point m3 = midpoint(point1(),point4());

  vector r1 = pl1.normal(); 
  vector r2 = pl2.normal(); 
  vector r3 = pl3.normal();
 
  d3_plane sr1(m1,m1+r1,m1+r3);
  d3_plane sr2(m2,m2+r2,m2+r3);
  d3_plane sr3(m3,m3+r1,m3+r2);
  
  vector n1 = sr1.normal(); 
  vector n2 = sr2.normal(); 
  vector n3 = sr3.normal();

  double D1 = -(n1*m1.to_vector()); 
  double D2 = -(n2*m2.to_vector()); 
  double D3 = -(n3*m3.to_vector());

  return intersect(n1,n2,n3,D1,D2,D3);
}

double d3_sphere::sqr_radius() const
{
  return point1().sqr_dist(center());  
}

double d3_sphere::radius() const
{
  return point1().distance(center());
}

double d3_sphere::surface() const
{ 
  return 4*sqr_radius()*LEDA_PI;
}

double d3_sphere::volume() const
{ double sr = radius();
  return (4.0/3.0)*LEDA_PI*sr*sr*sr;
 }

d3_sphere d3_sphere::translate(const vector& v) const
{ d3_point a=point1();
  d3_point b=point2();
  d3_point c=point3();
  d3_point d=point4();
  return d3_sphere(a.translate(v),b.translate(v),c.translate(v),d.translate(v));
}
 
d3_sphere d3_sphere::translate(double wx,double wy,double wz) const
{ d3_point a=point1();
  d3_point b=point2();
  d3_point c=point3();
  d3_point d=point4();
  return d3_sphere(a.translate(wx,wy,wz), b.translate(wx,wy,wz),
                   c.translate(wx,wy,wz), d.translate(wx,wy,wz));
 }



static double det(double a11,double a12,double a13,
                  double a21,double a22,double a23,
                  double a31,double a32,double a33)
{
 return a11*a22*a33+a12*a23*a31+a13*a21*a32-a13*a22*a31-a11*a23*a32-a12*a21*a33;
}

static d3_point intersect(const vector& n1,const vector& n2,const vector& n3,
                          double D1, double D2, double D3)
{
  double A1=n1.xcoord(); double B1=n1.ycoord(); double C1=n1.zcoord();
  double A2=n2.xcoord(); double B2=n2.ycoord(); double C2=n2.zcoord();
  double A3=n3.xcoord(); double B3=n3.ycoord(); double C3=n3.zcoord();
 
  double dt   = det(A1,B1,C1,A2,B2,C2,A3,B3,C3);
  double xdet = det(D1,B1,C1,D2,B2,C2,D3,B3,C3);
  double ydet = det(A1,D1,C1,A2,D2,C2,A3,D3,C3);
  double zdet = det(A1,B1,D1,A2,B2,D2,A3,B3,D3);
 
  if (dt==0) LEDA_EXCEPTION(1,"d3_sphere: intersect: determinant = 0.");
 
  double xw=-xdet/dt; 
  double yw=-ydet/dt; 
  double zw=-zdet/dt;
 
  return d3_point(xw,yw,zw);
}


LEDA_END_NAMESPACE
