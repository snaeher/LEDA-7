/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  bm_buttons.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <LEDA/graphics/bitmaps/button32.h>

using namespace leda;

int main()
{ 
  panel P("Bitmap Buttons");
  P.buttons_per_line(8);
  P.set_button_space(3);

  for(int i=0; i < num_xbm_button32; i++) 
    P.button(32,32,xbm_button32[i],string(name_xbm_button32[i]));
  
  P.open();
 
  P.screenshot("bm_buttons");
  return 0;
}

