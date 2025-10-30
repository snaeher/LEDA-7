/******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _memory_std.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// std Memory Manager
//
// last modified  05/2020  (sn)
//------------------------------------------------------------------------------


#include <LEDA/system/basic.h>
#include <LEDA/system/memory_std.h>
#include <LEDA/system/assert.h>


#include <string.h>
#include <stdlib.h>

#if defined (ASM_INTEL_386)
extern "C" void i386_init_sse2();
extern int sse2_flag;
#else
inline void i386_init_sse2() {}
#endif

int sse2_flag = 0;

LEDA_BEGIN_NAMESPACE


memory_manager std_memory_mgr(0,"STD_MEMORY_MGR",0); 

unsigned memory_manager_init::count = 0;


memory_manager_init::memory_manager_init() 
{ //cout << "memory_manager_init: count = " << count << endl;

  if (count++ > 0) return;

#if !defined(LEDA_MULTI_THREAD)
  std_memory_mgr.init(STDMEM_TABLESIZE,"STD_MEMORY_MGR",0); 
#endif

// some initializations

  i386_init_sse2();

  read_options();
//check_license();

  //leda_init_fpu();
  //user_log();
  //LEDA_CHECK_VERSION();
}

memory_manager_init::~memory_manager_init()
{ //cout << "~memory_manager_init: count = " << count << endl;

#if !defined(LEDA_MULTI_THREAD)
  if (--count == 0) std_memory_mgr.destroy();
#endif

}


LEDA_END_NAMESPACE

