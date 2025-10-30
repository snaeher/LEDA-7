/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  transform.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE 

TRANSFORM::TRANSFORM() 
{ M = INT_MATRIX(3,3);
  for (int i = 0; i < 3; i++)
  for (int j = 0; j < 3; j++)
    M(i,j) = ( i == j? 1 : 0 );
}

TRANSFORM::TRANSFORM(const INT_MATRIX t): M(t) 
{ if ( M.dim1() != 3 || M.dim2() != 3 )
   LEDA_EXCEPTION(1,"TRANSFORM::TRANSFORM: matrix must be 3x3.");
  if ( M(2,0) != 0 || M(2,1) != 0 )
   LEDA_EXCEPTION(1,"TRANSFORM::TRANSFORM: M(2,0) != 0 or M(2,1) != 0."); 
  if ( M(2,2) == 0 )  
   LEDA_EXCEPTION(1,"TRANSFORM::TRANSFORM: M(2,2) == 0."); 
  simplify();
}

INT_MATRIX TRANSFORM::T_matrix() const
{ return M; }


TRANSFORM TRANSFORM::operator()(const TRANSFORM& T1) const
{ INT_MATRIX result = M * T1.T_matrix();
  return TRANSFORM(result);
}

void TRANSFORM::simplify()
{
#if (KERNEL == RAT_KERNEL )
INT_TYPE g = M(2,2);
int i,j;
for (i = 0; i < 2; i++)
for (j = 0; j < 3; j++)
  g = gcd(g,M(i,j));
if ( g == 1) return;
for (i = 0; i < 3; i++)
for (j = 0; j < 3; j++)
  M(i,j) /= g;
#endif
}

RAT_TYPE TRANSFORM::norm() const
{ INT_TYPE A = M(0,0)*M(1,1) - M(0,1)*M(1,0);
  INT_TYPE B = M(2,2)*M(2,2);
#if (KERNEL == RAT_KERNEL )
  return rational(A,B);
#else
  return A/B;
#endif
}


POINT TRANSFORM::operator()(const POINT& p) const
{ INT_VECTOR v(p.X(),p.Y(),p.W());
  INT_VECTOR r = M*v;
  #if (KERNEL == RAT_KERNEL)
  return rat_point(r[0],r[1],r[2]);
  #else
  return POINT(r[0]/r[2],r[1]/r[2]);
  #endif
}

VECTOR TRANSFORM::operator()(const VECTOR & v) const
{ INT_VECTOR r(3);
  r[0] = M(0,0)*v.hcoord(0) + M(0,1)*v.hcoord(1);
  r[1] = M(1,0)*v.hcoord(0) + M(1,1)*v.hcoord(1);
  r[2] = M(2,2)*v.hcoord(2);
  #if (KERNEL == RAT_KERNEL)
  return rat_vector(r[0],r[1],r[2]);
  #else
  return VECTOR(r[0]/r[2],r[1]/r[2]);
  #endif
}

SEGMENT TRANSFORM::operator()(const SEGMENT& s) const
{ const TRANSFORM& T = *this;
  return SEGMENT(T(s.source()),T(s.target()));
}


LINE TRANSFORM::operator()(const LINE& l) const
{ const TRANSFORM& T = *this;
  return LINE(T(l.point1()),T(l.point2()));
}

RAY TRANSFORM::operator()(const RAY& r) const
{ const TRANSFORM& T = *this;
  return RAY(T(r.point1()),T(r.point2()));
}

CIRCLE TRANSFORM::operator()(const CIRCLE& C) const
{ const TRANSFORM& T = *this;
  RAT_TYPE N = norm();
  if ( N < 0 ) N = -N;
  if ( N != 1 )
    LEDA_EXCEPTION(1,"can only apply rigid transformations to circles");
  return CIRCLE(T(C.point1()),T(C.point2()),T(C.point3()));
}

