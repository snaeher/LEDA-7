/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _dlist.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// Doubly Linked Lists
//
// S. Naeher 
//
// last modifications: January 1997 (STL iterators and operations)
//                     March   1997 (sorting)
//                     August  1997 (sorting)
//                     August  1998 (sorting)
//------------------------------------------------------------------------------

#include <LEDA/core/impl/dlist.h>
#include <LEDA/core/quicksort.h>

LEDA_BEGIN_NAMESPACE

dlist::dlist()      
{ h=t=0;
  count=0;
}



dlist::dlist(const dlist& x)
{ 
  h = t = 0; 
  count = x.length(); 

  if (count == 0) return;
                              
  dlink* q = x.copy_elem(x.h);

  h = q;

  for (dlink* p = x.h->succ; p; p = p->succ) 
  { dlink* r = x.copy_elem(p);
    r->pred = q;
    q->succ = r;
    q = r;
   }
 
  t = q;
}

dlist::dlist(dlist&& x)
{ // move constructor;
  h = x.h;
  t = x.t;
  count = x.count;
  x.h = 0;
  x.t = 0;
  x.count = 0;
}



void dlist::recompute_length() const
{ int n = 0;
  for(dlink* it = h; it; it = it->succ)  n++;
  *(int*)&count = n;
}


//------------------------------------------------------------------------------

dlink* dlist::get_item(int i) const
{ 
  if (i < 0 || i >= length()) 
      LEDA_EXCEPTION(1,"dlist::get_item: index out of range");

  dlink* p = h;
  while ( p && i--) p = p->succ; 
  return p;
}

dlink* dlist::succ(dlink* p, int i)  const
{ while ( p && i--) p = p->succ; 
  return p;
}

dlink* dlist::pred(dlink* p, int i) const
{ while ( p && i--) p = p->pred; 
  return p;
}





dlink* dlist::insert(dlink* a, dlink* l, int dir) 
{ 
  //if (l==0) return dir ? append(a) : push(a); 
  //if (l==0) LEDA_EXCEPTION(1,"dlist::insert: nil position argument.");

  if (dir != leda::behind  && dir != leda::before)
    LEDA_EXCEPTION(1,"dlist::insert: illegal dir argument.");

  // for stl insert
  if (l==0) return append(a);

  dlink* s=l->succ;
  dlink* p=l->pred;
  
  if (dir==leda::behind) 
    { //insert a after l
      a->pred = l;
      a->succ = s;
      l->succ = a;
      if (l==t) t=a;
      else s->pred = a;
     }
  else
    { //insert before l
      a->pred = p;
      a->succ = l;
      l->pred = a;
      if (l==h) h=a;
      else p->succ = a;
     }

  if (count >= 0) count++;
  return a;
}


dlink* dlist::push(dlink* a)   
{ if (count >= 0) count++;
  if (h) { h->pred = a; a->succ = h; }
  else   { t = a; a->succ = 0; }
  a->pred = 0;
  h = a;
  return a;
 }
   
dlink* dlist::append(dlink* a)
{ if (count >= 0) count++;
  if (t) { t->succ = a; a->pred = t; }
  else   { h = a; a->pred = 0; }
  a->succ = 0;
  t = a;
  return a;
 } 
   

dlink* dlist::pop()    
{ if (h==nil) LEDA_EXCEPTION(1,"dlist::pop on empty list.");
  dlink* q=h; 
  h = h->succ;
  if (h) h->pred = 0;
  else t = 0;
  count--;
  return q;
}
   
   
dlink* dlist::pop_back()    
{ if (t==nil) LEDA_EXCEPTION(1,"dlist::pop_back on empty list.");
  dlink* q = t; 
  t = t->pred;
  if (t) t->succ = 0;
  else h = 0;
  count--;
  return q;
}


dlink* dlist::del(dlink* it)
{ if (it==nil) LEDA_EXCEPTION(999,"dlist: delete nil-item");
/*
  if (it==h) pop();
  if (it==t) pop_back();
*/
  dlink*  p = it->pred;
  dlink*  s = it->succ;

  if (p) p->succ = s;
  else h = s;

  if (s) s->pred = p;
  else t = p;

  count--;
  return it;
}

   
dlink* dlist::del(dlink* p, dlink* q, dlink* r) // for unique
{ // remove q  (p = pred(q) and r = succ(q))
  // used by list::unique
  p->succ = r;
  if (r) r->pred = p;
  else t = p;
  count--;
  return q;
}




