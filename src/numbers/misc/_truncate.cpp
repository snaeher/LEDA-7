/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _truncate.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// truncates the mantissa of x after k binary places

#include <LEDA/numbers/integer.h>

LEDA_BEGIN_NAMESPACE

double truncate(double x, int k)
{ if ( x == 0 ) return 0;
  if ( k >= 52 ) return x;
  int exp;
  double result = frexp(x,&exp);
  if ( k <= 30 )
  { result = result * (1 << k);
    result = (int) result;
    result = result / (1 << k);  
    return ldexp(result,exp);
  }
  result = result * (1 << 30);
  int result1 = (int ) result;
  result = result - result1;   // bit 31 to ...
  result = result1 + truncate(result,k - 30);
  result = result / (1 << 30);
  return ldexp(result,exp);
}

integer truncate(integer x, int /*k*/)
{ return x; }

LEDA_END_NAMESPACE
