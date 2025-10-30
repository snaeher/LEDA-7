/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_triangle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_triangle.h>

//_________________________________________________
// d3_triangle (2000)
//-------------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter d3_triangle_rep::id_counter(0);

d3_triangle_rep::d3_triangle_rep(const d3_point& p1,const d3_point& p2,const d3_point& p3) : a(p1),b(p2),c(p3)
{
 id=++id_counter;
}


d3_triangle::d3_triangle(const d3_point&  Ap,const d3_point& Bp,const d3_point& Cp)
{
  PTR = new d3_triangle_rep(Ap,Bp,Cp);
}

d3_triangle::d3_triangle()
{
 d3_point a(0,0,0),b(1,0,0),c(0,1,0);

 PTR = new d3_triangle_rep(a,b,c);
}

d3_plane d3_triangle::supporting_plane() const
{
 return d3_plane(point1(),point2(),point3());
}

d3_point d3_triangle::operator[](int i) const
{
  switch(i){
    case 1: { return point1(); }
    case 2: { return point2(); }
  }
  return point3(); //i==3 or precondition not fulfilled
} 
 
int d3_triangle::index(const d3_point& p) const
{
  if (p==point1()) return 1;
  if (p==point2()) return 2;
  if (p==point3()) return 3;
  return 0;
}

bool d3_triangle::is_degenerate() const
{
 return collinear(point1(),point2(),point3());
}


d3_triangle d3_triangle::reflect(const d3_point& pt, const d3_point& qt, const d3_point& vt) const
{
 d3_plane p(pt,qt,vt);
 return d3_triangle(p.reflect_point(point1()),p.reflect_point(point2()),p.reflect_point(point3()));
}

d3_triangle d3_triangle::reflect(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();
 d3_point p3=point3();

 return d3_triangle(p1.reflect(p),p2.reflect(p),p3.reflect(p)); 
}

d3_triangle d3_triangle::translate(const vector& v) const
// precondition v.dim==3
{
   d3_point a=point1();
   d3_point b=point2();
   d3_point c=point3();
 
   a=a.translate(v); b=b.translate(v); c=c.translate(v); 
   return d3_triangle(a,b,c); 
}

d3_triangle d3_triangle::translate(double wx,double wy,double wz) const
{
   d3_point a=point1();
   d3_point b=point2();
   d3_point c=point3();

   a=a.translate(wx,wy,wz); b=b.translate(wx,wy,wz);
   c=c.translate(wx,wy,wz); 
   return d3_triangle(a,b,c);
}

bool d3_triangle::operator==(const d3_triangle& t2) const
{  
  //return (point1()==t2.point1() && point2()==t2.point2() && point3()==t2.point3());
 d3_point pa1 = point1();
 d3_point pa2 = point2();
 d3_point pa3 = point3();
 
 d3_point pb1 = t2.point1();
 d3_point pb2 = t2.point2();
 d3_point pb3 = t2.point3();
 
 // test pa1 ...
 int fl1=0, fl2=0, fl3=0;
 if (pa1 == pb1) fl1=1;
 else {
   if (pa1 == pb2) fl1=2;
   else {
     if (pa1 == pb3) fl1=3;
     else return false;
   }
 }
 
 // test pa2 ...
 switch (fl1) {
  case 1: 
  {
   if (pa2 == pb2) fl2=2;
   else {
     if (pa2 == pb3) fl2=3;
     else return false;
   }
   break;
  }
  case 2:
  {
   if (pa2 == pb1) fl2=1;
   else {
     if (pa2 == pb3) fl2=3;
     else return false;
   }
   break;
  }
  case 3:
  {
   if (pa2 == pb1) fl2=1;
   else {
     if (pa2 == pb2) fl2=2;
     else return false;
   }
   break;
  }
 }
 
 // test pa3
 fl3 =1;
 if ((fl3==fl1) || (fl3==fl2)) fl3 = 2;
 if ((fl3==fl1) || (fl3==fl2)) fl3 = 3; 
 
 switch (fl3){
   case 1: { if (pa3==pb1) ; else return false;
    break;
   }
   case 2: { if (pa3==pb2) ; else return false;
    break;
   }
   case 3: { if (pa3==pb3) ; else return false;
    break;
   }
 }
 
 return true; 
}


LEDA_END_NAMESPACE


