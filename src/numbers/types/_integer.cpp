/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _integer.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/integer.h>
#include <LEDA/system/assert.h>
#include <LEDA/numbers/digit_vector.h>
#include <ctype.h>

LEDA_BEGIN_NAMESPACE


static int zero_constructed;

static integer integer_zero;

static const digit digit_decimal_base = digPower(10,digit_decimal_base_length10);


void integer::construct_zero()
{ if (zero_constructed == 0)
  { integer_zero.PTR = integer::new_rep(1);
    integer_zero.PTR->sign = ZERO;
    integer_zero.PTR->used = 0;
    integer_zero.PTR->ref_minus(); 
		// to make up for the ref_plus() below if this == &integer_zero
    zero_constructed = 1;
   }
  PTR = integer_zero.PTR;
  PTR->ref_plus();
}




inline integer_rep* integer::allocate_bytes(digit_sz sz)
{
#ifdef INTEGER_PRIV_CHECKS_ON
  obj_count++;
#endif
  return (integer_rep*) std_memory.allocate_bytes(sz);
}

inline void integer::deallocate_bytes(integer_rep* p, digit_sz sz)
{
#ifdef INTEGER_PRIV_CHECKS_ON
  obj_count--;
  vecFill(p->vec,p->size,~(2UL));
#endif
  std_memory.deallocate_bytes(p,sz);
}


integer_rep* integer::new_rep(digit_sz sz)
{
  digit_sz s = sizeof(integer_rep) + (sz-1)*sizeof(digit);

  //we make vec one digit longer than sz (see asm_mult)
  //digit_sz s = sizeof(integer_rep) + sz*sizeof(digit);

  integer_rep* p = integer::allocate_bytes(s);

  new(p) integer_rep;  // added by sn (01/2007) fixing a serious bug: 
                       // uninitialized mutex in multi-threading programs 
  p->size = sz;
#ifdef INTEGER_PUB_CHECKS_ON
  p->mod=~0UL;
#endif

  return p;
 }

template<class T>
inline void call_destructor(T& x) { x.~T(); }

void integer::delete_rep(integer_rep* p) 
{ //p->count.~atomic_counter();
  call_destructor(p->count);
  digit_sz s = sizeof(integer_rep) + (p->size-1)*sizeof(digit);
  integer::deallocate_bytes(p,s); 
 }

integer_rep* integer::copy_rep(integer_rep* x) { 
  integer_rep* result = integer::new_rep(x->size);
  result->sign = x->sign;
  result->used = x->used;
  vecCopy(result->vec,x->vec,x->used);
#ifdef INTEGER_PUB_CHECKS_ON
  result->mod = ~0UL;
#endif
  return result;
}


integer::integer(unsigned int n)
{ 
  if (n == 0)
    construct_zero();
  else
    { PTR = integer::new_rep(1);
      PTR->vec[0] = n;
      PTR->sign = POSITIVE;
      PTR->used = 1;
    }
}

integer::integer(int n)
{
  if (n == 0)
  { construct_zero();
    return;
   }

  PTR = integer::new_rep(1);
  PTR->used = 1;

  if (n > 0) 
  { PTR->sign = POSITIVE;
    PTR->vec[0] = n;
   }
  else
  { PTR->sign = NEGATIVE;
    PTR->vec[0] = -n;
  }
}

integer::integer(long n)
{
  if (n == 0)
  { construct_zero();
    return;
   }

  PTR = integer::new_rep(1);
  PTR->used = 1;

  if (n > 0) 
  { PTR->sign = POSITIVE;
    PTR->vec[0] = n;
   }
  else
  { PTR->sign = NEGATIVE;
    PTR->vec[0] = -n;
  }
}

integer::integer(unsigned long n)
{
  if (n == 0)
    construct_zero();
  else
  {
    PTR = integer::new_rep(1);
    PTR->sign = POSITIVE;
    PTR->vec[0] = n;
    PTR->used = 1;
  }
}

integer::integer(unsigned int sz, const digit* vec, int sign)
{
  while(sz > 0 && vec[sz-1] == 0) sz--;

  if (sz == 0 || sign==0)
    construct_zero();
  else
  { PTR = integer::new_rep(digNextPower(sz));
    PTR->sign = POSITIVE;
    PTR->used = sz;
    while (sz-- > 0) PTR->vec[sz] = vec[sz];
    PTR->sign = (sign>0 ?POSITIVE : NEGATIVE);
   }
  
}


