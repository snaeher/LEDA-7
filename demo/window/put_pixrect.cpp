/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  put_pixrect.c
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



int main()
{ 
  window W(400,400);
  W.display();

  char* pr = W.create_pixrect(leda_icon);

  point p;
  while (W >> p) W.put_pixrect(p,pr);

  W.del_pixrect(pr);

  W.screenshot("put_pixrect");
  return 0;
}
