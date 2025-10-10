/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_circle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/real_circle.h>
#include <LEDA/geo/rat_circle.h>


//------------------------------------------------------------------------------
// real circles
//
// S. Thiel (2004)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


real_circle_rep::real_circle_rep(const real_point& p1, const real_point& p2, const real_point& p3) : 
a(p1), b(p2), c(p3), cp(0), rp(0), first_side_of(true)
{ 
  orient = orientation(p1,p2,p3);

  if (orient == 0 && (p1 == p2 || p1 == p3 || p2 == p3) )
  { if (p1 == p2 && p1 == p3)
      { cp = new real_point(p1);
        rp = new real(0);
       }
    else
       LEDA_EXCEPTION(1,"real_circle::real_circle: non-admissable triple");
  }
} 
                              

real_circle_rep::~real_circle_rep() 
{ if (cp) delete cp;
  if (rp) delete rp;
 }



real_circle::real_circle(const real_point& a, const real_point& b, const real_point& c)
{ PTR = new real_circle_rep(a,b,c); }

real_circle::real_circle(const real_point& a, const real_point& b0)
{ // center a and b0 on circle
  real_point b1 = b0.rotate90(a);
  real_point b2 = b1.rotate90(a);
  PTR = new real_circle_rep(b0,b1,b2);
  if (ptr()->cp == 0) ptr()->cp = new real_point(a);
  if (ptr()->rp == 0) ptr()->rp = new real(a.distance(b0));
 }

real_circle::real_circle(const real_point& a) 
{ PTR = new real_circle_rep(a,a,a); }


real_circle::real_circle() 
{ real_point p(0,0);
  PTR = new real_circle_rep(p,p,p);
 }

real_circle::real_circle(const real_point& m, real r)       
{ real_point a(m.xcoord(),  m.ycoord()+r); 
  real_point b(m.xcoord(),  m.ycoord()-r); 
  real_point c(m.xcoord()+r,m.ycoord()); 
  PTR = new real_circle_rep(a,b,c); 
  if (ptr()->cp == 0) ptr()->cp = new real_point(m);
  if (ptr()->rp == 0) ptr()->rp = new real(r);
}

real_circle::real_circle(real x, real y, real r)  
{ PTR = new real_circle_rep(real_point(x,y+r),real_point(x,y-r),real_point(x+r,y)); 
  if (ptr()->cp == 0) ptr()->cp = new real_point(x,y);
  if (ptr()->rp == 0) ptr()->rp = new real(r);
}

real_circle::real_circle(const rat_circle& c)
{ PTR = new real_circle_rep(real_point(c.point1()),
						    real_point(c.point2()), 
							real_point(c.point3()));
  if (c.orientation() != 0) ptr()->cp = new real_point(c.center());
}

/*
real_point real_circle::center() const
{ if (ptr()->cp == 0)
  { if (collinear(ptr()->a,ptr()->b,ptr()->c)) 
       LEDA_EXCEPTION(1,"real_circle::center(): points are collinear.");
    real_line l1 = p_bisector(ptr()->a,ptr()->b);
    real_line l2 = p_bisector(ptr()->b,ptr()->c);
    real_point m;
    l1.intersection(l2,m);
    ptr()->cp = new real_point(m);
   }
  return *(ptr()->cp);
 }
*/

real_point real_circle::center() const
{ if (ptr()->cp == 0)
  {
    real_point p = ptr()->a;
    real_point q = ptr()->b;
    real_point r = ptr()->c;
 
    real xp = p.xcoord(), yp = p.ycoord();
    real xq = q.xcoord(), yq = q.ycoord();
    real xr = r.xcoord(), yr = r.ycoord();
 
    real cx  =    (xp*xp + yp*yp)*(yq - yr)
                 +  (xq*xq + yq*yq)*(yr - yp)
                 +  (xr*xr + yr*yr)*(yp - yq);
 
    real cy  = - (xp*xp + yp*yp)*(xq - xr)
                 - (xq*xq + yq*yq)*(xr - xp)
                 - (xr*xr + yr*yr)*(xp - xq);

    //real cw  =  2*( (xq*yr-xr*yq) + (xr*yp-xp*yr) + (xp*yq-xq*yp) );

    real cw  =  2*( (xq - xp)*(yr - yp) - (xr - xp)*(yq - yp) );

    if ( cw == 0 ) 
       LEDA_EXCEPTION(1,"real_circle::center(): points are collinear.");
    

    ptr()->cp = new real_point(cx/cw,cy/cw);
  
  }
  return *(ptr()->cp);
}




