#include "Qsort.h"


void Qsort::swap(int A_int[], int i, int j)
{  
    int tmp = A_int[i];
    A_int[i] = A_int[j];
    A_int[j] = tmp;
}

void Qsort::_qsort(int A_int[], int l, int r)
{ _1_event(*this, l, r);
  _2_event(*this, l, r);        
                          if (l >= r) return;     
                          int i = l;             
  _3_event(*this, i);                               

                          int j = r+1;               
  _4_event(*this, j);     
  _5_event(*this, l);     
                          int v = A_int[l]; 
  _6_event(*this);        
                          for(;;)  {
                      
                            do { i++;
  _7_event(*this, i, l); 
                            } while(A_int[i] < v && i < r);     
                      
                            do { j--;  
  _8_event(*this, j, l);         
                            } while(A_int[j] > v );  

  _9_event(*this, i, j);    if(i >= j) break;     

                            swap(A_int, i, j);     
  _10_event(*this, i, j);
                          } // end for                           
                          swap(A_int, l, j);       
  _11_event(*this, l, j);
  _12_event(*this, l, j-1); _qsort(A_int, l, j-1);
  _13_event(*this, j+1, r); _qsort(A_int, j+1, r);
}

Qsort QUICKSORT; 

