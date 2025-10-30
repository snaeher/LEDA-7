/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _digit.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/numbers/digit.h>

LEDA_BEGIN_NAMESPACE

digit digPower(digit a, digit_sz n) { 
  digit d=a;
  while (--n) d*=a;
  return d;
}




void digDivThreeHalvesByTwoSpecial
(digit A12, digit a3, digit B, digit b1, digit b2, digit& q, digit& R)
{
#ifdef DIGIT_CHECKS_ON
  assert(HIGH_WORD(A12)==b1);
#endif
  digit A,D;
  
  q = HALF_BASE-1;
  A = (LOW_WORD(A12)<<HALF_LENGTH)+a3; 
  D = q*b2;
  digit T = b1<<HALF_LENGTH;
  R = A-D+T;
  if (R>B)
    {
      q--;
      R+=B;
    }
}

char* digToDecimal(digit x, char* cp)
{
  // create decimal digits from right to left
  for (unsigned int i=0;i<digit_decimal_base_length10;i++)
    {
      digit r=x%10;
      x/=10;
      *--cp  = '0' + char(r);
    }
  return cp;
}

char* digFromDecimal(digit& d, char* cp)
{
  d=0;
  for (unsigned int i=0;i<digit_decimal_base_length10;i++)
    {
      char c=*cp++;
      if (c=='\0') break;
      d=d*10+(c-'0');
    }
  return cp;
} 



bool digIsPrime(digit p) {
  if (p%2 == 0) return false;
  int root = (int) sqrt(double(p));
  int k;
  for (k=3; k<=root; k+=2)
    if (p%k == 0) return false;
  return true;
}

bool digIsPseudoPrime(digit p, digit K) {
  if (p%2 == 0) return false;
  digit root = (unsigned int) sqrt(double(p));
  if (root>K)
    root=K;
  digit k;
  for (k=3; k<=root; k+=2)
    if (p%k == 0) return false;
  return true;
}


digit digGeneratePrimeWithBound(digit a) {
  if (a < 2)
    LEDA_EXCEPTION(1,"there is no prime smaller than 2");
  for(;;) {
   if (digIsPrime(a)) return a;
   a--;
  }
}

digit digGeneratePseudoPrimeWithBound(digit a, digit K) {
  if (a < 2)
    LEDA_EXCEPTION(1,"there is no prime smaller than 2");
  for(;;) {
   if (digIsPseudoPrime(a,K)) return a;
   a--;
  }
}

LEDA_END_NAMESPACE


#include <LEDA/numbers/fp.h>

LEDA_BEGIN_NAMESPACE

int digFromDouble(double x, digit& hi, digit& lo) {

  double frac=x;
  int exp = fp::get_biased_exponent(frac);

  if (exp==0) 
  { frac*=fp::power_two(54);
    exp = fp::get_biased_exponent(frac);
    exp-=54;
   }

  fp::set_unbiased_exponent(frac,0);
  exp-=1023;

#ifdef DIGIT_CHECKS_ON
  assert (x==ldexp(frac,exp));
  assert (0<x);
#endif

#ifdef WORD_LENGTH_32
  frac*=(1UL<<21);
  hi=(digit) frac;  
  lo=(digit) ((frac-hi)*digit_base);
  exp-=53;
#else
  frac*=(1UL<<53);
  hi=0UL;
  lo=(digit)frac;
  exp-=53;
#endif

  return exp;
}

LEDA_END_NAMESPACE

