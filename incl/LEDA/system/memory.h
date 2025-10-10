/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  memory.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MEMORY_H
#define LEDA_MEMORY_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721170
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>

#if defined(LEDA_MEMORY_STD) || defined(LEDA_USE_MEMORY_STD)
#include <LEDA/system/memory_std.h>
#define std_memory std_memory_mgr
#define LEDA_THREAD_BEGIN
#endif

#if defined(LEDA_MEMORY_SYS) || defined(LEDA_USE_MEMORY_SYS)
#include <LEDA/system/memory_sys.h>
#define std_memory sys_memory_mgr
#define LEDA_THREAD_BEGIN
#endif

#if defined(LEDA_MEMORY_MT) || defined(LEDA_USE_MEMORY_MT)
#include <LEDA/system/memory_mt.h>
#define std_memory mt_memory_mgr
#define LEDA_THREAD_BEGIN leda::thread_memory_init leda_thread_memory_init_x;
#endif


#define LEDA_MEMORY(type)\
void* operator new(size_t bytes)\
{  return leda::std_memory.allocate_bytes(bytes); }\
\
void  operator delete(void* p, size_t bytes)\
{ leda::std_memory.deallocate_bytes(p,bytes); }\
\
void* operator new(size_t, void* p) { return p; }\


#define LEDA_NEW_VECTOR(T,n) (T*)leda::std_memory.allocate_vector((n)*sizeof(T))
#define LEDA_DEL_VECTOR(x)    leda::std_memory.deallocate_vector(x)



#define LEDA_MEMORY_WITH_CHECK(type)\
void* operator new(size_t bytes)\
{ return leda::std_memory.allocate_bytes_with_check(bytes); }\
\
void  operator delete(void* p,size_t bytes)\
{ if (p != 0) leda::std_memory.deallocate_bytes_with_check(p,bytes); }\
\
void* operator new(size_t,void* p) { return p; }\



LEDA_BEGIN_NAMESPACE

inline unsigned long used_memory() { return std_memory.used_memory(); }

inline void print_statistics() { return std_memory.print_statistics(); }

LEDA_END_NAMESPACE


/*Manpage*/

/*{\Mtext

\section{Memory Management} \label{Memory Management}

LEDA offers an efficient memory management system that is used internally 
for all node, edge and item types. This system can easily be customized for 
user defined classes by the ``LEDA\_MEMORY" macro. You simply have
to add the macro call ``LEDA\_MEMORY($T$)" to the declaration of a class
$T$. This redefines new and delete operators for type $T$, such that
they allocate and deallocate memory using LEDA's internal memory manager. 

\begin{verbatim}
struct pair {
  double x;
  double y;

  pair() { x = y = 0; }
  pair(const pair& p) { x = p.x; y = p.y; }

  friend ostream& operator<<(ostream&, const pair&) { ... }
  friend istream& operator>>(istream&, pair&)       { ... }
  friend int compare(const pair& p, const pair& q)  { ... }

  LEDA_MEMORY(pair)

};

dictionary<pair,int> D;
\end{verbatim}
\ \\
The LEDA memory manager only frees memory at its time of destruction (program end or
unload of library) as this allows for much faster memory allocation requests. 
As a result, memory that was deallocated by a call to the redefined delete operator still
resides in the LEDA memory management system and is not returned to the system memory
manager. This might lead to memory shortages. To avoid those shortages, it is possible
to return unused memory of LEDA's memory management system to the system memory manager
by calling
\begin{verbatim}
leda::std_memory_mgr.clear();
\end{verbatim}

}*/



#if LEDA_ROOT_INCL_ID == 721170
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



#endif
