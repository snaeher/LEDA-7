/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _memory_manager.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// Memory Manager
//------------------------------------------------------------------------------


#include <LEDA/system/basic.h>
#include <LEDA/system/memory_manager.h>
#include <LEDA/system/assert.h>


#include <string.h>
#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

#if defined(LEDA_MULTI_THREAD)
static leda_lock mtx;
#endif


memory_manager::memory_manager(int sz, const char* name, unsigned long id) 
{ // do not call constructor for std_memory_mgr
  //if (this != &std_memory_mgr) init(sz,name,id);
  if (id != 0) init(sz,name,id);
}

memory_manager::~memory_manager() 
{ // do not call destructor for std_memory_mgr
  //if (this != &std_memory_mgr) destroy(); 
  if (id != 0) destroy();
}



//-----------------------------------------------------------------------------
// most malloc() implementations increase the size of the block to be allocated
// by one word to store additional information (e.g. the size of the block)
// and then round the size to the next power of two. So it seems to be a good
// idea to choose the block size to be a power of two minus one word.
//-----------------------------------------------------------------------------


const int block_bytes = (1<<13) - sizeof(void*);
//const int block_bytes = (1<<16) - sizeof(void*);


void memory_manager::init(int sz, const char* n, unsigned long i, bool top_level)
{ max_sz = sz-1;
  id = i;
  sprintf(name,"%s(0x%lx)",n,i);

  //cout << "memory_manager::init: " << name << " this = " << this << endl;

/*
  free_list   = new memory_elem_ptr[sz];
  block_list  = new memory_elem_ptr[sz];
  total_count = new long[sz];
  block_count = new long[sz];
*/
  free_list   = (memory_elem_ptr*)malloc(sz*sizeof(memory_elem_ptr));
  block_list  = (memory_elem_ptr*)malloc(sz*sizeof(memory_elem_ptr));
  total_count = (unsigned long*)malloc(sz*sizeof(unsigned long));
  block_count = (unsigned long*)malloc(sz*sizeof(unsigned long));
  alloc_count = 0;
  for(int i = 0; i < sz; i++)
  { free_list[i]   = 0;
    total_count[i] = 0;
    block_list[i]  = 0;
    block_count[i] = 0;
   }
  if (top_level) save_count = 0;
 }


void memory_manager::kill()
{ for (int i=1; i<=max_sz; i++)
  { while (block_list[i])
    { memory_elem_ptr p = block_list[i];
      block_list[i] = p->next;
      free((char*)p);
     }
    total_count[i] = 0;
    block_count[i] = 0;
    free_list[i] = 0;
   }
   alloc_count = 0;
}


void memory_manager::reset() {
  for (int i=1; i<=max_sz; i++) free_list[i] = 0;
}


void memory_manager::destroy()
{ 
  //cout << "memory_manager::destroy: " << name << " this = " << this << endl;

  if (free_list == 0) return;

  bool show_leak = false;
  bool show_stat = false;

  if (option_list) 
  { show_leak = strcmp(option_list,"show_memory_leaks") == 0;
    show_stat = strcmp(option_list,"show_memory_statistics") == 0;
   }

  if (show_leak || show_stat) 
  { 
#if defined(LEDA_MULTI_THREAD)
   mtx.lock();
#endif

   if (show_leak) 
   { //print_used_memory(); 
     unsigned long used_bytes = used_memory();
     if (used_bytes != 0) { 
       fprintf(stderr,"\n");
       fprintf(stderr,"%s  memory in use: %10ld bytes.\n",name,used_bytes);
     }
   }

   if (show_stat) print_statistics(); 

#if defined(LEDA_MULTI_THREAD)
   mtx.unlock();
#endif
  }

  clear();

/*
  delete[] free_list;
  delete[] block_list;
  delete[] total_count;
  delete[] block_count;
*/

  if (free_list[0]) free((char*)free_list[0]);

  free((char*)free_list);
  free((char*)block_list);
  free((char*)total_count);
  free((char*)block_count);
  free_list = 0;
  alloc_count = 0;

 }


void memory_manager::test_limit()
{ if (alloc_count >= (1<<20))
  { print_statistics();
    alloc_count = 0;
    LEDA_EXCEPTION(999,"\n\
LEDA DEMO VERSION: Use of memory limited to 1 MByte.\n\
Please contact support@algorithmic-solutions.com for information on\n\
how to obtain the full version.");
   }
}

 

