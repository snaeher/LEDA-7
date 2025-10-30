/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rat_rectangle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_rectangle.h>
//#include <LEDA/numbers/expcomp.h>


//------------------------------------------------------------------------------
// iso-oriented rectangles with rational coordinates (1999)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 

atomic_counter rat_rectangle_rep::id_counter(0);

rat_rectangle_rep::rat_rectangle_rep(const rat_point& p1, const rat_point& p2)
{

 rational xlow,ylow,xhigh,yhigh;
 if (p1.xcoord()<p2.xcoord()) { xlow=p1.xcoord(); xhigh=p2.xcoord(); } else { xlow=p2.xcoord(); xhigh=p1.xcoord(); }
 if (p1.ycoord()<p2.ycoord()) { ylow=p1.ycoord(); yhigh=p2.ycoord(); } else { ylow=p2.ycoord(); yhigh=p1.ycoord(); }

 xlow.normalize(); ylow.normalize();
 xhigh.normalize(); yhigh.normalize();

 p_ll=rat_point(xlow,ylow);
 p_lr=rat_point(xhigh,ylow);
 p_ur=rat_point(xhigh,yhigh);
 p_ul=rat_point(xlow,yhigh);

 id  = ++id_counter; 
}

rat_rectangle_rep::rat_rectangle_rep(const rat_point& p1, rational w, rational h)
{

 rational x,y;
 x=p1.xcoord(); y=p1.ycoord();
 x.normalize(); y.normalize();

 if (w<0) { x=x-w; w=-w; }
 if (h<0) { y=y-h; h=-h; }

 p_ll=rat_point(x,y);
 p_lr=rat_point(x+w,y);
 p_ur=rat_point(x+w,y+h);
 p_ul=rat_point(x,y+h);

 id  = ++id_counter; 
}

rat_rectangle_rep::rat_rectangle_rep()
{
 p_ll=rat_point(0,0,1);
 p_lr=rat_point(1,0,1);
 p_ur=rat_point(1,1,1);
 p_ul=rat_point(0,1,1);

 id  = ++id_counter; 
}

rat_rectangle::rat_rectangle(const rat_point& p, const rat_point& q)
{
 PTR = new rat_rectangle_rep(p,q);
}

rat_rectangle::rat_rectangle(const rat_point& p, rational wd, rational hei)
{
 PTR = new rat_rectangle_rep(p,wd,hei);
}

rat_rectangle::rat_rectangle(const rectangle& r, int prec)
{
  point p1,p2;
  p1=r.upper_left(); p2=r.lower_right();
  rat_point p(p1, prec);
  rat_point q(p2, prec);

  PTR = new rat_rectangle_rep(p,q);
}

rat_rectangle::rat_rectangle(rational x1,rational y1,rational x2,rational y2)
{
 PTR = new rat_rectangle_rep(rat_point(x1,y1),rat_point(x2,y2));
}

rat_rectangle::rat_rectangle()
{
 PTR = new rat_rectangle_rep;
}

rectangle rat_rectangle::to_rectangle() const
{
 rat_point p1=lower_left();
 rat_point p2=upper_right();

 return rectangle(p1.to_point(),p2.to_point());
}

rat_point rat_rectangle::upper_left() const { return ptr()->p_ul; }

rat_point rat_rectangle::upper_right() const { return ptr()->p_ur; }

rat_point rat_rectangle::lower_left() const { return ptr()->p_ll; }

rat_point rat_rectangle::lower_right() const { return ptr()->p_lr; }

rat_point rat_rectangle::center() const
{ return midpoint(ptr()->p_ll, ptr()->p_ur); }

list<rat_point> rat_rectangle::vertices() const {
 list<rat_point> PL;
 PL.append(ptr()->p_ll);
 PL.append(ptr()->p_lr);
 PL.append(ptr()->p_ur);
 PL.append(ptr()->p_ul);

 return PL;
}

rational rat_rectangle::xmin() const
{ return (ptr()->p_ll).xcoord(); }
 
rational rat_rectangle::xmax() const
{ return (ptr()->p_lr).xcoord(); }

rational rat_rectangle::ymin() const
{ return (ptr()->p_ll).ycoord(); }

rational rat_rectangle::ymax() const
{ return (ptr()->p_ul).ycoord(); }

rational rat_rectangle::width() const
{ rat_point p1= ptr()->p_ll; 
  rat_point p2= ptr()->p_lr;
  return p2.xcoord()-p1.xcoord();
}

