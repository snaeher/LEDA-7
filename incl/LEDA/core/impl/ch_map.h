/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  ch_map.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_CH_MAP_H
#define LEDA_CH_MAP_H

#include <LEDA/system/basic.h>
#include <assert.h>

LEDA_BEGIN_NAMESPACE
 

class __exportC ch_map_elem 
{
  friend class __exportC ch_map;

  size_t    k;
  GenPtr           i;
  ch_map_elem*  succ;
};

typedef ch_map_elem*  ch_map_item;


class __exportC ch_map   
{
   const size_t NULLKEY; 
   const size_t NONNULLKEY;

   
   ch_map_elem STOP;

   ch_map_elem* table;
   ch_map_elem* table_end;
   ch_map_elem* free;
   int table_size;           
   int table_size_1;  

   ch_map_elem* old_table;
   ch_map_elem* old_table_end;
   ch_map_elem* old_free;
   int old_table_size;           
   int old_table_size_1;  

   size_t old_index;


            
   virtual void clear_inf(GenPtr&)  const { }
   virtual void copy_inf(GenPtr&)   const { }
   virtual void init_inf(GenPtr&)   const { }

   
   ch_map_elem*  HASH(size_t x)  const
   { return table + (x & table_size_1);  }


   
   void init_table(int T);


   void rehash();
   void del_old_table();


   inline void insert(size_t x, GenPtr y);


   GenPtr& access(ch_map_item p, size_t x);


   protected:

   typedef ch_map_item item;

   
   size_t index(ch_map_item it) const { return it->k; }
   GenPtr&       inf(ch_map_item it) const { return it->i; }


   ch_map(int n = 1); 
   ch_map(const ch_map& D);
   ch_map& operator=(const ch_map& D);


   void clear_entries();
   void clear();
   virtual ~ch_map();


   GenPtr& access(size_t x);
   ch_map_item lookup(size_t x) const;


   ch_map_item first_item() const;
   ch_map_item next_item(ch_map_item it) const; 


   void statistics() const;


};


inline GenPtr& ch_map::access(size_t x)
{ ch_map_item p = HASH(x);


  if (old_table) del_old_table();

  assert(old_table == 0);

  if ( p->k == x ) 
   { old_index = x; 
     return p->i;
    }
  else
  { if ( p->k == NULLKEY )
    { p->k = x;
      init_inf(p->i);  // initializes p->i to xdef
      old_index = x;
      return p->i;
    }
    else 
      return access(p,x);
  }
}


LEDA_END_NAMESPACE

#endif

