/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _rat_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// rat_points :  points with rational (homogeneous) coordinates
//
// by S. Naeher (1995)
//------------------------------------------------------------------------------


#include <LEDA/geo/rat_point.h>
#include <LEDA/numbers/expcomp.h>


// the fabs function is used very often therefore we use the provide
// a fast version that simply clears the sign bit to zero  
//
// FABS(x) clears the sign bit of (double) floating point number x

#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ <= 2
#define FABS(x) x = fp::abs(x)
#else
// causes internal compiler error in g++-3.0.1 (s.n. August 2001)
// and 3.0.2 (s.n. november 2001)
#define FABS(x) fp::clear_sign_bit(x)
#endif

LEDA_BEGIN_NAMESPACE 

atomic_counter rat_point_rep::id_counter(0);


// static members


int rat_point::orient_count= 0;
int rat_point::exact_orient_count = 0;
int rat_point::error_orient_count = 0;

int rat_point::cmp_count= 0;
int rat_point::exact_cmp_count = 0;
int rat_point::error_cmp_count = 0;

int rat_point::soc_count= 0;
int rat_point::exact_soc_count = 0;
int rat_point::error_soc_count = 0;

// use_filter flag controls whether the floating point filter is used 

int rat_point::use_filter = 1;
int rat_point::float_computation_only = 0;

int rat_point::default_precision = 0;


bool   rat_point::use_static_filter = true;

double rat_point::x_abs_max = 0;
double rat_point::y_abs_max = 0;
double rat_point::w_abs_max = 1;

double rat_point::soc_eps_static = -1;
double rat_point::ori_eps_static = -1;


void rat_point::update_abs_bounds(double x, double y, double w)
{ FABS(x); FABS(y);
  if (x > x_abs_max) { x_abs_max = x; ori_eps_static = soc_eps_static = -1; }
  if (y > y_abs_max) { y_abs_max = y; ori_eps_static = soc_eps_static = -1; }
  if (w > w_abs_max) { w_abs_max = w; ori_eps_static = soc_eps_static = -1; }
}


// constructors

   rat_point_rep::rat_point_rep() : x(0), y(0), w(1)
   { xd = 0;
     yd = 0;
     wd = 1;
     id = ++id_counter;
    }

  rat_point_rep::rat_point_rep(const rational& a, const rational& b) 
       : x(a.numerator()*b.denominator()),
         y(b.numerator()*a.denominator()),
         w(a.denominator()*b.denominator())
   { 
     integer g = gcd(gcd(x,y),w); 
     if (g > 1) { x = x/g; y = y/g; w = w/g; }
     xd = x.to_double(); 
     yd = y.to_double(); 
     wd = w.to_double(); 
     rat_point::update_abs_bounds(xd,yd,wd);
     id = ++id_counter;
   }
   

   rat_point_rep::rat_point_rep(integer a, integer b) : x(a), y(b), w(1)
   { xd = a.to_double(); 
     yd = b.to_double(); 
     wd = 1;
     rat_point::update_abs_bounds(xd,yd,wd);
     id = ++id_counter;
    }

   rat_point_rep::rat_point_rep(integer a, integer b, integer c) 
                                 : x(a), y(b), w(c)
   { if (c < 0)
     { x = -a;
       y = -b;
       w = -c;
      }
     xd = x.to_double(); 
     yd = y.to_double(); 
     wd = w.to_double();
     rat_point::update_abs_bounds(xd,yd,wd);
     id = ++id_counter;
    }


rat_point::rat_point(const point& p, int prec)
{ 
  double x = p.xcoord();
  double y = p.ycoord();
 
  if ( prec > 0 )
  { double P = ldexp(1.0,prec);
    PTR = new rat_point_rep(integer(P*x),integer(P*y),integer(P));
    return;
  }
  rational xr(x), yr(y); // denominators are powers of two
  integer XX, YY, WW;
  if ( xr.denominator() == yr.denominator() )
  { XX = xr.numerator();
    YY = yr.numerator();
    WW = xr.denominator();
  }
  else
    if ( xr.denominator() < yr.denominator() )
    { integer quot = yr.denominator()/xr.denominator();
      WW = yr.denominator();
      XX = xr.numerator() * quot;
      YY = yr.numerator();
    }
    else
    { integer quot = xr.denominator()/yr.denominator();
      WW = xr.denominator();
      XX = xr.numerator();
      YY = yr.numerator() * quot;
    }
    
  PTR = new rat_point_rep(XX,YY,WW);
}


rat_point::rat_point(double x, double y, int prec)
{ rat_point p(point(x,y),prec);
  PTR = new rat_point_rep(p.X(),p.Y(),p.W());
}



void rat_point::normalize() const
{ integer x = X();
  integer y = Y();
  integer w = W();
  integer g = gcd(gcd(x,y),w);
  x /= g;
  y /= g;
  w /= g;
  ptr()->x = x;
  ptr()->y = y;
  ptr()->w = w;

  // s.n. (25.10.05): double coordinates must be updated !!!

  ptr()->xd = x.to_double(); 
  ptr()->yd = y.to_double(); 
  ptr()->wd = w.to_double(); 

  rat_point::update_abs_bounds(XD(),YD(),WD());
}

rat_point rat_point::rotate90(const rat_point& p, int i) const
{ 
  int r = i % 4;

  if (r == 0) return *this;
  if (r < 0) r += 4; // on some platforms the result of modulo (i.e. %) may be negative
  if (r == 2) return reflect(p);

  integer px = p.X();
  integer py = p.Y();
  integer pw = p.W();

  integer x = X();
  integer y = Y();
  integer w = W();

  integer rx,ry;

  if (r == 1)
  { rx = (px + py) * w - y * pw;
    ry = (py - px) * w + x * pw;
   }
  else // r == 3
  { rx = (px - py) * w + y * pw;
    ry = (py + px) * w - x * pw;
   }

  return rat_point(rx,ry,pw*w);
}


rat_point rat_point::rotate90(int i) const
{ return rotate90(rat_point(0,0,1),i); }



rat_point rat_point::reflect(const rat_point& q) const
{ // reflect point across point q

  integer x1 = X();
  integer y1 = Y();
  integer w1 = W();
  integer x2 = 2*q.X();
  integer y2 = 2*q.Y();
  integer w2 = q.W();

  return rat_point(x2*w1-x1*w2 , y2*w1-y1*w2, w1*w2); // Kurt 4.8.98
}




rat_point rat_point::reflect(const rat_point& p, const rat_point& q) const
{  // reflect point across line through p and q

  integer dx = p.X();
  integer dy = p.Y();
  integer dw = p.W();

  rat_point p0 = translate(-dx,-dy,dw);
  rat_point q0 = q.translate(-dx,-dy,dw);

  integer x1 = p0.X();
  integer y1 = p0.Y();
  integer w1 = p0.W();

  integer x2 = q0.X();
  integer y2 = q0.Y();

  integer cosfi = (x1*x2 + y1*y2);
  integer sinfi = (x1*y2 - x2*y1); 

  integer cos2 = (cosfi*cosfi - sinfi*sinfi);
  integer sin2 = 2*cosfi*sinfi;
  integer W    = w1 * (x1*x1 + y1*y1) * (x2*x2 + y2*y2);

  rat_point r0(x1*cos2-y1*sin2,x1*sin2+y1*cos2,W);

  return r0.translate(dx,dy,dw);
}
  

rat_point rat_point::translate(integer dx, integer dy, integer dw) const
{ integer x1 = X();
  integer y1 = Y();
  integer w1 = W();
  integer x = dx * w1 + x1 * dw;
  integer y = dy * w1 + y1 * dw;
  return rat_point(x,y,dw*w1);
 }

rat_point rat_point::translate(const rational& dx, const rational& dy) const
{ integer x = dx.numerator() * dy.denominator();
  integer y = dy.numerator() * dx.denominator();
  integer w = dx.denominator() * dy.denominator();
  return translate(x,y,w);
}

rational rat_point::sqr_dist(const rat_point& p) const
{ 
  integer x1 = X();
  integer y1 = Y();
  integer w1 = W();
  integer x2 = p.X();
  integer y2 = p.Y();
  integer w2 = p.W();

  integer dx = x1*w2 - x2*w1;
  integer dy = y1*w2 - y2*w1;

  return rational(dx*dx+dy*dy,w1*w1*w2*w2);
}

/*
int rat_point::cmp_dist(const rat_point& q, const rat_point& r) const
{ integer x1 = X();
  integer y1 = Y();
  integer w1 = W();
  integer x2 = q.X();
  integer y2 = q.Y();
  integer w2 = q.W();
  integer x3 = r.X();
  integer y3 = r.Y();
  integer w3 = r.W();
  integer dx2 = x1*w2 - x2*w1;
  integer dy2 = y1*w2 - y2*w1;
  integer dx3 = x1*w3 - x3*w1;
  integer dy3 = y1*w3 - y3*w1;
  integer RES = (dx2*dx2 + dy2*dy2)*w3*w3 - (dx3*dx3 + dy3*dy3)*w2*w2;
   return sign(RES);
}
*/



