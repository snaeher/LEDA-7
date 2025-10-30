/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  memory_sys.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MEMORY_SYS_H
#define LEDA_MEMORY_SYS_H

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

class __exportC sys_memory_manager {

public:

 sys_memory_manager() {}
 sys_memory_manager(int) {}
 sys_memory_manager(int, const char*) {}
~sys_memory_manager() {}

/*
 void* allocate_bytes(size_t sz)  { return new char[sz]; }
 void* allocate_vector(size_t sz) { return new char[sz]; }

 void deallocate_bytes(void* p, size_t) { delete[] (char*)p; }
 void deallocate_vector(void* p)        { delete[] (char*)p; }
*/

 void* allocate_bytes(size_t sz)  { return malloc(sz); }
 void* allocate_vector(size_t sz) { return malloc(sz); }

 void deallocate_bytes(void* p, size_t) { free(p); }
 void deallocate_vector(void* p)        { free(p); }


 void* allocate_words(size_t sz)  
 { return allocate_bytes(sz*sizeof(void*)); }

 void  deallocate_words(void* p, size_t sz) 
 { deallocate_bytes(p,sz*sizeof(void*)); }


 void  deallocate_list(void* first, void* last, size_t sz);

 
 void clear()              {}
 void kill()               {}
 void print_statistics()   {}
 void print_used_memory()  {}
 void check_free_list(int) {}

 int  used_memory() { return 0; }
};



extern __exportD sys_memory_manager sys_memory_mgr;

LEDA_END_NAMESPACE


#endif

