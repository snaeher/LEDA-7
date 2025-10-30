/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rectangle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rectangle.h>


//------------------------------------------------------------------------------
// iso-oriented rectangles
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


atomic_counter rectangle_rep::id_counter(0);


rectangle_rep::rectangle_rep(const point& p1, const point& p2)
{
 double xlow,ylow,xhigh,yhigh;
 if (p1.xcoord()<p2.xcoord()) { xlow=p1.xcoord(); xhigh=p2.xcoord(); } else { xlow=p2.xcoord(); xhigh=p1.xcoord(); }
 if (p1.ycoord()<p2.ycoord()) { ylow=p1.ycoord(); yhigh=p2.ycoord(); } else { ylow=p2.ycoord(); yhigh=p1.ycoord(); }

 p_ll=point(xlow,ylow);
 p_lr=point(xhigh,ylow);
 p_ur=point(xhigh,yhigh);
 p_ul=point(xlow,yhigh);

 id  = ++id_counter; 
}

rectangle_rep::rectangle_rep(const point& p1, double w, double h)
{
 double x,y;
 x=p1.xcoord(); y=p1.ycoord();

 if (w<0) { x=x-w; w=-w; }
 if (h<0) { y=y-h; h=-h; }

 p_ll=point(x,y);
 p_lr=point(x+w,y);
 p_ur=point(x+w,y+h);
 p_ul=point(x,y+h);

 id  = ++id_counter; 
}

rectangle_rep::rectangle_rep()
{
 p_ll=point(0,0);
 p_lr=point(1,0);
 p_ur=point(1,1);
 p_ul=point(0,1);

 id  = ++id_counter; 
}


rectangle::rectangle(const point& p, const point& q)
{
 PTR = new rectangle_rep(p,q);
}

rectangle::rectangle(const point& p, double wd, double hei)
{
 PTR = new rectangle_rep(p,wd,hei);
}

rectangle::rectangle(double x1,double y1,double x2,double y2)
{
 PTR = new rectangle_rep(point(x1,y1),point(x2,y2));
}

rectangle::rectangle()
{
 PTR = new rectangle_rep;
}

point rectangle::upper_left() const {
 return ptr()->p_ul;
}

point rectangle::upper_right() const {
 return ptr()->p_ur;
}

point rectangle::lower_left() const { return ptr()->p_ll; }

point rectangle::lower_right() const { return ptr()->p_lr;  }

point rectangle::center() const
{ return midpoint(ptr()->p_ll, ptr()->p_ur); }

list<point> rectangle::vertices() const {
 list<point> PL;
 PL.append(ptr()->p_ll);
 PL.append(ptr()->p_lr);
 PL.append(ptr()->p_ur);
 PL.append(ptr()->p_ul);

 return PL;
}

double rectangle::xmin() const
{ return (ptr()->p_ll).xcoord(); }
 
double rectangle::xmax() const
{ return (ptr()->p_lr).xcoord(); }

double rectangle::ymin() const
{ return (ptr()->p_ll).ycoord(); }

double rectangle::ymax() const
{ return (ptr()->p_ul).ycoord(); }

double rectangle::width() const
{ point p1= ptr()->p_ll; 
  point p2= ptr()->p_lr;
  return p2.xcoord()-p1.xcoord();
}

double rectangle::height() const
{ point p1= ptr()->p_ll; 
  point p2= ptr()->p_ul;
  return p2.ycoord()-p1.ycoord();
} 

bool rectangle::is_degenerate() const
{ return  collinear(ptr()->p_ll, ptr()->p_lr, ptr()->p_ur); }

bool rectangle::is_point() const
{
 return (ptr()->p_ll == ptr()->p_ur);
}

bool rectangle::is_segment() const
{
 if (is_degenerate() && !is_point()) return true; else return false;
}

int rectangle::cs_code(const point& p) const
// calculate 4-bit position code for p 
{
 int cd=0;
 if (p.ycoord() > ymax()) cd= cd + 8;
 if (p.ycoord() < ymin()) cd= cd + 4;
 if (p.xcoord() > xmax()) cd= cd + 2;
 if (p.xcoord() < xmin()) cd= cd + 1;
 return cd;
}

bool rectangle::inside(const point& p) const
{
 //old variant with orientation tests
 //point p1,p2,p3,p4;
 //p1=ptr()->p_ll; p2=ptr()->p_lr; p3=ptr()->p_ur; p4=ptr()->p_ul;
 //return (left_turn(p1,p2,p) && left_turn(p2,p3,p) && left_turn(p3,p4,p) && left_turn(p4,p1,p) );

 double xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw>xmin() && xw<xmax() && yw>ymin() && yw<ymax()) return true; else return false;
}