integer::integer(double x) 
{ 
  double d=x-x;
  if (d!=d)
    LEDA_EXCEPTION(1,"integer(double): argument infinite");

  int sig = POSITIVE;
  if (x < 0) 
  { sig = NEGATIVE;
    x = -x;
  }

  if (x < 1)
  { construct_zero();
    return;
  }

  if (x < digit_base)
  { PTR = integer::new_rep(1);
    PTR->vec[0] = digit(x);
    PTR->used = 1;
    PTR->sign = sig;
    return;
  }

  int expt;
  double mantissa = frexp(x, &expt);
  double int_mant;

  int r = expt % DIGIT_LENGTH;
  int q = expt / DIGIT_LENGTH;

  int used = q;
  if (r) used++;

  digit_sz sz = digNextPower(used);

  PTR = integer::new_rep(sz);
  PTR->used = used;
  PTR->sign = sig;

  digit* p = PTR->vec+used-1;

  if (r) { 
    mantissa *= ldexp(1.0,r);
    mantissa = modf(mantissa, &int_mant);
    *p--= (digit)int_mant;    // the highest digit has r bits
   }

  while (q--)
    if (mantissa != 0)
    { mantissa *= digit_base;
      mantissa = modf(mantissa, &int_mant);
      *p-- = (digit)int_mant; // next digit
     }
    else *p-- = 0;
}


// static

bool integer::use_karatsuba = true;

#ifdef INTEGER_PUB_CHECKS_ON
bool integer::do_checking = false;
void integer::activate_checking()   { do_checking = true; }
void integer::inactivate_checking() { do_checking = false; } 
#endif

#ifdef INTEGER_PRIV_CHECKS_ON
digit_sz integer::obj_count=0;
#endif


integer integer::operator++()
{ digit* p = PTR->vec;
  if (refs() == 1 && PTR->sign == integer::POSITIVE && p[0] != MAX_DIGIT)
     p[0]++;
  else 
    *this = *this + 1;
  return *this;
}


integer integer::operator--()
{ digit* p = PTR->vec;
  if (refs() == 1 && PTR->sign == integer::POSITIVE && p[0] != 0)
     p[0]--;
  else 
    *this = *this - 1;
  return *this;
}



integer operator+(const integer & a, const integer & b) 
{
#ifdef INTEGER_PRIV_CHECKING
  if (a.PTR->used>0) assert (a[a.PTR->used-1]!=0);
  if (b.PTR->used>0) assert (b[b.PTR->used-1]!=0);
#endif
  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign == integer::ZERO) return b;
  if (b_sign == integer::ZERO) return a;

  digit* a_vec = a.PTR->vec;
  digit* b_vec = b.PTR->vec;
  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;
  digit_sz a_size = a.PTR->size;
  digit_sz b_size = b.PTR->size;

  digit_sz sum_size;
  if (a_used >= b_used)
    sum_size = (a_used+1 <= a_size) ? a_size : 2*a_size;
  else
    sum_size = (b_used+1 <= b_size) ? b_size : 2*b_size;

  integer_rep* sum_ptr = integer::new_rep(sum_size);

  sum_ptr->used=vecSignedAdd (a_vec,a_used,a_sign,
                              b_vec,b_used,b_sign,
                              sum_ptr->vec,sum_ptr->sign);

#ifdef INTEGER_PUB_CHECKS_ON
  if (integer::do_checking) 
    vecModCheckOperation(vecADD,
      a_vec,a_used,a_sign,a.PTR->mod,
      b_vec,b_used,b_sign,b.PTR->mod,
      sum_ptr->vec,sum_ptr->used,sum_ptr->sign,sum_ptr->mod);
#endif

  return sum_ptr;
}



integer operator-(const integer & a, const integer & b) 
{
#ifdef INTEGER_PRIV_CHECKING
  if (a.PTR->used>0) assert (a[a.PTR->used-1]!=0);
  if (b.PTR->used>0) assert (b[b.PTR->used-1]!=0);
#endif
  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign == integer::ZERO) return -b;
  if (b_sign == integer::ZERO) return a;

  digit* a_vec = a.PTR->vec;
  digit* b_vec = b.PTR->vec;
  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;
  digit_sz a_size = a.PTR->size;
  digit_sz b_size = b.PTR->size;

  digit_sz sum_size;
  if (a_used >= b_used)
    sum_size = (a_used+1 <= a_size) ? a_size : 2*a_size;
  else
    sum_size = (b_used+1 <= b_size) ? b_size : 2*b_size;
  integer_rep* sum_ptr = integer::new_rep(sum_size);

  sum_ptr->used=vecSignedAdd (a_vec,a_used,a_sign,
                              b_vec,b_used,-b_sign,
                              sum_ptr->vec,sum_ptr->sign);