void dlist::swap(dlist& l)
{ dlink* tmp = l.h; l.h = h; h = tmp;
  tmp = l.t; l.t = t; t = tmp;
  int tmp1 = l.count; l.count = count; count = tmp1;
}


void dlist::conc(dlist& L, int dir)
{ 
  if (h == nil)
  { h = L.h; 
    t = L.t; 
    count = L.count;
    L.h = L.t = 0;
    L.count = 0;
    return;
  }

  if (dir==leda::behind)
    { // append l 
      t->succ = L.h;
      if (L.h) { L.h->pred = t; t = L.t; } 
     }
  else 
    if (dir==leda::before)
      { //prepend l
        h->pred = L.t;
        if (L.t) { L.t->succ= h; h = L.h; } 
       }
    else
      LEDA_EXCEPTION(1,"dlist::conc: illegal dir argument.");

 if (count < 0 || L.count < 0)
    count = -1;
 else
    count += L.count;

 L.h = 0;
 L.t = 0;
 L.count = 0;
}


void dlist::splice(dlink* pos, dlist& l)
{ if (this == &l)
     LEDA_EXCEPTION(1,"list::splice: cannot splice list into itself");

  if (l.h == nil) return;
  if (pos == 0) { conc(l); return; }
  dlist l1;
  split(pos,*this,l1);
  conc(l);
  conc(l1);
 }

void dlist::splice(dlink* pos, dlist& l0, dlink* it1, dlink* it2)
{ if (l0.h == nil) return;
  dlist l1,l2;
  if (it1 == 0) it1 = l0.h;
  l0.split(it1,l0,l1);
  if (it2) l1.split(it2,l1,l2);
  l0.conc(l2);
  splice(pos,l1);
}

void dlist::splice(dlink* pos, dlist& l0, dlink* it)
{ if (it && pos != it && pos != it->succ)
    splice(pos,l0,it,it->succ); 
 }




void dlist::split(dlink* p, dlist& L1, dlist& L2, int dir)
{ 
  // split L0 at item p into L1 and L2 and  L is made empty
  // if p == nil copy L0 to L2 and make L1 empty (if not identical to L0)
  // if p != nil we have to distinguish two cases
  // dir == leda::before: p becomes first item of L2
  // dir == leda::behind:  p becomes last item of L1

  if (h == nil) LEDA_EXCEPTION(1,"dlist::split: list is empty.");
  if (&L1 == &L2) LEDA_EXCEPTION(1,"dlist::split: identical arguments.");

  if (dir != leda::before && dir!=leda::behind)
            LEDA_EXCEPTION(1,"dlist::split: illegal dir argument.");

  if (this != &L1) L1.clear();
  if (this != &L2) L2.clear();

  if (p == nil) 
  { p = h;
    dir = leda::before;
   }

 /* The first item of L1 is either h or nil depending whether L1 is non-empty
  * or not. L1 is empty if dir == leda::before and p->pred does not exist. 
  * A similar argument applies to L2. 
  */

  dlink* L1_last  = (dir == leda::behind) ? p : p->pred;
  dlink* L1_first = (L1_last)  ? h : nil;

  dlink* L2_first = (dir == leda::before) ? p : p->succ;
  dlink* L2_last =  (L2_first) ? t : nil;

  h = t = 0;
  count = 0;

  L1.h = L1_first;
  L1.t = L1_last;
  L1.count = -1; // size unknown
  if (L1_last) L1_last->succ = 0;

  L2.h = L2_first;
  L2.t = L2_last;
  L2.count = -1; // size unknown
  if (L2_first) L2_first->pred = 0;
}

void dlist::extract(dlink* it1, dlink* it2, dlist& result, bool inclusive)
{
  int dir1 = leda::before, dir2 = leda::behind;
  if (!inclusive) { dir1 = leda::behind; dir2 = leda::before; }

  split(it1, *this, result, dir1);
  if (it2) {
    dlist dummy;
    result.split(it2, result, dummy, dir2);
    conc(dummy);
  }
}

