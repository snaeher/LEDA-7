/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  read_mouse1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;



int main() 
{ 
  window W;
  W.init(-1,+1,-1);
  W.display();

  W.draw_disc(0,0,0.5,red);

  W.read_mouse();

  W.screenshot("read_mouse1");
  return 0;
}
