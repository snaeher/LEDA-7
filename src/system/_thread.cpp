/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _thread.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/system/thread.h>
#include <LEDA/system/stream.h>


#if !defined(LEDA_MULTI_THREAD)

LEDA_BEGIN_NAMESPACE

// dummy functions for non-mt version
unsigned long get_thread_id() { return 0; }
unsigned long tls_alloc() { return 0; }
void  tls_free(unsigned long i) { }
void* tls_get_value(unsigned long i) { return 0; }
void  tls_set_value(unsigned long i, void* x) {}

LEDA_END_NAMESPACE

#else

#if defined(__win32__) || defined(__win64__)

#if defined(MAXINT)
#undef MAXINT
#endif

#include <windows.h>

LEDA_BEGIN_NAMESPACE

unsigned long get_thread_id() { return GetCurrentThreadId(); }

// thread local storage (tls)

unsigned long tls_alloc() { 
  unsigned long i = TlsAlloc();
  if (i == TLS_OUT_OF_INDEXES) i = 0;
  return i; 
}

void  tls_free(unsigned long i) { TlsFree(i); }
void* tls_get_value(unsigned long i) { return TlsGetValue(i); }
void  tls_set_value(unsigned long i, void* x) { TlsSetValue(i,x); }


leda_mutex::leda_mutex()   { mtx = CreateMutex(NULL, FALSE, NULL); }
leda_mutex::~leda_mutex()  { CloseHandle(mtx); }

void leda_mutex::lock()    { WaitForSingleObject(mtx, INFINITE); }
void leda_mutex::unlock()  { ReleaseMutex(mtx); }
int leda_mutex::trylock()
{ return (WaitForSingleObject(mtx, 0) == WAIT_OBJECT_0) ? 0 : EBUSY; }

LEDA_END_NAMESPACE


#else

#include <pthread.h>

LEDA_BEGIN_NAMESPACE

unsigned long get_thread_id()   { return (unsigned long)pthread_self(); }

unsigned long tls_alloc() { 
   pthread_key_t key = 0;
   pthread_key_create(&key,NULL);
   return (unsigned long)key; 
}

void  tls_free(unsigned long i) { 
   pthread_key_delete(i);
}

void* tls_get_value(unsigned long i) { 
   return pthread_getspecific(i);
}

void  tls_set_value(unsigned long i, void* x) {
   pthread_setspecific(i,x);
}

leda_mutex::leda_mutex()   { pthread_mutex_init(&mtx,NULL); }
leda_mutex::~leda_mutex()  { pthread_mutex_destroy(&mtx); }

void leda_mutex::lock()    { pthread_mutex_lock(&mtx); }
void leda_mutex::unlock()  { pthread_mutex_unlock(&mtx); }
int  leda_mutex::trylock() { return pthread_mutex_trylock(&mtx); }
  
LEDA_END_NAMESPACE

#endif

#endif

