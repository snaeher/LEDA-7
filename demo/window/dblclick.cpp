/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dblclick.c
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
  unsigned long timeout = 500;

  window W(800,800,"Double Click Demo");
  W.set_grid_dist(6);
  W.set_grid_style(line_grid);
  W.display(window::center,window::center);

  for(;;)
  { 
    int b;
    double x0,y0,x,y;
    unsigned long t, t_press, t_release;

    while (W.read_event(b,x0,y0,t_press) != button_press_event); 
    // a button was pressed at (x0,y0) at time t_press

    // the middle button erases the window
    if (b == MOUSE_BUTTON(2) ) { W.clear(); continue; }

    // the right button terminates the program
    if (b == MOUSE_BUTTON(3) ) break;  

    while (W.read_event(b,x,y,t_release) != button_release_event); 
    // the button was released at time t_release

    color col = yellow;

    // If the button was held down no longer than timeout msecs 
    // we wait for the remaining msecs for a second press, if the
    // the button is pressed again within this period of time we
    // have a double click and we change the color to red.

    if (t_release - t_press < timeout)
    { unsigned long timeout2 = timeout - (t_release - t_press);
      if (W.read_event(b,x,y,t,timeout2) == button_press_event) 
        col = red;
     }

    W.draw_disc(x0,y0,2.5,col);
    W.draw_circle(x0,y0,2.5,black);
  }

  W.screenshot("dblclick");
  return 0;
}
