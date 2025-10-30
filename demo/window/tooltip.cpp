/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  tooltip.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




#include <LEDA/graphics/window.h>

using namespace leda;

void redraw(window* wp)
{ wp->draw_rectangle(20,20,30,30,blue);  
  wp->draw_rectangle(60,40,70,50,red); 
}

int main()
{
  window W;
  W.set_redraw(redraw);
  W.display(window::center,window::center);
  W.redraw();

  W.set_tooltip(1,20,20,30,30,"Tooltip 1\nZeile 2\nZeile 3");
  W.set_tooltip(2,60,40,70,50,"Tooltip 2\nZeile 2\nZeile 3");


  W.read_mouse();
  W.del_tooltip(1);

  W.read_mouse();
  W.del_tooltip(2);

  W.read_mouse();

 
  return 0;
}

