/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  dic_performance_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:18 $


#include <LEDA/core/dictionary.h>
#include <LEDA/core/map.h>
#include <LEDA/core/IO_interface.h>

#include <LEDA/core/impl/ch_hash.h>
#include <LEDA/core/impl/avl_tree.h>
#include <LEDA/core/impl/bin_tree.h>
#include <LEDA/core/impl/rs_tree.h>
#include <LEDA/core/impl/rb_tree.h>
#include <LEDA/core/impl/skiplist.h>
#include <LEDA/core/impl/ab_tree.h>
#include <LEDA/core/impl/bb_tree.h>

using namespace leda;

using std::cout;
using std::endl;


IO_interface I("Running Times of Sparse Array Implementations");

template<class impl, class E>
void dic_test(dictionary<E,E,impl>& D, int N, E* A, const char* name)
{ 
  typedef typename dictionary<E,E,impl>::item dic_item;

  string mes = name;

  float T; 
  float T0 = T = used_time();

  int i;
  for(i = 0; i < N; i++)  D.insert(A[i],0);
  mes += ": insert: " + string("%8.4f",used_time(T));

  bool ok = true;
  for(i=0; i<N; i++)  
  { dic_item it = D.lookup(A[i]);
    if (it == nil || D.key(it) != A[i])  
    { ok = false; 
      cout << "\n\ni = " << i << "A[i] = " << A[i];
      cout << "\nit is " << it;
      if ( it != nil ) cout <<"\nD.key(it) = " << D.key(it);
      cout.flush();
    }
    
  }

  if (!ok) error_handler(0,"errors in lookups");

  mes += ", lookups: " + string("%8.4f",used_time(T));

  for(i=0; i<N; i++)  D.del(A[i]);
  mes += ", deletes: " + string("%8.4f",used_time(T));

  mes += ", total: " +  string("%8.4f",used_time(T0));
  I.write_demo(mes);

  if (!D.empty())
  {  cout << "size = " << D.size() << endl;
     error_handler(1,"not empty");
   }

}



void map_test(map<int,int>& D, int N, int* A, const char* name)
{ string mes = name;

  float T; float T0 = T = used_time();

  int i;
  for(i = 0; i < N; i++)  D[A[i]] = 1;
  mes += ": insert: " + string("%8.4f",used_time(T));

  bool ok = true;
  for(i=0; i<N; i++)  
  { if ( D[A[i]] != 1 ) 
   { ok = false; 
      cout << "\n\nmap: errors in lookup";
      cout << "\n\ni = " << i << " A[i] = " << A[i];
      cout <<"\nD[A[i]] = " << D[A[i]];
      cout.flush();
    }
  }

  if (!ok) error_handler(0,"errors in lookups");
  mes += ", lookups: " + string("%8.4f",used_time(T));

  mes += ", total: " + string("%8.4f",used_time(T0));
  I.write_demo(mes);
}

int main()
{

  dictionary<int,int,ch_hash> CHH_DIC;

  dictionary<int,int,avl_tree> AVL_DIC;
  dictionary<int,int,bin_tree> BIN_DIC;
  dictionary<int,int,rb_tree>  RB_DIC;
  dictionary<int,int,rs_tree>  RS_DIC;
  dictionary<int,int,skiplist> SK_DIC;
  dictionary<int,int,bb_tree>  BB_DIC;
  dictionary<int,int,ab_tree>  AB_DIC;

  dictionary<double,double,avl_tree> AVL_DIC1;
  dictionary<double,double,bin_tree> BIN_DIC1;
  dictionary<double,double,rb_tree>  RB_DIC1;
  dictionary<double,double,rs_tree>  RS_DIC1;
  dictionary<double,double,skiplist> SK_DIC1;
  dictionary<double,double,bb_tree>  BB_DIC1;
  dictionary<double,double,ab_tree>  AB_DIC1;

  map<int,int> M1;
  map<int,int> M2;

  I.write_demo("This demo illustrates the speed of our sparse array \
implementations. We ask for an integer N and then perform three kinds \
of experiments. We suggest that you choose N fairly large, e.g., N = 10000 \
is a good choice for a first experiment.");

I.write_demo("N insertions, lookups, and deletions of random integers \
between 0 and 10000000.");

I.write_demo("N insertions, lookups, and deletions of the integers \
0 to N - 1.");

I.write_demo("N insertions, lookups, and deletions of random doubles \
between 0 and 10000000.");

  
  int N      = I.read_int("# keys = ");
  int*    Int    = new int[N];
  int*    Int1   = new int[N];
  double* Double = new double[N];

  int i;
  for(i=0; i<N; i++) Double[i] = Int[i] = rand_int(0,10000000);

  for(i=0; i<N; i++) Int1[i] = i;

  I.write_demo("Random Integers");

  dic_test(CHH_DIC,N,Int,"ch_hash");
  map_test(M1,N,Int,"map");
  dic_test(SK_DIC,N,Int,"skiplist");
  dic_test(RS_DIC,N,Int,"rs_tree");
  dic_test(BIN_DIC,N,Int,"bin_tree");
  dic_test(RB_DIC,N,Int,"rb_tree");
  dic_test(AVL_DIC,N,Int,"avl_tree");
  dic_test(BB_DIC,N,Int,"bb_tree");
  dic_test(AB_DIC,N,Int,"ab_tree");

  I.stop();

  I.write_demo("Integers 0 to N - 1");

  dic_test(CHH_DIC,N,Int1,"ch_hash");

  map_test(M2,N,Int1,"map");

  dic_test(SK_DIC,N,Int1,"skiplist");
  dic_test(RS_DIC,N,Int1,"rs_tree");
  //dic_test(BIN_DIC,N,Int1,"bin_tree");
  dic_test(RB_DIC,N,Int1,"rb_tree");
  dic_test(AVL_DIC,N,Int1,"avl_tree");
  dic_test(BB_DIC,N,Int1,"bb_tree");
  dic_test(AB_DIC,N,Int1,"ab_tree");

  I.stop();
  I.write_demo("Random  Doubles");

  dic_test(SK_DIC1,N,Double,"skiplist");
  dic_test(RS_DIC1,N,Double,"rs_tree");
  dic_test(BIN_DIC1,N,Double,"bin_tree");
  dic_test(RB_DIC1,N,Double,"rb_tree");
  dic_test(AVL_DIC1,N,Double,"avl_tree");
  dic_test(BB_DIC1,N,Double,"bb_tree");
  dic_test(AB_DIC1,N,Double,"ab_tree");
 
  return 0;
}
