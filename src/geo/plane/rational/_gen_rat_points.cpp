/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _gen_rat_points.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/random_rat_point.h>
#include <LEDA/geo/rat_circle.h>
#include <LEDA/geo/rat_kernel_names.h>
#define D2
#define D2_POINT rat_point

#include "gen_points.cpp"

#include <LEDA/geo/kernel_names_undef.h>
#undef D2
#undef D2_POINT
