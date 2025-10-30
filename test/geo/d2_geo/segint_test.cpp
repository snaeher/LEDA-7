/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  segint_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <math.h>
#include <LEDA/geo/plane_alg.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



#define TEST_ALG(name,ALG,seglist,G,embed) {\
cout << string("%s  k = %2d  ",name,k) << flush;\
float T = used_time();\
ALG(seglist,G,embed);\
float t = used_time(T);\
cout << string("|V|= %4d ",G.number_of_nodes());\
cout << string("|E|= %4d ",G.number_of_edges());\
cout << string("time = %6.2f sec  ",t);\
cout << endl; \
if (n < 0) { n = G.number_of_nodes(); m = G.number_of_edges(); } \
else if (n != G.number_of_nodes() || m != G.number_of_edges()) exit(1); }


int main(int argc, char* argv[])
{ 
  int N  = read_int("N  = ");
  int k  = read_int("k  = ");
  int try_float = false;
//try_float = Yes("Try floating point kernel, too? ");
  cout << endl;

//int l  = 0;

  rand_int.set_seed(1234567*N);

  for (; k <= 500; k+=10)
  {
    integer size = 1;

    size <<= (k-1);

    list<rat_segment> seglist;
    list<segment> seglist1;

    GRAPH<rat_point,rat_segment> G;
    GRAPH<point,segment> G1;

    for(int i=0; i < N; i++)
    { integer x1 = integer::random(k)-size;
      integer y1 = integer::random(k)-size;
    //integer w1 = integer::random(l);
      integer w1 = 1;
      integer x2 = integer::random(k)-size;
      integer y2 = integer::random(k)-size;
    //integer w2 = integer::random(l);
      integer w2 = 1;
      rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
      seglist.append(s);
      segment s1(x1.to_double(),y1.to_double(),x2.to_double(),y2.to_double());
      seglist1.append(s1);
     }

   rat_point::clear_statistics();
  
   int n = -1, m = -1;
   TEST_ALG("SWEEP(rat)      ",SWEEP_SEGMENTS,seglist,G,true)
   TEST_ALG("MULMULEY(rat)   ",MULMULEY_SEGMENTS,seglist,G,true)
   TEST_ALG("CURVE_SWEEP(rat)",CURVE_SWEEP_SEGMENTS,seglist,G,true)
   cout << endl;

   n = -1; m= -1;
   TEST_ALG("SWEEP(rat)      ",SWEEP_SEGMENTS,seglist,G,false)
   TEST_ALG("MULMULEY(rat)   ",MULMULEY_SEGMENTS,seglist,G,false)
   TEST_ALG("CURVE_SWEEP(rat)",CURVE_SWEEP_SEGMENTS,seglist,G,false)
   cout << endl;

   if (try_float)
   { int n = -1, m = -1;
     TEST_ALG("SWEEP(float)   ",SWEEP_SEGMENTS,seglist1,G1,true)
     TEST_ALG("MULMULEY(float)",MULMULEY_SEGMENTS,seglist1,G1,true)
     cout << endl;
    }

   rat_point::print_statistics();
   cout << endl;

   }

  
  return 0;
}

