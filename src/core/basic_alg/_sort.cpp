/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _sort.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/system/basic.h>
#include <LEDA/core/quicksort.h>


#if defined(SMALLER)
#undef SMALLER
#endif

#define SMALLER(a,b) (a < b)

LEDA_BEGIN_NAMESPACE

/*

void QUICKSORT_C(int* l, int* r)
QS_BODY_C(QUICKSORT_C,int)

void QUICKSORT_C(float* l, float* r)
QS_BODY_C(QUICKSORT_C,float)

void QUICKSORT_C(double* l, double* r)
QS_BODY_C(QUICKSORT_C,double)

*/

LEDA_END_NAMESPACE
