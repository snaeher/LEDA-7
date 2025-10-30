/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _bigfloat.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/bigfloat.h>
#include <LEDA/numbers/fp.h>
#include <LEDA/numbers/digit_vector.h>
#include <LEDA/numbers/interval_constants.h>
#include <LEDA/numbers/rational.h>

#include <ctype.h>

#if defined(__DMC__)
#include <iomanip.h>
#endif

#if defined(powl)
#undef powl
#endif


LEDA_BEGIN_NAMESPACE

/* made inline
digit_sz bigfloat::get_significant_length(void) const  
{  return PTR->bitlength;  }
integer bigfloat::get_significant(void) const  
{ return integer(PTR->used,PTR->vec,PTR->sign); }
*/

integer bigfloat::get_exponent(void) const {  
  integer iexp = PTR->exponent; 
  if (PTR->exp_ptr!=NULL)
    iexp += *PTR->exp_ptr;
  return (iexp<<WORD_LENGTH_LOG);
} 

sz_t bigfloat::get_effective_significant_length(void) const
{
	normalize_rep(PTR); // remove trailing blocks of zeros
	if (PTR->bitlength == 0) return 0;
	return PTR->bitlength - digTrailingZeros(PTR->vec[0]);
}

bigfloat ipow2(const integer &p)  { return bigfloat(1,p);  }

inline bool vecIsZeroAt(digit* x, digit_sz position) {

// precondition: x has at least ceil(position/DIGIT_LENGTH) digits 

  digit_sz pos = position-1;
  digit_sz order  = pos>>WORD_LENGTH_LOG;
  digit_sz bitpos = pos-(order<<WORD_LENGTH_LOG);
  return ((x[order]&(1UL<<bitpos))==0);

}

inline bool vecIsZero(digit*x, digit_sz size) {
  digit *x_stop = x+size;
  while (x++<x_stop)
    if (*x)
      return false;
  return true;
}

inline void CutoffDigits(bigfloat_rep* x, digit_sz cutoff_digits) {
  x->vec       += cutoff_digits;
  x->exponent  += long(cutoff_digits);
  x->used      -= cutoff_digits;
  x->bitlength -= (cutoff_digits<<WORD_LENGTH_LOG);
}



static void binout(ostream &os,integer b)
{
  int i;
  os << "( ";
  if (sign(b)==0) {
    for (i=0;i<int(DIGIT_LENGTH);i++)
      os << "0";
    os << " ";
  }
  else {
    int  words = b.used_words();
    char*  tmp = new char[1+((words+1)<<WORD_LENGTH_LOG)];
    digit_sz   pos = 0;
    digit  cur = b[words-1];
    for (i=words-1;i>=0;i--) {
      cur = b[i];
      for (int j=DIGIT_LENGTH-1;j>=0;j--)
        tmp[pos++]=((cur&(1<<j))!=0 ? '1' : '0');
      tmp[pos++] = ' ';
    }
    tmp[pos]='\0';
    os << tmp;
    delete [] tmp;
  }
  os << ")";
}

static void hexout(ostream &os,integer b) 
{
  os << "( ";
  b.hex_print(os);
  os << " )";
/*
  int i;
  int width = DIGIT_LENGTH>>2;
  os << "( ";
  os << hex;
  if (sign(b)==0) {
    // os << setw(width) << setfill('0') << digit(0);
#if !defined(__aCC__)
    os << setw(width); 
#endif
    os << digit(0);
    cout << " ";
  }
  else {
    int  words = b.used_words();
    for (i=words-1;i>=0;i--) 
      // os << setw(width) << setfill('0') << b[i] << " ";
#if !defined(__aCC__)
    os << setw(width); 
#endif
      os << b[i] << " ";
  }
  os << dec;
  os << ")";
*/
}


inline bigfloat powl(const bigfloat &x, sz_t n, digit_sz prec = 1, 
                     rounding_modes mode = EXACT) 
// compute x^n
{
  bool dummy;
  return power(x, (unsigned long) n, prec, mode, dummy);
}


static const double log10_2 = log10(2.0);

static long log10(bigfloat b, digit_sz prec) {

  long b_len   = b.get_significant_length();
  integer iexp = b.get_exponent()+b_len;
  if (!iexp.is_long())
    LEDA_EXCEPTION(1,"operator<<(bigfloat): exponent too large");
  long exp = iexp.to_long();

  bigfloat b_norm = bigfloat(b.get_significant(),-b_len);
  double log10_b_norm = ::log10(to_double(b_norm));
  double log10_b_app  = log10_b_norm + exp*log10_2;
  double error;
  if (sign(iexp)>=0) 
    error = ldexp(log10_b_app,-50);
  else
    error = ldexp(log10_b_norm-exp*log10_2,-50);
  double log10_b_up = ::floor(log10_b_app + error);
  double log10_b_lo = ::floor(log10_b_app - error);
  long log10_b=(long) log10_b_up;
  if (log10_b_lo != log10_b_up) {
    // log10_b not safe using double
    long abslog = (log10_b >= 0 ? log10_b : - log10_b);
    bigfloat pow10 = powl(10,abslog+1,prec+32,TO_NEAREST);
    if (log10_b < 0)
      pow10 = div(1,pow10,prec+1,TO_NEAREST);
    if (pow10 > b) 
      log10_b = (long) log10_b_lo;
  }
  return log10_b;
}


static char* decimal_output
(bigfloat b, long& exp, digit_sz dec_prec, rounding_modes mode=TO_NEAREST)
{
  if (!b.get_exponent().is_long())
    LEDA_EXCEPTION(1,"decimal_output: not implemented for large exponents");
  if (dec_prec==0) 
    LEDA_EXCEPTION(1,"decimal_output: dec_prec has to be bigger than 0!");
  long dd = 10;
  bool is_negative = false;
  if (sign(b) < 0)  { b=-b; is_negative = true; }
  digit_sz bin_prec = (digit_sz) (1+dec_prec/log10_2);
  long log10_b = log10(b,bin_prec);
  
  int decimal_shift = long(dec_prec)-log10_b-1; 
   // decimal exponent by which we shift b
  digit_sz guard = 3;
   // number of guard digits for the following calculations
  digit_sz p = bin_prec + guard;
   // precision of the following calculations
  bigfloat pow10;   // nonegative power of 10 by which we multiply/divide b
  bigfloat b_shift; // the shifted b
  if (decimal_shift >= 0) {
    pow10  = powl(dd,decimal_shift,p,mode);
    b_shift = mul(b,pow10,p,mode);
  }
  else {
    pow10  = powl(dd,-decimal_shift,p,mode);
    b_shift = div(b,pow10,p,mode);
  }

  integer significant = to_integer(b_shift,mode);
    // the decimal digits that we output
  string num_str = significant.to_string();
  char* str = num_str.cstring();
    // the significant transformed into a C-type string

  
  int len_orig = (int) strlen(str);
  int len = len_orig;
  while (str[len-1]=='0') len--;
  char* str_out = new char[len+3];
  char* s = str_out;
  if (is_negative) { s[0]='-'; s++; }
  if (len>1) {
    s[0]=str[0]; s[1]='.';
    memcpy(s+2,str+1,len-1);
    len++;
  }
  else 
    memcpy(s,str,len);
  s[len]='\0';
  exp = len_orig-1-decimal_shift;
  return str_out;


}



static char* scan_sign(char* rep, int& sign) {
  sign=1;
  if (rep[0]=='-') {  
    sign=-1;  rep++;  
  } 
  else 
    if (rep[0]=='+') 
      rep++;
  return rep;
}

static char* scan_integer(char* rep, integer& scan, int& dec_len_int) {

  dec_len_int=0;
  while (isdigit(rep[dec_len_int])) dec_len_int++; 
  char* int_str = new char[dec_len_int+1];
  memcpy(int_str,rep,dec_len_int);
  int_str[dec_len_int]='\0';
  rep+=dec_len_int;
  scan=integer(int_str);
  delete[] int_str;
  return rep;

}


bigfloat& bigfloat::from_string(string s, digit_sz bin_prec) 
{
  char *rep=s.cstring();

  int sign, len;
 
  // first read the sign

  rep = scan_sign(rep,sign);

  // then scan the integral part of the significant:

  integer integral=0;
  rep = scan_integer(rep,integral,len);


  // then scan the fractional part of the significant:

  integer fraction=0;
  int frac_len=0;
  if (*rep == '.') { 
    rep++;
    rep = scan_integer(rep,fraction,frac_len);
  }

    
  // finally scan the exponent:

  integer exp=0;
  int exp_sign;

  if (*rep == 'e') { 
    rep++; 
    rep = scan_sign(rep,exp_sign);
    rep = scan_integer(rep,exp,len);
    if (exp_sign == -1) exp = -exp;
  }

  long lexp=0;

  if (!exp.is_long())
    LEDA_EXCEPTION(1,"bigfloat input error: exponent too large");
  else
    lexp = exp.to_long();

  digit_sz guard = 3;
  digit_sz p = bin_prec + guard;

  long dd=10;

  bigfloat frac = div(fraction,powl(dd,frac_len,p,TO_NEAREST),p);
  bigfloat significant = add(integral,frac,p,EXACT);
  bigfloat bexp = powl(dd,(lexp>=0?lexp:-lexp),p,TO_NEAREST);

  if (lexp>=0) 
    *this = mul(significant,bexp,p); 
  else
    *this = div(significant,bexp,p); 

  if (sign == -1) PTR->sign=-1;

  return *this; 
}




int sign_of_special_value (const bigfloat &x)
{
  if (x.special == bigfloat::NAN_VAL) 
    LEDA_EXCEPTION(1,"sign_of_special_value: want a special value but not NaN");
  if (x.special==bigfloat::NOT_VAL)
    return sign(x); 
  if (  x.special == bigfloat::PZERO_VAL 
     || x.special == bigfloat::PINF_VAL
     ) 
    return 1;
  else 
    return -1;
}


void bigfloat::normalize_long_exponent(bigfloat_rep* z, long exp) {
// first we put the exponent inside iexp
  integer iexp;
  if (z->exp_ptr!=NULL)
    iexp = *z->exp_ptr;
  iexp += exp;
  exp = 0;
// now we try to put the exponent back to z->exponent
  if (iexp.is_long()) {
    exp = iexp.to_long();
    if (exp <= MAXEXP && exp >= MINEXP) {
      z->exponent = exp;
      iexp=0;
    }
  }
// if there is a nonzero iexp, we put it into *z->exp_ptr
  if (iexp!=0) {
    z->exponent = 0;
    if (z->exp_ptr!=NULL)
      *z->exp_ptr = iexp;
    else
      z->exp_ptr = new integer(iexp);
  }
  else 
    if (z->exp_ptr!=NULL) {
      delete z->exp_ptr;
      z->exp_ptr=NULL;
    }
}




#define DOUBLE_PREC 53
digit_sz bigfloat::global_prec=DOUBLE_PREC;
digit_sz bigfloat::global_output_prec=16;
digit_sz bigfloat::global_input_prec=DOUBLE_PREC;
rounding_modes bigfloat::round_mode=TO_NEAREST;
bool bigfloat::dbool=true;
bigfloat::output_modes bigfloat::output_mode=bigfloat::DEC_OUT;
#ifdef BIGFLOAT_CHECKS_ON
digit_sz bigfloat::obj_count = 0;
#endif

const bigfloat bigfloat::pZero(bigfloat::PZERO_VAL);
const bigfloat bigfloat::nZero(bigfloat::NZERO_VAL);
const bigfloat bigfloat::pInf(bigfloat::PINF_VAL);
const bigfloat bigfloat::nInf(bigfloat::NINF_VAL);
const bigfloat bigfloat::NaN(bigfloat::NAN_VAL);



