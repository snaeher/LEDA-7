/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  defcolor.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>

using namespace leda;

//static color clr = blue2;
static color clr = green2;

void redraw(window* wp)
{ 
  window& W = *wp;

  W.start_buffering();

  W.set_bg_color(clr);
  W.clear();

  int sz = W.width()/7;
  W.set_font(string("F%02d",sz));

  W.set_color(clr.text_color());

  //W.draw_ctext(clr.get_string());

  double tw = W.text_width("#000000");
  double th = W.text_height("#000000");

  double x = (W.xmax() - tw)/2;
  double y = W.ymax() - (W.ymax() - th)/2;
  W.draw_text(x,y,clr.get_string());


  //cout << clr.get_string() << endl;

  W.flush_buffer();
  W.stop_buffering();

}

void update_red(int x)   
{ clr.set_red(x);
  redraw(window::get_call_window());
}

void update_green(int x)   
{ clr.set_green(x);
  redraw(window::get_call_window());
}

void update_blue(int x)   
{ clr.set_blue(x);
  redraw(window::get_call_window());
}


int main()
{
  int dpi = window::dpi();

  //window W("Define Color");
  //window W(5*dpi,5*dpi,"Define Color");
  window W(4*dpi,4*dpi,"Define Color");

  int r,g,b;
  clr.get_rgb(r,g,b);

  W.text_item("\\bf RGB Values");
  W.int_item("Red   ",r,0,255,update_red);
  W.int_item("Green ",g,0,255,update_green);
  W.int_item("Blue  ",b,0,255,update_blue);

  W.set_redraw(redraw);
  W.set_bg_color(clr);

  W.display(window::center,window::center);
  redraw(&W);

  W.read_mouse();

/*
  W.screenshot("defcolor");
*/
  return 0;
}