rational rat_rectangle::height() const
{ rat_point p1= ptr()->p_ll; 
  rat_point p2= ptr()->p_ul;
  return p2.ycoord()-p1.ycoord();
} 

bool rat_rectangle::is_degenerate() const
{ return  collinear(ptr()->p_ll, ptr()->p_lr, ptr()->p_ur); }

bool rat_rectangle::is_point() const
{
 return (ptr()->p_ll == ptr()->p_ur);
}

bool rat_rectangle::is_segment() const
{
 if (is_degenerate() && !is_point()) return true; else return false;
}

int rat_rectangle::cs_code(const rat_point& p) const
// calculate 4-bit position code for p 
{
 int cd=0;
 if (rat_point::cmp_y(p,ptr()->p_ur) == 1) cd= cd + 8;
 if (rat_point::cmp_y(p,ptr()->p_lr) == -1) cd= cd + 4;
 if (rat_point::cmp_x(p,ptr()->p_ur) == 1) cd= cd + 2;
 if (rat_point::cmp_x(p,ptr()->p_ul) == -1) cd= cd + 1;
 return cd;
}

bool rat_rectangle::inside(const rat_point& p) const
{
  if (rat_point::cmp_y(ptr()->p_ll,p) == -1) ; else return false;
  if (rat_point::cmp_x(ptr()->p_lr,p) == 1) ; else return false;
  if (rat_point::cmp_y(ptr()->p_ur,p) == 1) ; else return false;
  if (rat_point::cmp_x(ptr()->p_ul,p) == -1) ; else return false;
  return true;  
}


bool rat_rectangle::inside_or_contains(const rat_point& p) const
{
   return !outside(p);
}

/*
bool rat_rectangle::outside(const rat_point& p) const
{
  if (right_turn(ptr()->p_ll,ptr()->p_lr,p)) return true;
  if (right_turn(ptr()->p_lr,ptr()->p_ur,p)) return true;
  if (right_turn(ptr()->p_ur,ptr()->p_ul,p)) return true;
  if (right_turn(ptr()->p_ul,ptr()->p_ll,p)) return true;
  return false;  
}
*/

bool rat_rectangle::outside(const rat_point& p) const
{
  if (rat_point::cmp_y(ptr()->p_ll,p) == 1) return true;
  if (rat_point::cmp_x(ptr()->p_lr,p) == -1) return true;
  if (rat_point::cmp_y(ptr()->p_ur,p) == -1) return true;
  if (rat_point::cmp_x(ptr()->p_ul,p) == 1) return true;
  return false;  
}


bool rat_rectangle::contains(const rat_point& p) const
{
  int o;
  bool flag=false;
  
  o= rat_point::cmp_y(ptr()->p_ll,p);
  if (o==1) return false; 
  if (o==0) flag=true;
  o= rat_point::cmp_x(ptr()->p_lr,p);
  if (o==-1) return false; 
  if (o==0) flag=true;
  o= rat_point::cmp_y(ptr()->p_ur,p);
  if (o==-1) return false; 
  if (o==0) flag=true;
  o= rat_point::cmp_x(ptr()->p_ul,p);
  if (o==1) return false;
  if (o==0) flag=true;

  return flag;   
}

