/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  putback.c
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
  window W(400,400, "Putback Event Demo");
  W.init(-100,+100,-100);

  W.display(window::center,window::center);

  // partition the drawing area in four quadrants

  W.draw_hline(0);
  W.draw_vline(0);

  for(;;)
  { 
    double x,y; 

    // wait for first click
    int but = W.read_mouse(x,y);

    // middle button erases the window
    if (but == MOUSE_BUTTON(2)) 
    { W.clear();
      W.draw_hline(0);
      W.draw_vline(0);
      continue;
     }

    // right button terminates the program
    if (but == MOUSE_BUTTON(3)) break;

    // now we put the mouse click back to the event queue

    put_back_event();

    // and distinguish cases according to its position

    if (x < 0)
      if (y > 0)
        { point p;
          if (W >> p) W.draw_point(p,red);
         }
      else
        { segment s;
          if (W >> s) W.draw_segment(s,green);
         }
    else
      if (y > 0)
        { polygon pol;
          if (W >> pol) W.draw_polygon(pol,blue);
         }
      else
         { circle c;
           if (W >> c) W.draw_circle(c,orange);
          }
  }

 W.screenshot("putback");
 return 0;
}
