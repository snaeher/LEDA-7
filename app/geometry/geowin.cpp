/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>

using namespace leda;

int main() 
{
  string title("LEDA  GeoWin %.1f", 0.01*__GEOWIN_VERSION__);

  GeoWin gw(title);

  gw.display();
  gw.help_about();

  gw.edit();
  return 0;
}

