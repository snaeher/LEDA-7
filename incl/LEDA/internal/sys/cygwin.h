/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  cygwin32.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_SYS_CYGWIN32_H
#define LEDA_SYS_CYGWIN32_H

#if !defined(__unix__)
#define __unix__
#endif

#define LITTLE_ENDIAN_MACHINE
#define ASM_INTEL_386

#undef WINMAIN

//------------------------------------------------------------------------------
//  DLL definitions
//------------------------------------------------------------------------------

#if defined(LEDA_DLL)

#if defined(LEDA_INSTALL) && !defined(LEDA_IMPORT)
#define __exportC    __declspec(dllexport)
#define __exportF    __declspec(dllexport)
#define __exportD    __declspec(dllexport)
#define __exportGeoW __declspec(dllexport)
#else
#define __exportC    __declspec(dllimport)
#define __exportF    __declspec(dllimport)
#define __exportD    __declspec(dllimport)
#define __exportGeoW __declspec(dllimport)
#endif

#else

#define __exportC
#define __exportF
#define __exportD
#define __exportGeoW

#endif



#endif
