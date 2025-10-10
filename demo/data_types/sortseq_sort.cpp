/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sortseq_sort.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:20 $

#include <assert.h>
#include <LEDA/core/list.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/impl/skiplist.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


IO_interface I("Sorting with Finger Searching");


int convert(int a, int y){ return y;}

double convert(double a, int y){ return ((double) y); }

typedef four_tuple<int,int,int,int> Quad;

Quad convert(Quad a,int y)
       { return Quad(0,0,0,y);}



void report(const char* s, float t)
{
I.set_precision(3);
I.write_table(" & ",t);
I.write_demo(s,t);
}



template <class K>
void sort_demo(K a, int n, int f, string s)
{ 
  I.write_table(s);
  I.write_demo(string("test with type ") + s);

  list<K> L;
  int i;
  for(i = n - 1; i >= n - f; i--) L.append(convert(a,i)); 
  for(i = 0; i < n - f; i++)      L.append(convert(a,i));

  sortseq<K,int,skiplist> S; 
  typedef typename sortseq<K,int,skiplist>::item seq_item;

  seq_item it = 0;
  K k; 
  float T = used_time();
  
  forall(k,L) S.insert(k, 0); 
  report("repeated insertion sort",used_time(T));
  S.clear();

  T = used_time();
    
  forall(k, L)
  { if (S.empty()) it = S.insert(k, 0); 
    else 


    { seq_item it = S.finger_locate_from_rear(k); 
      if (it) S.insert_at(it,k,0,leda::before); 
      else S.insert_at(S.max_item(),k,0,leda::after);
    }
  }

  report("finger search from rear end",used_time(T));
  S.clear();

  T = used_time();
  
  forall(k, L)
  { if (S.empty()) it = S.insert(k, 0); 
    else 
    { seq_item it = S.finger_locate(k); 
      if (it) S.insert_at(it,k,0,leda::before); 
      else S.insert_at(S.max_item(),k,0,leda::after);
    }
  }

  report("finger search from both ends",used_time(T));
  S.clear();

  T = used_time();
  
  forall(k, L)
  { if (S.empty()) it = S.insert(k, 0); 
    else 
    { it = S.finger_locate(it,k); 
      it = ( it ? S.insert_at(it,k,0,leda::before) : 
                    S.insert_at(S.max_item(),k,0,leda::after) );
    }
  }

  report("finger search from last insertion",used_time(T));
  S.clear();
  
  T = used_time();
  list<K> L1(L);
  L1.sort();
  report("list sort",used_time(T));
  
  if (s == "quadruples") I.write_table(" \\\\ \\hline \\hline \n");
  else I.write_table(" \\\\ \\hline \n");

}

int main(){ 

I.write_demo("This demo compares four different sorting routines \
on an almost sorted sequence. See the LEDA book for more \
details on this demo");


int n = I.read_int("n = ",500000);
int f = I.read_int("f = ",50);


int a1 = 0; double a2 = 0.0; Quad a3(0,0,0,0);

sort_demo(a1,n,f,string("int")); I.stop();
sort_demo(a2,n,f,string("double")); I.stop();
#if !defined(__aCC__)
sort_demo(a3,n,f,string("quadruples")); I.stop();
#endif
return 0;
}