digit_sz bigfloat::set_precision(digit_sz p) 
{ 
   digit_sz tmp = global_prec; 
   global_prec=p; 
   return tmp;
}
digit_sz bigfloat::get_precision() 
{ 
   return global_prec; 
}
digit_sz bigfloat::set_output_precision(digit_sz d) 
{ 
   digit_sz tmp = global_output_prec; 
   global_output_prec=d; 
   return tmp;
}
digit_sz bigfloat::set_input_precision(digit_sz p) 
{ 
   digit_sz tmp = global_input_prec; 
   global_input_prec=p; 
   return tmp;
}
rounding_modes bigfloat::set_rounding_mode(rounding_modes m) 
{ 
   rounding_modes tmp = round_mode;
   round_mode=m; 
   return tmp;
}
rounding_modes bigfloat::get_rounding_mode() 
{ 
  return round_mode;
}
bigfloat::output_modes bigfloat::set_output_mode(output_modes o_mode) 
{ 
  output_modes tmp = output_mode;
  output_mode = o_mode;
  return tmp;
}



bigfloat& bigfloat::operator=(const bigfloat& x) {

 x.PTR->ref_plus();
 clear();
 PTR = x.PTR;

 special = x.special;

#ifdef BIGFLOAT_CHECKS_ON
 if (x.PTR->used>0)
   assert (x.PTR->bitlength==vecLength(x.PTR->vec,x.PTR->used));
#endif

 return *this;
}


bigfloat::bigfloat()
{
  static bigfloat bigfloat_zero(0);
  PTR = bigfloat_zero.PTR;
  PTR->ref_plus();
  special = bigfloat::PZERO_VAL;
/*
  PTR = bigfloat::new_rep(1);
  PTR->used = 0;
  PTR->sign = 0;
  special = bigfloat::PZERO_VAL;
*/
} 

bigfloat::bigfloat(const integer &s,const integer &e)
{
  if (s.PTR->sign==0) {
    PTR = bigfloat::new_rep(1);
    PTR->used = 0;
    PTR->sign = 0;
    special = bigfloat::PZERO_VAL;
  }
  else {
    digit_sz used = s.PTR->used;
    digit_sz size = 1+digNextPower(1+used);
    PTR = bigfloat::new_rep(size);
    PTR->sign = s.PTR->sign; 
    PTR->used = used;
    special = bigfloat::NOT_VAL;

// by s.n (06/2007)
    long m = (e%DIGIT_LENGTH).to_long();
    if (m < 0) m += DIGIT_LENGTH;

    digit_sz emod = (digit_sz)m;

    integer new_exponent = (e-emod)>>WORD_LENGTH_LOG;
    PTR->exponent = new_exponent.to_long();
    if (!new_exponent.is_long()||new_exponent>MAXEXP||new_exponent<MINEXP) {
      PTR->exponent = 0;
      PTR->exp_ptr = new integer(new_exponent);
    }
    PTR->bitlength = s.length()+emod;
    if (emod>0) {
      PTR->vec[used]=vecShiftLeftCarry(PTR->vec,s.PTR->vec,used,emod);
      if (PTR->vec[used]!=0)
         PTR->used++;
    }
    else
      vecCopy(PTR->vec,s.PTR->vec,used);
    bigfloat::normalize_rep(PTR);
  }
}


bigfloat::bigfloat(special_values sp)  {  
  PTR = bigfloat::new_rep(1);
  PTR->used = 0;
  special = sp;  
  if (special==PZERO_VAL || special == NZERO_VAL)
    PTR->sign=0;
}

bigfloat::bigfloat(const integer &a)
{   
  if (a.PTR->sign==0) {
    PTR = bigfloat::new_rep(1);
    PTR->used = 0;
    PTR->sign = 0;
    special = bigfloat::PZERO_VAL;
  }   
  else {
    PTR = bigfloat::copy_rep(a.PTR);
    PTR->bitlength = a.length();
    PTR->exponent = 0;
    special = NOT_VAL;
  }

  bigfloat::normalize_rep(PTR);
}


bigfloat::bigfloat (long a)
{  
  PTR = bigfloat::new_rep(1);

  if (a==0) {
    special = bigfloat::PZERO_VAL;
    PTR->used = 0;
    PTR->sign = 0;
  }
  else {
    digit apos = (digit) (a>0?a:-a);
    special = bigfloat::NOT_VAL;
    PTR->used = 1;
    PTR->bitlength = DIGIT_LENGTH-digLeadingZeros(apos);
    PTR->exponent = 0;
    if (a>0) {
      PTR->sign = 1;
      PTR->vec[0] = apos;
    }
    else {
      PTR->sign = -1;
      PTR->vec[0] = apos;
    }
  }



                                              
}

bigfloat::bigfloat (int a)
{  
  PTR = bigfloat::new_rep(1);

  if (a==0) {
    special = bigfloat::PZERO_VAL;
    PTR->used = 0;
    PTR->sign = 0;
  }
  else {
    digit apos = (digit) (a>0?a:-a);
    special = bigfloat::NOT_VAL;
    PTR->used = 1;
    PTR->bitlength = DIGIT_LENGTH-digLeadingZeros(apos);
    PTR->exponent = 0;
    if (a>0) {
      PTR->sign = 1;
      PTR->vec[0] = apos;
    }
    else {
      PTR->sign = -1;
      PTR->vec[0] = apos;
    }
  }



                                              
} 



bigfloat::bigfloat(double d)
{   
  if (fp::is_infinite(d)) {
    PTR = bigfloat::new_rep(1);
	PTR->used = 0;
    if (d>0) { special = bigfloat::PINF_VAL; return; }
    if (d<0) { special = bigfloat::NINF_VAL; return; }
    special = bigfloat::NAN_VAL; return;
  }

  bool is_long = (d == 0);
  is_long = is_long || 
    ( fp::get_unbiased_exponent(d)<DIGIT_LENGTH && d == long(d) );

  if (is_long) {
     long a = (long) d;
     
  PTR = bigfloat::new_rep(1);

  if (a==0) {
    special = bigfloat::PZERO_VAL;
    PTR->used = 0;
    PTR->sign = 0;
  }
  else {
    digit apos = (digit) (a>0?a:-a);
    special = bigfloat::NOT_VAL;
    PTR->used = 1;
    PTR->bitlength = DIGIT_LENGTH-digLeadingZeros(apos);
    PTR->exponent = 0;
    if (a>0) {
      PTR->sign = 1;
      PTR->vec[0] = apos;
    }
    else {
      PTR->sign = -1;
      PTR->vec[0] = apos;
    }
  }



  }
  else {
    PTR = bigfloat::new_rep(3);
    int exp;
    PTR->used = vecFromDoubleAligned(d,PTR->vec,PTR->sign,exp,PTR->bitlength);
    PTR->exponent = exp;

#ifdef BIGFLOAT_CHECKS_ON
    integer sig(PTR->used,PTR->vec,PTR->sign);
    double dsig  = sig.to_double();
    double check = ldexp(dsig,int(PTR->exponent<<WORD_LENGTH_LOG));
    assert (check == d);
#endif

    special = bigfloat::NOT_VAL;
    bigfloat::normalize_rep(PTR);
  }
}

// zlotowski hack  
// 642 : if (::sign(*x.PTR->exp_ptr)>0)

const double undo_scaling = fp::power_two(-1000);

double bigfloat::to_double() const
{
  const bigfloat& x = *this;
  
  if (isSpecial(x))
  {
    if (ispZero(x))  return fp::pZero;
    if (isnZero(x))  return fp::nZero;
    if (isNaN(x))    return fp::NaN;
    if (ispInf(x))   return fp::pInf;
    if (isnInf(x))   return fp::nInf;
  }

  if (x.PTR->exp_ptr) {
    bigfloat::normalize_long_exponent(x.PTR, 0);
    if (x.PTR->exp_ptr) {
      if (sign(*x.PTR->exp_ptr) > 0)
        return (x.PTR->sign>0 ? fp::pInf : fp::nInf);
      else 
        return (x.PTR->sign>0 ? fp::pZero : fp::nZero);
    }
  }

  bigfloat y = round(x, DOUBLE_PREC, TO_NEAREST);
  
  digit_sz used = y.PTR->used;
  double significant = 0;
  while (used>0) {
    significant = significant * digit_base + y.PTR->vec[used-1];
    used--;
  }
  if (y.PTR->sign<0)
    significant = -significant; 

  double result;
  long exp = (y.PTR->exponent << WORD_LENGTH_LOG); // no overflow possible! (cf. MINEXP, MAXEXP)
  if (exp > -1023 && exp <= 1023) 
    result = significant*fp::power_two(exp);
  else {
    if (exp > 1023)
      result = (y.PTR->sign>0 ? fp::pInf : fp::nInf);
    else {
      long sexp = fp::get_unbiased_exponent(significant)+exp;
      if (sexp < -1074)
        result = (x.PTR->sign>0 ? fp::pZero : fp::nZero);
      else  {
        // denormal case
        result  = significant*fp::power_two(exp+1000);
        result *= undo_scaling; // undo_scaling == 2^(-1000)
      }
    }
  }

  return result;
}

