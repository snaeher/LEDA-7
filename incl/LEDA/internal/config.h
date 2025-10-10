//-----------------------------------------------------------------------------
// configuration
//-----------------------------------------------------------------------------

// enable stl-style iterators
#ifndef LEDA_STL_ITERATORS
#define LEDA_STL_ITERATORS
#endif

#if defined(__GNUC__) && defined(_REENTRANT)

#if !defined(LEDA_MULTI_THREAD)
// #define LEDA_MULTI_THREAD
#error "Use -DLEDA_MULTI_THREAD when compiling with -pthread."
#endif

#endif


#if !defined(LEDA_MULTI_THREAD)

// single thread configuration

#define LEDA_MEMORY_STD

#else

// multi thread configuration

//-----------------------------------------------------------------------------
// choose mt memory manager
// (if not defined on compiler command line)
//-----------------------------------------------------------------------------

#if !defined(LEDA_MEMORY_MT) && !defined(LEDA_MEMORY_SYS) && !defined(LEDA_USE_MEMORY_MT) && !defined(LEDA_USE_MEMORY_SYS)


// use LEDA's mt memory manager 
#define LEDA_MEMORY_MT

/*
// use system memory manager
#define LEDA_MEMORY_SYS
*/

#endif


//-----------------------------------------------------------------------------
// choose atomic counter implementation
// (if not defined on compiler command line)
//-----------------------------------------------------------------------------


#if !defined(LEDA_ATOMIC_FAA) && !defined(LEDA_ATOMIC_SP) && !defined(LEDA_ATOMIC_MTX)

// FAA: fetch and add (default)
#define LEDA_ATOMIC_FAA

/*
// SP: spinlocks
#define LEDA_ATOMIC_SP

// MTX: mutex
#define LEDA_ATOMIC_MTX
*/

#endif



// use compare for equality test instead of operator==  (list::unique, ...)
// #define EQ_BY_COMPARE



#endif




