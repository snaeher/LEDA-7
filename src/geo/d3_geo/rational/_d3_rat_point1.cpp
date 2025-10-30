/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _d3_rat_point1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/numbers/expcomp.h>

LEDA_BEGIN_NAMESPACE

#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ == 1

int cmp_distances(const d3_rat_point& p1, const d3_rat_point& p2, 
                  const d3_rat_point& p3, const d3_rat_point& p4)
{
 int ressign=0;

 integer x1 = p1.X();
 integer y1 = p1.Y();
 integer z1 = p1.Z();
 integer w1 = p1.W();
 integer x2 = p2.X();
 integer y2 = p2.Y();
 integer z2 = p2.Z();
 integer w2 = p2.W();
 integer dxa = ((x1*w2)-(x2*w1));
 integer dya = ((y1*w2)-(y2*w1));
 integer dza = ((z1*w2)-(z2*w1));
 integer x3 = p3.X();
 integer y3 = p3.Y();
 integer z3 = p3.Z();
 integer w3 = p3.W();
 integer x4 = p4.X();
 integer y4 = p4.Y();
 integer z4 = p4.Z();
 integer w4 = p4.W();
 integer dxb = ((x3*w4)-(x4*w3));
 integer dyb = ((y3*w4)-(y4*w3));
 integer dzb = ((z3*w4)-(z4*w3));
 integer zl1 = (((dxa*dxa)+(dya*dya))+(dza*dza));
 integer n1 = (((w1*w1)*w2)*w2);
 integer zl2 = (((dxb*dxb)+(dyb*dyb))+(dzb*dzb));
 integer n2 = (((w3*w3)*w4)*w4);
 integer val = ((zl1*n2)-(zl2*n1));
 ressign = sign(val);
  
 return ressign;
}

#else


