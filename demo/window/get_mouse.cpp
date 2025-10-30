/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  get_mouse.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>

using namespace leda;

random_source& operator>>(random_source& ran, point& p)
{ int x,y;
  ran >> x >> y;
  p = point(x,y);
  return ran;
}


int main()
{
  window W(400,400);

  W.display(window::center,window::center);

  W.message("left button: clear    right button: stop");

  random_source ran(0,100);

  int but;
  while ( (but = W.get_mouse()) != MOUSE_BUTTON(3) )
  { 
    if (but == MOUSE_BUTTON(1)) W.clear();

    point p;
    ran >> p;
    W.draw_point(p,blue);
  }

  W.screenshot("get_mouse");
  return 0;
}
