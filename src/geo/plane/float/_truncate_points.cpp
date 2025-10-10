/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _truncate_points.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/truncate.h>


LEDA_BEGIN_NAMESPACE 

list<point> truncate(const list<point>& L0, int prec)
{ 
  double M = 0;

  point p; 
  forall(p,L0)
    M = leda_max(M,leda_max(fabs(p.xcoord()),fabs(p.ycoord())));

  if (M == 0) return L0;

  int exp;
  frexp(M,&exp);       // 2^(exp - 1) <= max < 2^exp
  M = ldexp(1.0,exp);  // round max to next power of two

  double C =     ldexp(1.0,prec - exp);  // P/M
  double C_inv = ldexp(1.0,exp - prec);  // M/P

  list<point> L;
  forall(p,L0) 
     L.append(point(::floor(p.xcoord() * C)*C_inv,
                    ::floor(p.ycoord() * C)*C_inv));

  return L;
}


list<rat_point> truncate(const list<rat_point>& L0, int prec)
{ list<point> L1;
  list<rat_point> L;

  rat_point p;
  forall(p,L0) L1.append(p.to_float());
  list<point> L2 = truncate(L1,prec);
  
  point q;
  forall(q,L2) L.append(rat_point(q));

  return L;
}

LEDA_END_NAMESPACE 
