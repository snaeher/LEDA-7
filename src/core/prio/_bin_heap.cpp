/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _bin_heap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/impl/bin_heap.h>

//------------------------------------------------------------------------------
// bin_heap: binary heaps  
//           (compressed representation with array doubling)
//
// S. Naeher (1993)
//
//------------------------------------------------------------------------------



#define KEY(i)   (HEAP[i]->key)
#define INF(i)   (HEAP[i]->inf)

LEDA_BEGIN_NAMESPACE

bin_heap::bin_heap(int n)  
{ if (n <= 0) 
     LEDA_EXCEPTION(1,string("bin_heap constructor: illegal size = %d",n));
  // sometimes we use HEAP[0], HEAP[count+1] as stoppers
  HEAP = new bin_heap_item[n+2];
  for(int i = 0; i <= n+1; i++) HEAP[i] = nil;
  count = 0; 
  max_size = n;  
}

bin_heap::bin_heap(const bin_heap& H)
{ max_size = H.max_size;
  count = H.count; 
  HEAP = new bin_heap_item[max_size+2];
  for(int i = 1; i <= count; i++) 
  { HEAP[i] = new bin_heap_elem(H.HEAP[i]->key, H.HEAP[i]->inf,i);
    H.copy_key(HEAP[i]->key);
    H.copy_inf(HEAP[i]->inf);
  }
  HEAP[0]=nil; HEAP[max_size+1]=nil; // set stop markers
}

bin_heap& bin_heap::operator=(const bin_heap& H)
{ clear();
  delete[] HEAP;
  max_size = H.max_size;
  count = H.count; 
  HEAP = new bin_heap_item[max_size+2];
  for(int i = 1; i <= count; i++) 
  { HEAP[i] = new bin_heap_elem(H.HEAP[i]->key, H.HEAP[i]->inf,i);
    copy_key(HEAP[i]->key);
    copy_inf(HEAP[i]->inf);
  }
  HEAP[0]=nil; HEAP[max_size+1]=nil; // set stop markers
  return *this;
}

bin_heap::~bin_heap()  
{ clear();
  delete[] HEAP; 
}

void bin_heap::clear()
{ for(int i=1; i <= count; i++) 
  { clear_key(KEY(i));
    clear_inf(INF(i));
    delete HEAP[i];
   }
  count = 0;
}


void bin_heap::rise(int pos, bin_heap_item it)
{ switch( key_type_id() ) {
  case INT_TYPE_ID    : int_rise(pos,it);
                        break;
  case DOUBLE_TYPE_ID : double_rise(pos,it);
                        break;
  default             : gen_rise(pos,it);
                        break;
  }
}


void bin_heap::gen_rise(int pos, bin_heap_item it)
{ HEAP[0] = it;  // use "it" as stopper
  int  pi = pos/2;                     // parent index
  bin_heap_item parent = HEAP[pi];     // parent node

  while (cmp(parent->key,it->key) > 0)
  { HEAP[pos] = parent;
    parent->index = pos;
    pos = pi;
    pi >>= 1;
    parent = HEAP[pi];
  }

  HEAP[pos] = it;
  it->index = pos;
}



void bin_heap::int_rise(int pos, bin_heap_item it)
{ HEAP[0] = it;  // use "it" as stopper
  int  pi = pos/2;                     // parent index
  bin_heap_item parent = HEAP[pi];     // parent node

  int  k0 = LEDA_ACCESS(int,it->key);

  while(LEDA_ACCESS(int,parent->key) > k0)
  { HEAP[pos] = parent;
    parent->index = pos;
    pos = pi;
    pi >>= 1;
    parent = HEAP[pi];
   }

  HEAP[pos] = it;
  it->index = pos;
}


void bin_heap::double_rise(int pos, bin_heap_item it)
{ HEAP[0] = it;  // use "it" as stopper
  int  pi = pos/2;                     // parent index
  bin_heap_item parent = HEAP[pi];     // parent node

  double  k0 = LEDA_ACCESS(double,it->key);

  while(LEDA_ACCESS(double,parent->key) > k0)
  { HEAP[pos] = parent;
    parent->index = pos;
    pos = pi;
    pi >>= 1;
    parent = HEAP[pi];
   }

  HEAP[pos] = it;
  it->index = pos;
}




void bin_heap::sink(int pos, bin_heap_item it)
{ switch( key_type_id() ) {
  case INT_TYPE_ID    : int_sink(pos,it);
                        break;
  case DOUBLE_TYPE_ID : double_sink(pos,it);
                        break;
  default             : gen_sink(pos,it);
                        break;
  }
}



