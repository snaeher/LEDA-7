/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ch_map2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/ch_map2.h>

//------------------------------------------------------------------------------
//
//  Two-dimensional hashing array with chaining and table doubling
//
//  S. Naeher (1997)
//
//------------------------------------------------------------------------------

#ifdef NULLKEY
#undef NULLKEY
#endif

#define NULLKEY 0xFFFFFFFF

LEDA_BEGIN_NAMESPACE

ch_map2::ch_map2(int sz, int n) 
{ 
  shift = 0;
  while (sz >>= 1) shift++;

  if (n < 512)
     init_table(512); 
  else
   { int ts = 1;
     while (ts < n) ts <<= 1;
     init_table(ts);
    }
}


ch_map2_item ch_map2::lookup(ch_map2_item p, size_t x, 
                                             size_t y) const 
{ ((size_t&)STOP.k1) = x;
  ((size_t&)STOP.k2) = y;
  while (p->k1 != x || p->k2 != y) p = p->succ;
  return (p == &STOP) ? nil : p;
}


GenPtr& ch_map2::access(ch_map2_item p, size_t x, size_t y)
{ 
  STOP.k1 = x;
  STOP.k2 = y;
  ch_map2_item q = p->succ; 
  while (q->k1 != x || q->k2 != y) q = q->succ;
  if (q != &STOP) return q->i;


  // index x not present, insert it

  if (free == table_end)   // table full: rehash
  { rehash();
    p = HASH(x,y);
   }

  if (p->k1 == NULLKEY && p->k2 == NULLKEY)
  { p->k1 = x;
    p->k2 = y;
    init_inf(p->i);
    return p->i;
   }

  q = free++;
  q->k1 = x;
  q->k2 = y;
  init_inf(q->i);
  q->succ = p->succ;
  p->succ = q;
  return q->i;
}




void ch_map2::init_table(int T)
{ 
  table_size = T;
  table_size_1 = T-1;
/*
cout << "2*T = " << 2*T << "  bytes= " << 2*T*sizeof(ch_map2_elem) << endl; 
*/
  table = new ch_map2_elem[2*T];
  free = table + T;
  table_end = free + T/2;

  for (ch_map2_item p=table; p < table_end; p++) 
  { p->k1 = NULLKEY;
    p->k2 = NULLKEY;
    p->succ = &STOP;
   }
}


#ifdef INSERT
#undef INSERT
#endif

#define INSERT(x,y,z)                                                     \
{ ch_map2_item q = HASH(x,y);                                             \
  if (q->k1 == NULLKEY && q->k2 == NULLKEY)                               \
    { q->k1 = x;                                                          \
      q->k2 = y;                                                          \
      q->i  = z; }                                                        \
  else                                                                    \
   { free->k1 = x;                                                        \
     free->k2 = y;                                                        \
     free->i  = z;                                                        \
     free->succ = q->succ;                                                \
     q->succ = free++; }                                                  }


void ch_map2::rehash()
{ 
  ch_map2_item old_table = table;
  ch_map2_item old_table_mid = table+table_size;
  ch_map2_item old_table_end = table_end;
  
  //init_table(4*table_size);
  init_table(2*table_size);

  ch_map2_item p;
  for(p = old_table; p < old_table_mid; p++)
  { size_t x = p->k1;
    size_t y = p->k2;
    if (x != NULLKEY || y != NULLKEY)
    { ch_map2_item q = HASH(x,y);
      q->k1 = x;
      q->k2 = y;
      q->i = p->i;
     }
   }

  while (p < old_table_end)
  { size_t x = p->k1;
    size_t y = p->k2;
    INSERT(x,y,p->i);
    p++;
   }

  delete[] old_table;
}


ch_map2_item ch_map2::first_item() const
{ return next_item(table); }

ch_map2_item ch_map2::next_item(ch_map2_item it) const 
{ if (it == nil) return nil;
  do it++; while (it < table + table_size && it->k1 == 0xffffffff 
                                          && it->k2 == 0xffffffff);
  return (it < free ? it : nil);
}



ch_map2::ch_map2(const ch_map2& D)
{ 
  init_table(D.table_size);

  for(ch_map2_item p = D.table; p < D.table_end; p++) 
  { if (p->k1 != NULLKEY || p->k2 != NULLKEY)
    { INSERT(p->k1,p->k2,p->i);
      D.copy_inf(p->i);
     }
   }
}


void ch_map2::clear_entries() 
{ for(ch_map2_item p = table; p < free; p++)
    if (p->k1 != NULLKEY || p->k2 != NULLKEY) clear_inf(p->i);
 }


void ch_map2::clear() 
{ clear_entries();
  delete[] table;
  init_table(512); 
 }


ch_map2& ch_map2::operator=(const ch_map2& D)
{ 
  clear_entries();
  delete[] table;

  init_table(D.table_size);

  for(ch_map2_item p = D.table; p < D.table_end; p++) 
  { if (p->k1 != NULLKEY || p->k2 != NULLKEY)
    { INSERT(p->k1,p->k2,p->i);
      copy_inf(p->i);
     }
   }
  return *this;
}

LEDA_END_NAMESPACE
