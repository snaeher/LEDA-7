/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  multiplication_times.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/numbers/integer.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;



int main()
{ integer n;
  
IO_interface I("Running Time of Multiplication");
I.write_demo("We illustrate the speed of the multiplication algorithm \
for long integers. You will be asked to input an integer n. We will \
square n 16 times. For the last 5 rounds we will report the length of \
the current n, the running time, and the ratio to the running time in \
the previous round.");

I.write_demo("Since squaring doubles the length of the binary \
representation, the number of digits in the final result is about \
2^16 = 64000 times the number of digits in the input.");
 
n = integer(I.read_int("n = ",1439683));


  int i;
  for (i = 0; i < 11; i++) n = n * n;  
  float T_prev = 0;

  for (i = 0; i <= 5; i++)
  { float T = used_time();
    n = n * n; 
    T = used_time(T); 
    
if (i > 0)
{ I.write_table("",n.length());
  I.write_table(" & ",T);
  I.write_table(" & ",T/T_prev);
  string separator = "\\\\ \\hline";
  if ( i == 5 ) separator += " \\hline";
  separator += " \n";
  I.write_table(separator); 
  I.write_demo("length of n =                               ",n.length());
  I.write_demo(1,"time for multiplication =                   ",T); 
  I.write_demo(1,"ratio to time for previous multiplication = ",T/T_prev);
}

    T_prev = T;
  }

return 0;
}


