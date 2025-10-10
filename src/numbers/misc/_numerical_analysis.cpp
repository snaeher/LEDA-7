/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _numerical_analysis.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/numbers/numerical_analysis.h>


LEDA_BEGIN_NAMESPACE

void bracket_minimum(double (*f)(double), double& a, double& b, double& c)
{ 
  const double GOLD = 1.618034;
  double fa, fb, fc;

  a = 0;  fa = f(a);
  b = 1;  fb = f(b);

  if ( fb > fa) { leda_swap(a,b); leda_swap(fa,fb);}
  // we have fb <= fa;

  while ( true )
  { c = b + GOLD*(b - a);  fc = f(c);
    if ( fb <= fc ) break;
    a = b; fa = fb;
    b = c; fb = fc;
  }

 }


double minimize_function(double (*f)(double), double& xmin, double tol)
{ 
  const double GOLD = 1.618034;
  const double R    = GOLD - 1;   // about 0.61
  const double C    = 1 - R;      // about 0.31


  double a, b, c, f1, f2, x0, x1, x2, x3;

  bracket_minimum(f,a,b,c);

  x0 = a; x3 = c;

  if ( fabs(c - b) > fabs(b - a) )  
    { x1 = b; x2 = b + C*(c - b); }
  else
    { x2 = b; x1 = b - C*(b - a); }

  f1 = f(x1); f2 = f(x2);

  while ( fabs(x3 - x0) > tol * leda_max(1.0,fabs(x1) + fabs(x2)) )
  { 
    if ( f2 < f1 ) 
    { x0 = x1; x1 = x2; x2 = R*x1 + C*x3;
               f1 = f2; f2 = f(x2);
    }
    else
    { x3 = x2; x2 = x1; x1 = R*x2 + C*x0;
               f2 = f1; f1 = f(x1);
    }
  }

  xmin = ( f1 < f2 ? x1 : x2);
  return f(xmin);

 }



double integrate_function(double (*f)(double), double l, double r, 
double delta)
{ 
  leda_assert(l <= r,"integrate function: r must not be smaller than l");

  leda_assert(delta > 0,"integrate function: expect delta > 0",1);
  double sum = 0;
  for (double x = l; x < r; x += delta) sum += f(x);

  return delta*sum;



 }



double binary_entropy(double x)
{ if ( x == 0 || x == 1 ) return 0;
  return ( (x *log (1/x) + (1 - x)* log(1/(1-x))) / log(2.0));
}







double zero_of_function(double (*f)(double), double l, double r, 
double tol)
{ 
  if ( !leda_assert(l <= r,"zero_of_function: expect l <= r") )
    r = l;

  if ( f(l) * f(r) > 0 ) 
    LEDA_EXCEPTION(1,"zero_of_function: expect f(l)*f(r) <= 0");

  if ( f(l) == 0 ) return l;
  if ( f(r) == 0 ) return r;

  double x0 = l;
  double x1 = r;

  bool increasing = ( f(x0) < 0 );

  while ( x1 - x0 > tol * leda_max(1.0, fabs(x0) + fabs(x1)) )
  { double y0 = f(x0);
    double y1 = f(x1);
    double xnew = x0 - y0 * (x1 - x0) / (y1 - y0);
    double ynew = f(xnew);

    //cout << "\n\n" << xnew << " " << ynew;

    if ( fabs(ynew) < 1.0e-10 ) return xnew;
       
    if ( ynew > 0 && increasing )
      { x1 = xnew; y1 = ynew; }
    else
      { x0 = xnew; y0 = ynew; }
  } 

  return x0;
 }

LEDA_END_NAMESPACE
