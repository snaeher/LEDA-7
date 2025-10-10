/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _rat_vector.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/numbers/rat_vector.h>
#include <LEDA/numbers/integer_matrix.h>

LEDA_BEGIN_NAMESPACE

rat_vector::rat_vector(int d) 
{ PTR = new geo_rep(d);
  for(int i=0; i<d; i++) ptr()->v[d] = 0;
  ptr()->v[d] = 1;
}

rat_vector::rat_vector(integer a,integer b,integer D)
{ if (D==0)
    LEDA_EXCEPTION(1,"rat_vector::constructor: D must be nonzero.");
  if (D<0)
    PTR = new geo_rep(-a,-b,-D);
  else
    PTR = new geo_rep(a,b,D);
}


rat_vector::rat_vector(rational a, rational b)
{ 
  integer a_num = a.numerator();
  integer b_num = b.numerator();
  integer a_den = a.denominator();
  integer b_den = b.denominator();

  PTR = new geo_rep(a_num*b_den, b_num*a_den, a_den*b_den);
}

rat_vector::rat_vector(rational a, rational b, rational c)
{ 
  integer a_num = a.numerator();
  integer b_num = b.numerator();
  integer c_num = c.numerator();
  integer a_den = a.denominator();
  integer b_den = b.denominator();
  integer c_den = c.denominator();

  PTR = new geo_rep(a_num*b_den*c_den, b_num*a_den*c_den, c_num*a_den*b_den,
                    a_den*b_den*c_den);
}


rat_vector::rat_vector(integer a,integer b,integer c,integer D)
{ if (D==0)
    LEDA_EXCEPTION(1,"rat_vector::constructor: D must be nonzero.");
  if (D<0)
    PTR = new geo_rep(-a,-b,-c,-D);
  else
    PTR = new geo_rep(a,b,c,D);
}



rat_vector::rat_vector(const array<rational>& B)
{
  array<rational> A = B;

  integer_vector iv(A.size());
  integer D = 1;
  int i;

  for(i=A.low(); i<=A.high(); i++)
  { A[i].normalize();
    D *= A[i].denominator();
   }

  for(i=A.low(); i<=A.high(); i++)
  { rational r = A[i];
    iv[i] = r.numerator() * (D/r.denominator());
   }

  PTR = new geo_rep(iv,D);
}





rat_vector::rat_vector(const integer_vector&c,integer D)
{ if (D==0)
    LEDA_EXCEPTION(1,"rat_vector::constructor: D must be nonzero.");
  if (D<0)
    PTR = new geo_rep(-c,-D);
  else
    PTR = new geo_rep(c,D);
}

rat_vector::rat_vector(const integer_vector&c)
{ int d = c.dim();
  integer D = c[d];
  if (D==0)
    LEDA_EXCEPTION(1,"rat_point::constructor: D must be nonzero");
  if (D<0)
    PTR = new geo_rep(-c);
  else
    PTR = new geo_rep(c);
}




// sn (02/1997)
rational  rat_vector::sqr_length() const
{ int      d = ptr()->dim;
  integer* V = ptr()->v;
  integer nom = 0;
  for(int i=0; i<d; i++) nom += V[i]*V[i];
  integer den = V[d]*V[d];
  return rational(nom,den);
}

// sn (02/1997)
vector rat_vector::to_vector() const
{ int      d = ptr()->dim;
  integer* V = ptr()->v;
  vector vec(d);
// sn (06/2007): changed to avoid overflow problems
/*
  double   w = V[d].to_double();
  for(int i=0; i<d; i++) vec[i] = V[i].to_double()/w;
*/
  integer  W = V[d];
  for(int i=0; i<d; i++) vec[i] = double_quotient(V[i],W);
  return vec;
}


rat_vector rat_vector::rotate90(int i) const
{ if (ptr()->dim != 2)
      LEDA_EXCEPTION(1,"rat_vector::rotate90: dimension must be two. ");

  int r = (i % 4);

  integer x = hcoord(0);
  integer y = hcoord(1);
  integer w = hcoord(2);

  rat_vector vec;

  switch (r) {
  case  0: vec = rat_vector( x, y, w);
           break;
  case  1:
  case -3: vec = rat_vector(-y, x, w);
           break;
  case  2:
  case -2: vec = rat_vector(-x,-y, w);
           break;
  case  3:
  case -1: vec = rat_vector( y,-x, w);
           break;
  }
  return vec;
}




