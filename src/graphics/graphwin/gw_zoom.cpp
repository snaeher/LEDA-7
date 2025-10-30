/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_zoom.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include "local.h"

LEDA_BEGIN_NAMESPACE


void gw_center_graph(GraphWin& gw)   
{ bool b = gw.get_flush();
  gw.center_graph(); 
  gw.set_flush(b);
}


void gw_zoom_graph(GraphWin& gw)   
{ bool b = gw.get_flush();
  gw.zoom_show_grid = true;
  gw.zoom_graph(); 
  gw.zoom_show_grid = false;
  gw.set_flush(b);
}


void gw_place_into_box(GraphWin& gw)   
{ window& W = gw.get_window();
  bool b = gw.get_flush();
  double x0,y0,x1,y1;

  W.set_cursor(XC_dotbox);

  W.set_show_coord_handler(nil);
  W.set_status_string("\\bf ~Place into Box:\\rm ~~~~Click on\\blue lower left\
                       \\black and drag to\\blue upper right\\black corner.");

  int but = W.read_mouse(x0,y0);

  if (but != MOUSE_BUTTON(3))
  { gw.read_mouse_rect(x0,y0,x1,y1);
    double d = W.pix_to_real(32);
    gw.place_into_box(x0+d,y0+d,x1-d,y1-d);
    gw.set_flush(b);
    gw.redraw();
  }
  W.set_cursor(-1); 
  W.set_show_coord_handler(GraphWin::coord_handler);
  gw.update_status_line();
}


void gw_place_into_win(GraphWin& gw)   
{ window& W = gw.get_window();
  bool b = gw.get_flush();
  double x0 = gw.get_xmin();
  double y0 = gw.get_ymin();
  //double y0 = gw.get_ymin() + W.pix_to_real(gw.status_win_height);
  double x1 = gw.get_xmax();
  double y1 = gw.get_ymax();
  double d = W.pix_to_real(gw.window_width()/13);
  gw.place_into_box(x0+d,y0+d,x1-d,y1-d);
  gw.set_flush(b);
  gw.redraw();
}


void gw_shrink_graph(GraphWin& gw)   
{ double f = 1/gw.get_zoom_factor();
  double x0,y0,x1,y1;
  gw.get_bounding_box(x0,y0,x1,y1);
  double xc = (x0+x1)/2;
  double yc = (y0+y1)/2;
  x0 = xc + f*(x0-xc);
  x1 = xc + f*(x1-xc);
  y0 = yc + f*(y0-yc);
  y1 = yc + f*(y1-yc);
  gw.place_into_box(x0,y0,x1,y1);
}

void gw_expand_graph(GraphWin& gw)   
{ double f = gw.get_zoom_factor();
  double x0,y0,x1,y1;
  gw.get_bounding_box(x0,y0,x1,y1);
  double xc = (x0+x1)/2;
  double yc = (y0+y1)/2;
  x0 = xc + f*(x0-xc);
  x1 = xc + f*(x1-xc);
  y0 = yc + f*(y0-yc);
  y1 = yc + f*(y1-yc);
  gw.place_into_box(x0,y0,x1,y1);
}


void gw_unzoom(GraphWin& gw)   
{ bool b = gw.get_flush();
  gw.zoom_show_grid = true;
  gw.unzoom(); 
  gw.zoom_show_grid = false;
  gw.set_flush(b);
}

void gw_zoom_up(GraphWin& gw)   
{ double f = gw.get_zoom_factor();
  bool b = gw.get_flush();
  gw.zoom_show_grid = true;
  gw.zoom(f); 
  gw.zoom_show_grid = false;
  gw.set_flush(b);
}

void gw_zoom_down(GraphWin& gw)
{ double f = 1/gw.get_zoom_factor();
  bool b = gw.get_flush();
  gw.zoom_show_grid = true;
  gw.zoom(f); 
  gw.zoom_show_grid = false;
  gw.set_flush(b);
}

void gw_zoom_up_step(GraphWin& gw, const point&)
{ int s = gw.set_animation_steps(0);
  gw.zoom_show_grid = false;
  gw.zoom(1.1);
  gw.set_animation_steps(s);
}

void gw_zoom_down_step(GraphWin& gw, const point&)
{ int s = gw.set_animation_steps(0);
  gw.zoom_show_grid = false;
  gw.zoom(0.9);
  gw.set_animation_steps(s);
}


void gw_zoom_up_to_point(GraphWin& gw, const point& p)
{ gw.zoom_show_grid = true;
  gw.zoom_up_to_point(p);
  gw.zoom_show_grid = false;
}

void gw_zoom_down_to_point(GraphWin& gw, const point& p)
{ gw.zoom_show_grid = true;
  gw.zoom_down_to_point(p);
  gw.zoom_show_grid = false;
}





void gw_zoom_area(GraphWin& gw) 
{ window& W = gw.get_window();

  point p1,p2;
  if (W.read_zoom_rect(p1,p2))
    { gw.zoom_show_grid = true;
      gw.zoom_area(p1.xcoord(),p1.ycoord(),p2.xcoord(),p2.ycoord());
      gw.zoom_show_grid = false;
     }
  else
     gw.redraw();
}

void gw_zoom_area(GraphWin& gw, const point&) { gw_zoom_area(gw); }



void gw_zoom_default(GraphWin& gw) 
{ 
  double xmin,xmax,ymin;
  gw.get_default_win_coords(xmin,xmax,ymin);

  panel P;

  P.text_item("\\bf\\blue Default Window Coordinates");
  P.text_item("");
  P.double_item("xmin",xmin);
  P.double_item("xmax",xmax);
  P.double_item("ymin",ymin);

  P.button("apply",   3);
  P.button("current", 2);
  P.button("reset",1);
  P.button("cancel",  0);

  P.buttons_per_line(4);

  int but = 0;

  do 
  { but = gw.open_panel(P);
    if (but == 1)
    { xmin = DefXMin;
      xmax = DefXMax;
      ymin = DefYMin;
     }
    if (but == 2)
    { xmin = gw.get_xmin();
      xmax = gw.get_xmax();
      ymin = gw.get_ymin();
     }
   } while (but == 1  || but == 2);


  if (but == 3) 
  { gw.set_default_win_coords(xmin,xmax,ymin);
    gw.zoom(0); 
   }

}


LEDA_END_NAMESPACE

