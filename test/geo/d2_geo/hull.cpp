/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include <fstream.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main(int argc, char** argv)
{

  list<rat_point>  L2;
  int N;

  if (argc == 2)
  { ifstream in(argv[1]);
    L2.read(in);
    N = L2.length();
   }
  else
  { N = read_int("N = ");
     random_points_on_circle(N,10000,L2);
     //random_points_in_disc(N,10000,L2);
   }

  cout << endl;
  cout << "N = " << N << endl;
  cout << endl;


 { cout << "CONVEX_HULL_IC(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  }
 rat_point::float_computation_only = 1;
 { cout << "CONVEX_HULL_IC(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  }
 rat_point::float_computation_only = 0;
 { cout << "CONVEX_HULL_IC1(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC1(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  }
  cout << endl;




  rat_point::print_statistics();
  cout << endl;

  return 0;
}
