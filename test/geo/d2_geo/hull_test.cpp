/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  hull_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


template<class POINT>
bool check_hull(const list<POINT>& H)
{ int err = 0;
  list_item i;
  forall_items(i,H)
  { list_item j = H.cyclic_succ(i);
    list_item k = H.cyclic_succ(j);
    POINT a = H[i];
    POINT b = H[j];
    POINT c = H[k];
    if (orientation(a,b,c) <= 0) err++;
  }
  return err == 0;
}

int main()
{
  int N = read_int("N = ");

  list<point>      L1;
  list<rat_point>  L2;

  string gen;

  while (gen == "")
  { gen  = read_string("generator d(isk)/s(quare)/c(ircle) : ");
    if (gen[0] == 'd')
       random_points_in_disc(N,10000,L2);
    else if (gen[0] == 's')
       random_points_in_square(N,10000,L2);
    else if (gen[0] == 'c')
       random_points_on_circle(N,10000,L2);
    else gen = "";
  }

  L2.permute();

  rat_point p;
  forall(p,L2) L1.append(p.to_point());

  cout << endl;

 { cout << "PERMUTE(point)      " << flush;
   float T = used_time();
   L1.permute();
   cout << string("time = %.2f",used_time(T)) << endl;
  }

 { cout << "PERMUTE(rat_point)  " << flush;
   float T = used_time();
   L2.permute();
   cout << string("time = %.2f",used_time(T)) << endl;
  }

 cout << endl;

/*
 { cout << "CONVEX_HULL_RIC(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_RIC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  }

 { cout << "CONVEX_HULL_RIC1(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_RIC1(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   //assert(check_hull(C));
  }
 cout << endl;
*/

 { cout << "CONVEX_HULL_IC(point)       " << flush;
   float T = used_time();
   list<point> C = CONVEX_HULL_IC(L1);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   //assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_IC(rat_point)   " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 rat_point::use_static_filter = false;
 { cout << "CONVEX_HULL_IC(dyn_filter)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }
 rat_point::use_static_filter = true;

 rat_point::float_computation_only = 1;

 { cout << "CONVEX_HULL_IC(float_only)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   //assert(check_hull(C));
  }

 rat_point::float_computation_only = 0;

 cout << endl;

 { cout << "CONVEX_HULL_ICF(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_ICF(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_ICF2(rat_point) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_ICF2(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }


 rat_point::use_static_filter = false;
 { cout << "CONVEX_HULL_ICF(dyn_filter) " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_ICF(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }
 rat_point::use_static_filter = true;



  cout << endl;

 { cout << "CONVEX_HULL_IC1(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC1(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }
 { cout << "CONVEX_HULL_IC2(rat_point)  " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_IC2(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }


  cout << endl;

 { cout << "CONVEX_HULL_S(point)        " << flush;
   float T = used_time();
   list<point> C = CONVEX_HULL_S(L1);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   //assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_S(rat_point)    " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }
  cout << endl;

/*
 { cout << "CONVEX_HULL_S0              " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S0(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_S1              " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S1(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_S2              " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S2(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }
*/

 { cout << "CONVEX_HULL_S3              " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S3(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_S3F             " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S3F(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }

 { cout << "CONVEX_HULL_S4F             " << flush;
   float T = used_time();
   list<rat_point> C = CONVEX_HULL_S4F(L2);
   cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
   assert(check_hull(C));
  }


  rat_point::print_statistics();
  cout << endl;

  return 0;
}
