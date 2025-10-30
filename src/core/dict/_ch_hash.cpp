/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _ch_hash.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/ch_hash.h>
#include <assert.h>

LEDA_BEGIN_NAMESPACE

void ch_hash::init(int T)
{ 
  ch_hash_item p;
  ch_hash_item stop;

  table_size = T;
  table_size_1 = T-1;

  low_table  = (T > 1024) ? T/2 : -1;
  high_table = 2*T;

  table = new ch_hash_elem[table_size];

  stop = table + table_size;
  for(p = table; p < stop; p++) p->succ = nil;

  count = 0;
}


void ch_hash::rehash(int T)
{ 
  ch_hash_item old_table = table;
  int old_table_size = table_size;
  int old_count = count;

  init(T); 
  count = old_count;   // since init overwrites count

  ch_hash_item stop = old_table + old_table_size;
  for (ch_hash_item it = old_table; it < stop; it++) 
  { 
    ch_hash_item old_r = it->succ;
    if (old_r == nil) continue;
    ch_hash_item old_p = it;
    do
    { ch_hash_item p = table_entry_for(old_p->k);
      //  table + (hash_fct(old_p->k) & table_size_1);
      if (p->succ == nil)
        { p->k = old_p->k;
          p->i = old_p->i;
          p->succ = &STOP;
        }
      else
        { p->succ = new ch_hash_elem(old_p->k,old_p->i,p->succ); }
      if (old_p != it)
        { delete old_p; }
      old_p = old_r;
      old_r = old_p->succ;
    } while (old_p != &STOP);
   }

  delete[] old_table;
}


void ch_hash::destroy()
{ 
  for(int i = 0; i < table_size; i++) 
  { ch_hash_item p = table + i;
    if ( p->succ == nil ) continue;
    do
    { clear_key(p->k);
      clear_inf(p->i);
      ch_hash_item q = p;
      p = p->succ;
      if ( q != table + i ) delete q;
    } while ( p != &STOP);
  }

  delete[] table;
}


ch_hash::ch_hash(const ch_hash& D)
{ ch_hash_item p;
  init(D.table_size);
  for (int i = 0; i < table_size; i++)
  { p = &D.table[i];
    if (p->succ == nil) continue;
    while(p != &(D.STOP))
    { GenPtr k = p->k;
      GenPtr i = p->i;
      D.copy_key(k);
      D.copy_inf(i);  
      ch_hash_item q = table + (D.hash_fct(k) & table_size_1);
      insert_at_pos(q,k,i);
      p = p->succ;
    }
  }
}

ch_hash& ch_hash::operator=(const ch_hash& D)
{ destroy();
  init(D.table_size);
  for (int i=0; i<D.table_size; i++)
  { ch_hash_item p = &D.table[i];
    if (p->succ == nil) continue;
    while(p != &(D.STOP))
    { fast_insert(p->k,p->i);
      p = p->succ;
     }
   }
  return *this;
}



ch_hash_item ch_hash::lookup(GenPtr x) const
{ 
  ch_hash_item q = table_entry_for(x);

  if ( q->succ == nil ) return nil;

  ((GenPtr&)STOP.k) = x;

/*
  if (key_def_order() &&  key_type_id() == INT_TYPE_ID )
  {
     while (!LEDA_EQUAL(int,q->k,x)) 
     { q = q->succ;
#if defined(LEDA_MULTI_THREAD)
       if (q == &STOP) break;
#endif
      }
   }
  else
*/
  {
     while (!equal_key(q->k,x))
     { 
       q = q->succ;
#if defined(LEDA_MULTI_THREAD)
       if (q == &STOP) break;
#endif
      }
   }

  return (q == &STOP) ? nil : q;
}


