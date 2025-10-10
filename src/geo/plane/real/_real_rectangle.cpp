/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_rectangle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/real_rectangle.h>
#include <LEDA/geo/rat_rectangle.h>
#include <ctype.h>


//------------------------------------------------------------------------------
// iso-oriented real rectangles
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


atomic_counter real_rectangle_rep::id_counter(0);


real_rectangle_rep::real_rectangle_rep(const real_point& p1, const real_point& p2)
{
 real xlow,ylow,xhigh,yhigh;
 if (p1.xcoord()<p2.xcoord()) { xlow=p1.xcoord(); xhigh=p2.xcoord(); } else { xlow=p2.xcoord(); xhigh=p1.xcoord(); }
 if (p1.ycoord()<p2.ycoord()) { ylow=p1.ycoord(); yhigh=p2.ycoord(); } else { ylow=p2.ycoord(); yhigh=p1.ycoord(); }

 p_ll=real_point(xlow,ylow);
 p_lr=real_point(xhigh,ylow);
 p_ur=real_point(xhigh,yhigh);
 p_ul=real_point(xlow,yhigh);

 id  = id_counter++; 
}

real_rectangle_rep::real_rectangle_rep(const real_point& p1, real w, real h)
{
 real x,y;
 x=p1.xcoord(); y=p1.ycoord();

 if (w<0) { x=x-w; w=-w; }
 if (h<0) { y=y-h; h=-h; }

 p_ll=real_point(x,y);
 p_lr=real_point(x+w,y);
 p_ur=real_point(x+w,y+h);
 p_ul=real_point(x,y+h);

 id  = id_counter++; 
}

real_rectangle_rep::real_rectangle_rep()
{
 p_ll=real_point(0,0);
 p_lr=real_point(1,0);
 p_ur=real_point(1,1);
 p_ul=real_point(0,1);

 id  = id_counter++; 
}


real_rectangle::real_rectangle(const real_point& p, const real_point& q)
{
 PTR = new real_rectangle_rep(p,q);
}

real_rectangle::real_rectangle(const real_point& p, real wd, real hei)
{
 PTR = new real_rectangle_rep(p,wd,hei);
}

real_rectangle::real_rectangle(real x1,real y1,real x2,real y2)
{
 PTR = new real_rectangle_rep(real_point(x1,y1),real_point(x2,y2));
}

real_rectangle::real_rectangle()
{
 PTR = new real_rectangle_rep;
}

real_rectangle::real_rectangle(const rat_rectangle& r1)
{ 
 PTR = new real_rectangle_rep(real_point(r1.upper_left()),
                              real_point(r1.lower_right()));
}

real_point real_rectangle::upper_left() const {
 return ptr()->p_ul;
}

real_point real_rectangle::upper_right() const {
 return ptr()->p_ur;
}

real_point real_rectangle::lower_left() const { return ptr()->p_ll; }

real_point real_rectangle::lower_right() const { return ptr()->p_lr;  }

real_point real_rectangle::center() const
{ return midpoint(ptr()->p_ll, ptr()->p_ur); }

list<real_point> real_rectangle::vertices() const {
 list<real_point> PL;
 PL.append(ptr()->p_ll);
 PL.append(ptr()->p_lr);
 PL.append(ptr()->p_ur);
 PL.append(ptr()->p_ul);

 return PL;
}

real real_rectangle::xmin() const
{ return (ptr()->p_ll).xcoord(); }
 
real real_rectangle::xmax() const
{ return (ptr()->p_lr).xcoord(); }

real real_rectangle::ymin() const
{ return (ptr()->p_ll).ycoord(); }

real real_rectangle::ymax() const
{ return (ptr()->p_ul).ycoord(); }

real real_rectangle::width() const
{ real_point p1= ptr()->p_ll; 
  real_point p2= ptr()->p_lr;
  return p2.xcoord()-p1.xcoord();
}

real real_rectangle::height() const
{ real_point p1= ptr()->p_ll; 
  real_point p2= ptr()->p_ul;
  return p2.ycoord()-p1.ycoord();
} 

bool real_rectangle::is_degenerate() const
{ return  collinear(ptr()->p_ll, ptr()->p_lr, ptr()->p_ur); }

bool real_rectangle::is_point() const
{
 return (ptr()->p_ll == ptr()->p_ur);
}

bool real_rectangle::is_segment() const
{
 if (is_degenerate() && !is_point()) return true; else return false;
}

int real_rectangle::cs_code(const real_point& p) const
// calculate 4-bit position code for p 
{
 int cd=0;
 if (p.ycoord() > ymax()) cd= cd + 8;
 if (p.ycoord() < ymin()) cd= cd + 4;
 if (p.xcoord() > xmax()) cd= cd + 2;
 if (p.xcoord() < xmin()) cd= cd + 1;
 return cd;
}

bool real_rectangle::inside(const real_point& p) const
{
 //old variant with orientation tests
 //real_point p1,p2,p3,p4;
 //p1=ptr()->p_ll; p2=ptr()->p_lr; p3=ptr()->p_ur; p4=ptr()->p_ul;
 //return (left_turn(p1,p2,p) && left_turn(p2,p3,p) && left_turn(p3,p4,p) && left_turn(p4,p1,p) );

 real xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw>xmin() && xw<xmax() && yw>ymin() && yw<ymax()) return true; else return false;
}

bool real_rectangle::outside(const real_point& p) const
{
 //old variant with orientation tests
 //real_point p1,p2,p3,p4;
 //p1=ptr()->p_ll; p2=ptr()->p_lr; p3=ptr()->p_ur; p4=ptr()->p_ul;
 //return (right_turn(p1,p2,p) || right_turn(p2,p3,p) || right_turn(p3,p4,p) || right_turn(p4,p1,p) );

 real xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw<xmin() || xw>xmax() || yw<ymin() || yw>ymax()) return true; else return false;
}

bool real_rectangle::inside_or_contains(const real_point& p) const
{
 real xw,yw;
 xw=p.xcoord();yw=p.ycoord();

 if (xw>=xmin() && xw<=xmax() && yw>=ymin() && yw<=ymax()) return true; else return false;
}

bool real_rectangle::contains(const real_point& p) const
{
 real xw,yw;
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

region_kind real_rectangle::region_of(const real_point& p) const
{
  int o;
  bool flag=false;
  
  o= real_point::cmp_y(ptr()->p_ll,p);
  if (o==1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= real_point::cmp_x(ptr()->p_lr,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= real_point::cmp_y(ptr()->p_ur,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= real_point::cmp_x(ptr()->p_ul,p);
  if (o==1) return UNBOUNDED_REGION;
  if (o==0) flag=true;

  if (flag) return  ON_REGION;
  
  return BOUNDED_REGION; 
}

real_rectangle real_rectangle::include(const real_point& p) const
{
 int code=cs_code(p);
 if (code==0) return *this; // inside or contains ...

 real xmin_new,xmax_new,ymin_new,ymax_new;

 if (code & 2) xmax_new=p.xcoord(); else xmax_new= xmax();
 if (code & 1) xmin_new=p.xcoord(); else xmin_new= xmin();
 if (code & 8) ymax_new=p.ycoord(); else ymax_new= ymax();
 if (code & 4) ymin_new=p.ycoord(); else ymin_new= ymin();
 return real_rectangle(real_point(xmin_new,ymin_new),real_point(xmax_new,ymax_new)); 
}

real_rectangle real_rectangle::include(const real_rectangle& r) const
{
 real_rectangle r1= include(r.upper_left());
 return r1.include(r.lower_right());
}

real_rectangle real_rectangle::translate(real dx,real dy) const
{
 real_point p1= (ptr()->p_ll).translate(dx,dy);
 real_point p2= (ptr()->p_ur).translate(dx,dy);
 
 return real_rectangle(p1,p2);
}

real_rectangle real_rectangle::translate(const real_vector& v) const
{
 return real_rectangle(ptr()->p_ll.translate(v),ptr()->p_ur.translate(v));
}

real_point real_rectangle::operator[](int i) const
{
  real_point p;
  switch (i){
  case 1: p=lower_left(); break;
  case 2: p=lower_right(); break;
  case 3: p=upper_right(); break;
  case 4: p=upper_left(); break;
  }

  return p;
}

real_rectangle real_rectangle::rotate90(const real_point& p, int i) const
{
 return real_rectangle(ptr()->p_ll.rotate90(p,i),ptr()->p_ur.rotate90(p,i)); 
}

real_rectangle real_rectangle::rotate90(int i) const
{
 return real_rectangle(ptr()->p_ll.rotate90(i),ptr()->p_ur.rotate90(i));
}


real_rectangle real_rectangle::reflect(const real_point& p) const
{
 return real_rectangle(ptr()->p_ll.reflect(p),ptr()->p_ur.reflect(p));
}


bool  real_rectangle::difference(const real_rectangle& b, list<real_rectangle>& L) const
{
 L.clear();

 real ax0 = this->xmin();
 real ay0 = this->ymin();
 real ax1 = this->xmax();
 real ay1 = this->ymax();
 real bx0 = b.xmin();
 real by0 = b.ymin();
 real bx1 = b.xmax();
 real by1 = b.ymax();

 if (ax1 <= bx0 || bx1 <= ax0 || ay1 <= by0 || by1 <= ay0) 
 {
  L.append(*this); 
  return true; 
 }

 if (by1 < ay1) L.append(real_rectangle(ax0,by1,ax1,ay1));
 if (ay0 < by0) L.append(real_rectangle(ax0,ay0,ax1,by0));

 real y0 = (by0 < ay0) ? ay0 : by0;
 real y1 = (ay1 < by1) ? ay1 : by1;

 if (ax0 < bx0) L.append(real_rectangle(ax0,y0,bx0,y1)); 
 if (bx1 < ax1) L.append(real_rectangle(bx1,y0,ax1,y1));

 return !L.empty();
}

list<real_point> real_rectangle::intersection(const real_segment& s) const
{
  real_point p1,p2;
  list<real_point> rt;
  p1=s.source(); p2=s.target();
  bool b1=inside(p1);
  bool b2=inside(p2);

  if (b1 && b2) return rt; // no intersection

  real_point ph1,ph2,ph3,ph4;
  b1=s.intersection(real_segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
  b1=s.intersection(real_segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
  b1=s.intersection(real_segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
  b1=s.intersection(real_segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);

  return rt;  
}

bool real_rectangle::intersect(real_point& ps,const real_segment& s,int c1,int c2) const
{
 real_segment s1,s2;
 bool rt = false;

 switch (c1){ 
  case 0:
    switch(c2){ 
      case 2:
       s2=real_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;        
      case 4:
       s2=real_segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       break;
      case 6:
       s2=real_segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=real_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 8:
       s2=real_segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 10:
       s2=real_segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=real_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
    }
    break;
  case 1:
    s2=real_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);    
    break;
  case 4:
    s2=real_segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s2,ps);
    break;
  case 5:
    s1=real_segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=real_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 8:
    s2=real_segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 9:
    s1=real_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=real_segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
 }

 return rt;
}

bool real_rectangle::clip(const real_segment& t,real_segment& inter,int c1,int c2) const
{ 
 bool flag=false;

 if (c1<0) c1=cs_code(t.source());  else flag=true;
 if (c2<0) c2=cs_code(t.target());  

 if (c1==0 && c2==0) { inter=t; return true; } //inside...
 if ((c1 & c2) != 0) return false;             //outside...

 real_point ps;
 real_segment t1;
 int tm;

 if (compare(t.source(),t.target())==-1) t1=t; else { t1=real_segment(t.target(),t.source()); tm=c1; c1=c2; c2=tm; }

 bool w=intersect(ps,t1,c1,c2); 

 if (! w) return false; //no intersection...
 if (flag) { inter=real_segment(t.source(),ps); return true; }

 if (outside(t1.source())) clip(real_segment(ps,t1.target()),inter,0,c2);
 else clip(real_segment(t1.source(),ps),inter,c1,0); 

 return true;
}

bool real_rectangle::clip(const real_segment& t,real_segment& inter) const
{
 return clip(t,inter,-1,-1);
}


bool real_rectangle::clip(const real_line& l,real_segment& inter) const
{
 if (l.is_vertical()){
   real xw=(l.point1()).xcoord();
   if (xw<=xmax() && xw>=xmin()) { inter=real_segment(xw,ymin(),xw,ymax()); return true; }
   else return false;   
 }
 // not vertical ...
 real_point p1,p2;

 if (l.slope() > 5 || l.slope() < -5)
 {
  p1=real_point(l.x_proj(ymin()-1) ,ymin()-1);
  p2=real_point(l.x_proj(ymax()+1) ,ymax()+1);  
 }
 else { 
  p1=real_point(xmin()-1,l.y_proj(xmin()-1));
  p2=real_point(xmax()+1,l.y_proj(xmax()+1));
 }

 return clip(real_segment(p1,p2),inter);
}

real_point real_rectangle::get_out_point(const real_ray& r) const
{
 real_point p1= r.point1();
 real_point p2= r.point2();
 real_point ps= p1;
 real_line l(r);

 if (p2.xcoord() > p1.xcoord() ) { 
   if (p1.xcoord() <= xmax()) ps=real_point(xmax(),l.y_proj(xmax())); 
 }
 else { 
   if (p1.xcoord() >= xmin()) ps=real_point(xmin(),l.y_proj(xmin())); 
 }
 
 return ps;
}

bool real_rectangle::clip(const real_ray& r,real_segment& inter) const
{
 if (r.is_vertical()){
   real xw=(r.point1()).xcoord();
   real yw,yw2;
   yw2=(r.point2()).ycoord();

   if (xw<=xmax() && xw>=xmin()) {
    yw=(r.point1()).ycoord();
    if (yw < ymin()){
      if (yw2<yw) return false;
      else { inter=real_segment(xw,ymin(),xw,ymax()); return true; }
    }
    if (yw > ymax()){
      if (yw2>yw) return false;
      else { inter=real_segment(xw,ymin(),xw,ymax()); return true; }
    }
    // source of the real_ray inside or contained by rectangle...
    if (yw2<yw) inter=real_segment(xw,yw,xw,ymin());
    else inter=real_segment(xw,yw,xw,ymax());
    return true;
   }

   else return false; 
 }

 real_point p1=r.source();
 real_point p2=get_out_point(r);
 return clip(real_segment(p1,p2),inter);
}

list<real_point> real_rectangle::intersection(const real_line& l) const
{
  list<real_point> rt;
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
   real_point ph1,ph2,ph3,ph4;
   b1=l.intersection(real_segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
   b1=l.intersection(real_segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
   b1=l.intersection(real_segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
   b1=l.intersection(real_segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);   
  }
  return rt;
}

list<real_rectangle> real_rectangle::intersection(const real_rectangle& r) const
{
 list<real_rectangle> lr;
 real_segment s1,s2,s3,s4;
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

 s1=real_segment(r.upper_left(),r.lower_left());
 s2=real_segment(r.lower_left(),r.lower_right());
 s3=real_segment(r.lower_right(),r.upper_right());
 s4=real_segment(r.upper_right(),r.upper_left());

 list<real_point> all,l1,l2,l3,l4;
 l1=intersection(s1); l2=intersection(s2);  l3=intersection(s3);  l4=intersection(s4);

 all.conc(l1); all.conc(l2); all.conc(l3); all.conc(l4);
 if (b1)  all.append(r.lower_left());
 if (b2)  all.append(r.lower_right()); 
 if (b3)  all.append(r.upper_right()); 
 if (b4)  all.append(r.upper_left()); 

 if (all.empty()) return lr;
 real_point p, pex1,pex2;
 pex1=all.head(); pex2=all.head();

 forall(p,all){
  if (p.xcoord() < pex1.xcoord() || (p.xcoord() == pex1.xcoord() && p.ycoord()!=pex2.ycoord())) pex1=p;
  if (p.xcoord() > pex2.xcoord() || (p.xcoord() == pex2.xcoord() && p.ycoord()!=pex1.ycoord())) pex2=p;
 } 

 lr.append(real_rectangle(pex1,pex2));

 return lr;
}

 
bool real_rectangle::do_intersect(const real_rectangle& b) const
{
 real ax0 = this->xmin();
 real ay0 = this->ymin();
 real ax1 = this->xmax();
 real ay1 = this->ymax();
 real bx0 = b.xmin();
 real by0 = b.ymin();
 real bx1 = b.xmax();
 real by1 = b.ymax();

 if (ax1 <= bx0 || bx1 <= ax0 || ay1 <= by0 || by1 <= ay0) 
 { return false; }
 return true;
}


real real_rectangle::area() const
{
 return width()*height();
}



// Bounding Boxes

real_rectangle BoundingBox(const real_point& p)
{ return real_rectangle(p,p); }

real_rectangle BoundingBox(const real_segment& s)
{ return real_rectangle(s.source(),s.target()); }

real_rectangle BoundingBox(const real_ray& r)
{ return BoundingBox(real_segment(r.source(),r.point2())); }


real_rectangle BoundingBox(const real_line& l)
{ return BoundingBox(real_segment(l.point1(),l.point2())); }


real_rectangle BoundingBox(const real_circle& c)
{ real_point  mp = c.center();
  real cx = mp.xcoord();
  real cy = mp.ycoord();
  real r  = c.radius();
  return real_rectangle(real_point(cx-r,cy-r),real_point(cx+r,cy+r));
}
 
real_rectangle BoundingBox(const real_polygon& P)
{
  const list<real_point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty polygon.");

  real x1 = L.head().xcoord(); 
  real y1 = L.head().ycoord(); 
  real x2 = x1;
  real y2 = y1;

  real_point p;
  forall(p, L)
  { real x = p.xcoord(); 
    real y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }


  return real_rectangle(real_point(x1,y1),real_point(x2,y2));
}


real_rectangle BoundingBox(const real_gen_polygon& P)
{
  const list<real_point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty gen_polygon.");

  real x1 = L.head().xcoord(); 
  real y1 = L.head().ycoord(); 
  real x2 = x1;
  real y2 = y1;

  real_point p;
  forall(p, L)
  { real x = p.xcoord(); 
    real y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }

  return real_rectangle(real_point(x1,y1),real_point(x2,y2));    
}

LEDA_END_NAMESPACE 
