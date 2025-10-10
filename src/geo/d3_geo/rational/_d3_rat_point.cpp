/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_rat_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// d3_rat_points :  3D points with rational (homogeneous) coordinates
//
// last modified (06/99)
//------------------------------------------------------------------------------


#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/numbers/fp.h>
#include <LEDA/numbers/expcomp.h>


#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ <= 2
#define FABS(x) x = fp::abs(x)
#else
// causes internal compiler error in g++-3.0.1 and g++-3.0.2
#define FABS(x) fp::clear_sign_bit(x)
#endif

LEDA_BEGIN_NAMESPACE


atomic_counter d3_rat_point_rep::id_counter(0);

// constructors

d3_rat_point_rep::d3_rat_point_rep() : x(0), y(0), z(0), w(1)
{ xd = 0;
  yd = 0;
  zd = 0;
  wd = 1;
  id = ++id_counter;
 }

d3_rat_point_rep::d3_rat_point_rep(const rational& a, const rational& b, 
                                                      const rational& c) 
: x(a.numerator()*b.denominator()*c.denominator()),
  y(b.numerator()*a.denominator()*c.denominator()),
  z(c.numerator()*a.denominator()*b.denominator()),
  w(a.denominator()*b.denominator()*c.denominator())
{ 
  xd = x.to_double(); 
  yd = y.to_double(); 
  zd = z.to_double(); 
  wd = w.to_double(); 
  id = ++id_counter;
 }

d3_rat_point_rep::d3_rat_point_rep(integer a, integer b, integer c) 
: x(a), y(b), z(c), w(1)
{ 
  xd = a.to_double(); 
  yd = b.to_double(); 
  zd = c.to_double(); 
  wd = 1;
  id = ++id_counter;
 }

d3_rat_point_rep::d3_rat_point_rep(integer a, integer b, integer c, integer d) 
: x(a), y(b), z(c), w(d)
{ 
  if (d < 0)
  { x = -a;
    y = -b;
    z = -c;
    w = -d;
   }
  xd = x.to_double(); 
  yd = y.to_double(); 
  zd = z.to_double(); 
  wd = w.to_double();
  id = ++id_counter;
 }



// static members used for statistics
//


int d3_rat_point::orient_count= 0;
int d3_rat_point::exact_orient_count = 0;

int d3_rat_point::cmp_count= 0;
int d3_rat_point::exact_cmp_count = 0;

int d3_rat_point::sos_count= 0;
int d3_rat_point::exact_sos_count = 0;

// use_filter flag controls whether the floating point filter is used 

int d3_rat_point::use_filter = 1;

int d3_rat_point::default_precision = 0;

/*
inline void cross_product( integer AX, integer AY, integer AZ, integer AW, 
                           integer BX, integer BY, integer BZ, integer BW,
                           integer& x, integer& y, integer& z, integer& w)
{ x = AZ*BY - AY*BZ;
  y = AX*BZ - AZ*BX;
  z = AY*BX - AX*BY;
  w = AW*BW;
}

inline rat_vector cross_product(const rat_vector& A, const rat_vector& B)
{ integer x,y,z,w;
  cross_product(A.hcoord(0),A.hcoord(1),A.hcoord(2),A.hcoord(3),
                B.hcoord(0),B.hcoord(1),B.hcoord(2),B.hcoord(3),
                x,y,z,w);
  return rat_vector(x,y,z,w);
 }
*/

d3_rat_point::d3_rat_point(const d3_point& p, int prec)
{
  double x = p.xcoord();
  double y = p.ycoord();
  double z = p.zcoord();
 
  if ( prec > 0 )
  { double P = ldexp(1.0,prec);
    PTR = new d3_rat_point_rep(integer(P*x),integer(P*y),integer(P*z),integer(P));
    return;
  }

  rational xr(x), yr(y), zr(z); // all denominators are powers of two
  integer XX, YY, ZZ, WW;
  
  if ( xr.denominator() == yr.denominator() &&  xr.denominator() == zr.denominator())
  { XX = xr.numerator();
    YY = yr.numerator();
    ZZ = zr.numerator();
    WW = xr.denominator();
  }
  else   // not all denominators are equal ...
  {
    // find largest denominator ...
    integer largest;
    if ( xr.denominator() < yr.denominator() ) largest = yr.denominator();      
    else largest = xr.denominator(); 
    if ( zr.denominator() > largest ) largest = zr.denominator();  
    
    integer q1 = largest/xr.denominator();
    integer q2 = largest/yr.denominator();
    integer q3 = largest/zr.denominator();
    
    WW = largest;
    XX = xr.numerator() * q1;
    YY = yr.numerator() * q2;
    ZZ = zr.numerator() * q3;       
  } 
  PTR = new d3_rat_point_rep(XX,YY,ZZ,WW);  
}



rational d3_rat_point::operator[](int i) const
{ switch (i) {
  case 0: return xcoord();
  case 1: return ycoord();
  case 2: return zcoord();
  }
  LEDA_EXCEPTION(1,"d3_rat_point[i]: index out of range");
  return 0;
}

integer d3_rat_point::hcoord(int i) const
{ switch (i) {
  case 0: return X();
  case 1: return Y();
  case 2: return Z();
  case 3: return W();
  }
  LEDA_EXCEPTION(1,"d3_rat_point::hcoord(i) index out of range");
  return 0;
}



d3_point d3_rat_point::to_d3_point() const
{ return d3_point(xcoordD(),ycoordD(),zcoordD()); }

rat_vector  d3_rat_point::to_vector() const 
{ return rat_vector(ptr()->x,ptr()->y,ptr()->z,ptr()->w); }


rat_point d3_rat_point::project_xy() const { return rat_point(X(),Y(),W()); }
rat_point d3_rat_point::project_yz() const { return rat_point(Y(),Z(),W()); }
rat_point d3_rat_point::project_xz() const { return rat_point(X(),Z(),W()); }