void memory_manager::allocate_block(size_t b, int N)
{ 
//cout << "allocate block: b = " << b << "  N = " << N << endl;

  if (N == 0) return;

  if (int(b) > max_sz || b == 0) 
    LEDA_EXCEPTION(1,string("allocate_block: illegal size (%d bytes)",(unsigned long)b));


  int sz = (N < 0) ?  block_bytes : int(b * (N+1));


  //test_limit();

  alloc_count += sz;


  //allocate new block of size sz byte and slice it into chunks of size b bytes

  memory_elem_ptr p;
  memory_elem_ptr stop;

  int words = int(b + sizeof(void*) - 1)/sizeof(void*);

  int bytes = words * sizeof(void*);

  // compute number of chunks (the first one is used for chaining blocks)

  int num   = sz/bytes - 1;

  block_count[b]++;

  total_count[b] += num;

  if ((p=memory_elem_ptr(malloc(sz))) == 0 )
  { cerr << endl;
    print_statistics();
    LEDA_EXCEPTION(999,"memory_manager::allocate_block: out of memory");
   }


  //insert block into list of used blocks
  p->next = block_list[b];
  block_list[b] = p;
  p += words;

  memory_elem_ptr q = free_list[b];

  free_list[b] = p;

  stop = p + (num-1)*words;
  stop->next = q;

  while (p < stop) p = (p->next = p+words);

}



void memory_manager::clear()
{ 
  //cout << "memory_manager::clear: " << name << endl;

  bool freed_everything = true;

  for (int b=1; b<=max_sz; b++)
  { memory_elem_ptr p = free_list[b];
    long used = total_count[b];

    while (p) 
    { memory_elem_ptr q = p->next; 
      used--; 
      //p->next = p; // mark element as free (used below)
      p = q;
     }

    if (used > 0)
       { freed_everything = false;
         /* print the used elements:
         int num = block_bytes/b - 1;
         int w = b/sizeof(void*);
         for(memory_elem_ptr bp = block_list[b]; bp; bp = bp->next)
         { memory_elem_ptr p = bp + w;
           for(int i=0; i<num; i++)
           { if (p->next != p) // check if element is marked as free
		       cerr << "used: " << p << endl;
             p += w;
            }
          }
         */
        }
    else
       { // compute (a lower bound on) the amount of freed memory
         int words = (b + sizeof(void*) - 1)/sizeof(void*);
         int bytes = words * sizeof(void*);
         alloc_count -= (total_count[b] + block_count[b]) * bytes;

         while (block_list[b])
         { p = block_list[b];
           block_list[b] = p->next;
           block_count[b]--;
           free((char*)p);
          }
         free_list[b] = 0;
         total_count[b] = 0;

	 freed_everything &= block_count[b] == 0;
        }
   }

  if (freed_everything) alloc_count = 0;
}

void memory_manager::save()
{ if (++save_count != 1) return;
  save_alloc_count = alloc_count;
  save_total_count = total_count;
  save_block_count = block_count;
  save_block_list = block_list;
  save_next = next;
  save_free_list = free_list;
  init(max_sz+1,name,save_count,false);
}


void memory_manager::restore()
{ 
  if (save_count == 0 || --save_count != 0) return;
  destroy();
  alloc_count = save_alloc_count;
  total_count = save_total_count;
  block_count = save_block_count;
  block_list = save_block_list;
  next = save_next;
  free_list = save_free_list;
}



void* memory_manager::allocate_vector(size_t sz)
{ //cout << "allocate_vector:  sz = " << sz << endl;
  sz += 2*sizeof(unsigned long);

// if (memory_manager_init::count > 0)
  { //test_limit();
    alloc_count += int(sz); 
    total_count[0]+=int(sz);
    block_count[0]++; 
   }

  unsigned long* p = (unsigned long*)malloc(sz); 
  if (p == 0) 
     LEDA_EXCEPTION(999,"memory_manager::allocate_vector: out of memory");
  *p = int(sz);
  return p+2;
 }


void memory_manager::deallocate_vector(void* p)
{ //cout << "deallocate_vector:  p = " << p << endl;
  if (p == 0) return;
  unsigned long* q = ((unsigned long*)p)-2;

//if (memory_manager_init::count > 0)
  { unsigned long sz = *q;
    alloc_count -= sz; 
    total_count[0]-=sz; 
    block_count[0]--; 
   }

  free((char*)q); 
 }


