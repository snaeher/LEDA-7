/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  array_sort_times.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/core/array.h>

using namespace leda;


template<class E> 
void straight_insertion_sort(array<E>& A)
{ int l = A.low();
  int h = A.high(); 

  for (int i = l + 1; i <= h; i++)
     { E x = A[i];
       int j = i - 1;  
       while ( j >= l && compare(x,A[j]) < 0 ) 
       { A[j+1] = A[j]; 
         j--; 
       }
       A[j+1] = x;
     }
}




template<class E>
void merge(array<E>& X, array<E>& Y, int i, int K, int h)
{ int il = i; int ih = i + K;
  int ml = leda::min(i + K - 1,h); int m = leda::min(i + 2*K - 1,h);
  for (int j = i; j <= m ; j++)
    { if ( ih <= m && ( il > ml || compare(X[ih],X[il]) < 0 ) )
         { Y[j] = X[ih]; ih++; }
      else 
         { Y[j] = X[il]; il++; }
    }
}


template<class E> 
void merge_sort(array<E>& A)
{ int l = A.low();  int h = A.high(); int n = h - l + 1;
  array<E> B(l,h);
  bool even_phase = true;
 
  for (int K = 1; K < n; K = 2*K)
  { for (int i = l; i <= h; i = i + 2*K)
    { if ( even_phase ) merge(A,B,i,K,h); 
      else merge(B,A,i,K,h);
    }
    even_phase = !even_phase;
  }

  if ( !even_phase ) 
    { for (int i = l; i <= h; i++) A[i] = B[i]; }
}


#include <LEDA/core/IO_interface.h>

int main()
{ int max; int n;

IO_interface R("Running Times of Sorting Routines");
R.set_precision(3);
R.write_demo("This demo illustrates the speed of three sorting functions \
on arrays: insertion sort, merge sort, and the member function sort (which \
is a version of quicksort). You are asked for an integer max. "); \
R.write_demo("We then run all three sorting functions for n = 2000, 4000, \
... until max. Since insertion sort is very slow we only use it for \
n up to 8000.");

#ifdef BOOK
  max  = 256000; 
#ifdef FIRST
  R.write_table("$n$ ");
  for (n = 2; n <= max/1000; n = 2*n) R.write_table(" & ",n);
  R.write_table("\\\\ \\hline \\hline \n");
#endif
#else
  max = R.read_int("max = ");
#endif

  R.write_table("insertion sort ");
  for (n = 2000; n <= max ; n = 2*n) 
  { array<int> A(n);
    for (int j = 0; j < n; j++) A[j] = j;

    float T1 = used_time();
    if (n <= 8000)
    { A.permute(); 
      straight_insertion_sort(A);
      T1 = used_time(T1);
      R.write_table(" & ",T1);
      R.write_demo("n = ",n);
      R.write_demo(0,", time for insertion sort = ",T1);
    }
#ifdef BOOK
    else R.write_table(" & ");
#endif
  }
  R.write_table("\\\\ \\hline \n");
  R.write_table("merge sort ");

  for (n = 2000; n <= max; n = 2*n) 
  { array<int> A(n);
    for (int j = 0; j < n; j++) A[j] = j;

    float T1 = used_time();
    { A.permute(); 
      merge_sort(A);
      T1 = used_time(T1);
      R.write_table(" & ",T1);
      R.write_demo("n = ",n);
      R.write_demo(0,", time for merge sort = ",T1);
    }
  }    
  R.write_table("\\\\ \\hline \\hline \n");

#ifndef FIRST
  R.write_table("member function");
  for (n = 2000; n <= max; n = 2*n) 
  { array<int> A(n);
    for (int j = 0; j < n; j++) A[j] = j;

    float T1 = used_time();
    { A.permute(); 
      A.sort();
      T1 = used_time(T1);
      R.write_table(" & ",T1); 
      R.write_demo("n = ",n);
      R.write_demo(0,", time for member function = ",T1);
    } 
  }
  R.write_table("\\\\ \\hline \\hline \n");
#endif
return 0;
}
