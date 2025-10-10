/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  clos_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include<LEDA/core/random.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main()
{
  int N = read_int("N = ");

  random_source S(1,10000000);

  S.set_seed(N);

  list<point>     L0;
  list<rat_point> L1;


  for (int i=1; i <= N; i++)
  { int x,y;
    S >> x >> y;
    L0.append(point(x,y));
    L1.append(rat_point(x,y));
   }

  { point p1,p2;
    cout << "CLOSEST_PAIR (double)   " << flush;
    float T = used_time();
    double d = CLOSEST_PAIR(L0,p1,p2);
    float t = used_time(T);
    cout << p1 << "   " << p2 << "   d= " << d << " (" << t << " sec)" << endl;
    cout << endl;
   }

  { rat_point p1,p2;
    cout << "CLOSEST_PAIR (rational) " << flush;
    float T = used_time();
    rational d = CLOSEST_PAIR(L1,p1,p2);
    float t = used_time(T);
    cout << p1 << " " << p2 << " d= " << d << " (" << t << " sec)" << endl;
    cout << endl;
   }

  return 0;
}