void bin_heap::gen_sink(int pos, bin_heap_item it)
{ int ci = 2*pos;       // child index
  bin_heap_item child;  // child node
  HEAP[count+1] = HEAP[count];   // stopper

  while (ci <= count)
  { child = HEAP[ci];
    if (ci < count && cmp(KEY(ci+1),child->key) < 0) child = HEAP[++ci];
    if (cmp(it->key,child->key) <= 0) break; 
    HEAP[pos] = child;
    child->index = pos;
    pos = ci;
    ci <<= 1;
  }

  HEAP[pos] =it;
  it->index = pos;
}



void bin_heap::int_sink(int pos, bin_heap_item it)
{ int ci = 2*pos;       // child index
  bin_heap_item child;  // child node
  HEAP[count+1] = HEAP[count];   // stopper

  int k0 = LEDA_ACCESS(int,it->key);

  while (ci <= count)
  { child = HEAP[ci];
    if (LEDA_ACCESS(int,KEY(ci+1)) < LEDA_ACCESS(int,child->key)) 
      child = HEAP[++ci];
    if (k0 <= LEDA_ACCESS(int,child->key))  break;
    HEAP[pos] = child;
    child->index = pos;
    pos = ci;
    ci <<= 1;
   }

  HEAP[pos] =it;
  it->index = pos;
}

void bin_heap::double_sink(int pos, bin_heap_item it)
{ int ci = 2*pos;       // child index
  bin_heap_item child;  // child node
  HEAP[count+1] = HEAP[count];   // stopper

  double k0 = LEDA_ACCESS(double,it->key);

  while (ci <= count)
  { child = HEAP[ci];
    if (LEDA_ACCESS(double,KEY(ci+1)) < LEDA_ACCESS(double,child->key)) 
      child = HEAP[++ci];
    if (k0 <= LEDA_ACCESS(double,child->key))  break;
    HEAP[pos] = child;
    child->index = pos;
    pos = ci;
    ci <<= 1;
   }

  HEAP[pos] =it;
  it->index = pos;
}




void bin_heap::decrease_key(bin_heap_item it, GenPtr k)
{ /* if (cmp(it->key,k)<0) 
       LEDA_EXCEPTION(1,"bin_heap: key too large in decrease_key");
   */
  switch( key_type_id() ) {

  case INT_TYPE_ID:
            it->key = k;
            int_rise(it->index, it);
            break;

  case DOUBLE_TYPE_ID:
            clear_key(it->key);
            copy_key(k);
            it->key = k;
            double_rise(it->index, it);
            break;

  default : clear_key(it->key);
            copy_key(k);
            it->key = k;
            gen_rise(it->index, it);
            break;
  }
}


bin_heap_item bin_heap::insert(GenPtr k, GenPtr i) 
{ 
  bin_heap_item* H;

  if (count == max_size)  // resize
  { // for small sizes we double the size, for big sizes we increase by a constant
    if (max_size <= 1024) max_size *= 2;     // double array
    else                  max_size += 1024;  // increase by constant amount
    H = new bin_heap_item[max_size+2];
    for(int i=1; i<= count; i++) H[i] = HEAP[i];
    delete[] HEAP;
    HEAP = H;
   }

  count++;
  copy_key(k);
  copy_inf(i);
  bin_heap_item it = new bin_heap_elem(k,i,count);
  rise(count,it);

  return it;
}




void bin_heap::del_item(bin_heap_item it)
{ bin_heap_item p = HEAP[count];

  HEAP[count] = nil;

  count--;

  if (it != p)
  { 
    switch( key_type_id() ) {

    case INT_TYPE_ID: 
              if (LEDA_ACCESS(int,p->key) > LEDA_ACCESS(int,it->key))
                 int_sink(it->index, p);
              else
                 int_rise(it->index, p);
              break;

    case DOUBLE_TYPE_ID: 
              if (LEDA_ACCESS(double,p->key) > LEDA_ACCESS(double,it->key))
                 double_sink(it->index, p);
              else
                 double_rise(it->index, p);
              break;

    default : if (cmp(p->key,it->key) > 0)
                 gen_sink(it->index, p);
              else
                 gen_rise(it->index, p);
              break;
    }
  }


  clear_key(it->key);
  clear_inf(it->inf);

  delete it;
}


void bin_heap::change_inf(bin_heap_item it, GenPtr i) 
{ clear_inf(it->inf);
  copy_inf(i);
  it->inf = i; 
 }

void bin_heap::print()
{ cout << "size = " << count << endl;
  for(int i=1;i<=count;i++) 
  { print_key(KEY(i));
    cout << "-";
    print_inf(INF(i));
    cout << "  ";
   }
  cout << endl;
}

LEDA_END_NAMESPACE
