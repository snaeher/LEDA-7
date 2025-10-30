/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _rational.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/rational.h>
#include <LEDA/numbers/digit_vector.h>
#include <LEDA/numbers/fp.h>

#include <LEDA/numbers/bigfloat.h>
#include <LEDA/numbers/interval_constants.h>


LEDA_BEGIN_NAMESPACE

/*
rational::rational(int i) : num(i), den(1) {}
*/


rational& rational::normalize()
{ 

  digit_sz num_used=num.PTR->used;
  digit_sz den_used=den.PTR->used;
  if (num_used==den_used) {
    // num_used must be >0, because den is not 0
    if (vecCompare(num.PTR->vec,den.PTR->vec,num_used)==0) {
      if (num.PTR->sign==integer::POSITIVE)
        num=den=1;
      else {
        num=-1; den=1;
      }
      return *this;
    }
  }
 
  integer ggt = gcd(num, den);
  if (ggt != 1) { 
    integer r;
    num = num.div(ggt,r);
    if (!r.iszero())
      LEDA_EXCEPTION(1,"rational::normalize: gcd computation failed");
    den = den.div(ggt,r);
    if (!r.iszero())
      LEDA_EXCEPTION(1,"rational::normalize: gcd computation failed");
  }

  return *this;
}


rational& rational::simplify(const integer& a)
{ 
  integer r;
  integer _den = den.div(a,r);
  if (r !=0)
    return *this;
  integer _num = num.div(a,r);
  if (r !=0)
    return *this;
  den=_den; num=_num;
  return *this;
}


rational& rational::operator= (const rational& r)
{ if (this == &r) return *this;
  num = r.num;
  den = r.den;
  return *this;
}

rational::rational(int n, int d)
{ 
  if (d==0)
    LEDA_EXCEPTION(1,"rational: Zero denominator!");

  if (d < 0) { 
    n = -n; d = -d; 
  }  

  num = integer(n); 
  den = integer(d);
}

rational::rational(integer n, integer d)
{ 
  int sgn=d.PTR->sign;
  if (sgn == 0) 
    LEDA_EXCEPTION(1,"rational: Zero denominator!");
 
  if (sgn  < 0) 
    { num = -n; den = -d; }
  else
    { num =  n; den =  d; }
}



rational::rational(double x) {

  if (fp::is_infinite(x))
    LEDA_EXCEPTION (1,"rational(double): infinite argument");

  if ( ::floor(x) == x || ::ceil(x) == x)
  { num = integer(x);
    den = integer(1);
    return;
   }


  digit vec[2];
  int sign,exp;
  digit_sz used=vecFromDouble(x,vec,sign,exp);

  num=integer(used,vec,sign);
 
  den = 1;

  if (exp < 0) den <<= (-exp); 
  if (exp > 0) num <<= exp;
  
}

double rational::to_double() const
{ 
  if (num.iszero())
    return 0;
  digit_sz num_used = num.PTR->used;
  digit_sz n_used = (num_used<=3?num_used:3); 
  digit* n_vec = num.PTR->vec+(num_used-n_used);
  double d_num=vecToDouble(n_vec,n_used);

  digit_sz den_used = den.PTR->used;
  digit_sz d_used = (den_used<=3?den_used:3); 
  digit* d_vec = den.PTR->vec+(den_used-d_used);
  double d_den=vecToDouble(d_vec,d_used);

  double x=d_num/d_den;
  int exp=((num_used-n_used)-(den_used-d_used))*DIGIT_LENGTH;

  if (num.PTR->sign>0)
    return ldexp(x,exp);
  else
    return ldexp(-x,exp);
} 

double rational::to_double_precise() const 
{ 
  return div(bigfloat(num),bigfloat(den),53).to_double();
}

double rational::to_double_precise(double& abs_err) const 
{ 
  bool exact_div = false;
  double d = div(bigfloat(num), bigfloat(den), 53, TO_NEAREST, exact_div).to_double(abs_err);
  if (! exact_div) {
    double factor = abs_err == 0 ? 1 : roundup_factor;
	abs_err += (fp::abs(d) * eps) + MinDbl;
    abs_err *= factor;
	  // the previous addition above may not be exact, roundup_factor should account for this
  }
  return d;
}



