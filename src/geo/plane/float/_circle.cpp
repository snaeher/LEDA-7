/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _circle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/circle.h>


//------------------------------------------------------------------------------
// circles
//
// S. N"aher (1996)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


circle_rep::circle_rep(const point& p1, const point& p2, const point& p3) : 
a(p1), b(p2), c(p3), cp(0), rp(0), first_side_of(true)
{ 
  orient = orientation(p1,p2,p3);

  if (orient == 0 && (p1 == p2 || p1 == p3 || p2 == p3) )
  { if (p1 == p2 && p1 == p3)
      { cp = new point(p1);
        rp = new double(0);
       }
    else
       LEDA_EXCEPTION(1,"circle::circle: non-admissable triple");
  }
} 
                              

circle_rep::~circle_rep() 
{ if (cp) delete cp;
  if (rp) delete rp;
 }



circle::circle(const point& a, const point& b, const point& c)
{ PTR = new circle_rep(a,b,c); }

circle::circle(const point& a, const point& b0)
{ // center a and b0 on circle
  point b1 = b0.rotate90(a);
  point b2 = b1.rotate90(a);
  PTR = new circle_rep(b0,b1,b2);
  if (ptr()->cp == 0) ptr()->cp = new point(a);
  if (ptr()->rp == 0) ptr()->rp = new double(a.distance(b0));
 }

circle::circle(const point& a) 
{ PTR = new circle_rep(a,a,a); }


circle::circle() 
{ point p(0,0);
  PTR = new circle_rep(p,p,p);
 }

circle::circle(const point& m, double r)       
{ point a(m.xcoord(),  m.ycoord()+r); 
  point b(m.xcoord(),  m.ycoord()-r); 
  point c(m.xcoord()+r,m.ycoord()); 
  PTR = new circle_rep(a,b,c); 
  if (ptr()->cp == 0) ptr()->cp = new point(m);
  if (ptr()->rp == 0) ptr()->rp = new double(r);
}

circle::circle(double x, double y, double r)  
{ PTR = new circle_rep(point(x,y+r),point(x-r,y),point(x,y-r)); 
  if (ptr()->cp == 0) ptr()->cp = new point(x,y);
  if (ptr()->rp == 0) ptr()->rp = new double(r);
}



/*
point circle::center() const
{ if (ptr()->cp == 0)
  { if (collinear(ptr()->a,ptr()->b,ptr()->c)) 
       LEDA_EXCEPTION(1,"circle::center(): points are collinear.");
    line l1 = p_bisector(ptr()->a,ptr()->b);
    line l2 = p_bisector(ptr()->b,ptr()->c);
    point m;
    l1.intersection(l2,m);
    ptr()->cp = new point(m);
   }
  return *(ptr()->cp);
 }
*/

point circle::center() const
{ if (ptr()->cp == 0)
  {
    point p = ptr()->a;
    point q = ptr()->b;
    point r = ptr()->c;
 
    double xp = p.xcoord(), yp = p.ycoord();
    double xq = q.xcoord(), yq = q.ycoord();
    double xr = r.xcoord(), yr = r.ycoord();
 
    double cx  =    (xp*xp + yp*yp)*(yq - yr)
                 +  (xq*xq + yq*yq)*(yr - yp)
                 +  (xr*xr + yr*yr)*(yp - yq);
 
    double cy  = - (xp*xp + yp*yp)*(xq - xr)
                 - (xq*xq + yq*yq)*(xr - xp)
                 - (xr*xr + yr*yr)*(xp - xq);

    //double cw  =  2*( (xq*yr-xr*yq) + (xr*yp-xp*yr) + (xp*yq-xq*yp) );

    double cw  =  2*( (xq - xp)*(yr - yp) - (xr - xp)*(yq - yp) );

    if ( cw == 0 ) 
       LEDA_EXCEPTION(1,"circle::center(): points are collinear.");
    

    ptr()->cp = new point(cx/cw,cy/cw);
  
  }
  return *(ptr()->cp);
}




double circle::sqr_radius() const
{ return ptr()->a.sqr_dist(center()); }

