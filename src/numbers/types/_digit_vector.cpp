/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _digit_vector.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/system/assert.h>
#include <LEDA/numbers/digit_vector.h>
#include <LEDA/numbers/integer.h>


LEDA_BEGIN_NAMESPACE


//static bool USE_KARA_MUL = true; 
static bool USE_KARA_DIV = true; 

#define USE_KARA_MUL integer::use_karatsuba



#if defined(ASM_INTEL_386) && !defined(__ia64__)

  static const digit_sz KARA_MULT_LIMIT     =  16; 
  static const digit_sz KARA_SQUARE_LIMIT   =  32; 

#elif defined(ASM_SPARC) && !defined(__ia64__)

  static const digit_sz KARA_MULT_LIMIT    =  12; 
  static const digit_sz KARA_SQUARE_LIMIT  =  16; 

#else
  static const digit_sz KARA_MULT_LIMIT     =  16; 
  static const digit_sz KARA_SQUARE_LIMIT    =  16; 

#endif


static const digit_sz INNER_DIVISION_LIMIT =  22;  
static const digit_sz OUTER_DIVISION_LIMIT =  26;  



void vecDivSchoolNormal
  (digit* a, digit* b, 
   digit* q, digit* r, digit* T, digit_sz n, digit_sz z_a, digit_sz z_b);

void vecDivKaraRec
  (digit* A, digit* B, 
   digit* Q, digit* R, digit* T, digit_sz n, digit_sz z_a, digit_sz z_b);

void vecDivThreeHalvesByTwo
  (digit* A, digit* B, 
   digit* q, digit* R, digit* T, digit_sz n, digit_sz z_a, digit_sz z_b);
 
void vecDivThreeHalvesByTwoSpecial
  (digit* A, digit* B, 
   digit* q, digit* R, digit* T, digit_sz n);

bool vecModCompareOperationResult(
  digit* c, digit_sz c_used, int c_sign, 
  digit a_mod, digit b_mod, digit& c_mod,
  num_op operation);

bool vecModCompareDivisionResult(
  digit* c, digit_sz c_used, int c_sign, 
  digit a_mod, digit b_mod, digit r_mod, digit& c_mod);


#ifdef vecShift_asm
extern "C" {
digit vecShiftLeftByBits(digit* a, digit *b, digit_sz b_used, digit_sz shift);
digit vecShiftRightByBits(digit* a, digit *b, digit_sz b_used, digit_sz shift);
}

#else

digit vecShiftLeftByBits(digit* a, digit *b, digit_sz b_used, digit_sz shift)
{ 
  int r = DIGIT_LENGTH-shift;
  digit carry = 0;

  for(digit_sz i = 0; i< b_used; i++) 
  { digit B = b[i];
    a[i] = (B << shift) | carry;
    carry = (B >> r);
   }
  return carry; 
}

digit vecShiftRightByBits(digit* a, digit* b, digit_sz b_used, digit_sz shift)
{ 
  int    r = DIGIT_LENGTH-shift;
  digit  carry = 0;

  for(int i=b_used-1; i>=0; i--)
  { digit B = b[i];
    a[i] = (B >> shift) | carry;
    carry = (B << r);
  }

  return carry; 
}

#endif


digit vecShiftLeftCarry(digit* a, digit *b, digit_sz b_used, digit_sz shift)
{  
  if (shift==0)
  { if (a != b) vecCopy(a,b,b_used);
    return 0;
   }
  return vecShiftLeftByBits(a,b,b_used,shift);
}

digit vecShiftRightCarry(digit* a, digit *b, digit_sz b_used, digit_sz shift)
{ 
  if (shift==0)
  { if (a != b) vecCopy(a,b,b_used);
    return 0;
   }
  return vecShiftRightByBits(a,b,b_used,shift);
}




digit_sz vecShiftLeft(digit *a, digit * b, digit_sz b_used, digit_sz shift)
{
  digit_sz bit_shift   = shift % DIGIT_LENGTH;
  digit_sz digit_shift = shift / DIGIT_LENGTH;

  digit_sz a_used = b_used + digit_shift;

  if (bit_shift)
  { digit c = vecShiftLeftCarry(a+digit_shift,b,b_used,bit_shift);
    if (c) a[a_used++] = c;
   }
  else
    memcpy(a+digit_shift,b,b_used*sizeof(digit));

  vecInit(a,digit_shift);

  return a_used;
}




digit_sz vecShiftRight(digit *a, digit * b, digit_sz b_used, digit_sz shift)
{
  digit_sz bit_shift   = shift % DIGIT_LENGTH;
  digit_sz digit_shift = shift / DIGIT_LENGTH;

  digit_sz a_used = b_used - digit_shift;

  if (bit_shift)
    { vecShiftRightCarry(a,b+digit_shift,a_used,bit_shift);
      if (a[a_used-1]==0) a_used--;
     }
  else
    memcpy(a,b+digit_shift,a_used*sizeof(digit));

  return a_used;
}




int vecCompare(digit* a, digit* b, digit_sz n)
{ 
  digit* ap = a + n;
  digit* bp = b + n;
  
  while(ap > a)
  { digit A = *--ap;
    digit B = *--bp;
    if (A > B) return  1;
    if (A < B) return -1;
   }
  return 0;
}




int vecCompareNormal(digit* a, digit_sz a_used, digit* b, digit_sz b_used)
{ 
  if (a_used > b_used) return  1;
  if (a_used < b_used) return -1;

  digit* ap = a + a_used;
  digit* bp = b + b_used;
  while(ap > a)
  { digit A = *--ap;
    digit B = *--bp;
    if (A > B) return  1;
    if (A < B) return -1;
   }
  return 0;
}

int vecCompare(digit* a, digit_sz a_l, digit* b, digit_sz b_l)
{ 
  digit_sz i = a_l;
  digit_sz j = b_l;

  while (i > j)
     if (a[--i]) return 1;

  while (j > i)
     if (b[--j]) return -1;
    
  digit* ap = a + i;
  digit* bp = b + j;
  
  while(ap > a)
  { digit A = *--ap;
    digit B = *--bp;
    if (A > B) return  1;
    if (A < B) return -1;
   }
  return 0;
}

//-----------------------------------------------------------------------------
// addition
//-----------------------------------------------------------------------------

#ifdef vecAdd_asm

extern "C" digit vecAddCarry(digit*,digit*,digit*,digit_sz);

extern "C" digit vecAddCarryC16(digit*,digit*,digit*,digit);

#else

digit vecAddCarry(digit*sum, digit *a, digit *b, digit_sz b_used)
{   
    int carry = 0;

    for (digit_sz i=0; i < b_used; i++)
    { digit aa = a[i];
      digit bb = b[i] + carry; 
      if (bb)           
      { aa += bb;      
        carry = (aa < bb);
       }              
      sum[i] = aa;      
    }

    return carry;
}

#endif



digit_sz vecAdd(digit *a,digit_sz a_used, digit *b,digit_sz b_used, digit* sum)
{ memcpy(sum+b_used,a+b_used,(a_used-b_used)*sizeof(digit));
  sum[a_used] = 0;
  digit_sz i = b_used;
  if (vecAddCarry(sum,a,b,b_used))
  { while (sum[i] == MAX_DIGIT) sum[i++] = 0;
    sum[i++] += 1;
   }
  return (i < a_used) ? a_used : i;
}


/*
digit_sz vecAdd(digit *a,digit_sz a_used, digit *b,digit_sz b_used, digit* sum)
{ memcpy(sum+b_used,a+b_used,(a_used-b_used)*sizeof(digit));
  sum[a_used] = 0;
  digit_sz i = b_used % 16;
  digit c = 0;
  if (i > 0) c = vecAddCarry(sum,a,b,i);
  while (i < b_used) 
  { c = vecAddCarryC16(sum+i,a+i,b+i,c);
    i += 16;
   }

  if (c)
  { while (sum[i] == MAX_DIGIT) sum[i++] = 0;
    sum[i++] += 1;
   }
  return (i < a_used) ? a_used : i;
}
*/