rational rational::addsub(const rational& x, const rational& y, bool subtract) 
{
  if (y.num.PTR->sign==0)
    return x;

  if (subtract) {
    if (x.num.PTR->sign==0) 
      return -y;

    if ((y.den.PTR->vec[0]==1) && (y.den.PTR->used==1))
      return rational(x.num-y.num*x.den,x.den);

    if ((x.den.PTR->vec[0]==1) && (x.den.PTR->used==1))
      return rational(-y.num+x.num*y.den,y.den);
  }
  else {
    if (x.num.PTR->sign==0) 
      return y;

    if ((y.den.PTR->vec[0]==1) && (y.den.PTR->used==1))
      return rational(x.num+y.num*x.den,x.den);

    if ((x.den.PTR->vec[0]==1) && (x.den.PTR->used==1))
      return rational(y.num+x.num*y.den,y.den);
  }

  
  digit_sz   xd_used = x.den.PTR->used;
  digit* xd_vec  = x.den.PTR->vec;

  digit_sz   yd_used = y.den.PTR->used;
  digit* yd_vec  = y.den.PTR->vec;


  // drep = xd*yd
  digit_sz den_size = digNextPower(xd_used+yd_used);

  integer_rep* drep = integer::new_rep(den_size);

  drep->used=vecMul(drep->vec,xd_vec,xd_used,yd_vec,yd_used);
  drep->sign=integer::POSITIVE;

  digit_sz xn_used = x.num.PTR->used;
  digit_sz yn_used = y.num.PTR->used;
  digit* xn_vec = x.num.PTR->vec;
  digit* yn_vec = y.num.PTR->vec;

  digit_sz num_size1 = xn_used+yd_used;
  digit_sz num_size2 = yn_used+xd_used;
  digit_sz num_size;

  if (num_size1 >= num_size2)
    num_size=digNextPower(num_size1+1);
  else
    num_size=digNextPower(num_size2+1);


  integer_rep* tmp_rep = integer::new_rep(2*num_size); 

  digit* tmp_vec = tmp_rep->vec+num_size;

  // tmp_rep = xn*yd;
  tmp_rep->used=vecMul(tmp_rep->vec,xn_vec,xn_used,yd_vec,yd_used);
  tmp_rep->sign=(x.num.PTR->sign==y.den.PTR->sign ?  integer::POSITIVE : 
                                                     integer::NEGATIVE);
  // tmp_vec = xd*yn
  digit_sz tmp_used=vecMul(tmp_vec,xd_vec,xd_used,yn_vec,yn_used);
  int tmp_sign=(y.num.PTR->sign==x.den.PTR->sign ?  integer::POSITIVE : 
                                                    integer::NEGATIVE);
  if (subtract) tmp_sign = -tmp_sign;

  // nrep = tmp_vec + tmp_rep
  integer_rep* nrep = integer::new_rep(num_size);
  nrep->used=vecSignedAdd(tmp_rep->vec,tmp_rep->used,tmp_rep->sign,
                          tmp_vec,tmp_used,tmp_sign,nrep->vec,nrep->sign);

  integer::delete_rep(tmp_rep);
 
  // return rational(nrep,drep);  // s.n: causes memory leak - why ?

  return rational(integer(nrep),integer(drep));
  
}

// We do not provide addsub(rational,integer) because 
// this function cannot (always) use AddTo/SubFrom, as the two 
// latter functions require that the first operand is bigger 
// than the second one. Consequently we would need temporary space, 
// and by this we would lose so much that we take the standard integer 
// operations instead.



// addition operators:

rational operator+(const rational& x, const rational& y) {
  return rational::addsub(x,y,false);
}

/*

// these next two functions yield overloading problems

rational operator+(const rational& x, const integer& y) {
  integer num = x.numerator()+x.denominator()*y;
  integer den = x.denominator();
  return rational(num,den);
}

rational operator+(const integer& x, const rational& y) {
  integer num = y.numerator()+y.denominator()*x;
  integer den = y.denominator();
  return rational(num,den);
}

*/

rational& rational::operator+= (const rational& r) { 
  *this = rational::addsub(*this,r,false);
  return *this;
}

// subtraction operators:

rational operator-(const rational& x, const rational& y) {
  return rational::addsub(x,y,true);
}


rational& rational::operator-= (const rational& r) { 
  *this = rational::addsub(*this,r,true);
  return *this;
}


