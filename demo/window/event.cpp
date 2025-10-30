/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  event.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <math.h>

using namespace leda;

int main()
{
  window W(450,500,"Event Demo");
  W.display();

  W.start_buffering();

  for(;;)
  { 
    // read the first corner p of the rectangle
    // terminate if the right button was clicked 

    point p;
    if (W.read_mouse(p) == MOUSE_BUTTON(3)) break;  
  
    // draw rectangle from p to current position while button down 

    int  val;
    double x,y;

    char* win_buf = W.get_window_pixrect();

    int k;
    while ((k = W.read_event(val,x,y)) != button_release_event) 
    { std::cout << k << std::endl;
      point q(x,y);
      //W.put_pixrect(win_buf);
      W.put_pixrect(W.xmin(),W.ymin(),win_buf);
      W.draw_box(p,q,yellow);
      W.draw_rectangle(p,q,black);
      W.flush_buffer();
     }

    W.del_pixrect(win_buf);
  }

  W.stop_buffering();

  W.screenshot("event");
  return 0;
}
