/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_rat_plane.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_plane.h>
#include <LEDA/system/assert.h>
#include <LEDA/numbers/expcomp.h>

//------------------------------------------------------------------------------
// d3_rat_plane 
//
// by M. Baesken & S. Naeher
//
// last modified  Mai 1999
//------------------------------------------------------------------------------


LEDA_BEGIN_NAMESPACE

atomic_counter d3_rat_plane_rep::id_counter(0);


d3_rat_plane_rep::d3_rat_plane_rep(const d3_rat_point& p, 
                                   const d3_rat_point& q,
                                   const d3_rat_point& r,
                                   const integer& x,
                                   const integer& y,
                                   const integer& z)
: a(p), b(q), c(r), nx(x*p.W()), ny(y*p.W()), nz(z*p.W()), nd(-x*p.X()-y*p.Y()-z*p.Z())

{ 
  nx_d = nx.to_double(); 
  ny_d = ny.to_double();
  nz_d = nz.to_double(); 
  nd_d = nd.to_double();   

  id  = ++id_counter; 
}

d3_rat_plane::d3_rat_plane() 
{ PTR = new d3_rat_plane_rep(d3_rat_point(0,0,0),
                             d3_rat_point(0,1,0), d3_rat_point(1,0,0),0,0,1); }

d3_rat_plane::d3_rat_plane(const d3_rat_point& a, const d3_rat_point& b, const d3_rat_point& c)
{ integer X1 = b.X()*a.W() - a.X()*b.W();
  integer Y1 = b.Y()*a.W() - a.Y()*b.W();
  integer Z1 = b.Z()*a.W() - a.Z()*b.W();
  integer X2 = c.X()*a.W() - a.X()*c.W();
  integer Y2 = c.Y()*a.W() - a.Y()*c.W();
  integer Z2 = c.Z()*a.W() - a.Z()*c.W();
  integer nx = Z1*Y2 - Y1*Z2;
  integer ny = X1*Z2 - Z1*X2;
  integer nz = Y1*X2 - X1*Y2;
  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_rat_plane::d3_rat_plane(): cannot construct plane.");
  PTR = new d3_rat_plane_rep(a,b,c,nx,ny,nz); 
}


d3_rat_plane::d3_rat_plane(const d3_rat_point& p, const rat_vector& n) 
{ 
  integer nx = n.hcoord(0);
  integer ny = n.hcoord(1);
  integer nz = n.hcoord(2);

  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_plane::d3_plane(): cannot construct plane.");

  rat_vector v1;
  if (nx == 0) v1 = rat_vector(1,0,0,1);
  else if (ny == 0) v1 = rat_vector(0,1,0,1);
  else if (nz == 0) v1 = rat_vector(0,0,1,1);
  else v1 = rat_vector(-ny,nx,0,1);

  rat_vector v2 = rat_vector(ny*v1.hcoord(2)-nz*v1.hcoord(1),
                             nz*v1.hcoord(0)-nx*v1.hcoord(2),
                             nx*v1.hcoord(1)-ny*v1.hcoord(0),1);

  PTR = new d3_rat_plane_rep(p,p+v1,p+v2,nx,ny,nz); 
}

d3_rat_plane::d3_rat_plane(const d3_rat_point& p, const d3_rat_point& p2) 
{ 
  rat_vector n = p2-p;
  integer nx = n.hcoord(0);
  integer ny = n.hcoord(1);
  integer nz = n.hcoord(2);

  if (nx == 0 && ny == 0 && nz == 0)
     LEDA_EXCEPTION(1,"d3_plane::d3_plane(): cannot construct plane.");

  rat_vector v1;
  if (nx == 0) v1 = rat_vector(1,0,0,1);
  else if (ny == 0) v1 = rat_vector(0,1,0,1);
  else if (nz == 0) v1 = rat_vector(0,0,1,1);
  else v1 = rat_vector(-ny,nx,0,1);

  rat_vector v2 = rat_vector(ny*v1.hcoord(2)-nz*v1.hcoord(1),
                             nz*v1.hcoord(0)-nx*v1.hcoord(2),
                             nx*v1.hcoord(1)-ny*v1.hcoord(0),1);

  PTR = new d3_rat_plane_rep(p,p+v1,p+v2,nx,ny,nz); 
}



