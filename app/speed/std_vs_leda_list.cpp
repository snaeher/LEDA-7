/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  stl_vs_leda_list.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/list.h>

class my_struct;

/*
namespace leda {
int compare(const my_struct& s1, const my_struct& s2);
}
*/

int compare(const my_struct& s1, const my_struct& s2);
int Int(int x);
int Int(double x);

#include <list>

using std::cout;
using std::flush;
using std::endl;


static int cmp_count = 0;
static int smaller_count = 0;
static int equal_count = 0;


template <class T>
int func1(leda::list<T> L) { return L.size(); }

template <class T>
int func2(std::list<T> L) { return L.size(); }

template <class T>
int cmp_inv(const T& x, const T& y) { return compare(y,x); }


template <class T>
void test_list(int N, leda::list<T>& L, std::list<T>& SL)
{
  leda::list<T> L1;
  std::list<T> SL1;

  T*  A = new T[N];

  int i;
  for (i=0; i<N; i++) A[i] = (T)leda::rand_int(1,10000);

  float t,t1;

  float leda_t = 0;
  float stl_t = 0;


  cout << "build list     ";
  cout.flush();

  leda::rand_int.set_seed(1234*N);
  t = leda::used_time();
  for (i = 0; i < N; i++) L.append(A[i]);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  leda::rand_int.set_seed(1234*N);
  t = leda::used_time();
  for (i = 0; i < N; i++) SL.push_back(A[i]);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;



  cout << "pop and push   ";
  cout.flush();

  t = leda::used_time();
  //while (!L.empty()) { L1.push(L.pop_back()); }
  while (!L.empty()) { L1.push(L.back()); L.pop_back(); }
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  while (!SL.empty()) { SL1.push_front(SL.back()); SL.pop_back(); }
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;



  cout << "reversing      ";
  cout.flush();

  t = leda::used_time();
  //L1.reverse();
  L1.reverse_items();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL1.reverse();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


  cout << "copy constr    ";
  cout.flush();

  t = leda::used_time();
  func1(L1);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  func2(SL1);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;



  cout << "assignment     ";
  cout.flush();

  t = leda::used_time();
  L = L1;
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL = SL1;
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;



  cout << "clearing       ";
  cout.flush();

  t = leda::used_time();
  L1.clear();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL1.erase(SL1.begin(),SL1.end());
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


  cmp_count = 0;
  smaller_count = 0;
  equal_count = 0;


  cout << "sorting        ";
  cout.flush();

  t = leda::used_time();
  //L.sort(cmp_inv);
  L.sort();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL.sort();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


  cout << "sorting again  ";
  cout.flush();

  t = leda::used_time();
  //L.sort(cmp_inv);
  L.sort();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL.sort();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


  cout << "merging        ";
  cout.flush();
  L1 = L;
  SL1 = SL;

  t = leda::used_time();
  L.merge(L1);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  t = leda::used_time();
  SL.merge(SL1);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


  cout << "unique         ";
  cout.flush();

  t = leda::used_time();
  L.unique();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;


  t = leda::used_time();
  SL.unique();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;


/*
  cout << "unique again   ";
  cout.flush();

  t = leda::used_time();
  L.unique();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;


  t = leda::used_time();
  SL.unique();
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;
*/



  cout << "iteration      ";
  cout.flush();

  int sum1 = 0;
  t = leda::used_time();
  //T x;
  //forall(x,L) sum1 += Int(x);
  leda::list_item it1;
  forall_items(it1,L) sum1 += Int(L[it1]);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << flush;
  leda_t += t1;

  int sum2 = 0;
  typename std::list<T>::iterator it2;
  t = leda::used_time();
  for(it2 = SL.begin(); it2 != SL.end(); it2++) sum2 += Int(*it2);
  t1 = leda::used_time(t);
  cout << leda::string("    %5.3f sec",t1) << endl;
  stl_t += t1;

  it1 = L.first();
  for(it2 = SL.begin(); it2 != SL.end(); it2++) 
  { int i =  Int(*it2);
    int j =  Int(L[it1]);
    if (i != j)
       cout << i << "    " << j << endl;
    it1 = L.succ(it1);
   }

  if (sum1 != sum2) 
     cout << "Error: lists are different." << endl;


  cout << "--------------------------------------------" << endl;
  cout << "total          ";
  cout << leda::string("    %5.3f sec    %5.3f sec",leda_t,stl_t) << endl;
  cout << endl;
  cout << endl;

  delete[] A;
}



class  my_struct {
public:

  int x,y,z;



my_struct(int a=0) : x(a), y(2*a), z(3*a) {}

/*
friend int Int(const my_struct& s);
friend int  compare(const my_struct& s1, const my_struct& s2);
friend bool operator==(const my_struct& s1, const my_struct& s2);
friend bool operator<(const my_struct& s1, const my_struct& s2);
*/

/*
friend ostream& operator<<(ostream& o, const my_struct& s) 
{ o << s.x << " " << s.y << " " << s.z; return o; }

friend istream& operator>>(istream& i, my_struct&) { return i; }
*/

LEDA_MEMORY(my_struct)

};



inline int compare(const my_struct& s1, const my_struct& s2)
{ cmp_count++;
  if (s1.x < s2.x) return -1;
  else if (s1.x > s2.x) return +1;
  else if (s1.y < s2.y) return -1;
  else if (s1.y > s2.y) return +1;
  else if (s1.z < s2.z) return -1;
  else if (s1.z > s2.z) return +1;
  else return 0;
 }

inline bool operator==(const my_struct& s1, const my_struct& s2)
{ equal_count++;
  return s1.x == s2.x && s1.y == s2.y && s1.z == s2.z; }

inline bool operator<(const my_struct& s1, const my_struct& s2)
{ smaller_count++;
  if (s1.x < s2.x) return true;
  else if (s1.x > s2.x) return false;
  else if (s1.y < s2.y) return true;
  else if (s1.y > s2.y) return false;
  else if (s1.z < s2.z) return true;
  else if (s1.z > s2.z) return false;
  else return false;
 }

#if defined(_MSC_VER)
inline bool operator!=(const my_struct& s1, const my_struct& s2) 
{ return !(s1 == s2); }

inline bool operator>(const my_struct& s1, const my_struct& s2)
{ return s2 < s1; }
#endif



inline int Int(const my_struct& s) { return s.x; }

inline int Int(int x)    { return x; }
inline int Int(double x) { return (int)x; }


int main () 
{
cout << endl;

cout << "\
We compare the list data types of LEDA (leda::list) and the C++ standard\n\
library (std::list). To see a considerably difference you should use a \n\
large number of entries  e.g. N >= 1 Million.\n";

cout << endl;

  int N = leda::read_int("Number of list entries: N =  "); 
  cout << endl;

  cout << "LIST<INT>          leda::list   std::list" << endl;
  cout << "-----------------------------------------" << endl;
  leda::list<int> L1;
  std::list<int> SL1;
  test_list(N,L1,SL1);


  cout << "LIST<DOUBLE>       leda::list   std::list" << endl;
  cout << "-----------------------------------------" << endl;
  leda::list<double> L3;
  std::list<double> SL3;
  test_list(N,L3,SL3);


  cout << "LIST<CLASS>        leda::list   std::list" << endl;
  cout << "-----------------------------------------" << endl;
  leda::list<my_struct> L4;
  std::list<my_struct> SL4;
  test_list(N,L4,SL4);

  cout << endl;
  cout << "FINISHED" << endl;
  cout << endl;

  return 0;
}
