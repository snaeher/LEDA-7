/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_help.c
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
  GeoWin gw;

  gw.add_help_text("about1");
  gw.add_help_text("about2");
  gw.add_help_text("about3");

  gw.edit();
  return 0;
}