d3_rat_point d3_rat_point::reflect(const d3_rat_point& q) const
{ // reflect point across point q

  integer x1 = X();
  integer y1 = Y();
  integer z1 = Z();
  integer w1 = W();
  integer x2 = 2*q.X();
  integer y2 = 2*q.Y();
  integer z2 = 2*q.Z();
  integer w2 = q.W();

  return d3_rat_point(x2*w1-x1*w2 , y2*w1-y1*w2, z2*w1-z1*w2, w1*w2);
}


d3_rat_point  d3_rat_point::reflect(const d3_rat_point& a, 
                                    const d3_rat_point& b,
                                    const d3_rat_point& c) const
{  
  // reflect point across plane through a, b, and c

  integer AX = b.X()*a.W() - a.X()*b.W();
  integer AY = b.Y()*a.W() - a.Y()*b.W();
  integer AZ = b.Z()*a.W() - a.Z()*b.W();
  //integer AW = a.W() * b.W();

  integer BX = c.X()*a.W() - a.X()*c.W();
  integer BY = c.Y()*a.W() - a.Y()*c.W();
  integer BZ = c.Z()*a.W() - a.Z()*c.W();
  //integer BW = a.W() * c.W();

  integer CX = X()*a.W() - a.X()*W();
  integer CY = Y()*a.W() - a.Y()*W();
  integer CZ = Z()*a.W() - a.Z()*W();
  integer CW = a.W() * W();

  integer x = AZ*BY - AY*BZ;
  integer y = AX*BZ - AZ*BX;
  integer z = AY*BX - AX*BY;
  //integer w = AW*BW;

  if (x == 0 && y == 0 && z == 0)
      LEDA_EXCEPTION(1,"d3_rat_point::reflect(a,b,c): a,b,c colinear");

  integer A = -2*(x*CX + y*CY + z*CZ);

  return translate(A*x, A*y, A*z, (x*x+y*y+z*z)*CW);
}


d3_rat_point d3_rat_point::translate(integer dx, integer dy, integer dz, 
                                                             integer dw) const
{ integer x1 = X();
  integer y1 = Y();
  integer z1 = Z();
  integer w1 = W();
  integer x = dx * w1 + x1 * dw;
  integer y = dy * w1 + y1 * dw;
  integer z = dz * w1 + z1 * dw;
  return d3_rat_point(x,y,z,dw*w1);
 }

d3_rat_point d3_rat_point::translate(const rational& dx, 
                                     const rational& dy,
                                     const rational& dz) const
{ integer x = dx.numerator() * dy.denominator();
  integer y = dy.numerator() * dx.denominator();
  integer z = dz.numerator() * dz.denominator();
  integer w = dx.denominator() * dy.denominator() * dz.denominator();
  return translate(x,y,z,w);
}


rational d3_rat_point::sqr_dist(const d3_rat_point& p) const
{ 
  integer x1 = X();
  integer y1 = Y();
  integer z1 = Z();
  integer w1 = W();

  integer x2 = p.X();
  integer y2 = p.Y();
  integer z2 = p.Z();
  integer w2 = p.W();

  integer dx = x1*w2 - x2*w1;
  integer dy = y1*w2 - y2*w1;
  integer dz = z1*w2 - z2*w1;

  return rational(dx*dx+dy*dy+dz*dz,w1*w1*w2*w2);
}

rational d3_rat_point::xdist(const d3_rat_point& p) const
{ integer x1  = X();
  integer w1  = W();
  integer x2  = p.X();
  integer w2  = p.W();
  return rational(abs(x1*w2-x2*w1), w1*w2);
}

rational d3_rat_point::ydist(const d3_rat_point& p) const
{ integer y1  = Y();
  integer w1  = W();
  integer y2  = p.Y();
  integer w2  = p.W();
  return rational(abs(y1*w2-y2*w1), w1*w2);
}


rational d3_rat_point::zdist(const d3_rat_point& p) const
{ integer z1  = Z();
  integer w1  = W();
  integer z2  = p.Z();
  integer w2  = p.W();
  return rational(abs(z1*w2-z2*w1), w1*w2);
}


d3_rat_point center(const d3_rat_point& a, const d3_rat_point& b)
{ integer x = a.X()*b.W()+b.X()*a.W();
  integer y = a.Y()*b.W()+b.Y()*a.W();
  integer z = a.Z()*b.W()+b.Z()*a.W();
  integer w = 2*a.W()*b.W(); 
  return d3_rat_point(x,y,z,w);
 } 



// basic stream I/O operations

ostream& operator<<(ostream& out, const d3_rat_point& p)
{ out << "(" << p.X() << "," << p.Y() << "," << p.Z() << "," << p.W() << ")";
  return out;
 } 

istream& operator>>(istream& in, d3_rat_point& p) 
{ // syntax: {(} x {,} y {,} z {,} w {)}   

  integer x,y,z,w;
  char c;

  do in.get(c); while (in && isspace(c));

  if (!in) return in;

  if (c != '(') in.putback(c);

  in >> x;

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> y; 

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> z; 

  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);

  in >> w; 

  do in.get(c); while (c == ' ');
  if (c != ')') in.putback(c);

  p = d3_rat_point(x,y,z,w ); 
  return in; 

 } 


// machine precision

const double eps0 = ldexp(1.0,-53);

// cmp - primitive