#ifdef INTEGER_PUB_CHECKS_ON
  if (integer::do_checking) 
    vecModCheckOperation(vecSUB,
      a_vec,a_used,a_sign,a.PTR->mod,
      b_vec,b_used,b_sign,b.PTR->mod,
      sum_ptr->vec,sum_ptr->used,sum_ptr->sign,sum_ptr->mod);
#endif

  return sum_ptr; // here a private constructor is called
}


integer integer::operator-()  const
{ // unary minus
  if (PTR->sign == integer::ZERO) return *this;
  integer_rep* p = integer::copy_rep(PTR);
  p->sign = -PTR->sign;
  return p;
}




integer operator*(const integer& a, const integer& b) 
{

  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign == integer::ZERO) return a;
  if (b_sign == integer::ZERO) return b;

  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;


  assert(digNextPower(a_used) <= a.PTR->size);
  assert(digNextPower(b_used) <= b.PTR->size);

  digit* a_vec = a.PTR->vec;
  digit* b_vec = b.PTR->vec;

  integer_rep* prod_ptr;
  digit_sz prod_size = digNextPower(a_used+b_used);
  prod_ptr = integer::new_rep(prod_size);

  prod_ptr->sign = (a_sign == b_sign ? integer::POSITIVE : integer::NEGATIVE);

  prod_ptr->used=vecMul(prod_ptr->vec,a_vec,a_used,b_vec,b_used);

#ifdef INTEGER_PUB_CHECKS_ON
  if (integer::do_checking) 
    vecModCheckOperation(vecMUL,
      a_vec,a_used,a_sign,a.PTR->mod,
      b_vec,b_used,b_sign,b.PTR->mod,
      prod_ptr->vec,prod_ptr->used,prod_ptr->sign,prod_ptr->mod);
#endif

  return prod_ptr; // here a private constructor is called
}



integer integer::square()  const
{ 
#ifdef INTEGER_PRIV_CHECKING
  if (PTR->used>0) assert ((*this)[PTR->used-1]!=0);
#endif
  digit_sz a_used = PTR->used;
  int a_sign = PTR->sign;

  if (a_sign == ZERO) return 0;

  digit_sz sz = digNextPower(a_used << 1);
  integer_rep* sqr_ptr = integer::new_rep(sz);
  sqr_ptr->sign = integer::POSITIVE;

  sqr_ptr->used = vecSquare(sqr_ptr->vec, PTR->vec, a_used);

#ifdef INTEGER_PUB_CHECKS_ON
  digit* a_vec = PTR->vec;
  if (integer::do_checking) 
    vecModCheckSquare(
      a_vec,a_used,a_sign,PTR->mod,
      sqr_ptr->vec,sqr_ptr->used,sqr_ptr->sign,sqr_ptr->mod);
#endif

  return sqr_ptr;
}

integer_rep* divide(const integer& a, const integer& b, integer_rep*& r_ptr) 
{
#ifdef INTEGER_PRIV_CHECKING
  if (a.PTR->used>0) assert (a[a.PTR->used-1]!=0);
  if (b.PTR->used>0) assert (b[b.PTR->used-1]!=0);
#endif
  digit* a_vec = a.PTR->vec;
  digit* b_vec = b.PTR->vec;
  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;
  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;

  digit* q_vec;
  digit* r_vec;

  integer_rep* q_ptr;
  if (vecCompare(a_vec,a_used,b_vec,b_used) < 0) 
   { q_ptr=integer::new_rep(0);
     q_ptr->sign=integer::ZERO;
     q_ptr->used=0;
     r_ptr=integer::copy_rep(a.PTR); 
    }
  else 
   { digit_sz sz = digNextPower(a_used - b_used + 1);
     q_ptr = integer::new_rep(sz);
     r_ptr = integer::new_rep(b.PTR->size);
     q_vec = q_ptr->vec;
     r_vec = r_ptr->vec;
     q_ptr->sign = (a_sign == b_sign) ? integer::POSITIVE : integer::NEGATIVE;
     q_ptr->used = vecDiv(q_vec,r_vec,&r_ptr->used,a_vec,a_used,b_vec,b_used); 

     if (r_ptr->used == 0)
        r_ptr->sign = integer::ZERO;
     else
        r_ptr->sign = a_sign;

     if (q_ptr->used == 0) q_ptr->sign=integer::ZERO; 
  }

#ifdef INTEGER_PUB_CHECKS_ON
  if (integer::do_checking) 
    vecModCheckDivision(
      a_vec,a_used,a_sign,a.PTR->mod,
      b_vec,b_used,b_sign,b.PTR->mod,
      q_ptr->vec,q_ptr->used,q_ptr->sign,q_ptr->mod,
      r_ptr->vec,r_ptr->used,r_ptr->sign,r_ptr->mod
    );
#endif

  return q_ptr;
}

