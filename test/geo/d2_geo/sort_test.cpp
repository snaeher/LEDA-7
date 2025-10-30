/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sort_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/point.h>
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main () 
{
  int i;
  int N = read_int("N = "); 

  float T = used_time();


  array<point>  A(N);
  list<point>   L;

  for (i = 0; i < N; i++) 
      A[i] = point(rand_int(1,100000),rand_int(1,100000));

  for (i = 0; i < N; i++) L.append(A[i]);

  cout << endl;

  cout << "sorting array<point>  ";
  cout.flush();
  A.sort();
  cout << string("  %5.3f sec",used_time(T));
  cout << endl;
  cout << endl;


  cout << "sorting list<point>   ";
  cout.flush();
  L.sort();
  cout << string("  %5.3f sec",used_time(T));
  cout << endl;

  return 0;
}


