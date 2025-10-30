/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  misc.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_MISC_H
#define LEDA_MISC_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722069
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/basic.h>
#include <LEDA/core/string.h>

#include <algorithm>

LEDA_BEGIN_NAMESPACE


/*{\Manpage {misc} {} {Some Useful Functions}  }*/

/*{\Mtext
The following functions and macros are defined in \<LEDA/core/misc.h\>.
}*/

/*{\Moptions nextwarning=no}*/
extern __exportF int read_int();
extern __exportF int read_int(string s);
/*{\Mfunc  prints $s$ and reads an integer from $cin$.}*/

/*{\Moptions nextwarning=no}*/
extern __exportF double read_real();
extern __exportF double read_real(string s);
/*{\Mfunc  prints $s$ and reads a real number from $cin$.}*/


extern __exportF void   skip_line(istream& =cin);
extern __exportF string read_line(istream& =cin);

/*{\Moptions nextwarning=no}*/
extern __exportF string read_string();
extern __exportF string read_string(string s);
/*{\Mfunc  prints $s$ and reads a line from $cin$.}*/

/*{\Moptions nextwarning=no}*/
extern __exportF char read_char();
extern __exportF char read_char(string s);
/*{\Mfunc  prints $s$ and reads a character from $cin$.}*/



/*{\Moptions nextwarning=no}*/
extern __exportF int Yes();
extern __exportF int Yes(string s);
/*{\Mfunc  returns (read\_char($s$) == `y').}*/

extern __exportF bool get_environment(string var);
/*{\Mfunc  returns |true| if variable |var| is defined in the current 
           environment and |false| otherwise. }*/

extern __exportF bool get_environment(string var, string& val);
/*{\Mfunc  if variable |var| is defined in the current environment
           its value is assigned to |val| and the result is |true|. 
           Otherwise, the result is |false|.}*/


extern __exportF unsigned long epoche_time();
/*{\Mfunc  returns the unix epoche timestamp in milliseconds. }*/


extern __exportF float used_time();
extern __exportF float used_time(float& T);

extern __exportF double cpu_time();
/*{\Mfunc  returns the currently used cpu time in seconds.
	   (The class |timer| in Section~\ref{timer} provides a
	    nicer interface for time measurements.)}*/

extern __exportF double cpu_time(double& T);
/*{\Mfunc  returns the cpu time used by the program from time
           $T$ up to this moment and assigns the current
           time to $T$.}*/


extern __exportF float elapsed_time();
/*{\Mfunc  returns the current daytime time in seconds. }*/

extern __exportF float elapsed_time(float& T);
/*{\Mfunc  returns the elapsed time since time $T$ and assigns the 
           current elapsed time to $T$.}*/

inline float real_time()         { return elapsed_time(); }
/*{\Mfunc  same as $elapsed\_time()$. }*/

inline float real_time(float& T) { return elapsed_time(T); }
/*{\Mfunc  same as $elapsed\_time(T)$. }*/


extern __exportF void get_time(int& ye,int& mo,int& da, int& h,int& m,int& s);

extern __exportF string time_string(time_t t);

extern __exportF string time_string();



extern __exportF void get_command_line(int& argc, char**& argv);


extern __exportF string get_user_name();
extern __exportF string get_host_name();



/*{\Moptions nextwarning=no}*/
/*
void  print_statistics();
*/
/*{\Mfunc prints a summary of the currently used memory, which is used by LEDA's
    internal memory manager. This only reports on memory usage of LEDA's internal types
    and user-defined types that implement the LEDA\_MEMORY macro (see 
    Section \ref {Memory Management}).}*/



extern __exportF bool is_space(char c);  
/*{\Mfunc returns |true| is |c| is a white space character.}*/


extern __exportF void sleep(double sec);  
/*{\Mfunc  suspends execution for $sec$ seconds.}*/

inline void wait(double sec) { sleep(sec); }
/*{\Mfunc  suspends execution for $sec$ seconds.}*/

// for backward compatibility
#define leda_sleep leda::sleep
#define leda_wait  leda::wait


extern __exportF double truncate(double x, int k = 10);
/*{\Mfunc returns a double whose mantissa is truncated after $k-1$ bits after the binary point, i.e, if
$x \not= 0$ then the binary representation of the mantissa of the 
result has the form d.dddddddd, where the number of d's is equal to $k$. 
There is a corresponding function for
|integers|; it has no effect.}*/


string cplus_filter(string name);


//-----------------------------------------------------------------------------
// define function templates: min, max, swap
//-----------------------------------------------------------------------------

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif


#if 0

template <class T> 
inline const T& min(const T& a, const T& b) { return (a<b) ? a : b; }
/*{\Mfunc     returns the minimum of $a$ and $b$. }*/

template <class T> 
inline const T& max(const T& a, const T& b) { return (b<a) ? a : b; }
/*{\Mfunc     returns the maximum of $a$ and $b$. }*/


template <class T> 
inline void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }
/*{\Mfunc   swaps values of $a$ and $b$. }*/

#endif


// better: import function templates from std into leda namespace

using std::min;
using std::max;
using std::swap;


#define leda_min   leda::min
#define leda_max   leda::max
#define leda_swap  leda::swap



#if LEDA_ROOT_INCL_ID == 722069
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
