/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stl_vs_leda_sort.c
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
#include <LEDA/core/sort.h>

#include<vector>
#include<algorithm>


using std::cout;
using std::cerr;
using std::flush;
using std::endl;



typedef leda::two_tuple<double,double> d_pair;

inline bool d_pair_lt(const d_pair& t1, const d_pair& t2)
{ return (t1.first() < t2.first() || 
         (t1.first() == t2.first() && t1.second() < t2.second())); 
  }



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

  leda::array<d_pair> C1(N);
  std::vector<d_pair> C2(N);

  int i;
  for(i=0;i<N;i++)
  { int x,y;
    S >> x >> y;
    C1[i] = C2[i] = d_pair(x,y);
    B1[i] = B2[i] = x;
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

/*

  cout << "LEDA_SORT(double): " << flush;
  t = leda::used_time();
  leda::sort(&B3[0],&B3[N-1]);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
*/

  cout << "STL_SORT(double):  " << flush;
  t = leda::used_time();
  std::sort(B2.begin(),B2.end());
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;



  cout << "LEDA_SORT(pair):   " << flush;
  t = leda::used_time();
  C1.sort();
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;


  cout << "STL_SORT(pair):    " << flush;
  t = leda::used_time();
  std::sort(C2.begin(),C2.end(),d_pair_lt);
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;




  // check sorting

  for(i=0;i<N;i++)
  { if (A1[i] != A2[i]) cerr << A1[i] << " " << A2[i] << endl;
    if (C1[i] != C2[i]) cerr << C1[i] << " " << C2[i] << endl;
   }
}




