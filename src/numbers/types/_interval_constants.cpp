/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _interval_constants.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/numbers/fp.h>
#include <LEDA/numbers/interval_constants.h>

LEDA_BEGIN_NAMESPACE

const double roundup_factor = 1+fp::power_two(-51);
const double rounddown_factor = 1-fp::power_two(-51);
const double MaxError = fp::power_two(915);
const double eps = fp::power_two(-52);  
const double MinDbl = fp::compose_parts(0,1,0,0);
const double twoMinDbl = 2*MinDbl; 
const double threeMinDbl = 3*MinDbl;
const double DenormalizedMinDbl = fp::compose_parts(0,0,0,1); // really smallest positive double
const double MaxDbl = fp::compose_parts(0, 2046, 0xFFFFF, 0xFFFFFFFF);

LEDA_END_NAMESPACE