integer operator/(const integer & a, const integer & b) 
{ 
  if (b.PTR->sign == integer::ZERO) 
    LEDA_EXCEPTION(1,"division by zero");
  if (a.PTR->sign == integer::ZERO) 
    return a;
  if (a.PTR->used<b.PTR->used)
    return 0;

  integer_rep* quot_ptr;
  integer_rep* r_ptr;
  quot_ptr = divide(a,b,r_ptr);

  // if (quot_ptr->sign!=0) // Warum nur in diesem Fall?
    integer::delete_rep(r_ptr);

  return quot_ptr;
}


integer integer::div(const integer & b, integer& r)  const
{ 
  if (b.PTR->sign == integer::ZERO) 
    LEDA_EXCEPTION(1,"division by zero");
  if (PTR->sign == integer::ZERO) { 
    r = 0;
    return *this;
  }
  if (PTR->used<b.PTR->used) {
    r= *this;
    return 0;
  }

  integer_rep* r_ptr;
  integer_rep* quot_ptr = divide(*this,b,r_ptr);

  r = r_ptr;
  return quot_ptr;
}


integer operator%(const integer & a, const integer & b) 
{ 
  if (b.PTR->sign == integer::ZERO) LEDA_EXCEPTION(1,"mod by zero");
  if (a.PTR->sign == integer::ZERO) return a;
  if (a.PTR->used < b.PTR->used) return a;

  integer_rep* r_ptr;
  integer_rep* quot_ptr = divide(a,b,r_ptr);
  integer::delete_rep(quot_ptr);

  return integer(r_ptr);
}



integer operator & (const integer & a, const integer & b)   // bitwise and
{
  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;
  int used = (a_used <= b_used ? a_used : b_used);
  int size = digNextPower(used);

  digit* ap = a.PTR->vec;
  digit* bp = b.PTR->vec;
  digit* a_stop = ap + used; 
  digit* p;

  integer_rep* and_ptr = integer::new_rep(size);

  p = and_ptr->vec;
  while (ap < a_stop) 
    *p++ = *ap++ & *bp++;

  while (*--p == 0 && used > 0) used--;

  and_ptr->used = used;
  if (used > 0)  
     and_ptr->sign = integer::POSITIVE;
  else
     and_ptr->sign = integer::ZERO;

  return and_ptr;
}


integer operator | (const integer & a, const integer & b)   // bitwise or
{
  if (a.PTR->sign == 0 && b.PTR->sign == 0) 
    return 0;

  digit_sz a_used = a.PTR->used;
  digit_sz b_used = b.PTR->used;

  integer_rep* or_ptr;

  digit* ap = a.PTR->vec;
  digit* bp = b.PTR->vec;
  digit* a_stop = ap + a_used; 
  digit* b_stop = bp + b_used; 

  if (a_used >= b_used)
     { or_ptr = integer::new_rep(a.PTR->size);
       or_ptr->used = a_used;
       digit* p = or_ptr->vec;
       while (bp < b_stop) *p++ = *ap++ | *bp++;
       while (ap < a_stop) *p++ = *ap++;
     }
  else
     { or_ptr = integer::new_rep(b.PTR->size);
       or_ptr->used = b_used;
       digit* p = or_ptr->vec;
       while (ap < a_stop) *p++ = *ap++ | *bp++;
       while (bp < b_stop) *p++ = *bp++;
      }

  or_ptr->sign = integer::POSITIVE;
  return or_ptr;
}

integer integer::operator~()  const
{ // negation
  integer_rep* p = integer::copy_rep(PTR);
  digit* wp = p->vec;
  while (wp < p->vec+p->used) {
    *wp = ~*wp; wp++; 
  }
  digit_sz used = p->used;
  while (--wp>=p->vec && *wp==0) 
    used--;

  p->used=used;
  if (used==0) 
    p->sign=integer::ZERO;
  return p;
}



