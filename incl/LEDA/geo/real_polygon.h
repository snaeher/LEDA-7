/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  real_polygon.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_REAL_POLYGON_H
#define LEDA_REAL_POLYGON_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 722078
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/geo/real_point.h>
#include <LEDA/geo/real_segment.h>
#include <LEDA/geo/real_line.h>
#include <LEDA/core/list.h>
#include <LEDA/geo/polygon.h>
//#include <LEDA/geo/polygon.h>


/* AND NO COMES THE PART THAT ADOPTS THE KERNEL */

#include <LEDA/geo/real_kernel_names.h>

#include <LEDA/geo/generic/POLYGON.h>

LEDA_BEGIN_NAMESPACE

inline const char* leda_tname(const real_polygon*) { return "real_polygon"; }

LEDA_END_NAMESPACE

#include <LEDA/geo/kernel_names_undef.h>

#if LEDA_ROOT_INCL_ID == 722078
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



#endif 
