/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _real_vector.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Vectors of real numbers
//
// S. Thiel (2003)
//------------------------------------------------------------------------------

#include <LEDA/numbers/real_vector.h>

LEDA_BEGIN_NAMESPACE

void real_vector::check_dimensions(const real_vector& vec) const
{ if (d!=vec.d)
   LEDA_EXCEPTION(1,"real_vector arguments have different dimensions.");
 }


real_vector::real_vector() 
{ d = 0; 
  v = nil;
 }


real_vector::real_vector(int n) 
{ 
 if (n<0) LEDA_EXCEPTION(1,"real_vector: negative dimension."); 
 d = n; 
 v = nil;
 if (d > 0)
 { v = (real*)std_memory.allocate_bytes(d*sizeof(real));
   real* p = v+d;
   while (p > v) new(--p) real(0);
 }
}


real_vector::~real_vector() 
{ if (v) 
  { real* p = v+d;
    while(p > v) (--p)->~real();
    std_memory.deallocate_bytes(v,d*sizeof(real));
  }
}


real_vector::real_vector(const real_vector& p) 
{ d = p.d; 
  v = nil;
  if (d > 0)
 { v = (real*)std_memory.allocate_bytes(d*sizeof(real));
   real* e = v+d; real* f = p.v+d;
   while (e > v) new(--e) real(*(--f));
 }
}

real_vector::real_vector(real x, real y) 
{ v = (real*)std_memory.allocate_bytes(2*sizeof(real));
  d = 2;
  new (v  ) real(x);
  new (v+1) real(y);
 }

real_vector::real_vector(real x, real y, real z) 
{ v = (real*)std_memory.allocate_bytes(3*sizeof(real));
  d = 3;
  new (v  ) real(x);
  new (v+1) real(y);
  new (v+2) real(z);
 }

real_vector::real_vector(double x, double y) 
{ v = (real*)std_memory.allocate_bytes(2*sizeof(real));
  d = 2;
  new (v  ) real(x);
  new (v+1) real(y);
 }

real_vector::real_vector(double x, double y, double z) 
{ v = (real*)std_memory.allocate_bytes(3*sizeof(real));
  d = 3;
  new (v  ) real(x);
  new (v+1) real(y);
  new (v+2) real(z);
 }


real_vector::real_vector(const vector& w, int prec) 
{ d = w.dim(); 
  v = nil;
  if (d > 0)
 { v = (real*)std_memory.allocate_bytes(d*sizeof(real));
   for(int i=0; i<d; ++i) {
     double val = w[i];
	 if (prec > 0) {
	   int exp; double fraction;
	   fraction = frexp(val,&exp);
	   val = truncate(fraction,exp + prec);
	 }
     new(v+i) real(val);
   }
 }
}

real_vector real_vector::rotate90(int i) const
{ if (d !=2)  LEDA_EXCEPTION(1,"real_vector::rotate90: dimension must be two. ");

  int r = (i % 4);

  real_vector vec;

  switch (r) {
  case  0: vec = real_vector( v[0], v[1]);
           break;
  case  1:
  case -3: vec = real_vector(-v[1], v[0]);
           break;
  case  2:
  case -2: vec = real_vector(-v[0],-v[1]);
           break;
  case  3:
  case -1: vec = real_vector( v[1],-v[0]);
           break;
  }
  return vec;
}

/*
real_vector real_vector::rotate(double fi ) const
{ if (d !=2)  LEDA_EXCEPTION(1,"real_vector::rotate: dimension must be two. ");
  double sinfi = sin(fi);
  double cosfi = cos(fi);
  return real_vector(v[0]*cosfi-v[1]*sinfi,v[0]*sinfi+v[1]*cosfi);
}
*/

real  real_vector::operator[](int i) const
{ if (i<0 || i>=d)  LEDA_EXCEPTION(1,"real_vector: index out of range ");
  return v[i]; 
}

real& real_vector::operator[](int i)
{ if (i<0 || i>=d)  LEDA_EXCEPTION(1,"real_vector: index out of range ");
  return v[i]; 
}


real_vector& real_vector::operator+=(const real_vector& vec)
{ check_dimensions(vec);
  int n = d;
  while (n--) v[n] += vec.v[n];
  return *this;
}

real_vector& real_vector::operator-=(const real_vector& vec)
{ check_dimensions(vec);
  int n = d;
  while (n--) v[n] -= vec.v[n];
  return *this;
}

