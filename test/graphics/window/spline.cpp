/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  spline.c
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
  window W("Draw Splines");

  W.display();

  polygon P;


  while (W >> P)
  { W << P;
    W.clear();
    W.set_line_width(3);
    W.draw_spline(P,32,blue);
    W.set_line_width(1);
  }

  return 0;
}


    

    

