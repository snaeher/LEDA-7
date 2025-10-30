/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_line.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_line.h>
#include <LEDA/geo/d3_plane.h>

//----------------------------------------------
// d3_line (1999)
//----------------------------------------------


LEDA_BEGIN_NAMESPACE

atomic_counter d3_line_rep::id_counter(0); 

d3_line_rep::d3_line_rep(const d3_point& p1,const d3_point& p2) : s(p1,p2)
{
 if (p1==p2) LEDA_EXCEPTION(1,"d3_line_rep: constructor got two equal points");
 id=++id_counter;
}
   
d3_line_rep::d3_line_rep(const d3_segment& s2) : s(s2)
{
 if (s2.is_trivial()) LEDA_EXCEPTION(1,"d3_line_rep: constructor got trivial segment");
 id=++id_counter;
}

d3_line::d3_line(const d3_point& p1,const d3_point& p2)
{
 PTR = new d3_line_rep(p1,p2);
}

d3_line::d3_line(const list<d3_point>& L)
{
 if (L.size() < 2) LEDA_EXCEPTION(1,"d3_line: constructor: list has fewer than 2 elements.");
 d3_point p1,p2;
 p1= L[L[0]]; p2=L[L[1]];
 PTR = new d3_line_rep(p1,p2);
}

d3_line::d3_line(const d3_segment& s)
{
 PTR = new d3_line_rep(s);
}

d3_line::d3_line()
{
 PTR = new d3_line_rep(d3_point(0,0,0),d3_point(1,0,0));
}

bool d3_line::get_para(double& a,double& k, double& b, double& h) const
{
 d3_point p= point1();
 vector v  = to_vector();
 double x,y,z;
 double vx=v.xcoord(),vy=v.ycoord(),vz=v.zcoord();
 if (vx==0) return false;

 x=p.xcoord(); y=p.ycoord(); z=p.zcoord();
 k=vy/vx; a=y-x*vy/vx;
 h=vz/vx; b=z-x*vz/vx;

 return true;
}

bool d3_line::contains(const d3_point& p) const
{
 d3_point p1=point1();
 d3_point p2=point2();
 if (collinear(p,p1,p2)) return true; else return false;
}

d3_point d3_line::point1() const
{
 d3_segment s= ptr()->s;
 return s.point1();
}

d3_point d3_line::point2() const
{
 d3_segment s= ptr()->s;
 return s.point2(); 
}

d3_segment d3_line::seg() const
{
 d3_segment s= ptr()->s;
 return s;
}

bool d3_line::x_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.xcoord() == p2.xcoord());
}

bool d3_line::y_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.ycoord() == p2.ycoord());
}

bool d3_line::z_equal() const
{
 d3_point p1=point1();
 d3_point p2=point2();
 return (p1.zcoord() == p2.zcoord());
}

bool d3_line::xy_equal() const
{ return (x_equal() && y_equal()); }

bool d3_line::xz_equal() const
{  return (x_equal() && z_equal()); }

bool d3_line::yz_equal() const
{ return (y_equal() && z_equal()); }

bool d3_line::project_xy(line& l) const
{
 d3_segment ds= seg();
 segment spr= ds.project_xy();
 if (spr.is_trivial()) return false;
 else { l=line(spr); return true; }
} 

bool d3_line::project_xz(line& l) const 
{
 d3_segment ds= seg();
 segment spr= ds.project_xz();
 if (spr.is_trivial()) return false;
 else { l=line(spr); return true; }
}

bool d3_line::project_yz(line& l) const 
{
 d3_segment ds= seg();
 segment spr= ds.project_yz();
 if (spr.is_trivial()) return false;
 else { l=line(spr); return true; }
}

bool d3_line::project(const d3_point& pt, const d3_point& qt, const d3_point& vt, d3_line& l) const
{
 d3_plane p(pt,qt,vt);
 vector v1=p.normal_project(point1());
 vector v2=p.normal_project(point2());
 d3_point pn1= point1()+v1, pn2= point2()+v2;
 if (pn1==pn2) return false;
 l=d3_line(pn1,pn2);
 return true;
}

d3_line d3_line::translate(double dx,double dy,double dz) const
{
 d3_segment ds= seg().translate(dx,dy,dz);
 return d3_line(ds);
}

d3_line d3_line::translate(const vector& v) const
{
 d3_segment ds= seg().translate(v);
 return d3_line(ds);
}

d3_line d3_line::reflect(const d3_point& pt, const d3_point& qt, const d3_point& vt) const
{
 d3_plane p(pt,qt,vt);
 return d3_line(p.reflect_point(point1()),p.reflect_point(point2()));
}

d3_line d3_line::reflect(const d3_point& p) const
{
 d3_segment ds= seg().reflect(p);
 return d3_line(ds); 
}

d3_line d3_line::reverse() const
{
 d3_segment ds= seg().reverse();
 return d3_line(ds); 
}

