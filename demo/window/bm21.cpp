/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  bm21.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <LEDA/graphics/bitmaps/button21.h>

using namespace leda;

int main()
{ 
  panel P("Bitmap Buttons");
  P.buttons_per_line(8);
  P.set_button_space(3);

  for(int i=0; i < num_button21; i++) 
    P.button(21,21,bits_button21[i],string(name_button21[i]));
  
  int button = P.open();
 
  P.screenshot("bm_buttons");
  return 0;
}