integer integer::operator<<(int n)  const
{
  if (n == 0 || PTR->sign == integer::ZERO) return *this;

  if (n < 0) return operator>>(-n);

  digit_sz sz = digNextPower(PTR->used + (n+DIGIT_LENGTH-1)/DIGIT_LENGTH);

  integer_rep* result = integer::new_rep(sz);
  result->sign = PTR->sign;
  result->used = vecShiftLeft(result->vec,PTR->vec,PTR->used,n);

  return result;
}


integer& integer::operator<<=(int n) 
{
  if (n == 0 || PTR->sign == integer::ZERO) return *this;

  digit_sz used = PTR->used + (n+DIGIT_LENGTH-1)/DIGIT_LENGTH;

  if (refs() == 1 && used <= PTR->size)
      PTR->used = vecShiftLeft(PTR->vec,PTR->vec,PTR->used,n);
  else
    { integer_rep* result = integer::new_rep(digNextPower(used));
      result->sign = PTR->sign;
      result->used = vecShiftLeft(result->vec,PTR->vec,PTR->used,n);
      *this = result;
     }

  return *this;
}




integer integer::operator>>(int n)  const
{
  if (n == 0 || PTR->sign == integer::ZERO) return *this;

  if (n < 0) return operator<<(-n);

  int used = PTR->used - n/DIGIT_LENGTH;

  if (used < 0) used = 0; // bugfix (sn): otherwise digNextPower loops forever  

  integer_rep* result = integer::new_rep(digNextPower(used));
  
  if (used == 0)
  { // return zero
    result->vec[0] = 0;
    result->sign = integer::ZERO;
    result->used = 0;
    return result;
   }
 
  result->sign = PTR->sign;
  result->used = vecShiftRight(result->vec,PTR->vec,PTR->used,n);

  if (result->used == 0) 
        result->sign = integer::ZERO;
 
  return result;
}





bool operator > (const integer & a, const integer & b)
{
  if (a.PTR == b.PTR) return false;

  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign > b_sign) return true;
  if (a_sign < b_sign) return false;

  // the signs are equal 

  if (a_sign == integer::ZERO) return false;

  if (a_sign == integer::POSITIVE)
    return vecCompareNormal(a.PTR->vec,a.PTR->used,b.PTR->vec,b.PTR->used) > 0;
  else
    return vecCompareNormal(a.PTR->vec,a.PTR->used,b.PTR->vec,b.PTR->used) < 0;
}


bool operator < (const integer & a, const integer & b)
{
  if (a.PTR == b.PTR) return false;

  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign < b_sign) return true;
  if (a_sign > b_sign) return false;

  // the signs are equal 

  if (a_sign == integer::ZERO) return false;

  if (a_sign == integer::POSITIVE)
    return vecCompareNormal(a.PTR->vec,a.PTR->used,b.PTR->vec,b.PTR->used) < 0;
  else
    return vecCompareNormal(a.PTR->vec,a.PTR->used,b.PTR->vec,b.PTR->used) > 0;
}


bool operator == (const integer & a, const integer & b)
{
  if (a.PTR == b.PTR) return true;

  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign != b_sign) return false;

  if (a_sign == integer::ZERO) return true;

  return vecCompareNormal(a.PTR->vec,a.PTR->used,b.PTR->vec,b.PTR->used) == 0;
}



bool integer::operator==(int l) const
{ int sig = PTR->sign;
  if (l==0) return sig == integer::ZERO;

  if (PTR->used > 1) return false;

  if (l > 0 )
  { if (sig <= 0) return false;
    return PTR->vec[0] == digit(l);
   }

  // l < 0
  if (sig >= 0) return false;
  return PTR->vec[0] == digit(-l);
}


bool integer::operator<(int l) const
{ int sig = PTR->sign;
  if (l==0) return sig == integer::NEGATIVE;

  if (l > 0 )
  { if (sig <= 0) return true;
    if (PTR->used > 1) return false;
    return PTR->vec[0] < digit(l);
   }
  
  // l < 0
  if (sig >= 0) return false;
  if (PTR->used > 1) return true;
  return PTR->vec[0] > digit(-l);
}


bool integer::operator>(int l) const
{ int sig = PTR->sign;
  if (l==0) return sig == integer::POSITIVE;

  if (l < 0 )
  { if (sig >= 0) return true;
    if (PTR->used > 1) return false;
    return PTR->vec[0] < digit(-l);
   }
  
  // l > 0
  if (sig <= 0) return false;
  if (PTR->used > 1) return true;
  return PTR->vec[0] > digit(l);
}



