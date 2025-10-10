/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stl_vs_leda_map.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/d_array.h>
#include <LEDA/core/map.h>
#include <LEDA/core/array.h>

#include <map>

/*
#if !defined(__SUNPRO_CC) && !defined(_MSC_VER) && !defined(__GNUC__)
#define HASH_MAP
#endif
*/

using std::cout;
using std::flush;
using std::endl;


template <class leda_map_T, class std_map_T>
void test_map(int N, leda_map_T& LM, std_map_T& SM)
{
  typedef typename leda_map_T::index_type T;

  T*  A = new T[N];

  int i;
  for (i=0; i<N; i++) A[i] = i;

  float t,t1;

  float leda_t = 0;
  float stl_t = 0;

  cout << "ordered insertions      ";
  cout.flush();

  t = leda::used_time();
  for (i = 0; i < N; i++) LM[i] = i;
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  for (i = 0; i < N; i++) SM[i] = i;
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << endl;
  stl_t += t1;

  int s = 0;

  cout << "ordered lookups         ";
  cout.flush();

  t = leda::used_time();
  for (i = 0; i < N; i++) s += LM[i];
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  for (i = 0; i < N; i++) s += SM[i];
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << endl;
  stl_t += t1;


  cout << "random  insertions      ";
  cout.flush();

  t = leda::used_time();
  for (i = 0; i < N; i++) LM[A[i]] = i;
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  for (i = 0; i < N; i++) SM[A[i]] = i;
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << endl;
  stl_t += t1;

  cout << "random  lookups         ";
  cout.flush();

  t = leda::used_time();
  for (i = 0; i < N; i++) s += LM[A[i]];
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  for (i = 0; i < N; i++) s += SM[A[i]];
  t1 = leda::used_time(t);
  cout << leda::string("   %5.3f sec",t1) << endl;
  stl_t += t1;




  cout << "------------------------------------------------" << endl;
  cout << "total                   ";
  cout << leda::string("   %5.3f sec   %5.3f sec",leda_t,stl_t) << endl;
  cout << endl;
  cout << endl;

  delete[] A;
}



int main () 
{

cout << endl;
cout << "\
We compare the map types of LEDA (leda::map/leda::d_array) and STL (std::map).\n\
To see a considerably difference you should use a large number of entries\n\
e.g. N >= 1 Million.\n";
cout << endl;

  int N = leda::read_int("Number of entries: "); 
  cout << endl;
  cout << endl;


  cout << "leda::map : std::map         LEDA        STL" << endl;
  cout << "------------------------------------------------" << endl;
  leda::map<int,int> LM0;
  std::map<int,int> SM0;
  test_map(N,LM0,SM0);


  cout << "leda::d_array : std::map     LEDA        STL" << endl;
  cout << "------------------------------------------------" << endl;
  leda::d_array<int,int> LM1;
  std::map<int,int> SM1;
  test_map(N,LM1,SM1);


  cout << "with double keys           LEDA        STL" << endl;
  cout << "------------------------------------------------" << endl;
  leda::d_array<double,int> LM2;
  std::map<double,int> SM2;
  test_map(N,LM2,SM2);

  return 0;
}
