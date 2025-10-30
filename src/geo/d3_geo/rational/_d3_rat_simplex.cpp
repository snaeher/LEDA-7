/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_rat_simplex.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_rat_simplex.h>
#include <LEDA/geo/d3_rat_plane.h>

//_________________________________________________
// d3_rat_simplex (1999)
//-------------------------------------------------

LEDA_BEGIN_NAMESPACE


atomic_counter d3_rat_simplex_rep::id_counter(0);

d3_rat_simplex_rep::d3_rat_simplex_rep(const d3_rat_point& p1,const d3_rat_point& p2,const d3_rat_point& p3,const d3_rat_point& p4) : a(p1),b(p2),c(p3),d(p4)
{
 id=++id_counter;
}


d3_rat_simplex::d3_rat_simplex(const d3_rat_point&  Ap,const d3_rat_point& Bp,const d3_rat_point& Cp,const d3_rat_point& Dp)
{
  PTR = new d3_rat_simplex_rep(Ap,Bp,Cp,Dp);
}

d3_rat_simplex::d3_rat_simplex()
{
 d3_rat_point a(0,0,0),b(1,0,0),c(0,1,0),d(0,0,1);

 PTR = new d3_rat_simplex_rep(a,b,c,d);
}

d3_simplex d3_rat_simplex::to_d3_simplex() const
{
  return d3_simplex(point1().to_float(),point2().to_float(),point3().to_float(),point4().to_float());
}
 
d3_simplex d3_rat_simplex::to_float() const
{
  return d3_simplex(point1().to_float(),point2().to_float(),point3().to_float(),point4().to_float());
}

d3_rat_point d3_rat_simplex::operator[](int i) const
{
  switch(i){
    case 1: { return point1(); }
    case 2: { return point2(); }
    case 3: { return point3(); }
  }
  return point4(); //i==4 or precondition not fulfilled
} 

int d3_rat_simplex::index(const d3_rat_point& p) const
{
  if (p==point1()) return 1;
  if (p==point2()) return 2;
  if (p==point3()) return 3;
  if (p==point4()) return 4;

  return 0;
}

 bool d3_rat_simplex::is_degenerate() const
 {
  return coplanar(point1(),point2(),point3(),point4());
 }


 d3_rat_sphere d3_rat_simplex::circumscribing_sphere() const
 {
  return d3_rat_sphere(point1(),point2(),point3(),point4());
 }

bool d3_rat_simplex::in_simplex(const d3_rat_point& p) const
{
  return contained_in_simplex( point1(), point2(), point3(), point4(), p);
}

bool d3_rat_simplex::insphere(const d3_rat_point& p) const
{
  return inside_sphere(point1(), point2(), point3(), point4(), p);
}


rational d3_rat_simplex::vol() const
{
  return volume(point1(),point2(),point3(),point4());
}

d3_rat_simplex d3_rat_simplex::reflect(const d3_rat_point& pt, const d3_rat_point& qt, const d3_rat_point& vt) const
{
 d3_rat_plane p(pt,qt,vt);
 return d3_rat_simplex(p.reflect_point(point1()),p.reflect_point(point2()),p.reflect_point(point3()),p.reflect_point(point4()));
}

d3_rat_simplex d3_rat_simplex::reflect(const d3_rat_point& p) const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 d3_rat_point p3=point3();
 d3_rat_point p4=point4(); 

 return d3_rat_simplex(p1.reflect(p),p2.reflect(p),p3.reflect(p),p4.reflect(p)); 
}

d3_rat_simplex d3_rat_simplex::translate(const rat_vector& v) const
// precondition v.dim==3
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();
   d3_rat_point c=point3();
   d3_rat_point d=point4();   
 
   a=a.translate(v); b=b.translate(v); c=c.translate(v); d=d.translate(v); 
   return d3_rat_simplex(a,b,c,d); 
}

d3_rat_simplex d3_rat_simplex::translate(rational wx,rational wy,rational wz) const
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();
   d3_rat_point c=point3();
   d3_rat_point d=point4();

   a=a.translate(wx,wy,wz); b=b.translate(wx,wy,wz);
   c=c.translate(wx,wy,wz); d=d.translate(wx,wy,wz);
      
   return d3_rat_simplex(a,b,c,d);
}

d3_rat_simplex d3_rat_simplex::translate(integer wx,integer wy,integer wz,integer ww) const
{
   d3_rat_point a=point1();
   d3_rat_point b=point2();
   d3_rat_point c=point3();
   d3_rat_point d=point4();   

   a=a.translate(wx,wy,wz,ww); b=b.translate(wx,wy,wz,ww);
   c=c.translate(wx,wy,wz,ww); d=d.translate(wx,wy,wz,ww);
      
   return d3_rat_simplex(a,b,c,d);
}

LEDA_END_NAMESPACE

