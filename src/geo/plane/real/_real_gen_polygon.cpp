/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _real_gen_polygon.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/core/stack.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/map.h>
#include <LEDA/system/assert.h>
#include <LEDA/core/string.h>

#include <LEDA/geo/real_gen_polygon.h>
#include <LEDA/geo/real_kernel.h>
#include <LEDA/core/dictionary.h>
#include <LEDA/core/int_set.h>

#include <LEDA/geo/real_kernel_names.h>
#include "gen_polygon.cpp"
#include <LEDA/geo/kernel_names_undef.h>