double circle::radius() const
{ if (ptr()->rp == 0)
    ptr()->rp = new double(center().distance(ptr()->a));
 return *(ptr()->rp);
}

  

bool circle::operator==(const circle& c)  const
{ if (!contains(c.ptr()->a)) return false;
  if (!contains(c.ptr()->b)) return false;
  if (!contains(c.ptr()->c)) return false;
  return true;
}


double circle::distance(const point& p) const
{ double d = p.distance(center()) - radius();
  return d;
}

double circle::sqr_dist(const point& p) const
{ double d = distance(p);
  return d*d;
}



double circle::distance(const line& l) const
{ double d = l.distance(center()) - radius();
  if (d < 0) d = 0;
  return d;
 }

double circle::distance(const circle& c) const
{ double d = center().distance(c.center()) - radius() - c.radius();
  if (d < 0) d = 0;
  return d;
}

int circle::side_of(const point& p) const
{ 
  double ax = ptr()->a.xcoord();
  double ay = ptr()->a.ycoord();

  // compute D1 D2 D3 (if not done before)

  if (ptr()->first_side_of) 
  { double bx = ptr()->b.xcoord() - ax;
    double by = ptr()->b.ycoord() - ay;
    double bw = bx*bx + by*by;

    double cx = ptr()->c.xcoord() - ax;
    double cy = ptr()->c.ycoord() - ay;
    double cw = cx*cx + cy*cy;

    ptr()->D1 = cy*bw - by*cw;
    ptr()->D2 = bx*cw - cx*bw;
    ptr()->D3 = by*cx - bx*cy;

    ptr()->first_side_of = false;
   }

   double px = p.xcoord() - ax;
   double py = p.ycoord() - ay;
   double pw = px*px + py*py;
 
   double D  = px*ptr()->D1  + py*ptr()->D2 + pw*ptr()->D3;

   if (D != 0)
      return (D > 0) ? 1 : -1;
   else
      return 0;
}



bool circle::outside(const point& p) const 
{ return (ptr()->orient * side_of(p)) < 0; }


bool circle::inside(const point& p) const 
{ return (ptr()->orient * side_of(p)) > 0; }


bool circle::contains(const point& p) const 
{ return side_of(p) == 0; }


circle circle::translate_by_angle(double alpha, double d) const
{ point a1 = ptr()->a.translate_by_angle(alpha,d);
  point b1 = ptr()->b.translate_by_angle(alpha,d);
  point c1 = ptr()->c.translate_by_angle(alpha,d);
  return circle(a1,b1,c1);
 }


circle circle::translate(double dx, double dy) const
{ point a1 = ptr()->a.translate(dx,dy);
  point b1 = ptr()->b.translate(dx,dy);
  point c1 = ptr()->c.translate(dx,dy);
  return circle(a1,b1,c1);
}


circle circle::translate(const vector& v) const
{ point a1 = ptr()->a.translate(v);
  point b1 = ptr()->b.translate(v);
  point c1 = ptr()->c.translate(v);
  return circle(a1,b1,c1);
 }

circle  circle::rotate(const point& o, double alpha) const
{ point a1 = ptr()->a.rotate(o,alpha);
  point b1 = ptr()->b.rotate(o,alpha);
  point c1 = ptr()->c.rotate(o,alpha);
  return circle(a1,b1,c1);
 }

circle  circle::rotate(double alpha)  const
{ point o(0,0);
  point a1 = ptr()->a.rotate(o,alpha);
  point b1 = ptr()->b.rotate(o,alpha);
  point c1 = ptr()->c.rotate(o,alpha);
  return circle(a1,b1,c1);
}


circle  circle::rotate90(const point& o, int i)  const
{ point a1 = ptr()->a.rotate90(o,i);
  point b1 = ptr()->b.rotate90(o,i);
  point c1 = ptr()->c.rotate90(o,i);
  return circle(a1,b1,c1);
}


circle  circle::reflect(const point& p, const point& q)  const
{ point a1 = ptr()->a.reflect(p,q);
  point b1 = ptr()->b.reflect(p,q);
  point c1 = ptr()->c.reflect(p,q);
  return circle(a1,b1,c1);
}