rat_vector rat_vector::d2(integer a, integer b, integer D)
{
  if (D == 0)
    LEDA_EXCEPTION(1, "rat_vector::d2: denominator must not be zero.");
  return rat_vector(a, b, D);
}

rat_vector rat_vector::d3(integer a, integer b, integer c, integer D)
{
  rat_vector d3v(3);
  if (D == 0)
    LEDA_EXCEPTION(1, "rat_vector::d3: denominator must not be zero.");
  if (D < 0)
    d3v.ptr()->init4(-a, -b, -c, -D);
  else
    d3v.ptr()->init4(a, b, c, D);
  return d3v;
}

rat_vector rat_vector::unit(int i, int d)
{
  if (i < 0 || i >= d)
    LEDA_EXCEPTION(1, "rat_vector::unit: i out of range.");
  rat_vector uv(d);
  uv.ptr()->v[i] = 1;
  return uv;
}

rat_vector rat_vector::zero(int d)
{
  return rat_vector(d);
}





void rat_vector::print(ostream & out) const
{
  out << ptr();
}

void rat_vector::read(istream & in)
{
  int d = dim();
  if (refs() > 1)
    operator = (rat_vector(d));
  in >> ptr();
  if (hcoord(d) == 0)
    LEDA_EXCEPTION(1, "operator>>: denominator of vector must be nonzero.");
  if (hcoord(d) < 0)
    ptr()->negate(d + 1);
}






rat_vector rat_vector::scale(integer m, integer n) const
{
  int d = dim();
  rat_vector result(d);
  result.ptr()->v[d] = ptr()->v[d] * n;
  integer g = gcd(ptr()->v[d], m);
  result.ptr()->v[d] /= g;
  m /= g;
  for (int i = 0; i < d; i++)
    result.ptr()->v[i] = ptr()->v[i] * m;
  return result;
}

void rat_vector::self_scale(integer m, integer n)
{
  int d = dim();
  ptr()->v[d] *= n;
  integer g = gcd(ptr()->v[d], m);
  ptr()->v[d] /= g;
  m /= g;
  for (int i = 0; i < d; i++)
    ptr()->v[i] *= m;
}

rat_vector operator *(int n, const rat_vector & p) {
  return p.scale(n, 1);
}

rat_vector operator *(integer n, const rat_vector & p) {
  return p.scale(n, 1);
}

rat_vector operator *(rational r, const rat_vector & p) {
  return p.scale(r.numerator(), r.denominator());
}


rat_vector operator / (const rat_vector & p, int n) {
  return p.scale(1, n);
}

rat_vector operator / (const rat_vector & p, integer n) {
  return p.scale(1, n);
}

rat_vector operator / (const rat_vector & p, rational r) {
  return p.scale(r.denominator(), r.numerator());
}
rat_vector & rat_vector::operator *= (integer n) {
  self_scale(n, 1);
  return *this;
}
rat_vector & rat_vector::operator *= (int n) {
  self_scale(n, 1);
  return *this;
}
rat_vector & rat_vector::operator *= (rational r) {
  self_scale(r.numerator(), r.denominator());
  return *this;
}
rat_vector & rat_vector::operator /= (integer n) {
  self_scale(1, n);
  return *this;
}
rat_vector & rat_vector::operator /= (int n) {
  self_scale(1, n);
  return *this;
}
rat_vector & rat_vector::operator /= (rational r) {
  self_scale(r.denominator(), r.numerator());
  return *this;
}


rational operator *(const rat_vector v, const rat_vector & w) {
  int d = v.dim();
  if (d != w.dim())
    LEDA_EXCEPTION(1, "inner product: dimensions disagree.");
  integer nom = 0;
  for (int i = 0; i < d; i++)
    nom += v.hcoord(i) * w.hcoord(i);
  integer denom = v.hcoord(d) * w.hcoord(d);
  return rational(nom, denom);
}




