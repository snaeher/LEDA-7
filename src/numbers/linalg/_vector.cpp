/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _vector.c
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
// S. Naeher (1996)
//------------------------------------------------------------------------------

#include <LEDA/numbers/vector.h>

LEDA_BEGIN_NAMESPACE

void vector::check_dimensions(const vector& vec) const
{ if (d!=vec.d)
   LEDA_EXCEPTION(1,"vector arguments have different dimensions.");
 }


vector::vector() 
{ d = 0; 
  v = nil;
 }


vector::vector(int n) 
{ 
 if (n<0) LEDA_EXCEPTION(1,"vector: negative dimension."); 
 d = n; 
 v = nil;
 if (d > 0)
 { v = (double*)std_memory.allocate_bytes(d*sizeof(double));
   for(int i=0; i<d; i++) 
     v[i] = 0.0;
  }
}


vector::~vector() 
{ if (v) std_memory.deallocate_bytes(v,d*sizeof(double)); }


vector::vector(const vector& p) 
{ d = p.d; 
  v = nil;
  if (d > 0)
  { v = (double*)std_memory.allocate_bytes(d*sizeof(double));
    for(int i=0; i<d; i++) v[i] = p.v[i];
   }
}



vector::vector(double x, double y) 
{ v = (double*)std_memory.allocate_bytes(2*sizeof(double));
  d = 2;
  v[0] = x;
  v[1] = y;
 }

vector::vector(double x, double y, double z) 
{ v = (double*)std_memory.allocate_bytes(3*sizeof(double));
  d = 3;
  v[0] = x;
  v[1] = y;
  v[2] = z;
 }


vector vector::rotate90(int i) const
{ if (d !=2)  LEDA_EXCEPTION(1,"vector::rotate90: dimension must be two. ");

  int r = (i % 4);

  vector vec;

  switch (r) {
  case  0: vec = vector( v[0], v[1]);
           break;
  case  1:
  case -3: vec = vector(-v[1], v[0]);
           break;
  case  2:
  case -2: vec = vector(-v[0],-v[1]);
           break;
  case  3:
  case -1: vec = vector( v[1],-v[0]);
           break;
  }
  return vec;
}

vector vector::rotate(double fi ) const
{ if (d !=2)  LEDA_EXCEPTION(1,"vector::rotate: dimension must be two. ");
  double sinfi = sin(fi);
  double cosfi = cos(fi);
  return vector(v[0]*cosfi-v[1]*sinfi,v[0]*sinfi+v[1]*cosfi);
}


double  vector::operator[](int i) const
{ if (i<0 || i>=d)  LEDA_EXCEPTION(1,"vector: index out of range ");
  return v[i]; 
}

double& vector::operator[](int i)
{ if (i<0 || i>=d)  LEDA_EXCEPTION(1,"vector: index out of range ");
  return v[i]; 
}


vector& vector::operator+=(const vector& vec)
{ check_dimensions(vec);
  int n = d;
  while (n--) v[n] += vec.v[n];
  return *this;
}

vector& vector::operator-=(const vector& vec)
{ check_dimensions(vec);
  int n = d;
  while (n--) v[n] -= vec.v[n];
  return *this;
}

vector vector::operator+(const vector& vec) const
{ check_dimensions(vec);
  int n = d;
  vector result(n);
  while (n--) result.v[n] = v[n]+vec.v[n];
  return result;
}

vector vector::operator-(const vector& vec) const
{ check_dimensions(vec);
  int n = d;
  vector result(n);
  while (n--) result.v[n] = v[n]-vec.v[n];
  return result;
}

vector vector::operator-() const  // unary minus
{ int n = d;
  vector result(n);
  while (n--) result.v[n] = -v[n];
  return result;
}


vector vector::operator*(double x) const
{ int n = d;
  vector result(n);
  while (n--) result.v[n] = v[n] * x;
  return result;
}

vector& vector::operator*=(double r)
{ int n = d;
  while (n--) v[n] *= r;
  return *this;
}

vector vector::operator/(double x) const
{ int n = d;
  vector result(n);
  while (n--) result.v[n] = v[n] / x;
  return result;
}

vector& vector::operator/=(double r)
{ int n = d;
  while (n--) v[n] /= r;
  return *this;
}

double vector::operator*(const vector& vec) const
{ check_dimensions(vec);
  double result=0;
  int n = d;
  while (n--) result = result+v[n]*vec.v[n];
  return result;
}

vector& vector::operator=(const vector& vec)
{ 
  if (d != vec.d)
  { if (v)
      std_memory.deallocate_bytes(v,d*sizeof(double));

    d = vec.d;

    if (d > 0)
      v = (double*)std_memory.allocate_bytes(d*sizeof(double));
    else
      v = 0;
   }

  for(int i=0; i<d; i++) v[i] = vec.v[i];

  return *this;
}


bool vector::operator==(const vector& vec)  const
{ if (vec.d != d) return false;
  int i = 0;
  while ((i<d) && (v[i]==vec.v[i])) i++;
  return (i==d) ? true : false;
 }


void vector::read(istream& is) 
{ for(int i=0;i<d;i++) is  >> v[i]; }

void vector::print(ostream& os) 
{ os << "(";
  for(int i=0;i<d;i++) os << string(" %6.2f",v[i]);
  os << " )";
}



ostream& operator<<(ostream& os, const vector& v)
{ os << v.d << " ";
  for (int i=0;i<v.d;i++) os << " " << v[i];
  return os;
}


istream& operator>>(istream& is, vector& v) 
{ int d;
  is >> d;
  vector w(d);
  for (int i=0;i<d;i++) is >> w[i];
  v = w;
  return is; 
} 


double vector::sqr_length() const { return *this * *this; }

double vector::length() const { return sqrt(sqr_length()); }


double vector::angle(const vector& y)  const
{ 
  const vector& x = *this;

  double L = x.length() * y.length();

  if (L == 0)
    LEDA_EXCEPTION(1,"angle: zero argument\n");

  double cosin = (x*y)/L;

  if (cosin < -1) cosin = -1;
  if (cosin >  1) cosin =  1;

  return acos(cosin);
}


inline int Sign(const double& x)
{ if ( x > 0 ) return 1;
  if ( x < 0 ) return -1;
  return 0;
}

int compare_by_angle(const vector& v1, const vector& v2)
{ double x1 = v1.coord(0);
  double y1 = v1.coord(1);

  double x2 = v2.coord(0);
  double y2 = v2.coord(1);

  if ( x1 == 0 && y1 == 0 ) return (x2 == 0 && y2 == 0) ? 0 : -1;
  if ( x2 == 0 && y2 == 0 ) return 1;

  // both vectors are non-zero 

  int sy1 = Sign(y1); 
  int sy2 = Sign(y2);

  int upper1 = ( sy1 != 0 ? sy1 : Sign(x1) );
  int upper2 = ( sy2 != 0 ? sy2 : Sign(x2) );

  if ( upper1 == upper2 ) return Sign(x2*y1 - x1*y2);

  return upper2 - upper1;
}


vector cross_product(const vector& v1, const vector& v2)
{
  double a1 = v1.xcoord();
  double a2 = v1.ycoord();
  double a3 = v1.zcoord();
  double b1 = v2.xcoord();
  double b2 = v2.ycoord();
  double b3 = v2.zcoord();    
    
  double vx = a2*b3-a3*b2;
  double vy = a3*b1-a1*b3;
  double vz = a1*b2-a2*b1;

  return vector(vx,vy,vz);
}


LEDA_END_NAMESPACE

