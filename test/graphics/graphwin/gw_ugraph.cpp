/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_ugraph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/ugraph.h>

using namespace leda;

int main() 
{
  UGRAPH<int,int> G;
  GraphWin gw(G);

  gw.display(window::center,window::center);
  gw.edit();

  return 0;
}