ch_hash_item ch_hash::insert(GenPtr x, GenPtr y)
{ 
  ch_hash_item p = table_entry_for(x);
  ch_hash_item q = p;

  if (p->succ != nil)
  { STOP.k = x;

/*
    if (key_def_order() && key_type_id() == INT_TYPE_ID)
       while (p->k != x) p = p->succ;
    else
*/
       while (!equal_key(p->k,x)) p = p->succ;

    if (p != &STOP)
    { clear_inf(p->i);
      p->i = y;
      copy_inf(p->i);
      return p;
    }
  }

/*
int i = LEDA_CONST_ACCESS(int,x);
cout << "x = " << x << endl;
cout << "i = " << i << endl;
cout << "hash = " << Hash(i) << endl;
cout << "hash = " << LEDA_HASH(int,x) << endl;
cout << "hash = " << hash_fct(x) << endl;
*/

  // rehash before inserting the new key

  if (++count == high_table) 
  { rehash(high_table);
    q = table_entry_for(x);
  }

  copy_key(x);
  copy_inf(y);

// cout << "x = " << x << endl;

  if (q->succ == nil) 
    { q->k = x;
      q->i = y;
      q->succ = &STOP;
     }
  else
    { q->succ = new ch_hash_elem(x,y,q->succ);
      q = q->succ;
     }

  return q;
}


ch_hash_item ch_hash::insert_at_pos(ch_hash_item q, GenPtr x, GenPtr y)
{ if (q->succ == nil) 
    { q->k = x;
      q->i = y;
      q->succ = &STOP;
     }
  else
    { q->succ = new ch_hash_elem(x,y,q->succ);
      q = q->succ;
     }
  return q;
}



ch_hash_item ch_hash::fast_insert(GenPtr x, GenPtr y)
{ 
  ch_hash_item p = table_entry_for(x);

  if (++count == high_table) 
  { rehash(high_table);
    p = table_entry_for(x);
  }

  copy_key(x);
  copy_inf(y);

  return insert_at_pos(p,x,y);
}


void ch_hash::del(GenPtr x)
{ ch_hash_item p = lookup(x);
  if ( p != nil) del_item(p);
 }

  

void ch_hash::del_item(ch_hash_item q)
{ ch_hash_item p = table_entry_for(q->k);

  clear_key(q->k);
  clear_inf(q->i);

  if (p == q) // q is first item in its list
    { if ( q->succ == &STOP ) q->succ = nil;
      else 
      { p       = q->succ;
        q->k    = p->k;
        q->i    = p->i;
        q->succ = p->succ;
        delete p;
      }
    }
  else // q is not the first item
    { while( p->succ != q ) p = p->succ;
      p->succ = q->succ;
      delete q;
     }
  count--;
  if (count == low_table) rehash(low_table);
 }
  

void ch_hash::change_inf(ch_hash_item p, GenPtr x)
{ clear_inf(p->i);
  p->i = x;
  copy_inf(p->i);
 }  


ch_hash_item ch_hash::first_item() const 
{ ch_hash_item q = table;
  ch_hash_item stop = table + table_size;
  while (q < stop && q->succ == nil) q++;
  return (q < stop) ? q : 0;
 }


// changed by s.n.  (nested forall_defined bug)

ch_hash_item ch_hash::next_item(ch_hash_item q) const 
{ if (q == 0) return 0;
  if ( q->succ != nil && q->succ != &STOP) return q->succ;
  ch_hash_item it = table_entry_for(q->k)+1;
  ch_hash_item stop = table + table_size;
  while (it < stop && it->succ == 0) it++;
  if (it == stop) it = 0;
  return it;
}


/*
ch_hash_item ch_hash::next_item(ch_hash_item q) const 
{ if (q == 0) return 0;
  if ( q->succ != nil && q->succ != &STOP) return q->succ;
  ch_hash_item stop = table + table_size;
  while (iterator < stop)
  { (*(ch_hash_item*)&iterator)++;
    if (iterator == stop) return nil;
    if (iterator->succ == nil) continue; 
    return iterator;
   }
  return 0;
}
*/

LEDA_END_NAMESPACE