int rat_point::cmp_dist(const rat_point& q, const rat_point& r) const
{
int sgn = EXCOMP_NO_IDEA;

if (use_filter)
{
// ******** Begin Expression Compiler Output
// **** Local Variables:
//              RES
//              dx2
//              dx3
//              dy2
//              dy3
//              w1
//              w2
//              w3
//              x1
//              x2
//              x3
//              y1
//              y2
//              y3
// **** External Variables:
//              W()
//              X()
//              Y()
//              q.W()
//              q.X()
//              q.Y()
//              r.W()
//              r.X()
//              r.Y()
// **** Sign Result Variables:
//              sgn

{
        // ****** Double evaluation
double W___g_val = WD();
double W___g_sup = fp::abs(W___g_val);
double X___g_val = XD();
double X___g_sup = fp::abs(X___g_val);
double Y___g_val = YD();
double Y___g_sup = fp::abs(Y___g_val);
double q_W___g_val = q.WD();
double q_W___g_sup = fp::abs(q_W___g_val);
double q_X___g_val = q.XD();
double q_X___g_sup = fp::abs(q_X___g_val);
double q_Y___g_val = q.YD();
double q_Y___g_sup = fp::abs(q_Y___g_val);
double r_W___g_val = r.WD();
double r_W___g_sup = fp::abs(r_W___g_val);
double r_X___g_val = r.XD();
double r_X___g_sup = fp::abs(r_X___g_val);
double r_Y___g_val = r.YD();
double r_Y___g_sup = fp::abs(r_Y___g_val);




        // ******************* New Declaration
        double x1_val;
        double x1_sup;

        x1_val = X___g_val;
        x1_sup = X___g_sup;

        // ******************* New Declaration
        double y1_val;
        double y1_sup;

        y1_val = Y___g_val;
        y1_sup = Y___g_sup;

        // ******************* New Declaration
        double w1_val;
        double w1_sup;

        w1_val = W___g_val;
        w1_sup = W___g_sup;

        // ******************* New Declaration
        double x2_val;
        double x2_sup;

        x2_val = q_X___g_val;
        x2_sup = q_X___g_sup;

        // ******************* New Declaration
        double y2_val;
        double y2_sup;

        y2_val = q_Y___g_val;
        y2_sup = q_Y___g_sup;

        // ******************* New Declaration
        double w2_val;
        double w2_sup;

        w2_val = q_W___g_val;
        w2_sup = q_W___g_sup;

        // ******************* New Declaration
        double x3_val;
        double x3_sup;

        x3_val = r_X___g_val;
        x3_sup = r_X___g_sup;

        // ******************* New Declaration
        double y3_val;
        double y3_sup;

        y3_val = r_Y___g_val;
        y3_sup = r_Y___g_sup;

        // ******************* New Declaration
        double w3_val;
        double w3_sup;

        w3_val = r_W___g_val;
        w3_sup = r_W___g_sup;

        // ******************* New Declaration
        double dx2_val;
        double dx2_sup;

        double tmp_11_val = x1_val*w2_val;
        double tmp_11_sup = x1_sup*w2_sup;

        double tmp_14_val = x2_val*w1_val;
        double tmp_14_sup = x2_sup*w1_sup;

        double tmp_15_val = tmp_11_val-tmp_14_val;
        double tmp_15_sup = tmp_11_sup+tmp_14_sup;

        dx2_val = tmp_15_val;
        dx2_sup = tmp_15_sup;

        // ******************* New Declaration
        double dy2_val;
        double dy2_sup;

        double tmp_18_val = y1_val*w2_val;
        double tmp_18_sup = y1_sup*w2_sup;

        double tmp_21_val = y2_val*w1_val;
        double tmp_21_sup = y2_sup*w1_sup;

        double tmp_22_val = tmp_18_val-tmp_21_val;
        double tmp_22_sup = tmp_18_sup+tmp_21_sup;

        dy2_val = tmp_22_val;
        dy2_sup = tmp_22_sup;

        // ******************* New Declaration
        double dx3_val;
        double dx3_sup;

        double tmp_25_val = x1_val*w3_val;
        double tmp_25_sup = x1_sup*w3_sup;

        double tmp_28_val = x3_val*w1_val;
        double tmp_28_sup = x3_sup*w1_sup;

        double tmp_29_val = tmp_25_val-tmp_28_val;
        double tmp_29_sup = tmp_25_sup+tmp_28_sup;

        dx3_val = tmp_29_val;
        dx3_sup = tmp_29_sup;

        // ******************* New Declaration
        double dy3_val;
        double dy3_sup;

        double tmp_32_val = y1_val*w3_val;
        double tmp_32_sup = y1_sup*w3_sup;

        double tmp_35_val = y3_val*w1_val;
        double tmp_35_sup = y3_sup*w1_sup;

        double tmp_36_val = tmp_32_val-tmp_35_val;
        double tmp_36_sup = tmp_32_sup+tmp_35_sup;

        dy3_val = tmp_36_val;
        dy3_sup = tmp_36_sup;

        // ******************* New Declaration
        double RES_val;
        double RES_sup;

        double tmp_39_val = dx2_val*dx2_val;
        double tmp_39_sup = dx2_sup*dx2_sup;

        double tmp_42_val = dy2_val*dy2_val;
        double tmp_42_sup = dy2_sup*dy2_sup;

        double tmp_43_val = tmp_39_val+tmp_42_val;
        double tmp_43_sup = tmp_39_sup+tmp_42_sup;

        double tmp_45_val = tmp_43_val*w3_val;
        double tmp_45_sup = tmp_43_sup*w3_sup;

        double tmp_47_val = tmp_45_val*w3_val;
        double tmp_47_sup = tmp_45_sup*w3_sup;

        double tmp_50_val = dx3_val*dx3_val;
        double tmp_50_sup = dx3_sup*dx3_sup;

        double tmp_53_val = dy3_val*dy3_val;
        double tmp_53_sup = dy3_sup*dy3_sup;

        double tmp_54_val = tmp_50_val+tmp_53_val;
        double tmp_54_sup = tmp_50_sup+tmp_53_sup;

        double tmp_56_val = tmp_54_val*w2_val;
        double tmp_56_sup = tmp_54_sup*w2_sup;

        double tmp_58_val = tmp_56_val*w2_val;
        double tmp_58_sup = tmp_56_sup*w2_sup;

        double tmp_59_val = tmp_47_val-tmp_58_val;
        double tmp_59_sup = tmp_47_sup+tmp_58_sup;

        RES_val = tmp_59_val;
        RES_sup = tmp_59_sup;

        // ***************** Sign Statement
        // ******* BitLength of Expression is 0
        // ******* Index of Expression is 15
        {
          static double _upper=15*EXCOMP_eps1;
          if ( fp::abs(RES_val) > RES_sup*_upper*EXCOMP_correction )
                sgn = fp::sign(RES_val);
          else if ( RES_sup*_upper*EXCOMP_correction<1)
                sgn = 0;
          else
                sgn = EXCOMP_NO_IDEA;
        }
}
}
if ((sgn == EXCOMP_NO_IDEA) || EXCOMP_FALSE )
        // ***** Exact Evaluation
        {
        integer x1 = X();
        integer y1 = Y();
        integer w1 = W();
        integer x2 = q.X();
        integer y2 = q.Y();
        integer w2 = q.W();
        integer x3 = r.X();
        integer y3 = r.Y();
        integer w3 = r.W();
        integer dx2 = ((x1*w2)-(x2*w1));
        integer dy2 = ((y1*w2)-(y2*w1));
        integer dx3 = ((x1*w3)-(x3*w1));
        integer dy3 = ((y1*w3)-(y3*w1));
        integer RES = (((((dx2*dx2)+(dy2*dy2))*w3)*w3)-((((dx3*dx3)+(dy3*dy3))*w2)*w2));
        sgn = sign(RES);
        }
// ******** End Expression Compiler Output

  return sgn;
}









rational rat_point::xdist(const rat_point& p) const
{ integer x1  = X();
  integer w1  = W();
  integer x2  = p.X();
  integer w2  = p.W();
  return rational(abs(x1*w2-x2*w1), w1*w2);
}

rational rat_point::ydist(const rat_point& p) const
{ integer y1  = Y();
  integer w1  = W();
  integer y2  = p.Y();
  integer w2  = p.W();
  return rational(abs(y1*w2-y2*w1), w1*w2);
}



// basic stream I/O operations

ostream& operator<<(ostream& out, const rat_point& p)
{ out << "(" << p.X() << "," << p.Y() << "," << p.W() << ")";
  return out;
 } 

istream& operator>>(istream& in, rat_point& p) 
{ // syntax: {(} x {,} y {,} w {)}   

  integer x,y,w;
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

  in >> w; 

  do in.get(c); while (c == ' ');
  if (c != ')') in.putback(c);

  p = rat_point(x,y,w ); 
  return in; 

 } 



// machine precision

const double point_eps0 = ldexp(1.0,-53);


// cmp - primitive

int rat_point::cmp_xy(const rat_point& a, const rat_point& b)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::cmp_count++;
#endif

  double E = 0;

  if (use_filter != 0)
  { 
    double axbw = a.XD()*b.WD();
    double bxaw = b.XD()*a.WD();

    E = axbw - bxaw;       // floating point result

    //----------------------------------------------------------------
    // ERROR BOUND:
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
    double eps = 4 * (axbw+bxaw) * point_eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
   
    if (eps < 1)  // compare y-coordinates
    { double aybw = a.YD()*b.WD();
      double byaw = b.YD()*a.WD();
      E    = aybw - byaw;
      FABS(aybw);
      FABS(byaw);
      double eps = 4 * (aybw+byaw) * point_eps0;
      if (E > +eps) return +1;
      if (E < -eps) return -1;
      if (eps < 1)  return  0; 
     }
   }
  
    //use big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
    rat_point::exact_cmp_count++;
#endif

    int result = 0;

    integer axbw = a.X()*b.W();
    integer bxaw = b.X()*a.W();
    if (axbw > bxaw) 
      result = 1;
    else
      if (axbw < bxaw) 
         result = -1;
      else
       { integer aybw = a.Y()*b.W();
         integer byaw = b.Y()*a.W();
         if (aybw > byaw) 
           result = 1;
         else
           if (aybw < byaw) result = -1;
        }

#if !defined(LEDA_MULTI_THREAD)
    if (result != fp::sign(E)) error_cmp_count++;
#endif

    return result;
}


int rat_point::cmp_xy_F(const rat_point& a, const rat_point& b)
{ 
  double axbw = a.XD()*b.WD();
  double bxaw = b.XD()*a.WD();
  if (axbw > bxaw) return  1;
  if (axbw < bxaw) return -1;

  double aybw = a.YD()*b.WD();
  double byaw = b.YD()*a.WD();
  if (aybw > byaw) return  1;
  if (aybw < byaw) return -1;

  return 0;
}


int rat_point::cmp_x(const rat_point& a, const rat_point& b)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::cmp_count++;
#endif

  if (use_filter != 0)
  { 
    double axbw = a.XD()*b.WD();
    double bxaw = b.XD()*a.WD();
    double E    = axbw - bxaw;       // floating point result

    //----------------------------------------------------------------
    // ERROR BOUND:
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
    double eps = 4 * (axbw+bxaw) * point_eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
  }
  
    //use big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
    rat_point::exact_cmp_count++;
#endif

    integer axbw = a.X()*b.W();
    integer bxaw = b.X()*a.W();
    if (axbw > bxaw) return  1;
    if (axbw < bxaw) return -1;

    return 0;
}


