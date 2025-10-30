/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  bouncing_leda.c
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

void noise(double& dx, double& dy)
{
   double spd = dx*dx + dy*dy;

   dx += 0.0001*rand_int(-100,+100);
 
   if (dx*dx > spd) return;

   if (dy > 0)
     dy = +sqrt(spd - dx*dx);
   else
     dy = -sqrt(spd - dx*dx);

  //cout << spd << "  (" << dx << "," << dy << ")" << endl;

}


double move_ball(window& W, circle& ball, double& dx, double& dy, 
                                        char* prect)
{ 
  ball = ball.translate(dx,dy);

  point  c = ball.center();
  double r = ball.radius();

/*
  double cx = c.xcoord() + dx;
  double cy = c.ycoord() + dy;

  cx = W.pix_to_real(W.real_to_pix(cx));
  cy = W.pix_to_real(W.real_to_pix(cy));

  c = point(cx,cy);

  ball = circle(c,r);
*/


  double r1=r, r2=r; // horizontal, vertical radii
  double C = W.pix_to_real(12);
  
  if (c.xcoord()-r +C < W.xmin() || c.xcoord()+r -C> W.xmax()) {
        // bounce
	dx = -dx;
        noise(dx,dy);
  }

  if (c.ycoord()-r +C < W.ymin() || c.ycoord()+r -C> W.ymax()) {
        // bounce
	dy = -dy;
        noise(dx,dy);
  }

  if (c.xcoord()-r  < W.xmin() ) {
        r1 = fabs(c.xcoord() - W.xmin());
        r2 = r + r - r1;
  }

  if (c.xcoord()+r > W.xmax()) {
        r1 = fabs(c.xcoord()  - W.xmax());
        r2 = r + r - r1;
  }           

  if (c.ycoord()-r < W.ymin() ) {
        r2 = fabs(c.ycoord()  -W.ymin());
	r1 = r + r - r2;
  }       

  if ( c.ycoord()+r > W.ymax()) {
        r2 = fabs(c.ycoord()  -W.ymax());
        r1 = r + r - r2;
  }       

  W.clear();
 
  W.clip_mask_window(0);
  W.clip_mask_ellipse(c.xcoord(),c.ycoord(),r1,r2,1);
  W.center_pixrect(c.xcoord(),c.ycoord(),prect);
  W.clip_mask_window(1);

  W.draw_ellipse(c, r1, r2, black);

  return 0;
}


int main()
{ 
  window W(1000,1000, "Bouncing Leda");

  W.init(0,200,0);
  W.set_bg_color(grey1);
  W.display(window::center,window::center);

  double r = W.pix_to_real(80);

  circle ball(70,110,r);


  double dx = W.pix_to_real(4);
  double dy = W.pix_to_real(3);

  if (W.display_type() == "xx") {
    dx = W.pix_to_real(7);
    dy = W.pix_to_real(5);
  }

  char* leda = W.create_pixrect(leda_icon);

  W.start_buffering();

  for(;;)
  { move_ball(W,ball,dx,dy,leda);

    point  c = ball.center();
    //double r = 1.5*ball.radius();
    double r = 1.3*ball.radius();
    r = W.pix_to_real(W.real_to_pix(r));
    double x0 = c.xcoord() - r;
    double y0 = c.ycoord() - r;
    double x1 = c.xcoord() + r;
    double y1 = c.ycoord() + r;

    W.flush_buffer(x0,y0,x1,y1);

    int val = 0;
    double x,y;
    unsigned long t = 0;

    int timeout = 7;
/*
    if (W.display_type() == "xx") timeout = 1;
*/
    if (W.read_event(val,x,y,t,timeout)== button_press_event) break;
   }

  W.stop_buffering();

  W.del_pixrect(leda);

  return 0;
}