region_kind rat_rectangle::region_of(const rat_point& p) const
{
  int o;
  bool flag=false;
  
  o= rat_point::cmp_y(ptr()->p_ll,p);
  if (o==1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= rat_point::cmp_x(ptr()->p_lr,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= rat_point::cmp_y(ptr()->p_ur,p);
  if (o==-1) return UNBOUNDED_REGION; 
  if (o==0) flag=true;
  o= rat_point::cmp_x(ptr()->p_ul,p);
  if (o==1) return UNBOUNDED_REGION;
  if (o==0) flag=true;

  if (flag) return  ON_REGION;
  
  return BOUNDED_REGION; 
}

rat_rectangle rat_rectangle::include(const rat_point& p) const
{
 int code=cs_code(p);
 if (code==0) return *this; // inside or contains ...

 rational xmin_new,xmax_new,ymin_new,ymax_new;

 if (code & 2) xmax_new=p.xcoord(); else xmax_new= xmax();
 if (code & 1) xmin_new=p.xcoord(); else xmin_new= xmin();
 if (code & 8) ymax_new=p.ycoord(); else ymax_new= ymax();
 if (code & 4) ymin_new=p.ycoord(); else ymin_new= ymin();
 return rat_rectangle(rat_point(xmin_new,ymin_new),rat_point(xmax_new,ymax_new)); 
}

rat_rectangle rat_rectangle::include(const rat_rectangle& r) const
{
 rat_rectangle r1= include(r.upper_left());
 return r1.include(r.lower_right());
}

rat_rectangle rat_rectangle::translate(rational dx,rational dy) const
{
 rat_point p1= (ptr()->p_ll).translate(dx,dy);
 rat_point p2= (ptr()->p_ur).translate(dx,dy);
 
 return rat_rectangle(p1,p2);
}

rat_rectangle rat_rectangle::translate(const rat_vector& v) const
{
 return rat_rectangle(ptr()->p_ll.translate(v),ptr()->p_ur.translate(v));
}

rat_point rat_rectangle::operator[](int i) const
{
  rat_point p;
  switch (i){
  case 1: p=lower_left(); break;
  case 2: p=lower_right(); break;
  case 3: p=upper_right(); break;
  case 4: p=upper_left(); break;
  }

  return p;
}

rat_rectangle rat_rectangle::rotate90(const rat_point& p, int i) const
{
 return rat_rectangle(ptr()->p_ll.rotate90(p,i),ptr()->p_ur.rotate90(p,i)); 
}

rat_rectangle rat_rectangle::rotate90(int i) const
{
 return rat_rectangle(ptr()->p_ll.rotate90(i),ptr()->p_ur.rotate90(i));
}

rat_rectangle rat_rectangle::reflect(const rat_point& p) const
{
 return rat_rectangle(ptr()->p_ll.reflect(p),ptr()->p_ur.reflect(p));
}

bool rat_rectangle::intersect(rat_point& ps,const rat_segment& s,int c1,int c2) const
{
 rat_segment s1,s2;
 bool rt = false;

 switch (c1){ 
  case 0:
    switch(c2){ 
      case 2:
       s2=rat_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;        
      case 4:
       s2=rat_segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       break;
      case 6:
       s2=rat_segment(xmin(),ymin(),xmax(),ymin());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=rat_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 8:
       s2=rat_segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
      case 10:
       s2=rat_segment(xmin(),ymax(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       if (rt) break;
       s2=rat_segment(xmax(),ymin(),xmax(),ymax());
       rt=s.intersection(s2,ps);    
       break;
    }
    break;
  case 1:
    s2=rat_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);    
    break;
  case 4:
    s2=rat_segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s2,ps);
    break;
  case 5:
    s1=rat_segment(xmin(),ymin(),xmax(),ymin());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=rat_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 8:
    s2=rat_segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
  case 9:
    s1=rat_segment(xmin(),ymin(),xmin(),ymax());
    rt=s.intersection(s1,ps);
    if (rt) break;
    s2=rat_segment(xmin(),ymax(),xmax(),ymax());
    rt=s.intersection(s2,ps);
    break;
 }

 return rt;
}

bool rat_rectangle::clip(const rat_segment& t,rat_segment& inter) const
{
 int c1,c2;
 
 c1=cs_code(t.source());
 c2=cs_code(t.target());

 if (c1==0 && c2==0) { inter=t; return true; } //inside...
 if ((c1 & c2) != 0) return false;             //outside...

 rat_point ps;
 rat_segment t1;
 int tm;

 if (compare(t.source(),t.target())==-1) t1=t; else { t1=rat_segment(t.target(),t.source()); tm=c1; c1=c2; c2=tm; }

 bool w=intersect(ps,t1,c1,c2); 

 if (! w) return false; //no intersection...

 if (outside(t1.source())) clip(rat_segment(ps,t1.target()),inter);
 else clip(rat_segment(t1.source(),ps),inter); 

 return true;
}

bool rat_rectangle::clip(const rat_line& l,rat_segment& inter) const
{
 bool b1,b2,b3,b4;
 rat_point p1,p2,p3,p4,ps1,ps2;
 bool s1=false,s2=false;
 b1=l.intersection(rat_segment(lower_left(),lower_right()),p1);
 b2=l.intersection(rat_segment(lower_right(),upper_right()),p2);
 b3=l.intersection(rat_segment(upper_right(),upper_left()),p3);
 b4=l.intersection(rat_segment(upper_left(),lower_left()),p4);

 if (!b1 && !b2 && !b3 && !b4) return false;

 if (b1) { ps1=p1; s1=true; }
 if (b2) { 
  if (!s1 || ps1==p2) { ps1=p2; s1=true; }
  else {
    if (!s2 || ps2==p2) { ps2=p2; s2=true; }
  }
 } 
 if (b3) { 
  if (!s1 || ps1==p3) { ps1=p3; s1=true; }
  else {
    if (!s2 || ps2==p3) { ps2=p3; s2=true; }
  }
 } 
 if (b4) { 
  if (!s1 || ps1==p4) { ps1=p4; s1=true; }
  else {
    if (!s2 || ps2==p4) { ps2=p4; s2=true; }
  }
 } 

 if (!s2) ps2=ps1;
 inter=rat_segment(ps1,ps2);
 return true;
}

bool rat_rectangle::clip(const rat_ray& r, rat_segment& inter) const
{
 bool b1,b2,b3,b4;
 rat_point p1,p2,p3,p4,ps1,ps2;
 bool s1=false,s2=false;
 b1=r.intersection(rat_segment(lower_left(),lower_right()),p1);
 b2=r.intersection(rat_segment(lower_right(),upper_right()),p2);
 b3=r.intersection(rat_segment(upper_right(),upper_left()),p3);
 b4=r.intersection(rat_segment(upper_left(),lower_left()),p4);

 if (!b1 && !b2 && !b3 && !b4) return false;

 if (b1) { ps1=p1; s1=true; }
 if (b2) { 
  if (!s1 || ps1==p2) { ps1=p2; s1=true; }
  else {
    if (!s2 || ps2==p2) { ps2=p2; s2=true; }
  }
 } 
 if (b3) { 
  if (!s1 || ps1==p3) { ps1=p3; s1=true; }
  else {
    if (!s2 || ps2==p3) { ps2=p3; s2=true; }
  }
 } 
 if (b4) { 
  if (!s1 || ps1==p4) { ps1=p4; s1=true; }
  else {
    if (!s2 || ps2==p4) { ps2=p4; s2=true; }
  }
 } 

 if (!s2 && inside(r.source())) ps2=r.source();
 if (!s2 && !inside(r.source())) ps2=ps1;

 inter=rat_segment(ps1,ps2);
 return true;
}

bool rat_rectangle::difference(const rat_rectangle& b, list<rat_rectangle>& L) const
{
 if ( (rat_point::cmp_x(upper_right(),b.lower_left()) != 1) ||
      (rat_point::cmp_x(b.upper_right(),lower_left()) != 1) ||
      (rat_point::cmp_y(upper_right(),b.lower_left()) != 1) ||
      (rat_point::cmp_y(b.upper_right(),lower_left()) != 1) ) 
 {
  L.append(*this); 
  return true; 
 }

 rational ax0 = this->xmin();
 rational ay0 = this->ymin();
 rational ax1 = this->xmax();
 rational ay1 = this->ymax();
 rational bx0 = b.xmin();
 rational by0 = b.ymin();
 rational bx1 = b.xmax();
 rational by1 = b.ymax(); 

 if (rat_point::cmp_y(b.upper_right(), upper_right()) == -1) 
      L.append(rat_rectangle(ax0,by1,ax1,ay1));
      
 if (rat_point::cmp_y(lower_left(), b.lower_left()) == -1) 
      L.append(rat_rectangle(ax0,ay0,ax1,by0));

 rational y0 = (by0 < ay0) ? ay0 : by0;
 rational y1 = (ay1 < by1) ? ay1 : by1;

 if (rat_point::cmp_x(lower_left(),b.lower_left()) == -1) L.append(rat_rectangle(ax0,y0,bx0,y1)); 
 if (rat_point::cmp_x(b.upper_right(),upper_right()) == -1) L.append(rat_rectangle(bx1,y0,ax1,y1));

 return !L.empty();
}

list<rat_point> rat_rectangle::intersection(const rat_segment& s) const
{
  rat_point p1,p2;
  list<rat_point> rt;
  p1=s.source(); p2=s.target();
  bool b1=inside(p1);
  bool b2=inside(p2);

  if (b1 && b2) return rt; // no intersection

  rat_point ph1,ph2,ph3,ph4;
  b1=s.intersection(rat_segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
  b1=s.intersection(rat_segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
  b1=s.intersection(rat_segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
  b1=s.intersection(rat_segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);

  return rt;  
}

list<rat_point> rat_rectangle::intersection(const rat_line& l) const
{
  list<rat_point> rt;
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
   rat_point ph1,ph2,ph3,ph4;
   b1=l.intersection(rat_segment(upper_left(),lower_left()),ph1); if (b1) rt.append(ph1);
   b1=l.intersection(rat_segment(lower_left(),lower_right()),ph2); if (b1 && ph1!=ph2) rt.append(ph2);
   b1=l.intersection(rat_segment(lower_right(),upper_right()),ph3); if (b1 && ph3!=ph2) rt.append(ph3);
   b1=l.intersection(rat_segment(upper_right(),upper_left()),ph4); if(b1 && ph4!=ph3 && ph4!=ph1) rt.append(ph4);   
  }
  return rt;
}

list<rat_rectangle> rat_rectangle::intersection(const rat_rectangle& r) const
{
 list<rat_rectangle> lr;
 rat_segment s1,s2,s3,s4;
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

 s1=rat_segment(r.upper_left(),r.lower_left());
 s2=rat_segment(r.lower_left(),r.lower_right());
 s3=rat_segment(r.lower_right(),r.upper_right());
 s4=rat_segment(r.upper_right(),r.upper_left());

 list<rat_point> all,l1,l2,l3,l4;
 l1=intersection(s1); l2=intersection(s2);  l3=intersection(s3);  l4=intersection(s4);

 all.conc(l1); all.conc(l2); all.conc(l3); all.conc(l4);
 if (b1)  all.append(r.lower_left());
 if (b2)  all.append(r.lower_right()); 
 if (b3)  all.append(r.upper_right()); 
 if (b4)  all.append(r.upper_left()); 

 if (all.empty()) return lr;
 rat_point p, pex1,pex2;
 pex1=all.head(); pex2=all.head();

 forall(p,all){
  if (p.xcoord() < pex1.xcoord() || (p.xcoord() == pex1.xcoord() && p.ycoord()!=pex2.ycoord())) pex1=p;
  if (p.xcoord() > pex2.xcoord() || (p.xcoord() == pex2.xcoord() && p.ycoord()!=pex1.ycoord())) pex2=p;
 } 

 lr.append(rat_rectangle(pex1,pex2));

 return lr;
}

bool rat_rectangle::do_intersect(const rat_rectangle& b) const
{
 if ( (rat_point::cmp_x(upper_right(),b.lower_left()) != 1) ||
      (rat_point::cmp_x(b.upper_right(),lower_left()) != 1) ||
      (rat_point::cmp_y(upper_right(),b.lower_left()) != 1) ||
      (rat_point::cmp_y(b.upper_right(),lower_left()) != 1) )
 { return false; }
 return true;
}

rational rat_rectangle::area() const
{
 return width()*height();
}


// Bounding Boxes

rat_rectangle BoundingBox(const rat_point& p)
{ return rat_rectangle(p,p); }

rat_rectangle BoundingBox(const rat_segment& s)
{ return rat_rectangle(s.source(),s.target()); }

rat_rectangle BoundingBox(const rat_ray& r)
{ return BoundingBox(rat_segment(r.source(),r.point2())); }

rat_rectangle BoundingBox(const rat_line& l)
{ return BoundingBox(rat_segment(l.point1(),l.point2())); }
 

rat_rectangle BoundingBox(const rat_polygon& P)
{
  const list<rat_point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty gen_polygon.");

  rational x1 = L.head().xcoord(); 
  rational y1 = L.head().ycoord(); 
  rational x2 = x1;
  rational y2 = y1;

  rat_point p;
  forall(p, L)
  { rational x = p.xcoord(); 
    rational y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }

  return rat_rectangle(rat_point(x1,y1),rat_point(x2,y2));    
}



rat_rectangle BoundingBox(const rat_gen_polygon& P)
{
  const list<rat_point>&  L = P.vertices();

  if (L.empty()) 
    LEDA_EXCEPTION(1,"BoundingBox: empty gen_polygon.");

  rational x1 = L.head().xcoord(); 
  rational y1 = L.head().ycoord(); 
  rational x2 = x1;
  rational y2 = y1;

  rat_point p;
  forall(p, L)
  { rational x = p.xcoord(); 
    rational y = p.ycoord();
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
   }

  return rat_rectangle(rat_point(x1,y1),rat_point(x2,y2));    
}


LEDA_END_NAMESPACE 