int d3_rat_plane::intersection(const d3_rat_point p1, const d3_rat_point p2, 
                                                      d3_rat_point& q) const
{ 
 // returns
 // 0: line through p1,p2 parallel
 // 1: line through p1,p2 intersects plane in a single point q
 // 2: line is in the plane

  d3_rat_point pp1 = p1;
  d3_rat_point pp2 = p2;

  rat_vector vec1 = normal_project(pp1);
  rat_vector vec2 = normal_project(pp2);

  if (vec1 == vec2) 
  { if (vec1 == rat_vector(0,0,0,1))
       return 2;
    else 
       return 0;
   }

  rational d1 = vec1.sqr_length();
  rational d2 = vec2.sqr_length();

  if (d1.numerator() == 0)
  { q = pp1;
    return 1;
   }


  if (d2.numerator() == 0)
  { q = pp2;
    return 1;
   }

  if (d1 < d2)
  { leda_swap(pp1,pp2);
    leda_swap(vec1,vec2);
   }

  vec1 = vec1 - vec2;

  int i = 0;
  while (vec1.hcoord(i) == 0) i++;

  assert(i < 3);

  integer f1 = vec1.hcoord(i);
  integer w1 = vec1.hcoord(3); 

  integer f2 = vec2.hcoord(i);
  integer w2 = vec2.hcoord(3);

  rat_vector trans = pp2.to_vector() - pp1.to_vector();

  q = pp2 + ((f2*w1)*trans)/(f1*w2);

  return 1; 
}


int d3_rat_plane::intersection(const d3_rat_plane& Q, 
                               d3_rat_point& i1, d3_rat_point& i2) const
{ 
  // returns:
  // 0: planes pl1 and pl2 do not intersect in a line (are parallel)
  // 1: intersect in the line through i1 and i2
  // 2: are equal

  d3_rat_point a = point1();
  d3_rat_point b = point2();
  d3_rat_point c = point3();


  int w1 = Q.intersection(a,b,i1);
  int w2 = Q.intersection(a,c,i2);


  if (w1 == 0 && w2 == 0) return 0; // parallel

  if (w1 == 2 || w2 == 2)
  { i1 = a;
    if (w1 == 2 && w2 == 2) return 2; // same plane
    if (w1 == 2) i2 = b;
    if (w2 == 2) i2 = c;
  }

  if (w1 == 0 && w2 == 1) i1 = b + i2.to_vector() - a.to_vector();
  if (w2 == 0 && w1 == 1) i2 = c + i1.to_vector() - a.to_vector();

  if(w1 == 1 && w2 == 1) Q.intersection(b,c,i2);

  return 1;
}



d3_rat_plane d3_rat_plane::translate(integer dx, integer dy, integer dz, integer dw) const
{ d3_rat_point p = point1().translate(dx,dy,dz,dw);
  return d3_rat_plane(p,normal());
}

d3_rat_plane d3_rat_plane::translate(const rational& dx, 
                                     const rational& dy, 
                                     const rational& dz) const
{ integer W = dx.denominator()*dy.denominator()*dz.denominator(); 
  return translate(dx.numerator(),dy.numerator(),dz.numerator(), W); 
 }


d3_rat_plane d3_rat_plane::translate(const rat_vector& v) const 
{ d3_rat_point p = point1().translate(v);
  return d3_rat_plane(p,normal());
}


// Distances

d3_rat_plane d3_rat_plane::reflect(const d3_rat_point& q) const
{ // reflect plane across point q
  d3_rat_point a = point1();
  d3_rat_point b = a.translate(normal());
  d3_rat_point a1 = a.reflect(q);
  d3_rat_point b1 = b.reflect(q);
  return d3_rat_plane(a1,b1-a1);
 }


