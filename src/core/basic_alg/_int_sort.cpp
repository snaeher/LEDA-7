/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _int_sort.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/basic_alg.h>
#include <LEDA/system/assert.h>
#include <LEDA/core/string.h>
#include <string.h>

LEDA_BEGIN_NAMESPACE

const int BITS = 12;
static int count[1<<BITS]; // 2^12 = 4096

static void counting_sort(int*& A, int*& B, int n, int mask, 
                                                   int mask1, 
                                                   int shift)
{
  int* A_stop = A+n;
  int* p = A;
  int  x = (A[0] >> shift) & mask;

  while (p < A_stop && ((*p >> shift) & mask) == x) p++;

  if (p == A_stop)
  { leda_swap(A,B);
    return;
   }

  memset(count,0,(mask+1)*sizeof(int));

  if (mask1 == 0)
  { count[x] = int(p-A);
    while (p < A_stop) 
    { int x = (*p++ >> shift) & mask;
      count[x]++;
     }
   }
  else
  { // flip sign bit
    count[x ^ mask1] = int(p-A);
    while (p < A_stop) 
    { int x = (*p++ >> shift) & mask;
      count[x ^ mask1]++;
     }
   }


  int sum = 0;
  for(int i=0; i<=mask; i++)
  { int r = count[i];
    count[i] = sum;
    sum += r;
   }


  if (mask1 == 0)
    for(p=A; p < A_stop; p++)
    { int x = *p;
      int r = count[((x >> shift) & mask)]++;
      B[r] = x;
     }
  else
    for(p=A; p < A_stop; p++)
    { int x = *p;
      int r = count[((x >> shift) & mask) ^ mask1]++;
      B[r] = x;
     }
}



void INTSORT(int* A, int N)
{
  int* B = new int[N];
  int* a = A;
  int* b = B;

  counting_sort(a,b,N,0xfff,0x000, 0);
  counting_sort(b,a,N,0xfff,0x000,12);
  counting_sort(a,b,N,0x0ff,0x080,24);

  if (b != A) memcpy(A,B,N*sizeof(int));

  delete[] B;
}


LEDA_END_NAMESPACE
   
