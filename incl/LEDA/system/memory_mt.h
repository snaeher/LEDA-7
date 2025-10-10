/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  memory_mt.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MEMORY_MT_H
#define LEDA_MEMORY_MT_H

#include <LEDA/system/memory_manager.h>

// we use thread local storage (TLS) to store for every thread 
// its own memory manager pointer (thread_memory_ptr)

// define macro
// THREAD_LOCAL_VAR  if thread_local variables are supported (c++11) and
// THREAD_LOCAL_SLOT otherwise (for using TLS API-functions)

#if defined(__APPLE__) 

#define THREAD_LOCAL_SLOT

#elif defined(_MSC_VER) 

#define thread_local __declspec(thread)

#if defined(LEDA_DLL)
#define THREAD_LOCAL_SLOT
#else
#define THREAD_LOCAL_VAR
#endif

#elif __cplusplus >= 201103L

// std >= 11

#define THREAD_LOCAL_VAR

#else

// std < c++11

#define THREAD_LOCAL_SLOT

#endif



LEDA_BEGIN_NAMESPACE


class __exportC memory_management {

  friend class __exportC memory_management_init;
  friend class __exportC thread_memory_init;


#if defined(THREAD_LOCAL_VAR)
static thread_local memory_manager* thread_memory_ptr;
#endif

#if defined(THREAD_LOCAL_SLOT)
static unsigned long thread_memory_slot;
#endif


private:

  int size;    // maximal number of managers   (default: see init)
  int tbl_sz;  // table size of managers  (default: see init) 

  memory_manager** mgrs;
  memory_manager** last;

  void init(int n=256, int sz=256);
  void destroy();

public:

  memory_management();
  memory_management(int n, int sz);
 ~memory_management();

  int max_size()    { return tbl_sz; }


  void kill();
  void clear();
  void print_statistics();
  int  used_memory();

  memory_manager* add_manager(unsigned long id);


#if defined(THREAD_LOCAL_VAR)

  // using tls variable (thread_memory_ptr)

  memory_manager* get_manager() { return thread_memory_ptr; }

  memory_manager* acquire()
  { // using tls variable (thread_memory_ptr)
    if (thread_memory_ptr == 0) 
      thread_memory_ptr = add_manager(get_thread_id());
    return thread_memory_ptr;
  }

#elif defined(THREAD_LOCAL_SLOT)

  // using tls slot (thread_memory_slot)

  memory_manager* get_manager() { 
    return (memory_manager*)tls_get_value(thread_memory_slot);
  }

  memory_manager* acquire()
  { memory_manager* ptr = (memory_manager*)tls_get_value(thread_memory_slot);
    if (ptr == 0) 
    { ptr = add_manager(get_thread_id());
      tls_set_value(thread_memory_slot,ptr);
     }
    return ptr;
  }


#else

  // linear search for thread id

  memory_manager* get_manager() { 
  { unsigned long id = get_thread_id();
    memory_manager** p = last;
    while (p >= mgrs && (*p)->id != id) p--;
    if (p >= mgrs)
      return *p;
    else
      return 0;
  }


  memory_manager* acquire()
  { unsigned long id = get_thread_id();
    memory_manager** p = last;
    while (p >= mgrs && (*p)->id != id) p--;
    if (p >= mgrs)
      return *p;
    else
      return add_manager(id);
  }

#endif


  void release_manager() { 
    memory_manager* mgr = get_manager();
    if (mgr) {
      mgr->clear();
      mgr->id = 0; 
    }
  }

  void* allocate_bytes(size_t bytes)
  { memory_manager* mgr = acquire();
    return mgr->allocate_bytes(bytes);
   }
    
  void deallocate_bytes(void* p, size_t bytes)
  { memory_manager* mgr = acquire();
    mgr->deallocate_bytes(p, bytes);
   }


  void allocate_block(size_t,int);

  void* allocate_words(size_t);
  void* allocate_bytes_with_check(size_t);
  void* allocate_vector(size_t);
  
  void deallocate_words(void*,size_t);
  void deallocate_bytes_with_check(void*,size_t);
  void deallocate_vector(void*);
  void deallocate_list(void*, void*, size_t);
};



class __exportC memory_management_init
{
  static unsigned count;

public:
  memory_management_init();
 ~memory_management_init();

};



extern memory_management __exportD mt_memory_mgr;

static memory_management_init mt_memory_mgr_init;



// LEDA_THREAD_BEGIN


class thread_memory_init {

  // avoid a warning of msvc 2008
  thread_memory_init(const thread_memory_init&);
  thread_memory_init& operator=(const thread_memory_init&);

public:

  thread_memory_init() {}

 ~thread_memory_init() { 
    // mark memory manager for reuse (see add_manager)
    memory_manager* mgr = mt_memory_mgr.acquire();
    mgr->set_id(0);
  }

};



LEDA_END_NAMESPACE



#endif
