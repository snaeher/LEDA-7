/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _fp.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/fp.h>

LEDA_BEGIN_NAMESPACE

const double fp::min=compose_parts(0,1,0,0);
const double fp::NaN=compose_parts(0,2047,1<<19,1);
  // this is a quiet = non-signalling NaN because 
  // the highest bit of the fraction (the quiet bit) is set
const double fp::pInf=compose_parts(0,2047,0,0);
const double fp::nInf=-fp::pInf;
const double fp::pZero=compose_parts(0,0,0,0);
const double fp::nZero=compose_parts(1,0,0,0);



double fp::dbl_add(double x, double y) { return x+y; }
double fp::dbl_sub(double x, double y) { return x-y; }
double fp::dbl_mul(double x, double y) { return x*y; }
double fp::dbl_div(double x, double y) { return x/y; }



int fp::sign(VOLATILE_FP double x)
{
    // Commented out by CU on Oct 19 2020: does not work for big doubles in numbers package
    /* 
  double_shape p;
  p.value = x;
#ifndef WORD_LENGTH_64
    if (p.parts.msw & exponent_mask) // if is_normalized(x)
  {
     if (p.parts.msw & signbit_mask) // if sign_bit(x)
#else
  p.w = (LEDA_UINT)x;
  if(p.w & exponent_mask) // if is_normalized(x)
  {
     if (p.w & signbit_mask) // if sign_bit(x)
#endif 
        return -1;
     else
        return 1;
  }
  */
  if (x == 0)
     return 0;
  if (x > 0) return 1;
  return -1;
}




LEDA_UINT fp::get_biased_exponent(VOLATILE_FP double x)
{
  double_shape p;
  p.value = x;
#ifndef WORD_LENGTH_64
  return (p.parts.msw & exponent_mask) >> 20;
#else
  return (p.w & exponent_mask) >> 52;
#endif
}


LEDA_UINT fp::get_unbiased_exponent(VOLATILE_FP double x)
{
  double_shape p;
  p.value = x;
#ifndef WORD_LENGTH_64
  LEDA_UINT biased_exponent =
     (p.parts.msw & exponent_mask) >> 20;
#else
  LEDA_UINT biased_exponent =
     (p.w & exponent_mask) >> 52;
#endif
  return biased_exponent-1023;
}


void fp::set_unbiased_exponent(VOLATILE_FP_REF double& x, int exp)
{
  double_shape p;
  p.value = x;

#ifndef WORD_LENGTH_64
  p.parts.msw &= ~exponent_mask;
  p.parts.msw |= ((exp+1023)<<20);
#else
  p.w &= ~exponent_mask;
  p.w |= ((LEDA_UINT(exp)+1023)<<52);
#endif
  x = p.value;
}



double fp::power_two(long L)
{
  double_shape p;
  p.value = 0;
#ifndef WORD_LENGTH_64
  p.parts.msw = L + 1023;
  p.parts.msw <<= 20;
  p.parts.msw &= exponent_mask;

#else
  p.w = L + 1023;
  p.w <<= 52;
  p.w &= exponent_mask;
#endif
  return p.value;
}

#ifndef WORD_LENGTH_64


double fp::compose_parts
(int sign_1,
 LEDA_UINT exp_11,
 LEDA_UINT most_sig_20, 
 LEDA_UINT least_sig_32)
{
  double_shape p;
  p.parts.msw = 0;
  if (sign_1) 
    p.parts.msw |= signbit_mask;
  p.parts.msw |= (exp_11 << 20);
  p.parts.msw |= most_sig_20;
  
  p.parts.lsw = least_sig_32;

  return p.value;
}


void fp::read_parts(
  VOLATILE_FP double a,
  int& sign_1, 
  LEDA_UINT& exp_11,
  LEDA_UINT& most_sig_20, 
  LEDA_UINT& least_sig_32)
{
  double_shape p;
  p.value = a;

  least_sig_32 = p.parts.lsw;

  sign_1 = (int) (p.parts.msw & signbit_mask) >> 31;
  exp_11 = (p.parts.msw & exponent_mask) >> 20;
  most_sig_20 = (p.parts.msw & 0x000fffff);
}


LEDA_UINT fp::binary_equal(VOLATILE_FP double x, VOLATILE_FP double y)
{
  double_shape p,q;
  p.value = x;
  q.value = y;
  if (p.parts.msw != q.parts.msw)
    return 0;
  
  if (p.parts.lsw != q.parts.lsw)
    return 0;
  return 1;
}

#else /* WORD_LENGTH_64 */


double fp::compose_parts(
 int sign_1, 
 LEDA_UINT exp_11,
 LEDA_UINT most_sig_20, 
 LEDA_UINT least_sig_32)
{
  double_shape p;

  LEDA_UINT word_64 = (exp_11 << 20) + most_sig_20;
  word_64 = (word_64 << 32) + least_sig_32;
  if (sign_1) 
    word_64 |= signbit_mask;
  
  p.w = word_64;

  return p.value;
}


