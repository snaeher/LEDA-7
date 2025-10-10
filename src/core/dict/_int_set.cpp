/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _int_set.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/int_set.h>

//------------------------------------------------------------------------------
//
//  S. N"aher (1993)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

typedef unsigned long  word;

int_set::int_set(int n)
{ sz = n; 
  low = 0;
  int i = 1+sz/SIZE_OF_ULONG;
  if ((V=new word[i]) == 0) LEDA_EXCEPTION(1,"int_set: out of memory"); 
  while (i--) V[i]=0;
 } 

int_set::int_set(int a, int b)
{ sz = b-a+1; 
  low = a;
  int i = 1+sz/SIZE_OF_ULONG;
  if ((V=new word[i]) == 0) LEDA_EXCEPTION(1,"int_set: out of memory"); 
  while (i--) V[i]=0;
 } 

int_set::int_set(const int_set& b)
{ sz = b.sz;
  low  = b.low;
  int n = 1+sz/SIZE_OF_ULONG;
  V = new word[n];
  while (n--) V[n] = b.V[n];
}

int_set& int_set::operator=(const int_set& b)
{ if (this == &b) return *this;
  delete[] V;
  sz = b.sz;
  low  = b.low;
  int n = 1+sz/SIZE_OF_ULONG;
  V = new word[n];
  while (n--) V[n] = b.V[n];
  return *this;
}

void int_set::clear()
{ int i = 1+sz/SIZE_OF_ULONG;
  while (i--) V[i]=0;
 }
  

int_set& int_set::join(const int_set& b) 
{ word* stop = V+sz/SIZE_OF_ULONG +1;
  word* p;
  word* q;
  for(p = V, q = b.V; p<stop; p++, q++) *p |= *q;
  return *this;
 }

int_set& int_set::intersect(const int_set& b) 
{ word* stop = V+sz/SIZE_OF_ULONG +1;
  word* p;
  word* q;
  for(p = V, q = b.V; p<stop; p++, q++) *p &= *q;
  return *this;
 }

int_set& int_set::diff(const int_set& b) 
{ word* stop = V+sz/SIZE_OF_ULONG +1;
  word* p;
  word* q;
  for(p = V, q = b.V; p<stop; p++, q++) *p &= ~(*q);
  return *this;
 }

int_set& int_set::symdiff(const int_set& b) 
{ word* stop = V+sz/SIZE_OF_ULONG +1;
  word* p;
  word* q;
  for(p = V, q = b.V; p<stop; p++, q++) *p ^= *q;
  return *this;
 }

int_set& int_set::complement() 
{ word* stop = V+sz/SIZE_OF_ULONG +1;
  for(word* p = V; p<stop; p++) *p = ~(*p);
  return *this;
 }

int_set  int_set::operator|(const int_set& b) 
{ int_set res(*this); 
  return res.join(b); 
 }

int_set  int_set::operator&(const int_set& b) 
{ int_set res(*this); 
  return res.intersect(b); 
 }

int_set  int_set::operator-(const int_set& b) 
{ int_set res(*this); 
  return res.diff(b); 
 }

int_set  int_set::operator%(const int_set& b) 
{ int_set res(*this); 
  return res.symdiff(b); 
 }

int_set  int_set::operator~()   
{ int_set res(*this); 
  return res.complement(); 
 }

LEDA_END_NAMESPACE