bool rectangle::outside(const point& p) const
{
 //old variant with orientation tests
 //point p1,p2,p3,p4;
 //p1=ptr()->p_ll; p2=ptr()->p_lr; p3=ptr()->p_ur; p4=ptr()->p_ul;
 //return (right_turn(p1,p2,p) || right_turn(p2,p3,p) || right_turn(p3,p4,p) || right_turn(p4,p1,p) );

 double xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw<xmin() || xw>xmax() || yw<ymin() || yw>ymax()) return true; else return false;
}

bool rectangle::inside_or_contains(const point& p) const
{
 double xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw>=xmin() && xw<=xmax() && yw>=ymin() && yw<=ymax()) return true; else return false;
}

bool rectangle::contains(const point& p) const
{
 double xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw==xmin() || xw==xmax()){
   if (yw<=ymax() && yw>=ymin()) return true;
   else return false;
 }

 if (yw==ymin() || yw==ymax()){
   if (xw<=xmax() && xw>=xmin()) return true;
   else return false;
 }

 return false;
}

region_kind rectangle::region_of(const point& p) const
{
  int o;
  bool flag=false;
  
  o= point::cmp_y(ptr()->p_ll,p);
  if (o==1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= point::cmp_x(ptr()->p_lr,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= point::cmp_y(ptr()->p_ur,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= point::cmp_x(ptr()->p_ul,p);
  if (o==1) return UNBOUNDED_REGION;
  if (o==0) flag=true;

  if (flag) return  ON_REGION;
  
  return BOUNDED_REGION; 
}

rectangle rectangle::include(const point& p) const
{
 int code=cs_code(p);
 if (code==0) return *this; // inside or contains ...

 double xmin_new,xmax_new,ymin_new,ymax_new;

 if (code & 2) xmax_new=p.xcoord(); else xmax_new= xmax();
 if (code & 1) xmin_new=p.xcoord(); else xmin_new= xmin();
 if (code & 8) ymax_new=p.ycoord(); else ymax_new= ymax();
 if (code & 4) ymin_new=p.ycoord(); else ymin_new= ymin();
 return rectangle(point(xmin_new,ymin_new),point(xmax_new,ymax_new)); 
}

rectangle rectangle::include(const rectangle& r) const
{
 rectangle r1= include(r.upper_left());
 return r1.include(r.lower_right());
}

rectangle rectangle::translate(double dx,double dy) const
{
 point p1= (ptr()->p_ll).translate(dx,dy);
 point p2= (ptr()->p_ur).translate(dx,dy);
 
 return rectangle(p1,p2);
}

rectangle rectangle::translate(const vector& v) const
{
 return rectangle(ptr()->p_ll.translate(v),ptr()->p_ur.translate(v));
}

point rectangle::operator[](int i) const
{
  point p;
  switch (i){
  case 1: p=lower_left(); break;
  case 2: p=lower_right(); break;
  case 3: p=upper_right(); break;
  case 4: p=upper_left(); break;
  }

  return p;
}

rectangle rectangle::rotate90(const point& p, int i) const
{
 return rectangle(ptr()->p_ll.rotate90(p,i),ptr()->p_ur.rotate90(p,i)); 
}

rectangle rectangle::rotate90(int i) const
{
 return rectangle(ptr()->p_ll.rotate90(i),ptr()->p_ur.rotate90(i));
}


rectangle rectangle::reflect(const point& p) const
{
 return rectangle(ptr()->p_ll.reflect(p),ptr()->p_ur.reflect(p));
}


bool  rectangle::difference(const rectangle& b, list<rectangle>& L) const
{
 L.clear();

 double ax0 = this->xmin();
 double ay0 = this->ymin();
 double ax1 = this->xmax();
 double ay1 = this->ymax();
 double bx0 = b.xmin();
 double by0 = b.ymin();
 double bx1 = b.xmax();
 double by1 = b.ymax();

 if (ax1 <= bx0 || bx1 <= ax0 || ay1 <= by0 || by1 <= ay0) 
 {
  L.append(*this); 
  return true; 
 }

 if (by1 < ay1) L.append(rectangle(ax0,by1,ax1,ay1));
 if (ay0 < by0) L.append(rectangle(ax0,ay0,ax1,by0));

 double y0 = (by0 < ay0) ? ay0 : by0;
 double y1 = (ay1 < by1) ? ay1 : by1;

 if (ax0 < bx0) L.append(rectangle(ax0,y0,bx0,y1)); 
 if (bx1 < ax1) L.append(rectangle(bx1,y0,ax1,y1));

 return !L.empty();
}

list<point> rectangle::intersection(const segment& s) const
{
  point p1,p2;
  list<point> rt;
  p1=s.source(); p2=s.target();
  bool b1=inside(p1);
  bool b2=inside(p2);

  if (b1 && b2) return rt; // no intersection

  point ph1,ph2,ph3,ph4;
  b1=s.intersection(segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
  b1=s.intersection(segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
  b1=s.intersection(segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
  b1=s.intersection(segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);

  return rt;  
}

bool rectangle::intersect(point& ps,const segment& s,int c1,int c2) const
{
 segment s1,s2;
 bool rt = false;

 switch (c1){ 
  case 0:
    switch(c2){ 
      case 2:
       s2=segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;        
      case 4:
       s2=segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       break;
      case 6:
       s2=segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 8:
       s2=segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 10:
       s2=segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
    }
    break;
  case 1:
    s2=segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);    
    break;
  case 4:
    s2=segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s2,ps);
    break;
  case 5:
    s1=segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 8:
    s2=segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 9:
    s1=segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
 }

 return rt;
}

bool rectangle::clip(const segment& t,segment& inter,int c1,int c2) const
{ 
 bool flag=false;

 if (c1<0) c1=cs_code(t.source());  else flag=true;
 if (c2<0) c2=cs_code(t.target());  

 if (c1==0 && c2==0) { inter=t; return true; } //inside...
 if ((c1 & c2) != 0) return false;             //outside...

 point ps;
 segment t1;
 int tm;

 if (compare(t.source(),t.target())==-1) t1=t; else { t1=segment(t.target(),t.source()); tm=c1; c1=c2; c2=tm; }

 bool w=intersect(ps,t1,c1,c2); 

 if (! w) return false; //no intersection...
 if (flag) { inter=segment(t.source(),ps); return true; }

 if (outside(t1.source())) clip(segment(ps,t1.target()),inter,0,c2);
 else clip(segment(t1.source(),ps),inter,c1,0); 

 return true;
}

bool rectangle::clip(const segment& t,segment& inter) const
{
 return clip(t,inter,-1,-1);
}


bool rectangle::clip(const line& l,segment& inter) const
{
 if (l.is_vertical()){
   double xw=(l.point1()).xcoord();
   if (xw<=xmax() && xw>=xmin()) { inter=segment(xw,ymin(),xw,ymax()); return true; }
   else return false;   
 }
 // not vertical ...
 point p1,p2;

 if (l.slope() > 5 || l.slope() < -5)
 {
  p1=point(l.x_proj(ymin()-1) ,ymin()-1);
  p2=point(l.x_proj(ymax()+1) ,ymax()+1);  
 }
 else { 
  p1=point(xmin()-1,l.y_proj(xmin()-1));
  p2=point(xmax()+1,l.y_proj(xmax()+1));
 }

 return clip(segment(p1,p2),inter);
}

point rectangle::get_out_point(const ray& r) const
{
 point p1= r.point1();
 point p2= r.point2();
 point ps= p1;
 line l(r);

 if (p2.xcoord() > p1.xcoord() ) { 
   if (p1.xcoord() <= xmax()) ps=point(xmax(),l.y_proj(xmax())); 
 }
 else { 
   if (p1.xcoord() >= xmin()) ps=point(xmin(),l.y_proj(xmin())); 
 }
 
 return ps;
}

bool rectangle::clip(const ray& r,segment& inter) const
{
 if (r.is_vertical()){
   double xw=(r.point1()).xcoord();
   double yw,yw2;
   yw2=(r.point2()).ycoord();

   if (xw<=xmax() && xw>=xmin()) {
    yw=(r.point1()).ycoord();
    if (yw < ymin()){
      if (yw2<yw) return false;
      else { inter=segment(xw,ymin(),xw,ymax()); return true; }
    }
    if (yw > ymax()){
      if (yw2>yw) return false;
      else { inter=segment(xw,ymin(),xw,ymax()); return true; }
    }
    // source of the ray inside or contained by rectangle...
    if (yw2<yw) inter=segment(xw,yw,xw,ymin());
    else inter=segment(xw,yw,xw,ymax());
    return true;
   }

   else return false; 
 }

 point p1=r.source();
 point p2=get_out_point(r);
 return clip(segment(p1,p2),inter);
}

list<point> rectangle::intersection(const line& l) const
{
  list<point> rt;
  int o1,o2;

  o1=orientation(l,upper_left());
  o2=orientation(l,lower_right());

  if (o1==o2) {
    if (o1==0) {
      if (is_point()) rt.append(lower_right());
      else { rt.append(upper_left()); rt.append(lower_right()); }
    }
  }
  else {
   bool b1;
   point ph1,ph2,ph3,ph4;
   b1=l.intersection(segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
   b1=l.intersection(segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
   b1=l.intersection(segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
   b1=l.intersection(segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);   
  }
  return rt;
}

list<rectangle> rectangle::intersection(const rectangle& r) const
{
 list<rectangle> lr;
 segment s1,s2,s3,s4;
 bool b1,b2,b3,b4;
 bool h1,h2,h3,h4;

 b1=inside_or_contains(r.lower_left());
 b2=inside_or_contains(r.lower_right());
 b3=inside_or_contains(r.upper_right());
 b4=inside_or_contains(r.upper_left());

 h1=r.inside_or_contains(lower_left());
 h2=r.inside_or_contains(lower_right());
 h3=r.inside_or_contains(upper_right());
 h4=r.inside_or_contains(upper_left());

 if (b1 && b2 && b3 && b4) { lr.append(r); return lr; } //whole rectangle r inside (*this)
 if (h1 && h2 && h3 && h4) { lr.append(*this); return lr; } // whole rectangle (*this) inside r

 s1=segment(r.upper_left(),r.lower_left());
 s2=segment(r.lower_left(),r.lower_right());
 s3=segment(r.lower_right(),r.upper_right());
 s4=segment(r.upper_right(),r.upper_left());

 list<point> all,l1,l2,l3,l4;
 l1=intersection(s1); l2=intersection(s2);  l3=intersection(s3);  l4=intersection(s4);

 all.conc(l1); all.conc(l2); all.conc(l3); all.conc(l4);
 if (b1)  all.append(r.lower_left());
 if (b2)  all.append(r.lower_right()); 
 if (b3)  all.append(r.upper_right()); 
 if (b4)  all.append(r.upper_left()); 

 if (all.empty()) return lr;
 point p, pex1,pex2;
 pex1=all.head(); pex2=all.head();

 forall(p,all){
  if (p.xcoord() < pex1.xcoord() || (p.xcoord() == pex1.xcoord() && p.ycoord()!=pex2.ycoord())) pex1=p;
  if (p.xcoord() > pex2.xcoord() || (p.xcoord() == pex2.xcoord() && p.ycoord()!=pex1.ycoord())) pex2=p;
 } 

 lr.append(rectangle(pex1,pex2));

 return lr;
}

 
bool rectangle::do_intersect(const rectangle& b) const
{
 double ax0 = this->xmin();
 double ay0 = this->ymin();
 double ax1 = this->xmax();
 double ay1 = this->ymax();
 double bx0 = b.xmin();
 double by0 = b.ymin();
 double bx1 = b.xmax();
 double by1 = b.ymax();

 if (ax1 <= bx0 || bx1 <= ax0 || ay1 <= by0 || by1 <= ay0) 
 { return false; }
 return true;
}


double rectangle::area() const
{
 return width()*height();
}



// Bounding Boxes

rectangle BoundingBox(const point& p)
{ return rectangle(p,p); }

rectangle BoundingBox(const segment& s)
{ return rectangle(s.source(),s.target()); }

rectangle BoundingBox(const ray& r)
{ return BoundingBox(segment(r.source(),r.point2())); }


rectangle BoundingBox(const line& l)
{ return BoundingBox(segment(l.point1(),l.point2())); }


rectangle BoundingBox(const circle& c)
{ point  mp = c.center();
  double cx = mp.xcoord();
  double cy = mp.ycoord();
  double r  = c.radius();
  return rectangle(point(cx-r,cy-r),point(cx+r,cy+r));
}
 

rectangle BoundingBox(const polygon& P)
{
  const list<point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty polygon.");

  double x1 = L.head().xcoord(); 
  double y1 = L.head().ycoord(); 
  double x2 = x1;
  double y2 = y1;

  point p;
  forall(p, L)
  { double x = p.xcoord(); 
    double y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }


  return rectangle(point(x1,y1),point(x2,y2));
}


rectangle BoundingBox(const gen_polygon& P)
{
  const list<point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty gen_polygon.");

  double x1 = L.head().xcoord(); 
  double y1 = L.head().ycoord(); 
  double x2 = x1;
  double y2 = y1;

  point p;
  forall(p, L)
  { double x = p.xcoord(); 
    double y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }

  return rectangle(point(x1,y1),point(x2,y2));    
}

LEDA_END_NAMESPACE 