int d3_rat_point::cmp_xyz(const d3_rat_point& a, const d3_rat_point& b)
{ 
  if (use_filter != 0)
  { 
    double axbw = a.XD()*b.WD();
    double bxaw = b.XD()*a.WD();
    double E    = axbw - bxaw;       // floating point result

    //----------------------------------------------------------------
    //
    // mes(E) = mes(ax*bw - bx*aw)
    //        = 2*(mes(ax)*mes(bw) + mes(bx)*mes(aw))
    //        = 2*(fabs(ax)*fabs(bw) + fabs(bx)*fabs(aw)) 
    //        = 2*(fabs(axbw) + fabs(bxaw)) 
    //
    // ind(E) = ind(ax*bw - bx*aw)
    //        = (ind(ax*bw) + ind(bx*aw) + 1)/2
    //        = ((ind(ax)+ind(bw)+0.5) + (ind(bx)+ind(aw)+0.5) + 1)/2
    //        = (1.5 + 1.5 + 1)/2
    //        = 2
    //
    // eps(E) = ind(E) * mes(E) * eps0 
    //        = 4 * (fabs(axbw) + fabs(bxaw)) * eps0
    //----------------------------------------------------------------


    FABS(axbw);
    FABS(bxaw);
    double eps = 4 * (axbw+bxaw) * eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
   
    if (eps < 1)  // compare y-coordinates
    { double aybw = a.YD()*b.WD();
      double byaw = b.YD()*a.WD();
      double E    = aybw - byaw;
      FABS(aybw);
      FABS(byaw);
      double eps = 4 * (aybw+byaw) * eps0;
      if (E > +eps) return +1;
      if (E < -eps) return -1;

      if (eps < 1)  // compare z-coordinates
      { double azbw = a.ZD()*b.WD();
        double bzaw = b.ZD()*a.WD();
        double E    = azbw - bzaw;
        FABS(azbw);
        FABS(bzaw);
        double eps = 4 * (azbw+bzaw) * eps0;
        if (E > +eps) return +1;
        if (E < -eps) return -1;
        if (eps < 1)  return  0; 
       }
     }

   }
  
    //use big integer arithmetic

    d3_rat_point::exact_cmp_count++;

    integer axbw = a.X()*b.W();
    integer bxaw = b.X()*a.W();
    if (axbw > bxaw) return  1;
    if (axbw < bxaw) return -1;

    integer aybw = a.Y()*b.W();
    integer byaw = b.Y()*a.W();
    if (aybw > byaw) return  1;
    if (aybw < byaw) return -1;

    integer azbw = a.Z()*b.W();
    integer bzaw = b.Z()*a.W();
    if (azbw > bzaw) return  1;
    if (azbw < bzaw) return -1;
    return 0;
}


int d3_rat_point::cmp_xyz_F(const d3_rat_point& a, const d3_rat_point& b)
{ 
    double axbw = a.XD()*b.WD();
    double bxaw = b.XD()*a.WD();
    if (axbw > bxaw) return  1;
    if (axbw < bxaw) return -1;

    double aybw = a.YD()*b.WD();
    double byaw = b.YD()*a.WD();
    if (aybw > byaw) return  1;
    if (aybw < byaw) return -1;

    double azbw = a.ZD()*b.WD();
    double bzaw = b.ZD()*a.WD();
    if (azbw > bzaw) return  1;
    if (azbw < bzaw) return -1;

    return 0;
}
  

int d3_rat_point::cmp_x(const d3_rat_point& a, const d3_rat_point& b)
{ 
  if (use_filter != 0)
  { double axbw = a.XD()*b.WD();
    double bxaw = b.XD()*a.WD();
    double E    = axbw - bxaw;       // floating point result

    FABS(axbw);
    FABS(bxaw);
    double eps = 4 * (axbw+bxaw) * eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
    if (eps < 1) return 0;
   }

  d3_rat_point::exact_cmp_count++;
  integer axbw = a.X()*b.W();
  integer bxaw = b.X()*a.W();
  if (axbw > bxaw) return  1;
  if (axbw < bxaw) return -1;
  return 0;
}
   

int d3_rat_point::cmp_y(const d3_rat_point& a, const d3_rat_point& b)
{ 
  if (use_filter != 0)
  { double aybw = a.YD()*b.WD();
    double byaw = b.YD()*a.WD();
    double E    = aybw - byaw;
    FABS(aybw);
    FABS(byaw);
    double eps = 4 * (aybw+byaw) * eps0;
    if (E > +eps) return +1;
    if (E < -eps) return -1;
    if (eps < 1) return 0;
   }
  integer aybw = a.Y()*b.W();
  integer byaw = b.Y()*a.W();
  if (aybw > byaw) return  1;
  if (aybw < byaw) return -1;
  return 0;
}


int d3_rat_point::cmp_z(const d3_rat_point& a, const d3_rat_point& b)
{ 
  if (use_filter != 0)
  { double azbw = a.ZD()*b.WD();
    double bzaw = b.ZD()*a.WD();
    double E    = azbw - bzaw;
    FABS(azbw);
    FABS(bzaw);
    double eps = 4 * (azbw+bzaw) * eps0;
    if (E > +eps) return +1;
    if (E < -eps) return -1;
    if (eps < 1)  return  0; 
   }
  
  integer azbw = a.Z()*b.W();
  integer bzaw = b.Z()*a.W();
  if (azbw > bzaw) return  1;
  if (azbw < bzaw) return -1;
  return 0;
}





rational volume(const d3_rat_point& a, const d3_rat_point& b, 
                                       const d3_rat_point& c,
                                       const d3_rat_point& d)
{ integer AX = a.X();
  integer AY = a.Y();
  integer AZ = a.Z();
  integer AW = a.W();

  integer BX = b.X();
  integer BY = b.Y();
  integer BZ = b.Z();
  integer BW = b.W();

  integer CX = c.X();
  integer CY = c.Y();
  integer CZ = c.Z();
  integer CW = c.W();

  integer DX = d.X();
  integer DY = d.Y();
  integer DZ = d.Z();
  integer DW = d.W();
   
  integer D =   AZ*BY*CX*DW - AY*BZ*CX*DW - AZ*BX*CY*DW + AX*BZ*CY*DW 
              + AY*BX*CZ*DW - AX*BY*CZ*DW - AZ*BY*CW*DX + AY*BZ*CW*DX 
              + AZ*BW*CY*DX - AW*BZ*CY*DX - AY*BW*CZ*DX + AW*BY*CZ*DX 
              + AZ*BX*CW*DY - AX*BZ*CW*DY - AZ*BW*CX*DY + AW*BZ*CX*DY 
              + AX*BW*CZ*DY - AW*BX*CZ*DY - AY*BX*CW*DZ + AX*BY*CW*DZ 
              + AY*BW*CX*DZ - AW*BY*CX*DZ - AX*BW*CY*DZ + AW*BX*CY*DZ;

  integer W = 6*AW*BW*CW*DW; 

  return rational(D,W);
}



