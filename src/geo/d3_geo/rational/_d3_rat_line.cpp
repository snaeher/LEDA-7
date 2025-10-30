/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_rat_line.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_rat_line.h>
#include <LEDA/geo/d3_rat_plane.h>

//----------------------------------------------
// d3_rat_line (1999)
//----------------------------------------------

LEDA_BEGIN_NAMESPACE

atomic_counter d3_rat_line_rep::id_counter(0);

d3_rat_line_rep::d3_rat_line_rep(const d3_rat_point& p1,const d3_rat_point& p2) : s(p1,p2)
{
 if (p1==p2) LEDA_EXCEPTION(1,"d3_rat_line_rep: constructor got two equal points"); 
 id=++id_counter;
}
   
d3_rat_line_rep::d3_rat_line_rep(const d3_rat_segment& s2) : s(s2)
{
 if (s2.is_trivial()) LEDA_EXCEPTION(1,"d3_rat_line_rep: constructor got trivial segment");
 id=++id_counter;
}

d3_rat_line::d3_rat_line(const d3_rat_point& p1,const d3_rat_point& p2)
{
 PTR = new d3_rat_line_rep(p1,p2);
}

d3_rat_line::d3_rat_line(const list<d3_rat_point>& L)
{
 if (L.size() < 2) LEDA_EXCEPTION(1,"d3_rat_line: constructor: list has fewer than 2 elements.");
 d3_rat_point p1,p2;
 p1= L[L[0]]; p2=L[L[1]];
 PTR = new d3_rat_line_rep(p1,p2);
}

d3_rat_line::d3_rat_line(const d3_rat_segment& s)
{
 PTR = new d3_rat_line_rep(s);
}

d3_rat_line::d3_rat_line()
{
 PTR = new d3_rat_line_rep(d3_rat_point(0,0,0,1),d3_rat_point(1,0,0,1));
}

bool d3_rat_line::get_para(integer& a, integer& k, integer& b, integer& h, integer& wt) const
{
 d3_rat_point p= point1();
 rat_vector v  = to_vector();
 integer x,y,z,w;
 integer vx=v.X(),vy=v.Y(),vz=v.Z(), vw=v.W();
 if (vx==0) return false;

 x=p.X(); y=p.Y(); z=p.Z(); w=p.W();

 a=y*vx-vy*x;
 b=z*vx-vz*x;
 k=vy*w;
 h=vz*w;
 
 wt=w*vx;

 return true;
}


bool d3_rat_line::contains(const d3_rat_point& p) const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 return collinear(p,p1,p2);
}

d3_rat_point d3_rat_line::point1() const
{
 d3_rat_segment s= ptr()->s;
 return s.point1();
}

d3_rat_point d3_rat_line::point2() const
{
 d3_rat_segment s= ptr()->s;
 return s.point2(); 
}

bool d3_rat_line::x_equal() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 return (p1.xcoord() == p2.xcoord());
}

bool d3_rat_line::y_equal() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 return (p1.ycoord() == p2.ycoord());
}

bool d3_rat_line::z_equal() const
{
 d3_rat_point p1=point1();
 d3_rat_point p2=point2();
 return (p1.zcoord() == p2.zcoord());
}

bool d3_rat_line::xy_equal() const
{ return (x_equal() && y_equal()); }

bool d3_rat_line::xz_equal() const
{  return (x_equal() && z_equal()); }

bool d3_rat_line::yz_equal() const
{ return (y_equal() && z_equal()); }


bool d3_rat_line::project_xy(rat_line& l) const
{
 d3_rat_segment ds= seg();
 rat_segment spr= ds.project_xy();
 if (spr.is_trivial()) return false;
 else { l=rat_line(spr); return true; }
} 

bool d3_rat_line::project_xz(rat_line& l) const 
{
 d3_rat_segment ds= seg();
 rat_segment spr= ds.project_xz();
 if (spr.is_trivial()) return false;
 else { l=rat_line(spr); return true; }
}

bool d3_rat_line::project_yz(rat_line& l) const 
{
 d3_rat_segment ds= seg();
 rat_segment spr= ds.project_yz();
 if (spr.is_trivial()) return false;
 else { l=rat_line(spr); return true; }
}

