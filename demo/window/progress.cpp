/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  progress.c
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
  int count = 0;

  window W(600,300);
  W.int_item("%Progress",count,0,100);
  W.display(window::center,window::center);

  for(;;)
  { count = 0;
    while (count <= 100)
    { W.redraw_panel();
      W.flush();
      leda_wait(0.02);
      count++;
     }
    if (W.read_mouse() == MOUSE_BUTTON(3)) break;
   }

  W.screenshot("progress");
  return 0;
}
