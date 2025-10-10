/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sortseq_split.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:18 $

#include <LEDA/core/sortseq.h>
#include <LEDA/core/array.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/random.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;




int main(){


IO_interface I("Repeated Splitting of Sorted Sequence");

I.write_demo("We construct a sorted sequence of size n and then dismantle \
it into n sequence of length 1 by a sequence of splits.");

I.write_demo("The running \
time is linear no matter which splits are performed.");

int n = I.read_int("number of items = ");


typedef sortseq<int,int> int_seq;

array<seq_item> A(n);
int_seq* S = new int_seq();;
for (int i = 0; i < n; i++) A[i] = S->insert(i,0);

typedef three_tuple<int_seq*,int,int> task;

stack<task> TS;
TS.push(task(S,0,n-1));

float UT = used_time();
while ( !TS.empty() )
{ task t = TS.pop();
  int_seq* S = t.first();
  int l = t.second();
  int r = t.third();
  if ( r - l + 1 < 2 ) { delete S; continue; }
  int_seq* T = new int_seq();
  int_seq* U = new int_seq();
  int m = rand_int(l,r-1); 
  S->split(A[m],*T,*U,leda::after);
  delete S;
  TS.push(task(T,l,m));
  TS.push(task(U,m+1,r));
}

I.write_demo("This took ",used_time(UT)," seconds");


return 0;
}


