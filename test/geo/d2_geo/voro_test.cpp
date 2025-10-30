/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  voro_test.c
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


  // flipping with floating point points

  GRAPH<point,int>  D1;
  GRAPH<circle,point> V1;

  float T, t1, t2, t3;

  cout << "DELAUNAY FLIP(float)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L,D1);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D1);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D1,V1);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V1.number_of_nodes(), V1.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;

  // flipping with rational points

  GRAPH<rat_point,int> D2;
  GRAPH<rat_circle,rat_point> V2;


  rat_point::use_static_filter = false;

  cout << "DELAUNAY FLIP(exact)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L1,D2);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D2);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D2,V2);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V2.number_of_nodes(), V2.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;

  D2.clear();
  V2.clear();

  rat_point::use_static_filter = false;
  cout << "DELAUNAY FLIP(exact)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L1,D2);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D2);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D2,V2);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V2.number_of_nodes(), V2.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;


  D2.clear();
  V2.clear();

  rat_point::use_static_filter = true;
  cout << "DELAUNAY FLIP(exact)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L1,D2);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D2);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D2,V2);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V2.number_of_nodes(), V2.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;


  D2.clear();
  V2.clear();

  rat_point::use_static_filter = false;
  cout << "DELAUNAY FLIP(exact)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L1,D2);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D2);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D2,V2);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V2.number_of_nodes(), V2.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;


  D2.clear();
  V2.clear();

  rat_point::use_static_filter = true;
  cout << "DELAUNAY FLIP(exact)  " << flush;
  T = used_time();
  TRIANGULATE_POINTS(L1,D2);
  t1 = used_time(T);
  DELAUNAY_FLIPPING(D2);
  t2 = used_time(T);
  DELAUNAY_TO_VORONOI(D2,V2);
  t3 = used_time(T);
  cout << string("|V| = %d  |E| = %d  time = %.2f (%.2f + %.2f + %.2f)",
          V2.number_of_nodes(), V2.number_of_edges()/2,t1+t2+t3,t1,t2,t3) << endl;


 return 0;
}
