/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  memory_manager.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MEMORY_MANAGER_H
#define LEDA_MEMORY_MANAGER_H

#define STDMEM_TABLESIZE 256


LEDA_BEGIN_NAMESPACE

struct  memory_elem_type { 
 memory_elem_type* next; 
};

typedef memory_elem_type* memory_elem_ptr;


class __exportC memory_manager {

 friend class __exportC memory_management;
 friend class __exportC memory_manager_init;

 int              max_sz;
 char             name[64];
 unsigned long    id;

 unsigned long int         alloc_count;
 unsigned long int*        total_count;
 unsigned long int*        block_count;
 memory_elem_ptr* block_list;
 memory_manager*  next;

 unsigned long    save_count; // to allow nested save/restore
 unsigned long int         save_alloc_count;
 unsigned long int*        save_total_count;
 unsigned long int*        save_block_count;
 memory_elem_ptr* save_block_list;
 memory_manager*  save_next;
 memory_elem_ptr* save_free_list;

 void init(int, const char* = 0, unsigned long = 0, bool top_level = true);
 void destroy();

public:

 memory_elem_ptr* free_list; // needed in handle_types.h

 memory_manager() {}
 memory_manager(int sz, const char* label, unsigned long id=0); 
~memory_manager();

 unsigned long get_id() { return id; }
 void     set_id(int i) { id = i; }

 void* allocate_bytes(size_t bytes)
 { if (int(bytes) < max_sz) { 
     memory_elem_ptr* q = free_list+bytes;
     if (*q==0) allocate_block(bytes,-1);
     memory_elem_ptr p = *q;
     *q = p->next;
     return (void*)p; 
   }
   else return allocate_vector(bytes);
 }


 void deallocate_bytes(void* p, size_t bytes)
 { if (int(bytes) < max_sz) { 
     memory_elem_ptr* q = free_list+bytes;
     memory_elem_ptr(p)->next = *q;
     *q = memory_elem_ptr(p);
   }
   else deallocate_vector(p);
 }


 void  allocate_block(size_t,int);
/*
 void* allocate_bytes(size_t);
 void deallocate_bytes(void*, size_t);
*/

 void* allocate_vector(size_t);
 void* allocate_bytes_with_check(size_t);

 void deallocate_vector(void*);
 void deallocate_vector_with_delay(void*);
 void deallocate_bytes_with_check(void*,size_t);

 void* allocate_list(size_t,int);
 void deallocate_list(void*, void*, size_t);

 void* allocate_words(size_t w) 
 { return allocate_bytes(w*sizeof(void*)); }

 void  deallocate_words(void* p,size_t w) 
 { deallocate_bytes(p,w*sizeof(void*)); }

 void  test_limit();
 
 void clear();
 void save();
 void restore();
 void kill();
 void reset();

 void push() { save(); }
 void pop()  { restore(); }

 void statistics_for_blocksize(int blk_sz, long& used_chks, long& free_chks, 
                                           long& blk_cnt, long& bytes);
 void print_statistics();
 void print_used_memory();
 void check_free_list(int bytes);
 int  max_size() { return max_sz; }


 unsigned long used_memory();
 unsigned long total_memory();
};


LEDA_END_NAMESPACE

#endif
