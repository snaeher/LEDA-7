/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _version.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>

LEDA_BEGIN_NAMESPACE

const char* compile_date  =  __DATE__ ;
const char* compile_time  =  __TIME__;

const char* version_string  = "LEDA 7.2.2";
const char* copyright_string= "LEDA 7.2.2 (c) 1995-2025 Algorithmic Solutions";

// The string "copyright_window_string" is used to print a
// notice into the lower right corner of every LEDA window. 
// The default value is "powered by LEDA". 

const char* copyright_window_string = "powered by LEDA";


LEDA_END_NAMESPACE