double bigfloat::to_double(double& abs_err, rounding_modes m) const
{
   const bigfloat& x = *this;
   abs_err = 0;

   // handle special values
   if (isSpecial(x))
   {
     if (ispZero(x))  return fp::pZero;
     if (isnZero(x))  return fp::nZero;

     abs_err = fp::NaN;
     if (isNaN(x))    return fp::NaN;
     if (ispInf(x))   return fp::pInf;
     if (isnInf(x))   return fp::nInf;
   }

   // handle the case that |exponent of x| is very large
   if (x.PTR->exp_ptr) {
     bigfloat::normalize_long_exponent(x.PTR, 0);
     if (x.PTR->exp_ptr) {
       if (sign(*x.PTR->exp_ptr) > 0) {
         abs_err = fp::pInf;
         if (x.PTR->sign > 0) 
           return (m == TO_ZERO || m == TO_N_INF) ?  MaxDbl : fp::pInf;
	    else
           return (m == TO_ZERO || m == TO_P_INF) ? -MaxDbl : fp::nInf;
	  }
	  else {
         abs_err = DenormalizedMinDbl;
         if (x.PTR->sign > 0)
           return (m == TO_P_INF || m == TO_INF) ?  DenormalizedMinDbl : fp::pZero;
	    else
           return (m == TO_N_INF || m == TO_INF) ? -DenormalizedMinDbl : fp::nZero;
	  }
     }
   }

   if (m == EXACT) m = TO_NEAREST;
   bool is_exact;
   bigfloat y = round(x, 53, m, is_exact);
   /*
      why not 52?
      Note: A double stores 52 bits for the mantissa, but the matissa is
      m = 1 . m1 m2 ... m52 (in the normalized case) or
      m = 0 . m1 m2 ... m52 (in the denormalized case)

      eps == 2^(-52)
      MinDbl == 2^(-1022)
      DenormalizedMinDbl == 2^(-1074) == 2^(-52)*2^(-1022) (smallest positive double)
   */

   bigfloat::normalize_rep(y.PTR); // is this necessary?
   if (y.PTR->exp_ptr) // just in case
     // if the exponent of y is too big we let the code above handle the situation ...
     return y.to_double(abs_err, m);

   // convert y to tuple (significant,exp) s.th. y = significant * 2^(exp)
   digit_sz used = y.PTR->used;
   double significant = 0;
   while (used > 0) {
     significant = significant * digit_base + y.PTR->vec[used-1];
     used--;
   }
   if (y.PTR->sign < 0) significant = -significant;
   // Note: Due to trailing zeros in y.significant maybe |significant| >= 2^53

   long exp = (y.PTR->exponent) << WORD_LENGTH_LOG;
     // no overflow possible (cf. MIN_EXP, MAX_EXP)

   // normalize (significant,exp)
   fp::set_unbiased_exponent(significant, 0);
     // should be same as multiplying with 2^(-(y.get_significant_length() - 1))
   if (exp < MAXEXP) exp += y.get_significant_length() - 1;
   // abs_err = is_exact ? 0 : 2^(-52) * 2^(exp) (note: maybe 2^(exp) is no double!)

   if (exp > -1023 && exp <= 1023) {
     // y is a normalized double (so the following is okay for all rounding modes)
     if (! is_exact) abs_err = eps * fp::power_two(exp);
     return significant * fp::power_two(exp);
   }

   else if (exp > 1023) {
     // |y| is too big for a double
	 abs_err = fp::pInf;
     if (y.PTR->sign > 0)
       return (m == TO_ZERO || m == TO_N_INF) ?  MaxDbl : fp::pInf;
     else
       return (m == TO_ZERO || m == TO_P_INF) ? -MaxDbl : fp::nInf;
   }

   else /* exp <= -1023 */ {
     if (exp >= -1074) /* -1074 <= exp < -1022 */ {
       // |y| is larger than DenormalizedMinDbl
       if (!is_exact) abs_err = DenormalizedMinDbl; // > 2^(-52)*2^(exp)

       /*
          y might be represented as a denormalized double:
            0 . m1' ... m52' * 2^(-1022)
          Since (significant, exp) = 1 . m1 ... m52 * 2^(exp)
          the conversion to a denormalized double loses some of the trailing
          digits of the mantissa of significant, namely -(exp + 1022)
          we have: 1 <= num_lost_digits <= 52 (i.e. at the first 1 remains)
          If the number of trailing zeros in the mantissa of significant is
          larger than num_lost_digits, y can be converted to a double without
          loss.
       */

       // count trailing zeros in the mantissa of significant
       int s; unsigned long e; digit m_most_sig_20, m_least_sig_32;
       fp::read_parts(significant, s, e, m_most_sig_20, m_least_sig_32);
       digit_sz num_trailing_zeros = 0;
       if (m_least_sig_32 != 0) 
         num_trailing_zeros = digTrailingZeros(m_least_sig_32);
       else
         num_trailing_zeros = 32 + digTrailingZeros(m_most_sig_20);

       long num_lost_digits = -(exp + 1022);
       if (long(num_trailing_zeros) < num_lost_digits) {
         // lossless conversion is not possible
         abs_err += DenormalizedMinDbl;

         // if rounding mode is TO_NEAREST we do nothing in order to remain compatible
         if (m != TO_NEAREST) {
           // ow. we first set all lost digits in significant to 0, i.e. we round TO_ZERO
           long remove_trailing_digits = num_lost_digits;
           if (remove_trailing_digits < 32) {
             digit mask = (1UL << remove_trailing_digits) - 1;
             m_least_sig_32 &= ~mask;
           }
           else {
             m_least_sig_32 = 0; remove_trailing_digits -= 32;
             if (remove_trailing_digits > 0) {
               digit mask = (1UL << remove_trailing_digits) - 1;
               m_most_sig_20 &= ~mask;
             }
           }
           significant = fp::compose_parts(s, e, m_most_sig_20, m_least_sig_32);

           // if we round TO_INF, we "increment" the digit before the lost digits
           double round_to_inf_value = fp::power_two( num_lost_digits - 52 );
	       if (significant > 0 && (m == TO_INF || m == TO_P_INF))
	         significant += round_to_inf_value;
	       else if (significant < 0 && (m == TO_INF || m == TO_N_INF))
	         significant -= round_to_inf_value;
	     }
       }

       significant *= eps; exp += 52; // now exp >= -1022!
       return significant * fp::power_two(exp);
     }

     else /* exp < -1074 */ {
       // |y| is too small for a double (smaller than DenormalizedMinDbl)
       abs_err = DenormalizedMinDbl;

       // if m == TO_NEAREST and exp == -1075, we may not return the nearest double
       // but we want to remain compatible, so we do not care ...

       if (y.PTR->sign > 0)
         return (m == TO_P_INF || m == TO_INF) ?  DenormalizedMinDbl : fp::pZero;
       else
         return (m == TO_N_INF || m == TO_INF) ? -DenormalizedMinDbl : fp::nZero;
     }
   }

   // This should never be reached!
   return fp::NaN;
}


static void DoNormalRounding(bigfloat_rep* x, digit_sz prec, bool do_increment)
{

#ifdef BIGFLOAT_CHECKS_ON
  assert (x->bitlength>=prec);
    // otherwise rounding is not necessary in mode to_nearest
#endif

  digit_sz position      = x->bitlength-prec;
    // the overall number of trailing bits cut off in the rounding
  digit_sz cutoff_digits = (position>>WORD_LENGTH_LOG);
    // the number of full digits that are cut off
  digit_sz cutoff_bits   = (position-(cutoff_digits<<WORD_LENGTH_LOG));
    // the bits that are cut off in addition to cutoff_digits

  digit last_bit = (1UL<<cutoff_bits);
    // the bit corresponding to order position

  // digit last_bit = (cutoff_bits ? (1UL<<cutoff_bits) : 1 );
    // the bit corresponding to order position

  
    
  // first forget about the cutoff digits
  if (cutoff_digits)
    CutoffDigits(x,cutoff_digits);

  digit last_digit=x->vec[0];
  // the digit that contains last_bit

  // then cut off the cutoff_bits
  if (cutoff_bits)last_digit &= ~(last_bit-1);



    // finally apply do_increment
  if (do_increment) {
    last_digit+=last_bit;
    x->vec[0]=last_digit;
    x->vec[x->used]=0;
      // this is important for the following vecIsZeroAt
    if (last_digit<last_bit && x->used>1)
      vecIncrement(x->vec+1,x->used-1);
    if (x->vec[x->used-1]==0) {
      // the result is a power of the base
      CutoffDigits(x,x->used);
      x->vec[0]=1;
      x->bitlength=1;
      x->used=1;
    }
    else {
      if (!vecIsZeroAt(x->vec,x->bitlength+1))
        x->bitlength++;
    }

  }
  else 
    x->vec[0]=last_digit;


}

static void DoBiasRounding(bigfloat_rep* x, digit_sz prec, bool do_increment)
{
#ifdef BIGFLOAT_CHECKS_ON
  assert (x->bitlength>=prec);
    // x->bitlength == prec is possible for BiasRounding
#endif
  

  digit_sz position      = x->bitlength-prec;
    // the overall number of trailing bits cut off in the rounding
  digit_sz cutoff_digits = (position>>WORD_LENGTH_LOG);
    // the number of full digits that are cut off
  digit_sz cutoff_bits   = (position-(cutoff_digits<<WORD_LENGTH_LOG));
    // the bits that are cut off in addition to cutoff_digits

  digit last_bit = (1UL<<cutoff_bits);
    // the bit corresponding to order position

  
    
  // first forget about the cutoff digits
  if (cutoff_digits)
    CutoffDigits(x,cutoff_digits);

  digit last_digit=x->vec[0];
  // the digit that contains last_bit

  // then cut off the cutoff_bits
  if (cutoff_bits)last_digit &= ~(last_bit-1);




    // increment or decrement
    if (do_increment) {
      
  last_digit+=last_bit;
  x->vec[0]=last_digit;
  x->vec[x->used]=0;
    // this is important for the following vecIsZeroAt
  if (last_digit<last_bit && x->used>1)
    vecIncrement(x->vec+1,x->used-1);
  if (x->vec[x->used-1]==0) {
    // the result is a power of the base
    CutoffDigits(x,x->used);
    x->vec[0]=1;
    x->bitlength=1;
    x->used=1;
  }
  else
    if (!vecIsZeroAt(x->vec,x->bitlength+1))
      x->bitlength++;


    }
    else {
      
  x->vec[0]=last_digit-last_bit;
  x->vec[x->used]=0;
    // this is important for the following vecIsZeroAt
  if (x->vec[0]>last_digit) 
    vecDecrement(x->vec+1,x->used);
#ifdef BIGFLOAT_CHECKS_ON
  assert(x->vec[x->used]==0);                        
#endif
  if (x->vec[x->used-1]==0) {
    x->used--;
    x->bitlength--;
  }
  else
    if (vecIsZeroAt(x->vec,x->bitlength))
      x->bitlength--;
#ifdef BIGFLOAT_CHECKS_ON
  assert(x->vec[x->used-1]!=0);                        
#endif



    }



}



void RoundNearestCase(bigfloat_rep* x, digit_sz prec, int bias, 
  bool& exact_without_bias, bool& increment)
{
  
// exact is true (initially and finally) iff x->vec is exact

#ifdef BIGFLOAT_CHECKS_ON
  assert (x->bitlength>prec);
    // otherwise rounding is not necessary in mode to_nearest
  assert (x->vec[0]!=0);
    // x is supposed to be normalized
#endif

  digit_sz position      = x->bitlength-prec;
    // the overall number of trailing bits cut off in the rounding
  digit_sz cutoff_digits = (position>>WORD_LENGTH_LOG);
    // the number of full digits that are cut off
  digit_sz cutoff_bits   = (position-(cutoff_digits<<WORD_LENGTH_LOG));
    // the bits that are cut off in addition to cutoff_digits

// we first consider the digit containing the bit after 
// the rounding position 

  digit rd_digit; 
    // the digit containing the bit of order position-1 from the right,
    // which decides about the rounding
  digit bit_after;
    // the bit corresponding to order position-1
  digit last_bit;
    // the bit corresponding to order position

  if (cutoff_bits) {
    rd_digit  = x->vec[cutoff_digits];
    bit_after = 1UL<<(cutoff_bits-1);
    last_bit  = (bit_after<<1);
  }
  else {
    rd_digit   = x->vec[cutoff_digits-1];
    bit_after  = 1UL<<(DIGIT_LENGTH-1);
    last_bit   = 1;
  }

  digit remaining_bits = rd_digit & (bit_after-1);
    // the bits in rd_digit smaller than bit_after
  bool bit_after_set = ((bit_after & rd_digit) != 0);
    // true iff bit_after is set in rd_digit

  exact_without_bias = (cutoff_digits==0) && (remaining_bits==0);

// now we have enough information to decide the rounding case

  bool do_increment=bit_after_set;
    // true iff we have to add a one at position+1. 

  if (exact_without_bias) {
    if (bit_after_set) {
      exact_without_bias = false;
      // round to even
      if (bias==0)
        do_increment = ((x->vec[cutoff_digits]&last_bit) != 0);
      else {
        bias = (bias>0?1:-1);
        do_increment = (bias == x->sign);
      }
    }
  }
  
  // never used
  // bool exact = exact_without_bias && (bias == 0);

  increment          = do_increment;
}