digit_sz vecAddTo(digit *a, digit_sz a_used, digit *b, digit_sz b_used)
{ digit_sz i = b_used;
  if (vecAddCarry(a,a,b,b_used))
  { while (a[i] == MAX_DIGIT) a[i++] = 0;
    a[i++] += 1;
   }
  return (i < a_used) ? a_used : i;
}


digit_sz vecSignedAdd(digit* a, digit_sz a_used, int a_sign,
                      digit* b, digit_sz b_used, int b_sign,
                      digit* sum, int& sum_sign)
{
  if (a_sign == b_sign) 
  { sum_sign = a_sign;
    if (a_used >= b_used)  
      return vecAdd(a,a_used,b,b_used,sum); 
    else 
      return vecAdd(b,b_used,a,a_used,sum); 
  }


  // signs are different

  digit_sz sum_used=0;

  int rel = vecCompareNormal(a,a_used,b,b_used);

  if (rel == 0) 
  { sum_sign=0;
    return 0;
   }

  if (rel > 0 ) 
    { // |a| > |b|
      vecSub(a,a_used,b,b_used,sum);
      sum_used=a_used;
      sum_sign=a_sign;
     }
  else
    { // |b| > |a|
      vecSub(b,b_used,a,a_used,sum);
      sum_used=b_used;
      sum_sign=b_sign;
     }

  digit* sp=sum+sum_used;

  while (*--sp == 0) sum_used--;

  return sum_used;
}





//-----------------------------------------------------------------------------
// subtraction
//-----------------------------------------------------------------------------

#ifdef vecSub_asm

extern "C" digit vecSubCarry(digit*,digit*,digit*,digit_sz);

#else

digit vecSubCarry(digit* diff, digit *a, digit *b, digit_sz b_used)
{   
  int borrow = 0;
  for(digit_sz i=0; i < b_used; i++)
  { digit aa = a[i];
    digit bb = b[i] + borrow; 
    if (bb) borrow = (aa < bb);          
    diff[i] = aa-bb;      
   }
  return borrow;
}

#endif


void vecSub(digit *a, digit_sz a_used, digit *b, digit_sz b_used, digit* diff)
{ memcpy(diff+b_used,a+b_used,(a_used-b_used)*sizeof(digit));
  if (vecSubCarry(diff,a,b,b_used))
  { digit_sz i = b_used;
    while (--diff[i] == MAX_DIGIT) i++;
  }
}

void vecSubFrom(digit *a, digit_sz a_used, digit *b, digit_sz b_used)
{ if (vecSubCarry(a,a,b,b_used))
  { digit_sz i = b_used;
    while (--a[i] == MAX_DIGIT) i++;
   }
}


//-----------------------------------------------------------------------------
// multiplication
//-----------------------------------------------------------------------------

#ifdef digMul_asm

extern "C" digit digMul(digit a, digit b, digit* high);

#else

inline digit digMul(digit a, digit b, digit* high)
  // computes a*b = (high,low) and returns low
{ digit al = LOW_WORD(a);   // low part of a
  digit ah = HIGH_WORD(a);  // high part of a
  digit bl = LOW_WORD(b);   // low part of b
  digit bh = HIGH_WORD(b);  // low part of b
  digit c,L,H;
  c = bl*al;           
  L = LOW_WORD(c);
  c = HIGH_WORD(c) + bl*ah;
  H = HIGH_WORD(c);
  c = LOW_WORD(c)  + bh*al;
  L |= (LOW_WORD(c) << HALF_LENGTH);
  *high =  H + HIGH_WORD(c) + bh*ah;
  return L;
}
#endif



#ifdef vecMul_asm

extern "C" digit vecMulLoopFirst(digit* p, digit* a, digit* a_stop, digit B);
extern "C" digit vecMulInnerLoop(digit* p, digit* a, digit* a_stop, digit B);