int rat_point::cmp_yx(const rat_point& a, const rat_point& b)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::cmp_count++;
#endif

  if (use_filter != 0)
  { 
    double aybw = a.YD()*b.WD();
    double byaw = b.YD()*a.WD();
    double E    = aybw - byaw;       // floating point result

    FABS(aybw);
    FABS(byaw);
    double eps = 4 * (aybw+byaw) * point_eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
   
    if (eps < 1)  // compare x-coordinates
    { double axbw = a.XD()*b.WD();
      double bxaw = b.XD()*a.WD();
      double E    = axbw - bxaw;
      FABS(axbw);
      FABS(bxaw);
      double eps = 4 * (axbw+bxaw) * point_eps0;
      if (E > +eps) return +1;
      if (E < -eps) return -1;
      if (eps < 1)  return  0; 
     }
   }
  
    //use big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
    rat_point::exact_cmp_count++;
#endif

    integer aybw = a.Y()*b.W();
    integer byaw = b.Y()*a.W();
    if (aybw > byaw) return  1;
    if (aybw < byaw) return -1;

    integer axbw = a.X()*b.W();
    integer bxaw = b.X()*a.W();
    if (axbw > bxaw) return  1;
    if (axbw < bxaw) return -1;
    return 0;
}



int rat_point::cmp_y(const rat_point& a, const rat_point& b)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::cmp_count++;
#endif

  if (use_filter != 0)
  { 
    double aybw = a.YD()*b.WD();
    double byaw = b.YD()*a.WD();
    double E    = aybw - byaw;       // floating point result

    FABS(aybw);
    FABS(byaw);
    double eps = 4 * (aybw+byaw) * point_eps0;

    if (E > +eps) return +1;
    if (E < -eps) return -1;
   }
  
    //use big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
    rat_point::exact_cmp_count++;
#endif

    integer aybw = a.Y()*b.W();
    integer byaw = b.Y()*a.W();
    if (aybw > byaw) return  1;
    if (aybw < byaw) return -1;

    return 0;
}







rational rat_point::area(const rat_point& q, const rat_point& r) const
{ return ((xcoord()-q.xcoord()) * (ycoord()-r.ycoord()) -
          (ycoord()-q.ycoord()) * (xcoord()-r.xcoord()))/2; }



int cmp_signed_dist(const rat_point& a, const rat_point& b, const rat_point& c,
                                                            const rat_point& d)
{  
  if (rat_point::use_filter != 0)
  { 
    double ax =  a.ptr()->xd; 
    double bx =  b.ptr()->xd; 
    double cx =  c.ptr()->xd; 
    double dx =  d.ptr()->xd; 

    double ay =  a.ptr()->yd;
    double by =  b.ptr()->yd;
    double cy =  c.ptr()->yd;
    double dy =  d.ptr()->yd; 

    double aw =  a.ptr()->wd;
    double bw =  b.ptr()->wd;
    double cw =  c.ptr()->wd;
    double dw =  d.ptr()->wd; 

    double axbw = ax*bw;
    double bxaw = bx*aw;
    double dycw = dy*cw;
    double cydw = cy*dw;

    double aybw = ay*bw;
    double byaw = by*aw;
    double dxcw = dx*cw;
    double cxdw = cx*dw;

    double E = (axbw-bxaw) * (dycw-cydw) - (aybw-byaw) * (dxcw-cxdw);

    FABS(axbw);
    FABS(bxaw);
    FABS(dycw);
    FABS(cydw);

    FABS(aybw);
    FABS(byaw);
    FABS(dxcw);
    FABS(cxdw);

    double eps = 40*((aybw+byaw)*(dxcw+cxdw)+(axbw+bxaw)*(dycw+cydw))*point_eps0;
    if (E > eps)  return  1;
    if (E < -eps) return -1;
    if (eps < 1)  return  0;
   }

   // big integer arithmetic

   integer AX = a.X(); integer AY = a.Y(); integer AW = a.W();
   integer BX = b.X(); integer BY = b.Y(); integer BW = b.W();
   integer CX = c.X(); integer CY = c.Y(); integer CW = c.W();
   integer DX = d.X(); integer DY = d.Y(); integer DW = d.W();
   integer D = (AX*BW-BX*AW) * (DY*CW-CY*DW) - (AY*BW-BY*AW) * (DX*CW-CX*DW);
   return sign(D);
}



int rat_point::orientation(const rat_point& b, const rat_point& c) const
{  

#if !defined(LEDA_MULTI_THREAD)
  rat_point::orient_count++;
#endif

  const rat_point& a = *this;

  double E=0;

  if (rat_point::use_filter != 0)
  { 
    double ax =  a.ptr()->xd; 
    double bx =  b.ptr()->xd; 
    double cx =  c.ptr()->xd; 
    double ay =  a.ptr()->yd;
    double by =  b.ptr()->yd;
    double cy =  c.ptr()->yd;
    double aw =  a.ptr()->wd;
    double bw =  b.ptr()->wd;
    double cw =  c.ptr()->wd;

/*
if (rat_point::float_computation_only && aw+bw+cw == 3)
{ E = bx*(cy-ay) + by*(ax-cx) + (ay*cx-ax*cy); 
  if ( E > 0 ) return 1;
  if ( E < 0 ) return -1;
  return 0;
}
*/
    
    double aycx = ay*cx;
    double axcy = ax*cy;
    double axcw = ax*cw;
    double cxaw = cx*aw;
    double aycw = ay*cw;
    double cyaw = cy*aw;    
    
    // old predicate:
    // double E = (ax*bw-bx*aw) * (ay*cw-cy*aw) - (ay*bw-by*aw) * (ax*cw-cx*aw);
    //
    // double E = ax*bw*ay*cw - bx*aw*ay*cw - ax*bw*cy*aw + bx*aw*cy*aw -
    //           (ay*bw*ax*cw - by*aw*ax*cw - ay*bw*cx*aw + by*aw*cx*aw)
    //
    // double E = - bx*aw*ay*cw - ax*bw*cy*aw + bx*aw*cy*aw -
    //            + by*aw*ax*cw + ay*bw*cx*aw - by*aw*cx*aw
    //     
    // aw is always positive (see definition of rat_point)
    // we only want to know the sign of the expression, so we can divide by aw
    //
    // double E = - bx*ay*cw - ax*bw*cy + bx*aw*cy -
    //            + by*ax*cw + ay*bw*cx - by*cx*aw   
    //   

    E = bx*(cyaw-aycw) + by*(axcw-cxaw) + bw*(aycx-axcy); 
    
    if ( rat_point::float_computation_only != 0 )
    { if ( E > 0 ) return 1;
      if ( E < 0 ) return -1;
      return 0;
    }    
    
    if (rat_point::use_static_filter) 
    {
      if (rat_point::ori_eps_static == -1)
        rat_point::set_static_orientation_error_bound(rat_point::x_abs_max,
                                                      rat_point::y_abs_max,
                                                      rat_point::w_abs_max);
      if (E >  rat_point::ori_eps_static) return  1;
      if (E < -rat_point::ori_eps_static) return -1; 
    }    
    
    FABS(bx); FABS(by);
    FABS(aycx); FABS(axcy);
    FABS(axcw); FABS(cxaw);
    FABS(aycw); FABS(cyaw);  
    
    // ind(E) :
    // i1 = ind(cyaw+aycw) < 4
    // i2 = ind(axcw+cxaw) < 4
    // i3 = ind(aycx+axcy) < 4
    // ind(bx*(cyaw+aycw)) < 7
    // ind(by*(axcw+cxaw)) < 7
    // ind(bw*(aycx+axcy)) < 7
    // ind(E)  < 11       
    
 double eps = 11*(bx*(cyaw+aycw) + by*(axcw+cxaw) + bw*(aycx+axcy))*point_eps0; 

#if defined(_MSC_VER)
   if(fp::is_finite(eps) && !fp::is_nan(E)) 
     // VC++ 7.1 (with full optimization) performs a wrong comparison 
     // if E == eps == infinity, thus fallback to
     //integer sign computation if eps is not finite or E is_nan.  (AC, 2004)
#endif
   { if (E > eps)  return  1;
     if (E < -eps) return -1;
     if (eps < 1)  return  0; 
    }
 }

   // big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
   rat_point::exact_orient_count++;
#endif

   integer AX = a.X(); integer AY = a.Y(); integer AW = a.W();
   integer BX = b.X(); integer BY = b.Y(); integer BW = b.W();
   integer CX = c.X(); integer CY = c.Y(); integer CW = c.W();
   
   integer D =  BX*(CY*AW-AY*CW)+BY*(AX*CW-CX*AW)+BW*(AY*CX-AX*CY); 

#if !defined(LEDA_MULTI_THREAD)
   if (sign(D) != fp::sign(E)) error_orient_count++;
#endif
   
   return sign(D);
}



int rat_point::orientation(const rat_point& b, const rat_point& c,
                           double aycx, double axcy, double axcw,
                           double cxaw, double aycw, double cyaw) const
{  

#if !defined(LEDA_MULTI_THREAD)
  rat_point::orient_count++;
#endif

  const rat_point& a = *this;

  double bx =  b.ptr()->xd; 
  double by =  b.ptr()->yd;
  double bw =  b.ptr()->wd;

  if (rat_point::use_filter != 0)
  { 
    double E = bx*(cyaw-aycw) + by*(axcw-cxaw) + bw*(aycx-axcy); 
    
    if ( rat_point::float_computation_only != 0 )
    { if ( E > 0 ) return 1;
      if ( E < 0 ) return -1;
      return 0;
    }    
    
    if (rat_point::use_static_filter) 
    {
      if (rat_point::ori_eps_static == -1)
        rat_point::set_static_orientation_error_bound(rat_point::x_abs_max,
                                                      rat_point::y_abs_max,
                                                      rat_point::w_abs_max);
      if (E >  rat_point::ori_eps_static) return  1;
      if (E < -rat_point::ori_eps_static) return -1; 
    }    
    
    FABS(bx); FABS(by);
    FABS(aycx); FABS(axcy);
    FABS(axcw); FABS(cxaw);
    FABS(aycw); FABS(cyaw);  
    
 double eps = 11*(bx*(cyaw+aycw) + by*(axcw+cxaw) + bw*(aycx+axcy))*point_eps0;

    if (E > eps)  return  1;
    if (E < -eps) return -1;
    if (eps < 1)  return  0; 
   }

   // big integer arithmetic

#if !defined(LEDA_MULTI_THREAD)
   rat_point::exact_orient_count++;
#endif

   integer AX = a.X(); integer AY = a.Y(); integer AW = a.W();
   integer BX = b.X(); integer BY = b.Y(); integer BW = b.W();
   integer CX = c.X(); integer CY = c.Y(); integer CW = c.W();
   
   integer D =  BX*(CY*AW-AY*CW)+BY*(AX*CW-CX*AW)+BW*(AY*CX-AX*CY); 
   
   return sign(D);
}