rat_vector operator + (const rat_vector & v, const rat_vector & w) {
  rat_vector res(v.dim());
  c_add(res.ptr(), v.ptr(), w.ptr());
  return res;
}
rat_vector & rat_vector::operator += (const rat_vector & w) {
  int d = dim();
  rat_vector old(*this);
  if (ptr()->ref_num() > 2)
    *this = rat_vector(d);
  c_add(ptr(), old.ptr(), w.ptr());
  return *this;
}

rat_vector operator - (const rat_vector & v, const rat_vector & w) {
  rat_vector res(v.dim());
  c_sub(res.ptr(), v.ptr(), w.ptr());
  return res;
}
rat_vector & rat_vector::operator -= (const rat_vector & w) {
  int d = dim();
  rat_vector old(*this);
  if (ptr()->ref_num() > 2)
    *this = rat_vector(d);
  c_sub(ptr(), old.ptr(), w.ptr());
  return *this;
}

rat_vector rat_vector::operator - () const
{
  int d = dim();
  rat_vector result(d);
  result.ptr()->copy(ptr());
  result.ptr()->negate(d);
  return result;
}





bool contained_in_linear_hull(const array <rat_vector > &A,
   const rat_vector & x)
{
  int al = A.low();
  int k = A.high() - al + 1;	/* |A| contains |k| vectors */
  int d = A[al].dim();
  integer_matrix M(d, k);
  integer_vector b(d);
  for (int i = 0; i < d; i++) {
    b[i] = x.hcoord(i);
    for (int j = 0; j < k; j++)
      M(i, j) = A[al + j].hcoord(i);
  }
  return is_solvable(M, b);
}



int linear_rank(const array <rat_vector > &A)
{
  int al = A.low();
  int k = A.high() - al + 1;	/* |A| contains |k| vectors */
  int d = A[al].dim();
  integer_matrix M(d, k);
  for (int i = 0; i < d; i++)
    for (int j = 0; j < k; j++)
      M(i, j) = A[al + j].hcoord(i);
  return rank(M);
}




bool linearly_independent(const array <rat_vector > &A)
{
  return (linear_rank(A) == A.high() - A.low() + 1);
}




array <rat_vector > linear_base(const array <rat_vector > &A)
{
  int al = A.low();
  int k = A.high() - al + 1;	/* |A| contains |k| vectors */
  int d = A[al].dim();
  integer denom;
  integer_matrix M(d, k);
  for (int j = 0; j < k; j++)
  { for (int i = 0; i < d; i++)
      M(i, j) = A[al + j].hcoord(i);
   }

  array <int >indcols;
  independent_columns(M, indcols);

  int indcolsdim = indcols.high() + 1;
  array <rat_vector > L(indcolsdim);
  for (int i = 0; i < indcolsdim; i++)
    L[i] = rat_vector(M.col(indcols[i]), 1);
  return L;
}


int compare_by_angle(const rat_vector& v1, const rat_vector& v2)
{ const integer& x1 = v1.hcoord(0);
  const integer& y1 = v1.hcoord(1);

  const integer& x2 = v2.hcoord(0);
  const integer& y2 = v2.hcoord(1);

  if ( x1 == 0 && y1 == 0 ) return (x2 == 0 && y2 == 0) ? 0 : -1;
  if ( x2 == 0 && y2 == 0 ) return 1;

  // both vectors are non-zero 

  int sy1 = sign(y1); 
  int sy2 = sign(y2);

  int upper1 = ( sy1 != 0 ? sy1 : sign(x1) );
  int upper2 = ( sy2 != 0 ? sy2 : sign(x2) );

  if ( upper1 == upper2 ) return sign(x2*y1 - x1*y2);

  return upper2 - upper1;
}

rat_vector cross_product(const rat_vector& v1, const rat_vector& v2)
{
  integer a1 = v1.X();
  integer a2 = v1.Y();
  integer a3 = v1.Z();
  integer b1 = v2.X();
  integer b2 = v2.Y();
  integer b3 = v2.Z();    
    
  integer vx = a2*b3-a3*b2;
  integer vy = a3*b1-a1*b3;
  integer vz = a1*b2-a2*b1;
  integer vw = v1.W()*v2.W();

  return rat_vector(vx,vy,vz,vw);
}




LEDA_END_NAMESPACE