real_vector real_vector::operator+(const real_vector& vec) const
{ check_dimensions(vec);
  int n = d;
  real_vector result(n);
  while (n--) result.v[n] = v[n]+vec.v[n];
  return result;
}

real_vector real_vector::operator-(const real_vector& vec) const
{ check_dimensions(vec);
  int n = d;
  real_vector result(n);
  while (n--) result.v[n] = v[n]-vec.v[n];
  return result;
}

real_vector real_vector::operator-() const  // unary minus
{ int n = d;
  real_vector result(n);
  while (n--) result.v[n] = -v[n];
  return result;
}


real_vector real_vector::operator*(real x) const
{ int n = d;
  real_vector result(n);
  while (n--) result.v[n] = v[n] * x;
  return result;
}

real_vector& real_vector::operator*=(real r)
{ int n = d;
  while (n--) v[n] *= r;
  return *this;
}

real_vector real_vector::operator/(real x) const
{ int n = d;
  real_vector result(n);
  while (n--) result.v[n] = v[n] / x;
  return result;
}

real_vector& real_vector::operator/=(real r)
{ int n = d;
  while (n--) v[n] /= r;
  return *this;
}

real real_vector::operator*(const real_vector& vec) const
{ check_dimensions(vec);
  real result=0;
  int n = d;
  while (n--) result = result+v[n]*vec.v[n];
  return result;
}

real_vector& real_vector::operator=(const real_vector& vec)
{ 
  if (d != vec.d)
  { if (v)
      std_memory.deallocate_bytes(v,d*sizeof(real));

    d = vec.d;

	if (d > 0) {
      v = (real*)std_memory.allocate_bytes(d*sizeof(real));
      real* p = v+d;
      while (p > v) new(--p) real(0);
	}
    else
      v = 0;
   }

  for(int i=0; i<d; i++) v[i] = vec.v[i];

  return *this;
}


bool real_vector::operator==(const real_vector& vec)  const
{ if (vec.d != d) return false;
  int i = 0;
  while ((i<d) && (v[i]==vec.v[i])) i++;
  return (i==d) ? true : false;
 }



void real_vector::read(istream& is) 
{ for(int i=0;i<d;i++) is  >> v[i]; }

void real_vector::print(ostream& os) 
{ os << "(";
  for(int i=0;i<d;i++) os << string(" %6.2f",v[i].to_close_double());
  os << " )";
}


ostream& operator<<(ostream& os, const real_vector& v)
{ os << v.d << " ";
  for (int i=0;i<v.d;i++) os << " " << v[i];
  return os;
}


istream& operator>>(istream& is, real_vector& v) 
{ int d;
  is >> d;
  real_vector w(d);
  for (int i=0;i<d;i++) is >> w[i];
  v = w;
  return is; 
} 



real real_vector::sqr_length() const { return *this * *this; }

real real_vector::length() const { return sqrt(sqr_length()); }

/*
double real_vector::angle(const real_vector& y)  const
{ 
  const real_vector& x = *this;

  real L = x.length() * y.length();

  if (L == 0)
    LEDA_EXCEPTION(1,"angle: zero argument\n");

  real cosin = (x*y)/L;
  double cosin_d = cosin.to_close_double();

  if (cosin_d < -1) cosin_d = -1;
  if (cosin_d >  1) cosin_d =  1;

  return acos(cosin_d);
}
*/

/*
inline int Sign(const double& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}
*/

vector real_vector::to_vector() const
{
	vector vec(dim());
	for (int i = 0; i < dim(); ++i) {
		vec[i] = coord(i).to_close_double();
	}
	return vec;
}

int compare_by_angle(const real_vector& v1, const real_vector& v2)
{ real x1 = v1.coord(0);
  real y1 = v1.coord(1);

  real x2 = v2.coord(0);
  real y2 = v2.coord(1);

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


real_vector cross_product(const real_vector& v1, const real_vector& v2)
{
  real a1 = v1.xcoord();
  real a2 = v1.ycoord();
  real a3 = v1.zcoord();
  real b1 = v2.xcoord();
  real b2 = v2.ycoord();
  real b3 = v2.zcoord();    
    
  real vx = a2*b3-a3*b2;
  real vy = a3*b1-a1*b3;
  real vz = a1*b2-a2*b1;

  return real_vector(vx,vy,vz);
}


LEDA_END_NAMESPACE