int cmp_distances(const d3_rat_point& p1, const d3_rat_point& p2, 
                  const d3_rat_point& p3, const d3_rat_point& p4)
{
int ressign=0;

// ******** Begin Expression Compiler Output
// **** Local Variables:
//		dxa
//		dxb
//		dya
//		dyb
//		dza
//		dzb
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
//		z3
//		z4
//		zl1
//		zl2
// **** External Variables:
//		p1.W()
//		p1.X()
//		p1.Y()
//		p1.Z()
//		p2.W()
//		p2.X()
//		p2.Y()
//		p2.Z()
//		p3.W()
//		p3.X()
//		p3.Y()
//		p3.Z()
//		p4.W()
//		p4.X()
//		p4.Y()
//		p4.Z()
// **** Sign Result Variables:
//		ressign

if (d3_rat_point::use_filter != 0)
{
	// ****** Double evaluation
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
double p3_W___g_val = p3.WD();
double p3_W___g_sup = fp::abs(p3_W___g_val);
double p3_X___g_val = p3.XD();
double p3_X___g_sup = fp::abs(p3_X___g_val);
double p3_Y___g_val = p3.YD();
double p3_Y___g_sup = fp::abs(p3_Y___g_val);
double p3_Z___g_val = p3.ZD();
double p3_Z___g_sup = fp::abs(p3_Z___g_val);
double p4_W___g_val = p4.WD();
double p4_W___g_sup = fp::abs(p4_W___g_val);
double p4_X___g_val = p4.XD();
double p4_X___g_sup = fp::abs(p4_X___g_val);
double p4_Y___g_val = p4.YD();
double p4_Y___g_sup = fp::abs(p4_Y___g_val);
double p4_Z___g_val = p4.ZD();
double p4_Z___g_sup = fp::abs(p4_Z___g_val);




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
	double z1_val;
	double z1_sup;

	z1_val = p1_Z___g_val;
	z1_sup = p1_Z___g_sup;

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
	double z2_val;
	double z2_sup;

	z2_val = p2_Z___g_val;
	z2_sup = p2_Z___g_sup;

	// ******************* New Declaration
	double w2_val;
	double w2_sup;

	w2_val = p2_W___g_val;
	w2_sup = p2_W___g_sup;

	// ******************* New Declaration
	double dxa_val;
	double dxa_sup;

	double tmp_10_val = x1_val*w2_val;
	double tmp_10_sup = x1_sup*w2_sup;

	double tmp_13_val = x2_val*w1_val;
	double tmp_13_sup = x2_sup*w1_sup;

	double tmp_14_val = tmp_10_val-tmp_13_val;
	double tmp_14_sup = tmp_10_sup+tmp_13_sup;

	dxa_val = tmp_14_val;
	dxa_sup = tmp_14_sup;

	// ******************* New Declaration
	double dya_val;
	double dya_sup;

	double tmp_17_val = y1_val*w2_val;
	double tmp_17_sup = y1_sup*w2_sup;

	double tmp_20_val = y2_val*w1_val;
	double tmp_20_sup = y2_sup*w1_sup;

	double tmp_21_val = tmp_17_val-tmp_20_val;
	double tmp_21_sup = tmp_17_sup+tmp_20_sup;

	dya_val = tmp_21_val;
	dya_sup = tmp_21_sup;

	// ******************* New Declaration
	double dza_val;
	double dza_sup;

	double tmp_24_val = z1_val*w2_val;
	double tmp_24_sup = z1_sup*w2_sup;

	double tmp_27_val = z2_val*w1_val;
	double tmp_27_sup = z2_sup*w1_sup;

	double tmp_28_val = tmp_24_val-tmp_27_val;
	double tmp_28_sup = tmp_24_sup+tmp_27_sup;

	dza_val = tmp_28_val;
	dza_sup = tmp_28_sup;

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
	double z3_val;
	double z3_sup;

	z3_val = p3_Z___g_val;
	z3_sup = p3_Z___g_sup;

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
	double z4_val;
	double z4_sup;

	z4_val = p4_Z___g_val;
	z4_sup = p4_Z___g_sup;

	// ******************* New Declaration
	double w4_val;
	double w4_sup;

	w4_val = p4_W___g_val;
	w4_sup = p4_W___g_sup;

	// ******************* New Declaration
	double dxb_val;
	double dxb_sup;

	double tmp_39_val = x3_val*w4_val;
	double tmp_39_sup = x3_sup*w4_sup;

	double tmp_42_val = x4_val*w3_val;
	double tmp_42_sup = x4_sup*w3_sup;

	double tmp_43_val = tmp_39_val-tmp_42_val;
	double tmp_43_sup = tmp_39_sup+tmp_42_sup;

	dxb_val = tmp_43_val;
	dxb_sup = tmp_43_sup;

	// ******************* New Declaration
	double dyb_val;
	double dyb_sup;

	double tmp_46_val = y3_val*w4_val;
	double tmp_46_sup = y3_sup*w4_sup;

	double tmp_49_val = y4_val*w3_val;
	double tmp_49_sup = y4_sup*w3_sup;

	double tmp_50_val = tmp_46_val-tmp_49_val;
	double tmp_50_sup = tmp_46_sup+tmp_49_sup;

	dyb_val = tmp_50_val;
	dyb_sup = tmp_50_sup;

	// ******************* New Declaration
	double dzb_val;
	double dzb_sup;

	double tmp_53_val = z3_val*w4_val;
	double tmp_53_sup = z3_sup*w4_sup;

	double tmp_56_val = z4_val*w3_val;
	double tmp_56_sup = z4_sup*w3_sup;

	double tmp_57_val = tmp_53_val-tmp_56_val;
	double tmp_57_sup = tmp_53_sup+tmp_56_sup;

	dzb_val = tmp_57_val;
	dzb_sup = tmp_57_sup;

	// ******************* New Declaration
	double zl1_val;
	double zl1_sup;

	double tmp_60_val = dxa_val*dxa_val;
	double tmp_60_sup = dxa_sup*dxa_sup;

	double tmp_63_val = dya_val*dya_val;
	double tmp_63_sup = dya_sup*dya_sup;

	double tmp_64_val = tmp_60_val+tmp_63_val;
	double tmp_64_sup = tmp_60_sup+tmp_63_sup;

	double tmp_67_val = dza_val*dza_val;
	double tmp_67_sup = dza_sup*dza_sup;

	double tmp_68_val = tmp_64_val+tmp_67_val;
	double tmp_68_sup = tmp_64_sup+tmp_67_sup;

	zl1_val = tmp_68_val;
	zl1_sup = tmp_68_sup;

	// ******************* New Declaration
	double n1_val;
	double n1_sup;

	double tmp_71_val = w1_val*w1_val;
	double tmp_71_sup = w1_sup*w1_sup;

	double tmp_73_val = tmp_71_val*w2_val;
	double tmp_73_sup = tmp_71_sup*w2_sup;

	double tmp_75_val = tmp_73_val*w2_val;
	double tmp_75_sup = tmp_73_sup*w2_sup;

	n1_val = tmp_75_val;
	n1_sup = tmp_75_sup;

	// ******************* New Declaration
	double zl2_val;
	double zl2_sup;

	double tmp_78_val = dxb_val*dxb_val;
	double tmp_78_sup = dxb_sup*dxb_sup;

	double tmp_81_val = dyb_val*dyb_val;
	double tmp_81_sup = dyb_sup*dyb_sup;

	double tmp_82_val = tmp_78_val+tmp_81_val;
	double tmp_82_sup = tmp_78_sup+tmp_81_sup;

	double tmp_85_val = dzb_val*dzb_val;
	double tmp_85_sup = dzb_sup*dzb_sup;

	double tmp_86_val = tmp_82_val+tmp_85_val;
	double tmp_86_sup = tmp_82_sup+tmp_85_sup;

	zl2_val = tmp_86_val;
	zl2_sup = tmp_86_sup;

	// ******************* New Declaration
	double n2_val;
	double n2_sup;

	double tmp_89_val = w3_val*w3_val;
	double tmp_89_sup = w3_sup*w3_sup;

	double tmp_91_val = tmp_89_val*w4_val;
	double tmp_91_sup = tmp_89_sup*w4_sup;

	double tmp_93_val = tmp_91_val*w4_val;
	double tmp_93_sup = tmp_91_sup*w4_sup;

	n2_val = tmp_93_val;
	n2_sup = tmp_93_sup;

	// ******************* New Declaration
	double val_val;
	double val_sup;

	double tmp_96_val = zl1_val*n2_val;
	double tmp_96_sup = zl1_sup*n2_sup;

	double tmp_99_val = zl2_val*n1_val;
	double tmp_99_sup = zl2_sup*n1_sup;

	double tmp_100_val = tmp_96_val-tmp_99_val;
	double tmp_100_sup = tmp_96_sup+tmp_99_sup;

	val_val = tmp_100_val;
	val_sup = tmp_100_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 20
	{
	  static double _upper=20*EXCOMP_eps1;
	  if ( fp::abs(val_val) > val_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(val_val);
	  else if ( val_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}
if ((d3_rat_point::use_filter) == 0 || (ressign==EXCOMP_NO_IDEA) || EXCOMP_FALSE )
	// ***** Exact Evaluation
	{
	integer x1 = p1.X();
	integer y1 = p1.Y();
	integer z1 = p1.Z();
	integer w1 = p1.W();
	integer x2 = p2.X();
	integer y2 = p2.Y();
	integer z2 = p2.Z();
	integer w2 = p2.W();
	integer dxa = ((x1*w2)-(x2*w1));
	integer dya = ((y1*w2)-(y2*w1));
	integer dza = ((z1*w2)-(z2*w1));
	integer x3 = p3.X();
	integer y3 = p3.Y();
	integer z3 = p3.Z();
	integer w3 = p3.W();
	integer x4 = p4.X();
	integer y4 = p4.Y();
	integer z4 = p4.Z();
	integer w4 = p4.W();
	integer dxb = ((x3*w4)-(x4*w3));
	integer dyb = ((y3*w4)-(y4*w3));
	integer dzb = ((z3*w4)-(z4*w3));
	integer zl1 = (((dxa*dxa)+(dya*dya))+(dza*dza));
	integer n1 = (((w1*w1)*w2)*w2);
	integer zl2 = (((dxb*dxb)+(dyb*dyb))+(dzb*dzb));
	integer n2 = (((w3*w3)*w4)*w4);
	integer val = ((zl1*n2)-(zl2*n1));
	ressign = sign(val);
	}
// ******** End Expression Compiler Output

  
  
  return ressign;
}


#endif


// internal compiler error in g++-3.0.1  (s.n.  August 2001)


#if __GNUC__ == 3 && __GNUC_MINOR__ == 0 && __GNUC_PATCHLEVEL__ == 1

int side_of_sphere(const d3_rat_point& a, 
                   const d3_rat_point& b,
                   const d3_rat_point& c,
                   const d3_rat_point& d,
                   const d3_rat_point& e)
{
  int ressign;

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
  integer b1 = ((BX*AW)-(AX*BW));
  integer b2 = ((BY*AW)-(AY*BW));
  integer b3 = ((BZ*AW)-(AZ*BW));
  integer c1 = ((CX*AW)-(AX*CW));
  integer c2 = ((CY*AW)-(AY*CW));
  integer c3 = ((CZ*AW)-(AZ*CW));
  integer d1 = ((DX*AW)-(AX*DW));
  integer d2 = ((DY*AW)-(AY*DW));
  integer d3 = ((DZ*AW)-(AZ*DW));
  integer e1 = ((EX*AW)-(AX*EW));
  integer e2 = ((EY*AW)-(AY*EW));
  integer e3 = ((EZ*AW)-(AZ*EW));
  integer wert1 = (AW*BW);
  integer b_x = (b1*wert1);
  integer b_y = (b2*wert1);
  integer b_z = (b3*wert1);
  integer b_w = (((b1*b1)+(b2*b2))+(b3*b3));
  integer wert2 = (AW*CW);
  integer c_x = (c1*wert2);
  integer c_y = (c2*wert2);
  integer c_z = (c3*wert2);
  integer c_w = (((c1*c1)+(c2*c2))+(c3*c3));
  integer wert3 = (AW*DW);
  integer d_x = (d1*wert3);
  integer d_y = (d2*wert3);
  integer d_z = (d3*wert3);
  integer d_w = (((d1*d1)+(d2*d2))+(d3*d3));
  integer wert4 = (AW*EW);
  integer e_x = (e1*wert4);
  integer e_y = (e2*wert4);
  integer e_z = (e3*wert4);
  integer e_w = (((e1*e1)+(e2*e2))+(e3*e3));
  integer X1 = ((c_x*b_w)-(b_x*c_w));
  integer Y1 = ((c_y*b_w)-(b_y*c_w));
  integer Z1 = ((c_z*b_w)-(b_z*c_w));
  integer X2 = ((d_x*b_w)-(b_x*d_w));
  integer Y2 = ((d_y*b_w)-(b_y*d_w));
  integer Z2 = ((d_z*b_w)-(b_z*d_w));
  integer X3 = ((e_x*b_w)-(b_x*e_w));
  integer Y3 = ((e_y*b_w)-(b_y*e_w));
  integer Z3 = ((e_z*b_w)-(b_z*e_w));
  integer cwx = ((Z1*Y2)-(Y1*Z2));
  integer cwy = ((X1*Z2)-(Z1*X2));
  integer cwz = ((Y1*X2)-(X1*Y2));
  integer Dw = (((cwx*X3)+(cwy*Y3))+(cwz*Z3));
  ressign = sign(Dw);

  return -ressign; 
}

#else

int side_of_sphere(const d3_rat_point& a, 
                   const d3_rat_point& b,
                   const d3_rat_point& c,
                   const d3_rat_point& d,
                   const d3_rat_point& e)
{ 
  d3_rat_point::sos_count++;
  
  int ressign=0;
 

// **** Sign Result Variables:
//		ressign
if (d3_rat_point::use_filter != 0)
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
double b_W___g_val = b.WD();
double b_W___g_sup = fp::abs(b_W___g_val);
double b_X___g_val = b.XD();
double b_X___g_sup = fp::abs(b_X___g_val);
double b_Y___g_val = b.YD();
double b_Y___g_sup = fp::abs(b_Y___g_val);
double b_Z___g_val = b.ZD();
double b_Z___g_sup = fp::abs(b_Z___g_val);
double c_W___g_val = c.WD();
double c_W___g_sup = fp::abs(c_W___g_val);
double c_X___g_val = c.XD();
double c_X___g_sup = fp::abs(c_X___g_val);
double c_Y___g_val = c.YD();
double c_Y___g_sup = fp::abs(c_Y___g_val);
double c_Z___g_val = c.ZD();
double c_Z___g_sup = fp::abs(c_Z___g_val);
double d_W___g_val = d.WD();
double d_W___g_sup = fp::abs(d_W___g_val);
double d_X___g_val = d.XD();
double d_X___g_sup = fp::abs(d_X___g_val);
double d_Y___g_val = d.YD();
double d_Y___g_sup = fp::abs(d_Y___g_val);
double d_Z___g_val = d.ZD();
double d_Z___g_sup = fp::abs(d_Z___g_val);
double e_W___g_val = e.WD();
double e_W___g_sup = fp::abs(e_W___g_val);
double e_X___g_val = e.XD();
double e_X___g_sup = fp::abs(e_X___g_val);
double e_Y___g_val = e.YD();
double e_Y___g_sup = fp::abs(e_Y___g_val);
double e_Z___g_val = e.ZD();
double e_Z___g_sup = fp::abs(e_Z___g_val);


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
	double AZ_val;
	double AZ_sup;

	AZ_val = a_Z___g_val;
	AZ_sup = a_Z___g_sup;

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
	double BZ_val;
	double BZ_sup;

	BZ_val = b_Z___g_val;
	BZ_sup = b_Z___g_sup;

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
	double CZ_val;
	double CZ_sup;

	CZ_val = c_Z___g_val;
	CZ_sup = c_Z___g_sup;

	// ******************* New Declaration
	double CW_val;
	double CW_sup;

	CW_val = c_W___g_val;
	CW_sup = c_W___g_sup;

	// ******************* New Declaration
	double DX_val;
	double DX_sup;

	DX_val = d_X___g_val;
	DX_sup = d_X___g_sup;

	// ******************* New Declaration
	double DY_val;
	double DY_sup;

	DY_val = d_Y___g_val;
	DY_sup = d_Y___g_sup;

	// ******************* New Declaration
	double DZ_val;
	double DZ_sup;

	DZ_val = d_Z___g_val;
	DZ_sup = d_Z___g_sup;

	// ******************* New Declaration
	double DW_val;
	double DW_sup;

	DW_val = d_W___g_val;
	DW_sup = d_W___g_sup;

	// ******************* New Declaration
	double EX_val;
	double EX_sup;

	EX_val = e_X___g_val;
	EX_sup = e_X___g_sup;

	// ******************* New Declaration
	double EY_val;
	double EY_sup;

	EY_val = e_Y___g_val;
	EY_sup = e_Y___g_sup;

	// ******************* New Declaration
	double EZ_val;
	double EZ_sup;

	EZ_val = e_Z___g_val;
	EZ_sup = e_Z___g_sup;

	// ******************* New Declaration
	double EW_val;
	double EW_sup;

	EW_val = e_W___g_val;
	EW_sup = e_W___g_sup;

	// ******************* New Declaration
	double b1_val;
	double b1_sup;

	double tmp_22_val = BX_val*AW_val;
	double tmp_22_sup = BX_sup*AW_sup;

	double tmp_25_val = AX_val*BW_val;
	double tmp_25_sup = AX_sup*BW_sup;

	double tmp_26_val = tmp_22_val-tmp_25_val;
	double tmp_26_sup = tmp_22_sup+tmp_25_sup;

	b1_val = tmp_26_val;
	b1_sup = tmp_26_sup;

	// ******************* New Declaration
	double b2_val;
	double b2_sup;

	double tmp_29_val = BY_val*AW_val;
	double tmp_29_sup = BY_sup*AW_sup;

	double tmp_32_val = AY_val*BW_val;
	double tmp_32_sup = AY_sup*BW_sup;

	double tmp_33_val = tmp_29_val-tmp_32_val;
	double tmp_33_sup = tmp_29_sup+tmp_32_sup;

	b2_val = tmp_33_val;
	b2_sup = tmp_33_sup;

	// ******************* New Declaration
	double b3_val;
	double b3_sup;

	double tmp_36_val = BZ_val*AW_val;
	double tmp_36_sup = BZ_sup*AW_sup;

	double tmp_39_val = AZ_val*BW_val;
	double tmp_39_sup = AZ_sup*BW_sup;

	double tmp_40_val = tmp_36_val-tmp_39_val;
	double tmp_40_sup = tmp_36_sup+tmp_39_sup;

	b3_val = tmp_40_val;
	b3_sup = tmp_40_sup;

	// ******************* New Declaration
	double c1_val;
	double c1_sup;

	double tmp_43_val = CX_val*AW_val;
	double tmp_43_sup = CX_sup*AW_sup;

	double tmp_46_val = AX_val*CW_val;
	double tmp_46_sup = AX_sup*CW_sup;

	double tmp_47_val = tmp_43_val-tmp_46_val;
	double tmp_47_sup = tmp_43_sup+tmp_46_sup;

	c1_val = tmp_47_val;
	c1_sup = tmp_47_sup;

	// ******************* New Declaration
	double c2_val;
	double c2_sup;

	double tmp_50_val = CY_val*AW_val;
	double tmp_50_sup = CY_sup*AW_sup;

	double tmp_53_val = AY_val*CW_val;
	double tmp_53_sup = AY_sup*CW_sup;

	double tmp_54_val = tmp_50_val-tmp_53_val;
	double tmp_54_sup = tmp_50_sup+tmp_53_sup;

	c2_val = tmp_54_val;
	c2_sup = tmp_54_sup;

	// ******************* New Declaration
	double c3_val;
	double c3_sup;

	double tmp_57_val = CZ_val*AW_val;
	double tmp_57_sup = CZ_sup*AW_sup;

	double tmp_60_val = AZ_val*CW_val;
	double tmp_60_sup = AZ_sup*CW_sup;

	double tmp_61_val = tmp_57_val-tmp_60_val;
	double tmp_61_sup = tmp_57_sup+tmp_60_sup;

	c3_val = tmp_61_val;
	c3_sup = tmp_61_sup;

	// ******************* New Declaration
	double d1_val;
	double d1_sup;

	double tmp_64_val = DX_val*AW_val;
	double tmp_64_sup = DX_sup*AW_sup;

	double tmp_67_val = AX_val*DW_val;
	double tmp_67_sup = AX_sup*DW_sup;

	double tmp_68_val = tmp_64_val-tmp_67_val;
	double tmp_68_sup = tmp_64_sup+tmp_67_sup;

	d1_val = tmp_68_val;
	d1_sup = tmp_68_sup;

	// ******************* New Declaration
	double d2_val;
	double d2_sup;

	double tmp_71_val = DY_val*AW_val;
	double tmp_71_sup = DY_sup*AW_sup;

	double tmp_74_val = AY_val*DW_val;
	double tmp_74_sup = AY_sup*DW_sup;

	double tmp_75_val = tmp_71_val-tmp_74_val;
	double tmp_75_sup = tmp_71_sup+tmp_74_sup;

	d2_val = tmp_75_val;
	d2_sup = tmp_75_sup;

	// ******************* New Declaration
	double d3_val;
	double d3_sup;

	double tmp_78_val = DZ_val*AW_val;
	double tmp_78_sup = DZ_sup*AW_sup;

	double tmp_81_val = AZ_val*DW_val;
	double tmp_81_sup = AZ_sup*DW_sup;

	double tmp_82_val = tmp_78_val-tmp_81_val;
	double tmp_82_sup = tmp_78_sup+tmp_81_sup;

	d3_val = tmp_82_val;
	d3_sup = tmp_82_sup;

	// ******************* New Declaration
	double e1_val;
	double e1_sup;

	double tmp_85_val = EX_val*AW_val;
	double tmp_85_sup = EX_sup*AW_sup;

	double tmp_88_val = AX_val*EW_val;
	double tmp_88_sup = AX_sup*EW_sup;

	double tmp_89_val = tmp_85_val-tmp_88_val;
	double tmp_89_sup = tmp_85_sup+tmp_88_sup;

	e1_val = tmp_89_val;
	e1_sup = tmp_89_sup;

	// ******************* New Declaration
	double e2_val;
	double e2_sup;

	double tmp_92_val = EY_val*AW_val;
	double tmp_92_sup = EY_sup*AW_sup;

	double tmp_95_val = AY_val*EW_val;
	double tmp_95_sup = AY_sup*EW_sup;

	double tmp_96_val = tmp_92_val-tmp_95_val;
	double tmp_96_sup = tmp_92_sup+tmp_95_sup;

	e2_val = tmp_96_val;
	e2_sup = tmp_96_sup;

	// ******************* New Declaration
	double e3_val;
	double e3_sup;

	double tmp_99_val = EZ_val*AW_val;
	double tmp_99_sup = EZ_sup*AW_sup;

	double tmp_102_val = AZ_val*EW_val;
	double tmp_102_sup = AZ_sup*EW_sup;

	double tmp_103_val = tmp_99_val-tmp_102_val;
	double tmp_103_sup = tmp_99_sup+tmp_102_sup;

	e3_val = tmp_103_val;
	e3_sup = tmp_103_sup;

	// ******************* New Declaration
	double wert1_val;
	double wert1_sup;

	double tmp_106_val = AW_val*BW_val;
	double tmp_106_sup = AW_sup*BW_sup;

	wert1_val = tmp_106_val;
	wert1_sup = tmp_106_sup;

	// ******************* New Declaration
	double b_x_val;
	double b_x_sup;

	double tmp_109_val = b1_val*wert1_val;
	double tmp_109_sup = b1_sup*wert1_sup;

	b_x_val = tmp_109_val;
	b_x_sup = tmp_109_sup;

	// ******************* New Declaration
	double b_y_val;
	double b_y_sup;

	double tmp_112_val = b2_val*wert1_val;
	double tmp_112_sup = b2_sup*wert1_sup;

	b_y_val = tmp_112_val;
	b_y_sup = tmp_112_sup;

	// ******************* New Declaration
	double b_z_val;
	double b_z_sup;

	double tmp_115_val = b3_val*wert1_val;
	double tmp_115_sup = b3_sup*wert1_sup;

	b_z_val = tmp_115_val;
	b_z_sup = tmp_115_sup;

	// ******************* New Declaration
	double b_w_val;
	double b_w_sup;

	double tmp_118_val = b1_val*b1_val;
	double tmp_118_sup = b1_sup*b1_sup;

	double tmp_121_val = b2_val*b2_val;
	double tmp_121_sup = b2_sup*b2_sup;

	double tmp_122_val = tmp_118_val+tmp_121_val;
	double tmp_122_sup = tmp_118_sup+tmp_121_sup;

	double tmp_125_val = b3_val*b3_val;
	double tmp_125_sup = b3_sup*b3_sup;

	double tmp_126_val = tmp_122_val+tmp_125_val;
	double tmp_126_sup = tmp_122_sup+tmp_125_sup;

	b_w_val = tmp_126_val;
	b_w_sup = tmp_126_sup;

	// ******************* New Declaration
	double wert2_val;
	double wert2_sup;

	double tmp_129_val = AW_val*CW_val;
	double tmp_129_sup = AW_sup*CW_sup;

	wert2_val = tmp_129_val;
	wert2_sup = tmp_129_sup;

	// ******************* New Declaration
	double c_x_val;
	double c_x_sup;

	double tmp_132_val = c1_val*wert2_val;
	double tmp_132_sup = c1_sup*wert2_sup;

	c_x_val = tmp_132_val;
	c_x_sup = tmp_132_sup;

	// ******************* New Declaration
	double c_y_val;
	double c_y_sup;

	double tmp_135_val = c2_val*wert2_val;
	double tmp_135_sup = c2_sup*wert2_sup;

	c_y_val = tmp_135_val;
	c_y_sup = tmp_135_sup;

	// ******************* New Declaration
	double c_z_val;
	double c_z_sup;

	double tmp_138_val = c3_val*wert2_val;
	double tmp_138_sup = c3_sup*wert2_sup;

	c_z_val = tmp_138_val;
	c_z_sup = tmp_138_sup;

	// ******************* New Declaration
	double c_w_val;
	double c_w_sup;

	double tmp_141_val = c1_val*c1_val;
	double tmp_141_sup = c1_sup*c1_sup;

	double tmp_144_val = c2_val*c2_val;
	double tmp_144_sup = c2_sup*c2_sup;

	double tmp_145_val = tmp_141_val+tmp_144_val;
	double tmp_145_sup = tmp_141_sup+tmp_144_sup;

	double tmp_148_val = c3_val*c3_val;
	double tmp_148_sup = c3_sup*c3_sup;

	double tmp_149_val = tmp_145_val+tmp_148_val;
	double tmp_149_sup = tmp_145_sup+tmp_148_sup;

	c_w_val = tmp_149_val;
	c_w_sup = tmp_149_sup;

	// ******************* New Declaration
	double wert3_val;
	double wert3_sup;

	double tmp_152_val = AW_val*DW_val;
	double tmp_152_sup = AW_sup*DW_sup;

	wert3_val = tmp_152_val;
	wert3_sup = tmp_152_sup;

	// ******************* New Declaration
	double d_x_val;
	double d_x_sup;

	double tmp_155_val = d1_val*wert3_val;
	double tmp_155_sup = d1_sup*wert3_sup;

	d_x_val = tmp_155_val;
	d_x_sup = tmp_155_sup;

	// ******************* New Declaration
	double d_y_val;
	double d_y_sup;

	double tmp_158_val = d2_val*wert3_val;
	double tmp_158_sup = d2_sup*wert3_sup;

	d_y_val = tmp_158_val;
	d_y_sup = tmp_158_sup;

	// ******************* New Declaration
	double d_z_val;
	double d_z_sup;

	double tmp_161_val = d3_val*wert3_val;
	double tmp_161_sup = d3_sup*wert3_sup;

	d_z_val = tmp_161_val;
	d_z_sup = tmp_161_sup;

	// ******************* New Declaration
	double d_w_val;
	double d_w_sup;

	double tmp_164_val = d1_val*d1_val;
	double tmp_164_sup = d1_sup*d1_sup;

	double tmp_167_val = d2_val*d2_val;
	double tmp_167_sup = d2_sup*d2_sup;

	double tmp_168_val = tmp_164_val+tmp_167_val;
	double tmp_168_sup = tmp_164_sup+tmp_167_sup;

	double tmp_171_val = d3_val*d3_val;
	double tmp_171_sup = d3_sup*d3_sup;

	double tmp_172_val = tmp_168_val+tmp_171_val;
	double tmp_172_sup = tmp_168_sup+tmp_171_sup;

	d_w_val = tmp_172_val;
	d_w_sup = tmp_172_sup;

	// ******************* New Declaration
	double wert4_val;
	double wert4_sup;

	double tmp_175_val = AW_val*EW_val;
	double tmp_175_sup = AW_sup*EW_sup;

	wert4_val = tmp_175_val;
	wert4_sup = tmp_175_sup;

	// ******************* New Declaration
	double e_x_val;
	double e_x_sup;

	double tmp_178_val = e1_val*wert4_val;
	double tmp_178_sup = e1_sup*wert4_sup;

	e_x_val = tmp_178_val;
	e_x_sup = tmp_178_sup;

	// ******************* New Declaration
	double e_y_val;
	double e_y_sup;

	double tmp_181_val = e2_val*wert4_val;
	double tmp_181_sup = e2_sup*wert4_sup;

	e_y_val = tmp_181_val;
	e_y_sup = tmp_181_sup;

	// ******************* New Declaration
	double e_z_val;
	double e_z_sup;

	double tmp_184_val = e3_val*wert4_val;
	double tmp_184_sup = e3_sup*wert4_sup;

	e_z_val = tmp_184_val;
	e_z_sup = tmp_184_sup;

	// ******************* New Declaration
	double e_w_val;
	double e_w_sup;

	double tmp_187_val = e1_val*e1_val;
	double tmp_187_sup = e1_sup*e1_sup;

	double tmp_190_val = e2_val*e2_val;
	double tmp_190_sup = e2_sup*e2_sup;

	double tmp_191_val = tmp_187_val+tmp_190_val;
	double tmp_191_sup = tmp_187_sup+tmp_190_sup;

	double tmp_194_val = e3_val*e3_val;
	double tmp_194_sup = e3_sup*e3_sup;

	double tmp_195_val = tmp_191_val+tmp_194_val;
	double tmp_195_sup = tmp_191_sup+tmp_194_sup;

	e_w_val = tmp_195_val;
	e_w_sup = tmp_195_sup;

	// ******************* New Declaration
	double X1_val;
	double X1_sup;

	double tmp_198_val = c_x_val*b_w_val;
	double tmp_198_sup = c_x_sup*b_w_sup;

	double tmp_201_val = b_x_val*c_w_val;
	double tmp_201_sup = b_x_sup*c_w_sup;

	double tmp_202_val = tmp_198_val-tmp_201_val;
	double tmp_202_sup = tmp_198_sup+tmp_201_sup;

	X1_val = tmp_202_val;
	X1_sup = tmp_202_sup;

	// ******************* New Declaration
	double Y1_val;
	double Y1_sup;

	double tmp_205_val = c_y_val*b_w_val;
	double tmp_205_sup = c_y_sup*b_w_sup;

	double tmp_208_val = b_y_val*c_w_val;
	double tmp_208_sup = b_y_sup*c_w_sup;

	double tmp_209_val = tmp_205_val-tmp_208_val;
	double tmp_209_sup = tmp_205_sup+tmp_208_sup;

	Y1_val = tmp_209_val;
	Y1_sup = tmp_209_sup;

	// ******************* New Declaration
	double Z1_val;
	double Z1_sup;

	double tmp_212_val = c_z_val*b_w_val;
	double tmp_212_sup = c_z_sup*b_w_sup;

	double tmp_215_val = b_z_val*c_w_val;
	double tmp_215_sup = b_z_sup*c_w_sup;

	double tmp_216_val = tmp_212_val-tmp_215_val;
	double tmp_216_sup = tmp_212_sup+tmp_215_sup;

	Z1_val = tmp_216_val;
	Z1_sup = tmp_216_sup;

	// ******************* New Declaration
	double X2_val;
	double X2_sup;

	double tmp_219_val = d_x_val*b_w_val;
	double tmp_219_sup = d_x_sup*b_w_sup;

	double tmp_222_val = b_x_val*d_w_val;
	double tmp_222_sup = b_x_sup*d_w_sup;

	double tmp_223_val = tmp_219_val-tmp_222_val;
	double tmp_223_sup = tmp_219_sup+tmp_222_sup;

	X2_val = tmp_223_val;
	X2_sup = tmp_223_sup;

	// ******************* New Declaration
	double Y2_val;
	double Y2_sup;

	double tmp_226_val = d_y_val*b_w_val;
	double tmp_226_sup = d_y_sup*b_w_sup;

	double tmp_229_val = b_y_val*d_w_val;
	double tmp_229_sup = b_y_sup*d_w_sup;

	double tmp_230_val = tmp_226_val-tmp_229_val;
	double tmp_230_sup = tmp_226_sup+tmp_229_sup;

	Y2_val = tmp_230_val;
	Y2_sup = tmp_230_sup;

	// ******************* New Declaration
	double Z2_val;
	double Z2_sup;

	double tmp_233_val = d_z_val*b_w_val;
	double tmp_233_sup = d_z_sup*b_w_sup;

	double tmp_236_val = b_z_val*d_w_val;
	double tmp_236_sup = b_z_sup*d_w_sup;

	double tmp_237_val = tmp_233_val-tmp_236_val;
	double tmp_237_sup = tmp_233_sup+tmp_236_sup;

	Z2_val = tmp_237_val;
	Z2_sup = tmp_237_sup;

	// ******************* New Declaration
	double X3_val;
	double X3_sup;

	double tmp_240_val = e_x_val*b_w_val;
	double tmp_240_sup = e_x_sup*b_w_sup;

	double tmp_243_val = b_x_val*e_w_val;
	double tmp_243_sup = b_x_sup*e_w_sup;

	double tmp_244_val = tmp_240_val-tmp_243_val;
	double tmp_244_sup = tmp_240_sup+tmp_243_sup;

	X3_val = tmp_244_val;
	X3_sup = tmp_244_sup;

	// ******************* New Declaration
	double Y3_val;
	double Y3_sup;

	double tmp_247_val = e_y_val*b_w_val;
	double tmp_247_sup = e_y_sup*b_w_sup;

	double tmp_250_val = b_y_val*e_w_val;
	double tmp_250_sup = b_y_sup*e_w_sup;

	double tmp_251_val = tmp_247_val-tmp_250_val;
	double tmp_251_sup = tmp_247_sup+tmp_250_sup;

	Y3_val = tmp_251_val;
	Y3_sup = tmp_251_sup;

	// ******************* New Declaration
	double Z3_val;
	double Z3_sup;

	double tmp_254_val = e_z_val*b_w_val;
	double tmp_254_sup = e_z_sup*b_w_sup;

	double tmp_257_val = b_z_val*e_w_val;
	double tmp_257_sup = b_z_sup*e_w_sup;

	double tmp_258_val = tmp_254_val-tmp_257_val;
	double tmp_258_sup = tmp_254_sup+tmp_257_sup;

	Z3_val = tmp_258_val;
	Z3_sup = tmp_258_sup;

	// ******************* New Declaration
	double cwx_val;
	double cwx_sup;

	double tmp_261_val = Z1_val*Y2_val;
	double tmp_261_sup = Z1_sup*Y2_sup;

	double tmp_264_val = Y1_val*Z2_val;
	double tmp_264_sup = Y1_sup*Z2_sup;

	double tmp_265_val = tmp_261_val-tmp_264_val;
	double tmp_265_sup = tmp_261_sup+tmp_264_sup;

	cwx_val = tmp_265_val;
	cwx_sup = tmp_265_sup;

	// ******************* New Declaration
	double cwy_val;
	double cwy_sup;

	double tmp_268_val = X1_val*Z2_val;
	double tmp_268_sup = X1_sup*Z2_sup;

	double tmp_271_val = Z1_val*X2_val;
	double tmp_271_sup = Z1_sup*X2_sup;

	double tmp_272_val = tmp_268_val-tmp_271_val;
	double tmp_272_sup = tmp_268_sup+tmp_271_sup;

	cwy_val = tmp_272_val;
	cwy_sup = tmp_272_sup;

	// ******************* New Declaration
	double cwz_val;
	double cwz_sup;

	double tmp_275_val = Y1_val*X2_val;
	double tmp_275_sup = Y1_sup*X2_sup;

	double tmp_278_val = X1_val*Y2_val;
	double tmp_278_sup = X1_sup*Y2_sup;

	double tmp_279_val = tmp_275_val-tmp_278_val;
	double tmp_279_sup = tmp_275_sup+tmp_278_sup;

	cwz_val = tmp_279_val;
	cwz_sup = tmp_279_sup;

	// ******************* New Declaration
	double Dw_val;
	double Dw_sup;

	double tmp_282_val = cwx_val*X3_val;
	double tmp_282_sup = cwx_sup*X3_sup;

	double tmp_285_val = cwy_val*Y3_val;
	double tmp_285_sup = cwy_sup*Y3_sup;

	double tmp_286_val = tmp_282_val+tmp_285_val;
	double tmp_286_sup = tmp_282_sup+tmp_285_sup;

	double tmp_289_val = cwz_val*Z3_val;
	double tmp_289_sup = cwz_sup*Z3_sup;

	double tmp_290_val = tmp_286_val+tmp_289_val;
	double tmp_290_sup = tmp_286_sup+tmp_289_sup;

	Dw_val = tmp_290_val;
	Dw_sup = tmp_290_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 68
	{
	  static double _upper=68*EXCOMP_eps1;
	  if ( fp::abs(Dw_val) > Dw_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(Dw_val);
	  else if ( Dw_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}
if (d3_rat_point::use_filter==0 || ressign==EXCOMP_NO_IDEA || EXCOMP_FALSE)
	// ***** Exact Evaluation
	{
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
	integer b1 = ((BX*AW)-(AX*BW));
	integer b2 = ((BY*AW)-(AY*BW));
	integer b3 = ((BZ*AW)-(AZ*BW));
	integer c1 = ((CX*AW)-(AX*CW));
	integer c2 = ((CY*AW)-(AY*CW));
	integer c3 = ((CZ*AW)-(AZ*CW));
	integer d1 = ((DX*AW)-(AX*DW));
	integer d2 = ((DY*AW)-(AY*DW));
	integer d3 = ((DZ*AW)-(AZ*DW));
	integer e1 = ((EX*AW)-(AX*EW));
	integer e2 = ((EY*AW)-(AY*EW));
	integer e3 = ((EZ*AW)-(AZ*EW));
	integer wert1 = (AW*BW);
	integer b_x = (b1*wert1);
	integer b_y = (b2*wert1);
	integer b_z = (b3*wert1);
	integer b_w = (((b1*b1)+(b2*b2))+(b3*b3));
	integer wert2 = (AW*CW);
	integer c_x = (c1*wert2);
	integer c_y = (c2*wert2);
	integer c_z = (c3*wert2);
	integer c_w = (((c1*c1)+(c2*c2))+(c3*c3));
	integer wert3 = (AW*DW);
	integer d_x = (d1*wert3);
	integer d_y = (d2*wert3);
	integer d_z = (d3*wert3);
	integer d_w = (((d1*d1)+(d2*d2))+(d3*d3));
	integer wert4 = (AW*EW);
	integer e_x = (e1*wert4);
	integer e_y = (e2*wert4);
	integer e_z = (e3*wert4);
	integer e_w = (((e1*e1)+(e2*e2))+(e3*e3));
	integer X1 = ((c_x*b_w)-(b_x*c_w));
	integer Y1 = ((c_y*b_w)-(b_y*c_w));
	integer Z1 = ((c_z*b_w)-(b_z*c_w));
	integer X2 = ((d_x*b_w)-(b_x*d_w));
	integer Y2 = ((d_y*b_w)-(b_y*d_w));
	integer Z2 = ((d_z*b_w)-(b_z*d_w));
	integer X3 = ((e_x*b_w)-(b_x*e_w));
	integer Y3 = ((e_y*b_w)-(b_y*e_w));
	integer Z3 = ((e_z*b_w)-(b_z*e_w));
	integer cwx = ((Z1*Y2)-(Y1*Z2));
	integer cwy = ((X1*Z2)-(Z1*X2));
	integer cwz = ((Y1*X2)-(X1*Y2));
	integer Dw = (((cwx*X3)+(cwy*Y3))+(cwz*Z3));
	ressign = sign(Dw);
	}
// ******** End Expression Compiler Output
 
 return -ressign;

}

#endif


#if 0

int side_of_sphere(const d3_rat_point& a,
                   const d3_rat_point& b,
                   const d3_rat_point& c,
                   const d3_rat_point& d,
                   const d3_rat_point& e)
{ 
  d3_rat_point::sos_count++;
  
  int ressign=0;
 

if (d3_rat_point::use_filter != 0)
{

double AX_val = a.XD();
double AY_val = a.YD();
double AZ_val = a.ZD();
double AW_val = a.WD();
double BX_val = b.XD();
double BY_val = b.YD();
double BZ_val = b.ZD();
double BW_val = b.WD();
double CX_val = c.XD();
double CY_val = c.YD();
double CZ_val = c.ZD();
double CW_val = c.WD();
double DX_val = d.XD();
double DY_val = d.YD();
double DZ_val = d.ZD();
double DW_val = d.WD();
double EX_val = e.XD();
double EY_val = e.YD();
double EZ_val = e.ZD();
double EW_val = e.WD();

double AX_sup = fp::abs(AX_val);
double AY_sup = fp::abs(AY_val);
double AZ_sup = fp::abs(AZ_val);
double AW_sup = fp::abs(AW_val);
double BX_sup = fp::abs(BX_val);
double BY_sup = fp::abs(BY_val);
double BZ_sup = fp::abs(BZ_val);
double BW_sup = fp::abs(BW_val);
double CX_sup = fp::abs(CX_val);
double CY_sup = fp::abs(CY_val);
double CZ_sup = fp::abs(CZ_val);
double CW_sup = fp::abs(CW_val);
double DX_sup = fp::abs(DX_val);
double DY_sup = fp::abs(DY_val);
double DZ_sup = fp::abs(DZ_val);
double DW_sup = fp::abs(DW_val);
double EX_sup = fp::abs(EX_val);
double EY_sup = fp::abs(EY_val);
double EZ_sup = fp::abs(EZ_val);
double EW_sup = fp::abs(EW_val);


	double tmp_22_val = BX_val*AW_val;
	double tmp_22_sup = BX_sup*AW_sup;

	double tmp_25_val = AX_val*BW_val;
	double tmp_25_sup = AX_sup*BW_sup;

	double tmp_26_val = tmp_22_val-tmp_25_val;
	double tmp_26_sup = tmp_22_sup+tmp_25_sup;

	double b1_val = tmp_26_val;
	double b1_sup = tmp_26_sup;

	double tmp_29_val = BY_val*AW_val;
	double tmp_29_sup = BY_sup*AW_sup;

	double tmp_32_val = AY_val*BW_val;
	double tmp_32_sup = AY_sup*BW_sup;

	double tmp_33_val = tmp_29_val-tmp_32_val;
	double tmp_33_sup = tmp_29_sup+tmp_32_sup;

	double b2_val = tmp_33_val;
	double b2_sup = tmp_33_sup;


	double tmp_36_val = BZ_val*AW_val;
	double tmp_36_sup = BZ_sup*AW_sup;

	double tmp_39_val = AZ_val*BW_val;
	double tmp_39_sup = AZ_sup*BW_sup;

	double tmp_40_val = tmp_36_val-tmp_39_val;
	double tmp_40_sup = tmp_36_sup+tmp_39_sup;

	double b3_val = tmp_40_val;
	double b3_sup = tmp_40_sup;


	double tmp_43_val = CX_val*AW_val;
	double tmp_43_sup = CX_sup*AW_sup;

	double tmp_46_val = AX_val*CW_val;
	double tmp_46_sup = AX_sup*CW_sup;

	double tmp_47_val = tmp_43_val-tmp_46_val;
	double tmp_47_sup = tmp_43_sup+tmp_46_sup;

	double c1_val = tmp_47_val;
	double c1_sup = tmp_47_sup;


	double tmp_50_val = CY_val*AW_val;
	double tmp_50_sup = CY_sup*AW_sup;

	double tmp_53_val = AY_val*CW_val;
	double tmp_53_sup = AY_sup*CW_sup;

	double tmp_54_val = tmp_50_val-tmp_53_val;
	double tmp_54_sup = tmp_50_sup+tmp_53_sup;

	double c2_val = tmp_54_val;
	double c2_sup = tmp_54_sup;


	double tmp_57_val = CZ_val*AW_val;
	double tmp_57_sup = CZ_sup*AW_sup;

	double tmp_60_val = AZ_val*CW_val;
	double tmp_60_sup = AZ_sup*CW_sup;

	double tmp_61_val = tmp_57_val-tmp_60_val;
	double tmp_61_sup = tmp_57_sup+tmp_60_sup;

	double c3_val = tmp_61_val;
	double c3_sup = tmp_61_sup;


	double tmp_64_val = DX_val*AW_val;
	double tmp_64_sup = DX_sup*AW_sup;

	double tmp_67_val = AX_val*DW_val;
	double tmp_67_sup = AX_sup*DW_sup;

	double tmp_68_val = tmp_64_val-tmp_67_val;
	double tmp_68_sup = tmp_64_sup+tmp_67_sup;

	double d1_val = tmp_68_val;
	double d1_sup = tmp_68_sup;


	double tmp_71_val = DY_val*AW_val;
	double tmp_71_sup = DY_sup*AW_sup;

	double tmp_74_val = AY_val*DW_val;
	double tmp_74_sup = AY_sup*DW_sup;

	double tmp_75_val = tmp_71_val-tmp_74_val;
	double tmp_75_sup = tmp_71_sup+tmp_74_sup;

	double d2_val = tmp_75_val;
	double d2_sup = tmp_75_sup;


	double tmp_78_val = DZ_val*AW_val;
	double tmp_78_sup = DZ_sup*AW_sup;

	double tmp_81_val = AZ_val*DW_val;
	double tmp_81_sup = AZ_sup*DW_sup;

	double tmp_82_val = tmp_78_val-tmp_81_val;
	double tmp_82_sup = tmp_78_sup+tmp_81_sup;

	double d3_val = tmp_82_val;
	double d3_sup = tmp_82_sup;


	double tmp_85_val = EX_val*AW_val;
	double tmp_85_sup = EX_sup*AW_sup;

	double tmp_88_val = AX_val*EW_val;
	double tmp_88_sup = AX_sup*EW_sup;

	double tmp_89_val = tmp_85_val-tmp_88_val;
	double tmp_89_sup = tmp_85_sup+tmp_88_sup;

	double e1_val = tmp_89_val;
	double e1_sup = tmp_89_sup;


	double tmp_92_val = EY_val*AW_val;
	double tmp_92_sup = EY_sup*AW_sup;

	double tmp_95_val = AY_val*EW_val;
	double tmp_95_sup = AY_sup*EW_sup;

	double tmp_96_val = tmp_92_val-tmp_95_val;
	double tmp_96_sup = tmp_92_sup+tmp_95_sup;

	double e2_val = tmp_96_val;
	double e2_sup = tmp_96_sup;


	double tmp_99_val = EZ_val*AW_val;
	double tmp_99_sup = EZ_sup*AW_sup;

	double tmp_102_val = AZ_val*EW_val;
	double tmp_102_sup = AZ_sup*EW_sup;

	double tmp_103_val = tmp_99_val-tmp_102_val;
	double tmp_103_sup = tmp_99_sup+tmp_102_sup;

	double e3_val = tmp_103_val;
	double e3_sup = tmp_103_sup;


	double tmp_106_val = AW_val*BW_val;
	double tmp_106_sup = AW_sup*BW_sup;

	double wert1_val = tmp_106_val;
	double wert1_sup = tmp_106_sup;


	double tmp_109_val = b1_val*wert1_val;
	double tmp_109_sup = b1_sup*wert1_sup;

	double b_x_val = tmp_109_val;
	double b_x_sup = tmp_109_sup;


	double tmp_112_val = b2_val*wert1_val;
	double tmp_112_sup = b2_sup*wert1_sup;

	double b_y_val = tmp_112_val;
	double b_y_sup = tmp_112_sup;


	double tmp_115_val = b3_val*wert1_val;
	double tmp_115_sup = b3_sup*wert1_sup;

	double b_z_val = tmp_115_val;
	double b_z_sup = tmp_115_sup;


	double tmp_118_val = b1_val*b1_val;
	double tmp_118_sup = b1_sup*b1_sup;

	double tmp_121_val = b2_val*b2_val;
	double tmp_121_sup = b2_sup*b2_sup;

	double tmp_122_val = tmp_118_val+tmp_121_val;
	double tmp_122_sup = tmp_118_sup+tmp_121_sup;

	double tmp_125_val = b3_val*b3_val;
	double tmp_125_sup = b3_sup*b3_sup;

	double tmp_126_val = tmp_122_val+tmp_125_val;
	double tmp_126_sup = tmp_122_sup+tmp_125_sup;

	double b_w_val = tmp_126_val;
	double b_w_sup = tmp_126_sup;


	double tmp_129_val = AW_val*CW_val;
	double tmp_129_sup = AW_sup*CW_sup;

	double wert2_val = tmp_129_val;
	double wert2_sup = tmp_129_sup;


	double tmp_132_val = c1_val*wert2_val;
	double tmp_132_sup = c1_sup*wert2_sup;

	double c_x_val = tmp_132_val;
	double c_x_sup = tmp_132_sup;


	double tmp_135_val = c2_val*wert2_val;
	double tmp_135_sup = c2_sup*wert2_sup;

	double c_y_val = tmp_135_val;
	double c_y_sup = tmp_135_sup;


	double tmp_138_val = c3_val*wert2_val;
	double tmp_138_sup = c3_sup*wert2_sup;

	double c_z_val = tmp_138_val;
	double c_z_sup = tmp_138_sup;


	double tmp_141_val = c1_val*c1_val;
	double tmp_141_sup = c1_sup*c1_sup;

	double tmp_144_val = c2_val*c2_val;
	double tmp_144_sup = c2_sup*c2_sup;

	double tmp_145_val = tmp_141_val+tmp_144_val;
	double tmp_145_sup = tmp_141_sup+tmp_144_sup;

	double tmp_148_val = c3_val*c3_val;
	double tmp_148_sup = c3_sup*c3_sup;

	double tmp_149_val = tmp_145_val+tmp_148_val;
	double tmp_149_sup = tmp_145_sup+tmp_148_sup;

	double c_w_val = tmp_149_val;
	double c_w_sup = tmp_149_sup;


	double tmp_152_val = AW_val*DW_val;
	double tmp_152_sup = AW_sup*DW_sup;

	double wert3_val = tmp_152_val;
	double wert3_sup = tmp_152_sup;


	double tmp_155_val = d1_val*wert3_val;
	double tmp_155_sup = d1_sup*wert3_sup;

	double d_x_val = tmp_155_val;
	double d_x_sup = tmp_155_sup;


	double tmp_158_val = d2_val*wert3_val;
	double tmp_158_sup = d2_sup*wert3_sup;

	double d_y_val = tmp_158_val;
	double d_y_sup = tmp_158_sup;


	double tmp_161_val = d3_val*wert3_val;
	double tmp_161_sup = d3_sup*wert3_sup;

	double d_z_val = tmp_161_val;
	double d_z_sup = tmp_161_sup;


	double tmp_164_val = d1_val*d1_val;
	double tmp_164_sup = d1_sup*d1_sup;

	double tmp_167_val = d2_val*d2_val;
	double tmp_167_sup = d2_sup*d2_sup;

	double tmp_168_val = tmp_164_val+tmp_167_val;
	double tmp_168_sup = tmp_164_sup+tmp_167_sup;

	double tmp_171_val = d3_val*d3_val;
	double tmp_171_sup = d3_sup*d3_sup;

	double tmp_172_val = tmp_168_val+tmp_171_val;
	double tmp_172_sup = tmp_168_sup+tmp_171_sup;

	double d_w_val = tmp_172_val;
	double d_w_sup = tmp_172_sup;


	double tmp_175_val = AW_val*EW_val;
	double tmp_175_sup = AW_sup*EW_sup;

	double wert4_val = tmp_175_val;
	double wert4_sup = tmp_175_sup;


	double tmp_178_val = e1_val*wert4_val;
	double tmp_178_sup = e1_sup*wert4_sup;

	double e_x_val = tmp_178_val;
	double e_x_sup = tmp_178_sup;


	double tmp_181_val = e2_val*wert4_val;
	double tmp_181_sup = e2_sup*wert4_sup;

	double e_y_val = tmp_181_val;
	double e_y_sup = tmp_181_sup;


	double tmp_184_val = e3_val*wert4_val;
	double tmp_184_sup = e3_sup*wert4_sup;

	double e_z_val = tmp_184_val;
	double e_z_sup = tmp_184_sup;


	double tmp_187_val = e1_val*e1_val;
	double tmp_187_sup = e1_sup*e1_sup;

	double tmp_190_val = e2_val*e2_val;
	double tmp_190_sup = e2_sup*e2_sup;

	double tmp_191_val = tmp_187_val+tmp_190_val;
	double tmp_191_sup = tmp_187_sup+tmp_190_sup;

	double tmp_194_val = e3_val*e3_val;
	double tmp_194_sup = e3_sup*e3_sup;

	double tmp_195_val = tmp_191_val+tmp_194_val;
	double tmp_195_sup = tmp_191_sup+tmp_194_sup;

	double e_w_val = tmp_195_val;
	double e_w_sup = tmp_195_sup;


	double tmp_198_val = c_x_val*b_w_val;
	double tmp_198_sup = c_x_sup*b_w_sup;

	double tmp_201_val = b_x_val*c_w_val;
	double tmp_201_sup = b_x_sup*c_w_sup;

	double tmp_202_val = tmp_198_val-tmp_201_val;
	double tmp_202_sup = tmp_198_sup+tmp_201_sup;

	double X1_val = tmp_202_val;
	double X1_sup = tmp_202_sup;


	double tmp_205_val = c_y_val*b_w_val;
	double tmp_205_sup = c_y_sup*b_w_sup;

	double tmp_208_val = b_y_val*c_w_val;
	double tmp_208_sup = b_y_sup*c_w_sup;

	double tmp_209_val = tmp_205_val-tmp_208_val;
	double tmp_209_sup = tmp_205_sup+tmp_208_sup;

	double Y1_val = tmp_209_val;
	double Y1_sup = tmp_209_sup;


	double tmp_212_val = c_z_val*b_w_val;
	double tmp_212_sup = c_z_sup*b_w_sup;

	double tmp_215_val = b_z_val*c_w_val;
	double tmp_215_sup = b_z_sup*c_w_sup;

	double tmp_216_val = tmp_212_val-tmp_215_val;
	double tmp_216_sup = tmp_212_sup+tmp_215_sup;

	double Z1_val = tmp_216_val;
	double Z1_sup = tmp_216_sup;


	double tmp_219_val = d_x_val*b_w_val;
	double tmp_219_sup = d_x_sup*b_w_sup;

	double tmp_222_val = b_x_val*d_w_val;
	double tmp_222_sup = b_x_sup*d_w_sup;

	double tmp_223_val = tmp_219_val-tmp_222_val;
	double tmp_223_sup = tmp_219_sup+tmp_222_sup;

	double X2_val = tmp_223_val;
	double X2_sup = tmp_223_sup;


	double tmp_226_val = d_y_val*b_w_val;
	double tmp_226_sup = d_y_sup*b_w_sup;

	double tmp_229_val = b_y_val*d_w_val;
	double tmp_229_sup = b_y_sup*d_w_sup;

	double tmp_230_val = tmp_226_val-tmp_229_val;
	double tmp_230_sup = tmp_226_sup+tmp_229_sup;

	double Y2_val = tmp_230_val;
	double Y2_sup = tmp_230_sup;


	double tmp_233_val = d_z_val*b_w_val;
	double tmp_233_sup = d_z_sup*b_w_sup;

	double tmp_236_val = b_z_val*d_w_val;
	double tmp_236_sup = b_z_sup*d_w_sup;

	double tmp_237_val = tmp_233_val-tmp_236_val;
	double tmp_237_sup = tmp_233_sup+tmp_236_sup;

	double Z2_val = tmp_237_val;
	double Z2_sup = tmp_237_sup;


	double tmp_240_val = e_x_val*b_w_val;
	double tmp_240_sup = e_x_sup*b_w_sup;

	double tmp_243_val = b_x_val*e_w_val;
	double tmp_243_sup = b_x_sup*e_w_sup;

	double tmp_244_val = tmp_240_val-tmp_243_val;
	double tmp_244_sup = tmp_240_sup+tmp_243_sup;

	double X3_val = tmp_244_val;
	double X3_sup = tmp_244_sup;


	double tmp_247_val = e_y_val*b_w_val;
	double tmp_247_sup = e_y_sup*b_w_sup;

	double tmp_250_val = b_y_val*e_w_val;
	double tmp_250_sup = b_y_sup*e_w_sup;

	double tmp_251_val = tmp_247_val-tmp_250_val;
	double tmp_251_sup = tmp_247_sup+tmp_250_sup;

	double Y3_val = tmp_251_val;
	double Y3_sup = tmp_251_sup;


	double tmp_254_val = e_z_val*b_w_val;
	double tmp_254_sup = e_z_sup*b_w_sup;

	double tmp_257_val = b_z_val*e_w_val;
	double tmp_257_sup = b_z_sup*e_w_sup;

	double tmp_258_val = tmp_254_val-tmp_257_val;
	double tmp_258_sup = tmp_254_sup+tmp_257_sup;

	double Z3_val = tmp_258_val;
	double Z3_sup = tmp_258_sup;


	double tmp_261_val = Z1_val*Y2_val;
	double tmp_261_sup = Z1_sup*Y2_sup;

	double tmp_264_val = Y1_val*Z2_val;
	double tmp_264_sup = Y1_sup*Z2_sup;

	double tmp_265_val = tmp_261_val-tmp_264_val;
	double tmp_265_sup = tmp_261_sup+tmp_264_sup;

	double cwx_val = tmp_265_val;
	double cwx_sup = tmp_265_sup;


	double tmp_268_val = X1_val*Z2_val;
	double tmp_268_sup = X1_sup*Z2_sup;

	double tmp_271_val = Z1_val*X2_val;
	double tmp_271_sup = Z1_sup*X2_sup;

	double tmp_272_val = tmp_268_val-tmp_271_val;
	double tmp_272_sup = tmp_268_sup+tmp_271_sup;

	double cwy_val = tmp_272_val;
	double cwy_sup = tmp_272_sup;


	double tmp_275_val = Y1_val*X2_val;
	double tmp_275_sup = Y1_sup*X2_sup;

	double tmp_278_val = X1_val*Y2_val;
	double tmp_278_sup = X1_sup*Y2_sup;

	double tmp_279_val = tmp_275_val-tmp_278_val;
	double tmp_279_sup = tmp_275_sup+tmp_278_sup;

	double cwz_val = tmp_279_val;
	double cwz_sup = tmp_279_sup;

	double tmp_282_val = cwx_val*X3_val;
	double tmp_282_sup = cwx_sup*X3_sup;

	double tmp_285_val = cwy_val*Y3_val;
	double tmp_285_sup = cwy_sup*Y3_sup;

	double tmp_286_val = tmp_282_val+tmp_285_val;
	double tmp_286_sup = tmp_282_sup+tmp_285_sup;

	double tmp_289_val = cwz_val*Z3_val;
	double tmp_289_sup = cwz_sup*Z3_sup;

	double tmp_290_val = tmp_286_val+tmp_289_val;
	double tmp_290_sup = tmp_286_sup+tmp_289_sup;

	double Dw_val = tmp_290_val;
	double Dw_sup = tmp_290_sup;

	// ***************** Sign Statement
	// ******* BitLength of Expression is 0
	// ******* Index of Expression is 68
	{
	  static double _upper=68*EXCOMP_eps1;
	  if ( fp::abs(Dw_val) > Dw_sup*_upper*EXCOMP_correction )
		ressign = fp::sign(Dw_val);
	  else if ( Dw_sup*_upper*EXCOMP_correction<1)
		ressign = 0;
	  else
		ressign = EXCOMP_NO_IDEA;
	}
}

   if (d3_rat_point::use_filter==0 || ressign==EXCOMP_NO_IDEA || EXCOMP_FALSE)
   { 
     // ***** Exact Evaluation

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
	integer b1 = ((BX*AW)-(AX*BW));
	integer b2 = ((BY*AW)-(AY*BW));
	integer b3 = ((BZ*AW)-(AZ*BW));
	integer c1 = ((CX*AW)-(AX*CW));
	integer c2 = ((CY*AW)-(AY*CW));
	integer c3 = ((CZ*AW)-(AZ*CW));
	integer d1 = ((DX*AW)-(AX*DW));
	integer d2 = ((DY*AW)-(AY*DW));
	integer d3 = ((DZ*AW)-(AZ*DW));
	integer e1 = ((EX*AW)-(AX*EW));
	integer e2 = ((EY*AW)-(AY*EW));
	integer e3 = ((EZ*AW)-(AZ*EW));
	integer wert1 = (AW*BW);
	integer b_x = (b1*wert1);
	integer b_y = (b2*wert1);
	integer b_z = (b3*wert1);
	integer b_w = (((b1*b1)+(b2*b2))+(b3*b3));
	integer wert2 = (AW*CW);
	integer c_x = (c1*wert2);
	integer c_y = (c2*wert2);
	integer c_z = (c3*wert2);
	integer c_w = (((c1*c1)+(c2*c2))+(c3*c3));
	integer wert3 = (AW*DW);
	integer d_x = (d1*wert3);
	integer d_y = (d2*wert3);
	integer d_z = (d3*wert3);
	integer d_w = (((d1*d1)+(d2*d2))+(d3*d3));
	integer wert4 = (AW*EW);
	integer e_x = (e1*wert4);
	integer e_y = (e2*wert4);
	integer e_z = (e3*wert4);
	integer e_w = (((e1*e1)+(e2*e2))+(e3*e3));
	integer X1 = ((c_x*b_w)-(b_x*c_w));
	integer Y1 = ((c_y*b_w)-(b_y*c_w));
	integer Z1 = ((c_z*b_w)-(b_z*c_w));
	integer X2 = ((d_x*b_w)-(b_x*d_w));
	integer Y2 = ((d_y*b_w)-(b_y*d_w));
	integer Z2 = ((d_z*b_w)-(b_z*d_w));
	integer X3 = ((e_x*b_w)-(b_x*e_w));
	integer Y3 = ((e_y*b_w)-(b_y*e_w));
	integer Z3 = ((e_z*b_w)-(b_z*e_w));
	integer cwx = ((Z1*Y2)-(Y1*Z2));
	integer cwy = ((X1*Z2)-(Z1*X2));
	integer cwz = ((Y1*X2)-(X1*Y2));
	integer Dw = (((cwx*X3)+(cwy*Y3))+(cwz*Z3));
	ressign = sign(Dw);
	}
 
 return -ressign;

}
#endif

LEDA_END_NAMESPACE
