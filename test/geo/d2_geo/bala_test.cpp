/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  bala_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <math.h>
#include <LEDA/geo/plane_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



static int Int_Counter = 0;

static void reporter (const segment& s1, const segment& s2)
{
  Int_Counter++; 
}

static void reporter (const rat_segment&, const rat_segment&)
{
  Int_Counter++;
}


#define TEST_BALABAN(seglist,fkt,cmp_count,exact_cmp_count) {\
cmp_count = 0;\
exact_cmp_count = 0;\
cout << string("k = %2d  ",k) << flush;\
float T = used_time();\
BALABAN_SEGMENTS(seglist,fkt);\
float t = used_time(T);\
cout << string("|N|= %4d ",seglist.length());\
cout << string("|I|= %6d ",Int_Counter);\
cout << string("time = %6.2f sec  ",t);\
cout << string("%6d / %6d   (%.2f %%)",exact_cmp_count,cmp_count,\
cmp_count ? 100*float(exact_cmp_count)/cmp_count : 0);\
Int_Counter=0;\
cout << endl; }

#define TEST_TRIVIAL(seglist,fkt) {\
cout << string("k = %2d  ",k) << flush;\
float T = used_time();\
TRIVIAL_SEGMENTS(seglist,fkt);\
float t = used_time(T);\
cout << string("|N|= %4d ",seglist.length());\
cout << string("|I|= %6d ",Int_Counter);\
cout << string("time = %6.2f sec  ",t);\
Int_Counter=0;\
cout << endl; }

int main()
{ 
  int N  = read_int("N  = ");
  int k  = read_int("k  = ");

  list<rat_segment> seglist;
  list<segment> seglist1;

  rand_int.set_seed(1234567*N);

  integer size(1);
  size <<= k;
  size = size/4;

  for(int j=0; j < N; j++)
  { integer x1 = integer::random(k)-size;
    integer y1 = integer::random(k)-size;
    integer w1 = 1;
    integer x2 = integer::random(k)-size;
    integer y2 = integer::random(k)-size;
    integer w2 = 1; 

    rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
    seglist.append(s);
    segment s1(x1.to_double(),y1.to_double(),
    x2.to_double(),y2.to_double());
    seglist1.append(s1);
  }   

  cout << endl << "Balaban : " << endl;

  int dummy = 0;
  rat_point::use_filter = 1;
  rat_segment::use_filter = true;

  TEST_BALABAN(seglist,reporter,dummy,dummy)
   
  TEST_BALABAN(seglist1,reporter,dummy,dummy)

  rat_point::use_filter = 0;
  rat_segment::use_filter = false;
  TEST_BALABAN(seglist,reporter,dummy,dummy)

  cout << endl << "Trivial : " << endl;

  rat_point::use_filter = 1;
  rat_segment::use_filter = true;

  TEST_TRIVIAL(seglist, reporter)
   
  TEST_TRIVIAL(seglist1, reporter)

  rat_point::use_filter = 0;
  rat_segment::use_filter = false;
  TEST_TRIVIAL(seglist,reporter)
 

  return 0;
}