void dlist::move_to_front(dlink* it)
{
  if (it == nil) 
     LEDA_EXCEPTION(1,"dlist::move_to_front: nil argument");

  dlink*  p = it->pred; 
  dlink*  s = it->succ; 

  if (p == nil) return;  // first item, nothing to do

  // unchain

  p->succ = s;

  if (s != nil)
     s->pred = p;
  else
     t = p;

  // insert at front

  it->pred = nil;
  it->succ = h;

  h->pred = it;
  h = it;
}



void dlist::move_to_back(dlink* it)
{
  if (it == nil) 
     LEDA_EXCEPTION(1,"dlist::move_to_back: nil argument");

  dlink*  p = it->pred; 
  dlink*  s = it->succ; 

  if (s == nil) return;  // last item, nothing to do

  // unchain

  s->pred = p;

  if (p != nil)
     p->succ = s;
  else
     h = s;

  // insert at rear end

  it->succ = nil;
  it->pred = t;

  t->succ = it;
  t = it;
}
  

void dlist::reverse_items()
{ 
  if (h == nil) return;

  dlink* p = h;
  h = t;
  t = p;

  while (p)
  { dlink* q = p->succ;
    p->succ = p->pred;
    p->pred = q;
    p = q;
   }
}


void dlist::reverse_items(dlink* it1, dlink* it2)
{ //reverse sublist between it1 and it2 (inclusive)

  if (h == nil) LEDA_EXCEPTION(1,"dlist::reverse_items:  empty list.");

  if (it2 == nil) it2 = t;

  if (it1 == nil) 
        LEDA_EXCEPTION(1,"dlist::reverse_items: illegal arguments.");

  if (it1 == it2) return;

  dlink* p = it1;
  while (p && p != it2) p = p->succ;

  if (p != it2)
      LEDA_EXCEPTION(1,"dlist::reverse_items(it1,it2): it1 not before it2");

  int n = length();

  dlist L1;
  dlist L2;
  split(it1,L1,L2,0);    // it1 becomes first item of L1
  conc(L1);
  L2.split(it2,L1,L2,1); // it2 becomes last item of L1
  L1.reverse_items();
  conc(L1);
  conc(L2);
  count = n;
}



void dlist::reverse(dlink* it1, dlink* it2)
{ 
  while (it1 != it2 && it2->succ != it1) 
  { if (it1 == nil || it2 == nil)
       LEDA_EXCEPTION(1,"list::reverse(it1,it2): illegal arguments.");
    swap_inf(it1,it2);
    it1 = it1->succ;
    it2 = it2->pred;
   }
}

void dlist::reverse() { if (h) reverse(h,t); }


void dlist::permute(dlink** I)
{
  if (empty()) return;

  dlink** stop = I+count;
  dlink** q = I;

  dlink* it1 = 0;
  dlink* it2 = *q;
  dlink* it3 = 0;

  h = it2;

  while (++q < stop)
  { it3 = *q;
    it2->pred = it1;
    it2->succ = it3;
    it1 = it2;
    it2 = it3;
   }

  it2->pred = it1;
  it2->succ = 0;
  
  t = it2;
}


void dlist::permute()
{ 
  length();

  //dlink** A = new dlink*[count+2];
  dlink** A = LEDA_NEW_VECTOR(dlink*,count+2);
  dlink* x = h;
  int j;

  A[0] = A[count+1] = 0;
 
  for(j=1; j <= count; j++)
  { A[j] = x;
    x = x->succ;
   }

  // multi-threads: we need a local random source here (sn)
  random_source ran;

  for(j=1; j<count; j++)  
  { //int r = rand_int(j,count);
    int r = ran(j,count);
    x = A[j];
    A[j] = A[r];
    A[r] = x;
   }

  for(j=1; j<=count; j++) 
  { A[j]->succ = A[j+1];
    A[j]->pred = A[j-1];
   }

  h = A[1];
  t = A[count];
  
  LEDA_DEL_VECTOR(A);
}
        

