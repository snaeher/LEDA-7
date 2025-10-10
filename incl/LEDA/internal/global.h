/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  global.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_GLOBAL_H
#define LEDA_GLOBAL_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721156
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <limits>


//------------------------------------------------------------------------------
// values and macros
//------------------------------------------------------------------------------


#if !defined(MAXINT)
#define MAXINT std::numeric_limits<int>::max()
#endif

#if !defined(MAXDOUBLE)
#define MAXDOUBLE std::numeric_limits<double>::max()
#endif

#if LONG_BIT == 64 || defined(__ia64__) || defined(_LONG64) || defined(__amd64) || defined(__WIN64__)
#define WORD_LENGTH_64
#define WORD_LENGTH_LOG 6
#else
#define WORD_LENGTH_32
#define WORD_LENGTH_LOG 5
#endif


#define	LEDA_PI   3.14159265358979323846
#define	LEDA_PI_2 1.57079632679489721156

#ifndef nil
//#define nil nullptr
#define nil 0
#endif

#define DEFINE_LINEAR_ORDER(type,cmp,new_type)\
struct new_type : public type\
{ new_type(const type& s)     : type(s) {}\
  new_type(const new_type& s) : type(s) {}\
  new_type() {}\
 ~new_type() {}\
};\
inline int compare(const new_type& x, const new_type& y) { return cmp(x,y); }


//------------------------------------------------------------------------------
// global types, constants, functions, and macros
//------------------------------------------------------------------------------


LEDA_BEGIN_NAMESPACE

typedef void* GenPtr;    // generic pointer type

const int behind = 0;
const int before = 1;
const int after = behind;


// deutsche Umlaute

const char uml_a = '\344';
const char uml_o = '\366';
const char uml_u = '\374';
const char uml_A = '\304';
const char uml_O = '\326';
const char uml_U = '\334';


// global flags and names

extern __exportD int   node_data_slots;
extern __exportD int   edge_data_slots;
extern __exportD int   face_data_slots;

extern __exportD char*       option_list;
extern __exportD const char* version_string;
extern __exportD const char* copyright_string;
extern __exportD const char* copyright_window_string;
extern __exportD const char* compile_date;
extern __exportD const char* compile_time;
extern __exportD const char* rootdir_name;


// global functions

extern __exportF void  compiler_string(char*);
extern __exportF void  write_log(const char*);
extern __exportF void  check_license();
extern __exportF void  user_log();
extern __exportF void  read_options();

extern __exportF int get_host_info(char* name, int sz, unsigned long* idlist,
                                                       int max_ids);

extern __exportF void  trace_stack(ostream&);
extern __exportF int   trace_stack(char**);

LEDA_END_NAMESPACE


#if LEDA_ROOT_INCL_ID == 721156
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
