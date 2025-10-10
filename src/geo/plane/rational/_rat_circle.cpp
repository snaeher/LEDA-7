/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _rat_circle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_circle.h>
#include <LEDA/geo/rat_line.h>
#include <LEDA/geo/circle.h>
#include <LEDA/numbers/fp.h>



//------------------------------------------------------------------------------
// rat_circles
//------------------------------------------------------------------------------


#define FABS(x) fp::clear_sign_bit(x)

LEDA_BEGIN_NAMESPACE 


const double circle_eps0 = ldexp(1.0,-53);

/*
Kurt:: The constructor checks whether the points p1, p2, p3 are admissable. 
If all three of them are equal it defines the center.
*/

rat_circle_rep::rat_circle_rep(const rat_point& p1, 
                               const rat_point& p2, 
                               const rat_point& p3) : 
a(p1), b(p2), c(p3), cp(0), first_side_of(true)
{ orient = orientation(p1,p2,p3);
  if (orient == 0 && (p1 == p2 || p1 == p3 || p2 == p3) )
  { if (p1 == p2 && p1 == p3)
       cp = new rat_point(p1);
    else
       LEDA_EXCEPTION(1,"rat_circle::rat_circle:non-admissable triple");
  }
} 
                              



rat_circle::rat_circle(const rat_point& a,const rat_point& b,const rat_point& c)
{ PTR = new rat_circle_rep(a,b,c); }


rat_circle::rat_circle(const rat_point& a, const rat_point& b0)
{ // center a and b0 on circle
  rat_point b1 = b0.rotate90(a);
  rat_point b2 = b1.rotate90(a);
  PTR = new rat_circle_rep(b0,b1,b2);
  if (ptr()->cp == 0) ptr()->cp = new rat_point(a);
 }


rat_circle::rat_circle(const rat_point& a) 
{ PTR = new rat_circle_rep(a,a,a); }

rat_circle::rat_circle() 
{ rat_point p(0,0,1);
  PTR = new rat_circle_rep(p,p,p);
 }


circle rat_circle::to_circle() const
{ point p = ptr()->a.to_point();
  point q = ptr()->b.to_point();
  point r = ptr()->c.to_point();
  return circle(p,q,r);
 }


rat_point rat_circle::center() const
{ 
  if (ptr()->cp) return *(ptr()->cp);

  if (is_trivial()) 
  { ptr()->cp = new rat_point(ptr()->a);
    return *(ptr()->cp);
   }

   if (ptr()->orient == 0)
     LEDA_EXCEPTION(1,"rat_circle::center(): points are collinear.");

  // The formula is analogous to the PhD dissertation of
  // C. Burnikel (page 30 above).

  rat_point p = ptr()->a;
  rat_point q = ptr()->b;
  rat_point r = ptr()->c;

 
  integer xp = p.X(), yp = p.Y(), zp = p.W();
  integer xq = q.X(), yq = q.Y(), zq = q.W();
  integer xr = r.X(), yr = r.Y(), zr = r.W();
 
  integer cx  =  zq*zr*(xp*xp+yp*yp)*(yq*zr-yr*zq)
               + zr*zp*(xq*xq+yq*yq)*(yr*zp-yp*zr)
               + zp*zq*(xr*xr+yr*yr)*(yp*zq-yq*zp);
 
  integer cy  = -zq*zr*(xp*xp+yp*yp)*(xq*zr-xr*zq)
                -zr*zp*(xq*xq+yq*yq)*(xr*zp-xp*zr)
                -zp*zq*(xr*xr+yr*yr)*(xp*zq-xq*zp);

  integer cw  =  2*zp*zq*zr*(zp*(xq*yr-xr*yq) 
               + zq*(xr*yp-xp*yr) + zr*(xp*yq-xq*yp));

  ptr()->cp = new rat_point(cx,cy,cw);


/*
  rat_line l1 = p_bisector(ptr()->a,ptr()->b);
  rat_line l2 = p_bisector(ptr()->b,ptr()->c);
  rat_point m;
  l1.intersection(l2,m);
  ptr()->cp = new rat_point(m);
*/


  return *(ptr()->cp);
}


/*
Kurt: outside muss auch wahr liefern, wenn orient == 0, 
alle drei Punkte gleich und p ungleich diesem Punkt.
*/

bool rat_circle::outside(const rat_point& p) const 
{ if ( ptr()->orient != 0 )
    return (ptr()->orient * side_of(p)) < 0;
  else
    return (!is_trivial() && p != ptr()->a); 
 }


