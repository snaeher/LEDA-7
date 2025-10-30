/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _memory_sys.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// System Memory Manager
//
// last modified  08/2008  (by sn)
//------------------------------------------------------------------------------


#include <LEDA/system/basic.h>
#include <LEDA/system/memory_sys.h>

LEDA_BEGIN_NAMESPACE

sys_memory_manager sys_memory_mgr;

struct list_elem {
list_elem* succ;
};


void  sys_memory_manager::deallocate_list(void* first, void* last, size_t bytes)
{
  while (first != last)
  { void* p = ((list_elem*)first)->succ;
    deallocate_bytes(first,bytes);
    first = p;
   } 
   if (last) deallocate_bytes(last,bytes);
}



LEDA_END_NAMESPACE