bool d3_rat_line::project(const d3_rat_point& pt, const d3_rat_point& qt, const d3_rat_point& vt,d3_rat_line& l) const
{
 d3_rat_plane p(pt,qt,vt);
 rat_vector v1=p.normal_project(point1());
 rat_vector v2=p.normal_project(point2());
 d3_rat_point pn1= point1()+v1, pn2=point2()+v2;
 if (pn1==pn2) return false;
 l=d3_rat_line(pn1, pn2);
 return true;
}

d3_rat_line d3_rat_line::translate(integer dx,integer dy,integer dz,integer dw) const
{
 d3_rat_segment ds= seg().translate(dx,dy,dz,dw);
 return d3_rat_line(ds);
}

d3_rat_line d3_rat_line::translate(rat_vector v) const
{
 d3_rat_segment ds= seg().translate(v);
 return d3_rat_line(ds);
}

d3_rat_line d3_rat_line::reflect(const d3_rat_point& pt, const d3_rat_point& qt, const d3_rat_point& vt) const
{
 d3_rat_plane p(pt,qt,vt);
 return d3_rat_line(p.reflect_point(point1()),p.reflect_point(point2()));
}

d3_rat_line d3_rat_line::reflect(const d3_rat_point& p) const
{
 d3_rat_segment ds= seg().reflect(p);
 return d3_rat_line(ds); 
}

d3_rat_line d3_rat_line::reverse() const
{
 d3_rat_segment ds= seg().reverse();
 return d3_rat_line(ds); 
}

rat_vector d3_rat_line::to_vector() const
{
 d3_rat_segment seg= ptr()->s;
 rat_vector v=seg.to_vector();
 return v;
}

bool d3_rat_line::intersection(const d3_rat_segment& t) const
{
 d3_rat_line l(t);
 d3_rat_point p;
 bool b= l.intersection(*this,p);

 return b && t.in_xyz(p);
}

bool d3_rat_line::intersection(const d3_rat_segment& t, d3_rat_point& p) const
{ 
 d3_rat_line l(t);
 bool b= l.intersection(*this,p);

 return b && t.in_xyz(p);
}


bool d3_rat_line::intersection(const d3_rat_line& t) const
{
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (collinear(point1(),point2(),t.point1()) && 
     collinear(point1(),point2(),t.point2())) return false;

 integer a1,k1,b1,h1,wpar1;
 integer a2,k2,b2,h2,wpar2;
 rational xs,ys,zs;

 bool ww=get_para(a1,k1,b1,h1,wpar1); 
 bool ww2=t.get_para(a2,k2,b2,h2,wpar2); 

 if (!ww && !ww2) {
    rat_line l1(point1().project_yz(),point2().project_yz());
    rat_line l2(t.point1().project_yz(),t.point2().project_yz());
    rat_point inter;
    bool b=l1.intersection(l2,inter);
    return b;
 }

 if (! ww) {
   rational xc= point1().xcoord();
   rational yc= (k2*xc + a2)/wpar2;
   rational zc= (h2*xc + b2)/wpar2;
   d3_rat_point p=d3_rat_point(xc,yc,zc);
   return collinear(t.point1(),t.point2(),p);
 }
 if (! ww2) {
   rational xc= t.point1().xcoord();
   rational yc= (k1*xc + a1)/wpar1;
   rational zc= (h1*xc + b1)/wpar1;
   d3_rat_point p=d3_rat_point(xc,yc,zc);
   return collinear(point1(),point2(),p);
 }

 a1=a1*wpar2; k1=k1*wpar2; b1=b1*wpar2; h1=h1*wpar2;
 a2=a2*wpar1; k2=k2*wpar1; b2=b2*wpar1; h2=h2*wpar1;

 if (k1==k2 && h1==h2) return false;
 return (a1-a2)*(h1-h2) == (b1-b2)*(k1-k2);
}