bool rat_circle::inside(const rat_point& p) const 
{ return (ptr()->orient * side_of(p)) > 0; }


bool rat_circle::contains(const rat_point& p) const 
{ if (is_trivial()) 
     return p == ptr()->a;
  else
     return side_of(p) == 0;
}


rat_circle rat_circle::translate(integer dx, integer dy, integer dw) const
{ rat_point a1 = ptr()->a.translate(dx,dy,dw);
  rat_point b1 = ptr()->b.translate(dx,dy,dw);
  rat_point c1 = ptr()->c.translate(dx,dy,dw);
  return rat_circle(a1,b1,c1);
 }

rat_circle rat_circle::translate(const rational& dx, const rational& dy) const
{ rat_point a1 = ptr()->a.translate(dx,dy);
  rat_point b1 = ptr()->b.translate(dx,dy);
  rat_point c1 = ptr()->c.translate(dx,dy);
  return rat_circle(a1,b1,c1);
 }

rat_circle rat_circle::translate(const rat_vector& v) const
{ rat_point a1 = ptr()->a.translate(v);
  rat_point b1 = ptr()->b.translate(v);
  rat_point c1 = ptr()->c.translate(v);
  return rat_circle(a1,b1,c1);
 }



rat_circle  rat_circle::rotate90(const rat_point& o, int i)  const
{ rat_point a1 = ptr()->a.rotate90(o,i);
  rat_point b1 = ptr()->b.rotate90(o,i);
  rat_point c1 = ptr()->c.rotate90(o,i);
  return rat_circle(a1,b1,c1);
}


rat_circle  rat_circle::reflect(const rat_point& p, const rat_point& q)  const
{ rat_point a1 = ptr()->a.reflect(p,q);
  rat_point b1 = ptr()->b.reflect(p,q);
  rat_point c1 = ptr()->c.reflect(p,q);
  return rat_circle(a1,b1,c1);
}


rat_circle  rat_circle::reflect(const rat_point& p)  const
{ rat_point a1 = ptr()->a.reflect(p);
  rat_point b1 = ptr()->b.reflect(p);
  rat_point c1 = ptr()->c.reflect(p);
  return rat_circle(a1,b1,c1);
}


bool rat_circle::operator==(const rat_circle& c)  const
{ if (orientation() != c.orientation()) return false;
  if (!contains(c.ptr()->a)) return false;
  if (!contains(c.ptr()->b)) return false;
  return contains(c.ptr()->c);
}


bool equal_as_sets(const rat_circle& c1, const rat_circle& c2)
{ if (!c1.contains(c2.point1())) return false;
  if (!c1.contains(c2.point2())) return false;
  if (!c1.contains(c2.point3())) return false;
  return true;
}


bool
radical_axis(const rat_circle& c1, const rat_circle& c2, rat_line& rad_axis)
// If c1 and c2 intersect, then the radical axis cuts through the intersection points.
// Let (x1,y1)=c1.center(), r1=r1.radius(), (x2,y2)=c2.center() and r2=c2.radius()
// (1) (x-x1)^2 + (y-y1)^2 - r1^2 = 0 for all points on c1
// (2) (x-x2)^2 + (y-y2)^2 - r2^2 = 0 for all points on c2
// (1)-(2) yields: 2*(x2-x1)*x + 2*(y2-y1)*y + x1^2-x2^2+y1^2-y2^2-r1^2+r2^2 = 0
//            i.e.    a     *x +    b     *y +               c               = 0
// This equality describes the points on the radical axis.
//
// Note: If c1 and the radical axis intersect in a point p=(x,y), 
//       then p satisfies (1) and (1)-(2), hence also (2), i.e. p lies on c2.
// Note: We make sure that rad_axis.point1() <_lex rad_axis.point2().
{
	if (c1.is_degenerate() || c2.is_degenerate()) return false;

	rational x1 = c1.center().xcoord(), y1 = c1.center().ycoord(), sqr_r1 = c1.sqr_radius();
	rational x2 = c2.center().xcoord(), y2 = c2.center().ycoord(), sqr_r2 = c2.sqr_radius();

	rational a = 2 * (x2-x1);
	rational b = 2 * (y2-y1);
	rational c = x1 * x1 - x2 * x2 + y1 * y1 - y2 * y2 - sqr_r1 + sqr_r2;

	if (b != 0) {
		rat_point p(0, -c/b);
		rat_point q(1, -(a+c)/b);
		rad_axis = rat_line(p,q);
		return true;
	}
	else if (a != 0) {
		// b == 0 => vertical line
		rational x = -c/a;
		rat_point p(x, 0);
		rat_point q(x, 1);
		rad_axis = rat_line(p,q);
		return true;
	}
	else {
		return false;
	}
}