void memory_manager::deallocate_vector_with_delay(void* p)
{ 
  unsigned long* q = ((unsigned long*)p)-2;

//if (memory_manager_init::count > 0)
  { unsigned long sz = *q;
    alloc_count -= sz; 
    total_count[0]-=sz; 
    //block_count[0]--; 
   }
  if (free_list[0]) free((char*)free_list[0]);
  free_list[0] = (memory_elem_ptr)q;
 }



void* memory_manager::allocate_list(size_t bytes, int len)
{ 
  memory_elem_ptr p = free_list[bytes];
  int i = len;
  while (i > 0 && p) { p = p->next; i--; }
  if (i > 0)
  { // p = free_list[bytes]; fixed, 12/4/12 cu
    allocate_block(bytes,i);
   }
  memory_elem_ptr q = free_list[bytes];
  free_list[bytes] = p;
  return (void*)q;
 }


void memory_manager::deallocate_list(void* head,void* tail, size_t bytes)
{ if (int(bytes) < max_sz)
    { memory_elem_ptr* q = free_list+bytes;
      memory_elem_ptr(tail)->next = *q;
      *q = memory_elem_ptr(head);
     }
  else
    { memory_elem_ptr p = memory_elem_ptr(head);
      while (p)
      { memory_elem_ptr q = p->next;
        deallocate_vector(p);
        p = q;
       }
     }
 }



void* memory_manager::allocate_bytes_with_check(size_t bytes)
{ if (int(bytes) < max_sz)
  { if (free_list[bytes] == 0) allocate_block(bytes,-1);
    memory_elem_ptr p = free_list[bytes];
    free_list[bytes] = p->next;
    return (void*)p;
   }
  else return allocate_vector(bytes);
}

void memory_manager::deallocate_bytes_with_check(void* p, size_t bytes)
{ if (int(bytes) < max_sz)
  { if (block_count[bytes] == 0) 
       LEDA_EXCEPTION(999,"no block allocated");
    memory_elem_ptr q = free_list[bytes];
    while(q && q != memory_elem_ptr(p)) q = q->next;
    if (q) LEDA_EXCEPTION(999,string("LEDA memory: pointer %d deleted twice",p));
    memory_elem_ptr(p)->next = free_list[bytes];
    free_list[bytes] = memory_elem_ptr(p);
   }
  else deallocate_vector(p);
 }

unsigned long memory_manager::total_memory()
{ unsigned long total_bytes=0;
  for (int i=1;i<=max_sz;i++) 
  { unsigned long total = total_count[i];
    total_bytes += total*i;
  }
  return total_bytes;
}



unsigned long memory_manager::used_memory()
{ 
  unsigned long total_bytes=0;
  unsigned long free_bytes=0;
  unsigned long used_bytes=0;

  for (int i=1;i<=max_sz;i++) 
  { unsigned long total = total_count[i];
    unsigned long free = 0;
    memory_elem_ptr p = free_list[i];
    while (p) { free++; p = p->next; }
    total_bytes += total*i;
    free_bytes  += free*i;
    used_bytes  = total_bytes - free_bytes;
  }

  return used_bytes;
 }


void memory_manager::check_free_list(int bytes)
{
  if (block_list[bytes] == 0) return;

  memory_elem_ptr p = free_list[bytes];
  while (p) 
  { int* ptr = (int*)p;
    ptr[1] = 0xffffffff;
    p = p->next; 
   }

  int words = bytes/4;

  int sz    = block_bytes;
  int num   = sz/bytes - 1;

  p = block_list[bytes] + words;

  memory_elem_ptr stop = p + (num-1)*words;

  while (p < stop) 
  { if (((int*)p)[1] != (int)0xffffffff)  
         // change for VS Studio 2015, 64 bit
		 // fprintf(stderr,"0x%lx\n",(unsigned long)p);
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
         fprintf(stderr,"0x%llx\n",(unsigned long long)p);
#else
		 fprintf(stderr,"0x%lx\n",(unsigned long)p);
#endif
         //fprintf(stderr,"0x%lx\n",*(unsigned long*)&p);
    p += words;
   }
}

