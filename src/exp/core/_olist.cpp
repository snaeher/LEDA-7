/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _olist.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/olist.h>

#ifdef SWAP
#undef SWAP
#endif

#define SWAP(a,b) { register obj_link* x = *a; *a = *b; *b = x; }

#define MIN_D 16 

//------------------------------------------------------------------------------
// Members of class obj_list
//
// S. Naeher (1994)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

obj_list::obj_list()      
{ h = nil; 
  t = nil;
  count = 0;
}

void obj_list::clear()
{ h = nil;
  t = nil;
  count = 0;
 }


obj_link* obj_list::get_item(int i) const
{ obj_link* p = h;
  while ( p && i--) p = p->succ_link; 
  return p;
}

obj_link* obj_list::succ(obj_link* p, int i)  const
{ while ( p && i--) p = p->succ_link; 
  return p;
}

obj_link* obj_list::pred(obj_link* p, int i) const
{ while ( p && i--) p = p->pred_link; 
  return p;
}


int obj_list::rank(obj_link* x)   const   /* rank by linear search */
{ obj_link* p = h;
  int r = 1;
  while ( p && p != x) 
  { p = p->succ_link; 
    r++;
   }
  return (p) ? r : 0;
} 


obj_link* obj_list::insert(obj_link* n, obj_link* l, int dir) 
{ 
  if (dir==0) //insert after l
    return insert(n,l);
  else //insert before l
    { obj_link* p=l->pred_link;
      n->pred_link = p;
      n->succ_link = l;
      l->pred_link = n;
      if (l==h) h=n;
      else p->succ_link = n;
      count++;
      return n;
    }
}


void obj_list::conc(obj_list& l)
{ if (t) { t->succ_link = l.h;
          if (l.h) { l.h->pred_link = t; t = l.t; } }
   else { h = l.h; t = l.t; }
 count = count+l.count;
 l.h = l.t = 0;
 l.count = 0;
}


void obj_list::split(obj_link* p, obj_list& l1, obj_list& l2)
{ 
  l1.clear();
  l2.clear();

  if (p==nil)    // l1 = empty,  l2 = l, l = empty;
  { l2.h = h;
    l2.t = t;
    l2.count = count;
    h = t = 0;
    count = 0;
    return;
   }

  if (h == 0) return;   /* empty list */


  if (p->pred_link)
  { l1.h = h;
    l1.t = p->pred_link;
    p->pred_link->succ_link = 0;
   }

  p->pred_link = 0;
  l2.h = p;
  l2.t = t;


  // have to set counters
  // "count the smaller half" gives amortized n log n  bound

  obj_link* l = l1.h;
  obj_link* r = l2.h;
  int    c = 0;

  while (l && r)
  { l = l->succ_link;
    r = r->succ_link;
    c++;
   }

  if (l==0)   // left end reached first
  { l1.count = c;
    l2.count = count - l1.count;
   }

  else
  { l2.count = c;
    l1.count = count - l2.count;
   }

  /* make original list empty */
  
  h = t = 0;
  count = 0;

}


void obj_list::del(obj_link* x)
{ 
  if (x==h)  
    pop();
  else
    if (x==t)  
       pop_back();
    else
       { obj_link*  p = x->pred_link;
         obj_link*  s = x->succ_link;
         p->succ_link = s;
         s->pred_link = p;
         count--;
        }
}


obj_link* obj_list::max(CMP_ITEM f) const
{ if (h==0) return 0;
  obj_link* m=h;
  obj_link* p=m->succ_link;

     while (p)
     { if (f(p,m) > 0) m=p;
       p=p->succ_link;
      }

  return m;
}

obj_link* obj_list::min(CMP_ITEM f) const
{ if (h==0) return 0;
  obj_link* m=h;
  obj_link* p=m->succ_link;

     while (p)
     { if (f(p,m) < 0) m=p;
       p=p->succ_link;
     }

  return m;
}


void obj_list::apply(APP_ITEM apply)
{ register obj_link* p = h;
  while (p)
  { apply(p);
    p = p->succ_link;
   }
}

