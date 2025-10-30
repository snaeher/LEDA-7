/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  win32.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_SYS_WIN32_H
#define LEDA_SYS_WIN32_H

#if defined(_WIN64)

// we compile for 64 bit (x64 platform)

#define __win64__

/*
// do not use assembler for win64
#define ASM_AMD64
#define SQRT_ROUNDING_OKAY
*/

#else

// we compile for 32 bit (x86 platform)

#define __win32__

#define ASM_INTEL_386

#if !defined(i386)
#define i386
#endif

#endif


#define LITTLE_ENDIAN_MACHINE
#define _BIND_TO_CURRENT_CRT_VERSION 1

#ifndef NOMINMAX
#define NOMINMAX
#endif



#if defined(_MSC_VER)

// turn off some msc warnings

// missing dll-interface
#pragma warning(disable:4251)
#pragma warning(disable:4275)

// no matching delete operator (in presence of -GX)
#pragma warning(disable:4291)

// turn of deprecated warning for insecure CRT runtime functions
#pragma warning(disable:4996)

/*
// exception handling turned off when using std headers
#pragma warning(disable:4530)

// this used in base class initialization
#pragma warning(disable:4355)

// resolved overload found by argument dependent lookup
#pragma warning(disable:4675)

// turn of reference exceeds storage space warning
#pragma warning(disable:4739)

*/


#include <float.h>

inline int leda_init_fpu() 
{ _control87(_PC_53,_MCW_PC);
  _control87(_RC_NEAR,_MCW_RC);
  return 0; 
}

static int setdouble_ieee_for_msc = leda_init_fpu();

#endif



//------------------------------------------------------------------------------
//  DLL definitions
//------------------------------------------------------------------------------

#define __exportGeoW

#if defined(LEDA_DLL)

#if defined(LEDA_INSTALL) && !defined(LEDA_IMPORT) 
#define __exportC    __declspec(dllexport)
#define __exportF    __declspec(dllexport)
#define __exportD    __declspec(dllexport)
// #define __exportGeoW __declspec(dllexport)
#else
#define __exportC    __declspec(dllimport)
#define __exportF    __declspec(dllimport)
#define __exportD    __declspec(dllimport)
// #define __exportGeoW __declspec(dllimport)
#endif

#else

#define __exportC
#define __exportF
#define __exportD
#define __exportGeoW

#endif



#endif
