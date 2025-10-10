/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  rgb.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cin;
using std::endl;
#endif




int main()
{  
   window W(500,500);
   int n = 125;

   W.init(0,n,0);
   W.set_show_coordinates(false);
   W.display();

   int i = 0;
   for(int r = -1; r < 256; r+=64)
     for(int g = -1; g < 256; g+=64)
       for(int b = -1; b < 256; b+=64)
       { W.draw_filled_rectangle(i,0,i+1,n,color(r,g,b));
         cout << r << " " << g << " " << b << endl;
         i++;
        }

    W.read_mouse();

  return 0;
}
  