void fp::read_parts(
 VOLATILE_FP double a,
 int& sign_1, 
 LEDA_UINT& exp_11,
 LEDA_UINT& most_sig_20,
 LEDA_UINT& least_sig_32)
{
  double_shape p;
  p.value = a;
  sign_1 = (int) (p.w >> 63);
  exp_11 = (p.w & exponent_mask) >> 52;
  most_sig_20  = (p.w & 0x000fffff00000000) >> 32;
  least_sig_32 = p.w & 0x00000000ffffffff;
}

LEDA_UINT fp::binary_equal(VOLATILE_FP double x, VOLATILE_FP double y)
{
  double_shape p,q;
  p.value = x;
  q.value = y;
  return (p.w == q.w);
}

#endif // #else of #ifndef WORD_LENGTH_64



#define fp_check(X) (check = check && X)

static LEDA_UINT fp_check_pZero(void)
{
   LEDA_UINT check;
   check = (fp::pZero == 0);
   fp_check(!fp::sign_bit_set(fp::pZero));
   fp_check(fp::is_finite(fp::pZero));
   fp_check(!fp::is_infinite(fp::pZero));
   fp_check(!fp::is_nan(fp::pZero));
   fp_check(fp::abs(fp::pZero)==0);
   fp_check(fp::get_biased_exponent(fp::pZero)==0);
   fp_check(fp::binary_equal(fp::pZero,double(+0)));
   return check;
}
   
static LEDA_UINT fp_check_nZero(void)
{
   LEDA_UINT check = (fp::nZero == 0);
   fp_check(fp::sign_bit_set(fp::nZero));
   fp_check(fp::is_finite(fp::nZero));
   fp_check(!fp::is_infinite(fp::nZero));
   fp_check(!fp::is_nan(fp::nZero));
   fp_check(fp::abs(fp::nZero)==fp::pZero);
   fp_check(fp::get_biased_exponent(fp::nZero)==0);
   fp_check(fp::sign(fp::nZero)==0);
   // fp_check(fp::binary_equal(fp::nZero,double(-0)));
   // problems on machines without -0 support
   return check;
}

static LEDA_UINT fp_check_pInf(void)
{
   LEDA_UINT check = (!fp::sign_bit_set(fp::pInf));
   fp_check(!fp::is_finite(fp::pInf));
   fp_check(fp::is_infinite(fp::pInf));
   fp_check(!fp::is_nan(fp::pInf));
   fp_check(fp::abs(fp::pInf)==fp::pInf);
   fp_check(fp::get_biased_exponent(fp::pInf)==2047);
   fp_check(fp::sign(fp::pInf)==1);
   return check;
}

static LEDA_UINT fp_check_nInf(void)
{
   LEDA_UINT check = (fp::sign_bit_set(fp::nInf));
   fp_check(!fp::is_finite(fp::nInf));
   fp_check(fp::is_infinite(fp::nInf));
   fp_check(!fp::is_nan(fp::nInf));
   fp_check(fp::abs(fp::nInf)==fp::pInf);
   fp_check(fp::get_biased_exponent(fp::nInf)==2047);
   fp_check(fp::sign(fp::nInf)==-1);
   return check;
}
 
static LEDA_UINT fp_check_NaN(void)
{
   LEDA_UINT check = (!fp::is_finite(fp::NaN));
   fp_check(fp::is_infinite(fp::NaN));
   fp_check(fp::is_nan(fp::NaN));
   fp_check(fp::get_biased_exponent(fp::nInf)==2047);
   return check;
}

static LEDA_UINT fp_check_power_two(void)
{
   LEDA_UINT check = (fp::power_two(13) == (1 << 13));
   fp_check(fp::min == fp::power_two(-1022));
   return check;
}
   
static LEDA_UINT fp_check_clear_sign_bit(void)
{
   double Inf = fp::nInf;
   fp::clear_sign_bit(Inf);
   LEDA_UINT check = (!fp::sign_bit_set(Inf));
   double Zero = fp::nZero;
   fp::clear_sign_bit(Zero);
   fp_check(!fp::sign_bit_set(Zero));
   return check;
}

static LEDA_UINT fp_check_is_finite(void)
{
   double big = fp::power_two(1023);
   LEDA_UINT check = (fp::is_finite(big));
   fp_check (!fp::is_infinite(big));
   big = fp::power_two(1024);
   // caution: do not cause an overflow trap 
   fp_check (!fp::is_finite(big));
   fp_check (fp::is_infinite(big));
   return check;
}


static LEDA_UINT fp_check_is_nan(void)
{
   LEDA_UINT check = (fp::is_nan(fp::NaN));
   fp_check(!fp::is_nan(fp::nInf));
   return 1;
}

static LEDA_UINT fp_check_binary_equal(void)
{
   LEDA_UINT check = (fp::binary_equal(fp::NaN,fp::NaN));
   fp_check(!fp::binary_equal(fp::nZero,fp::pZero));
   // fp_check(fp::binary_equal(-1.0/fp::nZero,fp::pInf));
   // caution, do not rely on zero arithmetic 
   return check;
}