void dlist::bucket_sort(int i, int j)
{ 
  if (h==nil) return; // empty list

  int n = j-i+1;

  //dlink** bucket= new dlink*[n+1];
  dlink** bucket = LEDA_NEW_VECTOR(dlink*,n+1);

  if (bucket == 0)
    LEDA_EXCEPTION(1,
      string("list::bucketsort: cannot allocate %d buckets (out of memory)",n));
  
  dlink** stop = bucket + n;
  dlink** p;

  dlink* q;
  dlink* x;

  for(p=bucket;p<=stop;p++)  *p = 0;

  while (h) 
  { x = h; 
    h = h->succ;
    int k = ord(x);
    if (k >= i && k <= j) 
     { p = bucket+k-i;
       x->pred = *p;
       if (*p) (*p)->succ = x;
       *p = x;
      }
    else 
       LEDA_EXCEPTION(4,string("bucket_sort: value %d out of range",k)) ;
   }

 for(p=stop; *p==0; p--); 

 // now p points to the end of the rightmost non-empty bucket
 // make it the new head  of the list (remember: list is not empty)

 t = *p;
 t->succ = nil;

 for(q = *p; q->pred; q = q->pred); // now q points to the start of this bucket

 // link buckets together from right to left:
 // q points to the start of the last bucket
 // p points to end of the next bucket

 while(--p >= bucket) 
   if (*p)
   { (*p)->succ = q;
     q->pred = *p;
     for(q = *p; q->pred; q = q->pred); 
    }

 h = q;   // head = start of leftmost non-empty bucket

 //delete[] bucket;
 LEDA_DEL_VECTOR(bucket);
}


void dlist::bucket_sort()
{ 
  if (h==nil) return; // empty list
  int i = ord(h);
  int j = i;
  
  dlink* p = h -> succ;
  
  while (p)
  { int o = ord(p);
    if ( o < i ) i = o;
    if ( o > j ) j = o;
    p = p -> succ;
  }

  bucket_sort(i,j);
}



dlist& dlist::operator=(const dlist& x)
{ 
  if (this == &x) return *this;

  clear();
  
  count = x.length(); 

  if (count == 0) return *this;
                              
  dlink* q = copy_elem(x.h);

  h = q;

  for (dlink* p = x.h->succ; p; p = p->succ) 
  { dlink* r = copy_elem(p);
    r->pred = q;
    q->succ = r;
    q = r;
   }
 
  t = q;

  return *this;
}



dlist dlist::operator+(const dlist& x)  // concatenation
{ dlist y = x;
  for (dlink* p = t; p; p = p->pred) y.push(x.copy_elem(p));    
  return y;
}


void dlist::clear()
{ 
  if (h==nil) return;

/*
  int id = elem_type_id();
  if (id != INT_TYPE_ID && id != FLOAT_TYPE_ID && 
      id != DOUBLE_TYPE_ID && id != PTR_TYPE_ID)
  {
    for(dlink* p = h; p; p = p->succ) clear_inf(p);
   }
*/

  if (elem_destructible()) {
    for(dlink* p = h; p; p = p->succ) clear_inf(p);
  }

  std_memory.deallocate_list(h,t,elem_size());

  h=t=nil;
  count=0;
}


void dlist::print(ostream& out, string s, char space) const
{ dlink* l = h;
  cout << s;
  while (l)
  { out << string(space);
    print_elem(l,out); 
    l = l->succ;
  }
  out.flush();
}

void dlist::print(ostream& out, char space) const
{ dlink* l = h;
  if (l)
  { print_elem(l,out); 
    l = l->succ;
    while (l)
    { out << string(space);
      print_elem(l,out); 
      l = l->succ;
     }
  }
  out.flush();
}


void dlist::read(istream& in, string s, int delim)
{ char c;
  cout << s;
  clear();
  if (delim == EOF)
     for(;;)
     { while (in.get(c) && isspace(c)) {}
       if (!in) break;
       in.putback(c);
       dlink* x = read_elem(in); 
       append(x);
      }
  else
     for(;;)
     { while (in.get(c) && isspace(c) && c!=delim) {}
       if (!in || c==delim) break;
       in.putback(c);
       dlink* x = read_elem(in); 
       append(x);
      }
}


LEDA_END_NAMESPACE