vector d3_line::to_vector() const
{
 d3_segment seg= ptr()->s;
 return seg.to_vector();
}

bool d3_line::project_2d_intersection_segment(const d3_segment& t) const
{
 if (t.is_trivial()) return contains(t.point1());

 line l1;
 segment s2;
 point pin;

 s2=t.project_xy();
 if (project_xy(l1)){
   if (l1!= line(s2)) return l1.intersection(s2,pin);
 }
 s2=t.project_xz();
 if (project_xz(l1)){
   if (l1!= line(s2)) return l1.intersection(s2,pin);
 }
 s2=t.project_yz();
 if (project_yz(l1)){
   if (l1!= line(s2)) return l1.intersection(s2,pin);
 }
 return false;
}


bool d3_line::intersection(const d3_segment& t) const
{
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (! project_2d_intersection_segment(t)) return false; //new...
 d3_line l(t);
 d3_point p;
 return l.intersection(*this,p);
}

bool d3_line::intersection(const d3_segment& t,d3_point& p) const
{ 
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (! project_2d_intersection_segment(t)) return false; // new
 d3_line l(t);
 return l.intersection(*this,p);
}

bool d3_line::intersection(const d3_line& t) const
{
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (collinear(point1(),point2(),t.point1()) && 
     collinear(point1(),point2(),t.point2())) return false;

 point pin;
 return project_2d_intersection(t,pin);
}

bool d3_line::project_2d_intersection(const d3_line& t, point& pin) const
{
 line l1,l2;
 bool i=false;

 if (project_xy(l1) && t.project_xy(l2)){
   if (l1!=l2) i=l1.intersection(l2,pin);
   if (i) return true;
 }
 if (project_xz(l1) && t.project_xz(l2)){
   if (l1!=l2) i=l1.intersection(l2,pin);
   if (i) return true;  
 }
 if (project_yz(l1) && t.project_yz(l2)){
   if (l1!=l2) i=l1.intersection(l2,pin);
   if (i) return true;  
 }
 return false;
}

bool d3_line::intersection(const d3_line& t,d3_point& p) const
{ 
 if (! coplanar(point1(),point2(),t.point1(),t.point2())) return false;
 if (collinear(point1(),point2(),t.point1()) && 
     collinear(point1(),point2(),t.point2())) return false;
  
 point pin;
 if (! project_2d_intersection(t,pin)) return false;

 // get point of intersection ...
 double a1,k1,b1,h1;
 double a2,k2,b2,h2;
 double xs,ys,zs;

 bool ww=get_para(a1,k1,b1,h1);
 bool ww2=t.get_para(a2,k2,b2,h2); 

 if (!ww && !ww2) {
    //cout << " ww und ww2 0!\n";
    line l1(point1().project_yz(),point2().project_yz());
    line l2(t.point1().project_yz(),t.point2().project_yz());
    point inter;
    //cout << " " << l1 << " " << l2 << "\n";
    bool b=l1.intersection(l2,inter);
    //cout << " " << b << "\n";
    p=d3_point(point1().xcoord(),inter.xcoord(),inter.ycoord());
    return b;
 }

 if (! ww) {
   double xc= point1().xcoord();
   double yc= k2*xc + a2;
   double zc= h2*xc + b2;
   p=d3_point(xc,yc,zc);
   return true;
 }
 if (! ww2) {
   double xc= t.point1().xcoord();
   double yc= k1*xc + a1;
   double zc= h1*xc + b1;
   p=d3_point(xc,yc,zc);
   return true; 
 }

 double w= k1-k2;
 if (w!=0) xs=(a2-a1)/w; else xs=(b2-b1)/(h1-h2);

 ys=k1*xs+a1;
 zs=h1*xs+b1;

 p=d3_point(xs,ys,zs);
 return true; 
}

double d3_line::sqr_dist(const d3_point& p) const
{ 
  double a,b,c,di;
  double x1=(point1()).xcoord(), y1=(point1()).ycoord(), z1=(point1()).zcoord();
  vector v= to_vector();
  double l=v.xcoord(),m=v.ycoord(),n=v.zcoord();

  a=p.xcoord(); b=p.ycoord(); c=p.zcoord();
  double ax1=a-x1, by1=b-y1, cz1=c-z1;

  double w1= ax1*m - by1*l;
  double w2= by1*n - cz1*m;
  double w3= cz1*l - ax1*n;

  di= (w1*w1+w2*w2+w3*w3)/(l*l+m*m+n*n);
  
  return di;
}

double d3_line::distance(const d3_point& p) const
{ 
  return sqrt(sqr_dist(p)); 
}

ostream& operator<<(ostream& s, const d3_line& l){
 s << l.seg();
 return s;
}

istream& operator>>(istream& in, d3_line& l)
{
 d3_point a,b;
 in >> a;
 in >> b; 
 
 l=d3_line(a,b);
 return in;
}


LEDA_END_NAMESPACE
