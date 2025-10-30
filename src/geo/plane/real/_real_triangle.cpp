/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _real_triangle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/real_triangle.h>
#include <LEDA/geo/real_segment.h>
#include <LEDA/geo/rat_triangle.h>
#include <LEDA/system/assert.h>

//_________________________________________________
// real triangle (2004)
//-------------------------------------------------

LEDA_BEGIN_NAMESPACE 

atomic_counter real_triangle_rep::id_counter(0);

real_triangle_rep::real_triangle_rep(const real_point& p1,const real_point& p2,const real_point& p3) : a(p1),b(p2),c(p3)
{
 id=id_counter++;
}


real_triangle::real_triangle(const real_point&  Ap,const real_point& Bp,const real_point& Cp)
{
  PTR = new real_triangle_rep(Ap,Bp,Cp);
}

real_triangle::real_triangle(real x1,real y1,real x2,real y2,real x3,real y3) 
{
 real_point a(x1,y1),b(x2,y2),c(x3,y3);  
 PTR = new real_triangle_rep(a,b,c);
}

real_triangle::real_triangle()
{
 real_point a(0,0),b(1,0),c(0,1);

 PTR = new real_triangle_rep(a,b,c);
}

real_triangle::real_triangle(const rat_triangle& t1)
{ 
 PTR = new real_triangle_rep(real_point(t1.point1()), 
                             real_point(t1.point2()), 
                             real_point(t1.point3())); 
}

real_point real_triangle::operator[](int i) const
{
  switch(i){
    case 1: { return point1(); }
    case 2: { return point2(); }
  }
  return point3(); //i==3 or precondition not fulfilled
} 

int real_triangle::orientation() const
{
  return point1().orientation(point2(),point3());
}

real real_triangle::area() const
{
  return point1().area(point2(),point3());
}

bool real_triangle::is_degenerate() const
{
 return collinear(point1(),point2(),point3());
}

int real_triangle::side_of(const real_point& p) const
{
  region_kind k = region_of(p);
  switch (k) {
    case ON_REGION:        return 0;
    case BOUNDED_REGION:   return orientation();
    case UNBOUNDED_REGION: return -orientation();
    default:               assert( 0 == 1); return 0;
  }
}

region_kind real_triangle::region_of(const real_point& p) const
{
  int ori1 = point1().orientation(point2(), p);
  int ori2 = point2().orientation(point3(), p);
  int ori3 = point3().orientation(point1(), p);
  
  if (ori1*ori2*ori3 == 0){ // ON_REGION or UNBOUNDED_REGION
    if ((ori1==0) && (ori2==0) && (ori3==0)){ // triangle is degenerate
      if (real_segment(point1(),point2()).contains(p) || real_segment(point2(),point3()).contains(p) ||  \
          real_segment(point3(),point1()).contains(p)) return ON_REGION;
      else return UNBOUNDED_REGION;
    }
    else {
      // case vertex ...
      if ((ori1==0 && ori2==0) || (ori2==0 && ori3==0) || (ori1==0 && ori3==0)) return ON_REGION;
      // case on edge ...
      // one ori==0 , the other 2 != 0
      int res;
      if (ori1==0) res = ori2*ori3;
      else {
        if (ori2==0) res = ori1*ori3;
	else res = ori1*ori2;
      }
      if (res==1) return ON_REGION;
      return UNBOUNDED_REGION;
    }
  }
  else {
    if ((ori1==ori2) && (ori2==ori3) && (ori1==ori3)) return BOUNDED_REGION;
    else return UNBOUNDED_REGION;
  }
}

bool   real_triangle::inside(const real_point& p) const
{ return side_of(p) == 1; }

bool   real_triangle::outside(const real_point& p) const
{ return side_of(p) == -1; }

bool   real_triangle::on_boundary(const real_point& p) const
{ return side_of(p) == 0; }

bool   real_triangle::contains(const real_point& p) const
{ return !outside(p); }


bool   real_triangle::intersection(const real_line& l) const
{
 int ori1 =  l.orientation(point1());
 int ori2 =  l.orientation(point2());
 int ori3 =  l.orientation(point3());
 
 if (ori1==0 || ori2==0 || ori3==0) return true;
 if (ori1!=ori2 || ori1!=ori3 || ori2!=ori3) return true;

 return false;
}

