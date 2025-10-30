/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_about.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>

using namespace leda;

main()
{
  GraphWin gw;

  gw.add_help_text("about1");
  gw.add_help_text("about2");
  gw.add_help_text("about3");

  gw.display(window::center,window::center);
  gw.display_help_text("about1");
  gw.edit();
  return 0;
}