ostream& operator<<(ostream& out, const rat_circle& c) 
{ out << c.ptr()->a << " " << c.ptr()->b << " " << c.ptr()->c;
  return out;
 } 

istream& operator>>(istream& in,  rat_circle& c) 
{ rat_point p,q,r;
  in >> p >> q >> r;
  c = rat_circle(p,q,r);
  return in;
}



int rat_circle::side_of(const rat_point& p) const
{ 
  integer AX = ptr()->a.X();
  integer AY = ptr()->a.Y();
  integer AW = ptr()->a.W();

  if (ptr()->first_side_of)
  {
    integer BX = ptr()->b.X();
    integer BY = ptr()->b.Y();
    integer BW = ptr()->b.W();
  
    integer CX = ptr()->c.X();
    integer CY = ptr()->c.Y();
    integer CW = ptr()->c.W();

    integer b1 = BX*AW - AX*BW;
    integer b2 = BY*AW - AY*BW;
    integer bx = b1 * AW * BW;
    integer by = b2 * AW * BW;
    integer bw = b1*b1 + b2*b2;

    integer c1 = CX*AW - AX*CW;
    integer c2 = CY*AW - AY*CW;
    integer cx = c1 * AW * CW;
    integer cy = c2 * AW * CW;
    integer cw = c1*c1 + c2*c2;

    ptr()->D1 = by*cx - bx*cy;
    ptr()->D2 = cy*bw - by*cw;
    ptr()->D3 = bx*cw - cx*bw;

    ptr()->first_side_of = false;
  }

  // floating point filter :

  double pxaw = p.XD() * ptr()->a.WD();   
  double pyaw = p.YD() * ptr()->a.WD();
  double axpw = ptr()->a.XD() * p.WD(); 
  double aypw = ptr()->a.YD() * p.WD(); 

  double pax = pxaw - axpw;
  double pay = pyaw - aypw;
  double paw = p.WD()*(ptr()->a.WD());

  double Pxi = pax * paw;
  double Pyi = pay * paw;
  double Pwi = pax*pax + pay*pay;

  double D1 = (ptr()->D1).to_double();
  double D2 = (ptr()->D2).to_double();
  double D3 = (ptr()->D3).to_double();

  double E = Pwi*D1 + Pxi*D2 + Pyi*D3;
        
  /* ----------------------------------------------------------------
   * ERROR BOUND:
   * ----------------------------------------------------------------
   *
   * mes(E)   = mes(D1*Pwi + D2*Pxi + D3*Pyi)
   *          = 2*( mes(D1*Pwi) + 2*mes(D2*Pxi) + 2*mes(D3*Pyi) )
   *          = 2*( mes(D1)*mes(Pwi) + 2*mes(D2)*mes(Pxi) + 2*mes(D3)*mes(Pyi) )
   *        
   * mes(Pyi) = mes(pay) * mes(paw) 
   *          = mes(pyaw - aypw)* mes(paw)
   *          = 2*(mes(pyaw) + mes(aypw))* mes(paw)
   *          = 2*(fabs(pyaw) + fabs(aypw))*paw
   *            ( because a.WD() and p.WD() are positive )
   *
   * mes(Pxi) = 2*(fabs(pxaw) + fabs(axpw))*paw
   *
   * mes(Pwi) = 2*(mes(pax)*mes(pax) + mes(pay)*mes(pay))
   *          = 2*( 2*(fabs(pxaw) + fabs(axpw)) * 
   *                2*(fabs(pxaw) + fabs(axpw)) +
   *                2*(fabs(pyaw) + fabs(aypw)) * 
   *                2*(fabs(pyaw) + fabs(aypw)))
   *          = 8*((fabs(pxaw)+fabs(axpw))*(fabs(pxaw)+fabs(axpw)) +
   *               (fabs(pyaw)+fabs(aypw))*(fabs(pyaw)+fabs(aypw)))
   *
   * mes(E) = 2*( fabs(D1) *
   *              8*((fabs(pxaw)+fabs(axpw))*(fabs(pxaw)+fabs(axpw)) +
   *                 (fabs(pyaw)+fabs(aypw))*(fabs(byaw)+fabs(aybw)))
   *              + 
   *              fabs(D2)*4*(fabs(pxaw) + fabs(axpw))*paw
   *              +
   *              fabs(D3)*4*(fabs(pyaw) + fabs(aypw))*paw )
   *
   * ind(E) = ( ind(D1*Pwi) + ind(D2*Pxi + D3*Pyi) +1 ) /2
   *        = ( ind(D1*Pwi) + (ind(D2*Pxi) + ind(D3*Pyi) + 1)/2 +1) /2
   *        = ( ind(D1) + ind(Pwi) + 0.5 + (ind(D2) + ind(Pxi) + 0.5 + 
   *                                  ind(D3) +ind(Pyi) + 0.5 + 1)/2 +1) /2
   *
   * ind(Pyi) = ind(pay)+ind(paw)+0.5
   *          = ind(pxaw-axpw)+ind(baw)+0.5
   *          = (1.5 + 1.5 +1)/2 + 1.5 +0.5
   *          = 4 = ind(Pxi)
   *
   * ind(Pwi) = ind(pax*pax + pay*pay)
   *          = (ind(pax) + ind(pax) +0.5 + ind(pay) + ind(pay)+0.5+1)/2
   *          = (1.5 + 1.5 +1)/2 + (1.5 + 1.5 +1)/2 + 1
   *          = 5 
   *
   * ind(E) = (0.5  + 5 + 0.5 + (0.5 + 4 + 0.5 + 
   *                             0.5 + 4 + 0.5 + 1)/2 +1) /2
   *        = (6.0 + 5.5 + 1)/2
   *        = 25/4
   *
   * eps(E) = ind(E) * mes(E) * eps0
   *        = 50*( fabs(D1) *
   *              2*((fabs(pxaw)+fabs(axpw))*(fabs(pxaw)+fabs(axpw)) +
   *                 (fabs(pyaw)+fabs(aypw))*(fabs(pyaw)+fabs(aypw)))
   *              + 
   *              fabs(D2)*(fabs(pxaw) + fabs(axpw))*paw
   *              +
   *              fabs(D3)*(fabs(pyaw) + fabs(aypw))*paw ) * eps0
   *             
   * -----------------------------------------------------------------*/
  
  FABS(D1); 
  FABS(D2); 
  FABS(D3);
  FABS(pxaw); 
  FABS(axpw); 
  FABS(pyaw); 
  FABS(aypw);

  double eps = 50*( D1*2*((pxaw+axpw)*(pxaw+axpw) +
                          (pyaw+aypw)*(pyaw+aypw)) + 
                    D2*(pxaw+axpw)*paw +D3*(pyaw+aypw)*paw ) * circle_eps0;
  
  if (E   >  eps)  return  1;
  if (E   < -eps)  return -1;
  if (eps <  1)    return  0;

  integer p1 = p.X()*AW - AX*p.W();
  integer p2 = p.Y()*AW - AY*p.W();

  integer px = p1 * AW * p.W();
  integer py = p2 * AW * p.W();
  integer pw = p1*p1 + p2*p2;

  return sign(pw*ptr()->D1 + px*ptr()->D2  + py*ptr()->D3);
}


