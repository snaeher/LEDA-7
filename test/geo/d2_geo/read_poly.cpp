/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  read_poly.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <fstream.h>

using namespace leda;

using std::ifstream;


int main(int argc, char** argv)
{
  window W(500,300);
  W.init(-2.5,2.5,-1);
  W.display();

  if (argc < 2) return 1;

  ifstream in(argv[1]);

  polygon pol;
  while (in >> pol)
  { W.draw_polygon(pol,blue);
    segment s;
    forall_segments(s,pol) 
       if (W.real_to_pix(s.length()) > 8) W.draw_arrow(s,blue);
   }

   return W.read_mouse();
}
