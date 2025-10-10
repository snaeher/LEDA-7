/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sortseq_merge.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:18 $

#include <LEDA/system/basic.h>
#include <LEDA/core/tuple.h>

#include <LEDA/core/sortseq.h>
#include <LEDA/core/impl/skiplist.h>

#include <assert.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


IO_interface I("Merging with Finger Searching");


int convert(int a, int y){ return y;}

double convert(double a, int y){ return ((double) y); }

typedef four_tuple<int,int,int,int> int_quad;

int_quad convert(int_quad a,int y)
       { return int_quad(0,0,0,y);}


template < class K >
void merging_by_repeated_insertion(sortseq<K,int,skiplist>& T, sortseq<K,int,skiplist>& U)
{ 
  typedef typename sortseq<K,int,skiplist>::item seq_item;
  seq_item it = U.min_item();
  while ( it )
  { T.insert(U.key(it),U.inf(it));
    it = U.succ(it);
  }
}

template < class K >
void merging_by_scanning(sortseq<K,int,skiplist>& T, sortseq<K,int,skiplist>& U)
{ typedef typename sortseq<K,int,skiplist>::item seq_item;
  seq_item it1 = T.min_item();
  seq_item it2 = U.min_item();

  while ( it2 && compare(U.key(it2),T.key(it1)) < 0 )
  { T.insert_at(it1,U.key(it2),U.inf(it2),leda::before);
    it2 = U.succ(it2);
  }
  seq_item succ1 = T.succ(it1);
  while ( it2 )
  { K k2 = U.key(it2);
    while ( succ1 && compare(T.key(succ1),k2) < 0 ) 
    { it1 = succ1;
      succ1 = T.succ(succ1);
    }
    it1 = T.insert_at(it1,k2,U.inf(it2),leda::after);
    it2 = U.succ(it2);
  }
}

template < class K >
void merging_by_finger_search(sortseq<K,int,skiplist>& T, sortseq<K,int,skiplist>& U)
{ T.merge(U); }


void report(const char* s, float t)
{
I.set_precision(3);
I.write_table(" & ",t);
I.write_demo(s,t);
}



template <class K>
void merge_test(K a,int n, string s)
{ 
I.write_table(s + string(" \\\\ \\hspace*{\\fill}")); 
I.write_demo(string("Test with type ") + s);

for (int m = 1; m <= n; m *= 10)
{ 
I.write_table("$m = ",m,"$");
I.write_demo("m = ",m);

  sortseq<K,int,skiplist> S1;
  sortseq<K,int,skiplist> S2;
  int k = n/m;

  int i;
  for (i = 0; i < n; i++) 
            S1.insert(convert(a,2*i),0);
  for (i = 1; i <= m; i++) 
            S2.insert(convert(a,2*k*i+1),0); 

  sortseq<K,int,skiplist> R1(S1);
  sortseq<K,int,skiplist> R2(S2);
  float UT = used_time();
  merging_by_repeated_insertion(R1,R2);
  report("time for repeated insert",used_time(UT));
  R1.clear(); R2.clear();

  sortseq<K,int,skiplist> T1(S1);
  sortseq<K,int,skiplist> T2(S2);
  UT = used_time();
  merging_by_scanning(T1,T2);
  report("time for scan",used_time(UT));
  T1.clear(); T2.clear();
  
  UT = used_time();
  merging_by_finger_search(S1,S2);
  report("time for merging by finger search",used_time(UT));
if ( n < 10*m )
I.write_table("\\\\ \\hline \\hline \n");
else
I.write_table("\\\\ \\hline \n");
}
}



int main(){ 

I.write_demo("This demo compares three different merging routines.");
I.write_demo("Merging by repeated insertion.");
I.write_demo("Merging by scanning.");
I.write_demo("Merging by finger searching.");

int n = I.read_int("n = ",500000);


int a1 = 0; double a2 = 0.0; int_quad a3(0,0,0,0);

merge_test(a1,n,string("int"));
merge_test(a2,n,string("double"));
#if !defined(__aCC__)
merge_test(a3,n,string("quadruple"));
#endif

return 0;
}