extern "C" digit vecMulInnerLoop1(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop2(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop3(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop4(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop5(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop6(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop7(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop8(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop9(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop10(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop11(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop12(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop13(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop14(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop15(digit* p, digit* a, digit B);
extern "C" digit vecMulInnerLoop16(digit* p, digit* a, digit B);

#else

digit vecMulLoopFirst(digit* p, digit* a, digit* a_stop, digit B)
{ 
   digit high;
   digit carry = 0;

   while (a < a_stop)
   { digit P = digMul(*a++,B,&high) + carry; 
     if (P < carry) high++; // carry in addition
     carry = high;
     *p++ = P;
   }
   return *p = carry;
} 


digit vecMulInnerLoop(digit* p, digit* a, digit* a_stop, digit B)
{ 
   digit low,high;
   digit carry = 0;

   while (a < a_stop)
   { digit P = *p;
     low = digMul(*a++,B,&high); 
     P += low;
     if (P < low )  high++; // carry in addition
     P += carry;
     if (P < carry) high++; // carry in addition
     carry = high;
     *p++ = P;
   }
   return *p = carry;
} 


#endif



digit_sz vecMulSchool(digit *a, digit_sz a_used, 
                      digit *b, digit_sz b_used, digit* p) 
{ 
  if (a_used == 0 || b_used == 0) return 0;

  if (a_used == 1 && b_used == 1) { 
    digit high;
    p[0] = digMul(a[0], b[0], &high);
    p[1] = high;
    return (high) ? 2 : 1;
  }

  digit* a_stop = a + a_used;
  digit* b_stop = b + b_used;

  digit c = 0;

  if (b_used == 1)
    c = vecMulLoopFirst(p++,a,a_stop,*b++);
  else
   { memset(p,0,a_used*sizeof(digit));
     while (b < b_stop) c = vecMulInnerLoop(p++,a,a_stop,*b++);
    }

  if (c) 
     return a_used + b_used;
  else
     return a_used + b_used - 1;
}






digit_sz vecMulSchoolHigh(digit *a, digit_sz a_used, 
                          digit *b, digit_sz b_used, digit* prod, digit_sz k) 
{ 
  digit_sz prod_size = a_used + b_used;
  digit_sz prod_used = prod_size-k;

  digit_sz rank_a = a_used-1; // the rank of the highest digit of a

  if (k > rank_a) 
  { int d = k-rank_a; // the first d digits of b do not contribute
    k -= d;
    b += d;             // omit d digits of b
    b_used -= d;        // adapt b_used, new b_used > 0 because k<prod_size-1
   }

  digit_sz rank_b = b_used-1; // the rank of the highest digit of b

  if (k > rank_b) 
  { int d = k-rank_b;
    k -= d;
    a +=d;
    a_used -= d;
  }

  // at this point we have k <= min{a_used-1,b_used-1}
  // next ensure that b_used <= a_used

  if (a_used < b_used) { 
    swap(a,b);
    swap(a_used,b_used);
  }
  
  digit* a_stop  = a + a_used;
  digit* b_stop  = b + b_used;
  digit* a_start = a + k;

  vecInit(prod,digit_sz(a_stop-a_start));

  while (k--) vecMulInnerLoop(prod,a_start--,a_stop,*b++);

  assert (b < b_stop);

  digit c = 0;
  while (b < b_stop) c = vecMulInnerLoop(prod++,a,a_stop,*b++);

  if (c) 
    return prod_used;
  else
    return prod_used-1;   
}


void vecMulSchoolLow(digit *a, digit_sz a_used, 
                     digit *b, digit_sz b_used, digit* prod, digit_sz k) 
{ 
  // first init regions that is never touched later on

  digit_sz l = a_used + b_used;

  if (l < k) vecInit(prod+l,k-l);

  if (a_used > k) a_used=k;
  if (b_used > k) b_used=k;

  // next ensure that b_used <= a_used

  if (a_used < b_used) { 
    swap(a,b);
    swap(a_used,b_used);
  }

  digit* a_stop  = a + a_used;
  digit* b_stop  = b + b_used;
  digit* p_start = prod;

  vecInit(prod,a_used);

  // first do normal School multiplication until 
  // the first digit product of rank |k-1| is used 

  for (digit_sz i=0;i<=k-a_used;i++)
      vecMulInnerLoop(p_start++,a,a_stop,*b++);

  // next add remaining rows
  // the highest digit has to be saved manually
  // because |vecMulInnerLoop| destroys it

  digit hi = prod[k];
  digit carry = 0;
  prod[k+1] = 0;

  while (b < b_stop) 
  { digit cur = vecMulInnerLoop(p_start++,a,--a_stop,*b++);
    hi += cur;
    if (hi < cur) carry++;
  }

  prod[k]=hi;
  prod[k+1]=carry;
}



void Karatsuba(digit* p, digit* a, digit* b, digit* tmp, digit_sz n)
{       
  if (n <= KARA_MULT_LIMIT || n&1)
  { 
    vecMulLoopFirst(p++,a,a+n,*b++);

#if defined(vecMul_asm)

    switch (n) {


    case  1: while(--n) vecMulInnerLoop1(p++,a,*b++);
             break;
    case  2: while(--n) vecMulInnerLoop2(p++,a,*b++);
             break;
    case  3: while(--n) vecMulInnerLoop3(p++,a,*b++);
             break;
    case  4: while(--n) vecMulInnerLoop4(p++,a,*b++);
             break;
    case  5: while(--n) vecMulInnerLoop5(p++,a,*b++);
             break;
    case  6: while(--n) vecMulInnerLoop6(p++,a,*b++);
             break;
    case  7: while(--n) vecMulInnerLoop7(p++,a,*b++);
             break;
    case  8: while(--n) vecMulInnerLoop8(p++,a,*b++);
             break;
    case  9: while(--n) vecMulInnerLoop9(p++,a,*b++);
             break;
    case 10: while(--n) vecMulInnerLoop10(p++,a,*b++);
             break;
    case 11: while(--n) vecMulInnerLoop11(p++,a,*b++);
             break;
    case 12: while(--n) vecMulInnerLoop12(p++,a,*b++);
             break;
    case 13: while(--n) vecMulInnerLoop13(p++,a,*b++);
             break;
    case 14: while(--n) vecMulInnerLoop14(p++,a,*b++);
             break;
    case 15: while(--n) vecMulInnerLoop15(p++,a,*b++);
             break;
    case 16: while(--n) vecMulInnerLoop16(p++,a,*b++);
             break;

    default: { digit* a_stop = a+n;
               while(--n) vecMulInnerLoop(p++,a,a_stop,*b++);
               break;
              }
    }
#else
    { digit* a_stop = a+n;
      while(--n) vecMulInnerLoop(p++,a,a_stop,*b++);
     }
#endif

    return;
   }


  assert((n&1) == 0);

  int n2 = n/2;
  int n3 = n+n/2;

  int a_sign = vecCompare(a+n2,a,n2);
  int b_sign = vecCompare(b,b+n2,n2);


  if (a_sign >= 0)
     vecSubCarry(p+n,a+n2,a,n2);   // prod[n..3n/2-1] := a1 - a0
  else 
     vecSubCarry(p+n,a,a+n2,n2);   // prod[n..3n/2-1] := a0 - a1

  if (b_sign >= 0)
     vecSubCarry(p+n3,b,b+n2,n2);   // prod[3n/2..2n-1] := b0 - b1
  else
     vecSubCarry(p+n3,b+n2,b,n2);   // prod[3n/2..2n-1] := b1 - b0


  Karatsuba(tmp,p+n,p+n3,tmp+n,n/2);   // tmp[0..n-1]   := |a1-a0|*|b0-b1|
  Karatsuba(p,a,b,tmp+n,n/2);          // prod[0..n-1]  := a0 * b0 
  Karatsuba(p+n,a+n2,b+n2,tmp+n,n/2);  // prod[n..2n-1] := a1 * b1


  digit c = 0;

  if (a_sign == b_sign)  
    c += vecAddCarry(tmp,p,tmp,n);   //tmp[0..n-1] = prod[0..n-1]+tmp[0..n-1]
  else
    c -= vecSubCarry(tmp,p,tmp,n);   //tmp[0..n-1] = prod[0..n-1]-tmp[0..n-1]

  c += vecAddCarry(tmp,tmp,p+n,n);   //tmp[0..n-1] = tmp[0..n-1] +prod[n..2n-1]

  c += vecAddCarry(p+n2,p+n2,tmp,n); //prod[n/2..3n/2-1] += tmp[0..n-1] 


  digit x = p[n3] + c;
  p[n3] = x;
  if (x >= c) return;

  int i = n3+1;
  while (p[i] == MAX_DIGIT) p[i++] = 0;
  p[i]++;
}


digit_sz Kara_Mult(digit* a, digit_sz a_used, digit* b, digit_sz b_used, 
                                                        digit* prod)
{
/*
	NOTE: We cannot assume that a/b/prod point to a memory chunk of size
	digNextPower(a_used/b_used/a_used + b_used).
	(E.g. KaraMult may be called via vecMult from vecDivThreeHalvesByTwoSpecial!)
	So if we need larger chunks than a_used/b_used/a_used+b_used, we allocate new
	memory and copy ...
*/

  // a_used >= b_used
  assert(a_used >= b_used);

  digit_sz p_used_orig = a_used + b_used;

//Let n = i*2^k with n >= b_used and KARA_LIMIT/2 < i <= KARA_LIMIT 

  digit_sz n   = b_used;
  digit_sz pot = 1;

  while (n > KARA_MULT_LIMIT) { n = (n+1)/2; pot *= 2; }

  n *= pot; 

  // extend length of b to n (append zeroes) 
  digit* bp = b;
  if (n > b_used)
  { bp = new digit[n];
    vecCopy(bp, b, b_used);
    vecInit(bp + b_used, n-b_used);
  }

  // partition a into t intervals of size n each
  // r is size of remainder

  unsigned t = a_used / n;
  unsigned r = a_used % n;

  digit_sz m = t*n;

  digit* ap = a;
  if (r > n/2) // make length(a) = t*n  (a muliple of n)
  { t++;
    m += n;
    r = 0;
    if (m > a_used)
    { ap  = new digit[m];
      vecCopy(ap, a, a_used);
      vecInit(ap + a_used, m-a_used);
      a_used = m;
    }
  }
  //assert(m <= a_used);

  digit* res = prod; // result
  digit_sz p_used = a_used + n;
  if (p_used_orig < p_used) res = new digit[p_used]; 
  vecInit(res, p_used);

  digit* buffer = new digit[4*n];

  digit* tpr = buffer;            // temporary product
  digit* tmp = buffer + 2*n;      // temporary space used by Karatsuba

  digit* q = ap;    // current position in a
  digit* p = res;   // current position in result

  while (t--)
  { Karatsuba(tpr, q, bp, tmp, n);    // tpr := q[0..n-1] * b[0..n-1]
    vecAddCarry(p,p,tpr,2*n);        // add tpr to result
    q += n;
    p += n;
   }

  while (r--) vecMulInnerLoop(p++,bp,bp+b_used, *q++);

  while (res[p_used-1] == 0) p_used--;      // adjust p_used

  if (res != prod)
  { vecCopy(prod,res,p_used);      // copy result to prod
    delete[] res;
   }

  if (ap != a) delete[] ap;
  if (bp != b) delete[] bp;
  delete[] buffer;

  return p_used; 
}
  
  

digit_sz vecMul(digit* prod, digit* a, digit_sz a_used,
                             digit* b, digit_sz b_used)
{
  if (a_used == 0 || b_used == 0) return 0;

  if (a_used == 1 && b_used == 1) 
  { *prod = digMul(a[0], b[0], prod+1);
    return (prod[1]) ? 2 : 1;
   }

  if (a_used < b_used) 
  { swap(a,b);
    swap(a_used,b_used);
   }

  if (USE_KARA_MUL && b_used > KARA_MULT_LIMIT) 
    return Kara_Mult(a, a_used, b, b_used, prod);
  else 
    return vecMulSchool(a, a_used, b, b_used, prod);
}


//-----------------------------------------------------------------------------
// square
//-----------------------------------------------------------------------------

#ifdef vecSqr_asm

extern "C" digit vecSqrInnerLoop(digit* s, digit* a, digit* a_stop);
extern "C" digit vecSqrInnerLoop_s(digit* s, digit* a, digit* a_stop);

#else

digit vecSqrInnerLoop(digit* square, digit* a, digit* a_stop)
{ 
  //vecShiftLeftCarry(square,square,2*(a_stop-a),1);

  digit carry=0;

  digit* s=square;

  while(a<a_stop) 
  { digit high;
    digit low=digMul(*a,*a,&high);

    digit aa=*s;

    digit bb=low + carry;
    if(bb) { aa+= bb; carry=(aa<bb); }
    *s++ = aa;

    aa=*s;
    bb=high + carry;
    if(bb) { aa+= bb; carry=(aa<bb); }
    *s++ = aa;

    a++;
  }

  return carry;
}

#endif




digit_sz vecSquareSchool(digit *a, digit_sz a_used, digit *square)
{
  if (a_used==0) return 0;

  digit_sz s_used= 2*a_used;

  vecInit(square,s_used);

  digit* s = square+1; 
  digit* ap = a;
  digit* a_stop = a+a_used;
  digit* a_last = a_stop-1;

#if !defined(vecMul_asm)

  while (ap < a_last)
  { vecMulInnerLoop(s,ap+1,a_stop,*ap);
    ap++;
    s += 2;
   }

#else

  if (a_used > 16)
  {
    while (ap < a_last)
    { vecMulInnerLoop(s,ap+1,a_stop,*ap);
      ap++;
      s += 2;
     }
   }
  else
  { 
    switch(a_used) {
     case 16: vecMulInnerLoop15(s,ap+1,*ap); s += 2; ap++;
     case 15: vecMulInnerLoop14(s,ap+1,*ap); s += 2; ap++;
     case 14: vecMulInnerLoop13(s,ap+1,*ap); s += 2; ap++;
     case 13: vecMulInnerLoop12(s,ap+1,*ap); s += 2; ap++;
     case 12: vecMulInnerLoop11(s,ap+1,*ap); s += 2; ap++;
     case 11: vecMulInnerLoop10(s,ap+1,*ap); s += 2; ap++;
     case 10: vecMulInnerLoop9(s,ap+1,*ap); s += 2; ap++;
     case  9: vecMulInnerLoop8(s,ap+1,*ap); s += 2; ap++;
     case  8: vecMulInnerLoop7(s,ap+1,*ap); s += 2; ap++;
     case  7: vecMulInnerLoop6(s,ap+1,*ap); s += 2; ap++;
     case  6: vecMulInnerLoop5(s,ap+1,*ap); s += 2; ap++;
     case  5: vecMulInnerLoop4(s,ap+1,*ap); s += 2; ap++;
     case  4: vecMulInnerLoop3(s,ap+1,*ap); s += 2; ap++;
     case  3: vecMulInnerLoop2(s,ap+1,*ap); s += 2; ap++;
     case  2: vecMulInnerLoop1(s,ap+1,*ap); s += 2; ap++;
     }
   }
#endif


  digit carry = vecShiftLeftCarry(square,square,s_used,1);
  assert(carry==0);
  carry = vecSqrInnerLoop(square,a,a_stop);
  assert(carry==0);

  if(square[s_used-1]==0) s_used--;

  return s_used;
}




void KaratsubaSquare(digit* p, digit* a, digit* tmp, digit_sz n)
{       

  if (n <= KARA_SQUARE_LIMIT)
  { vecSquareSchool(a,n,p);
    return;
   }

  assert((n&1) == 0); 

  int n2 = n/2;
  int n3 = n+n/2;

  int a_sign = vecCompare(a+n2,a,n2);

  if (a_sign >= 0)
     vecSubCarry(p+n,a+n2,a,n2);   // prod[n..3n/2-1] := a1 - a0
  else 
     vecSubCarry(p+n,a,a+n2,n2);   // prod[n..3n/2-1] := a0 - a1

  KaratsubaSquare(tmp,p+n,tmp+n,n/2);   // tmp[0..n-1]   := |a1-a0|*|a1-a0|
  KaratsubaSquare(p,a,tmp+n,n/2);       // prod[0..n-1]  := a0 * a0 
  KaratsubaSquare(p+n,a+n2,tmp+n,n/2);  // prod[n..2n-1] := a1 * a1


  digit c = 0;

  c -= vecSubCarry(tmp,p,tmp,n);     //tmp[0..n-1] = prod[0..n-1]-tmp[0..n-1]
  c += vecAddCarry(tmp,tmp,p+n,n);   //tmp[0..n-1] = tmp[0..n-1] +prod[n..2n-1]
  c += vecAddCarry(p+n2,p+n2,tmp,n); //prod[n/2..3n/2-1] += tmp[0..n-1] 


  digit x = p[n3] + c;
  p[n3] = x;
  if (x >= c) return;

  int i = n3+1;
  while (p[i] == MAX_DIGIT) p[i++] = 0;
  p[i]++;
}


digit_sz Kara_Square(digit* a, digit_sz a_used, digit* prod)
{ 
  digit_sz a_size = digNextPower(a_used);
  digit_sz p_size = digNextPower(2*a_used);

  digit_sz n   = a_used;
  digit_sz pot = 1;

  while (n > KARA_SQUARE_LIMIT) { n = (n+1)/2; pot *= 2; }

  n *= pot; 

  if (n > a_used)
  { assert(n <= a_size);
    vecInit(a+a_used,n-a_used);
   }

  digit_sz p_used = 2*n;

  digit* p = prod;
  if (p_size < p_used) p = new digit[p_used]; 
  vecInit(p,p_used);

  digit* tmp = new digit[2*n];

  KaratsubaSquare(p,a,tmp,n);

  while (p[p_used-1] == 0) p_used--;      // adjust p_used

  if (p != prod)
  { vecCopy(prod,p,p_used);      // copy result to prod
    delete[] p;
   }

  delete[] tmp;

  return p_used; 
}
  

digit_sz vecSquare(digit* prod, digit* a, digit_sz a_used)
{
  if (a_used == 0) return 0;

  if (a_used == 1) 
  { *prod = digMul(a[0], a[0], prod+1);
    return (prod[1]) ? 2 : 1;
   }

  if (USE_KARA_MUL && a_used > KARA_SQUARE_LIMIT) 
     return Kara_Square(a,a_used,prod);
  else 
     return vecSquareSchool(a,a_used,prod);
}




//-----------------------------------------------------------------------------
// division
//-----------------------------------------------------------------------------


#ifdef vecDiv_asm

extern "C" digit  vecDivInnerLoop(digit* p, digit* a, digit* a_stop, digit B);

#else

digit vecDivInnerLoop(digit* p, digit* a, digit* a_stop, digit B)
{ 
   digit low,high;
   digit carry = 0;
   digit P;
   while (a < a_stop)
   { P = *p;
     low = digMul(*a++,B,&high); 
     if (P < low) high++;   // borrow 
     P -= low;
     if (P < carry) high++; // borrow 
     P -= carry;
     *p++ = P;
     carry = high;
   }
   P = *p;
   *p = P - carry;
  
   return (P < carry);
}
#endif


void vecDivLoop(digit* a, digit_sz a_used, digit* b, digit_sz b_used, digit* Q)
{
  digit* a_ptr =a+(a_used-1);
  digit* Q_ptr =Q+(a_used-b_used-1);
  digit* b_stop=b+b_used;

  if (a_used <= b_used)
    LEDA_EXCEPTION(1,"vecDivLoop Error");
  
  digit b1 = b[b_used-1];               
  digit b2 = b[b_used-2];
  digit q,a1,a2,a3;
 
  while (Q_ptr >= Q) // quotient will have at most a_used-b_used digits
  {
    q = MAX_DIGIT;
    a1 = *a_ptr;
    a2 = *(a_ptr-1);
    a3 = *(a_ptr-2);

    if (a1 != b1)
      q = digDivThreeByTwoNormal(a1,a2,a3,b1,b2); // else q is already correct

    if (vecDivInnerLoop(a_ptr-b_used,b,b_stop,q)) // a -= b*q
    { // if there is a borrow left, add b back to a and decrement q  
      digit* ap;
      digit* bp;
      bool  carry = false;
      for(bp = (digit*) b, ap = a_ptr-b_used; bp < b_stop; bp++, ap++) 
      { digit aa = *ap;
        digit bb = *bp;
        bb += aa;
        if (carry)
          { if (++bb > aa) carry = false; }
        else
            if (bb < aa) carry = true;
        *ap = bb;
       }
      if (carry) (*ap)++; 
      q--;
    }

    *Q_ptr-- = q;
    a_ptr--;
  }
}


digit_sz vecDivSchool(digit* A, digit_sz a_used, digit* B, digit_sz b_used,
                        digit* Q, digit* R, digit_sz* R_used)
{
  if (b_used == 0) LEDA_EXCEPTION(1,"error vecDivSchool: Divisor is zero");

  if (b_used == 1) 
  { digit_sz q_size = vecDivByOneDigit(A,a_used,*B,Q,R);
    if (*R) *R_used=1; else *R_used=0;
    return q_size;
  }

  digit* a = new digit[a_used + 1];
  digit* b = new digit[b_used];

  digit_sz quot_used = a_used - b_used + 1;

  // we first normalize A and B

  digit sh = B[b_used-1];
  digit_sz shift = digLeadingZeros(sh);
  a[a_used] = vecShiftLeftCarry(a, A, a_used, shift);
  vecShiftLeftCarry(b, B, b_used, shift);

  vecDivLoop(a,a_used+1,b,b_used,Q);
  
  if (R)  // copy remainder to R[]
  {
    if (shift)
      vecShiftRightCarry(R,a,b_used,shift);
    else
      vecCopy(R,a,b_used);

    digit* q;
    for(q = R+b_used-1; b_used>0 && *q==0; q--)  b_used--; 

    *R_used = b_used;
   }


  delete[] a;
  delete[] b;

  if (Q[quot_used-1] == 0) {
    quot_used--;
  }
    
  return quot_used;
}

digit_sz vecDivByOneDigit(digit *a, digit_sz a_used, digit B, digit* quot, 
                                                              digit* rem)
{
  digit  r = 0;
  digit* a_ptr = a + a_used;
  digit* q_ptr = quot + a_used;

  while (a_ptr > a) r = digDivTwoByOne(r,*--a_ptr,B,--q_ptr);

  if (quot[a_used-1] == 0) a_used--;
  if (rem) *rem = r;
  return a_used;
}

void vecDivKaraRec(digit* A, digit* B, digit* Q, digit* R, digit* T, 
                   digit_sz n, digit_sz z_a, digit_sz z_b)
{
#ifdef DIGIT_CHECKS_ON
  assert(A[2*n-1]<=B[n-1]);
#endif
  if (n%2 || n <= INNER_DIVISION_LIMIT+z_a)
    vecDivSchoolNormal(A,B,Q,R,T,n,z_a,z_b); // base of the recursion
  else
    {
      digit_sz half_n=n/2;
      digit *q1=Q+half_n;  // here goes the upper part of the quotient
      digit *q2=Q;         // here the lower one
      digit *A1=A+half_n;  // position of the first 3 halves
      digit *A2=T+2*n+half_n; // temporary position of the second 3 halves
      digit *R1=T+3*n;        // first temporary remainder

      if (z_a<=half_n) // general case
      {
        vecDivThreeHalvesByTwo(A1,B,q1,R1,T,n,z_a,z_b); // first recursion
        vecCopy(A2,A,half_n);                           // create [r1,r2,a4]
        vecDivThreeHalvesByTwo(A2,B,q2,R,T,n,0,z_b);    // second recursion
      }
      else // q1 is zero and a2<b1; first recursion obsolete
        vecDivThreeHalvesByTwo(A,B,q2,R,T,n,z_a-half_n,z_b);  
        // only second recursion
        // q1 is assumed to be preset to 0 by the caller
    }
}

void vecDivSchoolNormal(digit* A, digit* B, digit* Q, digit* R, digit* T, 
                        digit_sz n, digit_sz z_a, digit_sz z_b)
{
  digit* a=T;            // a starts at the beginning of the temporary space
  vecCopy(a,A,2*n);            // copy A to a
  vecInit(Q+(n-z_a),z_a);      // set upper z_a digits of Q to zero
  digit_sz a_used = 2*n-(z_a+z_b); // number of nonzero digits of A
  digit_sz b_used = n-z_b;         // number of nonzero digits of B
  if (a_used == b_used)
    a_used++;            // caution: a_used > b_used needed for vecDivLoop
  vecDivLoop(a+z_b,a_used,B+z_b,b_used,Q); 
                             // divide A by B in temporary space
  vecCopy(R,a,n);            // copy remainder to R
}


void vecDivThreeHalvesByTwo(digit* A, digit* B, digit* q, digit* R, digit* T, 
                            digit_sz n, digit_sz z_a, digit_sz z_b) 
{


  digit_sz half_n=n/2;

  digit* A12=A+half_n; // position of [a1,a2]
  digit* a1 =A+n;      // position of [a1]
  digit* a3 =A; 
  digit* b1 =B+half_n;
  digit* b2 =B;
  digit* r1 =R+half_n; // here goes the upper part of the remainder
  digit* r2 =R;        // here the lower one
  digit* D=T+n;        // here goes the intermediate product
  

  if (vecCompare(a1,half_n,b1,half_n)==0)
    vecDivThreeHalvesByTwoSpecial(A,B,q,R,T,n);
  else
    {
      vecDivKaraRec(A12,b1,q,r1,T,half_n,z_a,0); 
                                 // q = [a1,a2]/b1 with remainder r1 

      vecCopy(r2,a3,half_n);     // create R=(r1<<half_n)+a3
      if (z_b) {  // in this case we can forget the lower z_b digits of D,R,B
         D += z_b; 
         R += z_b; 
         B += z_b; 
         n -= z_b; 
         b2 +=z_b;
      }

      // vecMulCallKaraRec(D,q,b2,T,half_n,z_a,z_b); // D = q*b2

      Karatsuba(D-z_b,q,b2-z_b,T,half_n);


      digit borrow =vecSubCarry(R,R,D,n);    // R=R-D

      if (borrow) {                          // is R < 0?
        vecDecrement(q,half_n);              // q--
        vecAddCarry(R,R,B,n);                // R = R+B
        if( vecCompare(R,n,B,n)>0) {         // is R < 0 yet?!
          vecDecrement(q,half_n);            // q-- again
          vecAddCarry(R,R,B,n);              // R = R+B
        }
      }
    }
}

void vecDivThreeHalvesByTwoSpecial(digit* A, digit* B, digit* q, digit* R, 
                                                       digit* T, digit_sz n) 
{
  digit_sz half_n=n/2;
  digit* D =T+n;
  digit* b1=B+half_n;
  digit* b2=B;

  memset(q,0xff,half_n*sizeof(digit));

/*
  vecFill(q,half_n,MAX_DIGIT);   // q=2^(n*DIGIT_LENGTH)-1
*/

  vecCopy(R,A,n);                // R=[a2,a3]

/*
  if (half_n%2==0)               // D=q*b2 using T as temp
    vecMulCallKaraRec(D,q,b2,T,half_n);      
  else
    vecMulSchool(q,half_n,b2,half_n,D);
*/

  vecMul(D,q,half_n,b2,half_n);

  vecInit(T,half_n);             // fill T with 0    
  vecCopy(T+half_n,b1,half_n);   // create T=[b1,0]

  vecSubCarry(R,R,D,n);               // R=R-q*b2
  vecAddCarry(R,R,T,n);             // R=R+[b1,0]
  if(vecCompare(R,n,B,n)>0 ) {
    vecDecrement(q,half_n);         // q--;
    vecAddCarry(R,R,B,n);           // R+=B
  }

}

static digit_sz compute_block_size(digit_sz a_used, digit_sz b_used)
{
  digit_sz div_complexity = b_used;
   // determines the complexity of the division operation
  if (2*b_used>a_used)
    div_complexity = a_used-b_used;

  digit_sz kara_blocks = div_complexity/KARA_MULT_LIMIT;
  if (div_complexity%KARA_MULT_LIMIT)
     kara_blocks++;
  return digNextPower(kara_blocks);
}
 
digit_sz vecDivKara(digit* A, digit_sz a_used, digit* B, digit_sz b_used,
                                     digit* Q, digit* R, digit_sz* R_used)
{
  digit_sz n,m,j;
  m = compute_block_size(a_used,b_used);

  // first compute n = (2^k)*j
  j=b_used/m;
  if(b_used % m !=0) j++; // round j up
  n=m*j;

  // next compute the left-shift necessary to normalize b
  digit_sz shift = digLeadingZeros(B[b_used-1]);
  digit_sz extend_digits = n-b_used; // by how many digits is b extended?

  // compute t = ceil((a_used+extend_digits+1)/n)
  digit_sz new_a_used = a_used+extend_digits+1;
  digit_sz t = new_a_used / n;  // number of division blocks of a
  digit_sz r = new_a_used % n;  // number of digits in the last block
  if (r!=0) t++; // round t up
  // note: t>=2 here since a_used >= b_used and our choice of new_a_used

  // create vectors a of size t*n, b of size n
  digit_sz new_a_size = t*n;
  digit*a= new digit[new_a_size];   // shifted dividend
  digit*b= new digit[n];            // shifted divisor
  vecInit(a,extend_digits);  // a has now extend_digits 0s at the end
  vecInit(b,extend_digits);  // b has now extend_digits 0s at the end
  vecShiftLeftCarry(b+extend_digits,B,b_used,shift);             
    // create shifted B
  digit carry=vecShiftLeftCarry(a+extend_digits,A,a_used,shift); 
    // create shifted A
  a[new_a_used-1]=carry;        // write carry to a
  if (!carry && a[new_a_used-2]<b[n-1])
    new_a_used--;
  // now we have ensured that q_used <= new_a_used-n 

#ifdef DIGIT_CHECKS_ON
  assert(a[new_a_used-1]<b[n-1]);
#endif
    
  digit_sz z_a = new_a_size-new_a_used; // number of zeros of a from left
  digit_sz z_b = n-b_used;
  vecInit(a+new_a_used,z_a);        // fill a with z 0s from left
  
  digit* aa  = new digit[2*n];      // temporary for dividend
  digit* q   = new digit[(t-1)*n];  // temporary for resulting quotient
  digit* qq  = q+(t-2)*n;           // the result's part that we compute now
  digit* rem = new digit[n];        // temporary for the remainder
  digit* tmp = new digit[4*n];      // temporary for vecDivKaraRec
  vecInit(tmp,4*n);

  vecCopy(rem,a+(t-1)*n,n); // create first remainder = leftmost block of A
  vecInit(q,(t-1)*n);       // initialize temporary quotient with zero
  while(--t) { // for every block do
    vecCopy(aa+n,rem,n);                // old remainder  -> high part of aa
    vecCopy(aa,a+(t-1)*n,n);            // next part of a -> low part  of aa
    vecDivKaraRec(aa,b,qq,rem,tmp,n,z_a,z_b); 
                                           // qq = aa/b with remainder rem
    qq-=n;                                 // proceed to next lower part of q
    z_a=0;                          // after the first call of Recursive Division 
                                    // we don't know the leading zeros anymore
  }
  // rem is now the shifted remainder R (not exact)
  // q   is now the exact result Q

  digit_sz quot_used = a_used-b_used+1;     // quot_used <= (t-1)*n
  vecCopy(Q,q,quot_used);               // the quotient needs no back-shift
  if (Q[quot_used-1] == 0) quot_used--; // afterwards Q[quot_used-1] !=0
  
  if (R) // if R is to be computed
    {
      vecShiftRightCarry(R,rem+extend_digits,b_used,shift); // shift back
      digit* ptr; // run with *ptr through remainder
      digit_sz r_used=b_used; // counting how many digits are used
      for(ptr = R+r_used-1; r_used>0 && *ptr==0; ptr--)  r_used--; 
      *R_used = r_used;
    }

  // delete temporary arrays
  delete[] tmp;
  delete[] rem;
  delete[] q;
  delete[] aa;
  delete[] b;
  delete[] a;
  
  return quot_used;
}


digit_sz vecDiv(digit* quot, digit* r, digit_sz *r_used,
            digit* a_vec, digit_sz a_used,
            digit* b_vec, digit_sz b_used)
{ 
#ifdef DIGIT_CHECKS_ON
  if (a_used>0) assert(a_vec[a_used-1]!=0);
  if (b_used>0) assert(b_vec[b_used-1]!=0);
#endif
  // int rel = vecCompareNormal(a_vec,a_used, b_vec, b_used);
  int rel = vecCompare(a_vec,a_used, b_vec, b_used);
  if (rel == 0) { // a==b => a=1*a+0
    *r_used=0;  
    quot[0]=1;
    return 1;
  }
  if (rel  < 0) { //a<b => a=0*b+a 
    vecCopy(r,a_vec,a_used);
    *r_used=a_used;
    return 0;
  }
  // now we have |a| > |b|
 
  digit_sz quot_used;

  if (b_used == 1) { 
    quot_used=vecDivByOneDigit(a_vec, a_used, b_vec[0],quot,r);
     if(*r) 
       *r_used=1;
     else 
       *r_used=0;
  }     
  else {
   // now a and b both have at least 2 digits
    digit_sz div_complexity=b_used;
    if (2*b_used>a_used)
      div_complexity = a_used-b_used;
    if (div_complexity <= OUTER_DIVISION_LIMIT || !USE_KARA_DIV)
      quot_used=vecDivSchool(a_vec, a_used, b_vec, b_used,quot,r,r_used);
    else  
      quot_used=vecDivKara(a_vec,a_used,b_vec,b_used,quot,r,r_used);
  }

#ifdef DIGIT_CHECKS_ON
  digit_sz n=quot_used+b_used+1;
  digit* prod=new digit[n];
  vecInit(prod,n);
  vecMul(prod,quot,quot_used,b_vec,b_used);
  assert(*r_used<=b_used);  
  assert(vecCompare(b_vec,b_used,r,*r_used)>0);
  vecAddTo(prod,n,r,*r_used);
  assert(prod[n-1]==0);
  assert(vecCompare(prod,a_vec,a_used)==0);
  delete[] prod;
  r = NULL;
#endif

  return quot_used; 
}


digit_sz vecModMul(digit* rem,
            digit* a_vec, digit_sz a_used,
            digit* b_vec, digit_sz b_used,
            digit* n_vec, digit_sz n_used)
// rem=a*b mod n
// rem must have n_used digits
// a and b must be in Z/n (have at most n_used digits) 
{
  digit* tp=new digit[digNextPower(a_used+b_used)];    
  digit_sz tp_used =   vecMul(tp,a_vec,a_used,b_vec,b_used);
  
  digit* tq=new digit[digNextPower(tp_used-n_used+1)];  
  digit_sz rem_used;
  vecDiv(tq,rem,&rem_used,tp,tp_used,n_vec,n_used);

  delete[] tp; delete[] tq;

  return rem_used;
}

digit_sz vecModMulLean(digit* rem,
            digit* a_vec, digit_sz a_used,
            digit* b_vec, digit_sz b_used,
            digit* n_vec, digit_sz n_used, digit *tmp)
// rem=a*b mod n
// rem must have n_used digits
// a and b must be in Z/n (have at most n_used digits)
// tmp must have (2*n_used)+(n_used+1)=3*n_used+1  digits (temp. product and quotient)
{
#ifdef DIGIT_CHECKS_ON
  assert(a_used<=n_used);
#endif
  digit* tp=tmp; 
  digit_sz tp_used =   vecMul(tp,a_vec,a_used,b_vec,b_used);
  
  digit* tq=tmp+(n_used<<1); 
  digit_sz rem_used;
  vecDiv(tq,rem,&rem_used,tp,tp_used,n_vec,n_used);

  return rem_used;
}


digit_sz vecModSquare(digit* rem,
            digit* a_vec, digit_sz a_used,
            digit* n_vec, digit_sz n_used)
// rem=a*a mod n
// rem must have n_used digits
// a must be in Z/n (have at most n_used digits) 
{
  digit* tp=new digit[digNextPower(a_used<<1)];   // tmp. product
  digit_sz tp_used =   vecSquare(tp,a_vec,a_used);
  
  digit_sz tp_minsize = (tp_used >= n_used ? tp_used-n_used+1 : 1);
  digit* tq=new digit[digNextPower(tp_minsize)];  // tmp. quotient 
  digit_sz rem_used;
  vecDiv(tq,rem,&rem_used,tp,tp_used,n_vec,n_used);

  delete[] tp; delete[] tq;

  return rem_used;
}

digit_sz vecModSquareLean(digit* rem,
            digit* a_vec, digit_sz a_used,
            digit* n_vec, digit_sz n_used, digit* tmp)
// rem=a*a mod n
// rem must have n_used digits
// a must be in Z/n (have at most n_used digits)
// tmp must have 2*n_used+(n_used+1) digits
{
#ifdef DIGIT_CHECKS_ON
  assert(a_used<=n_used);
#endif
  digit* tp=tmp; // tmp. product
  digit_sz tp_used =   vecSquare(tp,a_vec,a_used);
  
  digit* tq=tmp+(n_used<<1); // tmp. quotient 
  digit_sz rem_used;
  vecDiv(tq,rem,&rem_used,tp,tp_used,n_vec,n_used);

  return rem_used;
}


digit_sz vecModExp(digit *prod0,digit* a_vec, digit_sz a_used,
               digit* e_vec, digit_sz e_used,
               digit* n_vec, digit_sz n_used)
// computes a^e mod n
// prod0 must have n_used digits
// a must be <=n, i.e. have at most n_used digits.
{
  assert(a_used<=n_used);

   *prod0=1;
   digit_sz used=1;
   if(n_used==0) return used; // a^0=1

   // store the current squares and product in these variables;
   digit* cursq0=new digit[digNextPower(n_used)];
   digit* cursq1=new digit[digNextPower(n_used)];
   digit* prod1=new digit[digNextPower(n_used)];
   digit *prods=0, *prodt=0; // source and target
   digit *cursqs, *cursqt;   // source and target

   int toggleP=0,toggleSq=0; // toggle for product and square
   int bitsInLastDigit=DIGIT_LENGTH-digLeadingZeros(e_vec[e_used-1]);
   int numIterations=DIGIT_LENGTH*(n_used-1)+bitsInLastDigit;
   int count=0; // count the iterations;

   digit* modtmp= new digit[digNextPower(3*n_used+1)]; 
     // tmp for lean functions

   vecCopy(cursq0,a_vec,a_used); // cursq0=a^0
   digit_sz sq_used=a_used;
   cursqs=cursq0;

   // step through all digits of e from right to left
   digit *ep=e_vec, *e_stop=e_vec+e_used;
   while(ep<e_stop) {
      digit curdigit=*ep;

      // step through all bits of curdigit from right to left
      // are we in the last digit?
      int jstop= ((ep<e_stop-1) ? DIGIT_LENGTH : bitsInLastDigit); 
      for(int j=0;j<jstop;j++) {
         count++;
         
         if(curdigit & 1) { // make a multiplication step
            if(toggleP==0) { prods=prod0; prodt=prod1;}
            else           { prods=prod1; prodt=prod0;}
            if(toggleSq==0) cursqs=cursq0; else cursqs=cursq1;
            toggleP = 1 - toggleP;
            used=vecModMulLean
                (prodt,prods,used,cursqs,sq_used,n_vec,n_used,modtmp);
            }

         if(count!=numIterations) { // square if not in the last iteration
           if(toggleSq==0) { cursqs=cursq0; cursqt=cursq1;}
            else           { cursqs=cursq1; cursqt=cursq0;}
            toggleSq = 1 - toggleSq;
            sq_used=vecModSquareLean
                (cursqt,cursqs,sq_used,n_vec,n_used,modtmp);
            }
         curdigit>>=1;
         }
         ep++;
     }
   

  if(prodt==prod1) // if last product went into prod1
     vecCopy(prod0,prod1,used); // copy it to prod0

  delete[] cursq0; delete[] cursq1; delete[] prod1; delete[] modtmp;

  return used;
}


inline digit digModReduceSmall(digit x, digit p, digit p_inv) {
  digit z = ((x>>HALF_LENGTH)*p_inv)>>(HALF_LENGTH-3); 
#ifdef DIGIT_CHECKS_ON
  assert ((p>>(HALF_LENGTH-2))==0);
  assert ((p>>(HALF_LENGTH-3))!=0);
  assert (x-z*p<=15*p);
  digit high=0;
  digMul(x>>HALF_LENGTH,p_inv,&high);
  assert(high==0);
#endif
  return x-z*p;
}


const digit vecSomeConstant = 11; 
  // the value 11 is arbitrary
const digit vecPrimeSmall = digGeneratePseudoPrimeWithBound
   ((HALF_BASE>>2)-vecSomeConstant*(HALF_BASE>>10),(1UL<<20));
  // vecPrimeSmall is just a little smaller than 2^(HALF_BASE-2)
const digit vecPrimeSmallInverse =
  (1UL<<(DIGIT_LENGTH-3))/vecPrimeSmall;
  // inverse of digPrimeSmall shifted by DIGIT_LENGTH-3
const digit vecBaseModPrime =
  ((HALF_BASE%vecPrimeSmall)*(HALF_BASE%vecPrimeSmall))%vecPrimeSmall;
  // the base modulo digPrimeSmall


digit vecModReduce(digit* a, digit_sz a_used, int a_sign) {
   digit p = vecPrimeSmall;
   digit mod;
   digit* a_stop=a;
   if (a_used == 0)
     return 0;
   if (a_used==1) 
     mod = (*a)%p; 
   else {  
     const digit p_inv = vecPrimeSmallInverse;
     const digit ModBase = vecBaseModPrime;
     a = a + (a_used-1);
     mod = digModReduceSmall(*a,p,p_inv);
     a--;
     while (a >= a_stop) {
        digit d = *a; 
        digit s = mod*ModBase+d; 
        if (s<d)      // overflow occured in the previous addition 
          s+=ModBase; // there is no overflow in this addition
        mod=digModReduceSmall(s,p,p_inv);
        a--;
     }
     mod = digModReduce(mod,p);
   }
   if (a_sign<0 && mod!=0)
     mod=p-mod;
   return mod;
}


bool vecModCompareOperationResult(
  digit* c, digit_sz c_used, int c_sign, 
  digit a_mod, digit b_mod, digit& c_mod,
  num_op operation) {

  c_mod = vecModReduce(c,c_used,c_sign);
  digit mod=0;
  digit p=vecPrimeSmall;
  switch (operation) {
    case vecADD:
      mod=a_mod+b_mod; 
      if (mod>=p)
        mod-=p;
      break;
    case vecSUB:
      mod=a_mod-b_mod; 
      if (mod>=p)
        mod+=p;
      break;
    case vecMUL: case vecSQR:
      mod=(a_mod*b_mod)%p;
      break;
    case vecDIV:
      LEDA_EXCEPTION(1,"vecModCompareOperationResult needs division remainder");
  }
  return (mod==c_mod);  

}

bool vecModCompareDivisionResult(
  digit* c, digit_sz c_used, int c_sign, 
  digit a_mod, digit b_mod, digit r_mod, digit& c_mod)
{
   c_mod = vecModReduce(c,c_used,c_sign);
   digit mod = digModReduce(c_mod*b_mod+r_mod,vecPrimeSmall);
   return (mod==a_mod);
}


void vecModCheckOperation(num_op operation,
    digit* a_vec, digit_sz a_used, int a_sign, digit& a_mod,
    digit* b_vec, digit_sz b_used, int b_sign, digit& b_mod,
    digit* c_vec, digit_sz c_used, int c_sign, digit& c_mod)
{
   if (a_mod==digit(~0))
     a_mod=vecModReduce(a_vec,a_used,a_sign);
   if (b_mod==digit(~0))
     b_mod=vecModReduce(b_vec,b_used,b_sign);
   bool is_correct=
     vecModCompareOperationResult(
        c_vec,c_used,c_sign,a_mod,b_mod,c_mod,operation);
   if (!is_correct) 
     switch (operation) {
       case vecADD: 
         LEDA_EXCEPTION(1,"bigint::operator + failed"); break;
       case vecSUB: 
         LEDA_EXCEPTION(1,"bigint::operator - failed"); break;
       case vecMUL: 
         LEDA_EXCEPTION(1,"bigint::operator * failed"); break;
       case vecDIV: case vecSQR:
         break; // these cases do not occur
     }
}


void vecModCheckSquare(
    digit* a_vec, digit_sz a_used, int a_sign, digit& a_mod,
    digit* c_vec, digit_sz c_used, int c_sign, digit& c_mod)
{
   if (a_mod==digit(~0))
     a_mod=vecModReduce(a_vec,a_used,a_sign);
   bool is_correct=
     vecModCompareOperationResult(c_vec,c_used,c_sign,a_mod,a_mod,c_mod,vecSQR);
   if (!is_correct) 
     LEDA_EXCEPTION(1,"bigint square failed");
}


void vecModCheckDivision(
    digit* a_vec, digit_sz a_used, int a_sign, digit& a_mod,
    digit* b_vec, digit_sz b_used, int b_sign, digit& b_mod,
    digit* c_vec, digit_sz c_used, int c_sign, digit& c_mod,
    digit* r_vec, digit_sz r_used, int r_sign, digit& r_mod)
{
   if (a_mod==digit(~0))
     a_mod=vecModReduce(a_vec,a_used,a_sign);
   if (b_mod==digit(~0))
     b_mod=vecModReduce(b_vec,b_used,b_sign);
   r_mod=vecModReduce(r_vec,r_used,r_sign);
   bool is_correct=
     vecModCompareDivisionResult(c_vec,c_used,c_sign,a_mod,b_mod,r_mod,c_mod);
   if (vecCompare(b_vec,b_used,r_vec,r_used)<=0)
     is_correct=false;
   if (!is_correct) 
     LEDA_EXCEPTION(1,"operator / failed"); 
}


static digit*   pot10_vec[32];
static digit_sz pot10_len[32];
static int      pot10_max_i = -1;


char* vec_to_decimal(digit* a, digit_sz a_used, int i, char* p, char* start)
{ 
  if (a_used == 0 && p == start) return p;

  if (i == 0)
  { digit r = (a_used == 1) ? a[0] : 0;
    if (p == start)
       { sprintf(p,"%lu",r);
         while (*p != '\0') p++;
        }
    else
       { sprintf(p,"%09lu",r);
         p += 9;
        }
       
    return p;
  }

  digit_sz len = pot10_len[i-1];

  digit* q = new digit[a_used]; // changed by ST (originally: digNextPower(len))
	// in method vecDivByOneDigit we have: digit* q_ptr = q + a_used;
    // so it seems that q should have at least a_used digits
	// (and since we divide a by pot10_vec[i-1] it cannot have more)
  digit* r = new digit[digNextPower(len)];

  digit_sz r_used;
  digit_sz q_used = vecDiv(q,r,&r_used,a,a_used,pot10_vec[i-1],len);

  p = vec_to_decimal(q,q_used,i-1,p,start);
  p = vec_to_decimal(r,r_used,i-1,p,start);

  delete[] q;
  delete[] r;

  return p;
}



char* vecToDecimal(digit* a, digit_sz n, char* p)
{ 
   if (pot10_max_i == -1)
   { delete[] pot10_vec[0];
     pot10_vec[0] = new digit[1]; 

     pot10_vec[0][0] = 1000000000;
     pot10_len[0] = 1;
     pot10_max_i = 0;
    }

   int i = 0;
   while (pot10_len[i] < n 
          || (pot10_len[i] == n && vecCompare(a,pot10_vec[i],n) > 0))
   { 
     if (++i <= pot10_max_i) continue;
     digit_sz ln = pot10_len[i-1];
     digit_sz sz = digNextPower(2*ln);

     delete[] pot10_vec[i];
     pot10_vec[i] = new digit[sz];

     pot10_len[i] = vecSquare(pot10_vec[i],pot10_vec[i-1],ln);
     pot10_max_i = i;
   } 


   p = vec_to_decimal(a,n,i,p,p);

   *p = '\0';

   return p;
}


/*
char* _approximating.cpp(digit* a, digit_sz n, char* p)
{ 
  digit* tmp = new digit[n];
  vecCopy(tmp,a,n);
  digit r;
  *--p = '\0';
  while (n > 0)
  { n = vecDivByOneDigit(tmp,n,1000000000,tmp,&r); 
    for(int i=0; i<9; i++)
    { *--p = '0' + char(r%10);
      r /= 10;
     }
   }
  while (*p == '0') p++;
  delete[] tmp;
  return p;
}
*/




digit_sz vecFromDouble(double x, digit* vec, int& sign, int& exp) {

// precondition: |x|<infinity
// vec needs two digits

  digit_sz used=0;
  if (x==0) 
    { sign=0; exp=0; }
  else {
    if (x < 0) 
      { sign=-1; x=-x; }
    else
      sign=1;
    exp=digFromDouble(x,vec[1],vec[0]);
    if (vec[1]==0)
      used=1;
    else
      used=2;
  }
  return used;

}


digit_sz vecFromDoubleAligned
(double x, digit* vec, int& sign, int& exp, digit_sz& precision) {

// precondition: |x|<infinity
// vec needs three digits

  digit_sz used=0;
  if (x==0) { sign=0; exp=0; }
  else {
    if (x < 0) 
      { sign=-1; x=-x; }
    else
      sign=1;
    vec[2]=0;
    exp=digFromDouble(x,vec[1],vec[0]);

#ifdef DIGIT_CHECKS_ON
    if (vec[1]!=0)
      assert (vecLength(vec,2)==54);
    else
      assert (vecLength(vec,1)==54);
#endif

    // the following is equivalent to 'int emod = exp%DIGIT_LENGTH;'
    int emod;
    if (exp<0) {
      emod  = (-exp) & ((1UL<<WORD_LENGTH_LOG)-1);
      if (emod)
      emod = DIGIT_LENGTH-emod;
    }
    else 
      emod  = exp & ((1UL<<WORD_LENGTH_LOG)-1);

    if (emod) {
      digit_sz rsh = DIGIT_LENGTH-emod;
      if (vec[0]&((1UL<<rsh)-1)) {
        vec[2] = vecShiftLeftCarry(vec,vec,2,emod);
        precision=54+emod;
        exp-=emod;
      }
      else {
        vecShiftRightCarry(vec,vec,2,rsh);
        precision = 54-rsh;
        exp+=rsh;
      }
    }
    else
      precision = 54;
    if (vec[2]==0) {
      if (vec[1]==0)
        used = 1;
      else
        used = 2;
    }
    else
      used=3;
  }
  exp>>=WORD_LENGTH_LOG;
  return used;

}


double vecToDouble(digit* vec, digit_sz used) {

  double d;
  if (used==0)
    d=0;
  else {
    d=double(vec[used-1]);
    for (digit_sz i=used-1;i>0;i--) 
      d=d*digit_base+double(vec[i-1]);
    }
  return d;

}







LEDA_END_NAMESPACE