int side_of_halfspace(const rat_point& a, 
                      const rat_point& b, 
                      const rat_point& c)
{ 

  int sgn = EXCOMP_NO_IDEA;
    
if (rat_point::use_filter)
{ 
// ******** Begin Expression Compiler Output
// **** Local Variables:
//              AW
//              AX
//              AY
//              BAX
//              BAY
//              BW
//              BX
//              BY
//              CAX
//              CAY
//              CW
//              CX
//              CY
//              RES
// **** External Variables:
//              a.W()
//              a.X()
//              a.Y()
//              b.W()
//              b.X()
//              b.Y()
//              c.W()
//              c.X()
//              c.Y()
// **** Sign Result Variables:
//              sgn

{
        // ****** Double evaluation
double a_W___g_val = a.WD();
double a_W___g_sup = fp::abs(a_W___g_val);
double a_X___g_val = a.XD();
double a_X___g_sup = fp::abs(a_X___g_val);
double a_Y___g_val = a.YD();
double a_Y___g_sup = fp::abs(a_Y___g_val);
double b_W___g_val = b.WD();
double b_W___g_sup = fp::abs(b_W___g_val);
double b_X___g_val = b.XD();
double b_X___g_sup = fp::abs(b_X___g_val);
double b_Y___g_val = b.YD();
double b_Y___g_sup = fp::abs(b_Y___g_val);
double c_W___g_val = c.WD();
double c_W___g_sup = fp::abs(c_W___g_val);
double c_X___g_val = c.XD();
double c_X___g_sup = fp::abs(c_X___g_val);
double c_Y___g_val = c.YD();
double c_Y___g_sup = fp::abs(c_Y___g_val);




        // ******************* New Declaration
        double AX_val;
        double AX_sup;

        AX_val = a_X___g_val;
        AX_sup = a_X___g_sup;

        // ******************* New Declaration
        double AY_val;
        double AY_sup;

        AY_val = a_Y___g_val;
        AY_sup = a_Y___g_sup;

        // ******************* New Declaration
        double AW_val;
        double AW_sup;

        AW_val = a_W___g_val;
        AW_sup = a_W___g_sup;

        // ******************* New Declaration
        double BX_val;
        double BX_sup;

        BX_val = b_X___g_val;
        BX_sup = b_X___g_sup;

        // ******************* New Declaration
        double BY_val;
        double BY_sup;

        BY_val = b_Y___g_val;
        BY_sup = b_Y___g_sup;

        // ******************* New Declaration
        double BW_val;
        double BW_sup;

        BW_val = b_W___g_val;
        BW_sup = b_W___g_sup;

        // ******************* New Declaration
        double CX_val;
        double CX_sup;

        CX_val = c_X___g_val;
        CX_sup = c_X___g_sup;

        // ******************* New Declaration
        double CY_val;
        double CY_sup;

        CY_val = c_Y___g_val;
        CY_sup = c_Y___g_sup;

        // ******************* New Declaration
        double CW_val;
        double CW_sup;

        CW_val = c_W___g_val;
        CW_sup = c_W___g_sup;

        // ******************* New Declaration
        double BAX_val;
        double BAX_sup;

        double tmp_11_val = BX_val*AW_val;
        double tmp_11_sup = BX_sup*AW_sup;

        double tmp_14_val = AX_val*BW_val;
        double tmp_14_sup = AX_sup*BW_sup;

        double tmp_15_val = tmp_11_val-tmp_14_val;
        double tmp_15_sup = tmp_11_sup+tmp_14_sup;

        BAX_val = tmp_15_val;
        BAX_sup = tmp_15_sup;

        // ******************* New Declaration
        double BAY_val;
        double BAY_sup;

        double tmp_18_val = BY_val*AW_val;
        double tmp_18_sup = BY_sup*AW_sup;

        double tmp_21_val = AY_val*BW_val;
        double tmp_21_sup = AY_sup*BW_sup;

        double tmp_22_val = tmp_18_val-tmp_21_val;
        double tmp_22_sup = tmp_18_sup+tmp_21_sup;

        BAY_val = tmp_22_val;
        BAY_sup = tmp_22_sup;

        // ******************* New Declaration
        double CAX_val;
        double CAX_sup;

        double tmp_25_val = CX_val*AW_val;
        double tmp_25_sup = CX_sup*AW_sup;

        double tmp_28_val = AX_val*CW_val;
        double tmp_28_sup = AX_sup*CW_sup;

        double tmp_29_val = tmp_25_val-tmp_28_val;
        double tmp_29_sup = tmp_25_sup+tmp_28_sup;

        CAX_val = tmp_29_val;
        CAX_sup = tmp_29_sup;

        // ******************* New Declaration
        double CAY_val;
        double CAY_sup;

        double tmp_32_val = CY_val*AW_val;
        double tmp_32_sup = CY_sup*AW_sup;

        double tmp_35_val = AY_val*CW_val;
        double tmp_35_sup = AY_sup*CW_sup;

        double tmp_36_val = tmp_32_val-tmp_35_val;
        double tmp_36_sup = tmp_32_sup+tmp_35_sup;

        CAY_val = tmp_36_val;
        CAY_sup = tmp_36_sup;

        // ******************* New Declaration
        double RES_val;
        double RES_sup;

        double tmp_39_val = BAX_val*CAX_val;
        double tmp_39_sup = BAX_sup*CAX_sup;

        double tmp_42_val = BAY_val*CAY_val;
        double tmp_42_sup = BAY_sup*CAY_sup;

        double tmp_43_val = tmp_39_val+tmp_42_val;
        double tmp_43_sup = tmp_39_sup+tmp_42_sup;

        RES_val = tmp_43_val;
        RES_sup = tmp_43_sup;

        // ***************** Sign Statement
        // ******* BitLength of Expression is 0
        // ******* Index of Expression is 10
        {
          static double _upper=10*EXCOMP_eps1;
          if ( fp::abs(RES_val) > RES_sup*_upper*EXCOMP_correction )
                sgn = fp::sign(RES_val);
          else if ( RES_sup*_upper*EXCOMP_correction<1)
                sgn = 0;
          else
                sgn = EXCOMP_NO_IDEA;
        }
}
}
if ((sgn==EXCOMP_NO_IDEA) || EXCOMP_FALSE )
        // ***** Exact Evaluation
        {
        integer AX = a.X();
        integer AY = a.Y();
        integer AW = a.W();
        integer BX = b.X();
        integer BY = b.Y();
        integer BW = b.W();
        integer CX = c.X();
        integer CY = c.Y();
        integer CW = c.W();
        integer BAX = ((BX*AW)-(AX*BW));
        integer BAY = ((BY*AW)-(AY*BW));
        integer CAX = ((CX*AW)-(AX*CW));
        integer CAY = ((CY*AW)-(AY*CW));
        integer RES = ((BAX*CAX)+(BAY*CAY));
        sgn = sign(RES);
        }
// ******** End Expression Compiler Output

return sgn;

}




#if 0
// causes internal compiler error in g++-3.0.1  (s.n. August 2001)