integer modMul(const integer & a, const integer & b,const integer &n) 
{ 
  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;
  int n_sign = n.PTR->sign;
  int a_used = a.PTR->used;
  int b_used = b.PTR->used;

  if (n_sign == integer::ZERO) LEDA_EXCEPTION(1,"mod by zero");

  if (a_sign < 0 || b_sign < 0) 
    return (a*b)%n;

  if (a_sign == integer::ZERO) return a;
  if (b_sign == integer::ZERO) return b;

  integer_rep* rem_ptr;
  
  rem_ptr = integer::new_rep(n.PTR->size);  
  rem_ptr->used=vecModMul(
      rem_ptr->vec,a.PTR->vec,a_used,b.PTR->vec,
      b_used,n.PTR->vec,n.PTR->used); 

  if (rem_ptr->used == 0) 
     rem_ptr->sign = integer::ZERO;
  else
     rem_ptr->sign = integer::POSITIVE;

  return rem_ptr; 
}

integer modSquare(const integer & a,const integer &n)
{ 
  int a_sign = a.PTR->sign;
  int n_sign = n.PTR->sign;
  digit_sz a_used = a.PTR->used;

  if (n_sign == integer::ZERO) 
    LEDA_EXCEPTION(1,"mod by zero");
  if (a_sign == integer::ZERO) 
    return a;

  integer_rep* rem_ptr = integer::new_rep(n.PTR->size);  
  
  rem_ptr->used=vecModSquare(
      rem_ptr->vec,a.PTR->vec,a_used,n.PTR->vec,n.PTR->used); 
  
  if (rem_ptr->used == 0) 
     rem_ptr->sign = integer::ZERO;
  else
     rem_ptr->sign = integer::POSITIVE;

  return rem_ptr; // here a private constructor is called
}

integer modExp(const integer & a, const integer& e, const integer &n) 
{ 
  int a_sign = a.PTR->sign;
  int n_sign = n.PTR->sign;
  int e_sign = e.PTR->sign;

  if (n_sign == integer::ZERO) 
    LEDA_EXCEPTION(1,"mod by zero");
  if (e_sign < 0)
    LEDA_EXCEPTION(1,"modExp exponent must be >= 0");
  if (e_sign==integer::ZERO) 
    return 1;
  if (a_sign == integer::ZERO) 
    return 0;

  digit_sz a_used = a.PTR->used;
  if(a_used>n.PTR->used) {
    return modExp(a%n,e,n);
  }

  integer_rep *rem_ptr = integer::new_rep(n.PTR->size);  
  rem_ptr->used=vecModExp(
    rem_ptr->vec,a.PTR->vec,a_used,
    e.PTR->vec,e.PTR->used,n.PTR->vec,n.PTR->used);

  if (rem_ptr->used == 0) 
     rem_ptr->sign = integer::ZERO;
  else
     rem_ptr->sign = integer::POSITIVE;

  return rem_ptr; 
}

double integer::to_double() const
{ 
  if (PTR->sign == integer::ZERO) return 0;
  
  double d=0;
  if (PTR->used > 1024/DIGIT_LENGTH) { 
    d = ldexp(1.0,1022);
    d *= 16.0;
    // d = infinity
  }
  else
  {
    unsigned int i=0;
    double pow_base = 1;
    digit*  v = PTR->vec;
    while (i < PTR->used) 
    {
      d += double(v[i++])*pow_base;
      pow_base *= digit_base;
    }
  }

  return (PTR->sign == integer::NEGATIVE) ? -d : d;
}

double integer::to_double(bool& is_double) const
{ 
  if (PTR->sign == integer::ZERO) return 0;
  
  double d=0;
  if (PTR->used > 1024/DIGIT_LENGTH) { 
    d = ldexp(1.0,1022);
    d *= 16.0;
    // d = infinity
	is_double = false;
  }
  else
  {
    is_double = true;
    unsigned int i=0;
    double pow_base = 1;
    digit*  v = PTR->vec;
	/* NOTE:
		As long as is_double is true, we have that the exponent of d is
		strictly less than the exponent of pow_base. This means that 
		d += to_add basically ors the mantissa of d into some part at the end 
		of the mantissa of to_add which contains only zeros. 
		Rounding the exact value (d+to_add) to the closest double value may cut
		off some part of the mantissa of d (and maybe also "increment" the 
		non-zero part of the mantissa of to_add).
		We can detect this by checking (d+to_add) - to_add = d.
	*/
    while (i < PTR->used) 
    {
      double digit_as_dbl = double(v[i]);	  
	  if ((DIGIT_LENGTH > 52) && (digit(digit_as_dbl) != v[i])) is_double = false;
	  double to_add = digit_as_dbl*pow_base, d_old = d;
      d += to_add;
	  if (d - to_add != d_old) is_double = false;
      pow_base *= digit_base; ++i;
    }
  }

  return (PTR->sign == integer::NEGATIVE) ? -d : d;
}