// We implement the procedure rat_point_on_circle that computes 
// for a circle C given by rational points p, q, r and an angle
// _alpha a rational point s on C such that the angle of s 
// at C differs from _alpha by at most a given delta


// The following procedure computes a rat_point phi_alpha
// on the circle C whose angle at C satisfies
//     |angle(phi_alpha)-_alpha| < epsilon
// _alpha and the corresponding error bound epsilon are in radians

// It is similar to the method shown in the paper by Canny et al:
// "A Rational Rotation Method for Robust Geometric Algorithms"
// (Proc. of the 8th ACM Symposium on Computational Geometry, 
//  pages 251-260, 1992)

// preconditions: 0 <= _alpha <= 2*pi; epsilon >= 2^{-48}


static void compute_phi_C(
  const integer& m,  const integer& n,
  const rat_point& p, 
  integer& phi_x, integer& phi_y, integer& phi_w)
{
  phi_x = (m*m-n*n) * p.X() - 2*m*n * p.Y();
  phi_y = (n*n-m*m) * p.Y() - 2*m*n * p.X();
  phi_w = (m*m+n*n) * p.W();
}

static double compute_t(double _alpha, const rat_point& p)
{
  // preconditions: 0 <= _alpha <= pi/2, 
  //                0 >= p.Y() >= p.X()

  double si = sin(_alpha), co = cos(_alpha);
  double sin_alpha, cos_alpha;
  if (co < si) {  sin_alpha = co; cos_alpha = si; }
  else         {  sin_alpha = si; cos_alpha = co; }

  double px = p.X().to_double();
  double py = p.Y().to_double();
  
  integer ipx = p.X(), ipy = p.Y();
  integer rsq = ipx*ipx+ipy*ipy;
  double r = ::sqrt(rsq.to_double());
  double t = (r*sin_alpha-py) / (r*cos_alpha-px); 
  return(t);
}


