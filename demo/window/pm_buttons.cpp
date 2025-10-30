/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  pm_buttons.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/button32.h>

using namespace leda;


int main()
{ 
  panel P("Pixrect Buttons");
  P.buttons_per_line(10);
  P.set_button_space(10);
  //P.set_button_height(100);

  P.text_item("");

  for(int i=0; i < num_xpm_button32; i++) 
  { //if (i % 10 == 0) P.text_item(" ");
    //if (i % 10 == 0) P.vspace(10);
    char* pr = P.create_pixrect(xpm_button32[i]);
    P.button(pr,pr,name_xpm_button32[i],i);
   }
    
  P.open();
 
  P.screenshot("pm_buttons");
  return 0;
}
