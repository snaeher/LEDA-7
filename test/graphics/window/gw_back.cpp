/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_back.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main(int argc, char** argv) 
{
  char* pm;

  window w;

  w.display();
  w.start_buffering();
  w.init(0,100,0);
  double x = 45;
  double y = 50;
  w.draw_disc(x,y,45,blue2);
  w.stop_buffering(pm);
  w.close();

  GraphWin gw;
  gw.set_bg_pixmap(pm);

  gw.set_directed(true);
  gw.set_node_shape(rectangle_node);
  gw.display();

  if (argc > 1) gw.read_gw(argv[1]);
  gw.edit();

  return 0;
}