bool   real_triangle::intersection(const real_segment& s) const
{
 real_segment s1(point1(), point2());
 real_segment s2(point2(), point3());
 real_segment s3(point3(), point1());
 
 int ori_tr = orientation();
 
 if (ori_tr == 0) { // triangle is degenerate ...
   bool b1 = s1.intersection(s);
   if (b1) return true;
   bool b2 = s2.intersection(s);
   if (b2) return true; 
   return false; 
 }
 
 // non - degenerate triangle ...
 // test with supporting line of segment ...
 int lori1 =  s.orientation(point1());
 int lori2 =  s.orientation(point2());
 int lori3 =  s.orientation(point3());
 
 if (lori1==0 || lori2==0 || lori3==0 || lori1!=lori2 || lori1!=lori3 || lori2!=lori3) ; // could be an intersection ...
 else return false; 
 
 // intersection test with s1 ...
 int o1 = s1.orientation(s.source());
 int o2 = s1.orientation(s.target());
 
 if (ori_tr == 1) {
   if (o1 == -1 && o2 == -1) return false;
 }
 else { // ori_tr == -1
   if (o1 == 1 && o2 == 1) return false; 
 }
 
 if ((o1 != o2) && (lori1 != lori2)) return true; // intersection with s1 ...
 
 // intersection test with s2 ...
 int o3 = s2.orientation(s.source());
 int o4 = s2.orientation(s.target());
 
 // added
 if (ori_tr == 1) {
   if (o3 == -1 && o4 == -1) return false;
 }
 else { // ori_tr == -1
   if (o3 == 1 && o4 == 1) return false; 
 } 
  
 if ((o3 != o4) && (lori2 != lori3)) return true; // intersection with s2 ...

 // intersection test with s3 ...
 int o5 = s3.orientation(s.source());
 int o6 = s3.orientation(s.target());
  
 if ((o5 != o6) && (lori3 != lori1)) return true; // intersection with s3 ...

 // is s completely inside ?
 if (o1==o3 && o3==o5) return true;
 if (o1==0 && o3==o5) return true;
 if (o3==0 && o1==o5) return true; 
 if (o5==0 && o1==o3) return true; 
 if ((o1==0 && o3==0) || (o1==0 && o5==0) || (o3==0 && o5==0)) return true;

 return false;
}

/*
real_triangle real_triangle::rotate(const real_point& q, real a) const
{
 real_point p1=point1();
 real_point p2=point2();
 real_point p3=point3();
 return real_triangle(p1.rotate(q,a), p2.rotate(q,a), p3.rotate(q,a));
}

real_triangle real_triangle::rotate(real a) const
{
 real_point p1=point1();
 real_point p2=point2();
 real_point p3=point3();
 return real_triangle(p1.rotate(a), p2.rotate(a), p3.rotate(a));
}
*/

real_triangle real_triangle::rotate90(const real_point& q, int i) const
{
 real_point p1=point1();
 real_point p2=point2();
 real_point p3=point3();
 return real_triangle(p1.rotate90(q,i), p2.rotate90(q,i), p3.rotate90(q,i));
}

real_triangle real_triangle::rotate90(int i) const
{ return rotate90(real_point(0,0),i); }


real_triangle real_triangle::reflect(const real_point& p, const real_point& q) const
{
 return real_triangle(point1().reflect(p,q), point2().reflect(p,q),point3().reflect(p,q));
}

real_triangle real_triangle::reflect(const real_point& p) const
{
 real_point p1=point1();
 real_point p2=point2();
 real_point p3=point3();
 return real_triangle(p1.reflect(p),p2.reflect(p),p3.reflect(p)); 
}

real_triangle real_triangle::translate(const real_vector& v) const
{
   real_point a=point1();
   real_point b=point2();
   real_point c=point3();
 
   a=a.translate(v); b=b.translate(v); c=c.translate(v); 
   return real_triangle(a,b,c); 
}

real_triangle real_triangle::translate(real wx,real wy) const
{
   real_point a=point1();
   real_point b=point2();
   real_point c=point3();

   a=a.translate(wx,wy); b=b.translate(wx,wy);
   c=c.translate(wx,wy); 
   return real_triangle(a,b,c);
}

bool real_triangle::operator==(const real_triangle& t2) const
{  
 real_point pa1 = point1();
 real_point pa2 = point2();
 real_point pa3 = point3();
 
 real_point pb1 = t2.point1();
 real_point pb2 = t2.point2();
 real_point pb3 = t2.point3();
 
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