bool d3_rat_line::intersection(const d3_rat_line& t, d3_rat_point& p) const
{
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (collinear(point1(),point2(),t.point1()) && 
     collinear(point1(),point2(),t.point2())) return false;

 integer a1,k1,b1,h1,wpar1;
 integer a2,k2,b2,h2,wpar2;
 integer xs,ys,zs;

 bool ww=get_para(a1,k1,b1,h1,wpar1); // returns 0 if vx==0
 bool ww2=t.get_para(a2,k2,b2,h2,wpar2); // returns 0 if vx==0

 if (!ww && !ww2) {
    // both lines in a plane parallel to yz-plane...
    rat_line l1(point1().project_yz(),point2().project_yz());
    rat_line l2(t.point1().project_yz(),t.point2().project_yz());
    rat_point inter;
    bool b=l1.intersection(l2,inter);
    //cout << b << " " << l1 << " " << l2 << "\n";
    p=d3_rat_point(point1().xcoord(),inter.xcoord(),inter.ycoord());
    return b;
 }

 if (! ww) {
   rational xc= point1().xcoord();
   rational yc= (k2*xc + a2)/wpar2;
   rational zc= (h2*xc + b2)/wpar2;
   p=d3_rat_point(xc,yc,zc);
   return collinear(t.point1(),t.point2(),p);
 }
 if (! ww2) {
   rational xc= t.point1().xcoord();
   rational yc= (k1*xc + a1)/wpar1;
   rational zc= (h1*xc + b1)/wpar1;
   p=d3_rat_point(xc,yc,zc);
   return collinear(point1(),point2(),p);
 }

 //cout << "ww:" << ww << "  ww2:" << ww2 << "\n";
 //cout << "Parameters (a1,k1,b1,h1,w):" <<  a1 << " " << k1 << " " << b1 << " " << h1 << " " << wpar1 << "\n";
 //cout << "Parameters (a2,k2,b2,h2,w):" <<  a2 << " " << k2 << " " << b2 << " " << h2 << " " << wpar2 << "\n";
 a1=a1*wpar2; k1=k1*wpar2; b1=b1*wpar2; h1=h1*wpar2;
 a2=a2*wpar1; k2=k2*wpar1; b2=b2*wpar1; h2=h2*wpar1;

 if ((a1-a2)*(h1-h2) != (b1-b2)*(k1-k2)) return false; //no intersection
 if (k1==k2 && h1==h2) return false;

 integer fkt= wpar1*wpar2;
 integer fkt2;
 integer w = k1-k2;
 integer hw= h1-h2;

 if (w!=0) { xs=a2-a1; fkt2=w;  } else { xs=b2-b1; fkt2=hw; }
 ys=k1*xs+a1*fkt2;
 zs=h1*xs+b1*fkt2;
 xs=xs*fkt;

 if (fkt*fkt2==0) {
   cout << "Error! Nenner=0 !\n";
   cout << "Linie1: " << *this << "\n";
   cout << "Linie2: " << t << "\n";
 }
 p=d3_rat_point(xs,ys,zs,fkt*fkt2);
 return true; 
}

rational d3_rat_line::sqr_dist(const d3_rat_point& p) const
{ 
  integer a,b,c,w;
  integer x1=point1().X(), y1=point1().Y(), z1=point1().Z();
  integer w1=point1().W();
  rat_vector v= to_vector();
  integer l=v.X(),m=v.Y(),n=v.Z();

  a=p.X(); b=p.Y(); c=p.Z(); w=p.W();
  integer ax1= a*w1 - x1*w;
  integer by1= b*w1 - y1*w;
  integer cz1= c*w1 - z1*w;

  integer wm1= ax1*m - by1*l; 
  integer wm2= by1*n - cz1*m;
  integer wm3= cz1*l - ax1*n;

  integer zl= wm1*wm1+wm2*wm2+wm3*wm3;
  integer nn= (l*l+m*m+n*n)*w*w*w1*w1;

  rational rt= rational(zl,nn);
  rt.normalize();
  return rt;
}


ostream& operator<<(ostream& s, const d3_rat_line& l){
 s << l.seg();
 return s;
}

istream& operator>>(istream& in, d3_rat_line& l)
{
 d3_rat_point a,b;
 in >> a;
 in >> b; 
 
 l=d3_rat_line(a,b);
 return in;
}


LEDA_END_NAMESPACE