void RoundNearestDestructive(bigfloat_rep* x, digit_sz prec, bool& exact, int bias) 
{
  bool exact_without_bias=true;
  
// exact is true (initially and finally) iff x->vec is exact

#ifdef BIGFLOAT_CHECKS_ON
  assert (x->bitlength>prec);
    // otherwise rounding is not necessary in mode to_nearest
  assert (x->vec[0]!=0);
    // x is supposed to be normalized
#endif

  digit_sz position      = x->bitlength-prec;
    // the overall number of trailing bits cut off in the rounding
  digit_sz cutoff_digits = (position>>WORD_LENGTH_LOG);
    // the number of full digits that are cut off
  digit_sz cutoff_bits   = (position-(cutoff_digits<<WORD_LENGTH_LOG));
    // the bits that are cut off in addition to cutoff_digits

// we first consider the digit containing the bit after 
// the rounding position 

  digit rd_digit; 
    // the digit containing the bit of order position-1 from the right,
    // which decides about the rounding
  digit bit_after;
    // the bit corresponding to order position-1
  digit last_bit;
    // the bit corresponding to order position

  if (cutoff_bits) {
    rd_digit  = x->vec[cutoff_digits];
    bit_after = 1UL<<(cutoff_bits-1);
    last_bit  = (bit_after<<1);
  }
  else {
    rd_digit   = x->vec[cutoff_digits-1];
    bit_after  = 1UL<<(DIGIT_LENGTH-1);
    last_bit   = 1;
  }

  digit remaining_bits = rd_digit & (bit_after-1);
    // the bits in rd_digit smaller than bit_after
  bool bit_after_set = ((bit_after & rd_digit) != 0);
    // true iff bit_after is set in rd_digit

  exact_without_bias = (cutoff_digits==0) && (remaining_bits==0);

// now we have enough information to decide the rounding case

  bool do_increment=bit_after_set;
    // true iff we have to add a one at position+1. 

  if (exact_without_bias) {
    if (bit_after_set) {
      exact_without_bias = false;
      // round to even
      if (bias==0)
        do_increment = ((x->vec[cutoff_digits]&last_bit) != 0);
      else {
        bias = (bias>0?1:-1);
        do_increment = (bias == x->sign);
      }
    }
  }
  
  exact = exact_without_bias && (bias == 0);




  if (!exact_without_bias) {
    
    
  // first forget about the cutoff digits
  if (cutoff_digits) CutoffDigits(x,cutoff_digits);

  digit last_digit=x->vec[0];
  // the digit that contains last_bit

  // then cut off the cutoff_bits
  if (cutoff_bits) last_digit &= ~(last_bit-1);




    // finally apply do_increment
    if (do_increment) {
      
  last_digit+=last_bit;
  x->vec[0]=last_digit;
  x->vec[x->used]=0;
    // this is important for the following vecIsZeroAt
  if (last_digit<last_bit && x->used>1)
    vecIncrement(x->vec+1,x->used-1);
  if (x->vec[x->used-1]==0) {
    // the result is a power of the base
    CutoffDigits(x,x->used);
    x->vec[0]=1;
    x->bitlength=1;
    x->used=1;
  }
  else
    if (!vecIsZeroAt(x->vec,x->bitlength+1))
      x->bitlength++;


    }
    else 
      x->vec[0]=last_digit;

  }
} 



bool RoundDirectedCase
(bigfloat_rep* x, digit_sz prec, rounding_modes mode, int bias,         
 bool& exact_without_bias, bool& do_increment) {
// 
// returns need_rounding == true iff the rounding to prec bits 
// in the specified rounding mode with bias will change x
// exact_without_bias is set iff without bias no rounding is required
// do_increment is only defined if need_rounding == true;
// it is set iff the significant of x needs to be increased.
// 

#ifdef BIGFLOAT_CHECKS_ON
  assert (mode!=TO_NEAREST&&mode!=EXACT);
  assert (x->vec[0]!=0);
#endif

  do_increment=false; 
    // true iff the significant is rounded away from zero

  exact_without_bias=true;
  if (x->bitlength > prec) {
    digit_sz prec_diff     = x->bitlength-prec;
    digit_sz cutoff_digits = (prec_diff>>WORD_LENGTH_LOG);
    if (cutoff_digits > 0) 
      exact_without_bias=false;
    else {
      digit remaining_bits = x->vec[0]&((1UL<<prec_diff)-1);
      exact_without_bias = (remaining_bits == 0);
    } 
  }
 
  bool exact = exact_without_bias && (bias == 0);
  bool need_rounding;

  if (exact) 
    need_rounding = false;
  else {
    need_rounding = true;
    if (bias)
      bias = (bias>0?1:-1);
    switch (mode) {
      case TO_ZERO:
        do_increment = false;
        if (exact_without_bias)
          need_rounding = (bias != x->sign);
        break;
      case TO_INF:
        do_increment = true;
        if (exact_without_bias)
          need_rounding = (bias == x->sign);
        break;
      case TO_P_INF:
        if (!exact_without_bias)
          do_increment = (x->sign > 0);
        else {
          need_rounding = (bias>0);
          do_increment = (x->sign>0);
        }
        break;
      case TO_N_INF:
        if (!exact_without_bias)
          do_increment = (x->sign < 0);
        else {
          need_rounding = (bias<0);
          do_increment = (x->sign<0);
        }
        break;
      default:
        break;
    }
  }
  return need_rounding;
}




bigfloat round(const bigfloat& x, digit_sz prec, rounding_modes mode, 
  bool &is_exact, int bias)
{

  if (isSpecial(x) || (mode == EXACT))
  {
    is_exact=true;
    return x;
  }

  bool exact_without_bias = true;
  bool need_rounding      = false;
  bool do_increment       = false;

  if (mode == TO_NEAREST) {
    if (x.PTR->bitlength>prec) {
      // we certainly don't need rounding otherwise
      RoundNearestCase(x.PTR,prec,bias,exact_without_bias,do_increment);
      need_rounding = !exact_without_bias;
    }
  }
  else 
    // we might have to round even though exact_without_bias==true
    need_rounding = 
      RoundDirectedCase(x.PTR,prec,mode,bias,exact_without_bias,do_increment);
    
  is_exact = exact_without_bias && (bias==0);

  if (need_rounding) {

    digit_sz max_used = 3+(prec>>WORD_LENGTH_LOG);
      // the maximal amount of digits we use
      // caution, the leading and the trailing zeros
      // might add to 2*(DIGIT_LENGTH-1) bits
      // need one more bit for possible overflow
    digit_sz size = 1+digNextPower(max_used);
    bigfloat_rep* result = bigfloat::new_rep(size);
    result->sign         = x.PTR->sign;

    if (exact_without_bias && !do_increment) {
      digit_sz used = x.PTR->used;
      digit_sz length_highword  
        = x.PTR->bitlength - ((used-1)<<WORD_LENGTH_LOG);
      if (x.PTR->vec[used-1] == (1UL<<(length_highword-1)))
        if ( used==1 || vecIsZero(x.PTR->vec,used-1) )
          prec++;
      // this is a hack to ensure that bias rounding is correct
      // even if we decrement a power of two
    }
    
    if (x.PTR->bitlength<prec) {
      digit_sz diff        = prec-x.PTR->bitlength;
      digit_sz fill_digits = diff>>WORD_LENGTH_LOG;
      if (diff != (fill_digits<<WORD_LENGTH_LOG))
        fill_digits++;
      if (fill_digits)
        vecInit(result->vec,fill_digits);
      vecCopy(result->vec+fill_digits,x.PTR->vec,x.PTR->used);
      result->exponent  = x.PTR->exponent  - long(fill_digits);
      result->bitlength = x.PTR->bitlength + (fill_digits<<WORD_LENGTH_LOG);
      result->used      = x.PTR->used      + fill_digits;
    }
    else {
      digit_sz cutoff_digits = (x.PTR->bitlength-prec)>>WORD_LENGTH_LOG;
      result->used       = x.PTR->used-cutoff_digits;
      vecCopy(result->vec,x.PTR->vec+cutoff_digits,result->used);
      result->exponent  = x.PTR->exponent   + long(cutoff_digits);
      result->bitlength = x.PTR->bitlength  - (cutoff_digits<<WORD_LENGTH_LOG);
    }
    if (x.PTR->exp_ptr!=NULL) {
      result->exp_ptr = new integer(*x.PTR->exp_ptr);
      bigfloat::normalize_long_exponent(result,0);
    }
  
    if (exact_without_bias)
      DoBiasRounding(result,prec,do_increment);
    else 
      DoNormalRounding(result,prec,do_increment);

    return result;
  }
  else
    return x;

}

// zlotowski hack
// 1208 : if (::sign(iexp)>0)

enum nearest_case { rd_exact = 0, rd_zero = 1, rd_inf = 2, rd_even = 3} ;

integer bigfloat::to_integer(rounding_modes rmode, bool& is_exact) const
{
  bigfloat x = *this;

  if ((isNaN(x))||(isInf(x)))
    LEDA_EXCEPTION(1,"to_integer(bigfloat): non-finite value");
 
  if (isZero(x)) { is_exact = true; return 0; }

  if (x.PTR->exp_ptr!=NULL) {
    integer iexp(*x.PTR->exp_ptr);
    if (sign(iexp)>0)
      LEDA_EXCEPTION(1,"bigfloat::to_integer: integer too long");
    else
      x = x.PTR->sign*0.25;
      // the result is the same as for +- epsilon, epsilon < 1/2
  }

  long lexp = (long) x.PTR->exponent;
  is_exact = (lexp>=0);
  // this works because x is normalized

  bool round_increment=false;
  if (!is_exact) {
    nearest_case rd_case=rd_exact;
    bool is_even=true;
    long xused = (long) x.PTR->used;
    switch (rmode) {
      case TO_NEAREST:
        if (xused+lexp>=0) {
          digit cur_digit = x.PTR->vec[-1-lexp];
          if (cur_digit >> (DIGIT_LENGTH-1)) {
            if (lexp<-1 || (cur_digit != (1UL<<(DIGIT_LENGTH-1))))
              rd_case = rd_inf;
            else {
              rd_case = rd_even;
              if (xused+lexp>0)
                is_even = ((x.PTR->vec[-lexp]&1UL)==0);
              else
                is_even = true;
            }
          }
          else
            rd_case = rd_zero;
        }
        else 
          rd_case = rd_zero;
        round_increment = 
          (rd_case == rd_inf || (rd_case == rd_even && !is_even));
        break;

      case TO_ZERO:
        round_increment = false;
        break;
      case TO_INF:
        round_increment = true;
        break;
      case TO_P_INF:
        round_increment = (x.PTR->sign > 0);
        break;
      case TO_N_INF:
        round_increment = (x.PTR->sign < 0);
        break;
      default:
        break;
    }
  }

  long sum = x.PTR->used+lexp;
  digit_sz used = (digit_sz) (sum > 0?sum:0);
   // the number of needed digits in the result
  digit_sz size = 1+digNextPower(used+1);

  integer_rep* z = integer::new_rep(size);
  z->used = used;
  z->sign = x.PTR->sign;

  if (used>0) {
    if (lexp<=0)
      vecCopy(z->vec,x.PTR->vec-lexp,used);
    else {
      vecCopy(z->vec+lexp,x.PTR->vec,x.PTR->used);
      vecInit(z->vec,lexp);
    }
  }
  else 
    z->vec[0]=0;

  if (round_increment) {
    z->vec[used]=0;
    vecIncrement(z->vec,used+1);
    if (z->vec[used]!=0) 
      z->used++;
  }
  if (z->used==0)
    z->sign=0;
    
  return z;

}


integer to_integer(const bigfloat& x,rounding_modes rmode,bool& is_exact) {
  return x.to_integer(rmode,is_exact);
}