void memory_manager::statistics_for_blocksize(int blk_sz, long& used_chks, 
                                                          long& free_chks, 
                                                          long& blk_cnt, 
                                                          long& bytes)
{ if (blk_sz < 0 || blk_sz > max_sz) 
  { used_chks = 0; free_chks = 0; blk_cnt = -1; bytes = 0; // error
   }
  else 
    if (blk_sz == 0) 
      { used_chks = -1; free_chks = -1; blk_cnt = block_count[0]; 
        bytes = total_count[0];
       }
    else 
      { blk_cnt = block_count[blk_sz];
	long total_chunks = total_count[blk_sz];
        bytes = total_chunks * blk_sz; 
        memory_elem_ptr p = free_list[blk_sz];
        free_chks = 0;
        while (p) { free_chks++; p = p->next; }
        used_chks = total_chunks - free_chks;
       }
}

void memory_manager::print_statistics()
{ 
  fprintf(stderr,"\n");
  // change for VS 2015, 64 bit
#if (defined(_MSC_VER) && _MSC_VER >= 1900 && defined(_M_X64))
  fprintf(stderr,"\t %s   [%llu]\n",name,(unsigned long long)this); 
#else
  fprintf(stderr,"\t %s   [%lu]\n",name,(unsigned long)this); 
#endif

  fprintf(stderr,
          "\t+------------------------------------------------------------+\n");
  fprintf(stderr,
          "\t|   size       used       free       blocks         bytes    |\n");
  fprintf(stderr,
          "\t+------------------------------------------------------------+\n");

  long int total_bytes=0, free_bytes=0, used_bytes=0;

  for (int i=1;i<=max_sz;i++)
  { long total = total_count[i];
    if (block_count[i])
    { fprintf(stderr,"\t|   %3d    ",i);
      fflush(stderr);
      memory_elem_ptr p = free_list[i];
      long free = 0;
      while (p) { free++; p = p->next; }
      long b = total*i; 
      long used = total - free;
      free_bytes  += free*i;
      used_bytes  += used*i;
      total_bytes += b;
      fprintf(stderr,"%8ld   %8ld    %8ld   %12ld    |\n",
              used,free,block_count[i],b);
     }
   }

   
  if (block_count[0])
    fprintf(stderr,"\t| > %3d           -          -    %8ld   %12ld    |\n",
                          max_sz,block_count[0],total_count[0]);
     

 //float kb = float(total_bytes)/1024;
 float kb = float(alloc_count)/1024;
 float mb = kb/1024;
 float sec = used_time();

 fprintf(stderr,
       "\t+------------------------------------------------------------+\n");
 fprintf(stderr,
       "\t|   cpu: %7.2f sec                      space: %8.2f MB |\n",sec,mb);
 fprintf(stderr,
       "\t+------------------------------------------------------------+\n");
 fprintf(stderr,"\n");
 fflush(stderr);
}


void memory_manager::print_used_memory()
{ 
  long int total,free,used;
  long int total_bytes=0, free_bytes=0, used_bytes=0, b;
  int count = 0;

  for (int i=1;i<=max_sz;i++)
  if ((total = total_count[i]) > 0 || free_list[i])
  { memory_elem_ptr p = free_list[i];
    free = 0;
    while (p) { free++; p = p->next; }
    b = total*i; 
    used = total - free;
    free_bytes  += free*i;
    used_bytes  += used*i;
    total_bytes += b;
    //if (used > 0)
    if (used != 0)
    { if (count++ == 0)
      { fprintf(stderr,"\n");
        fprintf(stderr,"\t %s (memory still in use)\n",name); 
        fprintf(stderr,
          "\t+------------------------------------------------------------+\n");
        fprintf(stderr,
          "\t|   size       used       free       blocks         bytes    |\n");
        fprintf(stderr,
          "\t+------------------------------------------------------------+\n");
       }
      fprintf(stderr,"\t|   %3d    %8ld   %8ld    %8ld   %12ld    |\n",
                      i,used,free,block_count[i],b);
     }
   }

  if (count > 0)
  { float kb = float(total_bytes)/1024;
    float mb = kb/1024;
    float sec = used_time();
    fprintf(stderr,
       "\t+------------------------------------------------------------+\n");
    fprintf(stderr,
       "\t|   cpu: %7.2f sec                      space: %8.2f MB |\n",sec,mb);
    fprintf(stderr,
       "\t+------------------------------------------------------------+\n");
    fprintf(stderr,"\n");
    fflush(stderr);

    // for (int i=5;i<=max_sz;i++) check_free_list(i);
   }
}

LEDA_END_NAMESPACE

