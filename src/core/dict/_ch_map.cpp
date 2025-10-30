/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _ch_map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/impl/ch_map.h>

LEDA_BEGIN_NAMESPACE

ch_map::~ch_map() 
{ if (old_table) delete[] old_table;
  //cout << "delete table: table = " << table << endl;
  if (table) delete[] table; 
}  


void ch_map::init_table(int T)
{ 
  table_size = T;
  table_size_1 = T-1;
  table = new ch_map_elem[T + T/2];
  free = table + T;
  table_end = table + T + T/2;      

  //cout << "init_table: T = " <<  T << "  table = " << table << endl;

  for (ch_map_item p = table; p < free; p++) 
  { p->succ = &STOP; 
    p->k = NULLKEY;
  }
  table->k = NONNULLKEY;
}


inline void ch_map::insert(size_t x, GenPtr y)
{ ch_map_item q = HASH(x);                                    
  if ( q->k == NULLKEY )        
    { q->k = x;                                                  
      q->i = y; 
    }                                                
  else                                                           
   { free->k = x;                                                
     free->i = y;                                                
     free->succ = q->succ;                                       
     q->succ = free++; 
   }                                         
}

                                                                            
void ch_map::rehash()
{ 
  old_table = table;
  old_table_end = table_end;
  old_table_size = table_size;
  old_table_size_1 = table_size_1;
  old_free = free;

  ch_map_item old_table_mid = table + table_size;


  init_table(2*table_size);

  ch_map_item p;

  for(p = old_table + 1; p < old_table_mid; p++)
  { size_t x = p->k;
    if ( x != NULLKEY ) // list p is non-empty
    { ch_map_item q = HASH(x);  
      q->k = x;
      q->i = p->i;
    }
  }

  while (p < old_table_end)
  { size_t x = p->k;
    insert(x,p->i);
    p++;
  }

}


void ch_map::del_old_table()
{
  ch_map_item save_table = table;
  ch_map_item save_table_end = table_end;
  ch_map_item save_free = free;
  int save_table_size = table_size;
  int save_table_size_1 = table_size_1;

  table = old_table;
  table_end = old_table_end;
  table_size = old_table_size;
  table_size_1 = old_table_size_1;
  free = old_free;

  old_table = 0;

  GenPtr p = access(old_index);

  delete[] table;

  table = save_table;
  table_end = save_table_end;
  table_size = save_table_size;
  table_size_1 = save_table_size_1;
  free = save_free;

  access(old_index) = p;
}




GenPtr& ch_map::access(ch_map_item p, size_t x)
{
  STOP.k = x;
  ch_map_item q = p->succ; 
  while (q->k != x) q = q->succ;
  if (q != &STOP) 
  { old_index = x;
    return q->i;
   }

  // index x not present, insert it

  if (free == table_end)   // table full: rehash
  { rehash();
    p = HASH(x);
  }

  if (p->k == NULLKEY)
  { p->k = x;
    init_inf(p->i);  // initializes p->i to xdef
    return p->i;
  }

  q = free++;
  q->k = x;
  init_inf(q->i);    // initializes q->i to xdef
  q->succ = p->succ;
  p->succ = q;
  return q->i;
}


ch_map::ch_map(int n) : NULLKEY(0), NONNULLKEY(1), old_table(0), old_index(0)
{ 
  if (n < 512)
     init_table(512); 
  else
   { int ts = 1;
     while (ts < n) ts <<= 1;
     init_table(ts);
    }
}


ch_map::ch_map(const ch_map& D) : NULLKEY(0), NONNULLKEY(1), old_table(0), old_index(0)
{ 
  init_table(D.table_size);

  for(ch_map_item p = D.table + 1; p < D.free; p++) 
  { if (p->k != NULLKEY || p >= D.table + D.table_size)
    { insert(p->k,p->i);
      D.copy_inf(p->i);  // see chapter Implementation
     }
   }
}

ch_map& ch_map::operator=(const ch_map& D)
{ 
  clear_entries();
  delete[] table;
  init_table(D.table_size);

  for(ch_map_item p = D.table + 1; p < D.free; p++) 
  { if (p->k != NULLKEY || p >= D.table + D.table_size)
    { insert(p->k,p->i);
      copy_inf(p->i);    // see chapter Implementation
     }
   }
  return *this;
}


void ch_map::clear_entries() 
{ for(ch_map_item p = table + 1; p < free; p++)
    if (p->k != NULLKEY || p >= table + table_size) 
      clear_inf(p->i);  // see chapter Implementation
 }


void ch_map::clear() 
{ clear_entries();
  delete[] table;
  init_table(512); 
}



ch_map_item ch_map::lookup(size_t x) const 
{ ch_map_item p = HASH(x);
  ((size_t &)STOP.k) = x;  // cast away const
  while (p->k != x) 
  { p = p->succ;
#if defined(LEDA_MULTI_THREAD)
    if (p == &STOP) break;
#endif
   }
  return (p == &STOP) ? nil : p;
}


ch_map_item ch_map::first_item() const
{ return next_item(table); }

ch_map_item ch_map::next_item(ch_map_item it) const 
{ if (it == nil) return nil;
  do it++; while (it < table + table_size && it->k == NULLKEY);
  return (it < free ? it : nil);
}


void ch_map::statistics() const
{ cout << "table_size: " << table_size <<"\n";
  int n = 0;
  for (ch_map_item p = table + 1; p < table + table_size; p++)
     if (p ->k != NULLKEY) n++;
  int used_in_overflow = int(free - (table + table_size));
  n += used_in_overflow;
  cout << "number of entries: " << n << "\n";
  cout << "fraction of entries in first position: " << ((double) (n -
used_in_overflow))/n <<"\n";
  cout << "fraction of empty lists: " << ((double) (n -
used_in_overflow))/table_size<<"\n";

}

LEDA_END_NAMESPACE