circle  circle::reflect(const point& p)  const
{ point a1 = ptr()->a.reflect(p);
  point b1 = ptr()->b.reflect(p);
  point c1 = ptr()->c.reflect(p);
  return circle(a1,b1,c1);
}



list<point> circle::intersection(const line& l) const
{ list<point> result;

  segment s = l.perpendicular(center());
  double  d = s.length();
  double  r = radius();

  if (d==r) result.append(s.end());
  
  if (d < r)
  { segment sl = l.ptr()->seg;
    double t = sqrt(r*r - d*d)/sl.length();
    double dx = t*sl.dx();
    double dy = t*sl.dy();
    result.append(s.end().translate(vector(-dx,-dy)));
    result.append(s.end().translate(vector(dx,dy)));
  }

  return result;
}


list<point> circle::intersection(const segment& s) const
{ list<point> result,L;

  L = intersection(line(s));

  point p;
  double  d  = s.length();

  forall(p,L)
  { double d1 = s.ptr()->start.distance(p);
    double d2 = s.ptr()->end.distance(p);
    if (d1 <= d && d2 <= d) result.append(p);
   }

  return result;
}

list<point> circle::intersection(const circle& c) const
{ list<point> result;
  segment s(center(), c.center());
  double d  = s.length();
  double r1 = radius();
  double r2 = c.radius();

  if (d > (r1+r2) || (d+r2) < r1 || (d+r1) < r2 || d == 0) return result;


  double x = (d*d + r1*r1 - r2*r2)/(2*d);
  double alpha = acos(x/r1);
  double beta  = s.angle() + alpha;
  double gamma = s.angle() - alpha;

  result.append(center().translate_by_angle(beta,r1));
  if (alpha!=0) result.append(center().translate_by_angle(gamma,r1));

  return result;
 }


segment circle::left_tangent(const point& p) const
{ if (inside(p)) LEDA_EXCEPTION(1,"left_tangent:: point inside circle");
  segment s(p,center());
  double d = s.length();
  double alpha = asin(radius()/d) + s.angle();
  point touch = p.translate_by_angle(alpha,sqrt(d*d - radius()*radius()));
  return segment(p,touch);
}

segment circle::right_tangent(const point& p) const
{ if (inside(p)) LEDA_EXCEPTION(1,"right_tangent:: point inside circle");
  segment s(p,center());
  double d = s.length();
  double alpha = s.angle() - asin(radius()/d);
  point touch = p.translate_by_angle(alpha,sqrt(d*d - radius()*radius()));
  return segment(p,touch);
}


point circle::point_on_circle(double phi,double) const
{ point  c = center();
  double r = radius();
  double dx = cos(phi) * r;
  double dy = sin(phi) * r;
  return point(c.xcoord()+dx,c.ycoord()+dy);
}


bool
radical_axis(const circle& c1, const circle& c2, line& rad_axis)
// for comments see _rat_circle.c
{
	if (c1.is_degenerate() || c2.is_degenerate()) return false;

	double x1 = c1.center().xcoord(), y1 = c1.center().ycoord(), sqr_r1 = c1.sqr_radius();
	double x2 = c2.center().xcoord(), y2 = c2.center().ycoord(), sqr_r2 = c2.sqr_radius();

	double a = 2 * (x2-x1);
	double b = 2 * (y2-y1);
	double c = x1 * x1 - x2 * x2 + y1 * y1 - y2 * y2 - sqr_r1 + sqr_r2;

	if (b != 0) {
		point p(0, -c/b);
		point q(1, -(a+c)/b);
		rad_axis = line(p,q);
		return true;
	}
	else if (a != 0) {
		// b == 0 => vertical line
		double x = -c/a;
		point p(x, 0);
		point q(x, 1);
		rad_axis = line(p,q);
		return true;
	}
	else {
		return false;
	}
}


istream& operator>>(istream& in, circle& c) 
{ point p,q,r;
  in >> p >> q >> r;
  c = circle(p,q,r);
  return in;
}

LEDA_END_NAMESPACE 