void obj_list::permute()
{ 
  //obj_link** A = new obj_link*[count+2];
  obj_link** A = LEDA_NEW_VECTOR(obj_link*,count+2);
  obj_link* x = h;
  int j;

  A[0] = A[count+1] = 0;
 
  for(j=1; j <= count; j++)
  { A[j] = x;
    x = x->succ_link;
   }

  random_source ran;

  for(j=1; j<count; j++)  
  { int r = ran(j,count);
    x = A[j];
    A[j] = A[r];
    A[r] = x;
   }

  for(j=1; j<=count; j++) 
  { A[j]->succ_link = A[j+1];
    A[j]->pred_link = A[j-1];
   }

  h = A[1];
  t = A[count];
  
  //delete[] A;
  LEDA_DEL_VECTOR(A);
}
        

void obj_list::bucket_sort(int i, int j, ORD_ITEM ord)
{ 
  int n = j-i+1;

  //obj_link** bucket= new obj_link*[n+1];
  obj_link** bucket = LEDA_NEW_VECTOR(obj_link*,n+1);
  obj_link** stop = bucket + n;
  obj_link** p;

  for(p=bucket;p<=stop;p++)  *p = 0;

  while (h) 
  { obj_link* x = h; 
    h = h->succ_link;
    int k = ord(x);
    if (k >= i && k <= j) 
     { p = bucket+k-i;
       x->succ_link = *p;
       if (*p) (*p)->pred_link = x;
       *p = x;
      }
    else 
       LEDA_EXCEPTION(4,"bucket_sort: value out of range") ;
   }

 for(p=bucket; *p==0 && p<stop; p++);
  
 h = *p;

 obj_link* q = 0;

 if (h) 
 { for(q=h;q->succ_link; q = q->succ_link);
   h->pred_link = 0;
   p++;
  }

 while(p<stop) 
 { if (*p)
   { q->succ_link = *p;
     (*p)->pred_link = q;
     while(q->succ_link) q = q->succ_link;
    }
   p++;
  }

 t = q;

 //delete[] bucket;
 LEDA_DEL_VECTOR(bucket);
}

        
void obj_list::quick_sort(obj_link** l, obj_link** r, CMP_ITEM usr_cmp)
{ // use parameter usr_cmp

  register obj_link** i = l+(r-l)/2; //rand_int()%(r-l);
  register obj_link** k;
 
  if (usr_cmp((*i),(*r))>0) SWAP(i,r);

  SWAP(l,i);
 
  obj_link* s = (*l);
 
  i = l;
  k = r;

  for(;;)
  { while (usr_cmp((*(++i)),s)<0);
    while (usr_cmp((*(--k)),s)>0);
    if (i<k) SWAP(i,k) else break;
   }

  SWAP(l,k);

  if (k > l+MIN_D) quick_sort(l,k-1,usr_cmp);
  if (r > k+MIN_D) quick_sort(k+1,r,usr_cmp);
}
        


void obj_list::insertion_sort(obj_link** l, obj_link** r, obj_link** min_stop, 
                               CMP_ITEM usr_cmp)
{
  register obj_link** min=l;
  register obj_link** run;
  register obj_link** p;
  register obj_link** q;

  for (run = l+1; run <= min_stop; run++)
      if (usr_cmp((*run),(*min)) < 0) min = run;

  SWAP(min,l);

  if (r == l+1) return; 

  for(run=l+2; run <= r; run++)
  { for (min = run-1; usr_cmp((*run),(*min)) < 0; min--);
    min++;
    if (run != min) 
    { obj_link* save = *run;
      for(p=run, q = run-1; p > min; p--,q--) *p = *q;
      *min = save;
     }
   }
}



void obj_list::sort(CMP_ITEM f)
{ 
  if (count<=1) return;    // nothing to sort

  //obj_link** A = new obj_link*[count+2];
  obj_link** A = LEDA_NEW_VECTOR(obj_link*,count+2);

  register obj_link*  loc = h;
  register obj_link** p;
  register obj_link** stop = A+count+1;

  obj_link** left  = A+1;
  obj_link** right = A+count;
  obj_link** min_stop = left + MIN_D;

  if (min_stop > right) min_stop = right;

min_stop = right;

  for(p=A+1; p<stop; p++)
  { *p = loc;
    loc = loc->succ_link;
   }


   quick_sort(left,right,f);
   insertion_sort(left,right,min_stop,f);

  *A = *stop = 0;

  for (p=A+1;p<stop;p++) 
  { (*p)->succ_link = *(p+1);
    (*p)->pred_link = *(p-1);
   }

  h = A[1];
  t = A[count];

  //delete[] A;
  LEDA_DEL_VECTOR(A);
}

LEDA_END_NAMESPACE