// multiplication operators:

rational& rational::operator*= (const rational& r) { 
  num *= r.num;
  den *= r.den;
  return *this;
}

rational operator*(const rational& x, const rational& y) { 
  rational z = x; return z *= y; 
}


// division operators:

rational& rational::operator/= (const rational& r)
{ 
  if (r.num.PTR->sign == 0) 
    LEDA_EXCEPTION(1,"rational::division by 0");

  if (r.num.PTR->sign == -1)
    { num *= -r.den;
      den *= -r.num;
     }
   else
    { num *= r.den;
      den *= r.num;
     }
  return *this;
}

rational operator/(const rational& x, const rational& y) { 
  rational z = x; return z /= y; 
}


rational operator-(const rational& x) { 
  return rational(-x.num,x.den); 
}

rational& rational::operator++ ()       { num += den; return *this; }
rational& rational::operator-- ()       { num -= den; return *this; }



integer rational::numerator()   const { return num; }
integer rational::denominator() const { return den; }
void    rational::negate()            { num = - num; }

int sign(const rational& r) { return sign(r.num); }

rational abs(const rational& r)
{ if (sign(r.num) > -1) return r; else return -r; }

rational sqr(const rational& r)
{ return rational(r.num*r.num, r.den*r.den); }

integer trunc(const rational& r)
{ return (r.num / r.den); }


rational rational::inverse()
{ 
  if (num.PTR->sign == 0) LEDA_EXCEPTION(1,"Zero denominator!");
  
  if (num.PTR->sign == 1)
     return rational(den,num);
  else
     return rational(-den,-num);
}

void rational::invert()
{ 
  if (num.PTR->sign == 0) LEDA_EXCEPTION(1,"Zero denominator!");

  integer tmp = num;
  if (num.PTR->sign == 1)
    { num = den;
      den = tmp;
     }
  else
    { num = -den;
      den = -tmp;
     }
}

rational pow(const rational& r, int i)
{ 
  // r.normalize();

  rational mul(r);
  rational result(1,1);

  if (i < 0) 
  { mul = mul.inverse();
    i = -i;
   }

  while (i--)
  { result.num *= mul.num;
    result.den *= mul.den;
   }

  return result;
 }


rational pow(const rational& r, integer I)
{ 
  // r.normalize();

  rational mul(r);
  rational result(1,1);

  if (I < 0)
  { mul = mul.inverse();
    I=-I;
   }

  while (I-- != 0)
  { result.num *= mul.num;
    result.den *= mul.den;
   }

  return result;
 }

integer floor(const rational& r) { 
  integer x = r.num/r.den;
  if (sign(r.num) == -1 && r.num%r.den != 0) 
    x--;
  return x;
}


integer ceil(const rational& r) { 
  integer x = r.num/r.den; 
  if (sign(r.num) == 1 && r.num%r.den != 0) 
    x++;
  return x;
}


integer round(const rational& r) {
 
  integer rem;
  integer quot = (r.num).div(r.den, rem);
  rem <<= 1;
  if (sign(rem) < 0) 
      { if (-rem >= r.den) quot--; }
  else 
      { if ( rem >= r.den) quot++; } 
  return quot;

}



int rational::cmp(const rational& x, const rational& y)
{ 
  int xsign = sign(x.num);
  int ysign = sign(y.num);

  if (xsign == 0) return -ysign;
  if (ysign == 0) return  xsign;

  if (xsign == ysign) 
    return compare(x.num*y.den, y.num*x.den);
  else 
    return compare(xsign,ysign);
}


bool operator==(const rational& x, const rational& y)
{ return x.num * y.den == x.den * y.num; }

bool operator==(const rational& x, int y)
{ return x.den * y == x.num; }

bool operator==(int x, const rational& y)
{ return y.den * x == y.num; }

bool operator==(const rational& x, const integer& y)
{ return x.den * y == x.num; }

bool operator==(const integer& x, const rational& y)
{ return y.den * x == y.num; }

bool operator!=(const rational& x, const rational& y)
{ return x.num * y.den != x.den * y.num; }

bool operator!=(const rational& x, int y)
{ return x.den * y != x.num; }

bool operator!=(int x, const rational& y)
{ return y.den * x != y.num; }

bool operator!=(const rational& x, const integer& y)
{ return x.den * y != x.num; }

