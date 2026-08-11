/*******************************************************************************
+
+  LEDA 7.2.3  
+
+
+  draw_points.c
+
+
+  Copyright (c) 1995-2026
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>

using namespace leda;

int main()
{ 
  window W(400,400);
  W.display(window::center,window::center);

  point p;
  while (W >> p) W << p;

  W.screenshot("draw_points");
  return 0;
}