bigfloat bigfloat::from_rational(const rational& r, sz_t prec, 
                                 rounding_modes rmode, bool& is_exact)
{
	bigfloat num(r.numerator()), den(r.denominator());
	return div(num, den, prec, rmode, is_exact);
}





  bigfloat operator+(const bigfloat &a,const bigfloat &b) 
  {  return add(a,b);}
  
  bigfloat operator-(const bigfloat &a,const bigfloat &b) 
  {  return sub(a,b); }
  
  bigfloat operator*(const bigfloat &a,const bigfloat &b) 
  {  return mul(a,b); }
  
  bigfloat operator/(const bigfloat &a,const bigfloat &b) 
  {  return div(a,b); }

  bigfloat operator+=(bigfloat &a,const bigfloat &b) 
  {  a = add(a,b);  return a;  }
  
  bigfloat operator-=(bigfloat &a,const bigfloat &b) 
  {  a = sub(a,b);  return a;  }
  
  bigfloat operator*=(bigfloat &a,const bigfloat &b) 
  {  a = mul(a,b);  return a; }
  
  bigfloat operator/=(bigfloat &a,const bigfloat &b) 
  {  a = div(a,b);  return a; }



bigfloat operator-(const bigfloat &a)
{
  if (isSpecial(a))
  {
    if (ispZero(a)) return bigfloat::NZERO_VAL;
    if (isnZero(a)) return bigfloat::PZERO_VAL;
    if (ispInf(a))  return bigfloat::NINF_VAL;
    if (isnInf(a))  return bigfloat::PINF_VAL;
    return bigfloat::NAN_VAL;
  }
  bigfloat_rep* PTR = bigfloat::copy_rep(a.PTR);
  PTR->sign=-PTR->sign;
  return PTR;
}



bigfloat bigfloat::special_addsub
(const bigfloat& x, const bigfloat& y, int y_sign, bool& is_exact) {
  is_exact = true;

  if (y_sign>0) {
    
    if (isNaN(x) || isNaN(y)) 
      { is_exact = false; return bigfloat::NAN_VAL; }

    if (isZero(x) && isZero(y)) {
      if (isnZero(x) && isnZero(y))
        return bigfloat::NZERO_VAL;
      else
        return bigfloat::PZERO_VAL;
    }

    if (isZero(x)) return y;
    if (isZero(y)) return x;

    if (isInf(x) && isInf(y)) {
      if (sign_of_special_value(x) == sign_of_special_value(y))
        return x;
      else { is_exact = false; return bigfloat::NAN_VAL; }
    }
    if (isInf(x)) return x;

    // y has to be infinity 
    return y;
  }
  else {
    
    if (isNaN(x) || isNaN(y))
      { is_exact = false; return bigfloat::NAN_VAL; }

    if (isZero(x) && isZero(y)) {
      if (isnZero(x) && ispZero(y))
        return bigfloat::NZERO_VAL;
      else
        return bigfloat::PZERO_VAL;
    }

    if (isZero(x)) return -y;
    if (isZero(y)) return x;

    if (isInf(x) && isInf(y)) {
      if (sign_of_special_value(x) == -sign_of_special_value(y))
        return x;
      else { is_exact = false; return bigfloat::NAN_VAL; }
    }
    if (isInf(x)) return x;

    // y has to be infinity 
    return -y;
  }
}


bigfloat add(const bigfloat &x, const bigfloat &y, 
            digit_sz prec, rounding_modes mode, bool& is_exact)
{

//assert(sign(x) >= 0);

  if (x.special || y.special)
    return bigfloat::special_addsub(x,y,1,is_exact);

  if (x.PTR->exp_ptr || y.PTR->exp_ptr)
    return bigfloat::longexp_addsub(x,y,1,prec,mode,is_exact);

  bigfloat_rep *a=x.PTR;
  bigfloat_rep *b=y.PTR;

  int a_sign = a->sign; 
  int b_sign = b->sign;

  bool do_add = true;

  
  bigfloat_rep* s=NULL;
  is_exact=true;
  int cmp=0;

  
  long log_a = (a->exponent<<WORD_LENGTH_LOG)+long(a->bitlength);
  long log_b = (b->exponent<<WORD_LENGTH_LOG)+long(b->bitlength);

  if (a_sign==b_sign) {
    if (log_a >= log_b) {
      if (log_a-log_b <= long(prec+1) || mode==EXACT) 
        s = bigfloat::exact_unsigned_add(a,b);  
      else
        cmp = 1;
    }
    else {
      if (log_b-log_a <= long(prec+1) || mode==EXACT)
        s = bigfloat::exact_unsigned_add(b,a);
      else 
        cmp = -1;
    }
    if (s)
      s->sign = a_sign;
  }
  else {
    if (log_a==log_b) {
      cmp = bigfloat::abs_compare(x,y); 
      if (cmp == 0)
        return bigfloat::PZERO_VAL;
    }
    else 
      cmp = (log_a>log_b?1:-1);

    if (cmp>0) {
      if (log_a-log_b <= long(prec+2) || mode==EXACT) {
        s = bigfloat::exact_unsigned_sub(a,b);
        s->sign = a_sign;
      }
    }
    else {
      if (log_b-log_a <= long(prec+2) || mode==EXACT) {
        s = bigfloat::exact_unsigned_sub(b,a);
        s->sign = b_sign;
      }
    }
  }

  if (s==NULL) {
    
    if (cmp > 0)
      return round(x,prec,mode,is_exact,b_sign);
    else {
      if (do_add)
        return round(y,prec,mode,is_exact,a_sign);
      else
        return round(-y,prec,mode,is_exact,a_sign);
    }




  }
  
  if (mode!=EXACT && prec<s->bitlength) {
    if (mode==TO_NEAREST) 
      RoundNearestDestructive(s,prec,is_exact,0);
    else {
      bigfloat sum(s);
      return round(sum,prec,mode,is_exact,0);
    }
  }
  

  return s;

}

bigfloat sub(const bigfloat &x, const bigfloat &y, 
digit_sz prec, rounding_modes mode, bool& is_exact)
{
  if (x.special || y.special)
    return bigfloat::special_addsub(x,y,-1,is_exact);

  if (x.PTR->exp_ptr || y.PTR->exp_ptr)
    return bigfloat::longexp_addsub(x,y,-1,prec,mode,is_exact);

  bigfloat_rep *a=x.PTR;
  bigfloat_rep *b=y.PTR;

  int a_sign = a->sign; 
  int b_sign = -b->sign;

  bool do_add = false;

  
  bigfloat_rep* s=NULL;
  is_exact=true;
  int cmp=0;

  
  long log_a = (a->exponent<<WORD_LENGTH_LOG)+long(a->bitlength);
  long log_b = (b->exponent<<WORD_LENGTH_LOG)+long(b->bitlength);

  if (a_sign==b_sign) {
    if (log_a >= log_b) {
      if (log_a-log_b <= long(prec+1) || mode==EXACT) 
        s = bigfloat::exact_unsigned_add(a,b);  
      else
        cmp = 1;
    }
    else {
      if (log_b-log_a <= long(prec+1) || mode==EXACT)
        s = bigfloat::exact_unsigned_add(b,a);
      else 
        cmp = -1;
    }
    if (s)
      s->sign = a_sign;
  }
  else {
    if (log_a==log_b) {
      cmp = bigfloat::abs_compare(x,y); 
      if (cmp == 0)
        return bigfloat::PZERO_VAL;
    }
    else 
      cmp = (log_a>log_b?1:-1);

    if (cmp>0) {
      if (log_a-log_b <= long(prec+2) || mode==EXACT) {
        s = bigfloat::exact_unsigned_sub(a,b);
        s->sign = a_sign;
      }
    }
    else {
      if (log_b-log_a <= long(prec+2) || mode==EXACT) {
        s = bigfloat::exact_unsigned_sub(b,a);
        s->sign = b_sign;
      }
    }
  }

  if (s==NULL) {
    
    if (cmp > 0)
      return round(x,prec,mode,is_exact,b_sign);
    else {
      if (do_add)
        return round(y,prec,mode,is_exact,a_sign);
      else
        return round(-y,prec,mode,is_exact,a_sign);
    }




  }
  
  if (mode!=EXACT && prec<s->bitlength) {
    if (mode==TO_NEAREST) 
      RoundNearestDestructive(s,prec,is_exact,0);
    else {
      bigfloat sum(s);
      return round(sum,prec,mode,is_exact,0);
    }
  }
  

  return s;

}


bigfloat_rep* bigfloat::exact_unsigned_add(bigfloat_rep*a,bigfloat_rep*b)
{

  long a_low = a->exponent; 
  long b_low = b->exponent;
#ifdef BIGFLOAT_CHECKS_ON
  long a_high = a_low+a->used;
  long b_high = b_low+b->used;
  assert (a_high >= b_high);
#endif

  digit_sz sum_used; // required number of digits for the sum
  digit_sz sum_size; // the space for the sum
  digit_sz diff_low; // |a_low-b_low|
  bigfloat_rep* s; // the sum
  if (a_low <= b_low) {
    // digit spectrum of a contains that of b
    sum_used = a->used;
    sum_size = 1+digNextPower(sum_used+1);
    s = bigfloat::new_rep(sum_size);
    s->vec[sum_used]=0;
    if (a_low<b_low) {
      diff_low = digit_sz(b_low-a_low);
      vecCopy(s->vec,a->vec,diff_low);
      s->used = diff_low + 
      vecAdd(a->vec+diff_low,a->used-diff_low,b->vec,b->used,s->vec+diff_low);
    }
    else 
      s->used = vecAdd(a->vec,a->used,b->vec,b->used,s->vec);
    s->bitlength = a->bitlength;
    s->exponent  = a->exponent;
  }
  else {
    diff_low = digit_sz(a_low-b_low);
    sum_used = a->used+diff_low;
    sum_size = 1+digNextPower(sum_used+1);
    s = bigfloat::new_rep(sum_size);
    s->vec[sum_used]=0;
    if (diff_low < b->used) {
      vecCopy(s->vec,b->vec,diff_low);
      s->used = diff_low + 
      vecAdd(a->vec,a->used,b->vec+diff_low,b->used-diff_low,s->vec+diff_low);
    }
    else { 
      // digit spectra of a and b are disjoint
      // no addition required
      digit_sz b_used = b->used;
      vecCopy(s->vec,b->vec,b_used);
      if (diff_low > b_used)
        vecInit(s->vec+b_used,diff_low-b_used);
      vecCopy(s->vec+diff_low,a->vec,a->used);
      s->used = sum_used;
    }
    s->bitlength = a->bitlength+(diff_low<<WORD_LENGTH_LOG);
    s->exponent  = a->exponent -long(diff_low);
  }

  if (!vecIsZeroAt(s->vec,s->bitlength+1))
    s->bitlength++;
  bigfloat::normalize_rep(s);

  return s;
}