bool operator!=(const integer& x, const rational& y)
{ return y.den * x != y.num; }
    
bool operator<(const rational& x, const rational& y)
{ return rational::cmp(x,y) < 0; }

bool operator<=(const rational& x, const rational& y)
{ return rational::cmp(x,y) <= 0; }

bool operator>(const rational& x, const rational& y)
{ return rational::cmp(x,y) > 0; }

bool operator>=(const rational& x, const rational& y)
{ return rational::cmp(x,y) >= 0; }


string rational::to_string() const {
  return num.to_string() + "/" + den.to_string(); 
}

ostream& operator<< (ostream& s, const rational& r)
{  s << r.num << "/" << r.den; 
   return s; 
}

istream& operator>> (istream& in, rational& r)
{ 
   // Format: "r.num / r.den"

   char c;

   do in.get(c); while (isspace(c));
   in.putback(c);

   integer rx;
   in >> rx;

   do in.get(c); while (isspace(c));
   if (c != '/')   LEDA_EXCEPTION(1,"rational input: syntax error."); 

   do in.get(c); while (isspace(c));
   in.putback(c);

   integer ry;
   in >> ry;

   r = rational(rx,ry);

   return in;
}



rational small_rational_between(const rational& X, const rational& Y)
{  
  // This procedure is similar to Figures 6, 7 in the paper by 
  // Canny et al:
  // "A Rational Rotation Method for Robust Geometric Algorithms"
  // (Proc. of the 8th ACM Symposium on Computational Geometry, 
  //  pages 251-260, 1992)

  if (Y < X) 
    LEDA_EXCEPTION(1, "small_rational_between: precondition violated");

  rational floor_y = floor(Y);

  if (floor_y >= X) return floor_y;
 
  integer p0 = 0;
  integer q0 = 1;
  integer p1 = 1;
  integer q1 = 1;
  integer r;

  rational x = X-floor_y;
  rational y = Y-floor_y;
  rational result;

  //cout << "before for loop !\n"; cout.flush();

/*
  // old version has trouble with g++ 3.0.x
  for(;;) {  

    // invariant (I):    0 <= p0/q0 < x <= y < p1/q1 <= 1

    r = floor((rational(p1)-rational(q1)*x)/(rational(q0)*x-rational(p0)));
    p1 = r*p0+p1; 
    q1 = r*q0+q1;
    if ( rational(p1,q1) <= y)
    { result = rational(p1+q1*floor(Y),q1); 
      break;
    }

    // Invariant (I) from above holds again 

    r = floor((rational(q0)*y-rational(p0))/(rational(p1)-rational(q1)*y));
    p0 = r*p1+p0; 
    q0 = r*q1+q0;
    if (rational(p0,q0) >= x)
    { result = rational(p0+q0*floor(Y),q0);
      break;
    }
  } 
*/

  for(;;) {  

    // invariant (I):    0 <= p0/q0 < x <= y < p1/q1 <= 1
    rational rp1(p1);
    rational rq1(q1);
    rational rq0(q0);
    rational rp0(p0);

    r = floor((rp1-rq1*x)/(rq0*x-rp0));
    p1 = r*p0+p1; 
    q1 = r*q0+q1;
    if ( rational(p1,q1) <= y)
    { result = rational(p1+q1*floor(Y),q1); 
      break;
    }

    // Invariant (I) from above holds again 
    rp1 = rational(p1);
    rq1 = rational(q1);

    r = floor((rq0*y-rp0)/(rp1-rq1*y));
    p0 = r*p1+p0; 
    q0 = r*q1+q0;
    if (rational(p0,q0) >= x)
    { result = rational(p0+q0*floor(Y),q0);
      break;
    }
  } 

  
  //cout << "after for loop !\n"; cout.flush();  
  
  if ((result > Y) || (result < X))
         LEDA_EXCEPTION(1, "small_rational_between: internal error.");

  return result;

}   

rational small_rational_near(const rational& X, rational epsilon)
{  
  // returns a small rational between X-epsilon and X+epsilon
  // precondition: epsilon >= 0

  if (sign(epsilon) < 0)
     LEDA_EXCEPTION(1,
        "small_rational_near: precondition violated");

  return small_rational_between(X-epsilon,X+epsilon);
}


LEDA_END_NAMESPACE
