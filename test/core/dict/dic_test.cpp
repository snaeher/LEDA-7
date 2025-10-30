/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dic_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/dictionary.h>
#include <LEDA/core/array.h>

#include <LEDA/core/impl/ch_hash.h>
#include <LEDA/core/impl/avl_tree.h>
#include <LEDA/core/impl/avl_tree1.h>
#include <LEDA/core/impl/bin_tree.h>
#include <LEDA/core/impl/bin_tree1.h>
#include <LEDA/core/impl/rs_tree.h>
#include <LEDA/core/impl/rb_tree.h>
#include <LEDA/core/impl/skiplist.h>
#include <LEDA/core/impl/ab_tree.h>
#include <LEDA/core/impl/bb_tree.h>
/*
#include <LEDA/core/impl/cuckoo_hash.h>
*/

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


template<class T, class impl, class ARRAY>
void dic_test(dictionary<T,T,impl>& D, int N, const ARRAY& A, const char* name)
{ 
  typedef typename dictionary<T,T,impl>::item dic_item;

  cout << string("%-12s",name);
  cout.flush();

  double t0 = cpu_time();
  double t = t0;

  bool ok = true;
  int i;
  for(i=0; i<N; i++)
  { dic_item it;
    it = D.insert(A[i],A[i]);
    if (it == nil)  { cout << "error in insert: i == nil" << endl; }
    else
    if (D.key(it) != A[i])  {
     cout << "error in insert: " << A[i] << " != " << D.key(it) << endl;
     ok = false;
    }
   }

/*
dic_item it;
forall_items(it,D) cout << D.key(it) << endl;
*/

  if (!ok) error_handler(0,"errors in inserts");

  cout << string("%8.3f",cpu_time(t));
  cout.flush();


  for(i=0; i<N; i++)  
  { dic_item it = D.lookup(A[i]);
    if (it == nil || D.key(it) != A[i])  {
     //cout << string("error in lookup(%d): %d",A[i], it ? D.key(it) : 0) << endl;
     cout << "error in lookup: " << A[i] << endl;
     ok = false;
    }
   }

  if (!ok) error_handler(0,"errors in lookups");

  cout << string("%8.3f",cpu_time(t));
  cout.flush();

  for(i=0; i<N; i++)  D.del(A[i]);
  cout << string("%8.3f",cpu_time(t));

  cout << string("%8.3f",cpu_time(t0));

  if (!D.empty())
  {  cout << " NOT EMPTY ";	
     cout << "size = " << D.size() << endl;
   }
  

  cout << endl;

//memory_clear();

}

int cmp_int(const int& x, const int& y) { return -compare(x,y); }
int cmp_string(const string& x, const string& y) { return -compare(x,y); }
int cmp_double(const double& x, const double& y) { return -compare(x,y); }