int orientation(const d3_rat_point& a, const d3_rat_point& b, 
                const d3_rat_point& c, const d3_rat_point& d)
{
    d3_rat_point::orient_count++;
    
    if (d3_rat_point::use_filter != 0)
      {
        double ax = a.XD();
        double bx = b.XD();
        double cx = c.XD();
        double dx = d.XD();

        double ay = a.YD();
        double by = b.YD();
        double cy = c.YD();
        double dy = d.YD();

        double az = a.ZD();
        double bz = b.ZD();
        double cz = c.ZD();
        double dz = d.ZD();

        double aw = a.WD();
        double bw = b.WD();
        double cw = c.WD();
        double dw = d.WD();

        double x1 = bx*aw - ax*bw;
        double y1 = by*aw - ay*bw;
        double z1 = bz*aw - az*bw;
        double x2 = cx*aw - ax*cw;
        double y2 = cy*aw - ay*cw;
        double z2 = cz*aw - az*cw;
        double x3 = dx*aw - ax*dw;
        double y3 = dy*aw - ay*dw;
        double z3 = dz*aw - az*dw;

        double E = (z1*y2-y1*z2)*x3 + (x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3;
      
        /* ----------------------------------------------------------------
         * ERROR BOUND:
         * ----------------------------------------------------------------
         *
         * mes(E) = mes((z1*y2-y1*z2)*x3 + (x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3)
         *        = 2*( mes((z1*y2-y1*z2)*x3) + 
         *              2*( mes((x1*z2-z1*x2)*y3) + mes((y1*x2-x1*y2)*z3)))
         *
         * mes(x1) = 2*(mes(bx)*mes(aw) + mes(ax)*mes(bw))
         *         = 2*(fabs(bx)*aw + fabs(ax)*bw)
         *           (because a.WD() and b.WD() are positive )
         *
         * mes(y1) = 2*(fabs(by)*aw + fabs(ay)*bw)
         * mes(z1) = 2*(fabs(bz)*aw + fabs(az)*bw)
         * mes(x2) = 2*(fabs(cx)*aw + fabs(ax)*cw)
         * mes(y2) = 2*(fabs(cy)*aw + fabs(ay)*cw)
         * mes(z2) = 2*(fabs(cz)*aw + fabs(az)*cw)
         * mes(x3) = 2*(fabs(dx)*aw + fabs(ax)*dw)
         * mes(y3) = 2*(fabs(dy)*aw + fabs(ay)*dw)
         * mes(z3) = 2*(fabs(dz)*aw + fabs(az)*dw)
         *
         * mes((z1*y2-y1*z2)*x3) = 2*(mes(z1)*mes(y2)+mes(y1)*mes(z2))*mes(x3)
         *  = 2*(2*(fabs(bz)*aw + fabs(az)*bw)*2*(fabs(cy)*aw + fabs(ay)*cw)+
         *       2*(fabs(by)*aw + fabs(ay)*bw)*2*(fabs(cz)*aw + fabs(az)*cw))
         *     * 2*(fabs(dx)*aw + fabs(ax)*dw)
         *  = 16*((fabs(bz)*aw + fabs(az)*bw)*(fabs(cy)*aw + fabs(ay)*cw)+
         *        (fabs(by)*aw + fabs(ay)*bw)*(fabs(cz)*aw + fabs(az)*cw))
         *      *(fabs(dx)*aw + fabs(ax)*dw)
         *
         * mes((x1*z2-z1*x2)*y3) = 2*(mes(x1)*mes(z2)+mes(z1)*mes(x2))*mes(y3)
         *  = 16*((fabs(bx)*aw + fabs(ax)*bw)*(fabs(cz)*aw + fabs(az)*cw)+
         *        (fabs(bz)*aw + fabs(az)*bw)*(fabs(cx)*aw + fabs(ax)*cw))
         *      *(fabs(dy)*aw + fabs(ay)*dw)
         *
         * mes((y1*x2-x1*y2)*z3) = 2*(mes(y1)*mes(x2)+mes(x1)*mes(y2))*mes(z3)
         *  = 16*((fabs(by)*aw + fabs(ay)*bw)*(fabs(cx)*aw + fabs(ax)*cw)+
         *        (fabs(bx)*aw + fabs(ax)*bw)*(fabs(cy)*aw + fabs(ay)*cw))
         *      *(fabs(dz)*aw + fabs(az)*dw)
         *
         * mes(E) = 2*(16*((fabs(bz)*aw+fabs(az)*bw)*(fabs(cy)*aw+fabs(ay)*cw)+
         *                 (fabs(by)*aw+fabs(ay)*bw)*(fabs(cz)*aw+fabs(az)*cw))
         *               *(fabs(dx)*aw+fabs(ax)*dw) + 
         *              2*( 
         *             16*((fabs(bx)*aw+fabs(ax)*bw)*(fabs(cz)*aw+fabs(az)*cw)+
         *                 (fabs(bz)*aw+fabs(az)*bw)*(fabs(cx)*aw+fabs(ax)*cw))
         *               *(fabs(dy)*aw + fabs(ay)*dw) 
         *              + 
         *             16*((fabs(by)*aw+fabs(ay)*bw)*(fabs(cx)*aw+fabs(ax)*cw)+
         *                 (fabs(bx)*aw+fabs(ax)*bw)*(fabs(cy)*aw+fabs(ay)*cw))
         *               *(fabs(dz)*aw + fabs(az)*dw)))
         *        = 32*(((fabs(bz)*aw+fabs(az)*bw)*(fabs(cy)*aw+fabs(ay)*cw)+
         *                 (fabs(by)*aw+fabs(ay)*bw)*(fabs(cz)*aw+fabs(az)*cw))
         *               *(fabs(dx)*aw+fabs(ax)*dw) + 
         *           + 2* 
         *              (((fabs(bx)*aw+fabs(ax)*bw)*(fabs(cz)*aw+fabs(az)*cw)+
         *                (fabs(bz)*aw+fabs(az)*bw)*(fabs(cx)*aw+fabs(ax)*cw))
         *               *(fabs(dy)*aw + fabs(ay)*dw) 
         *              + 
         *               ((fabs(by)*aw+fabs(ay)*bw)*(fabs(cx)*aw+fabs(ax)*cw)+
         *                (fabs(bx)*aw+fabs(ax)*bw)*(fabs(cy)*aw+fabs(ay)*cw))
         *               *(fabs(dz)*aw + fabs(az)*dw)))
         *
         * ind(E) = ind((z1*y2-y1*z2)*x3 + (x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3)
         *        = ( ind((z1*y2-y1*z2)*x3) + 
         *            ind((x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3) + 1) / 2
         *        = ( ind((z1*y2-y1*z2)*x3) + (ind((x1*z2-z1*x2)*y3) +
         *            ind((y1*x2-x1*y2)*z3) + 1) /2 +1 ) / 2
         *
         * ind(x1) = ind(bx*aw - ax*bw) = (ind(bx*aw) + ind(ax*bw) + 1) / 2
         *         = (1.5 + 1.5 + 1) / 2
         *         = 2
         *         = ind(y1) = ind(z1) = ind(x2) = ind(y2) = ind(z2) 
         *         = ind(x3) = ind(y3) = ind(z3)
         *
         * ind((x1*z2-z1*x2)*y3) =
         * ind((y1*x2-x1*y2)*z3) =
         * ind((z1*y2-y1*z2)*x3) = ind((z1*y2-y1*z2)) + ind(x3) + 0.5
         *         = (ind(z1*y2) + ind(y1*z2) + 1 ) / 2 + ind(x3) + 0.5
         *         = (ind(z1)+ind(y2)+0.5+ind(y1)+ind(z2)+0.5+1)/2+ind(x3)+0.5
         *         = (  2    +   2   +0.5+   2   +   2   +0.5+1)/2+   2   +0.5
         *         = 10/2 + 2 + 0.5 = 15/2
         *
         * ind(E)  = ( 15/2 + ( 15/2 + 15/2 + 1) / 2 + 1   ) / 2
         *         = ( 15/2 +          16/2          + 2/2 ) / 2 = 33/4
         *
         * eps(E) = ind(E) * mes(E) * eps0
         *        = 8*33*
         *             (((fabs(bz)*aw+fabs(az)*bw)*(fabs(cy)*aw+fabs(ay)*cw)+
         *               (fabs(by)*aw+fabs(ay)*bw)*(fabs(cz)*aw+fabs(az)*cw))
         *               *(fabs(dx)*aw+fabs(ax)*dw)
         *              + 2* 
         *              (((fabs(bx)*aw+fabs(ax)*bw)*(fabs(cz)*aw+fabs(az)*cw)+
         *                (fabs(bz)*aw+fabs(az)*bw)*(fabs(cx)*aw+fabs(ax)*cw))
         *               *(fabs(dy)*aw + fabs(ay)*dw) 
         *              + 
         *               ((fabs(by)*aw+fabs(ay)*bw)*(fabs(cx)*aw+fabs(ax)*cw)+
         *                (fabs(bx)*aw+fabs(ax)*bw)*(fabs(cy)*aw+fabs(ay)*cw))
         *               *(fabs(dz)*aw + fabs(az)*dw)))
         *           *eps0
         * ----------------------------------------------------------------*/

        FABS(ax); FABS(bx); FABS(cx); FABS(dx); FABS(ay); FABS(by);
        FABS(cy); FABS(dy); FABS(az); FABS(bz); FABS(cz); FABS(dz);

        double eps = 264*(((bz*aw+az*bw)*(cy*aw+ay*cw)+
                           (by*aw+ay*bw)*(cz*aw+az*cw))*(dx*aw+ax*dw)
                          + 2*(((bx*aw+ax*bw)*(cz*aw+az*cw)+
                                (bz*aw+az*bw)*(cx*aw+ax*cw))
                               *(dy*aw + ay*dw)
                               + 
                               ((by*aw+ay*bw)*(cx*aw+ax*cw)+
                                (bx*aw+ax*bw)*(cy*aw+ay*cw))
                               *(dz*aw + az*dw))) * eps0;
        
        if (E   < -eps)  return  1;
        if (E   >  eps)  return -1;
        if (eps <  1)    return  0;
      }

    // big integer arithmetic
    
    d3_rat_point::exact_orient_count++;
    
    integer AX =  a.X(); 
    integer BX =  b.X(); 
    integer CX =  c.X(); 
    integer DX =  d.X(); 
    integer AY =  a.Y();
    integer BY =  b.Y();
    integer CY =  c.Y();
    integer DY =  d.Y();
    integer AZ =  a.Z();
    integer BZ =  b.Z();
    integer CZ =  c.Z();
    integer DZ =  d.Z();
    integer AW =  a.W();
    integer BW =  b.W();
    integer CW =  c.W();
    integer DW =  d.W();
    
/*
           D = - AZ*BY*CX*DW + AY*BZ*CX*DW + AZ*BX*CY*DW - AX*BZ*CY*DW 
               - AY*BX*CZ*DW + AX*BY*CZ*DW + AZ*BY*CW*DX - AY*BZ*CW*DX 
               - AZ*BW*CY*DX + AW*BZ*CY*DX + AY*BW*CZ*DX - AW*BY*CZ*DX 
               - AZ*BX*CW*DY + AX*BZ*CW*DY + AZ*BW*CX*DY - AW*BZ*CX*DY 
               - AX*BW*CZ*DY + AW*BX*CZ*DY + AY*BX*CW*DZ - AX*BY*CW*DZ 
               - AY*BW*CX*DZ + AW*BY*CX*DZ + AX*BW*CY*DZ - AW*BX*CY*DZ;
*/

    integer X1,Y1,Z1;
    integer X2,Y2,Z2;
    integer X3,Y3,Z3;
    
    if (AW == 1 && BW == 1 && CW == 1 && DW == 1)
      { X1 = BX-AX;
        Y1 = BY-AY;
        Z1 = BZ-AZ;
        X2 = CX-AX;
        Y2 = CY-AY;
        Z2 = CZ-AZ;
        X3 = DX-AX;
        Y3 = DY-AY;
        Z3 = DZ-AZ;
       }
    else  
    {   X1 = BX*AW - AX*BW;
        Y1 = BY*AW - AY*BW;
        Z1 = BZ*AW - AZ*BW;
        X2 = CX*AW - AX*CW;
        Y2 = CY*AW - AY*CW;
        Z2 = CZ*AW - AZ*CW;
        X3 = DX*AW - AX*DW;
        Y3 = DY*AW - AY*DW;
        Z3 = DZ*AW - AZ*DW;
      }
      
    integer cx = Z1*Y2 - Y1*Z2;
    integer cy = X1*Z2 - Z1*X2;
    integer cz = Y1*X2 - X1*Y2;

    integer D  = cx*X3 + cy*Y3 + cz*Z3;

    return -sign(D);
}


/*
int orientation(const d3_rat_point& a, const d3_rat_point& b, 
                                       const d3_rat_point& c,
                                       const d3_rat_point& d)
{  
    d3_rat_point::orient_count++;

    if (d3_rat_point::use_filter != 0)
    { 
      double ax =  a.ptr()->xd; 
      double bx =  b.ptr()->xd; 
      double cx =  c.ptr()->xd; 
      double dx =  d.ptr()->xd; 
      double ay =  a.ptr()->yd;
      double by =  b.ptr()->yd;
      double cy =  c.ptr()->yd;
      double dy =  d.ptr()->yd;
      double az =  a.ptr()->zd;
      double bz =  b.ptr()->zd;
      double cz =  c.ptr()->zd;
      double dz =  d.ptr()->zd;
      double aw =  a.ptr()->wd;
      double bw =  b.ptr()->wd;
      double cw =  c.ptr()->wd;
      double dw =  d.ptr()->wd;
  
      double x1 = bx*aw - ax*bw;
      double y1 = by*aw - ay*bw;
      double z1 = bz*aw - az*bw;
      double x2 = cx*aw - ax*cw;
      double y2 = cy*aw - ay*cw;
      double z2 = cz*aw - az*cw;
      double x3 = dx*aw - ax*dw;
      double y3 = dy*aw - ay*dw;
      double z3 = dz*aw - az*dw;
        
      double d = (z1*y2-y1*z2)*x3 + (x1*z2-z1*x2)*y3 + (y1*x2-x1*y2)*z3;

      if (d > 0) return +1;
      if (d < 0) return -1;
      return 0;
    }
 

    // big integer arithmetic

    d3_rat_point::exact_orient_count++;

    integer AX =  a.ptr()->x; 
    integer BX =  b.ptr()->x; 
    integer CX =  c.ptr()->x; 
    integer DX =  d.ptr()->x; 
    integer AY =  a.ptr()->y;
    integer BY =  b.ptr()->y;
    integer CY =  c.ptr()->y;
    integer DY =  d.ptr()->y;
    integer AZ =  a.ptr()->z;
    integer BZ =  b.ptr()->z;
    integer CZ =  c.ptr()->z;
    integer DZ =  d.ptr()->z;
    integer AW =  a.ptr()->w;
    integer BW =  b.ptr()->w;
    integer CW =  c.ptr()->w;
    integer DW =  d.ptr()->w;

    integer X1,Y1,Z1;
    integer X2,Y2,Z2;
    integer X3,Y3,Z3;
  
    if (AW == 1 && BW == 1 && CW == 1 && DW == 1)
      { X1 = BX-AX;
        Y1 = BY-AY;
        Z1 = BZ-AZ;
        X2 = CX-AX;
        Y2 = CY-AY;
        Z2 = CZ-AZ;
        X3 = DX-AX;
        Y3 = DY-AY;
        Z3 = DZ-AZ;
       }
    else
      { X1 = BX*AW - AX*BW;
        Y1 = BY*AW - AY*BW;
        Z1 = BZ*AW - AZ*BW;
        X2 = CX*AW - AX*CW;
        Y2 = CY*AW - AY*CW;
        Z2 = CZ*AW - AZ*CW;
        X3 = DX*AW - AX*DW;
        Y3 = DY*AW - AY*DW;
        Z3 = DZ*AW - AZ*DW;
      }

      
    integer cx = Z1*Y2 - Y1*Z2;
    integer cy = X1*Z2 - Z1*X2;
    integer cz = Y1*X2 - X1*Y2;

    integer D  = cx*X3 + cy*Y3 + cz*Z3;

    return sign(D);
 }
*/



bool collinear(const d3_rat_point& a, const d3_rat_point& b,
                                      const d3_rat_point& c)
{ 
  //rat_vector v = cross_product(b-a,c-a);
  //return  v[0] == 0 && v[1] == 0 && v[2] == 0;

  integer AX = a.X();
  integer AY = a.Y();
  integer AZ = a.Z();
  integer AW = a.W();

  integer BX = b.X();
  integer BY = b.Y();
  integer BZ = b.Z();
  integer BW = b.W();

  integer CX = c.X();
  integer CY = c.Y();
  integer CZ = c.Z();
  integer CW = c.W();

  integer X1 = BX*AW - AX*BW;
  integer Y1 = BY*AW - AY*BW;
  integer Z1 = BZ*AW - AZ*BW;
  integer X2 = CX*AW - AX*CW;
  integer Y2 = CY*AW - AY*CW;
  integer Z2 = CZ*AW - AZ*CW;

  integer cx = Z1*Y2 - Y1*Z2;
  integer cy = X1*Z2 - Z1*X2;
  integer cz = Y1*X2 - X1*Y2;
  //integer cw = AW*BW*AW*CW;
  return  cx == 0 && cy == 0 && cz == 0;
 }





int side_of_sphere_old(const d3_rat_point& a, const d3_rat_point& b, 
                                          const d3_rat_point& c, 
                                          const d3_rat_point& d,
                                          const d3_rat_point& e)
{ 
  d3_rat_point::sos_count++;

/*
  if (d3_rat_point::use_filter != 0)
*/

  d3_rat_point::exact_sos_count++;

  integer AX = a.X();
  integer AY = a.Y();
  integer AZ = a.Z();
  integer AW = a.W();

  integer BX = b.X();
  integer BY = b.Y();
  integer BZ = b.Z();
  integer BW = b.W();

  integer CX = c.X();
  integer CY = c.Y();
  integer CZ = c.Z();
  integer CW = c.W();

  integer DX = d.X();
  integer DY = d.Y();
  integer DZ = d.Z();
  integer DW = d.W();

  integer EX = e.X();
  integer EY = e.Y();
  integer EZ = e.Z();
  integer EW = e.W();

  integer bx,by,bz,bw,cx,cy,cz,cw,dx,dy,dz,dw,ex,ey,ez,ew;

  if (AW==1 && BW==1 && CW==1 && DW==1 && EW == 1)
  { bx = BX - AX;
    by = BY - AY;
    bz = BZ - AZ;
    bw = bx*bx + by*by + bz*bz;

    cx = CX - AX;
    cy = CY - AY;
    cz = CZ - AZ;
    cw = cx*cx + cy*cy + cz*cz;
    
    dx = DX - AX;
    dy = DY - AY;
    dz = DZ - AZ;
    dw = dx*dx + dy*dy + dz*dz;

    ex = EX - AX;
    ey = EY - AY;
    ez = EZ - AZ;
    ew = ex*ex + ey*ey + ez*ez;
   }
  else
  { integer b1 = BX*AW - AX*BW;
    integer b2 = BY*AW - AY*BW;
    integer b3 = BZ*AW - AZ*BW;
    integer c1 = CX*AW - AX*CW;
    integer c2 = CY*AW - AY*CW;
    integer c3 = CZ*AW - AZ*CW;
    integer d1 = DX*AW - AX*DW;
    integer d2 = DY*AW - AY*DW;
    integer d3 = DZ*AW - AZ*DW;
    integer e1 = EX*AW - AX*EW;
    integer e2 = EY*AW - AY*EW;
    integer e3 = EZ*AW - AZ*EW;

    bx = b1 * AW * BW;
    by = b2 * AW * BW;
    bz = b3 * AW * BW;
    bw = b1*b1 + b2*b2 + b3*b3;

    cx = c1 * AW * CW;
    cy = c2 * AW * CW;
    cz = c3 * AW * CW;
    cw = c1*c1 + c2*c2 + c3*c3;

    dx = d1 * AW * DW;
    dy = d2 * AW * DW;
    dz = d3 * AW * DW;
    dw = d1*d1 + d2*d2 + d3*d3;

    ex = e1 * AW * EW;
    ey = e2 * AW * EW;
    ez = e3 * AW * EW;
    ew = e1*e1 + e2*e2 + e3*e3;
   }

  return   orientation(d3_rat_point(bx,by,bz,bw),
                       d3_rat_point(cx,cy,cz,cw),
                       d3_rat_point(dx,dy,dz,dw),
                       d3_rat_point(ex,ey,ez,ew));
}


int region_of_sphere(const d3_rat_point& a, const d3_rat_point& b, 
                                            const d3_rat_point& c, 
                                            const d3_rat_point& d,
                                            const d3_rat_point& x)
{ int orient  = orientation(a,b,c,d);
  if (orient == 0)
     LEDA_EXCEPTION(1,"region_of_sphere: degenerate sphere.");
  return orient * side_of_sphere(a,b,c,d,x); 
 }



bool contained_in_simplex(const d3_rat_point& a, const d3_rat_point& b,
                                                 const d3_rat_point& c,
                                                 const d3_rat_point& d,
                                                 const d3_rat_point& x)
{ int orient1 = orientation(a,b,c,d);
  if (orient1 == 0)
    LEDA_EXCEPTION(1,"contained_in_simplex: degenerate simplex.");
  if (orientation(a,b,c,x) == -orient1) return false;
  if (orientation(d,c,b,x) == -orient1) return false;
  if (orientation(d,b,a,x) == -orient1) return false;
  if (orientation(d,a,c,x) == -orient1) return false;
  return true;
}



bool contained_in_simplex(const array<d3_rat_point>& A, const d3_rat_point& x)
{ int l = A.low();
  int d = A.high() - l + 1;

  switch (d) {

  case 4: return contained_in_simplex(A[l],A[l+1],A[l+2],A[l+3],x); 

  case 3: { const d3_rat_point& a = A[l];
            const d3_rat_point& b = A[l+1];
            const d3_rat_point& c = A[l+2];
            d3_rat_point d = point_on_positive_side(a,b,c);
            if (orientation(a,b,c,x) != 0) return false;
            return contained_in_simplex(a,b,c,d,x);
           }

  case 2: { const d3_rat_point& a = A[l];
            const d3_rat_point& b = A[l+1];
            if ( !collinear(a,b,x) ) return false;
            if (compare(a,b) < 0)
               return compare(a,x) <= 0 && compare(x,b) <= 0;
            else
               return compare(b,x) <= 0 && compare(x,a) <= 0;
           }

  case 1: return A[l] == x;


  default: LEDA_EXCEPTION(1,"contained_in_simplex: more than d+1 simplex points.");
           break;
  }

  return false;
}





static int compute_base(list<d3_rat_point>& L, array<d3_rat_point>& base)
{
  // precond: L is sorted

  d3_rat_point A = L.pop();
  base[0] = A;
  while(!L.empty() && L.head() == A) L.pop();

  if (L.empty()) return 0;
      
  d3_rat_point B = L.pop();
  base[1] = B;
  while(!L.empty() && collinear(A,B,L.head())) L.pop();

  if (L.empty()) return 1;

  d3_rat_point C = L.pop();
  base[2] = C;
  while(!L.empty() && orientation(A,B,C,L.head()) == 0) L.pop();

  if (L.empty()) return 2;

  d3_rat_point D = L.pop();
  base[3] = D;
  return 3;
}
  

int affine_rank(const list<d3_rat_point>& L)
{ list<d3_rat_point> L1 = L;
  L1.sort();
  array<d3_rat_point> base(4);
  return compute_base(L1,base);
}

int affine_rank(const array<d3_rat_point>& A)
{ list<d3_rat_point> L;
  d3_rat_point p;
  forall(p,A) L.append(p);
  L.sort();
  array<d3_rat_point> base(4);
  return compute_base(L,base);
}

bool affinely_independent(const list<d3_rat_point>& L)
{ return affine_rank(L) == L.size()-1; }

bool affinely_independent(const array<d3_rat_point>& A)
{ return affine_rank(A) == A.size()-1; }




bool contained_in_affine_hull(const list<d3_rat_point>& L, const d3_rat_point& x)
{ list<d3_rat_point> L1 = L;
  L1.sort();
  array<d3_rat_point> base(4);
  int d = compute_base(L1,base);
  switch (d) {
    case 0: return base[0] == x;
    case 1: return collinear(base[0],base[1],x);
    case 2: return orientation(base[0],base[1],base[2],x) == 0;
    case 3: return true;
  }
  LEDA_EXCEPTION(1,"conained_in_affine_hull: internal error.");
  return false;
}

bool contained_in_affine_hull(const array<d3_rat_point>& A, const d3_rat_point& x)
{ list<d3_rat_point> L;
  d3_rat_point p;
  forall(p,A) L.append(p);
  return contained_in_affine_hull(L,x);
}

  




static int d2_orientation(double ax,double ay,double aw,
                          double bx,double by,double bw,
                          double cx,double cy,double cw)
{ double aybw = ay*bw;
  double byaw = by*aw;
  double axcw = ax*cw;
  double cxaw = cx*aw;
  double axbw = ax*bw;
  double bxaw = bx*aw;
  double aycw = ay*cw;
  double cyaw = cy*aw;

  double E = (axbw-bxaw) * (aycw-cyaw) - (aybw-byaw) * (axcw-cxaw);

  FABS(aybw); FABS(byaw); FABS(axcw); FABS(cxaw);
  FABS(axbw); FABS(bxaw); FABS(aycw); FABS(cyaw);

  double eps = 40*((aybw+byaw)*(axcw+cxaw)+(axbw+bxaw)*(aycw+cyaw))*eps0;
  
  if (E > eps)  return  1;
  if (E < -eps) return -1;
  if (eps < 1)  return  0;

  return 2;
}
 

inline int d2_orientation(integer ax,integer ay,integer aw,
                          integer bx,integer by,integer bw,
                          integer cx,integer cy,integer cw)
{ integer D = (ax*bw-bx*aw) * (ay*cw-cy*aw) - (ay*bw-by*aw) * (ax*cw-cx*aw);
  return sign(D);
}



int orientation_xy(const d3_rat_point& a, const d3_rat_point& b, 
                                          const d3_rat_point& c)
{ if (d3_rat_point::use_filter != 0)
  { int D = d2_orientation(a.XD(),a.YD(),a.WD(),b.XD(),b.YD(),b.WD(),
                                                c.XD(),c.YD(),c.WD());
    if (D != 2) return D;
   }
  return d2_orientation(a.X(),a.Y(),a.W(),b.X(),b.Y(),b.W(), c.X(),c.Y(),c.W());
}


int orientation_yz(const d3_rat_point& a, const d3_rat_point& b, 
                                          const d3_rat_point& c)
{ if (d3_rat_point::use_filter != 0)
  { int D = d2_orientation(a.YD(),a.ZD(),a.WD(),b.YD(),b.ZD(),b.WD(),
                                                c.YD(),c.ZD(),c.WD());
    if (D != 2) return D;
   }
  return d2_orientation(a.Y(),a.Z(),a.W(),b.Y(),b.Z(),b.W(),c.Y(),c.Z(),c.W());
}


int orientation_xz(const d3_rat_point& a, const d3_rat_point& b, 
                                          const d3_rat_point& c)
{ if (d3_rat_point::use_filter != 0)
  { int D = d2_orientation(a.XD(),a.ZD(),a.WD(),b.XD(),b.ZD(),b.WD(),
                                                c.XD(),c.ZD(),c.WD());
    if (D != 2) return D;
   }
  return d2_orientation(a.X(),a.Z(),a.W(),b.X(),b.Z(),b.W(),c.X(),c.Z(),c.W());
}




d3_rat_point point_on_positive_side(const d3_rat_point& a, 
                                    const d3_rat_point& b,
                                    const d3_rat_point& c)
{ integer ax =  a.X(); 
  integer bx =  b.X(); 
  integer cx =  c.X(); 

  integer ay =  a.Y();
  integer by =  b.Y();
  integer cy =  c.Y();

  integer az =  a.Z();
  integer bz =  b.Z();
  integer cz =  c.Z();

  integer aw =  a.W();
  integer bw =  b.W();
  integer cw =  c.W();
   
  integer X1 = cx*aw - ax*cw;
  integer Y1 = cy*aw - ay*cw;
  integer Z1 = cz*aw - az*cw;
  integer W1 = aw*cw;

  integer X2 = bx*aw - ax*bw;
  integer Y2 = by*aw - ay*bw;
  integer Z2 = bz*aw - az*bw;
  integer W2 = aw*bw;

  integer x = Z1*Y2 - Y1*Z2;
  integer y = X1*Z2 - Z1*X2;
  integer z = Y1*X2 - X1*Y2;
  integer w = W1*W2;

  return a.translate(x,y,z,w);
}


void d3_rat_point::print_statistics()
{
  float cmp_percent = 0;
  float orient_percent = 0;
  float sos_percent = 0;
  
  if (cmp_count > 0) 
      cmp_percent= 100*float(exact_cmp_count)/cmp_count;
  
  if (orient_count > 0) 
      orient_percent= 100*float(exact_orient_count)/orient_count;
  
  if (sos_count > 0) 
      sos_percent= 100*float(exact_sos_count)/sos_count;
  
  cout << endl;
  cout << string("compare:        %6d /%6d   (%.2f %%)",
                  exact_cmp_count,cmp_count,cmp_percent) << endl;
  cout << string("orientation:    %6d /%6d   (%.2f %%)",
                  exact_orient_count,orient_count,orient_percent) << endl;
  cout << string("side of sphere: %6d /%6d   (%.2f %%)",
                  exact_sos_count,sos_count,sos_percent) << endl;
  cout << endl;
}
  

LEDA_END_NAMESPACE
