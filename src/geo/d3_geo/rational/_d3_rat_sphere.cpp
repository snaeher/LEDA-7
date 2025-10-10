/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_rat_sphere.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_rat_sphere.h>

//------------------------------------------------------------------------------
// d3_rat_sphere 
// 
// last modified:  07/99
//------------------------------------------------------------------------------


LEDA_BEGIN_NAMESPACE

static d3_rat_point intersect(const rat_vector& n1,const rat_vector& n2,const rat_vector& n3,
                              const rational& D1, const rational& D2, const rational& D3);



atomic_counter d3_rat_sphere_rep::id_counter(0);

d3_rat_sphere_rep::d3_rat_sphere_rep(const d3_rat_point& p1,
                                     const d3_rat_point& p2,
                                     const d3_rat_point& p3,
                                     const d3_rat_point& p4) 
                                     : a(p1),b(p2),c(p3),d(p4)
{ id=++id_counter; }


d3_rat_sphere::d3_rat_sphere()
{ d3_rat_point p1(1,0,0), p2(0,0,0), p3(0,1,0), p4(0,0,1);
  PTR = new d3_rat_sphere_rep(p1,p2,p3,p4);
 }

d3_rat_sphere::d3_rat_sphere(const d3_rat_point& p1, const d3_rat_point& p2,
                                                     const d3_rat_point& p3,
                                                     const d3_rat_point& p4)
{ PTR = new d3_rat_sphere_rep(p1,p2,p3,p4); }


bool d3_rat_sphere::contains(const d3_rat_point& p) const
{ 
  return on_sphere(point1(),point2(),point3(),point4(),p); 
}


bool d3_rat_sphere::inside(const d3_rat_point& p) const
{
  return inside_sphere(point1(),point2(),point3(),point4(),p);
}
 

bool d3_rat_sphere::outside(const d3_rat_point& p) const
{
  return outside_sphere(point1(),point2(),point3(),point4(),p);
}



d3_rat_point d3_rat_sphere::center() const
{ 
  if (is_degenerate())
     LEDA_EXCEPTION(1,"d3_rat_sphere::center: degenerate sphere.");

  d3_rat_plane pl1(point1(),point2(),point4());
  d3_rat_plane pl2(point1(),point3(),point4());
  d3_rat_plane pl3(point1(),point2(),point3());

  d3_rat_point m1 = midpoint(point1(),point2()); 
  d3_rat_point m2 = midpoint(point1(),point3()); 
  d3_rat_point m3 = midpoint(point1(),point4());

  rat_vector r1 = pl1.normal(); 
  rat_vector r2 = pl2.normal(); 
  rat_vector r3 = pl3.normal();
 
  d3_rat_plane sr1(m1,m1+r1,m1+r3),sr2(m2,m2+r2,m2+r3),sr3(m3,m3+r1,m3+r2);

  rat_vector n1 =sr1.normal(); 
  rat_vector n2 =sr2.normal(); 
  rat_vector n3 =sr3.normal();

  rational D1=-(n1*m1.to_vector()); 
  rational D2=-(n2*m2.to_vector()); 
  rational D3=-(n3*m3.to_vector());

  return intersect(n1,n2,n3,D1,D2,D3);
}


rational d3_rat_sphere::sqr_radius() const
{
  return point1().sqr_dist(center());  
}

d3_rat_sphere d3_rat_sphere::translate(const rat_vector& v) const
{ d3_rat_point a = point1();
  d3_rat_point b = point2();
  d3_rat_point c = point3();
  d3_rat_point d = point4();
  return d3_rat_sphere(a.translate(v), b.translate(v), c.translate(v),
                                                       d.translate(v));
 }
 
d3_rat_sphere d3_rat_sphere::translate(const rational& wx, const rational& wy, const rational& wz) const
{ d3_rat_point a=point1();
  d3_rat_point b=point2();
  d3_rat_point c=point3();
  d3_rat_point d=point4();
  return d3_rat_sphere(a.translate(wx,wy,wz), b.translate(wx,wy,wz),
                       c.translate(wx,wy,wz), d.translate(wx,wy,wz));
  }



// auxiliary functions

static rational rat_det(const rational& a11, const rational& a12, const rational& a13,
                        const rational& a21, const rational& a22, const rational& a23,
                        const rational& a31, const rational& a32, const rational& a33) {
 return a11*a22*a33+a12*a23*a31+a13*a21*a32-a13*a22*a31-a11*a23*a32-a12*a21*a33;
}


static d3_rat_point intersect(const rat_vector& n1, const rat_vector& n2, const rat_vector& n3,
                              const rational& D1, const rational& D2, const rational& D3)
{
 rational xw,yw,zw;
 rational det,xdet,ydet,zdet;
 rational A1,B1,C1,A2,B2,C2,A3,B3,C3;

 A1=n1.xcoord(); B1=n1.ycoord(); C1=n1.zcoord();
 A2=n2.xcoord(); B2=n2.ycoord(); C2=n2.zcoord();
 A3=n3.xcoord(); B3=n3.ycoord(); C3=n3.zcoord();

 det= rat_det(A1,B1,C1,A2,B2,C2,A3,B3,C3);
 xdet=rat_det(D1,B1,C1,D2,B2,C2,D3,B3,C3);
 ydet=rat_det(A1,D1,C1,A2,D2,C2,A3,D3,C3);
 zdet=rat_det(A1,B1,D1,A2,B2,D2,A3,B3,D3);

 //if (det==0) cout << "det 0 !\n";
 //degenerated d3_rat_sphere !
 if (det==0) LEDA_EXCEPTION(1,"intersect: determinant==0 .");

 xw=-xdet/det; yw=-ydet/det; zw=-zdet/det;
 xw=xw.normalize();
 yw=yw.normalize();
 zw=zw.normalize();

 return d3_rat_point(xw,yw,zw);
}

LEDA_END_NAMESPACE