int main()
{

cout << endl;
cout << "\
We compare various implementations for LEDA's dictionary data type.\n";
cout << endl;



  double total_time = cpu_time();

  // determine a new seed and report it!
  int seed = rand_int.reinit_seed();
/*
  cout << "seed = " << seed << endl;
*/

  dictionary<int,int,ch_hash> CH_DIC;

  dictionary<int,int,ab_tree>    AB16_DIC;
  dictionary<int,int,ab_tree>    AB32_DIC(2,32);
  dictionary<int,int,ab_tree>    AB64_DIC(2,64);
  dictionary<int,int,avl_tree>   AVL_DIC;
  dictionary<int,int,avl_tree1>  AVL1_DIC;
  dictionary<int,int,avl_tree>   AVL_DIC_c(cmp_int);
  dictionary<int,int,avl_tree1>  AVL1_DIC_c(cmp_int);
  dictionary<int,int,bin_tree>   BIN_DIC;
  dictionary<int,int,bin_tree1>  BIN1_DIC;
  dictionary<int,int,rb_tree>    RB_DIC;
  dictionary<int,int,rs_tree>    RS_DIC;
  dictionary<int,int,skiplist>   SK_DIC;
  dictionary<int,int,bb_tree>    BB_DIC;


  dictionary<string,string,ch_hash>    CH_DIC_S;

  dictionary<string,string,ab_tree>    AB16_DIC_S;
  dictionary<string,string,ab_tree>    AB32_DIC_S(2,32);
  dictionary<string,string,ab_tree>    AB64_DIC_S(2,64);

  dictionary<string,string,avl_tree>   AVL_DIC_S;
  dictionary<string,string,avl_tree>   AVL_DIC_S_c(cmp_string);

  dictionary<string,string,avl_tree1>  AVL1_DIC_S;
  dictionary<string,string,avl_tree1>  AVL1_DIC_S_c(cmp_string);

  dictionary<string,string,bin_tree>   BIN_DIC_S;
  dictionary<string,string,bin_tree1>  BIN1_DIC_S;
  dictionary<string,string,rb_tree>    RB_DIC_S;
  dictionary<string,string,rs_tree>    RS_DIC_S;
  dictionary<string,string,skiplist>   SK_DIC_S;
  dictionary<string,string,bb_tree>    BB_DIC_S;


  dictionary<double,double,ab_tree>    AB16_DIC1;
  dictionary<double,double,ab_tree>    AB32_DIC1(2,32);
  dictionary<double,double,ab_tree>    AB64_DIC1(2,64);

  dictionary<double,double,avl_tree>   AVL_DIC1;
  dictionary<double,double,avl_tree>   AVL_DIC1_c;

  dictionary<double,double,avl_tree1>  AVL1_DIC1;
  dictionary<double,double,avl_tree1>  AVL1_DIC1_c;

  dictionary<double,double,bin_tree>   BIN_DIC1;
  dictionary<double,double,bin_tree1>  BIN1_DIC1;
  dictionary<double,double,rb_tree>    RB_DIC1;
  dictionary<double,double,rs_tree>    RS_DIC1;
  dictionary<double,double,skiplist>   SK_DIC1;
  dictionary<double,double,bb_tree>    BB_DIC1;


  int N = read_int("# keys = ");

  array<int>    rand_int(N);
  array<int>    sorted_int(N);
  array<double> rand_double(N);

  array<string> rand_string(N);

  for(int i=0; i<N; i++) {
    rand_double[i] = i;
    rand_int[i]    = i;
    sorted_int[i]  = i;
    rand_string[i] = string("s=%d",i);
  }

  rand_int.permute();
  rand_double.permute();


  cout << endl;
  cout << "                insert  lookup  delete  total\n";

  cout << endl;
  cout << "Random Integers" << endl;
  cout << endl;

  dic_test(CH_DIC,    N, rand_int, "ch_hash");
  cout << endl;

  dic_test(AB16_DIC,  N, rand_int, "ab_tree16");
  dic_test(AB32_DIC,  N, rand_int, "ab_tree32");
  dic_test(AB64_DIC,  N, rand_int, "ab_tree64");
  cout << endl;

  dic_test(BIN1_DIC,  N, rand_int, "bin_tree1");
  dic_test(AVL1_DIC,  N, rand_int, "avl_tree1");
  //dic_test(AVL1_DIC_c,N, rand_int, "avl_tree1(c)");
  cout << endl;

  dic_test(BIN_DIC,   N, rand_int, "bin_tree");
  dic_test(AVL_DIC ,  N, rand_int, "avl_tree");
  //dic_test(AVL_DIC_c, N, rand_int, "avl_tree(c)");
  dic_test(RB_DIC,    N, rand_int, "rb_tree");
  dic_test(BB_DIC,    N, rand_int, "bb_tree");
  dic_test(RS_DIC,    N, rand_int, "rs_tree");
  cout << endl;

  dic_test(SK_DIC,    N, rand_int, "skiplist");
  cout << endl;

  cout << endl;
  cout << "Random Strings" << endl;
  cout << endl;

  dic_test(CH_DIC_S,    N, rand_string, "ch_hash");
  cout << endl;

  dic_test(AB16_DIC_S,  N, rand_string, "ab_tree16");
  dic_test(AB32_DIC_S,  N, rand_string, "ab_tree32");
  dic_test(AB64_DIC_S,  N, rand_string, "ab_tree64");
  cout << endl;

  dic_test(BIN1_DIC_S,  N, rand_string, "bin_tree1");
  dic_test(AVL1_DIC_S,  N, rand_string, "avl_tree1");
  //dic_test(AVL1_DIC_S_c,N, rand_string, "avl_tree1(c)");
  cout << endl;

  dic_test(BIN_DIC_S,   N, rand_string, "bin_tree");
  dic_test(AVL_DIC_S,   N, rand_string, "avl_tree");
  //dic_test(AVL_DIC_S_c, N, rand_string, "avl_tree(c)");
  dic_test(RB_DIC_S,    N, rand_string, "rb_tree");
  dic_test(BB_DIC_S,    N, rand_string, "bb_tree");
  dic_test(RS_DIC_S,    N, rand_string, "rs_tree");
  cout << endl;

  dic_test(SK_DIC_S,    N, rand_string, "skiplist");
  cout << endl;




  cout << endl;
  cout << "Sorted Integers" << endl;
  cout << endl;

  dic_test(CH_DIC,    N, sorted_int, "ch_hash");
  cout << endl;

  dic_test(AB16_DIC,  N, sorted_int, "ab_tree16");
  dic_test(AB32_DIC,  N, sorted_int, "ab_tree32");
  dic_test(AB64_DIC,  N, sorted_int, "ab_tree64");
  cout << endl;

//dic_test(BIN1_DIC,  N, sorted_int, "bin_tree1");
  dic_test(AVL1_DIC,  N, sorted_int, "avl_tree1");
//dic_test(AVL1_DIC_c,N, sorted_int, "avl_tree1(c)");
  cout << endl;

//dic_test(BIN_DIC,   N, sorted_int, "bin_tree");
  dic_test(AVL_DIC ,  N, sorted_int, "avl_tree");
//dic_test(AVL_DIC_c, N, sorted_int, "avl_tree(c)");
  dic_test(RB_DIC,    N, sorted_int, "rb_tree");
  dic_test(BB_DIC,    N, sorted_int, "bb_tree");
  dic_test(RS_DIC,    N, sorted_int, "rs_tree");
  cout << endl;

  dic_test(SK_DIC,    N, sorted_int, "skiplist");
  cout << endl;


  cout << endl;
  cout << "Random  Double's" << endl;
  cout << endl;

  dic_test(CH_DIC,    N, rand_double, "ch_hash");
  cout << endl;

  dic_test(AB16_DIC1,    N, rand_double, "ab_tree");
  dic_test(AB32_DIC1,  N, rand_double, "ab_tree64");
  dic_test(AB64_DIC1,  N, rand_double, "ab_tree64");
  cout << endl;

  dic_test(BIN1_DIC1,  N, rand_double, "bin_tree1");
  dic_test(AVL1_DIC1,  N, rand_double, "avl_tree1");
//dic_test(AVL1_DIC1_c,N, rand_double, "avl_tree(c)");
  cout << endl;

  dic_test(BIN_DIC1,   N, rand_double, "bin_tree");
  dic_test(AVL_DIC1 ,  N, rand_double, "avl_tree");
//dic_test(AVL_DIC1_c, N, rand_double, "avl_tree(c)");
  dic_test(RB_DIC1,    N, rand_double, "rb_tree");
  dic_test(BB_DIC1,    N, rand_double, "bb_tree");
  dic_test(RS_DIC1,    N, rand_double, "rs_tree");
  cout << endl;

  dic_test(SK_DIC,    N, rand_double, "skiplist");
  cout << endl;


  cout << string("Total Time: %.3f sec", cpu_time(total_time)) << endl;
  cout << endl;
 
  return 0;
}