rat_vector d3_rat_plane::normal_project(const d3_rat_point& p) const
{ rat_vector v = p - point1();
  integer nx = ptr()->nx;
  integer ny = ptr()->ny;
  integer nz = ptr()->nz;

  integer A = -(nx*v.hcoord(0)+ny*v.hcoord(1)+nz*v.hcoord(2));
  integer W = nx*nx+ny*ny+nz*nz;
  return rat_vector(A*nx,A*ny,A*nz,W*v.hcoord(3));
}

d3_rat_point d3_rat_plane::reflect_point(const d3_rat_point& p) const
{ return p.translate(2*normal_project(p)); } 


d3_rat_plane  d3_rat_plane::reflect(const d3_rat_plane& Q) const
{ // reflect plane across plane q
  d3_rat_point a = point1();
  d3_rat_point b = a.translate(normal());
  d3_rat_point a1 = Q.reflect_point(a);
  d3_rat_point b1 = Q.reflect_point(b);
  return d3_rat_plane(a1,b1-a1);
 }

/*
rational d3_rat_plane::sqr_dist(const d3_rat_point& p)  const
{ rat_vector vec = normal_project(p);
  return vec.sqr_length();
 }
*/

// ---------------------------------------------------------- 
// new implementation of squared distance ...
 
rational d3_rat_plane::sqr_dist(const d3_rat_point& p)  const
{ 
  integer A = ptr()->nx;
  integer B = ptr()->ny;
  integer C = ptr()->nz;
  integer D = ptr()->nd;
  
  integer x = p.X();
  integer y = p.Y();
  integer z = p.Z();
  integer w = p.W();
  
  //  d = (|A*x/w + B*y/w + C*z/w + D|)/sqrt(A*A+B*B+C*C)
  //    
  //  d = (|A*x + B*y + C*z + D*w|)/(sqrt(A*A+B*B+C*C) * w )
  //
  //  d*d = (|A*x + B*y + C*z + D*w|)^2
  //        ---------------------------
  //            (A*A+B*B+C*C) * w*w
  //
  
  integer sq_nom = A*x + B*y + C*z + D*w;
  sq_nom = sq_nom * sq_nom;
  
  // we could precalculate (A*A + B*B + C*C) ...
  
  integer sq_denom = (A*A + B*B + C*C)*w*w;
  
  return rational(sq_nom, sq_denom);
} 

#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ <= 2

int d3_rat_plane::cmp_distances(const d3_rat_point& p1, const d3_rat_point& p2) const
{
  integer A = ptr()->nx;
  integer B = ptr()->ny;
  integer C = ptr()->nz;
  integer D = ptr()->nd;
  
  integer x1 = p1.X();
  integer y1 = p1.Y();
  integer z1 = p1.Z();
  integer w1 = p1.W();
  
  integer x2 = p2.X();
  integer y2 = p2.Y();
  integer z2 = p2.Z();
  integer w2 = p2.W();    
  
  // compute squared nominators * w1w2
  integer sq_nom_p1 = A*x1*w2 + B*y1*w2 + C*z1*w2 + D*w1*w2;
  sq_nom_p1 = sq_nom_p1 * sq_nom_p1;
  
  integer sq_nom_p2 = A*x2*w1 + B*y2*w1 + C*z2*w1 + D*w1*w2;
  sq_nom_p2 = sq_nom_p2 * sq_nom_p2;
  
  if (sq_nom_p1 > sq_nom_p2)  return 1;
  if (sq_nom_p1 == sq_nom_p2) return 0;
  return -1;
}

