/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  buffering1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/leda_icon.xpm>

using namespace leda;

void move_ball(window& W, circle& ball, double& dx, double& dy, 
                                        char* prect)
{ 
  ball = ball.translate(dx,dy);

  point  c = ball.center();
  double r = ball.radius();

  if (c.xcoord()-r < W.xmin() || c.xcoord()+r > W.xmax()) dx = -dx;
  if (c.ycoord()-r < W.ymin() || c.ycoord()+r > W.ymax()) dy = -dy;

  W.clear();
  W.clip_mask_window(0);
  W.clip_mask_ellipse(c.xcoord(),c.ycoord(),r,r,1);
  W.center_pixrect(c.xcoord(),c.ycoord(),prect);
  W.reset_clipping();
  W.draw_circle(ball,black);
}


int main()
{ 
  window W(300,300, "Bouncing Leda");

  W.set_bg_color(grey1);
  W.display(window::center,window::center);

  circle ball(50,50,16);

  double dx = W.pix_to_real(2);
  double dy = W.pix_to_real(1);

  char* leda = W.create_pixrect(leda_icon);

  W.start_buffering();

  while (W.get_mouse() == NO_BUTTON)
  { move_ball(W,ball,dx,dy,leda);
    W.flush_buffer();
   }

  W.stop_buffering();

  W.del_pixrect(leda);

  W.screenshot("buffering1");
  return 0;
}
