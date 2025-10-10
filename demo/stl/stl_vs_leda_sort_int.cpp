/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stl_vs_leda_sort_int.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/array.h>
#include <LEDA/core/random.h>
#include <assert.h>

#include<vector>
#include<algorithm>

using std::cout;
using std::flush;
using std::endl;


int main()
{
  int n   = leda::read_int("#int's = ");
  int min = leda::read_int("min    = ");
  int max = leda::read_int("max    = ");

  cout << endl;
  cout << "Constructing Arrays ... " << flush;

  leda::array<int>    A1(n);
  std::vector<int>   A2(n);

  for(int i=0;i<n;i++) A1[i] = A2[i] = leda::rand_int(min,max);

  cout << "done" << endl;
  cout << endl;

  float t;

  cout << "LEDA array::sort():   " << flush;
  t = leda::used_time();
  A1.sort();
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;

  cout << "STL  std::sort():     " << flush;
  t = leda::used_time();
  std::sort(A2.begin(),A2.end());
  cout << leda::string("%.2f sec", leda::used_time(t)) << endl;
  cout << endl;

  // check sorting
  for(int j=0;j<n;j++) assert(A1[j] == A2[j]);
}