#else
int d3_rat_plane::cmp_distances(const d3_rat_point& p1, const d3_rat_point& p2) const
{
/*
  integer A = ptr()->nx;
  integer B = ptr()->ny;
  integer C = ptr()->nz;
  integer D = ptr()->nd;
  
  integer x1 = p1.X();
  integer y1 = p1.Y();
  integer z1 = p1.Z();
  integer w1 = p1.W();
  
  integer x2 = p2.X();
  integer y2 = p2.Y();
  integer z2 = p2.Z();
  integer w2 = p2.W();    
  
  // compute squared nominators * w1w2
  integer sq_nom_p1 = A*x1*w2 + B*y1*w2 + C*z1*w2 + D*w1*w2;
  sq_nom_p1 = sq_nom_p1 * sq_nom_p1;
  
  integer sq_nom_p2 = A*x2*w1 + B*y2*w1 + C*z2*w1 + D*w1*w2;
  sq_nom_p2 = sq_nom_p2 * sq_nom_p2;
  
  if (sq_nom_p1 > sq_nom_p2)  return 1;
  if (sq_nom_p1 == sq_nom_p2) return 0;
  return -1;
*/
  int ressign;
  
  integer A = ptr()->nx;
  integer B = ptr()->ny;
  integer C = ptr()->nz;
  integer D = ptr()->nd;
  
  double  Ad = ptr()->nx_d;
  double  Bd = ptr()->ny_d;
  double  Cd = ptr()->nz_d;
  double  Dd = ptr()->nd_d;      
  
// ******** Begin Expression Compiler Output
// **** Local Variables:
//		RES
//		sq_nom_p1
//		sq_nom_p1_sq
//		sq_nom_p2
//		sq_nom_p2_sq
// **** External Variables:
//		A
//		B
//		C
//		D
//		p1.W()
//		p1.X()
//		p1.Y()
//		p1.Z()
//		p2.W()
//		p2.X()
//		p2.Y()
//		p2.Z()
// **** Sign Result Variables:
//		ressign

{
	// ****** Double evaluation
double A_g_val = Ad;
double A_g_sup = fp::abs(A_g_val);
double B_g_val = Bd;
double B_g_sup = fp::abs(B_g_val);
double C_g_val = Cd;
double C_g_sup = fp::abs(C_g_val);
double D_g_val = Dd;
double D_g_sup = fp::abs(D_g_val);
double p1_W___g_val = p1.WD();
double p1_W___g_sup = fp::abs(p1_W___g_val);
double p1_X___g_val = p1.XD();
double p1_X___g_sup = fp::abs(p1_X___g_val);
double p1_Y___g_val = p1.YD();
double p1_Y___g_sup = fp::abs(p1_Y___g_val);
double p1_Z___g_val = p1.ZD();
double p1_Z___g_sup = fp::abs(p1_Z___g_val);
double p2_W___g_val = p2.WD();
double p2_W___g_sup = fp::abs(p2_W___g_val);
double p2_X___g_val = p2.XD();
double p2_X___g_sup = fp::abs(p2_X___g_val);
double p2_Y___g_val = p2.YD();
double p2_Y___g_sup = fp::abs(p2_Y___g_val);
double p2_Z___g_val = p2.ZD();
double p2_Z___g_sup = fp::abs(p2_Z___g_val);




	// ******************* New Declaration
	double sq_nom_p1_val;
	double sq_nom_p1_sup;

	double tmp_2_val = A_g_val*p1_X___g_val;
	double tmp_2_sup = A_g_sup*p1_X___g_sup;

	double tmp_4_val = tmp_2_val*p2_W___g_val;
	double tmp_4_sup = tmp_2_sup*p2_W___g_sup;

	double tmp_7_val = B_g_val*p1_Y___g_val;
	double tmp_7_sup = B_g_sup*p1_Y___g_sup;

	double tmp_9_val = tmp_7_val*p2_W___g_val;
	double tmp_9_sup = tmp_7_sup*p2_W___g_sup;

	double tmp_10_val = tmp_4_val+tmp_9_val;
	double tmp_10_sup = tmp_4_sup+tmp_9_sup;

	double tmp_13_val = C_g_val*p1_Z___g_val;
	double tmp_13_sup = C_g_sup*p1_Z___g_sup;

	double tmp_15_val = tmp_13_val*p2_W___g_val;
	double tmp_15_sup = tmp_13_sup*p2_W___g_sup;

	double tmp_16_val = tmp_10_val+tmp_15_val;
	double tmp_16_sup = tmp_10_sup+tmp_15_sup;

	double tmp_19_val = D_g_val*p1_W___g_val;
	double tmp_19_sup = D_g_sup*p1_W___g_sup;

	double tmp_21_val = tmp_19_val*p2_W___g_val;
	double tmp_21_sup = tmp_19_sup*p2_W___g_sup;

	double tmp_22_val = tmp_16_val+tmp_21_val;
	double tmp_22_sup = tmp_16_sup+tmp_21_sup;

	sq_nom_p1_val = tmp_22_val;
	sq_nom_p1_sup = tmp_22_sup;

	// ******************* New Declaration
	double sq_nom_p1_sq_val;
	double sq_nom_p1_sq_sup;

	double tmp_25_val = sq_nom_p1_val*sq_nom_p1_val;
	double tmp_25_sup = sq_nom_p1_sup*sq_nom_p1_sup;

	sq_nom_p1_sq_val = tmp_25_val;
	sq_nom_p1_sq_sup = tmp_25_sup;

	// ******************* New Declaration
	double sq_nom_p2_val;
	double sq_nom_p2_sup;

	double tmp_28_val = A_g_val*p2_X___g_val;
	double tmp_28_sup = A_g_sup*p2_X___g_sup;

	double tmp_30_val = tmp_28_val*p1_W___g_val;
	double tmp_30_sup = tmp_28_sup*p1_W___g_sup;

	double tmp_33_val = B_g_val*p2_Y___g_val;
	double tmp_33_sup = B_g_sup*p2_Y___g_sup;

	double tmp_35_val = tmp_33_val*p1_W___g_val;
	double tmp_35_sup = tmp_33_sup*p1_W___g_sup;

	double tmp_36_val = tmp_30_val+tmp_35_val;
	double tmp_36_sup = tmp_30_sup+tmp_35_sup;

	double tmp_39_val = C_g_val*p2_Z___g_val;
	double tmp_39_sup = C_g_sup*p2_Z___g_sup;

	double tmp_41_val = tmp_39_val*p1_W___g_val;
	double tmp_41_sup = tmp_39_sup*p1_W___g_sup;

	double tmp_42_val = tmp_36_val+tmp_41_val;
	double tmp_42_sup = tmp_36_sup+tmp_41_sup;

	double tmp_45_val = D_g_val*p1_W___g_val;
	double tmp_45_sup = D_g_sup*p1_W___g_sup;

	double tmp_47_val = tmp_45_val*p2_W___g_val;
	double tmp_47_sup = tmp_45_sup*p2_W___g_sup;

	double tmp_48_val = tmp_42_val+tmp_47_val;
	double tmp_48_sup = tmp_42_sup+tmp_47_sup;

	sq_nom_p2_val = tmp_48_val;
	sq_nom_p2_sup = tmp_48_sup;

	// ******************* New Declaration
	double sq_nom_p2_sq_val;
	double sq_nom_p2_sq_sup;

	double tmp_51_val = sq_nom_p2_val*sq_nom_p2_val;
	double tmp_51_sup = sq_nom_p2_sup*sq_nom_p2_sup;

	sq_nom_p2_sq_val = tmp_51_val;
	sq_nom_p2_sq_sup = tmp_51_sup;

	// ******************* New Declaration
	double RES_val;
	double RES_sup;

	double tmp_54_val = sq_nom_p1_sq_val-sq_nom_p2_sq_val;
	double tmp_54_sup = sq_nom_p1_sq_sup+sq_nom_p2_sq_sup;

	RES_val = tmp_54_val;
	RES_sup = tmp_54_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 18
	{
	  static double _upper=18*EXCOMP_eps1;
	  if ( fp::abs(RES_val) > RES_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(RES_val);
	  else if ( RES_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}
if ((ressign==EXCOMP_NO_IDEA) || EXCOMP_FALSE )
	// ***** Exact Evaluation
	{
	integer sq_nom_p1 = (((((A*p1.X())*p2.W())+((B*p1.Y())*p2.W()))+((C*p1.Z())*p2.W()))+((D*p1.W())*p2.W()));
	integer sq_nom_p1_sq = (sq_nom_p1*sq_nom_p1);
	integer sq_nom_p2 = (((((A*p2.X())*p1.W())+((B*p2.Y())*p1.W()))+((C*p2.Z())*p1.W()))+((D*p1.W())*p2.W()));
	integer sq_nom_p2_sq = (sq_nom_p2*sq_nom_p2);
	integer RES = (sq_nom_p1_sq-sq_nom_p2_sq);
	ressign = sign(RES);
	}
// ******** End Expression Compiler Output

  
  return ressign;  
   
}
#endif


// ---------------------------------------------------------- 

bool d3_rat_plane::parallel(const d3_rat_plane& Q) const
{ integer nx1 = ptr()->nx;
  integer ny1 = ptr()->ny;
  integer nz1 = ptr()->nz;
  integer nx2 = Q.ptr()->nx;
  integer ny2 = Q.ptr()->ny;
  integer nz2 = Q.ptr()->nz;
  return nz1*ny2 == ny1*nz2 && nx1*nz2 == nz1*nx2 && ny1*nx2 == nx1*ny2;
}


int d3_rat_plane::side_of(const d3_rat_point& q) const
{ 
  d3_rat_point a = point1();
  
  
int ressign;

integer nx = ptr()->nx;
integer ny = ptr()->ny;
integer nz = ptr()->nz;

double nxd = ptr()->nx_d;
double nyd = ptr()->ny_d;
double nzd = ptr()->nz_d;

// ******** Begin Expression Compiler Output
// **** Local Variables:
//		D
//		qx
//		qy
//		qz
// **** External Variables:
//		a.W()
//		a.X()
//		a.Y()
//		a.Z()
//		nx
//		ny
//		nz
//		q.W()
//		q.X()
//		q.Y()
//		q.Z()
// **** Sign Result Variables:
//		ressign

{
	// ****** Double evaluation
double a_W___g_val = a.WD();
double a_W___g_sup = fp::abs(a_W___g_val);
double a_X___g_val = a.XD();
double a_X___g_sup = fp::abs(a_X___g_val);
double a_Y___g_val = a.YD();
double a_Y___g_sup = fp::abs(a_Y___g_val);
double a_Z___g_val = a.ZD();
double a_Z___g_sup = fp::abs(a_Z___g_val);
double nx_g_val = nxd;
double nx_g_sup = fp::abs(nx_g_val);
double ny_g_val = nyd;
double ny_g_sup = fp::abs(ny_g_val);
double nz_g_val = nzd;
double nz_g_sup = fp::abs(nz_g_val);
double q_W___g_val = q.WD();
double q_W___g_sup = fp::abs(q_W___g_val);
double q_X___g_val = q.XD();
double q_X___g_sup = fp::abs(q_X___g_val);
double q_Y___g_val = q.YD();
double q_Y___g_sup = fp::abs(q_Y___g_val);
double q_Z___g_val = q.ZD();
double q_Z___g_sup = fp::abs(q_Z___g_val);




	// ******************* New Declaration
	double qx_val;
	double qx_sup;

	double tmp_2_val = q_X___g_val*a_W___g_val;
	double tmp_2_sup = q_X___g_sup*a_W___g_sup;

	double tmp_5_val = a_X___g_val*q_W___g_val;
	double tmp_5_sup = a_X___g_sup*q_W___g_sup;

	double tmp_6_val = tmp_2_val-tmp_5_val;
	double tmp_6_sup = tmp_2_sup+tmp_5_sup;

	qx_val = tmp_6_val;
	qx_sup = tmp_6_sup;

	// ******************* New Declaration
	double qy_val;
	double qy_sup;

	double tmp_9_val = q_Y___g_val*a_W___g_val;
	double tmp_9_sup = q_Y___g_sup*a_W___g_sup;

	double tmp_12_val = a_Y___g_val*q_W___g_val;
	double tmp_12_sup = a_Y___g_sup*q_W___g_sup;

	double tmp_13_val = tmp_9_val-tmp_12_val;
	double tmp_13_sup = tmp_9_sup+tmp_12_sup;

	qy_val = tmp_13_val;
	qy_sup = tmp_13_sup;

	// ******************* New Declaration
	double qz_val;
	double qz_sup;

	double tmp_16_val = q_Z___g_val*a_W___g_val;
	double tmp_16_sup = q_Z___g_sup*a_W___g_sup;

	double tmp_19_val = a_Z___g_val*q_W___g_val;
	double tmp_19_sup = a_Z___g_sup*q_W___g_sup;

	double tmp_20_val = tmp_16_val-tmp_19_val;
	double tmp_20_sup = tmp_16_sup+tmp_19_sup;

	qz_val = tmp_20_val;
	qz_sup = tmp_20_sup;

	// ******************* New Declaration
	double D_val;
	double D_sup;

	double tmp_23_val = nx_g_val*qx_val;
	double tmp_23_sup = nx_g_sup*qx_sup;

	double tmp_26_val = ny_g_val*qy_val;
	double tmp_26_sup = ny_g_sup*qy_sup;

	double tmp_27_val = tmp_23_val+tmp_26_val;
	double tmp_27_sup = tmp_23_sup+tmp_26_sup;

	double tmp_30_val = nz_g_val*qz_val;
	double tmp_30_sup = nz_g_sup*qz_sup;

	double tmp_31_val = tmp_27_val+tmp_30_val;
	double tmp_31_sup = tmp_27_sup+tmp_30_sup;

	D_val = tmp_31_val;
	D_sup = tmp_31_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 8
	{
	  static double _upper=8*EXCOMP_eps1;
	  if ( fp::abs(D_val) > D_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(D_val);
	  else if ( D_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}
if ((ressign==EXCOMP_NO_IDEA) || EXCOMP_FALSE )
	// ***** Exact Evaluation
	{
	integer qx = ((q.X()*a.W())-(a.X()*q.W()));
	integer qy = ((q.Y()*a.W())-(a.Y()*q.W()));
	integer qz = ((q.Z()*a.W())-(a.Z()*q.W()));
	integer D = (((nx*qx)+(ny*qy))+(nz*qz));
	ressign = sign(D);
	}
// ******** End Expression Compiler Output

  
  return -ressign;  
  
/* old:
  integer qx = q.X()*a.W() - a.X()*q.W();
  integer qy = q.Y()*a.W() - a.Y()*q.W();
  integer qz = q.Z()*a.W() - a.Z()*q.W();

  integer nx = ptr()->nx;
  integer ny = ptr()->ny;
  integer nz = ptr()->nz;

  integer D = nx*qx + ny*qy + nz*qz;

  if (D > 0) return +1;
  if (D < 0) return -1;
  return 0;
*/  
}


d3_plane d3_rat_plane::to_d3_plane() const
{ return d3_plane(point1().to_d3_point(),
                  point2().to_d3_point(),
                  point3().to_d3_point()); 
}


bool d3_rat_plane::operator==(const d3_rat_plane& P) const 
{ return parallel(P) && contains(P.point1()); }

   
ostream& operator<<(ostream& out, const d3_rat_plane& P)
{ cout << P.point1() << " " << P.normal(); return out; } 

istream& operator>>(istream& in, d3_rat_plane& P) 
{ d3_rat_point p;
  rat_vector n(3);
  cin >> p >> n;
  P = d3_rat_plane(p,n);
  return in; 
} 

LEDA_END_NAMESPACE