rat_point rat_circle::point_on_circle(double _alpha, double epsilon) const
{

  // precondition: epsilon >= 2^{-48}

  if (epsilon==0)
     LEDA_EXCEPTION(1,
        "rat_circle::point_on_circle: epsilon must be > 0");

  // convert to degrees (sn)

  _alpha *= 180/LEDA_PI;


  rat_point phi_alpha;
  rat_point c = center();

  double _alpha_radians, _alpha_trans;
  
  {
    int sector_360 = (int) ::floor(_alpha/360);
    _alpha -= sector_360 * 360;
    int sector_90 = (int) ::floor(_alpha/90);
    double _alpha_90 = _alpha - sector_90 * 90;
    _alpha_radians = _alpha*asin(1.0)/90;
    _alpha_trans = _alpha_90*asin(1.0)/90;
  }

  double sin_alpha = sin(_alpha_radians);
  double cos_alpha = cos(_alpha_radians);

  rat_point p_trans;
  
  {
    rat_point p = point1();
    integer pfactor = c.W()/p.W();
    integer common_w = c.W();
    integer dx, dy;
    if (pfactor*p.W()==c.W())
     {
       // p.W() is a divisor of c.W()
       dx = pfactor*p.X()-c.X();
       dy = pfactor*p.Y()-c.Y();
     }
    else
     {
       // the general case
       dx = c.W()*p.X()-c.X()*p.W();
       dy = c.W()*p.Y()-c.Y()*p.W();
       common_w = c.W()*p.W();
     }
    if (abs(dx) >= abs(dy))
       p_trans = rat_point(-abs(dx),-abs(dy),common_w);
    else
       p_trans = rat_point(-abs(dy),-abs(dx),common_w);
       
  }

  
  double t = compute_t(_alpha_trans,p_trans);

  double delta;
  {
    double epsilon_radians = epsilon * asin(1.0) /90;
    delta = epsilon_radians - 26*t*ldexp(1.0,-53);
    delta = ldexp(delta,-1);
    if (delta <= 0) 
      LEDA_EXCEPTION(1,
      "rat_circle::point_on_circle called with a too small epsilon");
  }


  rational rt_low = t - delta, rt_high = t + delta;
  if (rt_high > 1) rt_high = 1;
  if (rt_low  < 0) rt_low  = 0;
  rational rt = small_rational_between(rt_low, rt_high);
  integer m = rt.numerator(), n = rt.denominator();

  integer phi_x, phi_y, phi_w;
  compute_phi_C(m,n,p_trans,phi_x,phi_y,phi_w);
  
  {
    integer xresult, yresult, wresult;
    if (fabs(cos_alpha) >= fabs(sin_alpha))
      { xresult = phi_x; yresult = phi_y; }
    else
      { xresult = phi_y; yresult = phi_x; }
    wresult = phi_w;
    if (cos_alpha < 0) xresult = -xresult;
    if (sin_alpha < 0) yresult = -yresult;
    if (p_trans.W()==c.W())
      phi_alpha = rat_point(
        xresult+(m*m+n*n)*c.X(),yresult+(m*m+n*n)*c.Y(),wresult);
    else
    {
      rat_point p = point1();
      phi_alpha = rat_point(
        xresult+(m*m+n*n)*c.X()*p.W(),yresult+(m*m+n*n)*c.Y()*p.W(),wresult);
    }
  }

  {
    if (!contains(phi_alpha))
      LEDA_EXCEPTION(1,"rat_circle::point_on_circle: internal error");
    rat_point phi_C = phi_alpha.translate(-c.X(),-c.Y(),c.W());
    double x = phi_C.xcoordD(), y = phi_C.ycoordD();
    double radius = ::sqrt(x*x+y*y);
    if (fabs(x/radius - cos_alpha) > epsilon)
      LEDA_EXCEPTION(1,"rat_circle::point_on_circle: internal error");
    if (fabs(y/radius - sin_alpha) > epsilon)
      LEDA_EXCEPTION(1,"rat_circle::point_on_circle: internal error");
  }

  return phi_alpha;
}



rational rat_circle::sqr_radius() const
{ return ptr()->a.sqr_dist(center()); }


LEDA_END_NAMESPACE 
