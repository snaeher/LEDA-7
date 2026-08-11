/*******************************************************************************
+
+  LEDA 7.2.3  
+
+
+  geowin.c
+
+
+  Copyright (c) 1995-2026
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>

using namespace leda;

int main() 
{
  GeoWin gw("GeoWin");
  gw.display();
  gw.edit();

  return 0;
}

