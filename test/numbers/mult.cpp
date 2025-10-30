/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mult.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/numbers/integer.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;

using std::cout;
using std::cin;
using std::endl;
using std::flush;



int main(int argc, char** argv)
{ 

  integer::use_karatsuba = false;

  //int n = 25000;
  int n = 100000;

/*
  if (argc > 1) n = atoi(argv[1]);
*/

  if (argc > 1) { 
     if (string(argv[1]) == "-k") integer::use_karatsuba = true;
  }

  rand_int.set_seed(n);


  integer a = integer::random(n);

  a =  a | (integer(1) << n);


  float T0 = 0;

  cout << endl;
  cout << endl;

  for(;;)
  {
     cout << string("n = %8d  ",n) << flush;
     float T = used_time();
     a = a*a;
     float T1 = used_time(T);
     cout << string("%6.2f sec   ",T1);
     if (T0 != 0) cout << string("f = %.2f",T1/T0); 
     cout << endl;
     n = 2*n;
     T0 = T1;
  }

return sign(a);
}


