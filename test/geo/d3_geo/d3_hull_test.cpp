/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_hull_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_hull.h>
#include <LEDA/internal/std/iostream.h>

using namespace leda;

using std::cout;
using std::endl;
using std::flush;



template <class POINT>
inline 
void d3_hull_test(const list<POINT>& L, GRAPH<POINT,int>& H, string s, int k)
{ int n,m;
  float t;
  cout << s << ":   " << flush;
  t = used_time();
  D3_HULL(L,H,k);
  t = used_time() - t;
  n = H.number_of_nodes();
  m = H.number_of_edges();
  cout << string("|V| = %4d  |E| = %4d  time = %5.2f sec",n,m,t) << endl;
  CHECK_HULL(H);
}

template <class POINT>
inline
void d3_dc_hull(const list<POINT>& L, GRAPH<POINT,int>& H, string s, int k)
{ int n,m;
  cout << s << ":   " << flush;
  float t = used_time();
  D3_DC_HULL(L,H, k);
  t = used_time() - t;
  n = H.number_of_nodes();
  m = H.number_of_edges();
  cout << string("|V| = %4d  |E| = %4d  time = %5.2f sec",n,m,t) << endl;
  CHECK_HULL(H);
}



template <class POINT>
inline
void d3_hull_flip(const list<POINT>& L, GRAPH<POINT,int>& H, string s, int k)
{ int n,m;
  float t;
  cout << s << ":   " << flush;
  t = used_time();
  D3_HULL_FLIP(L,H,k!=0);
  t = used_time() - t;
  n = H.number_of_nodes();
  m = H.number_of_edges();
  cout << string("|V| = %4d  |E| = %4d  time = %5.2f sec",n,m,t) << endl;
  //CHECK_HULL(H);
}



int main() {

 int N = read_int("n = "); 

 d3_rat_point p;
 list<d3_rat_point> L;
 list<d3_point> L1;
 int k;

 GRAPH<d3_rat_point,int> H;
 GRAPH<d3_point,int> H1;

 random_points_in_cube(N,1000,L);
 L1.clear();
 forall(p,L) L1.append(p.to_d3_point());

 d3_hull_test(L, H, string("cube  "), 0);
 //d3_hull_test(L, H, "cube   (exact)",-1);
 //d3_hull_test(L1,H1,"cube   (float)",-1);
 //d3_hull_flip(L, H, "cube   (flip) ",0);
 //d3_hull_flip(L, H, "cube   (flip) ",1);
 for(k = 16; k<L.length(); k *= 2)
    d3_dc_hull(L, H,   string("k = %d",k), k);
 cout << endl;

 random_points_in_ball(N,1000,L);
 L1.clear();
 forall(p,L) L1.append(p.to_d3_point());

 d3_hull_test(L, H, string("ball  "), 0);
 //d3_hull_test(L, H, "ball   (exact)",-1);
 //d3_hull_test(L1,H1,"ball   (float)",-1);
 //d3_hull_flip(L, H, "ball   (flip) ",0);
 //d3_hull_flip(L, H, "ball   (flip) ",1);
 for(k = 16; k<L.length(); k *= 2)
    d3_dc_hull(L, H,   string("k = %d",k), k);
 cout << endl;

 random_points_on_sphere(N,1000,L);
 L1.clear();
 forall(p,L) L1.append(p.to_d3_point());
 d3_hull_test(L, H, string("sphere"),0);
 //d3_hull_test(L, H, "sphere (exact)",-1);
 //d3_hull_test(L1,H1,"sphere (float)",-1);
 //d3_hull_flip(L, H, "sphere (flip) ",0);
 //d3_hull_flip(L, H, "sphere (flip) ",1);
 for(k = 16; k<L.length(); k *= 2)
    d3_dc_hull(L, H,   string("k = %d",k), k);
 cout << endl;

 return 0;
}
