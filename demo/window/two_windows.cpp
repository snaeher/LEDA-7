/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  two_windows.c
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
   window W1(1200,800,"Window 1: circles");
   W1.display(window::min,window::min);

   window W2(1200,800,"Window 2: segments");
   W2.display(window::max,window::min);

   window W3(1200,800,"Window 3: points");
   W3.display(window::min,window::max);

   window W4(1200,800,"Window 4: lines");
   W4.display(window::max,window::max);

   for(;;)
   { window* wp;
     double x,y;
     int val;

     if (read_event(wp,val,x,y) != button_press_event) continue;

     if (val == MOUSE_BUTTON(3)) break;

     put_back_event();

     if (wp == &W1)
     { circle x;
       W1 >> x;
       W1.draw_disc(x,yellow);
       W1.draw_circle(x,black);
      }

     if (wp == &W2)
     { segment x;
       W2 >> x;
       W2 << x;
      }

     if (wp == &W3)
     { point x;
       W3 >> x;
       W3 << x;
      }

     if (wp == &W4)
     { line x;
       W4 >> x;
       W4 << x;
      }
    }

  return 0;
}
