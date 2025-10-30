/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  version.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include "local.h"

LEDA_BEGIN_NAMESPACE

double GraphWin::version()
{ return GraphWinVersion; }

string GraphWin::version_str() 
{ return string("GraphWin %.1f", GraphWinVersion); }

LEDA_END_NAMESPACE
