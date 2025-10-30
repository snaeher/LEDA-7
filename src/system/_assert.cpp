/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _assert.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/system/assert.h>

int assert_error(const char* cond, const char* fname, int line)
{ error_handler(999,
    leda::string("Assertion failed in file %s at line %d: %s",fname,line,cond));
  return 0;
 }

