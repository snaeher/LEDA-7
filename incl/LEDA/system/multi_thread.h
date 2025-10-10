/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  multi-thread.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MULTI_THREAD_H
#define LEDA_MULTI_THREAD_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721164
#include <LEDA/internal/PREAMBLE.h>
#endif


/* thread_t
   thread_t thread_create(void* (*func)(void*), void* arg);
   void     thread_join(const thread_t& thr); 
   unsigned long thread_self();
*/
   

#if defined(__win32__) || defined(__win64__)

/*
#if defined(MAXINT)
#undef MAXINT
#endif

#include <windows.h>

STDAPI VarCmp(tagVARIANT*, tagVARIANT*, unsigned long, unsigned long) {
    return 0;
}
*/

LEDA_BEGIN_NAMESPACE

typedef void* thread_t;
typedef unsigned long thread_id_t;

thread_t    __exportF thread_create(void* (*func)(void*), void* arg);
void        __exportF thread_join(const thread_t& thr);
void        __exportF thread_detach(const thread_t& thr);
thread_id_t __exportF thread_self();


LEDA_END_NAMESPACE

#else

#include <pthread.h>

LEDA_BEGIN_NAMESPACE

typedef pthread_t thread_t;
typedef long thread_id_t;

extern thread_t thread_create(void* (*func)(void*), void* arg);
extern void     thread_join(const thread_t& thr);
extern void     thread_detach(const thread_t& thr);
extern thread_id_t thread_self();

LEDA_END_NAMESPACE

#endif


#if LEDA_ROOT_INCL_ID == 721164
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif

