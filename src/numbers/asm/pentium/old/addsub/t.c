#include <string.h>

extern int carry;

int vecAdd_i386(int*a, int a_used, int *b, int b_used, int* sum)
{   a += b_used;
    b += b_used;
    sum += b_used;

    memcpy(sum,a,(a_used-b_used)*sizeof(int));

    int i;
    for(i=-b_used; i != 0; i++) sum[i] = a[i] + b[i];

    if (carry)
    { sum[i] += 1;
      while (sum[i++] == 0) sum[i] += 1; 
     }
    i += b_used;
    if (i < a_used) i = a_used; 
    return i;
}