integer abs(const integer & a)
{ if (a.PTR->sign >= 0) return a;
  integer_rep* ptr = integer::copy_rep(a.PTR);
  ptr->sign = integer::POSITIVE;
  return ptr;
}

void integer::absolute() // *this = abs(*this)
{ if (refs() == 1) 
    PTR->sign = integer::POSITIVE;
  else 
    *this = abs(*this); 
}

int log(const integer & a)
{
  if (a.PTR->sign != integer::POSITIVE)
     LEDA_EXCEPTION(1,"nonpositive argument for log(integer)");
  int l  = a.PTR->used;
  int lg = l * DIGIT_LENGTH;
  lg=lg-digLeadingZeros(a.PTR->vec[l-1])-1;

  return lg;
}

int log2_abs(const integer & a)
{
  if (a.PTR->sign == integer::ZERO)
     LEDA_EXCEPTION(1,"zero argument for log2_abs(integer)");
  int l = a.PTR->used;
  int bitlen = l * DIGIT_LENGTH - digLeadingZeros(a.PTR->vec[l-1]);
  return bitlen == 1+a.zeros() ? (bitlen-1) : bitlen;
}

integer integer::random(int n)  // return n-bit random integer
{ integer x;
  int save = rand_int.set_precision(n);
  rand_int >> x;
  rand_int.set_precision(save);
  return x;
 } 
 
random_source& operator>>(random_source& ran, integer& x)
{ 
  int n = ran.get_precision(); 
  if (n <= 0) n=1;
  int w = n / DIGIT_LENGTH;
  int r = n % DIGIT_LENGTH;

  digit_sz size = (r>0? w+1 : w);
  digit *vec = new digit[size];
  for (digit_sz i=0;i<size;i++) 
    vec[i]=ran.get();
  if (r!=0)
    vec[size-1] &= ((1UL<<r)-1);
  x = integer(size,vec,1);
  delete[] vec;

  return ran;
 }

int integer::zeros() const
{ // gives the number of zeros at the end of the integer (in the binary rep.)
  if (PTR->sign == integer::ZERO) return 0;
  int k=0;
  while (!(PTR->vec[k])) k++;
  int len = k * DIGIT_LENGTH;
  digit low = PTR->vec[k];
  len+=digTrailingZeros(low);
  return len;
}


int integer::length() const   
{ // gives the number of bits
  if (PTR->sign == integer::ZERO) return 0;
  int len = PTR->used - 1;
  digit hi= PTR->vec[len];
  len *= DIGIT_LENGTH;
  len+=(DIGIT_LENGTH-digLeadingZeros(hi));
  return len;
}

bool integer::is_long() const { 
 if (PTR->sign == 0) return true;
 else return PTR->used == 1 && PTR->vec[0] <= ((PTR->sign > 0) ? 
    MIN_NDIGIT-1 : MIN_NDIGIT);
}

long integer::to_long() const { 
 if (PTR->sign == 0) return 0;
 else return  (PTR->sign > 0) ? ((long)PTR->vec[0]) : -((long)PTR->vec[0]);
}

int integer::cmp(const integer & a, const integer & b) { 
  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;

  if (a_sign < b_sign) return -1;
  if (a_sign > b_sign) return  1;

  // the signs are equal 

  if (a_sign == integer::ZERO) return 0;

  if (a_sign == integer::POSITIVE)
      return  vecCompareNormal(a.PTR->vec,a.PTR->used,
                           b.PTR->vec,b.PTR->used);
  else
      return -vecCompareNormal(a.PTR->vec,a.PTR->used,
                           b.PTR->vec,b.PTR->used);
}



integer sqrt(const integer& x)
{
  if (x.PTR->sign == integer::NEGATIVE)
    LEDA_EXCEPTION(1, "negative argument in sqrt");

  if (x.PTR->sign == integer::ZERO) return x;

  integer root = x >> (x.length()/2);  // first approx.
  integer fix  = (root*root + x)/(root << 1);

  do { root = fix;
       fix  = (root*root + x)/(root << 1);
     } while (fix < root);

  return root;  
}


