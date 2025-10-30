/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  area_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include  <LEDA/geo/rat_polygon.h>
#include  <LEDA/geo/rat_circle.h>

using namespace leda;

using std::cout;
using std::endl;


int main()
{
   int N = 1000;

   rat_circle C(rat_point(0,0,1),rat_point(N,0,1));

   for(int i=1; i<10; i++)
   { rat_polygon pol = reg_n_gon(i*N,C,0.0001);

     float T = used_time();

     double A0 = pol.area0().to_float();
     cout << string("area0 = %.4f    time = %.2f",A0,used_time(T)) << endl;

     double A1 = pol.area1().to_float();
     cout << string("area1 = %.4f    time = %.2f",A1,used_time(T)) << endl;

     double A = pol.area().to_float();
     cout << string("area  = %.4f    time = %.2f",A, used_time(T)) << endl;

     cout << endl;
    }

   return 0;
}
