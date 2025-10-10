/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  real_demo1.c
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

I.write_demo("Consider expressions A = (sqrt(x+5)+sqrt(x))*(sqrt(x+5)-sqrt(x))");
I.write_demo(1,"and                  B = A - 5");
I.write_demo("The value of A is 5 and the value of B is 0.");
I.write_demo("The demo asks you for an integer L, chooses a random \
integer with L decimal digits and computes the signs of A and B.");
 
while (true) { 

int L = I.read_int("L = "); 

integer x = 0;
while (L > 0)
{ x = x*10 + rand_int(0,9);
  L--;
}
float T = used_time();

real X = x;
real SX = sqrt(X);
real SXP = sqrt(X+5);
real A = (SXP + SX) * (SXP - SX);
real B = A - 5;

int A_sign = A.sign(); float TA = used_time(T);
int B_sign = B.sign(); float TB = used_time(T);


I.write_demo("The sign of A is ",A_sign);
I.write_demo(1,"This took ",TA," seconds.");

I.write_demo("The sign of B is ",B_sign);
I.write_demo(1,"This took ",TB," seconds.");


}

return 0;
}


