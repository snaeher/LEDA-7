/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  qs_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_point.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


static int k = 0;
static int count1 = 0;
static int count2 = 0;

typedef d3_rat_point KEY;


inline bool smaller(KEY x, KEY y) 
{ //count1++;
  return compare(x,y) < 0; 
}

inline bool smaller_F(KEY x, KEY y) 
{ //count2++; 
  return compare_F(x,y) < 0; 
}

void INSERTION_SORT(KEY* l, KEY* r)
{
  if (l >= r) return;

  KEY* min=l;
  KEY* run;

  for (run = l+1; run <= r; run++)
      if (smaller(*run,*min)) min = run;

  leda_swap(*min,*l);

  if (r == l+1) return;

  for(run=l+2; run <= r; run++)
  { for (min = run-1; smaller(*run,*min); min--);
    min++;
    if (run != min) 
    { KEY tmp = *run;
      KEY* p;
      KEY* q;
      for(p=run, q = run-1; p > min; p--,q--) *p = *q;
      *min = tmp;
     }
   }
}


void INSERTION_SORT(KEY* l, KEY* m, KEY* r)
{
  if (l >= r) return;


  // precondition: (*l, ..., *(m-1)) and (*(m+1),...,*r) are sorted

  // swap minimum to *l

  KEY* p = m+1;
  if (smaller(*m,*l)) leda_swap(*l,*m);
  if (smaller(*p,*l)) leda_swap(*l,*p);

  if (r == l+1) return;

  KEY* run;
  for(run=m; run <= r; run++)
  { KEY* min;
    for (min = run-1; smaller(*run,*min); min--);
    min++;
    if (run != min) 
    { KEY tmp = *run;
      KEY* p;
      KEY* q;
      for(p=run, q = run-1; p > min; p--,q--) *p = *q;
      *min = tmp;
     }
   }
}




void QUICKSORT_EXACT(KEY* l, KEY* r)
{ 
  KEY* i = l;
  KEY* j = r+1;
  KEY  s = *l;

  for(;;)
  { while (i < r && smaller(*++i,s));
    while (j > l && smaller(s,*--j));
    if (i < j) leda_swap(*i,*j); else break;
   }

  leda_swap(*l,*j);

  if (l < j-1) QUICKSORT_EXACT(l,j-1);
  if (r > j+1) QUICKSORT_EXACT(j+1,r);
}


void QUICKSORT_APPROX(KEY* l, KEY* r)
{ 
  KEY* i = l;
  KEY* j = r+1;
  KEY  s = *l;

  for(;;)
  { while (i < r && smaller_F(*++i,s));
    while (j > l && smaller_F(s,*--j));
    if (i < j) leda_swap(*i,*j); else break;
   }

  leda_swap(*l,*j);

  if (l < j-1) QUICKSORT_APPROX(l,j-1);
  if (r > j+1) QUICKSORT_APPROX(j+1,r);

  if ((j > l && smaller(s,*(j-1))) || (j < r && smaller(*(j+1),s))) 
    INSERTION_SORT(l,j,r);
}



void sort_test(string msg, int N, KEY* A, KEY* B)
{

  cout << endl;
  cout << msg << endl;

  float T = used_time();

/*
  T = used_time();
  cout << string("list::sort     ") << flush;
  L.sort();
  cout << string("%.2f sec",used_time(T)) << endl;
  cout << endl;
*/


  cout << "exact:  " << flush;
  QUICKSORT_EXACT(B,B+N-1);
  cout << string("%.2f sec     ",used_time(T)) << flush;


  cout << "approx: " << flush;
  QUICKSORT_APPROX(A,A+N-1);
  cout << string("%.2f sec",used_time(T)) << endl;



  int err = 0;
  int i;
  for(i=0; i<N-1; i++) 
    if (smaller(A[i+1],A[i+1])) err++;

  if (err > 0) 
     error_handler(1, "not correctly sorted");
}


main()
{
  int N = read_int("N = ");
  int i;

  KEY* A = new KEY[N];
  KEY* B = new KEY[N];


  for(i=0; i<N; i++) 
  { d3_rat_point p(rand_int(0,1000),rand_int(0,1000),rand_int(0,1000),1);
    A[i] = B[i] = p; 
   }
  sort_test("random:",N,A,B);


  for(i=0; i<N; i++) 
  { d3_rat_point p(0,rand_int(0,1000),rand_int(0,1000),1);
    A[i] = B[i] = p; 
   }
  sort_test("x = 0:",N,A,B);


  for(i=0; i<N; i++) 
  { d3_rat_point p(0,0,rand_int(0,1000),1);
    A[i] = B[i] = p; 
   }
  sort_test("x = y = 0:",N,A,B);


  for(i=0; i<N; i++) 
  { d3_rat_point p(0,0,0,1);
    A[i] = B[i] = p; 
   }
  sort_test("x = y = z = 0:",N,A,B);


  delete[] A;
  delete[] B;

  return 0;
}