bigfloat_rep* bigfloat::exact_unsigned_sub(bigfloat_rep*a,bigfloat_rep*b)
{
  long a_low = a->exponent; 
  long b_low = b->exponent;
#ifdef BIGFLOAT_CHECKS_ON
  long a_high = a_low+a->used;
  long b_high = b_low+b->used;
  assert (a_high >= b_high);
#endif

  digit_sz sum_used; // required number of digits for the sum
  digit_sz sum_size; // the space for the sum
  digit_sz diff_low; // |a_low-b_low|
  bigfloat_rep* s; // the sum
  if (a_low <= b_low) {
    // digit spectrum of a contains that of b
    sum_used = a->used;
    sum_size = 1+digNextPower(sum_used);
    s = bigfloat::new_rep(sum_size);
    if (a_low<b_low) {
      diff_low = digit_sz(b_low-a_low);
      vecCopy(s->vec,a->vec,diff_low);
      vecSub(a->vec+diff_low,a->used-diff_low,b->vec,b->used,s->vec+diff_low);
    }
    else {
      vecSub(a->vec,a->used,b->vec,b->used,s->vec);
      s->used = a->used;
    }
    s->bitlength = a->bitlength;
    s->exponent  = a->exponent;
  }
  else {
    diff_low = digit_sz(a_low-b_low);
    sum_used = a->used+diff_low;
    sum_size = 1+digNextPower(sum_used);
    s = bigfloat::new_rep(sum_size);
    vecInit(s->vec,diff_low);
    vecCopy(s->vec+diff_low,a->vec,a->used);
    vecSubFrom(s->vec,a->used+diff_low,b->vec,b->used); 
    s->bitlength = a->bitlength+(diff_low<<WORD_LENGTH_LOG);
    s->exponent  = a->exponent -long(diff_low);
  }
  while (sum_used>0) {
    if (s->vec[sum_used-1])
      break;
    sum_used--;
  }
  if (vecIsZeroAt(s->vec,s->bitlength)) 
    s->bitlength = (sum_used<<WORD_LENGTH_LOG)-digLeadingZeros(s->vec[sum_used-1]);
  s->used = sum_used;
  bigfloat::normalize_rep(s);
  
  return s;
} 



bigfloat bigfloat::longexp_addsub(
     const bigfloat&x,const bigfloat&y,int sign_y,
     digit_sz prec,rounding_modes mode,bool &is_exact) {
  
  integer xexp = x.PTR->exponent;
  integer yexp = y.PTR->exponent;
  if (x.PTR->exp_ptr!=NULL)
    xexp+= *x.PTR->exp_ptr;
  if (y.PTR->exp_ptr!=NULL)
    yexp+= *y.PTR->exp_ptr;
  integer xlog = x.PTR->bitlength+(xexp<<WORD_LENGTH_LOG);
  integer ylog = y.PTR->bitlength+(yexp<<WORD_LENGTH_LOG);
  integer logdiff = (xlog-ylog);
  if (abs(logdiff)>MAXPREC) {
    if (mode == EXACT)
      LEDA_EXCEPTION(1,"bigfloat::addsub: operation too expensive");
    if (xlog > ylog)
      return round(x,prec,mode,is_exact,sign_y*y.PTR->sign);
    else
      return round(sign_y*y,prec,mode,is_exact,x.PTR->sign);
  }
  else {
    bigfloat xnorm = bigfloat(x.get_significant(),0);
    bigfloat ynorm = 
      bigfloat(y.get_significant(),y.get_exponent()-x.get_exponent());
    bigfloat resnorm;
    if (sign_y>0)
      resnorm = add(xnorm,ynorm,prec,mode,is_exact);
    else
      resnorm = sub(xnorm,ynorm,prec,mode,is_exact);
    
    return bigfloat(
     resnorm.get_significant(),resnorm.get_exponent()+x.get_exponent());
  }
}




#define BIGFLOAT_PREC_LIMIT (32<<WORD_LENGTH_LOG)
#define BIGFLOAT_KARA_LIMIT  32

bigfloat mul(const bigfloat& a,const bigfloat& b, digit_sz prec,
             rounding_modes mode, bool& is_exact) {

  
  int sign_result;

  if ((isSpecial(a))||(isSpecial(b)))
  { 
    is_exact = false;
    if ((isNaN(a))||(isNaN(b)))
      return bigfloat(bigfloat::NAN_VAL);
    if ( (isZero(a) && isInf(b)) || (isInf(a) && isZero(b)) )
      return bigfloat(bigfloat::NAN_VAL);

    sign_result = sign_of_special_value(a)*sign_of_special_value(b);

    is_exact = true;
    if (isZero(a) || isZero(b)) 
    {
      if (sign_result == 1)
        return bigfloat(bigfloat::PZERO_VAL);
      else
        return bigfloat(bigfloat::NZERO_VAL);
    }
    if (isInf(a) || isInf(b)) 
    {
      if (sign_result == 1)
        return bigfloat(bigfloat::PINF_VAL);
      else
        return bigfloat(bigfloat::NINF_VAL);
    }
  }



  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;

  digit_sz prod_prec = a.PTR->bitlength+b.PTR->bitlength;
   // the precision of the result can be either prod_prec or prod_prec-1
  digit_sz cutoff_bits = (prod_prec>prec+2?prod_prec-prec-2:0);
   // the number of bits that are rounded away
   // we need two extra bits and two extra digits
  digit_sz cutoff_digits = (cutoff_bits>>WORD_LENGTH_LOG);
   // the number of digits that are rounded away
  digit_sz k = (cutoff_digits>4?cutoff_digits-2:0);
   // the number of trailing digits of the product that we don't compute

  bigfloat_rep* prod;

  if (mode == EXACT || ( prec>BIGFLOAT_PREC_LIMIT 
       && a_used>BIGFLOAT_KARA_LIMIT && b_used>BIGFLOAT_KARA_LIMIT))
    k=0;
    // to make sure that the asymptotical complexity is not spoiled

  if (k>0) {
    digit_sz prod_size = 1+digNextPower(a_used+b_used-k);
    prod = bigfloat::new_rep(prod_size);
    prod->used 
      = vecMulSchoolHigh(a.PTR->vec,a_used,b.PTR->vec,b_used,prod->vec,k);
    digit digOne = prod->vec[1]+a_used;

#ifdef BIGFLOAT_CHECKS_ON
    digit_sz size = 1+digNextPower(a_used+b_used);
    bigfloat_rep* test_prod = bigfloat::new_rep(size);
    test_prod->used 
      = vecMul(test_prod->vec,a.PTR->vec,a_used,b.PTR->vec,b_used);
    digit* exact = new digit[size];
    vecInit(exact+k,size-k);
    vecMulSchoolLow(a.PTR->vec,a_used,b.PTR->vec,b_used,exact,k);
    exact[k+prod->used]=0;
    vecAddTo(exact+k,size-k,prod->vec,prod->used);
    if (exact[k+prod->used])
      prod->used++;
    assert (vecCompare(exact,prod->used+k,test_prod->vec,test_prod->used)==0);
    delete test_prod;
#endif

    if (digOne<=a_used) {
      // approximation by |vecMulSchoolHigh| is not sufficient
      digit_sz size = 1+digNextPower(a_used+b_used);
      bigfloat_rep* exact_prod = bigfloat::new_rep(size);
      vecInit(exact_prod->vec+k,size-k);
      vecMulSchoolLow(a.PTR->vec,a_used,b.PTR->vec,b_used,exact_prod->vec,k);
      vecAddTo(exact_prod->vec+k,size-k,prod->vec,prod->used);
      exact_prod->used = prod->used+k;
      if (exact_prod->vec[prod->used+k-1]==0)
        exact_prod->used++;
      exact_prod->exponent = a.PTR->exponent+b.PTR->exponent;
      bigfloat::delete_rep(prod);
      prod = exact_prod;
    }
    else {
      prod->vec[0]=1;  // this is just to ensure bigfloat consistency
      prod->exponent  = a.PTR->exponent+b.PTR->exponent+long(k);
      prod_prec-=(k<<WORD_LENGTH_LOG);
    }
  }
  else {
    // we compute the exact product
    digit_sz prod_size = 1+digNextPower(a_used+b_used);
    prod = bigfloat::new_rep(prod_size);
    prod->used = vecMul(prod->vec,a.PTR->vec,a_used,b.PTR->vec,b_used);
    prod->exponent = a.PTR->exponent+b.PTR->exponent;
  }

/*
	Note: 
	If prod_prec == i * DIGIT_LENGTH + 1 for some integer i, then 
	prod->used may be equal to i (because the precision of the result may be 
	prod_prec-1, see above).
	Then the precondition of vecIsZeroAt may be violated, because vecMul (in 
	particular Kara_Mult) does not guarantee anymore that prod->vec[prod->used]
	is zero in that case!
	Hence we need the first part of the if-statement below to catch this case!
*/

  if ((prod->used << WORD_LENGTH_LOG) < prod_prec || vecIsZeroAt(prod->vec,prod_prec))
    prod_prec--;
  prod->bitlength = prod_prec;
  prod->sign = a.PTR->sign*b.PTR->sign;

  if (a.PTR->exp_ptr||b.PTR->exp_ptr) {
    integer iexp;
    if (a.PTR->exp_ptr!=NULL)
      iexp+= *a.PTR->exp_ptr;
    if (b.PTR->exp_ptr!=NULL)
      iexp+= *b.PTR->exp_ptr;
    prod->exp_ptr = new integer(iexp);
  }
  bigfloat::normalize_rep(prod);
  if (mode==TO_NEAREST) {
    is_exact=true;
    if (prod->bitlength>prec)
      RoundNearestDestructive(prod,prec,is_exact,0);
    return prod;
  }
  else {
    bigfloat p(prod);
    bigfloat result = round(p,prec,mode,is_exact);
    return result;
 }

}


bigfloat sqr(const bigfloat& a, digit_sz prec, rounding_modes mode, bool &is_exact) 
{
  
if (isSpecial(a)) {
  is_exact = false;
  if (isNaN(a))
    return bigfloat::NAN_VAL;
  is_exact = true;
  if (isZero(a))
    return bigfloat::PZERO_VAL;
  if (isInf(a))
    return bigfloat::PINF_VAL;
}



  digit_sz a_used = a.PTR->used;
  digit_sz prod_size = 1+digNextPower(2*a_used);
  bigfloat_rep* prod = bigfloat::new_rep(prod_size);
  prod->sign = 1;
  prod->used = vecSquare(prod->vec,a.PTR->vec,a_used);
  prod->bitlength = 2*a.PTR->bitlength;
  if (vecIsZeroAt(prod->vec,prod->bitlength))
    prod->bitlength--;

  prod->exponent = (a.PTR->exponent<<1);

  if (a.PTR->exp_ptr) 
    prod->exp_ptr = new integer(2*(*a.PTR->exp_ptr));
  bigfloat p(prod);
  bigfloat result = round(p,prec,mode,is_exact);

  return result;

}