real real_circle::sqr_radius() const
{ return ptr()->a.sqr_dist(center()); }

real real_circle::radius() const
{ if (ptr()->rp == 0)
    ptr()->rp = new real(center().distance(ptr()->a));
 return *(ptr()->rp);
}

  

bool real_circle::operator==(const real_circle& c)  const
{ if (!contains(c.ptr()->a)) return false;
  if (!contains(c.ptr()->b)) return false;
  if (!contains(c.ptr()->c)) return false;
  return true;
}


real real_circle::distance(const real_point& p) const
{ real d = p.distance(center()) - radius();
  return d;
}

real real_circle::sqr_dist(const real_point& p) const
{ real d = distance(p);
  return d*d;
}



real real_circle::distance(const real_line& l) const
{ real d = l.distance(center()) - radius();
  if (d < 0) d = 0;
  return d;
 }

real real_circle::distance(const real_circle& c) const
{ real d = center().distance(c.center()) - radius() - c.radius();
  if (d < 0) d = 0;
  return d;
}

int real_circle::side_of(const real_point& p) const
{ 
  real ax = ptr()->a.xcoord();
  real ay = ptr()->a.ycoord();

  // compute D1 D2 D3 (if not done before)

  if (ptr()->first_side_of) 
  { real bx = ptr()->b.xcoord() - ax;
    real by = ptr()->b.ycoord() - ay;
    real bw = bx*bx + by*by;

    real cx = ptr()->c.xcoord() - ax;
    real cy = ptr()->c.ycoord() - ay;
    real cw = cx*cx + cy*cy;

    ptr()->D1 = cy*bw - by*cw;
    ptr()->D2 = bx*cw - cx*bw;
    ptr()->D3 = by*cx - bx*cy;

    ptr()->first_side_of = false;
   }

   real px = p.xcoord() - ax;
   real py = p.ycoord() - ay;
   real pw = px*px + py*py;
 
   real D  = px*ptr()->D1  + py*ptr()->D2 + pw*ptr()->D3;

   return sign(D);
}



bool real_circle::outside(const real_point& p) const 
{ return (ptr()->orient * side_of(p)) < 0; }


bool real_circle::inside(const real_point& p) const 
{ return (ptr()->orient * side_of(p)) > 0; }


bool real_circle::contains(const real_point& p) const 
{ return side_of(p) == 0; }

/*
real_circle real_circle::translate_by_angle(real alpha, real d) const
{ real_point a1 = ptr()->a.translate_by_angle(alpha,d);
  real_point b1 = ptr()->b.translate_by_angle(alpha,d);
  real_point c1 = ptr()->c.translate_by_angle(alpha,d);
  return real_circle(a1,b1,c1);
 }
*/

real_circle real_circle::translate(real dx, real dy) const
{ real_point a1 = ptr()->a.translate(dx,dy);
  real_point b1 = ptr()->b.translate(dx,dy);
  real_point c1 = ptr()->c.translate(dx,dy);
  return real_circle(a1,b1,c1);
}


real_circle real_circle::translate(const real_vector& v) const
{ real_point a1 = ptr()->a.translate(v);
  real_point b1 = ptr()->b.translate(v);
  real_point c1 = ptr()->c.translate(v);
  return real_circle(a1,b1,c1);
 }

