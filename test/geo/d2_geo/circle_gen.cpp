/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  circle_gen.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_circle.h>
#include <LEDA/numbers/rational.h>

using namespace leda;
using std::cout;

void test()
{
  point p(35.3914,76.7505);
  point q(35.3914,36.4218); 
  point r(55.5557,56.5862);

  cout << p << "\n";
  cout << q << "\n"; 
  cout << r << "\n";  
 
  rat_point rp(p), rq(q), rr(r);
  rat_circle R(rp,rq,rr);

  double eps = 0.001;
  double a= 0.628319;
  R.point_on_circle(a,eps);  // <--- crash mit g++ 3.0.x
}

int main()
{
 test();
 
 integer i1("11706210103582580"),i2("36028797018963968");
 integer i3("11706838924716060"),i4("36028797018963968");
 
 rational rt_low(i1,i2);
 rational rt_high(i3,i4);
 
 cout << rt_low << "\n";
 cout << rt_high << "\n";
 
 rational res = small_rational_between(rt_low, rt_high);
 
 cout << res << "\n";
 
 return 0;
}
