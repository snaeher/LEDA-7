/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  braided_lines.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

IO_interface I("Braided Lines");

I.write_demo("This demo illustrates the danger of floating point \
arithmetic in geometric computation. We evaluate the line equations");

I.write_demo("l1: y = 9833*x/9454");
I.write_demo("l2: y = 9366*x/9005");

I.write_demo("for x = 0, x < 1, in steps of 0.001. Both lines pass \
pass through the origin and l1 is slightly steeper than l2. \
Whenever the relationship \
between the y-values changes we output x and the new relationship.");
int last_comp = -1;
float a = 9833; float b = 9454;
float c = 9366; float d = 9005;
float xmax = 0; 
int number_of_changes = 0;
for (float x = 0; x < 1; x = x + 0.001f)
{ float y1 = a*x/b;   // l1 is steeper
  float y2 = c*x/d;

  int comp = (y1 < y2? -1 : (y1 == y2? 0 : +1));
  if (comp != last_comp)
  { string s;
    if (comp == -1) { xmax = x; s = "l1 is below l2"; }
    if (comp ==  0) { xmax = x; s = "l1 intersects l2"; }
    if (comp == +1) s = "l1 is above l2";
    I.write_demo("At x = ",x,": " + s);
    number_of_changes++;
    if ( number_of_changes%10 == 0 ) I.stop();
  } 
  last_comp = comp;
}

I.write_demo("The largest value of x, for which l1 is not above l2 \
is equal to ",xmax);

return 0;
}

