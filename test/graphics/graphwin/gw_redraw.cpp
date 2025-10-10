/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_redraw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

void redraw(window* wp, double, double, double, double)
{ 
  wp->draw_rectangle(50,50,150,150,blue);
  wp->draw_rectangle(200,200,400,400,red);
  wp->draw_rectangle(50,200,150,400,green2);
  wp->draw_rectangle(200,50,400,150,orange);

  //GraphWin* gwp = wp->get_graphwin();
}


int main(int argc, char** argv) 
{
  GraphWin gw;
  gw.set_bg_redraw(redraw);
  gw.display();
  gw.edit();

  return 0;
}

