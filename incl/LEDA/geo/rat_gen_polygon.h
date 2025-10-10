/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  rat_gen_polygon.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_RAT_GEN_POLYGON_H
#define LEDA_RAT_GEN_POLYGON_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721098
#include <LEDA/internal/PREAMBLE.h>
#endif


#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/geo/rat_kernel.h>

#include <LEDA/geo/polygon.h>
#include <LEDA/geo/gen_polygon.h>


#include <LEDA/geo/rat_kernel_names.h>

#include <LEDA/geo/generic/GEN_POLYGON.h>

LEDA_BEGIN_NAMESPACE

inline const char* leda_tname(const rat_gen_polygon*) { return "rat_gen_polygon"; }

#include <LEDA/geo/kernel_names_undef.h>


#if LEDA_ROOT_INCL_ID == 721098
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif 



