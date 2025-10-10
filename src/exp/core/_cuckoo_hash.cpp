/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _cuckoo_hash.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/impl/cuckoo_hash.h>

LEDA_BEGIN_NAMESPACE

void cuckoo_hash::init(int sz)
{
  count      = 0;
  table_size = sz;
  min_size   = (2 * table_size) / 5;

  mean_size  = 5 * (2 * table_size) / 12;

  max_chain  = 4 + (int)(4 * log((float)table_size) / log(2.0) + 0.5);
 
  a1[3] = 32 - (int)(log((float)table_size) / log(2.0) + 0.5 );
  a2[3] = a1[3];    /* shift */
  
  T1 = new cuckoo_hash_elem[sz];
  
  if (!T1)
    LEDA_EXCEPTION(1,"cuckoo_hash::init: error while allocating mem for T1");
  
  cuckoo_hash_item x;
  
  int i;
  for(i = 0; i < sz; i++)
  {
    x = &T1[i];    
	x->used = false;
//    x->key = 0;
//    x->inf = 0;
  }
  
  
  T2 = new cuckoo_hash_elem[sz];
  
  if (!T2)
    LEDA_EXCEPTION(1,"cuckoo_hash::init: error while allocating mem for T2");

  for(i = 0; i < sz; i++)
  {
    x = &T2[i];    
	x->used = false;
//    x->key = 0;
//    x->inf = 0;
  }

  init_hashfunction(a1);
  init_hashfunction(a2);
}



void cuckoo_hash::rehash(int sz)
{
  cuckoo_hash_elem* T1_old = T1;
  cuckoo_hash_elem* T2_old = T2;
  
  int tbl_sz_old = table_size;
  
  int sav_count = count;
  init(sz);
  count = sav_count;
  
  // reinsert all used entries into the new tables
  int i;
  for (i = 0; i < tbl_sz_old; i++)
  {
    if (T1_old[i].used && !rehash_insert(T1_old[i]))
    { 
      i = -1;
      continue;
    }
    
    if (T2_old[i].used && !rehash_insert(T2_old[i]))
      i = -1;
  }
  
  /* destroy old tables */
  delete [] T1_old;
  delete [] T2_old;
}


void cuckoo_hash::destroy()
{
  int i;
  for (i = 0; i < table_size; i++)
  {
	if (T1[i].used) {
      clear_key(T1[i].key);
      clear_inf(T1[i].inf);
	}
  }
  
  for (i = 0; i < table_size; i++)
  {  
	if (T2[i].used) {
      clear_key(T2[i].key);
      clear_inf(T2[i].inf);
	}
  }
  
  count = 0;
  
  delete [] T1;
  delete [] T2;
}


/* --- public members ------------------------------------------ */


void cuckoo_hash::change_inf(cuckoo_hash_item it, GenPtr x)
{
  clear_inf(it->inf);
  it->inf = x;
  copy_inf(it->inf);
}


cuckoo_hash_item cuckoo_hash::lookup(GenPtr k) const
{
  int key  = hash_fct(k);
  
  ulong hkey;  
  hash_cuckoo(hkey,a1,key);
  if (T1[hkey].used && equal_key(T1[hkey].key, k)) return &T1[hkey];
    
  hash_cuckoo(hkey,a2,key);
  if (T2[hkey].used && equal_key(T2[hkey].key, k)) return &T2[hkey];
  
  return 0;
}


cuckoo_hash_item cuckoo_hash::insert(GenPtr k, GenPtr i)
{
  cuckoo_hash_item it = lookup(k);

  /* if element already stored then change its info and return it */

  if (it != nil) {
	change_inf(it, i);
	return it;
  }

  /* else insert a new element */
  
  copy_key(k); copy_inf(i);
  insert_new_element(k, i);
  return lookup(k);
}

void cuckoo_hash::insert_new_element(GenPtr k, GenPtr i)
{
  cuckoo_hash_elem x, tmp;
  
  x.used = true;
  x.key = k;
  x.inf = i;
    
  for (;;) {
    for(int j = 0; j < max_chain; j++)
    {
      ulong hkey;
      hash_cuckoo(hkey,a1,hash_fct(x.key));
      tmp = T1[hkey];
    
      T1[hkey] = x;
    
      if (!tmp.used)
      {
        count++;
        if (table_size < count) rehash(2 * table_size);
		return;
      }
    
      x = tmp;
    
      hash_cuckoo(hkey,a2,hash_fct(x.key));
    
      tmp = T2[hkey];
        
      T2[hkey] = x;
    
      if (!tmp.used)
      {
        count++;
        if (table_size < count) rehash(2 * table_size);
	    return;
      }
    
      x = tmp;
    }
 
    /* forced rehash */
  
    if (count < mean_size)
      rehash(table_size);
    else
      rehash(2 * table_size);
  }
}


void cuckoo_hash::del_item(cuckoo_hash_item it)
{
  if (it) {
	it->used = false;
//    it->key = 0; 
//    it->inf = 0;
    clear_key(it->key);
    clear_inf(it->inf);        
    count--;    
    if (count < min_size && table_size > 4) rehash(table_size / 2);
  }
}


void cuckoo_hash::assign(const cuckoo_hash& D)
{
  init(D.table_size);
  
  int i;
  for (i = 0; i < table_size; i++)
  {
	if (D.T1[i].used) {
      T1[i] = D.T1[i];    
      copy_key(T1[i].key);
      copy_inf(T1[i].inf);
	}
	else T1[i].used = false;

	if (D.T2[i].used) {
      T2[i] = D.T2[i];    
      copy_key(T2[i].key);
      copy_inf(T2[i].inf);
	}
	else T2[i].used = false;
  }
  count = D.count;

  for (i = 0; i < 3; i++)
  {
    a1[i] = D.a1[i];
    a2[i] = D.a2[i];
  }
}


LEDA_END_NAMESPACE
