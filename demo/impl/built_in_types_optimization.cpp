/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  built_in_types_optimization.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:16 $


#include <LEDA/core/array.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;
using std::istream;
using std::ostream;



class myint
{ int x; 
public:
  myint() {}
  myint(const int _x): x(_x) {}
  myint(const myint& p)  {  x = p.x;}

  friend void operator>>(istream& is, myint& p) { is >> p.x; };
  friend ostream& operator<<(ostream& os, const myint& p) 
         { os << p.x; return os; }; 
  friend int compare(const myint&,const myint&); 
}; 

int compare(const myint& p,const myint& q)
{
  if (p.x == q.x) return 0;
  if (p.x < q.x) return -1; else return +1; 
}


int main() {

IO_interface I;

int n = I.read_int("n = ",1000000);
int N = 32*n;

I.write_table(n);

array<myint> A(n);
array<int>   B(n);

for (int i = 0; i < n; i++) 
{ B[i] = rand_int(0,N-1);
  A[i] = myint(B[i]);
}

float T = used_time(); float UT;

B.sort();
UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("time for myints", UT);

B.sort();
UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("time for ints", UT);

I.write_table(" \\\\ \\hline \\hline");

return 0;
}

