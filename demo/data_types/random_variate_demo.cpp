/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  random_variate_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:17 $


#include <LEDA/core/random_variate.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main()
{ IO_interface I("Random Variates");

  I.write_demo("This demo illustrates the speed of classes \
random variate and dynamic random variate. \nYou will be asked \
to input integers n and N. We set up the weight vector w with \
w[2] = 2, w[3] = 3, ..., w[n+1] = n + 1 and generate N random \
variates according to this weight vector.");

  int n, N;
  n = I.read_int("n = ",100);  
  N = I.read_int("N = ",100000); 
  if ( n < 1 ) error_handler(1,"n must be at least one"); 
 
#ifdef BOOK
N = 10000000;
for (n = 100; n <= 10000; n = n*n)
{ I.write_table("\n ", n);
#endif
  array<int> w(2, 1 + n);
  array<double> Rfreq(2,n+1), Qfreq(2,n+1);
  int W = 0; int i;
  for (i = 2; i < n + 2; i++) { W += w[i] = i; Qfreq[i] = Rfreq[i] = 0; }
  
  dynamic_random_variate R(w);
  random_variate Q(w);

  float T = used_time(); float UT;
  for (i = 0; i < N; i++) Qfreq[Q.generate()]++;

  UT = used_time(T);
  I.write_demo("static random variate, time = ",UT);
  I.write_table(" & ",UT);

  for (i = 0; i < N; i++) Rfreq[R.generate()]++; 

  UT = used_time(T);
  I.write_demo("dynamic random variate, time = ",UT);
  I.write_table(" & ",UT, " \\\\ \\hline");

  I.write_demo("We report some frequencies.");

  for (i = n + 1; i >= leda_max(2,n - 3); i--) 
  { I.write_demo("relative frequency, i = ",i);
    I.write_demo(0,", w[i]/W = ",((double)w[i])/W);
    I.write_demo(1,"generated freq, static variate = ",  Qfreq[i]/N);
    I.write_demo(1,"generated freq, dynamic variate = ", Rfreq[i]/N);
  }

#ifdef BOOK
}
#endif
return 0;
}