static LEDA_UINT fp_check_abs(void)
{
   LEDA_UINT check = (fp::abs(-7) == 7);
   fp_check(fp::binary_equal(fp::abs(fp::nZero),fp::pZero));
   fp_check(fp::binary_equal(fp::abs(fp::nInf),fp::pInf));
   return check;
}

static LEDA_UINT fp_check_sign(void)
{
   LEDA_UINT check = (fp::sign(fp::nZero) == 0);
   fp_check(fp::sign(fp::nInf)  == -1);
   fp_check(fp::sign(5/3.0) == 1);
   fp_check(fp::sign(-5/3.0) == -1);
   fp_check(fp::sign(-fp::min)==-1);
   fp_check(fp::sign(double(0))==0);
   if (fp::min/2 != 0) 
     fp_check(fp::sign(-fp::min/2)==-1);
   return check;
}


// 1 sign bit                       : sign_1
// 11 bits of the exponent          : exponent_11
// higher 20 bit of the significant : most_sig_20
// lower  32 bit of the significant : least_sig_32

static LEDA_UINT fp_check_compose_read_parts(void)
{
   double test = 5/3;
   int sign_1;
   LEDA_UINT exp_11;
   LEDA_UINT most_sig_20;
   LEDA_UINT least_sig_32;
   fp::read_parts(test,sign_1,exp_11,most_sig_20,least_sig_32);
   LEDA_UINT check = (test == 
     fp::compose_parts(sign_1,exp_11,most_sig_20,least_sig_32));
   test = -5/7;
   fp::read_parts(test,sign_1,exp_11,most_sig_20,least_sig_32);
   fp_check(test == 
   fp::compose_parts(sign_1,exp_11,most_sig_20,least_sig_32));
   return check;
}

static LEDA_UINT fp_check_get_exponent(void)
{
   LEDA_UINT check = (fp::get_biased_exponent(fp::NaN)==2047);
   fp_check(fp::get_unbiased_exponent(fp::NaN)==1024);
   return check;
}

static LEDA_UINT fp_check_is_not_denormal(void)
{
   LEDA_UINT check = (fp::is_not_denormal(fp::min));
   double small = fp::compose_parts(1,0,0,1);
   fp_check (!fp::is_not_denormal(small));
   return check;
}

static LEDA_UINT fp_check_is_single_precision(void)
{
   double x = (1UL<<24)-1;
   LEDA_UINT check = (fp::is_single_precision(x));
   x = (1UL<<25)-1;
   fp_check (!fp::is_single_precision(x));
   return check;
}

static LEDA_UINT fp_selftest(void)
{
   LEDA_UINT check = (fp_check_pZero());
   fp_check(fp_check_nZero());
   fp_check(fp_check_pInf());
   fp_check(fp_check_nInf());
   fp_check(fp_check_NaN());
   fp_check(fp_check_power_two());
   fp_check(fp_check_clear_sign_bit());
   fp_check(fp_check_is_finite());
   fp_check(fp_check_is_nan());
   fp_check(fp_check_binary_equal());
   fp_check(fp_check_abs());
   fp_check(fp_check_sign());
   fp_check(fp_check_compose_read_parts());
   fp_check(fp_check_get_exponent());
   fp_check(fp_check_is_not_denormal());
   fp_check(fp_check_is_single_precision());

   return check;
}

LEDA_UINT fp::selftest_okay(void)
{
  static LEDA_UINT okay=fp_selftest();
  // Note that the check needs to be done only once 
  return okay;
}



enum h_type { B_ENDIAN=0, L_ENDIAN=1, LENGTH_64 = 2};


static h_type check_status()
{
   unsigned int size = sizeof(double);

#ifndef WORD_LENGTH_64
   if (size == sizeof(LEDA_UINT))
   { string msg = "Error in compilation of fp.c: use flag -DWORD_LENGTH_64";
     LEDA_EXCEPTION(1, msg);
    }
   double x = 1;
   double_shape p;
   p.value = x;
#ifndef LITTLE_ENDIAN_MACHINE
   if (p.parts.lsw)
   { string msg = 
        "Error in compilation of fp.c: use flag -DLITTLE_ENDIAN_MACHINE";
     LEDA_EXCEPTION(1, msg);
   }
   return B_ENDIAN;
#else
   if (p.parts.lsw)
   { string msg = 
       "Error in compilation of fp.c: don't use flag -DLITTLE_ENDIAN_MACHINE";
     LEDA_EXCEPTION(1, msg);
   }
   return L_ENDIAN;
#endif


#else
   if (sizeof(LEDA_UINT) < size)
   { string msg = 
        "Error in compilation of fp.c: don't use flag -DWORD_LENGTH_64";
     LEDA_EXCEPTION(1, msg);
   }
   return LENGTH_64;
#endif

}

h_type type = check_status();

LEDA_END_NAMESPACE
