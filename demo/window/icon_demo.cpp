/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  icon_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/earth.xpm>
#include <LEDA/graphics/pixmaps/tintin.xpm>

#include <time.h>


using namespace leda;



static void display_time(window* wp)
{ wp->set_mode(src_mode);
  wp->set_color(white);
  time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 
  wp->clear();
  wp->draw_ctext(string("%2d:%02d:%02d",T->tm_hour,T->tm_min,T->tm_sec));
}


void redraw(window* wp)
{ //wp->set_bold_font();
  wp->set_font("B64");
  wp->draw_ctext("Iconify this window !"); 
}

int main()
{

  window W(800,400);

/*
  char* earth = W.create_pixrect(earth_xpm);
  int w = W.get_pixrect_width(earth);
  int h = W.get_pixrect_height(earth);

  // construct icon window

  window icon(w,h);
  icon.init(0,100,0);
  icon.set_bg_pixrect(earth);
  icon.set_font("lucidasans-bold-16");
  icon.start_timer(1000,display_time);
  W.set_icon_window(icon);
*/


  char* pm = W.create_pixrect(tintin_xpm);

  W.set_icon_pixrect(pm);
  W.set_frame_label("LEDA");
  W.set_redraw(redraw);
  W.display(window::center,window::center);
  redraw(&W);

  W.read_mouse();

  return 0;
}