/*
real_circle  real_circle::rotate(const real_point& o, real alpha) const
{ real_point a1 = ptr()->a.rotate(o,alpha);
  real_point b1 = ptr()->b.rotate(o,alpha);
  real_point c1 = ptr()->c.rotate(o,alpha);
  return real_circle(a1,b1,c1);
 }
*/

/*
real_circle  real_circle::rotate(real alpha)  const
{ real_point o(0,0);
  real_point a1 = ptr()->a.rotate(o,alpha);
  real_point b1 = ptr()->b.rotate(o,alpha);
  real_point c1 = ptr()->c.rotate(o,alpha);
  return real_circle(a1,b1,c1);
}
*/

real_circle  real_circle::rotate90(const real_point& o, int i)  const
{ real_point a1 = ptr()->a.rotate90(o,i);
  real_point b1 = ptr()->b.rotate90(o,i);
  real_point c1 = ptr()->c.rotate90(o,i);
  return real_circle(a1,b1,c1);
}


real_circle  real_circle::reflect(const real_point& p, const real_point& q)  const
{ real_point a1 = ptr()->a.reflect(p,q);
  real_point b1 = ptr()->b.reflect(p,q);
  real_point c1 = ptr()->c.reflect(p,q);
  return real_circle(a1,b1,c1);
}

real_circle  real_circle::reflect(const real_point& p)  const
{ real_point a1 = ptr()->a.reflect(p);
  real_point b1 = ptr()->b.reflect(p);
  real_point c1 = ptr()->c.reflect(p);
  return real_circle(a1,b1,c1);
}


list<real_point> real_circle::intersection(const real_line& l) const
{ list<real_point> result;
  two_tuple<real_point,real_point> inter;
  int num_inter = intersection(l, inter);
  if (num_inter >= 1) result.append(inter.first());
  if (num_inter >= 2) result.append(inter.second());
  return result;
}

// reimplemented by S. Thiel: use only one root!
int real_circle::intersection(const real_line& l, two_tuple<real_point,real_point>& inter) const
{
  if (is_trivial()) {
    inter.first() = point1();
	return l.contains(point1()) ? 1 : 0;
  }
  else if (orientation() == 0) {
    real_line support(point1(), point3());
	return support.intersection(l, inter.first()) ? 1 : 0;
  }

  real cx = center().xcoord();
  real cy = center().ycoord();
  real r_sqr = sqr_radius();

  if (l.is_vertical()) {
    real x = l.point1().xcoord(); 
		// get the x-coord of l, this is also the x-coord of any intersection

	real dy_sqr = r_sqr - sqr(x - cx); // (dx)^2 + (dy)^2 = r^2

	int s = sign(dy_sqr);
	if (s == 0) { // l is tangent to *this
	  inter.first() = real_point(x,cy); return 1;
	}
	else if (s > 0) {
	  real dy = sqrt(dy_sqr);
	  real_point p1(x, cy - dy), p2(x, cy + dy);
      // two intersection, we need to order them in the direction of l
	  if (l.point1().ycoord() <= l.point2().ycoord()) {
	    inter.first() = p1; inter.second() = p2; return 2;
	  }
	  else {
	    inter.first() = p2; inter.second() = p1; return 2;
	  }
	}
  }
  else { // l is not vertical => slope exists
	// we have (1) y = m * x + c and (2) (x-cx)^2 + (y-cy)^2 = r^2
	// inserting (1) in (2) yields:
	// (m^2+1) x^2 + 2 * (m*(c-cy)-cx) * x + cx^2 + (c-cy)^2 - r^2 = 0
	//    A    x^2 + 2 *     B         * x +          C            = 0
	// => | x + B/A | = sqrt( (B/A)^2 - C/A )

	real m = l.slope(), c = l.y_abs();
	real A = sqr(m)+1, B = m * (c-cy) - cx, C = sqr(cx) + sqr(c-cy) - r_sqr;
	real B_div_A = B/A;
	real discrim = sqr(B_div_A) - C/A;

	int s = sign(discrim);
	if (s == 0) {
	  real x = -B_div_A; real y = m * x + c;
	  inter.first() = real_point(x,y); return 1;
	}
	else if (s > 0) {
	  real root = sqrt(discrim);
	  real x1 = -B_div_A - root; real y1 = m * x1 + c; real_point p1(x1,y1);
	  real x2 = -B_div_A + root; real y2 = m * x2 + c; real_point p2(x2,y2);
	  if (l.point1().xcoord() <= l.point2().xcoord()) {
	    inter.first() = p1; inter.second() = p2; return 2;
	  }
	  else {
	    inter.first() = p2; inter.second() = p1; return 2;
	  }
	}
  }

  return 0;
}

