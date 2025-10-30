/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  system.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_SYSTEM_H
#define LEDA_SYSTEM_H

#if defined(__CYGWIN__) || defined(__CYGWIN32__)
// cygwin with g++

#include <LEDA/internal/sys/windows.h>

#elif defined(unix) || defined(__unix__) || defined(__unix) || defined(_AIX) || defined(__APPLE__)

#include <LEDA/internal/sys/unix.h>

#elif defined(_WIN32) || defined(_WIN64)

#include <LEDA/internal/sys/windows.h>

#endif


#endif

