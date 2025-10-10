#include "Heapsort.h"


void Heapsort::swap(int A_int[], int i, int j)
{  
    int tmp = A_int[i];
    A_int[i] = A_int[j];
    A_int[j] = tmp;
}

void Heapsort::heapify(int A_int[], int k, int N)
{ _4_event(*this, k); 
  _5_event(*this, k, N);
  while(k*2 <= N) 
  { 
    int j = 2*k;
    _6_event(*this, j);
    
    _7_event(*this, N, j);
    if(j < N && A_int[j] < A_int[j+1]) j++;

    _8_event(*this, k, j);
    if(A_int[k] >= A_int[j]) break;
  
    swap(A_int, k, j);
    _9_event(*this, k, j);
    
    _10_event(*this, k, j);   
    k = j;
    
    _5_event(*this, k, N);
  }
}


void Heapsort::_heapsort(int A_int [], int N)
{ _1_event(*this, N);
  for(int i = N/2; i >= 1; i--)
  { _2_event(*this, i); 
    _3_event(*this, i, N);
    heapify(A_int, i, N);
  }

  _11_event(*this, N);
  while(N > 1)
  { 
    _12_event(*this, N);
    swap(A_int, N, 1);
    _13_event(*this, N);
    heapify(A_int, 1, --N);
    _11_event(*this, N);
  }
}

Heapsort HEAPSORT; 

