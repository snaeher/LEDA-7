/*******************************************************************************
+
+  LEDA 5.0  
+
+
+  Euler_demo.cpp
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/bigfloat.h>
#include <LEDA/core/IO_interface.h>
#include <LEDA/system/timer.h>

using namespace leda;



int main(){

int m;

IO_interface I("Euler's Number");

I.write_demo("We compute an approximation of Euler's number e = 2,71... \
with error less than 2^{-m}. You will be asked to input m. The approximation \
is computed by summing a sufficient number of terms of the series");

I.write_demo("1/1! + 1/2! + 1/3! + 1/4! +  ");

I.write_demo("using the bigfloat number type.");

m = I.read_int("m = ");

timer t;
t.start();

bigfloat::set_precision(2*m);   
bigfloat::set_rounding_mode(TO_ZERO);
bigfloat z = 2; 
integer fac = 2; 
int n = 2; 
while ( fac.length() < m + 3 )
  {  // fac = n! and z approximates 1/0! + 1/1! + ... + 1/(n-1)!
    z = z + 1/bigfloat(fac); 
    n++; fac = fac * n;
  }
// |z - e| <= 2^-{m+1} at this point

z = round(z,m+3,TO_NEAREST);

int d = (int) floor( (m+1)*log(2.0)/log(10.0) - 1 ); // decimal digits
if (d < 1) d = 1;

t.stop();

I.write_demo("z = " + z.to_string(d)); 

I.write_demo("The computation of z took ", t.elapsed_time(), " seconds.");

return 0;
}