int intersection(const rat_circle& circ, const rat_line& l, two_tuple<real_point,real_point>& inter)
{
  if (circ.is_trivial()) {
	  if (l.contains(circ.point1())) {
	    inter.first() = circ.point1(); return 1;
	  }
	  return 0;
  }
  else if (circ.orientation() == 0) {
    rat_line support(circ.point1(), circ.point3());
	rat_point rat_inter;
	if (support.intersection(l, rat_inter)) {
		inter.first() = rat_inter; return 1;
	}
	return 0;
  }

  rational cx = circ.center().xcoord();
  rational cy = circ.center().ycoord();
  rational r_sqr = circ.sqr_radius();

  if (l.is_vertical()) {
    rational x = l.point1().xcoord(); 
		// get the x-coord of l, this is also the x-coord of any intersection

	rational dy_sqr = r_sqr - sqr(x - cx); // (dx)^2 + (dy)^2 = r^2

	int s = sign(dy_sqr);
	if (s == 0) { // l is tangent to *this
	  inter.first() = real_point(x,cy); return 1;
	}
	else if (s > 0) {
	  real dy = sqrt(dy_sqr);
	  real_point p1(x, cy - dy), p2(x, cy + dy);
      // two intersection, we need to order them in the direction of l
	  if (l.point1().ycoord() <= l.point2().ycoord()) {
	    inter.first() = p1; inter.second() = p2; return 2;
	  }
	  else {
	    inter.first() = p2; inter.second() = p1; return 2;
	  }
	}
  }
  else { // l is not vertical => slope exists
	// we have (1) y = m * x + c and (2) (x-cx)^2 + (y-cy)^2 = r^2
	// inserting (1) in (2) yields:
	// (m^2+1) x^2 + 2 * (m*(c-cy)-cx) * x + cx^2 + (c-cy)^2 - r^2 = 0
	//    A    x^2 + 2 *     B         * x +          C            = 0
	// => | x + B/A | = sqrt( (B/A)^2 - C/A )

	rational m = l.slope(), c = l.y_abs();
	rational A = sqr(m)+1, B = m * (c-cy) - cx, C = sqr(cx) + sqr(c-cy) - r_sqr;
	rational B_div_A = B/A;
	rational discrim = sqr(B_div_A) - C/A;

	int s = sign(discrim);
	if (s == 0) {
	  rational x = -B_div_A; rational y = m * x + c;
	  inter.first() = real_point(x,y); return 1;
	}
	else if (s > 0) {
	  real root = sqrt(discrim);
	  real x1 = -B_div_A - root; real y1 = m * x1 + c; real_point p1(x1,y1);
	  real x2 = -B_div_A + root; real y2 = m * x2 + c; real_point p2(x2,y2);
	  if (l.point1().xcoord() <= l.point2().xcoord()) {
	    inter.first() = p1; inter.second() = p2; return 2;
	  }
	  else {
	    inter.first() = p2; inter.second() = p1; return 2;
	  }
	}
  }

  return 0;
}

list<real_point> intersection(const rat_circle& c, const rat_line& l)
{ list<real_point> result;
  two_tuple<real_point,real_point> inter;
  int num_inter = intersection(c, l, inter);
  if (num_inter >= 1) result.append(inter.first());
  if (num_inter >= 2) result.append(inter.second());
  return result;
}

