/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  bitmap.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <LEDA/graphics/bitmaps/leda_icon.xbm>

using namespace leda;

int main()
{ 
  window W(400,400);
  W.set_bg_color(yellow);
  W.display();

  // construct bitmap from the bitmap data in 
  // <LEDA/bitmaps/leda_icon.xbm>

  char* bm = W.create_bitmap(leda_icon_width,
                             leda_icon_height, 
                             leda_icon_bits);

  // copy copies of bm into the window

  point p;
  while (W >> p) W.put_bitmap(p.xcoord(),p.ycoord(),bm,blue);

  W.del_bitmap(bm);

  W.screenshot("bitmap");
  return 0;
}