integer factorial(const integer& n)
{ integer result = 1;
  for(integer i=2; i<=n; i++) result *= i;
  return result;
}


integer gcd(const integer& a, const integer& b)
{ 
  // TODO gcd is not efficient. See Knuth 2 for improvements. 

  int a_sign = a.PTR->sign;
  int b_sign = b.PTR->sign;
  
  if (a_sign == integer::ZERO)
  { if (b_sign == integer::ZERO)
        return 1;
    else
        return abs(b);
   }

  // here a is non-zero
  if (b_sign == 0) return abs(a);

  // here both a and b are non-zero
  integer u = abs(a);
  integer v = abs(b);

  if (u < v) v = v%u;
   
  while (sign(v) != 0)
  { integer tmp = u % v; 
    u = v;
    v = tmp;
   }

  return u;
}


void integer::decimal_string(char* p) const 
{ if (PTR->sign == integer::ZERO) 
    strcpy(p,"0");
  else
    { if (PTR->sign == integer::NEGATIVE) *p++ = '-';
      vecToDecimal(PTR->vec,PTR->used,p);
     }
}


string integer::to_string() const
{ char* digits = new char[PTR->used * (digit_decimal_base_length10+1) + 2];
  decimal_string(digits);
  string str = digits;
  delete[] digits;
  return str;
}


ostream & operator << (ostream & out, const integer & x) 
{ 
  string str = x.to_string();
  out << str; 
  return out;
}

void integer::hex_print(ostream& out) const
{ 
  if (PTR->sign == integer::ZERO)
  { out << "0";
    return;
   }

  digit* p = PTR->vec + PTR->used;

  if (DIGIT_LENGTH == 64)
  { out << string("%016lx",*--p);
    while (p > PTR->vec) out << string(":%016lx",*--p);
   }

  if (DIGIT_LENGTH == 32)
  { out << string("%08x",*--p);
    while (p > PTR->vec) out << string(":%08x",*--p);
   }
}


integer::integer(const char* cp)
{
  PTR=NULL;

  while(isspace(*cp)) cp++;

  int a_sign=1;
  if (*cp == '-') { 
    a_sign=-1; cp++; 
  }

  // look for leading zeros, ignore multiple ones
  bool leading_zero=false;
  if (*cp == '0')
     leading_zero=true;
  while (*cp == '0') cp++;

  // compute decimal length
  int len=0;
  const char* p = cp;
  while (isdigit(*p)) {
    p++; len++;
  }

  while (isspace(*p)) 
    p++;

  // check that the string is terminated
  bool terminated=false;
  if (*p == '\0')
    terminated=true;

  if (!terminated || (len==0 && !leading_zero))
    LEDA_EXCEPTION(1,"input string is not an integer\n");

  if (leading_zero && len==0) { 
    construct_zero();
    return;
  }

  // we want to partition cp into blocks of 
  // |digit_decimal_base_length10| decimal digits
  digit_sz str_size=len;
  digit_sz u=len%digit_decimal_base_length10;
  digit_sz v=digit_decimal_base_length10-u;
  if (u>0)
    str_size+=v;

  // copy cp into str and add leading 0's
  char* str = new char[str_size];
  char* str_begin=str;
  if (u==0) v=0;
  unsigned int i;
  for (i=0;i<v;i++) 
    str[i] = '0';
  for (i=v;i<str_size;i++) 
    str[i]=cp[i-v]; 

  integer result=0;	
  for (i=0;i<str_size/digit_decimal_base_length10;i++) {
    digit block_value;
    str=digFromDecimal(block_value,str);
    result=result*digit_decimal_base+block_value;
  }  

  PTR = copy_rep(result.PTR);
  if (a_sign==-1)
    PTR->sign=integer::NEGATIVE;

  delete[] str_begin;
}




istream & operator >> (istream & in, integer & a) 
{
  bool negative = false;
  char c;

  while (in.get(c) && isspace(c));

  if (c == '-') 
  { negative = true;
    while (in.get(c) && isspace(c));
   }

  if (isdigit(c)) 
  { a = c - '0';
    while (in.get(c) && isdigit(c)) a = 10*a + (c-'0');
   }

  if (in) in.putback(c);

  if (sign(a) != 0 && negative) a = -a;

  return in;

}


double double_quotient(const integer& a, const integer& b)
{ int d = b.length() - a.length() + 53;
  //integer q = (d > 0) ? ((a<<d)/b) : ((a>>-d)/b);
  integer q = (a<<d)/b;
  return ldexp(q.to_float(),-d);
}


LEDA_END_NAMESPACE