list<real_point> real_circle::intersection(const real_segment& s) const
{ list<real_point> result,L;

  L = intersection(real_line(s));

  real_point p;
  real  d  = s.length();

  forall(p,L)
  { real d1 = s.ptr()->start.distance(p);
    real d2 = s.ptr()->end.distance(p);
    if (d1 <= d && d2 <= d) result.append(p);
   }

  return result;
}

// reimplemented to avoid the use of trigonometric functions
list<real_point> real_circle::intersection(const real_circle& C2) const
{
  real_line rad_axis;
  if (radical_axis(*this, C2, rad_axis)) {
	return intersection(rad_axis);
  }
  else {
	// same center or one circle is degenerate
    return list<real_point>();
  }
}


real_segment real_circle::left_tangent(const real_point& p) const
{ 
  switch(side_of(p) * orientation()) {	
  case +1: LEDA_EXCEPTION(1,"left_tangent:: point inside circle"); return real_segment();
  case  0: return real_segment(p,p); // for compatibility with double kernel
  default: { // p outside of circle
			real d_sqr = p.sqr_dist(center());
			real l     = sqrt(d_sqr - sqr(radius()));

			// optimizations are possible below
			real_circle C2(p, l); // circle with center p and radius l
			list<real_point> touches = intersection(C2);
			real_point touch = touches.pop();
			if (right_turn(p, center(), touch)) touch = touches.pop();
			return real_segment(p, touch);
		   }
  }
}

real_segment real_circle::right_tangent(const real_point& p) const
{ 
  switch(side_of(p) * orientation()) {	
  case +1: LEDA_EXCEPTION(1,"left_tangent:: point inside circle"); return real_segment();
  case  0: return real_segment(p,p); // for compatibility with double kernel
  default: { // p outside of circle
			real d_sqr = p.sqr_dist(center());
			real l     = sqrt(d_sqr - sqr(radius()));

			// optimizations are possible below
			real_circle C2(p, l); // circle with center p and radius l
			list<real_point> touches = intersection(C2);
			real_point touch = touches.pop();
			if (left_turn(p, center(), touch)) touch = touches.pop();
			return real_segment(p, touch);
		   }
  }
}

real_point real_circle::point_on_circle(double phi,double) const
{ // compute cos_phi and sin_phi s.th. sin_phi^2 + cos_phi^2 = 1 holds
  real cos_phi = cos(phi);
  if (cos_phi > 1) cos_phi = 1; else if (cos_phi < -1) cos_phi = -1;
  real sin_phi = sqrt(1 - sqr(cos_phi));
  if (sin(phi) < 0) sin_phi *= -1;

  real_point  c = center();
  real r = radius();
  real dx = cos_phi * r;
  real dy = sin_phi * r;
  return real_point(c.xcoord()+dx,c.ycoord()+dy);
}

bool
radical_axis(const real_circle& c1, const real_circle& c2, real_line& rad_axis)
// for comments see _rat_circle.c
{
	if (c1.is_degenerate() || c2.is_degenerate()) return false;

	real x1 = c1.center().xcoord(), y1 = c1.center().ycoord(), sqr_r1 = c1.sqr_radius();
	real x2 = c2.center().xcoord(), y2 = c2.center().ycoord(), sqr_r2 = c2.sqr_radius();

	real a = 2 * (x2-x1);
	real b = 2 * (y2-y1);
	real c = x1 * x1 - x2 * x2 + y1 * y1 - y2 * y2 - sqr_r1 + sqr_r2;

	if (b != 0) {
		real_point p(0, -c/b);
		real_point q(1, -(a+c)/b);
		rad_axis = real_line(p,q);
		return true;
	}
	else if (a != 0) {
		// b == 0 => vertical line
		real x = -c/a;
		real_point p(x, 0);
		real_point q(x, 1);
		rad_axis = real_line(p,q);
		return true;
	}
	else {
		return false;
	}
}

istream& operator>>(istream& in, real_circle& c) 
{ real_point p,q,r;
  in >> p >> q >> r;
  c = real_circle(p,q,r);
  return in;
}

LEDA_END_NAMESPACE 
