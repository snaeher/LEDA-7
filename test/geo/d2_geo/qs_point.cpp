/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  qs_point.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include <LEDA/core/array.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



inline bool smaller(rat_point x, rat_point y)
{ return rat_point::cmp_xy(x,y) < 0; }

inline bool smaller_F(rat_point x, rat_point y) 
{ return rat_point::cmp_xy_F(x,y) < 0; }


class rat_point_smaller_F {
public:
bool operator()(const rat_point& a, const rat_point& b) const
{ double d = a.XD()*b.WD() - b.XD()*a.WD();
  if (d == 0) d = a.YD()*b.WD() - b.YD()*a.WD();
  return d < 0;
}
};

class rat_point_smaller_FX {
public:
bool operator()(const rat_point& a, const rat_point& b) const
{ return a.XD()*b.WD() < b.XD()*a.WD(); }
};




class rat_point_smaller {
public:
bool operator()(const rat_point& a, const rat_point& b) const
{ return rat_point::cmp_xy(a,b) < 0; }
};

class rat_point_smaller_X {
public:
bool operator()(const rat_point& a, const rat_point& b) const
{ return rat_point::cmp_x(a,b) < 0; }
};



template<class ARRAY, class SMALLER>
int check_sort(const ARRAY& A, int n, const SMALLER& smaller)
{ int err = 0;
  for(int i=0; i<n-1; i++)
    if (smaller(A[i+1],A[i])) err++;
  return err;
}



int main()
{
  int N = read_int("N = ");

  list<rat_point> L;
  list<rat_point> L2;

  string gen;

  while (gen == "")
  { gen  = read_string("generator d(isk)/s(quare)/c(ircle) : ");
    if (gen[0] == 'd')
       random_points_in_disc(N,100000,L);
    else if (gen[0] == 's')
       random_points_in_square(N,100000,L);
    else if (gen[0] == 'c')
       random_points_on_circle(N,100000,L);
    else gen = "";
  }

  L.permute();


  array<rat_point> ARR(N);
  array<rat_point> ARR1(N);
  array<rat_point> ARR2(N);

  rat_point* A = new rat_point[N];

  int i = 0;
  rat_point p;
  forall(p,L) 
  { L2.append(p);
    ARR2[i] = ARR1[i] = ARR[i] = A[i] = p;
    i++;
   }


  rat_point_smaller    s;
  rat_point_smaller_X  sX;

  rat_point_smaller_F  sF;
  rat_point_smaller_FX sFX;


  float T = used_time();

  cout << string("list::sort      ") << flush;
  L.sort();
  cout << string("%.2f sec",used_time(T)) << endl;

  cout << string("list::sort_F    ") << flush;
  L2.sort(sF);
  cout << string("%.2f sec",used_time(T)) << endl;

  cout << endl;


  cout << string("array::sort     ") << flush;
  ARR.sort();
  cout << string("%.2f sec", used_time(T)) << endl;

  cout << string("array::sort_F   ") << flush;
  ARR1.sort(sF);
  cout << string("%.2f sec", used_time(T)) << endl;

  cout << string("insertion sort  ") << flush;
  insertion_sort(ARR1.first_item(),ARR1.last_item(),s);
  cout << string("%.2f sec",used_time(T)) << endl;

  cout << string("check sorting   ") << flush;
  int err = check_sort(ARR1,N,s);
  cout << string("%.2f sec   err = %d",used_time(T), err) << endl;

  cout << endl;


  cout << string("quicksort_SF    ") << flush;
  quicksort(A,A+N-1,s,sF);
  cout << string("%.2f sec",used_time(T)) << endl;

  cout << string("check sorting   ") << flush;
  err = check_sort(A,N,s);
  cout << string("%.2f sec   err = %d",used_time(T), err) << endl;

  cout << endl;



  cout << string("array::sort_FX  ") << flush;
  ARR2.sort(sFX);
  cout << string("%.2f sec", used_time(T)) << endl;

  cout << string("insertion sort  ") << flush;
  insertion_sort(ARR2.first_item(),ARR2.last_item(),sX);
  cout << string("%.2f sec",used_time(T)) << endl;

  cout << string("check sorting   ") << flush;
  err = check_sort(ARR2,N,sX);
  cout << string("%.2f sec   err = %d",used_time(T), err) << endl;

  cout << endl;




  int err_count = 0;

  for(i=0; i<N-1; i++) 
    if (smaller(A[i+1],A[i])) err_count++;
  cout << endl;
  cout << "err  = " << err_count << endl;
  cout << endl;

  delete[] A;

  return 0;
}
