/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  multi-thread.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/system/multi_thread.h>

#if defined(__win32__) || defined(__win64__)

#if defined(MAXINT)
#undef MAXINT
#endif

#include <windows.h>

STDAPI VarCmp(tagVARIANT*, tagVARIANT*, unsigned long, unsigned long) {
    return 0;
}

LEDA_BEGIN_NAMESPACE

thread_t thread_create(void* (*func)(void*), void* arg)
{ DWORD thread_id;
  return CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)func,arg,0,&thread_id);
}

void thread_join(const thread_t& thr) 
{ WaitForSingleObject(thr,INFINITE); }

void thread_detach(const thread_t& thr) { CloseHandle(thr); }

thread_id_t thread_self() { return (thread_id_t)GetCurrentThreadId(); }


LEDA_END_NAMESPACE

#else

#include <pthread.h>

typedef pthread_t thread_t;

LEDA_BEGIN_NAMESPACE

#if !defined(LEDA_MULTI_THREAD)

thread_t thread_create(void* (*func)(void*), void* arg) { 
  return 0;
}

void thread_join(const thread_t& thr) { }

void thread_detach(const thread_t& thr) { }

thread_id_t thread_self() { return 0; }

#else

typedef void* (*tfunc)(void*);

thread_t thread_create(void* (*func)(void*), void* arg)
{ thread_t thr;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&(thr),&attr,(tfunc)func,arg);
  pthread_attr_destroy(&attr);
  return thr;
}

void thread_join(const thread_t& thr) { pthread_join(thr,NULL); }

void thread_detach(const thread_t& thr) { pthread_detach(thr); }

thread_id_t thread_self() { return (thread_id_t)pthread_self(); }

#endif

LEDA_END_NAMESPACE


#endif