bigfloat div(const bigfloat& a,const bigfloat& b, digit_sz prec,
             rounding_modes mode, bool& is_exact) {

  
if ((isSpecial(a))||(isSpecial(b)))
{
  int sign_result;

  is_exact = false;
  if ((isNaN(a))||(isNaN(b))) 
    return bigfloat(bigfloat::NAN_VAL);
  if (((isZero(a))&&(isZero(b)))||((isInf(a))&&(isInf(b))))
    return bigfloat(bigfloat::NAN_VAL);
  sign_result = sign_of_special_value(a)*sign_of_special_value(b);
  is_exact = true;
  if ((isInf(a))||(isZero(b)))
  { 
    if (sign_result == 1)
      return bigfloat(bigfloat::PINF_VAL);
    else 
      return bigfloat(bigfloat::NINF_VAL);
  }

  // it is clear that isZero(a) || isInf(b) 
  if (sign_result == 1)
    return bigfloat(bigfloat::PZERO_VAL); else 
    return bigfloat(bigfloat::NZERO_VAL);
}


  int min_precdiff = prec+1;
    // the minimal difference in the bitlengths of dividend and divisor
  int act_precdiff = long(a.PTR->bitlength)-long(b.PTR->bitlength);
    // actual difference in the bitlengths of dividend and divisor
  digit_sz shift_digits = 0;
    // number of digits by which we have to shift the dividend
  if (act_precdiff<min_precdiff) {
    shift_digits = 1+((min_precdiff-act_precdiff)>>WORD_LENGTH_LOG);
  }
 
  // the sizes of the shifted numbers involved in the division:

  digit_sz dividend_used = shift_digits+a.PTR->used;
  digit_sz divisor_used  = b.PTR->used;
  digit_sz quotient_size = 1+dividend_used-divisor_used;

  digit_sz tmp_size = 1+digNextPower(dividend_used+divisor_used);
  digit* tmp = new digit[tmp_size];
  digit* dividend  = tmp;
  digit* remainder = tmp+dividend_used;
  digit_sz rem_used;

  vecCopy(dividend+shift_digits,a.PTR->vec,a.PTR->used);
  if (shift_digits)
    vecInit(dividend,shift_digits);

  bigfloat_rep* quot = bigfloat::new_rep(1+digNextPower(quotient_size));
  quot->sign = a.PTR->sign*b.PTR->sign;
  quot->used = vecDiv(quot->vec,remainder,&rem_used,dividend,
    dividend_used,b.PTR->vec,b.PTR->used);

  quot->exponent = a.PTR->exponent-b.PTR->exponent-long(shift_digits);
  quot->bitlength = (shift_digits<<WORD_LENGTH_LOG)+act_precdiff;
  if (!vecIsZeroAt(quot->vec,quot->bitlength+1))
    quot->bitlength++;

#ifdef BIGFLOAT_CHECKS_ON
  assert (quot->bitlength == vecLength(quot->vec,quot->used));
#endif
  
  if (a.PTR->exp_ptr||b.PTR->exp_ptr) {
    integer iexp;
    if (a.PTR->exp_ptr!=NULL)
      iexp+= *a.PTR->exp_ptr;
    if (b.PTR->exp_ptr!=NULL)
      iexp-= *b.PTR->exp_ptr;
    quot->exp_ptr = new integer(iexp);
  }
  bigfloat q(quot);
  int bias = (rem_used>0?a.PTR->sign*b.PTR->sign:0);
  
  if (mode==EXACT) mode=TO_NEAREST;
  bigfloat result = round(q,prec,mode,is_exact,bias);

  delete[] tmp;

  return result;

}

bigfloat sqrt_bf(const bigfloat& a, digit_sz prec, rounding_modes mode, 
  	         bool& is_exact, const bigfloat& start)
{
  if (isNaN(a) || sign(a) < 0) { is_exact = false; return bigfloat::NAN_VAL;  }
  if (isSpecial(a)) { is_exact = true; return a; }

  bigfloat start_value; // the start value of the Newtonian iteration
  digit_sz init_prec=0;     // accuracy of start_value
  bigfloat x;      // returned approximation of sqrt(a)
  long aprec = a.get_effective_significant_length();
  rounding_modes old_mode = bigfloat::set_rounding_mode(EXACT);
  
if (!isSpecial(start) && start > 0) {
  if (aprec > DOUBLE_PREC) {
    bigfloat residue = sqr(start)-a;
    long res_prec = residue.get_effective_significant_length();
    if (res_prec<aprec) 
      init_prec = aprec-res_prec-1;
    if (init_prec > DOUBLE_PREC) 
      start_value = start;
  }
}
if (init_prec <= DOUBLE_PREC) 
{
  long sig_a_len = a.PTR->bitlength;
  long exp_a = a.PTR->exponent;
  long exp_a_shift = a.PTR->exponent<<WORD_LENGTH_LOG;
  long log_a = sig_a_len + exp_a_shift;
  if (   exp_a == (exp_a_shift>>WORD_LENGTH_LOG)
      && a.PTR->exp_ptr==NULL && log_a < 1024 && log_a > -1023 )
  {
    double d = to_double(a);
    start_value = ::sqrt(d);
  }
  else {
    bigfloat tmp = bigfloat(a.get_significant(),-long(sig_a_len));
    double d = to_double(tmp);
    integer log_a = sig_a_len + a.get_exponent();
    if (log_a%2!=0)
      d*= 2;
    start_value = bigfloat(::sqrt(d));
    start_value *= ipow2(log_a>>1);
  }
  init_prec = DOUBLE_PREC;
}

  
  digit_sz count = 0; // number of iteration steps
  digit_sz guard = 3; // extra bits needed such that the accuracy really doubles
  digit_sz cur_prec = init_prec; 
   // the currently used bitlength
  digit_sz accuracy = cur_prec-guard;  
   // the guaranteed precision is always smaller than cur_prec by the guard bits 
  
  digit_sz fin_prec = prec+1;
   // the wanted final precision
  while ((accuracy<<count) < fin_prec) 
    count++;
  digit_sz min_prec = 1+guard+(fin_prec>>count);
   // the minimal precision needed to obtain fin_prec after count iteration steps
  if (min_prec < cur_prec) {
    cur_prec    = min_prec;
    accuracy    = min_prec-guard;
    start_value = round(start_value,min_prec,TO_NEAREST);
  }

  bigfloat it = start_value;
    // the current approximation of sqrt(a)
  bigfloat residue;     
    // the exact residue it*it-a of the current approximation it
  bigfloat one_half = 0.5;
  digit_sz i;
  for(i=0;i<count;i++)
  {
    
    residue = sqr(it)-a; // this is the exact residue
    if (isZero(residue)) {
      is_exact = true;
      bigfloat::set_rounding_mode(old_mode);
      return it;
    }  

    rounding_modes rmode;
      // the rounding mode is important only in the last iteration
    rmode = (i==count-1 ? TO_N_INF : TO_NEAREST); 
    bigfloat ne=one_half*div(residue,it,cur_prec,rmode); 
      // the division is still done in the old precision
    accuracy<<=1;
    cur_prec=accuracy+guard;
      // the subtraction is done in the new precision
    rmode = (i==count-1 ? TO_P_INF : TO_NEAREST); 
    ne=sub(it,ne,cur_prec,rmode);
    it=ne; 
  }
  x = round(it,fin_prec,TO_P_INF);
  
  residue = sqr(it)-a; // this is the exact residue
  if (isZero(residue)) {
    is_exact = true;
    bigfloat::set_rounding_mode(old_mode);
    return it;
  }
  


  bool x_safe = false;
  if (x.PTR->exp_ptr==NULL) {
    //
    // result x is too large by its least significant bit lsb(x) iff
    //   2*lsb(x)*x - lsb(x)^2 >= residue(x)
    // as a necessary condition, we check that 
    //   msb(2*lsb(x)*x) >= msb(residue(x))
    //
    long msb_res = long(residue.PTR->bitlength)
                     -1+(residue.PTR->exponent<<WORD_LENGTH_LOG);
      // the exponent of the most significant bit of residue(x). 
    long msb_x   = long(x.PTR->bitlength)-1+(x.PTR->exponent<<WORD_LENGTH_LOG);
      // the exponent of the most significant bit of x. 
    long lsb_x   = msb_x + 1 - long(fin_prec);
      // the exponent of the least significant bit of x. 
    long msb     = msb_x+lsb_x+1;
      // the exponent of the most significant bit of 2*lsb(x)*x
    if (msb_res<msb)
      x_safe = true;
  }
  if (x_safe==false) {
    // this seems to be very unlikely 
    bool dummy_exact=true;
    bigfloat smaller = round(x,fin_prec,TO_N_INF,dummy_exact,-1); 
    if (sqr(smaller,prec,EXACT,dummy_exact) >= a)
      x = smaller;
  }

  
  switch(mode)
  {
    case TO_NEAREST: 
       x = round(x,prec,TO_ZERO,is_exact);  break;
    case EXACT:
       LEDA_EXCEPTION(1,"bigfloat::sqrt : EXACT mode not provided!");
    case TO_P_INF: case TO_INF: 
       x = round(x,prec,mode,is_exact);  break;
    case TO_N_INF: case TO_ZERO:
       x = round(x,prec,mode,is_exact,-1);
  }
  is_exact = false;

  bigfloat::set_rounding_mode(old_mode);
  return x;
}

bigfloat sqrt_d(const bigfloat& a, digit_sz prec, int d)
{
  if (d < 2)
    LEDA_EXCEPTION(1,"sqrt_d called with d < 2");
  bigfloat result;
  bigfloat start_value;
  
  if (sign(a) < 0) return bigfloat::NAN_VAL;
  if (isSpecial(a)) return a;
  
  int sig_a_len = a.PTR->bitlength;
  bigfloat tmp = bigfloat(a.get_significant(),-sig_a_len);
  double dbl = to_double(tmp);
  integer log_a = long(a.PTR->bitlength)+(a.PTR->exponent<<WORD_LENGTH_LOG);
  long log_a_long = log_a.to_long();
  if (log_a_long%d)
     dbl *= fp::power_two(log_a_long%d);
  start_value = bigfloat(::pow(dbl,1/double(d)));
  start_value *= ipow2(log_a/d);

  
  bigfloat it; // iteration value
  bigfloat ne; // next iteration value

  bigfloat D=d, D_minus_one=d-1;
  bigfloat pow_it;
  int logd = (int) ::ceil(::log10(double(d))/::log10(2.0));
  ne = start_value;
  digit_sz t_prec = 51;
  digit_sz p_bound = prec+1;
  bool it_has_full_precision = false;

  digit_sz old_prec = bigfloat::set_precision(DOUBLE_PREC);
  rounding_modes old_mode = bigfloat::set_rounding_mode(TO_INF);
  while(1==1)
  {
    if (t_prec == p_bound)
       it_has_full_precision = true;
    t_prec*=2; 
    if(t_prec > p_bound) 
       t_prec = p_bound;
    bigfloat::set_precision(t_prec+5);
    it=ne;
    pow_it=powl(it,d-1,t_prec+logd+5,TO_INF);
    ne = (it*D_minus_one+a/pow_it)/D;
    ne = round(ne,prec+2,TO_INF);
    if (t_prec == p_bound)
      if (ne >= it)
         if (it_has_full_precision)
           break;
  }
  bigfloat::set_precision(old_prec);
  bigfloat::set_rounding_mode(old_mode);
  result = round(it,prec+1,TO_NEAREST);
  

#ifdef DEBUG_BIGFLOAT
  if (abs(powl(result,d,prec+20)-a)/(d*a) > ipow2(-prec))  
    LEDA_EXCEPTION(1,"error sqrt_d: precision not achieved");
#endif
  return result;
}

bigfloat power(const bigfloat &x, unsigned long n, 
               digit_sz prec, rounding_modes mode, bool& is_exact) 
// compute x^n
{
  bigfloat z = 1, y = x;
  is_exact = true;
    
  while (n > 0)
  {
    if (n % 2) {
      bool ex;
      z = mul(z,y,prec,mode,ex);
      is_exact &= ex;
    }
    n /= 2;

    bool ex;
    y = sqr(y,prec,mode,ex);
    is_exact &= ex;
  }
  return z;
}


int bigfloat::compare(const bigfloat& x, const bigfloat& y) {

  if (isSpecial(x)||isSpecial(y)) {
    if (isZero(y))
      return sign(x);
    if (isZero(x))
      return -sign(y);
    return sign(x-y);
  }

  int x_sign = x.PTR->sign;
  int y_sign = y.PTR->sign;

  if (x_sign!=y_sign)
    return (x_sign>y_sign?1:-1);

  int res = abs_compare(x,y);
  return (x_sign>0?res:-res);

}

int bigfloat::abs_compare(const bigfloat& x, const bigfloat& y) {

  long x_log = long(x.PTR->bitlength) + (x.PTR->exponent<<WORD_LENGTH_LOG);
  long y_log = long(y.PTR->bitlength) + (y.PTR->exponent<<WORD_LENGTH_LOG);

  if (x.PTR->exp_ptr || y.PTR->exp_ptr) {
    integer xlog = x_log;
    if (x.PTR->exp_ptr) 
      xlog += (*x.PTR->exp_ptr)<<WORD_LENGTH_LOG;
    integer ylog = y_log;
    if (y.PTR->exp_ptr)
      ylog += (*y.PTR->exp_ptr)<<WORD_LENGTH_LOG;
    if (xlog!=ylog)
      return (xlog>ylog?1:-1);
  }
  else
    if (x_log!=y_log) 
      return (x_log>y_log?1:-1);
  
  digit_sz x_used = x.PTR->used;
  digit_sz y_used = y.PTR->used;

  if (x_used==y_used)
    return vecCompare(x.PTR->vec,y.PTR->vec,x.PTR->used);
  else {
    digit_sz cmp_used = (x_used<=y_used?x_used:y_used);
    int cmp = vecCompare(x.PTR->vec+(x_used-cmp_used),
                         y.PTR->vec+(y_used-cmp_used),cmp_used);
    if (cmp!=0)
      return cmp;
    else
      return (x_used>y_used?1:-1);
  }
  
}

  
  bool operator > (const bigfloat &a,const bigfloat &b)
  { return (bigfloat::compare(a,b)>0); }
  bool operator >= (const bigfloat &a,const bigfloat &b)
  { return (bigfloat::compare(a,b)>=0); }
  bool operator < (const bigfloat &a,const bigfloat &b)
  { return (bigfloat::compare(a,b)<0); }
  bool operator <= (const bigfloat &a,const bigfloat &b)
  { return (bigfloat::compare(a,b)<=0); }


bool operator==(const bigfloat &a,const bigfloat &b)
{
  
  if (isSpecial(a) || isSpecial(b))
  {
    if (isZero(a) && isZero(b))
      return true;
    if (isNaN(a) || isNaN(b))
      LEDA_EXCEPTION(1,"bigfloat::operator == : NaN case occurred");
    return (a.special == b.special);
  }




  if (a.PTR->exp_ptr || b.PTR->exp_ptr) {
    bigfloat::normalize_long_exponent(a.PTR,0);
    bigfloat::normalize_long_exponent(b.PTR,0);
    if (a.PTR->exp_ptr == NULL || b.PTR->exp_ptr == NULL)
      return false;
    if (*a.PTR->exp_ptr != *b.PTR->exp_ptr)
      return false;
  }
  
  if (a.PTR->bitlength    != b.PTR->bitlength
       || a.PTR->sign     != b.PTR->sign
       || a.PTR->exponent != b.PTR->exponent)
    return false;

  int cmp = vecCompare(a.PTR->vec,b.PTR->vec,a.PTR->used);
  return (cmp == 0);

}

bool operator != (const bigfloat &a,const bigfloat &b) { return !(a==b); }



  bool isNaN(const bigfloat &x) {  
    return (x.special == bigfloat::NAN_VAL);  
  }
  bool isnInf(const bigfloat &x) {  
    return (x.special == bigfloat::NINF_VAL);   
  } 
  bool ispInf(const bigfloat &x) {  
    return (x.special == bigfloat::PINF_VAL);   
  }
  bool isnZero(const bigfloat &x) {  
    return (x.special == bigfloat::NZERO_VAL);  
  }
  bool ispZero(const bigfloat &x) {  
    return (x.special == bigfloat::PZERO_VAL);  
  }
  bool isZero(const bigfloat &x) { 
    return ((x.special==bigfloat::PZERO_VAL)||(x.special==bigfloat::NZERO_VAL));
  }
  bool isInf(const bigfloat &x) {  
    return ((x.special==bigfloat::PINF_VAL)||(x.special==bigfloat::NINF_VAL)); 
  }
 



  //bigfloat ipow2(const integer &p)  { return bigfloat(1,p);  }
  integer ceil(const bigfloat &x)  { return to_integer(x,TO_P_INF); }
  integer floor(const bigfloat &x) { return to_integer(x,TO_N_INF); }



int sign(const bigfloat &x)
{
  switch(x.special)
  {
    case bigfloat::NOT_VAL:
      return x.PTR->sign;
    case bigfloat::PZERO_VAL:  case bigfloat::NZERO_VAL:  return 0; 
    case bigfloat::PINF_VAL:  return 1; 
    case bigfloat::NINF_VAL:  return -1; 
    case bigfloat::NAN_VAL: LEDA_EXCEPTION(1,"sign: NaN has no sign");
      return 1;
  }
  return 0;
}


bigfloat abs(const bigfloat &x)
{  
   if (isZero(x))
     return bigfloat::PZERO_VAL;
   else
       if (sign(x) > 0)
          return  x;
       else
          return -x;
}

integer ilog2(const bigfloat &x)  
{    
   if (isSpecial(x))
     LEDA_EXCEPTION(1,"ilog2(bigfloat): special value"); 
   integer sig(x.PTR->used,x.PTR->vec);
   unsigned int zeros = (unsigned int) sig.zeros();
   if (x.PTR->bitlength != 1+zeros)
     return long(x.PTR->bitlength) + x.get_exponent();
   else 
     return long(x.PTR->bitlength-1) + x.get_exponent();
}




string bigfloat::to_string(digit_sz dec_prec) const {

  if (isSpecial(*this)) {
    string str = "";
    if (isZero(*this)) 
      str = "0";
    if (ispInf(*this)) 
      str = "pInf";
    if (isnInf(*this)) 
      str = "nInf";
    if (isNaN(*this))
      str = "NaN";
    return str;
  }

  long exp_out;
  char* str_mant = decimal_output(*this,exp_out,dec_prec,TO_NEAREST);
  char* str_exp = 0;
  int len_mant = (int) strlen(str_mant);
  int len_exp=0; 
  char* str_out = new char[len_mant+23];
   // 20 bits for the exponent (64-bit machine) plus bits for sign and 'e' + one for \0
  memcpy(str_out,str_mant,len_mant);
  

  if (exp_out!=0) {
    string num_str = integer(exp_out).to_string();
    str_exp = num_str.cstring();
    len_exp = (int) strlen(str_exp);
    str_out[len_mant]='e';
    if (exp_out>0) {
      str_out[len_mant+1]='+';
      memcpy(str_out+len_mant+2,str_exp,len_exp);
      len_exp++; // to account for the '+'
    }
    else // exp_out < 0 -> the '-' is the first char of str_exp
      memcpy(str_out+len_mant+1,str_exp,len_exp);
	len_exp++; // to account for the 'e'
  }
  str_out[len_mant+len_exp]='\0';
  
  string result(str_out);
  delete[] str_mant;
  delete[] str_out;
  return result;

}



ostream& operator << (ostream& os, const bigfloat& b)
{
  if (isSpecial(b))
  {
    if (isNaN(b))  return os << "NaN";
    if (ispInf(b))  return os << "+Inf";
    if (isnInf(b))  return os << "-Inf";  
    if (ispZero(b))  return os << "0";
    if (isnZero(b))  return os << "-0";
  }
  integer b_significant = b.get_significant();
  int sign_b = sign(b_significant);
  switch(bigfloat::output_mode) {
    case bigfloat::BIN_OUT: case bigfloat::HEX_OUT:
      if (sign_b<0) os << "-";
      if (bigfloat::output_mode==bigfloat::BIN_OUT) {
        if (sign_b>=0) binout(os, b_significant);
        else           binout(os,-b_significant);
      }
      else {
        if (sign_b>=0) hexout(os, b_significant);
        else           hexout(os,-b_significant);
      }
      os << " E ";
      if (b.PTR->exponent<0) os << "-"; else  os << "+";
      os<<b.get_exponent();
      break;
    case bigfloat::DEC_OUT:
      if (b == to_integer(b))
        os << to_integer(b);
      else {
        digit_sz decimal_precision = bigfloat::global_output_prec;
        os << b.to_string(decimal_precision);
      } 
    default:
      break;
  }

  return os;
}


// bugfix by sn  (10/27/2008)
// use leda::string instead of fixed size buffer

//static const digit_sz bin_maxlen=10000;

istream &operator >> (istream &is,bigfloat &b)
{
/*
  char tmp[bin_maxlen];
  is >> tmp;
  string str(tmp);
*/
  string str;
  is >> str;
  b.from_string(str);
  return is;
}



inline bigfloat_rep* bigfloat::allocate_bytes(digit_sz s)
{
#ifdef BIGFLOAT_CHECKS_ON
  bigfloat::obj_count++;
#endif
  return (bigfloat_rep*) std_memory.allocate_bytes(s);
}

inline void bigfloat::deallocate_bytes(bigfloat_rep* p, digit_sz s)
{
#ifdef BIGFLOAT_CHECKS_ON
  bigfloat::obj_count--;
#endif
  std_memory.deallocate_bytes(p,s);
}

bigfloat_rep* bigfloat::new_rep(digit_sz sz) { 
  digit_sz s = sizeof(bigfloat_rep) + (sz-1)*sizeof(digit);

  bigfloat_rep* p = bigfloat::allocate_bytes(s);

  new(p) bigfloat_rep;  // bugfix by s.n. (04/2007)

  p->exp_ptr = NULL;  // fix by sn  (10/2008)

  p->size = sz;
  p->exp_ptr=NULL;
  p->vec=&p->_vec[0];
  return p;
 }

template<class T>
inline void call_destructor(T& x) { x.~T(); }

void bigfloat::delete_rep(bigfloat_rep* p) 
{
  //p->count.~atomic_counter();

  call_destructor(p->count);

  digit_sz s = sizeof(bigfloat_rep) + (p->size-1)*sizeof(digit);
#ifdef BIGFLOAT_CHECKS_ON
  vecFill(p->_vec,p->size,MAX_DIGIT-1);
#endif
  if (p->exp_ptr!=NULL)
    delete p->exp_ptr;
  bigfloat::deallocate_bytes(p,s); 
 }

bigfloat_rep* bigfloat::copy_rep(bigfloat_rep* x) { 
  bigfloat_rep* result = bigfloat::new_rep(x->size);
  result->bitlength = x->bitlength;
  result->exponent = x->exponent;
  result->sign = x->sign;
  result->used = x->used;
  if (result->used)
    vecCopy(result->vec,x->vec,x->used);
  if (x->exp_ptr!=NULL)
    result->exp_ptr = new integer(*x->exp_ptr);
  else
    result->exp_ptr = NULL;
  return result;
}

bigfloat_rep* bigfloat::copy_rep(integer_rep* x) { 
  digit_sz size = x->size;
  if ((size&1U)==0)
    size++;
  bigfloat_rep* result = bigfloat::new_rep(size);
  result->sign = x->sign;
  result->used = x->used;
  if (result->used)
    vecCopy(result->vec,x->vec,x->used);
  return result;
}

LEDA_END_NAMESPACE


// conversion to rational is separated from the rest to avoid unwanted implicit
// conversions from double/integer to rational in the code above!

#include <LEDA/numbers/rational.h>

LEDA_BEGIN_NAMESPACE

rational bigfloat::to_rational() const
{
  integer e = get_exponent();
  if(e<0)
  { 
    bigfloat b_two_to_e = ipow2(-e);
    integer two_to_e = floor(b_two_to_e);
    return rational(get_significant(),two_to_e);
  }

  // e >= 0
  bigfloat b_two_to_e = ipow2(get_exponent());
  integer two_to_e = floor(b_two_to_e);
  return rational(get_significant()* two_to_e);
}

LEDA_END_NAMESPACE
