/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delau_test.c
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
using std::endl;
using std::flush;


random_source& operator>>(random_source& R, point& p)
{ double x,y;
  R >> x >>y;
  p = point(x,y);
  return R;
}


int main()
{
   int N = read_int("N = ");

   list<point>     L;
   list<rat_point> L1;
   random_source ran(0,100000);

   ran.set_seed(12345*N);

   for(int i=0; i<N; i++) 
   { int x,y;
     ran >> x >> y;
     L.append(point(x,y));
     L1.append(rat_point(x,y));
    }



  GRAPH<point,int> G;
  GRAPH<rat_point,int> G1;

  // flipping 

  cout << "DELAUNAY_FLIP   (float) " << flush;
  float T = used_time();
  int f1 = DELAUNAY_FLIP(L,G);
  float t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec  (%4d flips)",
             G.number_of_nodes(), G.number_of_edges()/2,t,f1) << endl;

  cout << "DELAUNAY_FLIP   (exact) " << flush;
  T = used_time();
  f1 = DELAUNAY_FLIP(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec  (%4d flips)",
             G.number_of_nodes(), G.number_of_edges()/2,t,f1) << endl;
  

  cout << "DELAUNAY_FLIP_F         " << flush;
  T = used_time();
  f1 = DELAUNAY_FLIP_F(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec  (%4d flips)",
             G.number_of_nodes(), G.number_of_edges()/2,t,f1) << endl;
  cout << endl;


  // Stolfi's Divide and Conquer

  cout << "DELAUNAY_STOLFI (float) " << flush;
  T = used_time();
  DELAUNAY_STOLFI(L,G);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;

  cout << "DELAUNAY_STOLFI (exact) " << flush;
  T = used_time();
  DELAUNAY_STOLFI(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;


  cout << "DELAUNAY_STOLFI_F       " << flush;
  T = used_time();
  DELAUNAY_STOLFI_F(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;
  cout << endl;


  // improved by Dwyer

  cout << "DELAUNAY_DWYER  (float) " << flush;
  T = used_time();
  DELAUNAY_DWYER(L,G);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;

  cout << "DELAUNAY_DWYER  (exact) " << flush;
  T = used_time();
  DELAUNAY_DWYER(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;


  cout << "DELAUNAY_DWYER_F        " << flush;
  T = used_time();
  DELAUNAY_DWYER_F(L1,G1);
  t = used_time(T);
  cout << string("|V| = %d |E| = %d  %.2f sec",
             G.number_of_nodes(), G.number_of_edges()/2,t) << endl;
  cout << endl;


 return 0;
}
