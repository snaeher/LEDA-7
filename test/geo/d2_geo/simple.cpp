/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  simple.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/polygon.h>
#include <LEDA/graphics/window.h>

using namespace leda;




int main()
{

   window W("general polygon test");
   W.init(0,1000,0,25);
   W.display();

   list<point> L = W.read_polygon();

   W.draw_polygon(L);

   polygon poly(L,polygon::NO_CHECK);

   list<polygon> poly_list = poly.simple_parts();

   int p_count = 0;
   int h_count = 0;

   polygon p;
   forall(p,poly_list)
   { if (p.area() > 0)
      { p_count++;
        W.draw_filled_polygon(p,color(p_count+2));
       }
     else
      { h_count++;
        W.draw_filled_polygon(p,white);
       }
     W.draw_polygon(p,black);
    }

   W.message(string("polygons: %2d  holes: %2d",p_count,h_count));

   W.read_mouse();

   return 0;
}