POLYGON TRANSFORM::operator()(const POLYGON& P) const
{ const TRANSFORM& T = *this;
  if ( P.empty() ) return P;
  list<POINT> PL;
  POINT p;
  forall(p,P.vertices()) PL.append(T(p));
  
  return POLYGON(PL,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}

GEN_POLYGON TRANSFORM::operator()(const GEN_POLYGON& P) const
{ const TRANSFORM& T = *this;
  if ( P.trivial() ) return P;
  list<POLYGON> PL;
  POLYGON p;
  forall(p,P.polygons()) PL.append(T(p));
  
  return GEN_POLYGON(PL,GEN_POLYGON::NO_CHECK);
}


TRANSFORM translation(const INT_TYPE& dx, const INT_TYPE& dy, const INT_TYPE& dw)
{ INT_MATRIX M(3,3);
  for (int i = 0; i < 3; i++)
  for (int j = 0; j < 3; j++)
  if (i == j)
    M(i,j) = dw;
  else
    M(i,j) = INT_TYPE(0);
  
  M(0,2) = dx; M(1,2) = dy;
  return TRANSFORM(M);
}

TRANSFORM translation(const RAT_TYPE& dx, const RAT_TYPE& dy)
{ 
#if (KERNEL == RAT_KERNEL)
  INT_TYPE xn = dx.numerator();
  INT_TYPE xw = dx.denominator();
  INT_TYPE yn = dy.numerator();
  INT_TYPE yw = dy.denominator();
  return translation(xn*yw,yn*xw,xw*yw);
#else
  return translation(dx,dy,1);
#endif
}

TRANSFORM translation(const VECTOR& v)
{ return translation(v.xcoord(),v.ycoord()); }



static TRANSFORM rotation90_origin()
{ 
  INT_MATRIX M(3,3);
  for (int i = 0; i < 3; i++)
  for (int j = 0; j < 3; j++)
    M(i,j) = 0 ;
  
  M(0,1) = -1;   M(1,0) = +1;
  M(2,2) = 1;
  
  return TRANSFORM(M);

}

TRANSFORM rotation90(const POINT& q) 
{ 
  TRANSFORM R = rotation90_origin();

  TRANSFORM T0 = translation(-q.X(),-q.Y(),q.W());
  TRANSFORM T1 = translation( q.X(), q.Y(),q.W());

  TRANSFORM T = T1(R(T0));
  T.simplify();
  return T;
}


static TRANSFORM rotation_origin(double alpha, double eps)
{ POINT origin(0,0);
  POINT X(1,0);
  CIRCLE C(origin,X); // unit circle centered at origin

  POINT p = C.point_on_circle(alpha,eps);

  INT_MATRIX M(3,3);
  
  M(0,2) = M(1,2) = M(2,0) = M(2,1) = 0;
  M(0,0) = M(1,1) = p.X() ;
  M(0,1) = -p.Y();   M(1,0) = p.Y(); 
  M(2,2) = p.W();

  return TRANSFORM(M);
}


TRANSFORM rotation(const POINT& q, double alpha, double eps)
{ 
  TRANSFORM R = rotation_origin(alpha,eps);

  TRANSFORM T0 = translation(-q.X(),-q.Y(),q.W());
  TRANSFORM T1 = translation( q.X(), q.Y(),q.W());

  TRANSFORM T = T1(R(T0));
  T.simplify();
  return T;
}





static TRANSFORM reflection_at_ray(const POINT& r) 
{ INT_MATRIX M(3,3);
  INT_TYPE rX = r.X();
  INT_TYPE rY = r.Y();
  INT_TYPE rX2 = rX * rX;
  INT_TYPE rY2 = rY * rY;
  
  M(0,0) = rX2 - rY2; M(1,1) = - M(0,0);

  M(0,1) = M(1,0) = rX * rY;   

  M(2,0) = M(2,1) = 0; M(2,2) = rX2 + rY2;
  
  return TRANSFORM(M);
}

TRANSFORM reflection(const POINT& q, const POINT& r) 
{ POINT origin(0,0);
  TRANSFORM R = reflection_at_ray(origin + (r - q));

  TRANSFORM T0 = translation(-q.X(),-q.Y(),q.W());
  TRANSFORM T1 = translation( q.X(), q.Y(),q.W());

  TRANSFORM T = T1(R(T0));
  T.simplify();
  return T;
}

TRANSFORM reflection(const POINT& q)
{ INT_MATRIX M(3,3);
  #if (KERNEL == RAT_KERNEL)  
  M(0,0) = -q.W();
  M(0,1) = 0; 
  M(0,2) = 2*q.X();
  M(1,0) = 0;
  M(1,1) = -q.W();
  M(1,2) = 2*q.Y();
  M(2,0) = M(2,1) = 0;
  M(2,2) = q.W();
  #else
  M(0,0) = -1;
  M(0,1) = 0; 
  M(0,2) = 2*q.xcoord();
  M(1,0) = 0;
  M(1,1) = -1;
  M(1,2) = 2*q.ycoord();
  M(2,0) = M(2,1) = 0;
  M(2,2) = 1;
  #endif
  return TRANSFORM(M);
}
 
LEDA_END_NAMESPACE 
