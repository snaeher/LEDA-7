/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _k_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/k_heap.h>


#define KEY(i)   (HEAP[i]->key)
#define INF(i)   (HEAP[i]->inf)

#define PARENT(pos) ((K+pos-2)/K)
#define CHILD(pos)  ((pos-1)*K+2)

LEDA_BEGIN_NAMESPACE

k_heap::k_heap(int n, int k)  
{ if (n<=0) LEDA_EXCEPTION(1,string("k_heap constructor: illegal size = %d",n));
  K = k;
  HEAP = new k_heap_item[n+2];
  for(int i = 0; i <= n; i++) HEAP[i] = nil;
  count = 0; 
  max_size = n;
}

k_heap::k_heap(const k_heap& H)
{ K = H.K;
  max_size = H.max_size;
  count = H.count; 
  HEAP = new k_heap_item[max_size+2];
  for(int i = 1; i <= count; i++) 
  { HEAP[i] = new k_heap_elem(H.HEAP[i]->key, H.HEAP[i]->inf,i);
    H.copy_key(HEAP[i]->key);
    H.copy_inf(HEAP[i]->inf);
  }
}

k_heap& k_heap::operator=(const k_heap& H)
{ clear();
  delete[] HEAP;
  K = H.K;
  max_size = H.max_size;
  count = H.count; 
  HEAP = new k_heap_item[max_size+2];
  for(int i = 1; i <= count; i++) 
  { HEAP[i] = new k_heap_elem(H.HEAP[i]->key, H.HEAP[i]->inf,i);
    copy_key(HEAP[i]->key);
    copy_inf(HEAP[i]->inf);
  }
  return *this;
}

k_heap::~k_heap()  
{ clear();
  delete[] HEAP; 
}

void k_heap::clear()
{ for(int i=1; i <= count; i++) 
  { clear_key(KEY(i));
    clear_inf(INF(i));
    delete HEAP[i];
   }
  count = 0;
}


void k_heap::rise(int pos, k_heap_item it)
{ 
  HEAP[0] = it;  // use "it" as stopper

  if (pos > 1)
  { 
    int         pi = PARENT(pos);  // parent index
    k_heap_item p  = HEAP[pi];     // parent node

    if (key_type_id() == INT_TYPE_ID)
       while(p->key > it->key)
       { HEAP[pos] = p;
         p->index = pos;
         pos = pi;
         pi = PARENT(pi);
         p = HEAP[pi];
        }
    else
       while (cmp(p->key,it->key) > 0)
       { HEAP[pos] = p;
         p->index = pos;
         pos = pi;
         pi = PARENT(pi);
         p = HEAP[pi];
        }
   }

  HEAP[pos] = it;
  it->index = pos;
}


void k_heap::sink(int pos, k_heap_item it)
{ 
  int ci = CHILD(pos);   // child index

  k_heap_item p;         // child node


  HEAP[count+1] = HEAP[count];   // stopper

  if (key_type_id() == INT_TYPE_ID)
     while (ci <= count)
     { p = HEAP[ci];

       int r = leda_min(count+1,ci+K);

       for (int j=ci+1;j<r;j++)
         if (cmp(KEY(j),p->key)<0 ) 
         { ci = j;
           p = HEAP[j];
          }

       if (it->key <= p->key) break;

       HEAP[pos] = p;
       p->index = pos;
       pos = ci;
       ci = CHILD(ci);
      }
  else
     while (ci <= count)
     { p = HEAP[ci];

       int r = leda_min(count+1,ci+K);

       for (int j=ci+1;j<r;j++)
         if (cmp(KEY(j),p->key)<0 ) 
         { ci = j;
           p = HEAP[j];
          }
       

       if (cmp(it->key,p->key) <= 0) break; 
       HEAP[pos] = p;
       p->index = pos;
       pos = ci;
       ci = CHILD(ci);
      }

  HEAP[pos] =it;
  it->index = pos;
}




void k_heap::decrease_key(k_heap_item it, GenPtr k)
{
//if (cmp(it->key,k)<0) LEDA_EXCEPTION(1,"k-heap:key too large in decrease_key");
  clear_key(it->key);
  copy_key(k);
  it->key = k;
  rise(it->index, it);
}



k_heap_item k_heap::insert(GenPtr k, GenPtr i) 
{
  if (count == max_size) LEDA_EXCEPTION(1,"k_heap: overflow");
  count++;
  copy_key(k);
  copy_inf(i);
  k_heap_item it = new k_heap_elem(k,i,count);
  rise(count,it);

  return it;
}




void k_heap::del_item(k_heap_item it)
{ k_heap_item p = HEAP[count];

  HEAP[count] = nil;

  count--;

  if (it != p)
  { if (cmp(p->key,it->key) > 0)
       sink(it->index, p);
    else
       rise(it->index, p);
   }

  clear_key(it->key);
  clear_inf(it->inf);

  delete it;


}

void k_heap::change_inf(k_heap_item it, GenPtr i) 
{ clear_inf(it->inf);
  copy_inf(i);
  it->inf = i; 
 }

void k_heap::print()
{ 
  cout << "count = " << count << endl;
  for(int i=1;i<=count;i++) 
  { print_key(KEY(i));
    //cout << "-";
    //print_inf(INF(i));
    cout << "  ";
   }
  cout << endl;
}

LEDA_END_NAMESPACE
