/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  zoom.c
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


void GraphWin::zoom_area(double x0, double y0, double x1, double y1)
{  window& W = get_window();
 //graph&  G = get_graph();

  if (!W.is_open())
    LEDA_EXCEPTION(1,"GraphWin::zoom_area: window must be displayed first.");

   bool save_flush = set_flush(false);

   double wx0 = W.xmin();
   double wy0 = W.ymin();
   double wx1 = W.xmax();
   double wy1 = W.ymax();

   W.adjust_zoom_rect(x0,y0,x1,y1);

   if (x0 > x1) { double tmp = x0; x0 = x1; x1 = tmp; }
   if (y0 > y1) { double tmp = y0; y0 = y1; y1 = tmp; }

/*
   if (show_status) 
   { double f = (wy1-wy0)/(y1-y0);
     y0 -= f*W.pix_to_real(status_win_height);
    }
*/

   int anim = get_animation_steps() + 1;

   if (!save_flush) anim = 1;

   double dx0 = (x0 - wx0);
   double dy0 = (y0 - wy0);
   double dx1 = (x1 - wx1);
   double dy1 = (y1 - wy1);


   if (fabs((double)W.real_to_pix(dx0)) < 10 &&
       fabs((double)W.real_to_pix(dy0)) < 10 &&
       fabs((double)W.real_to_pix(dx1)) < 10){
       // nothing to do
       return;
   }

   dx0 /= anim;
   dy0 /= anim;
   dx1 /= anim;
   dy1 /= anim;

   double g_dist = get_grid_dist();

   char* bg_pr = W.get_bg_pixrect();

   if (zoom_show_grid && g_dist == 0 && bg_pr == 0 && W.real_to_pix(20) >= 8) 
      W.set_grid_dist(-20);

   if (zoom_hide_labels && zoom_objects)  hide_labels = true;

   W.start_buffering();

   while (anim--)
   { wx0 += dx0;
     wy0 += dy0;
     wx1 += dx1;
     wy1 += dy1;
     window_init(wx0,wx1,wy0);
     W.flush_buffer();
    }

   W.stop_buffering();

   update_win_bounds();

   if (zoom_show_grid && g_dist == 0) 
   { leda_wait(0.75);
     W.set_grid_dist(0);
    }

   hide_labels = false;

   redraw();
   set_flush(save_flush);
}



void GraphWin::unzoom() 
{ zoom_area(zoom_x0,zoom_y0,zoom_x1,zoom_y1); }  


void GraphWin::zoom(double f) 
{
  if (f == 0) // default window
  { double x0 = xmin_def;
    double x1 = xmax_def;
    double y0 = ymin_def;
    double ratio = (get_ymax() - get_ymin())/(get_xmax() - get_xmin());
    double y1 = y0 + ratio * (x1-x0);
    if (show_status) 
    { double f = (x1-x0)/(get_xmax()-get_xmin());
      y0 += f*win_p->pix_to_real(status_win_height);
     }
    zoom_area(x0,y0,x1,y1);
    return;
   }

  double xmin=get_xmin();
  double xmax=get_xmax();
  double ymin=get_ymin();
  double ymax=get_ymax();
  double xd=(xmax-xmin)*(1/f-1)/2;
  double yd=(ymax-ymin)*(1/f-1)/2;
  zoom_area(xmin-xd,ymin-yd,xmax+xd,ymax+yd);
}



void GraphWin::zoom_graph()
{
  graph&  G = get_graph();
  window& W = get_window();

  if (!W.is_open())
    LEDA_EXCEPTION(1,"GraphWin::zoom_graph: window has to be displayed.");

  if (G.number_of_nodes() == 0 && shape_list.empty()) return;

  double mb_height = W.pix_to_real(W.menu_bar_height());

  double bxmin;
  double bxmax;
  double bymin;
  double bymax;
  get_bounding_box(bxmin,bymin,bxmax,bymax);

  bymin -= mb_height/2;

  double fx = (x_max - x_min)/(bxmax - bxmin);
  double fy = (y_max - y_min)/(bymax - bymin);

  double xmargin = W.pix_to_real(W.width())/(15*fx);
  double ymargin = W.pix_to_real(W.height())/(15*fy);

  if (!zoom_objects)
  { double r1,r2;
    get_extreme_coords(bxmin,bymin,bxmax,bymax,r1,r2);
    xmargin += r1;
    ymargin += r2;
   }

  double x0 = bxmin-xmargin;
  double x1 = bxmax+xmargin;
  double y0 = bymin-ymargin;
  double y1 = bymax+ymargin;

  W.adjust_zoom_rect(x0,y0,x1,y1);

  zoom_area(x0,y0,x1,y1);
}




void GraphWin::zoom_to_point(point p, point q, double f)
{ double dx = q.xcoord() - p.xcoord();
  double dy = q.ycoord() - p.ycoord();
  double x0 = get_xmin() + dx;
  double x1 = get_xmax() + dx;
  double y0 = get_ymin() + dy;
  double y1 = get_ymax() + dy;
  dx = f*(x1-x0);
  dy = f*(y1-y0);
  zoom_area(x0+dx,y0+dy,x1-dx,y1-dy);
}

void GraphWin::zoom_to_point(point p)
{ double x = (get_xmax() + get_xmin())/2;
  double y = (get_ymax() + get_ymin())/2;
  zoom_to_point(point(x,y),p,1.0);
}

void GraphWin::zoom_up_to_point(point p)
{ double x = (get_xmax() + get_xmin())/2;
  double y = (get_ymax() + get_ymin())/2;
  zoom_to_point(point(x,y),p,zoom_factor);
}

void GraphWin::zoom_down_to_point(point p)
{ double x = (get_xmax() + get_xmin())/2;
  double y = (get_ymax() + get_ymin())/2;
  zoom_to_point(point(x,y),p,1/zoom_factor);
}

void GraphWin::center_graph()
{ double x = (get_xmax() + get_xmin())/2;
  double y = (get_ymax() + get_ymin())/2;
  double x0,y0,x1,y1;
  get_bounding_box(x0,y0,x1,y1);
  zoom_to_point(point(x,y),point((x0+x1)/2,(y0+y1)/2),1.0);
}

LEDA_END_NAMESPACE
