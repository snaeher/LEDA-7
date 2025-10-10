/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sortseq_merge_sort.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:20 $

#include <LEDA/core/sortseq.h>
#include <LEDA/core/impl/skiplist.h>
#include <LEDA/core/array.h>
#include <LEDA/core/IO_interface.h>

#include <assert.h>

using namespace leda;


IO_interface I("Merge Sort");


void report(const char* s, float t)
{
I.set_precision(3);
I.write_table(" & ",t);
I.write_demo(s,t);
}



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
{  typedef typename sortseq<K,int,skiplist>::item seq_item;
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


typedef sortseq<int,int,skiplist> int_seq;

void merge_sort(int n, string s, void (*merge)(int_seq&, int_seq&))
{ 

typedef int_seq::item seq_item;

I.write_table(s);
I.write_demo(s);

  array<int_seq*> A(n);
  int i; 
    
  for (i = 0; i < n; i++)
  { A[i] = new int_seq;
    A[i]->insert(i,0);
  }
  A.permute();
 

  float UT = used_time();
  
  for (i = 1; i < n; i++)
  { merge(*A[0],*A[i]);
    delete A[i];
  }
  
  report("  unbalanced merge",used_time(UT));
  seq_item it; int j = 0;
  forall_items(it,*A[0]) 
    assert(A[0]->key(it) == j++);
  delete A[0];

    
  for (i = 0; i < n; i++)
  { A[i] = new int_seq;
    A[i]->insert(i,0);
  }
  A.permute();

  UT = used_time();
  
  while (n > 1)
  { int k = n/2;
    for (i = 0; i < k; i++)
    { merge(*A[i],*A[k + i]); 
      delete A[k+i];
    }
    if ( 2 * k < n ) // n is odd
      { A[k] = A[n - 1]; n = k + 1; }
    else 
      { n = k; }
  }

  report("    balanced merge",used_time(UT));
  j = 0;  
  forall_items(it,*A[0]) 
    assert(A[0]->key(it) == j++);
  delete A[0];

if ( s == "merging by finger search" )
I.write_table(" \\\\ \\hline \\hline \n"); 
else
I.write_table(" \\\\ \\hline \n");

}

int main(){ 
int n = I.read_int("n = ",100000);

merge_sort(n,"merging by repeated insertion",merging_by_repeated_insertion);
merge_sort(n,"merging by scanning",merging_by_scanning);
merge_sort(n,"merging by finger search",merging_by_finger_search);
return 0;

}

