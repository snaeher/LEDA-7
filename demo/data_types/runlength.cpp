/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  runlength.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:17 $


#include <LEDA/core/random.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/array.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;



int main(){

int M, n;

IO_interface I("Simulating a Snow Plow");
I.write_demo("Consider a circular track of length 1 on which \
a snow plow is operating. When the snow plow starts to \
operate there are M snow flakes on the track (at random locations). \
In every time unit the snow plow removes one snow flake and one new \
flake falls (at a random location). We compute how many snow flakes \
the snow plow removes in its i-th circulation of the track.");

I.write_demo("The running time of this program with M snowflakes \
and n rounds is n*M*log M. We suggest that you choose m less than \
twenty and M fairly large, e.g., M = 10000.");

M = I.read_int("Number of snow flakes, M = ",100000);
n = I.read_int("Number of rounds, n = ",20);


p_queue<double,int> Q;  // second type parameter is not used
stack<double> R;
random_source S;
double x;

int i;
for (i = 0; i < M; i++) { S >> x; Q.insert(x,0); }

array<double> RL(1,n);  // RL[i] = length of i-th run

for (i = 1; i <= n; i++)
{ // production of i-th run
  
  int runlength = 0;
  while ( !Q.empty() )
  { double Q_min = Q.del_min(); runlength++ ;
    S >> x;
    if (x < Q_min) R.push(x);
    else           Q.insert(x,0); 
  }
  
  RL[i] = (double)runlength / M;

#ifndef BOOK
I.write_demo("number of flakes removed in turn "+ string("%d", i) + " = M *",RL[i]);
#endif

  while ( !R.empty() ) Q.insert(R.pop(),0);
}


#ifdef BOOK
for (i = 1; i <= n/4; i++)
{ for (int j = 0; j <= 3; j++)
  { I.write_table((j == 0 ? "" : " & "),j*n/4 + i);
    I.write_table(" & ",RL[j*n/4 + i]);
  }
    if ( i < n/4) 
      I.write_table("\\\\ \\hline \n");
    else
      I.write_table("\\\\ \\hline \\hline \n");
}
#endif



return 0;
}


