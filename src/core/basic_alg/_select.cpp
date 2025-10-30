/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _select.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/basic_alg.h>

LEDA_BEGIN_NAMESPACE

int SELECT(int* l, int* r, int pos)
{ 
  // compute element at position "pos" in sequence *l,...,*r
  // expected running time: O(r-l)

  while (l < r) 
  { int* i = l+(r-l)/2;
    if (*i > *r) swap(*i,*r);
    swap(*l,*i);

    i = l;

    int* k = r;
    int  s = *l;

    for(;;)
    { while (*(++i) < s) {}
      while (*(--k) > s) {}
      if (i<k) swap(*i,*k); else break;
     }
  
    swap(*l,*k);
  
    int j =  int(k-l)+1;
    if (pos <= j) 
       r = k;
    else 
       { l = k+1;
         pos -= j;
        }
  }

  return *l;
}



double SELECT(double* l, double* r, int pos)
{ 
  while (l < r) 
  { double* i = l+(r-l)/2;
    if (*i > *r) swap(*i,*r);
    swap(*l,*i);

    i = l;

    double* k = r;
    double  s = *l;

    for(;;)
    { while (*(++i) < s) {}
      while (*(--k) > s) {}
      if (i<k) swap(*i,*k); else break;
     }
  
    swap(*l,*k);
  
    int j =  int(k-l)+1;
    if (pos <= j) 
       r = k;
    else 
       { l = k+1;
         pos -= j;
        }
  }

  return *l;
}

LEDA_END_NAMESPACE
