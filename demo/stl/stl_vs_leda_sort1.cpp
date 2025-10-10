/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stl_vs_leda_sort1.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/array.h>
#include <LEDA/core/random.h>
#include <LEDA/core/tuple.h>

#include<vector>
#include<algorithm>

using std::cout;
using std::cerr;
using std::flush;
using std::endl;


typedef leda::two_tuple<double,double> d_pair;

LEDA_BEGIN_NAMESPACE

inline bool operator<(const d_pair& t1, const d_pair& t2)
{ return (t1.first() < t2.first() || 
         (t1.first() == t2.first() && t1.second() < t2.second())); 
  }

LEDA_END_NAMESPACE



int main()
{
  int N = leda::read_int("N = ");

  leda::random_source S(0,N);

  cout << endl;
  cout << "Constructing Arrays " << flush;

  leda::array<int>    A1(N);
  std::vector<int>    A2(N);

  leda::array<double> B1(N);
  std::vector<double> B2(N);
  double*             B3 = new double[N];  
  double*             B4 = new double[N];  

  leda::array<d_pair> C1(N);
  std::vector<d_pair> C2(N);
  d_pair*             C3 = new d_pair[N];
  d_pair*             C4 = new d_pair[N];
  
  int i;
  for(i=0;i<N;i++)
  { int x,y;
    S >> x >> y;
    C1[i] = C2[i] = C3[i] = C4[i] = d_pair(x,y);
    B1[i] = B2[i] = B3[i] = B4[i] = x;
    A1[i] = A2[i] = y;
   }

  cout << "done" << endl;
  cout << endl;

  float t;

  cout << "LEDA_SORT(int):    " << flush;
  t = leda::used_time();
  A1.sort();
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "STL_SORT(int):     " << flush;
  t = leda::used_time();
  std::sort(A2.begin(),A2.end());
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;


  cout << "LEDA_SORT(double): " << flush;
  t = leda::used_time();
  B1.sort();
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "LEDA_QUICKSORT_C(double):   " << flush;
  t = leda::used_time();
  leda::QUICKSORT_C(B4,B4+N-1);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "LEDA_QUICKSORT_C_OPERATOR(double): " << flush;
  t = leda::used_time();
  leda::QUICKSORT_C_OPERATOR(B3,B3+N-1);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "STL_SORT(double):  " << flush;
  t = leda::used_time();
  std::sort(B2.begin(),B2.end());
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;



  cout << "LEDA_SORT(pair):   " << flush;
  t = leda::used_time();
  C1.sort();
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "LEDA_QUICKSORT_C(pair):   " << flush;
  t = leda::used_time();
  leda::QUICKSORT_C(C4,C4+N-1);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "LEDA_QUICKSORT_C_OPERATOR(pair):   " << flush;
  t = leda::used_time();
  leda::QUICKSORT_C_OPERATOR(C3,C3+N-1);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "STL_SORT(pair):    " << flush;
  t = leda::used_time();
  std::sort(C2.begin(),C2.end());
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;


  // check sorting

  for(i=0;i<N;i++)
  { if (A1[i] != A2[i]) cerr << A1[i] << " " << A2[i] << endl;
    if (B1[i] != B2[i] || B1[i] != B3[i]) cerr << B1[i] << " " << B2[i] << " " << B3[i] << endl;  
    if (C1[i] != C2[i] || C1[i] != C3[i]) cerr << C1[i] << " " << C2[i] << " " << C3[i] << endl;
   }
   
  delete [] B3; delete [] C3;
}




