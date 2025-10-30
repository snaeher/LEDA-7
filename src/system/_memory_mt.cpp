/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _memory_mt.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// Memory Management for Multi-Thread  Applications
//
// C. Uhrig (1996)
//
// modified 12/1999 (sn)
//
// redesign 09/2008 (sn)
//
// modified 05/2020 (sn)
//
//------------------------------------------------------------------------------


#include <LEDA/system/basic.h>
#include <LEDA/system/memory_mt.h>
#include <LEDA/system/assert.h>

#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

memory_management mt_memory_mgr;

unsigned  memory_management_init::count = 0;


#if defined(THREAD_LOCAL_VAR)
thread_local memory_manager* memory_management::thread_memory_ptr = 0;
#endif

#if defined(THREAD_LOCAL_SLOT)
unsigned long memory_management::thread_memory_slot = 0;
#endif



memory_management_init::memory_management_init()
{ //cout << "memory_management_init: count = " << count << endl;
  if (count++ == 0) mt_memory_mgr.init(); }

memory_management_init::~memory_management_init()
{ //cout << "~memory_management_init: count = " << count << endl;
  if (--count == 0) {
     //mt_memory_mgr.kill(); 
     mt_memory_mgr.destroy(); 
  }
}



void memory_management::init(int n, int sz)
{
#if defined(THREAD_LOCAL_SLOT)
  thread_memory_slot = tls_alloc();
  //cout << "tls_alloc: " << thread_memory_slot << endl;
#endif

  size = n;
  tbl_sz =sz;
  mgrs=(memory_manager**)malloc(n*sizeof(void*));
  for(int i = 0; i < n; i++) mgrs[i] = NULL;
  last = mgrs-1;
 }

static leda_lock mtx;

memory_manager* memory_management::add_manager(unsigned long id)
{ 
  memory_manager* p = 0;

  mtx.lock();

  // try to reuse a manager with id == 0
  memory_manager** q = mgrs;
  while(q <= last && (*q)->id != 0) q++; 

  if (q <= last) 
   { // reuse *q
     p = *q;
     //cout << "REUSE MEMORY MANAGER: " << p << "  id = " << p->id <<endl;
     p->id = id;
    }
  else
  {
   if (last == mgrs+size-1) 
   { // double the size of the table of managers whenever this is required
     int n = 2*size;
     memory_manager** mgrs_new = (memory_manager**)malloc(n*sizeof(void*));
     for(int i = 0; i < n; i++) mgrs_new[i] = (i < size) ? mgrs[i] : NULL;
     free(mgrs); // segmentation fault ?
     mgrs = mgrs_new;
     last = mgrs + size - 1;
     size = n;
   }
 
   p = new memory_manager(tbl_sz,"MT_MEMORY_MGR",id); 
   *(++last) = p;
  }
 
  mtx.unlock();
  return p;

 }




memory_management::memory_management() {}

memory_management::memory_management(int n, int sz) 
{ if (this != &mt_memory_mgr) init(n,sz); }

memory_management::~memory_management()
{ if (this != &mt_memory_mgr) destroy(); }


void* memory_management::allocate_vector(size_t bytes)
{ memory_manager* mgr = acquire();
  return mgr->allocate_vector(bytes);
}
  
void memory_management::deallocate_vector(void* p)
{ memory_manager* mgr = acquire();
  mgr->deallocate_vector(p);
}


void memory_management::deallocate_list(void* head,void* tail, size_t bytes)
{ memory_manager* mgr = acquire();
  mgr->deallocate_list(head, tail, bytes);
}


void memory_management::allocate_block(size_t b, int N)
{ memory_manager* mgr = acquire();
  mgr->allocate_block(b, N);
}



void memory_management::clear()
{ for(int i=0; i<size; i++)
    if (mgrs[i]) mgrs[i]->clear();
 }

void memory_management::destroy()
{ //cout << "memory_management::destroy" << endl;
#if defined(THREAD_LOCAL_SLOT)
  tls_free(thread_memory_slot);
#endif
  for(int i=0; i<size; i++) {
    if (mgrs[i]) mgrs[i]->destroy();
  }
}

void memory_management::kill()
{ //cout << "memory_management::kill" << endl;
  for(int i=0; i<size; i++)
    if (mgrs[i]) mgrs[i]->kill();
 }


void memory_management::print_statistics() 
{ float total_bytes = 0;
  for(int i=0; i<size; i++)
  { if (mgrs[i]) {
       mgrs[i]->print_statistics();
       total_bytes += mgrs[i]->used_memory();
    }
  }
  float mb = total_bytes/(1024*1024);
  fprintf(stderr,"memory still in use: %.2f MB\n\n",mb);
 }



void* memory_management::allocate_bytes_with_check(size_t bytes)
{ memory_manager *mgr = acquire();
  return mgr->allocate_bytes_with_check(bytes);
}

void memory_management::deallocate_bytes_with_check(void* p, size_t bytes)
{ memory_manager *mgr = acquire();
  mgr->deallocate_bytes_with_check(p, bytes);
}

void* memory_management::allocate_words(size_t words)
{ memory_manager *mgr = acquire();
  return mgr->allocate_words(words);
}

void memory_management::deallocate_words(void* p, size_t words)
{ memory_manager *mgr = acquire();
  mgr->deallocate_words(p, words);
}


int memory_management::used_memory()
{ memory_manager *mgr = acquire();
  return mgr->used_memory();
}

LEDA_END_NAMESPACE
