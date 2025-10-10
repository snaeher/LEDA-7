/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  real_demo2.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/real.h>
#include <LEDA/core/random.h>
#include <LEDA/core/IO_interface.h>


using namespace leda;


int main(){

IO_interface I("The Data Type real");

I.write_demo("We demonstrate the LEDA number type real which \
gives you exact computation with expressions involving roots.");

I.write_demo("This demo asks you for an integer k.");
  
I.write_demo("It will square the number two k times, add 1, and then take \
roots k times. Finally, it will subtract 2 and output the \
sign of the result");
I.write_demo("The result has its first nonzero bit about 2**k places \
after the binary point.");
I.write_demo("Warning: Try small values of k first, e.g., k = 6.");

 
while (true) { 
int k = I.read_int("k = ");

float T = used_time();

real E = 2; 
int i;
for (i = 0; i < k; i++)  { E = E*E; }
E = E + 1;
for (i = 0; i < k; i++) { E = sqrt(E); }
E = E - 2;

I.write_demo("The sign of E is ",E.sign(),".");
I.write_demo("This took ",used_time(T)," seconds.");
I.write_demo("An approximation of E: " + E.to_bigfloat().to_string()); 

}

return 0;
}

