/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sort_merge_times.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.2 $  $Date: 2005/11/25 12:33:58 $


#include <LEDA/core/list.h>
#include <math.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;



int main()
{ int min, max;
  IO_interface R("Time to merge and sort");

  R.write_demo("This demo illustrates the speed of sorting and merging. \
  You will be asked to input a number max. We will then perform the \
  following experiment for n = 16000, 32000, ..., max: \
  We generate two sorted lists of n elements and merge them. We then \
  permute the list and sort it.");
  #ifdef BOOK
  max = 1024000;
  min = 128000;
  #else
  max = R.read_int("max = ");
  min = 16000;
  if (max < min)
  { max = min;
    R.write_demo("max should be at least 16000; I set it to this value.");
  }
  #endif


  for (int n = min; n <= max; n = 2*n) 
  { list<int> L;
    for (int j = 0; j < n; j++) L.append(j);

    list<int> L1 = L;
    list<int> L2 = L;

    float T1 = used_time();
    L1.merge(L2);
    T1 = used_time(T1);

    L.permute();

    float T2 = used_time();
    L.sort();
    T2 = used_time(T2);

    
    #ifdef BOOK
    cout.precision(3);
    cout << n  << "&"  << T1 << "&" << 1000000*T1/n 
          << "&" << T2  << "&" << 1000000 * T2/ (n*log(n))
          << "\\\\ \\hline" << (n == max? "\\hline":"") <<"\n";
    #else
    R.write_demo(3,"n =                            ",n);
    R.write_demo("time to merge =                ",T1);
    R.write_demo("1000000*merge time/n =         ",1000000*T1/n);
    R.write_demo("time to sort =                 ",T2);
    R.write_demo("1000000*sort time/(n*log(n)) = ",1000000 * T2/ (n*log(double(n))));
    #endif

  }
  return 0;
}