int cmp_distances(const rat_point& p1, const rat_point& p2, 
                  const rat_point& p3, const rat_point& p4)
{
  int ressign = 0;
  
  if (rat_point::use_filter != 0)
    {
// ******** Begin Expression Compiler Output
// **** Local Variables:
//		dxa
//		dxb
//		dya
//		dyb
//		n1
//		n2
//		val
//		w1
//		w2
//		w3
//		w4
//		x1
//		x2
//		x3
//		x4
//		y1
//		y2
//		y3
//		y4
//		z1
//		z2
// **** External Variables:
//		p1.W()
//		p1.X()
//		p1.Y()
//		p2.W()
//		p2.X()
//		p2.Y()
//		p3.W()
//		p3.X()
//		p3.Y()
//		p4.W()
//		p4.X()
//		p4.Y()
// **** Sign Result Variables:
//		ressign

{
	// ****** Double evaluation
double p1_W___g_val = p1.WD();
double p1_W___g_sup = fp::abs(p1_W___g_val);
double p1_X___g_val = p1.XD();
double p1_X___g_sup = fp::abs(p1_X___g_val);
double p1_Y___g_val = p1.YD();
double p1_Y___g_sup = fp::abs(p1_Y___g_val);
double p2_W___g_val = p2.WD();
double p2_W___g_sup = fp::abs(p2_W___g_val);
double p2_X___g_val = p2.XD();
double p2_X___g_sup = fp::abs(p2_X___g_val);
double p2_Y___g_val = p2.YD();
double p2_Y___g_sup = fp::abs(p2_Y___g_val);
double p3_W___g_val = p3.WD();
double p3_W___g_sup = fp::abs(p3_W___g_val);
double p3_X___g_val = p3.XD();
double p3_X___g_sup = fp::abs(p3_X___g_val);
double p3_Y___g_val = p3.YD();
double p3_Y___g_sup = fp::abs(p3_Y___g_val);
double p4_W___g_val = p4.WD();
double p4_W___g_sup = fp::abs(p4_W___g_val);
double p4_X___g_val = p4.XD();
double p4_X___g_sup = fp::abs(p4_X___g_val);
double p4_Y___g_val = p4.YD();
double p4_Y___g_sup = fp::abs(p4_Y___g_val);




	// ******************* New Declaration
	double x1_val;
	double x1_sup;

	x1_val = p1_X___g_val;
	x1_sup = p1_X___g_sup;

	// ******************* New Declaration
	double y1_val;
	double y1_sup;

	y1_val = p1_Y___g_val;
	y1_sup = p1_Y___g_sup;

	// ******************* New Declaration
	double w1_val;
	double w1_sup;

	w1_val = p1_W___g_val;
	w1_sup = p1_W___g_sup;

	// ******************* New Declaration
	double x2_val;
	double x2_sup;

	x2_val = p2_X___g_val;
	x2_sup = p2_X___g_sup;

	// ******************* New Declaration
	double y2_val;
	double y2_sup;

	y2_val = p2_Y___g_val;
	y2_sup = p2_Y___g_sup;

	// ******************* New Declaration
	double w2_val;
	double w2_sup;

	w2_val = p2_W___g_val;
	w2_sup = p2_W___g_sup;

	// ******************* New Declaration
	double dxa_val;
	double dxa_sup;

	double tmp_8_val = x1_val*w2_val;
	double tmp_8_sup = x1_sup*w2_sup;

	double tmp_11_val = x2_val*w1_val;
	double tmp_11_sup = x2_sup*w1_sup;

	double tmp_12_val = tmp_8_val-tmp_11_val;
	double tmp_12_sup = tmp_8_sup+tmp_11_sup;

	dxa_val = tmp_12_val;
	dxa_sup = tmp_12_sup;

	// ******************* New Declaration
	double dya_val;
	double dya_sup;

	double tmp_15_val = y1_val*w2_val;
	double tmp_15_sup = y1_sup*w2_sup;

	double tmp_18_val = y2_val*w1_val;
	double tmp_18_sup = y2_sup*w1_sup;

	double tmp_19_val = tmp_15_val-tmp_18_val;
	double tmp_19_sup = tmp_15_sup+tmp_18_sup;

	dya_val = tmp_19_val;
	dya_sup = tmp_19_sup;

	// ******************* New Declaration
	double x3_val;
	double x3_sup;

	x3_val = p3_X___g_val;
	x3_sup = p3_X___g_sup;

	// ******************* New Declaration
	double y3_val;
	double y3_sup;

	y3_val = p3_Y___g_val;
	y3_sup = p3_Y___g_sup;

	// ******************* New Declaration
	double w3_val;
	double w3_sup;

	w3_val = p3_W___g_val;
	w3_sup = p3_W___g_sup;

	// ******************* New Declaration
	double x4_val;
	double x4_sup;

	x4_val = p4_X___g_val;
	x4_sup = p4_X___g_sup;

	// ******************* New Declaration
	double y4_val;
	double y4_sup;

	y4_val = p4_Y___g_val;
	y4_sup = p4_Y___g_sup;

	// ******************* New Declaration
	double w4_val;
	double w4_sup;

	w4_val = p4_W___g_val;
	w4_sup = p4_W___g_sup;

	// ******************* New Declaration
	double dxb_val;
	double dxb_sup;

	double tmp_28_val = x3_val*w4_val;
	double tmp_28_sup = x3_sup*w4_sup;

	double tmp_31_val = x4_val*w3_val;
	double tmp_31_sup = x4_sup*w3_sup;

	double tmp_32_val = tmp_28_val-tmp_31_val;
	double tmp_32_sup = tmp_28_sup+tmp_31_sup;

	dxb_val = tmp_32_val;
	dxb_sup = tmp_32_sup;

	// ******************* New Declaration
	double dyb_val;
	double dyb_sup;

	double tmp_35_val = y3_val*w4_val;
	double tmp_35_sup = y3_sup*w4_sup;

	double tmp_38_val = y4_val*w3_val;
	double tmp_38_sup = y4_sup*w3_sup;

	double tmp_39_val = tmp_35_val-tmp_38_val;
	double tmp_39_sup = tmp_35_sup+tmp_38_sup;

	dyb_val = tmp_39_val;
	dyb_sup = tmp_39_sup;

	// ******************* New Declaration
	double z1_val;
	double z1_sup;

	double tmp_42_val = dxa_val*dxa_val;
	double tmp_42_sup = dxa_sup*dxa_sup;

	double tmp_45_val = dya_val*dya_val;
	double tmp_45_sup = dya_sup*dya_sup;

	double tmp_46_val = tmp_42_val+tmp_45_val;
	double tmp_46_sup = tmp_42_sup+tmp_45_sup;

	z1_val = tmp_46_val;
	z1_sup = tmp_46_sup;

	// ******************* New Declaration
	double n1_val;
	double n1_sup;

	double tmp_49_val = w1_val*w1_val;
	double tmp_49_sup = w1_sup*w1_sup;

	double tmp_51_val = tmp_49_val*w2_val;
	double tmp_51_sup = tmp_49_sup*w2_sup;

	double tmp_53_val = tmp_51_val*w2_val;
	double tmp_53_sup = tmp_51_sup*w2_sup;

	n1_val = tmp_53_val;
	n1_sup = tmp_53_sup;

	// ******************* New Declaration
	double z2_val;
	double z2_sup;

	double tmp_56_val = dxb_val*dxb_val;
	double tmp_56_sup = dxb_sup*dxb_sup;

	double tmp_59_val = dyb_val*dyb_val;
	double tmp_59_sup = dyb_sup*dyb_sup;

	double tmp_60_val = tmp_56_val+tmp_59_val;
	double tmp_60_sup = tmp_56_sup+tmp_59_sup;

	z2_val = tmp_60_val;
	z2_sup = tmp_60_sup;

	// ******************* New Declaration
	double n2_val;
	double n2_sup;

	double tmp_63_val = w3_val*w3_val;
	double tmp_63_sup = w3_sup*w3_sup;

	double tmp_65_val = tmp_63_val*w4_val;
	double tmp_65_sup = tmp_63_sup*w4_sup;

	double tmp_67_val = tmp_65_val*w4_val;
	double tmp_67_sup = tmp_65_sup*w4_sup;

	n2_val = tmp_67_val;
	n2_sup = tmp_67_sup;

	// ******************* New Declaration
	double val_val;
	double val_sup;

	double tmp_70_val = z1_val*n2_val;
	double tmp_70_sup = z1_sup*n2_sup;

	double tmp_73_val = z2_val*n1_val;
	double tmp_73_sup = z2_sup*n1_sup;

	double tmp_74_val = tmp_70_val-tmp_73_val;
	double tmp_74_sup = tmp_70_sup+tmp_73_sup;

	val_val = tmp_74_val;
	val_sup = tmp_74_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 19
	{
	  static double _upper=19*EXCOMP_eps1;
	  if ( fp::abs(val_val) > val_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(val_val);
	  else if ( val_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}
}
if ((ressign==EXCOMP_NO_IDEA) || EXCOMP_FALSE || (rat_point::use_filter == 0))
	// ***** Exact Evaluation
	{
	integer x1 = p1.X();
	integer y1 = p1.Y();
	integer w1 = p1.W();
	integer x2 = p2.X();
	integer y2 = p2.Y();
	integer w2 = p2.W();
	integer dxa = ((x1*w2)-(x2*w1));
	integer dya = ((y1*w2)-(y2*w1));
	integer x3 = p3.X();
	integer y3 = p3.Y();
	integer w3 = p3.W();
	integer x4 = p4.X();
	integer y4 = p4.Y();
	integer w4 = p4.W();
	integer dxb = ((x3*w4)-(x4*w3));
	integer dyb = ((y3*w4)-(y4*w3));
	integer z1 = ((dxa*dxa)+(dya*dya));
	integer n1 = (((w1*w1)*w2)*w2);
	integer z2 = ((dxb*dxb)+(dyb*dyb));
	integer n2 = (((w3*w3)*w4)*w4);
	integer val = ((z1*n2)-(z2*n1));
	ressign = sign(val);
	}
// ******** End Expression Compiler Output

  
  
  return ressign;
}
#endif



int cmp_distances(const rat_point& p1, const rat_point& p2, 
                  const rat_point& p3, const rat_point& p4)
{
  int ressign = 0;
  
  if (rat_point::use_filter != 0)
  {
    double x1_val = p1.XD();
    double y1_val = p1.YD();
    double w1_val = p1.WD();
    
    double x2_val = p2.XD();
    double y2_val = p2.YD();
    double w2_val = p2.WD();
    
    double x3_val = p3.XD();
    double y3_val = p3.YD();
    double w3_val = p3.WD();
    
    double x4_val = p4.XD();
    double y4_val = p4.YD();
    double w4_val = p4.WD();
    
    
    double x1_sup = fp::abs(x1_val);
    double y1_sup = fp::abs(y1_val);
    double w1_sup = fp::abs(w1_val);
    
    double x2_sup = fp::abs(x2_val);
    double y2_sup = fp::abs(y2_val);
    double w2_sup = fp::abs(w2_val);
    
    double x3_sup = fp::abs(x3_val);
    double y3_sup = fp::abs(y3_val);
    double w3_sup = fp::abs(w3_val);
    
    double x4_sup = fp::abs(x4_val);
    double y4_sup = fp::abs(y4_val);
    double w4_sup = fp::abs(w4_val);
    

	double tmp_8_val = x1_val*w2_val;
	double tmp_8_sup = x1_sup*w2_sup;

	double tmp_11_val = x2_val*w1_val;
	double tmp_11_sup = x2_sup*w1_sup;

	double tmp_12_val = tmp_8_val-tmp_11_val;
	double tmp_12_sup = tmp_8_sup+tmp_11_sup;

	double dxa_val = tmp_12_val;
	double dxa_sup = tmp_12_sup;


	double tmp_15_val = y1_val*w2_val;
	double tmp_15_sup = y1_sup*w2_sup;

	double tmp_18_val = y2_val*w1_val;
	double tmp_18_sup = y2_sup*w1_sup;

	double tmp_19_val = tmp_15_val-tmp_18_val;
	double tmp_19_sup = tmp_15_sup+tmp_18_sup;

	double dya_val = tmp_19_val;
	double dya_sup = tmp_19_sup;


	double tmp_28_val = x3_val*w4_val;
	double tmp_28_sup = x3_sup*w4_sup;

	double tmp_31_val = x4_val*w3_val;
	double tmp_31_sup = x4_sup*w3_sup;

	double tmp_32_val = tmp_28_val-tmp_31_val;
	double tmp_32_sup = tmp_28_sup+tmp_31_sup;

	double dxb_val = tmp_32_val;
	double dxb_sup = tmp_32_sup;


	double tmp_35_val = y3_val*w4_val;
	double tmp_35_sup = y3_sup*w4_sup;

	double tmp_38_val = y4_val*w3_val;
	double tmp_38_sup = y4_sup*w3_sup;

	double tmp_39_val = tmp_35_val-tmp_38_val;
	double tmp_39_sup = tmp_35_sup+tmp_38_sup;

	double dyb_val = tmp_39_val;
	double dyb_sup = tmp_39_sup;


	double tmp_42_val = dxa_val*dxa_val;
	double tmp_42_sup = dxa_sup*dxa_sup;

	double tmp_45_val = dya_val*dya_val;
	double tmp_45_sup = dya_sup*dya_sup;

	double tmp_46_val = tmp_42_val+tmp_45_val;
	double tmp_46_sup = tmp_42_sup+tmp_45_sup;

	double z1_val = tmp_46_val;
	double z1_sup = tmp_46_sup;


	double tmp_49_val = w1_val*w1_val;
	double tmp_49_sup = w1_sup*w1_sup;

	double tmp_51_val = tmp_49_val*w2_val;
	double tmp_51_sup = tmp_49_sup*w2_sup;

	double tmp_53_val = tmp_51_val*w2_val;
	double tmp_53_sup = tmp_51_sup*w2_sup;

	double n1_val = tmp_53_val;
	double n1_sup = tmp_53_sup;


	double tmp_56_val = dxb_val*dxb_val;
	double tmp_56_sup = dxb_sup*dxb_sup;

	double tmp_59_val = dyb_val*dyb_val;
	double tmp_59_sup = dyb_sup*dyb_sup;

	double tmp_60_val = tmp_56_val+tmp_59_val;
	double tmp_60_sup = tmp_56_sup+tmp_59_sup;

	double z2_val = tmp_60_val;
	double z2_sup = tmp_60_sup;


	double tmp_63_val = w3_val*w3_val;
	double tmp_63_sup = w3_sup*w3_sup;

	double tmp_65_val = tmp_63_val*w4_val;
	double tmp_65_sup = tmp_63_sup*w4_sup;

	double tmp_67_val = tmp_65_val*w4_val;
	double tmp_67_sup = tmp_65_sup*w4_sup;

	double n2_val = tmp_67_val;
	double n2_sup = tmp_67_sup;


	double tmp_70_val = z1_val*n2_val;
	double tmp_70_sup = z1_sup*n2_sup;

	double tmp_73_val = z2_val*n1_val;
	double tmp_73_sup = z2_sup*n1_sup;

	double tmp_74_val = tmp_70_val-tmp_73_val;
	double tmp_74_sup = tmp_70_sup+tmp_73_sup;

	double val_val = tmp_74_val;
	double val_sup = tmp_74_sup;

	static double _upper=19*EXCOMP_eps1;

	if ( fp::abs(val_val) > val_sup*_upper*EXCOMP_correction )
	   ressign = fp::sign(val_val);
	else 
           if ( val_sup*_upper*EXCOMP_correction<1)
	      ressign = 0;
	   else
	      ressign = EXCOMP_NO_IDEA;
    }

    if (ressign==EXCOMP_NO_IDEA || EXCOMP_FALSE || rat_point::use_filter == 0)
    {
      integer x1 = p1.X();
      integer y1 = p1.Y();
      integer w1 = p1.W();
      integer x2 = p2.X();
      integer y2 = p2.Y();
      integer w2 = p2.W();
      integer dxa = ((x1*w2)-(x2*w1));
      integer dya = ((y1*w2)-(y2*w1));
      integer x3 = p3.X();
      integer y3 = p3.Y();
      integer w3 = p3.W();
      integer x4 = p4.X();
      integer y4 = p4.Y();
      integer w4 = p4.W();
      integer dxb = ((x3*w4)-(x4*w3));
      integer dyb = ((y3*w4)-(y4*w3));
      integer z1 = ((dxa*dxa)+(dya*dya));
      integer n1 = (((w1*w1)*w2)*w2);
      integer z2 = ((dxb*dxb)+(dyb*dyb));
      integer n2 = (((w3*w3)*w4)*w4);
      integer val = ((z1*n2)-(z2*n1));
      ressign = sign(val);
    }

  return ressign;
}



// floating point version of cmp_distances

int rat_point::cmp_distances_F(const rat_point& p1, const rat_point& p2, 
                               const rat_point& p3, const rat_point& p4)
{
	double x1 = p1.XD();
	double y1 = p1.YD();
	double w1 = p1.WD();
	double x2 = p2.XD();
	double y2 = p2.YD();
	double w2 = p2.WD();
	double dxa = ((x1*w2)-(x2*w1));
	double dya = ((y1*w2)-(y2*w1));
	double x3 = p3.XD();
	double y3 = p3.YD();
	double w3 = p3.WD();
	double x4 = p4.XD();
	double y4 = p4.YD();
	double w4 = p4.WD();
	double dxb = ((x3*w4)-(x4*w3));
	double dyb = ((y3*w4)-(y4*w3));
	double z1 = ((dxa*dxa)+(dya*dya));
	double n1 = (((w1*w1)*w2)*w2);
	double z2 = ((dxb*dxb)+(dyb*dyb));
	double n2 = (((w3*w3)*w4)*w4);
	double val = ((z1*n2)-(z2*n1));

        if (val > 0) return +1;
	if (val < 0) return -1;
	
	return 0;
}



// this functions sets the static error bound for the input points
// the previous error bound is returned

double rat_point::set_static_soc_error_bound(double bound)
{
    double prev_bound = rat_point::soc_eps_static;
    rat_point::soc_eps_static = bound;
    return prev_bound;    
}

double rat_point::set_static_orientation_error_bound(double bound)
{
    double prev_bound = rat_point::ori_eps_static;
    rat_point::ori_eps_static = bound;
    return prev_bound;    
}


double rat_point::set_static_soc_error_bound(double xm, double ym, double wm)
{

//cout << "set_soc_error_bound" << endl;

      double prev_bound = rat_point::soc_eps_static;
      
      FABS(xm); FABS(ym); FABS(wm);

      double bxaw = xm * wm;   
      double byaw = ym * wm;
      double axbw = xm * wm; 
      double aybw = ym * wm;
  
      double bax = bxaw + axbw;
      double bay = byaw + aybw;    

      double cxaw = xm * wm;   
      double cyaw = ym * wm;
      double axcw = xm * wm; 
      double aycw = ym * wm; 
  
      double cax = cxaw + axcw;
      double cay = cyaw + aycw;  

      double dxaw = xm * wm;   
      double dyaw = ym * wm;
      double axdw = xm * wm; 
      double aydw = ym * wm;  
  
      double dax = dxaw + axdw;
      double day = dyaw + aydw;      
      
      rat_point::soc_eps_static =  34 * (wm * wm * (dax*dax + day*day) * (bay*cax + bax*cay)
       +  wm * (wm*(bax*bax+bay*bay)*(cay*dax+cax*day) + wm*(cax*cax+cay*cay)*(bax*day+bay*dax)))
       * point_eps0;      

      return prev_bound;    
}


double rat_point::set_static_orientation_error_bound(double xm, double ym, double wm)
{

//cout << "set_ori_error_bound" << endl;

      double prev_bound = rat_point::ori_eps_static;
      
      FABS(xm); FABS(ym); FABS(wm);
      
      double cyaw = ym * wm;   
      double aycw = ym * wm; 
      double axcw = xm * wm;
      double cxaw = xm * wm;    
      
      rat_point::ori_eps_static = 11* (xm*(cyaw+aycw) + ym*(axcw+cxaw) + wm*(ym*xm+xm*ym)) *point_eps0;	      

      return prev_bound;    
}



int side_of_circle_old(const rat_point& a, 
                   const rat_point& b, 
                   const rat_point& c, 
                   const rat_point& d)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::soc_count++;
#endif

  if (rat_point::use_filter != 0)
    {
      double bxaw = b.XD() * a.WD();   
      double byaw = b.YD() * a.WD();
      double axbw = a.XD() * b.WD(); 
      double aybw = a.YD() * b.WD();    

      double bax = bxaw - axbw;
      double bay = byaw - aybw;   
      double baw = b.WD()*a.WD();

      double cxaw = c.XD() * a.WD();   
      double cyaw = c.YD() * a.WD();
      double axcw = a.XD() * c.WD(); 
      double aycw = a.YD() * c.WD(); 
     
      double cax = cxaw - axcw;
      double cay = cyaw - aycw;
      double caw = c.WD()*a.WD();

      double dxaw = d.XD() * a.WD();   
      double dyaw = d.YD() * a.WD();
      double axdw = a.XD() * d.WD(); 
      double aydw = a.YD() * d.WD(); 
     
      double dax = dxaw - axdw;
      double day = dyaw - aydw;
      double daw = d.WD()*a.WD();
      
      double Bxi = bax * baw;
      double Byi = bay * baw;
      double Bwi = bax*bax + bay*bay;

      double Cxi = cax * caw;
      double Cyi = cay * caw;
      double Cwi = cax*cax + cay*cay;

      double Dxi = dax * daw;
      double Dyi = day * daw;
      double Dwi = dax*dax + day*day;

      double E = (Byi*Cxi - Bxi*Cyi)*Dwi + 
        (Bwi*Cyi - Byi*Cwi)*Dxi + (Bxi*Cwi - Bwi*Cxi)*Dyi;

      if ( rat_point::float_computation_only != 0 )
      { if ( E > 0 ) return 1;
        if ( E < 0 ) return -1;
        return 0;
       }
      
      /* ----------------------------------------------------------------
       * ERROR BOUND:
       * ----------------------------------------------------------------
       *
       * mes(E) = mes((Byi*Cxi - Bxi*Cyi)*Dwi + (Bwi*Cyi - Byi*Cwi)*Dxi + 
       *              (Bxi*Cwi - Bwi*Cxi)*Dyi)
       *        = 2*( mes((Byi*Cxi - Bxi*Cyi)*Dwi) + 
       *              2*mes((Bwi*Cyi - Byi*Cwi)*Dxi) +
       *              2*mes((Bxi*Cwi - Bwi*Cxi)*Dyi) )
       *        = 4*( (mes(Byi)*mes(Cxi) + mes(Bxi)*mes(Cyi))*mes(Dwi) + 
       *              2*(mes(Bwi)*mes(Cyi) + mes(Byi)*mes(Cwi))*mes(Dxi) +
       *              2*(mes(Bxi)*mes(Cwi) + mes(Bwi)*mes(Cxi))*mes(Dyi) )
       *
       * mes(Byi) = mes(bay) * mes(baw) 
       *          = mes(byaw - aybw)* mes(baw)
       *          = 2*(mes(byaw) + mes(aybw))* mes(Bw)*mes(Aw)
       *          = 2*(fabs(byaw) + fabs(aybw))*baw
       *            ( because a.WD() and b.WD() are positive )
       *
       * mes(Bxi) = 2*(fabs(bxaw) + fabs(axbw))*baw
       *
       * mes(Bwi) = 2*(mes(bax)*mes(bax) + mes(bay)*mes(bay))
       *          = 2*( 2*(fabs(bxaw) + fabs(axbw)) * 
       *                2*(fabs(bxaw) + fabs(axbw)) +
       *                2*(fabs(byaw) + fabs(aybw)) * 
       *                2*(fabs(byaw) + fabs(aybw)))
       *          = 8*((fabs(bxaw)+fabs(axbw))*(fabs(bxaw)+fabs(axbw)) +
       *               (fabs(byaw)+fabs(aybw))*(fabs(byaw)+fabs(aybw)))
       * 
       * mes(Cyi) = 2*(fabs(cyaw) + fabs(aycw))*caw
       * 
       * mes(Cxi) = 2*(fabs(cxaw) + fabs(axcw))*caw
       *
       * mes(Cwi) = 8*((fabs(cxaw)+fabs(axcw))*(fabs(cxaw)+fabs(axcw)) +
       *               (fabs(cyaw)+fabs(aycw))*(fabs(cyaw)+fabs(aycw)))
       *
       * mes(Dyi) = 2*(fabs(dyaw) + fabs(aydw))*daw
       * 
       * mes(Dxi) = 2*(fabs(dxaw) + fabs(axdw))*daw
       *
       * mes(Dwi) = 8*((fabs(dxaw)+fabs(axdw))*(fabs(dxaw)+fabs(axdw)) +
       *               (fabs(dyaw)+fabs(aydw))*(fabs(dyaw)+fabs(aydw)))
       *
       *
       * mes(E) = 4*(  ( 2*(fabs(byaw) + fabs(aybw))*baw *
       *                 2*(fabs(cxaw) + fabs(axcw))*caw + 
       *                 2*(fabs(bxaw) + fabs(axbw))*baw *
       *                 2*(fabs(cyaw) + fabs(aycw))*caw ) * 
       *               8*((fabs(dxaw)+fabs(axdw))*(fabs(dxaw)+fabs(axdw)) +
       *               (fabs(dyaw)+fabs(aydw))*(fabs(dyaw)+fabs(aydw)))
       *             + 
       *               ( 8*((fabs(bxaw)+fabs(axbw))*(fabs(bxaw)+fabs(axbw))+
       *                 (fabs(byaw)+fabs(aybw))*(fabs(byaw)+fabs(aybw))) *
       *                 2*(fabs(cyaw) + fabs(aycw))*caw + 
       *                 2*(fabs(byaw) + fabs(aybw))*baw *
       *                 8*((fabs(cxaw)+fabs(axcw))*(fabs(cxaw)+fabs(axcw))+
       *                 (fabs(cyaw)+fabs(aycw))*(fabs(cyaw)+fabs(aycw))) )*
       *               4*(fabs(dxaw) + fabs(axdw))*daw 
       *             +
       *               ( 2*(fabs(bxaw) + fabs(axbw))*baw *
       *                 8*((fabs(cxaw)+fabs(axcw))*(fabs(cxaw)+fabs(axcw))+
       *                 (fabs(cyaw)+fabs(aycw))*(fabs(cyaw)+fabs(aycw))) + 
       *                 8*((fabs(bxaw)+fabs(axbw))*(fabs(bxaw)+fabs(axbw))+
       *                 (fabs(byaw)+fabs(aybw))*(fabs(byaw)+fabs(aybw))) * 
       *                 2*(fabs(cxaw) + fabs(axcw))*caw ) *
       *               4*(fabs(dyaw) + fabs(aydw))*daw )
       *
       *
       * ind(E) = ( ind((Byi*Cxi-Bxi*Cyi)*Dwi) +
       *            ind((Bwi*Cyi-Byi*Cwi)*Dxi + (Bxi*Cwi-Bwi*Cxi)*Dyi) +
       *            1 ) /2
       *        = ( ind((Byi*Cxi-Bxi*Cyi)*Dwi) +
       *            (ind((Bwi*Cyi-Byi*Cwi)*Dxi)+
       *             ind((Bxi*Cwi-Bwi*Cxi)*Dyi)+1)/2
       *            + 1 ) / 2
       *        = ( (ind(Byi)+ind(Cxi)+0.5+ind(Bxi)+ind(Cyi)+0.5+1)/2 
       *            + ind(Dwi)+0.5+
       *            ((ind(Bwi)+ind(Cyi)+0.5+ind(Byi)+ind(Cwi)+0.5+1)/2
       *             +ind(Dxi)+0.5+(ind(Bxi)+ind(Cwi)+0.5+ind(Bwi)+ind(Cxi)
       *             +0.5+1)/2+ind(Dyi)+0.5+1)/2 +1)/2
       *
       * ind(Byi) = ind(bay)+ind(baw)+0.5
       *          = ind(bxaw-axbw)+ind(baw)+0.5
       *          = (1.5 + 1.5 +1)/2 + 1.5 +0.5
       *          = 4
       *          = ind(Bxi) = ind(Cxi) = ind(Cyi) = ind(Dxi) = ind(Dyi)
       *
       * ind(Dwi) = ind(dax*dax + day*day)
       *          = (ind(dax) + ind(dax) +0.5 + ind(day) + ind(day)+0.5+1)/2
       *          = ind(dax) + ind(day) + 1
       *          = (1.5 + 1.5 +1)/2 + (1.5 + 1.5 +1)/2 + 1
       *          = 5 
       *          = ind(Bwi) = ind(Cwi)
       *
       * ind(E) = ( 4 + 4 + 0.5 + 4 + 4 + 0.5 + 1 )/2 + 5 + 0.5 +
       *            ((5 + 4 + 0.5 + 4 + 5 + 0.5 + 1)/2 + 4 + 0.5 +
       *             (4 + 5 + 0.5 + 5 + 4 + 0.5 + 1)/2 + 4 + 0.5 + 1)/2 +
       *            1 ) /2
       *        = (9 + 5 + 0.5 + (10 + 4 + 0.5 + 10 + 4 + 0.5 + 1)/2 + 1)/2
       *        = (9 + 5 + 0.5 + 15 + 1)/2 = 30.5/2 = 61/4
       *
       * eps(E) = ind(E) * mes(E) * eps0
       *        = 32*61*
       *            (  ( (fabs(byaw) + fabs(aybw))*baw *
       *                 (fabs(cxaw) + fabs(axcw))*caw + 
       *                 (fabs(bxaw) + fabs(axbw))*baw *
       *                 (fabs(cyaw) + fabs(aycw))*caw ) * 
       *               ((fabs(dxaw)+fabs(axdw))*(fabs(dxaw)+fabs(axdw)) +
       *                (fabs(dyaw)+fabs(aydw))*(fabs(dyaw)+fabs(aydw)))
       *             + 
       *               ( ((fabs(bxaw)+fabs(axbw))*(fabs(bxaw)+fabs(axbw))+
       *                  (fabs(byaw)+fabs(aybw))*(fabs(byaw)+fabs(aybw))) *
       *                 (fabs(cyaw) + fabs(aycw))*caw + 
       *                 (fabs(byaw) + fabs(aybw))*baw *
       *                 ((fabs(cxaw)+fabs(axcw))*(fabs(cxaw)+fabs(axcw))+
       *                  (fabs(cyaw)+fabs(aycw))*(fabs(cyaw)+fabs(aycw))))*
       *               (fabs(dxaw) + fabs(axdw))*daw*2
       *             +
       *               ( (fabs(bxaw) + fabs(axbw))*baw *
       *                 ((fabs(cxaw)+fabs(axcw))*(fabs(cxaw)+fabs(axcw))+
       *                 (fabs(cyaw)+fabs(aycw))*(fabs(cyaw)+fabs(aycw))) + 
       *                 ((fabs(bxaw)+fabs(axbw))*(fabs(bxaw)+fabs(axbw))+
       *                  (fabs(byaw)+fabs(aybw))*(fabs(byaw)+fabs(aybw)))* 
       *                 (fabs(cxaw) + fabs(axcw))*caw ) *
       *               (fabs(dyaw) + fabs(aydw))*daw*2 )
       *           * eps0
       *             
       * -----------------------------------------------------------------*/
      
      FABS(bxaw); FABS(axbw); FABS(byaw); FABS(aybw);
      FABS(cxaw); FABS(axcw); FABS(cyaw); FABS(aycw);
      FABS(dxaw); FABS(axdw); FABS(dyaw); FABS(aydw);

      double eps = 1952 *
        (((byaw + aybw)*baw *(cxaw + axcw)*caw + 
          (bxaw + axbw)*baw *(cyaw + aycw)*caw ) * 
         ((dxaw + axdw)*(dxaw + axdw) + (dyaw + aydw)*(dyaw +aydw))
         + 
         (((bxaw + axbw)*(bxaw + axbw) + (byaw + aybw)*(byaw + aybw)) *
          (cyaw +aycw)*caw   +  (byaw + aybw)*baw *
          ((cxaw + axcw)*(cxaw + axcw) + (cyaw + aycw)*(cyaw + aycw))) *
         (dxaw + axdw)*daw*2
         +
         ((bxaw + axbw)*baw *
          ((cxaw + axcw)*(cxaw + axcw) + (cyaw + aycw)*(cyaw + aycw)) + 
          ((bxaw + axbw)*(bxaw + axbw) + (byaw + aybw)*(byaw + aybw)) * 
          (cxaw + axcw)*caw ) *
         (dyaw + aydw)*daw*2 )
        * point_eps0;


      if (E   >  eps)  return  1;
      if (E   < -eps)  return -1;
      if (eps <  1)    return  0;
    }

#if !defined(LEDA_MULTI_THREAD)
  rat_point::exact_soc_count++;
#endif

  integer AX = a.X();
  integer AY = a.Y();
  integer AW = a.W();

  integer BX = b.X();
  integer BY = b.Y();
  integer BW = b.W();

  integer CX = c.X();
  integer CY = c.Y();
  integer CW = c.W();

  integer DX = d.X();
  integer DY = d.Y();
  integer DW = d.W();

  integer bx,by,bw,cx,cy,cw,dx,dy,dw;

  if (AW==1 && BW==1 && CW==1 && DW==1)
  { bx = BX - AX;
    by = BY - AY;
    bw = bx*bx + by*by;

    cx = CX - AX;
    cy = CY - AY;
    cw = cx*cx + cy*cy;
    
    dx = DX - AX;
    dy = DY - AY;
    dw = dx*dx + dy*dy;
   }
  else
  { integer b1 = BX*AW - AX*BW;
    integer b2 = BY*AW - AY*BW;
    integer c1 = CX*AW - AX*CW;
    integer c2 = CY*AW - AY*CW;
    integer d1 = DX*AW - AX*DW;
    integer d2 = DY*AW - AY*DW;

    bx = b1 * AW * BW;
    by = b2 * AW * BW;
    bw = b1*b1 + b2*b2;

    cx = c1 * AW * CW;
    cy = c2 * AW * CW;
    cw = c1*c1 + c2*c2;

    dx = d1 * AW * DW;
    dy = d2 * AW * DW;
    dw = d1*d1 + d2*d2;
   }

  return sign((by*cx-bx*cy)*dw + (cy*bw-by*cw)*dx  + (bx*cw-cx*bw)*dy);
}



int side_of_circle(const rat_point& a, 
                   const rat_point& b, 
                   const rat_point& c, 
                   const rat_point& d)
{ 
#if !defined(LEDA_MULTI_THREAD)
  rat_point::soc_count++;
#endif

  if (rat_point::use_filter != 0)
    {
     double bxaw = b.XD() * a.WD();   
     double byaw = b.YD() * a.WD();
     double axbw = a.XD() * b.WD(); 
     double aybw = a.YD() * b.WD();

     double bax = bxaw - axbw;
     double bay = byaw - aybw;   

     double cxaw = c.XD() * a.WD();   
     double cyaw = c.YD() * a.WD();
     double axcw = a.XD() * c.WD(); 
     double aycw = a.YD() * c.WD(); 
 
     double cax = cxaw - axcw;
     double cay = cyaw - aycw;

     double dxaw = d.XD() * a.WD();   
     double dyaw = d.YD() * a.WD();
     double axdw = a.XD() * d.WD(); 
     double aydw = a.YD() * d.WD(); 
 
     double dax = dxaw - axdw;
     double day = dyaw - aydw;
  
     double E = b.WD() * c.WD() * (dax*dax + day*day) * (bay*cax - bax*cay)
       +  d.WD() * (c.WD()*(bax*bax+bay*bay)*(cay*dax-cax*day) + b.WD()*(cax*cax+cay*cay)*(bax*day-bay*dax));	
       
     if ( rat_point::float_computation_only != 0 )
     { if ( E > 0 ) return 1;
        if ( E < 0 ) return -1;
        return 0;
     }       
      

     if (rat_point::use_static_filter)
     { if (rat_point::soc_eps_static == -1)
            rat_point::set_static_soc_error_bound(rat_point::x_abs_max,
                                                  rat_point::y_abs_max,
                                                  rat_point::w_abs_max);
       if (E >  rat_point::soc_eps_static) return  1;
       if (E < -rat_point::soc_eps_static) return -1;
     }    
     
     // semi-dynamic filter
     
     // attention: take the a+b instead of a-b ...
     FABS(bxaw); FABS(byaw); FABS(axbw); FABS(aybw);
     FABS(cxaw); FABS(cyaw); FABS(axcw); FABS(aycw);
     FABS(dxaw); FABS(dyaw); FABS(axdw); FABS(aydw);
  
     // 6 additional operations
     bax = bxaw + axbw;
     bay = byaw + aybw;
     cax = cxaw + axcw;
     cay = cyaw + aycw; 
     dax = dxaw + axdw;
     day = dyaw + aydw;     
  
     double eps = 34 * (b.WD() * c.WD() * (dax*dax + day*day) * (bay*cax + bax*cay)
       +  d.WD() * (c.WD()*(bax*bax+bay*bay)*(cay*dax+cax*day) + b.WD()*(cax*cax+cay*cay)*(bax*day+bay*dax)))
        * point_eps0;     
	
     if (E   >  eps)  return  1;
     if (E   < -eps)  return  -1;
     if (eps <  1)    return  0;	 
    }
    
   // exact computation ...
#if !defined(LEDA_MULTI_THREAD)
  rat_point::exact_soc_count++;
#endif

  integer AX = a.X();
  integer AY = a.Y();
  integer AW = a.W();

  integer BX = b.X();
  integer BY = b.Y();
  integer BW = b.W();

  integer CX = c.X();
  integer CY = c.Y();
  integer CW = c.W();

  integer DX = d.X();
  integer DY = d.Y();
  integer DW = d.W();

  integer bx,by,bw,cx,cy,cw,dx,dy,dw;
  
  if (AW==1 && BW==1 && CW==1 && DW==1)
  { // 15 operations
    bx = BX - AX;
    by = BY - AY;
    bw = bx*bx + by*by;

    cx = CX - AX;
    cy = CY - AY;
    cw = cx*cx + cy*cy;
    
    dx = DX - AX;
    dy = DY - AY;
    dw = dx*dx + dy*dy;
  }
  else
  { // 39 operations
    integer b1 = BX*AW - AX*BW;
    integer b2 = BY*AW - AY*BW;
    integer c1 = CX*AW - AX*CW;
    integer c2 = CY*AW - AY*CW;
    integer d1 = DX*AW - AX*DW;
    integer d2 = DY*AW - AY*DW;

    bx = b1 * AW * BW;
    by = b2 * AW * BW;
    bw = b1*b1 + b2*b2;

    cx = c1 * AW * CW;
    cy = c2 * AW * CW;
    cw = c1*c1 + c2*c2;

    dx = d1 * AW * DW;
    dy = d2 * AW * DW;
    dw = d1*d1 + d2*d2;
  }
  // 14 operations
  return sign((by*cx-bx*cy)*dw + (cy*bw-by*cw)*dx  + (bx*cw-cx*bw)*dy);          
}

static int sign(const int& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}


int compare_by_angle(const rat_point& p1, const rat_point& p2, 
                     const rat_point& p3, const rat_point& p4)
{ 
  if (p1 == p2) return (p3 == p4) ? 0 : -1;
  if (p3 == p4) return 1;

  // both vectors are non-zero ...
  integer x1 = p2.X()*p1.W() - p1.X()*p2.W();
  integer y1 = p2.Y()*p1.W() - p1.Y()*p2.W();

  integer x2 = p4.X()*p3.W() - p3.X()*p4.W();
  integer y2 = p4.Y()*p3.W() - p3.Y()*p4.W();  
  
  int sy1 = sign(y1); 
  int sy2 = sign(y2);

  int upper1 = (sy1 != 0) ? sy1 : sign(x1);
  int upper2 = (sy2 != 0) ? sy2 : sign(x2);

  if (upper1 == upper2) 
    return sign(x2*y1 - x1*y2);
  else
    return sign(upper2 - upper1);
}


bool affinely_independent(const array<rat_point>& A)
{ if (A.size() == 1) return true;
  if (A.size() == 2 && A[0] != A[1]) return true;
  if (A.size() == 3 && orientation(A[0],A[1],A[2]) != 0) return true;
  return false; 
}


bool contained_in_simplex(const array<rat_point>& A, const rat_point& p)
{ if (A.size() == 1 && A[0] == p) return true;
  if (A.size() == 2) {
    rat_point a = A[0];
    rat_point b = A[1];
    if (a == b) LEDA_EXCEPTION(1,"cis: equal points");
    if (compare(a,b) > 0) leda_swap(a,b);
    return compare(a,p) <= 0 && compare(p,b) <= 0;
  }
 if (A.size() == 3) {
    rat_point a = A[0];
    rat_point b = A[1];
    rat_point c = A[2];
   int orient = orientation(a,b,c);
   if (orient == 0) 
      LEDA_EXCEPTION(1,"cis: equal points or no simplex");
   if (orient < 0) leda_swap(b,c);
   return (orientation(a,b,p)>=0 &&
           orientation(b,c,p)>=0 &&
           orientation(c,a,p)>=0);
 }
 return false;
}


bool contained_in_affine_hull(const array<rat_point>& A, const rat_point& p)
{ if (A.size() == 1 && A[0] == p) return true;
  if (A.size() == 2) {
    if (A[0]==A[1]) return (A[0] == p);
    return orientation(A[0],A[1],p) == 0;
  }
  if (A.size() == 3) {
    if (orientation(A[0],A[1],A[2]) != 0)
      return true;
    else
      return (orientation(A[0],A[1],p) == 0);
  }
  return false;
}



void rat_point::clear_statistics()
{
  cmp_count = 0;
  orient_count = 0;
  soc_count = 0;
  exact_cmp_count = 0;
  exact_orient_count = 0;
  exact_soc_count = 0;
  error_cmp_count = 0;
  error_orient_count = 0;
  error_soc_count = 0;
}


void rat_point::print_statistics()
{
  float exact_cmp_percent = 0;
  float exact_orient_percent = 0;
  float exact_soc_percent = 0;

  float error_cmp_percent = 0;
  float error_orient_percent = 0;
  float error_soc_percent = 0;
  
  if (cmp_count > 0) 
  { exact_cmp_percent= 100*float(exact_cmp_count)/cmp_count;
    error_cmp_percent= 100*float(error_cmp_count)/cmp_count;
  }
  
  if (orient_count > 0) 
  { exact_orient_percent= 100*float(exact_orient_count)/orient_count;
    error_orient_percent= 100*float(error_orient_count)/orient_count;
   }
  
  if (soc_count > 0) 
  { exact_soc_percent= 100*float(exact_soc_count)/soc_count;
    error_soc_percent= 100*float(error_soc_count)/soc_count;
   }
  
  cout << endl;
  cout << string("compare:        %8d /%10d  (%5.2f %%)  ",
                  exact_cmp_count,cmp_count,exact_cmp_percent);

  cout << string("float-errors: %3d (%5.2f %%)",
                  error_cmp_count,error_cmp_percent) << endl;

  cout << string("orientation:    %8d /%10d  (%5.2f %%)  ",
                  exact_orient_count,orient_count,exact_orient_percent);

  cout << string("float-errors: %3d (%5.2f %%)",
                  error_orient_count,error_orient_percent) << endl;

  cout << string("side of circle: %8d /%10d  (%5.2f %%)  ",
                  exact_soc_count,soc_count,exact_soc_percent);

  cout << string("float-errors: %3d (%5.2f %%)",
                  error_soc_count,error_soc_percent) << endl;
  cout << endl;
}

LEDA_END_NAMESPACE 

