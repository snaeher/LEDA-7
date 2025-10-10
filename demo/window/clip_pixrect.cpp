/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  clip_pixrect.c
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

void draw_pix_circle(window& W, const circle& C, char* prect)
{ 
  point  p = C.center();
  double x = p.xcoord();
  double y = p.ycoord();

  double r = C.radius();

  W.draw_disc(C,black); 

  W.clip_mask_window(0);
  W.clip_mask_ellipse(x,y,r,r,1);
  W.center_pixrect(x,y,prect);
  W.reset_clipping();
}

int main()
{
   window W(400,400, "Clipping a Pixmap");
   W.display();

   // create a pixrect using LEDA's xpm icon

   char* leda_pix = W.create_pixrect(leda_icon);

   circle c;
   while (W >> c) draw_pix_circle(W,c,leda_pix);

   W.del_pixrect(leda_pix);

   W.screenshot("clip_pixrect");
   return 0;
}
