/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_objects.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#if GRAPH_REP == 0
#include "_g_objects0.cpp"

#elif GRAPH_REP == 1
#include "_g_objects1.cpp"

#elif GRAPH_REP == 2
#include "_g_objects2.cpp"

#else
#error Error: Illegal value for GRAPH_REP.

#endif


