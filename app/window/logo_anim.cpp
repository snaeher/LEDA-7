/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  logo_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;

static char* window_pr = 0;

static int  Distance    = 46;
static int  Thickness   = 46;


static color color_L    = blue2;
static color color_E    = yellow;
static color color_D    = green2;  // color(0,196,128)
static color color_A    = orange;

static color bg_clr   = 0xdddddd;
static color text_clr = 0x555555;
static color line_clr = 0x333333;

static bool frame = true;

static double anim_d = 0.0001;


void anim_arc_arrows(window& W, point a1, point b1, point c1, 
                                point a2, point b2, point c2, color col)
{
  circle C1(a1,b1,c1);
  circle C2(a2,b2,c2);

  point  m = C1.center();
  double phi1 = 2*LEDA_PI - m.angle(a1,c1);
  double phi2 = 2*LEDA_PI - m.angle(a1,c2);

  double adir1 = segment(m,a1).direction();
  double adir2 = segment(m,a2).direction();

  double phi_d = anim_d/C1.radius();

  for(double d = 0.02; d < phi1; d += phi_d)
  { point bb = C1.point_on_circle(adir1-d/2);
    point cc = C1.point_on_circle(adir1-d);
    W.draw_arc(a1,bb,cc,col);
    if (d+anim_d <= phi2)
    { bb = C2.point_on_circle(adir2-d/2);
      cc = C2.point_on_circle(adir2-d);
      W.draw_arc(a2,bb,cc,col);
     }
    W.flush_buffer(-2.3,-0.5,2.3,1.3);
    //W.get_mouse(); // allow interaction
  }
  W.draw_arc_arrow(a1,b1,c1,col);
  W.draw_arc_arrow(a2,b2,c2,col);
  W.flush_buffer();
}



void draw_varrows(window& W, int n, double* x, color c, bool anim = false) 
{ double ymin = -0.4;
  double ymax =  1.6;
  if (anim)
  { for(double y = ymin; y <= ymax; y += anim_d)
    { for (int i=0; i<n; i++) W.draw_segment(x[i],ymin,x[i],y,c);
      W.flush_buffer(-1.5,-0.6,1.5,1.6);
      //W.get_mouse(); // allow interaction
     }
   }
  for (int i=0; i<n; i++) W.draw_arrow(x[i],ymin,x[i],ymax,c);
  if (anim) W.flush_buffer();
}


void draw_harrows(window& W, int n, double* y, color c, bool anim = false) 
{ if (anim)
  { for(double d = -2.5; d <= 2.5; d += anim_d)
    { for (int i=0; i<n; i++) W.draw_segment(-2.5,y[i],d,y[i],c);
      W.flush_buffer(-2.5,-0.1,2.5,0.75);
      //W.get_mouse(); // allow interaction
     }
   }
  for (int i=0; i<n; i++) W.draw_arrow(-2.5,y[i],2.5,y[i],c);
  if (anim) W.flush_buffer();
}


void draw_circle_arrows(window& W, circle C1, circle C2, double y, color col, bool anim = false)
{ line l(point(0,y),point(1,y));
  list<point> I1 = C1.intersection(l);
  point a1 = I1.pop();
  point b1 = C1.point_on_circle(LEDA_PI/2);
  point c1 = I1.pop();
  list<point> I2 = C2.intersection(l);
  point a2 = I2.pop();
  point b2 = C2.point_on_circle(LEDA_PI/2);
  point c2 = I2.pop();
  if (anim)
     anim_arc_arrows(W,a1,b1,c1,a2,b2,c2,col);
  else
     { W.draw_arc_arrow(a1,b1,c1,col);
       W.draw_arc_arrow(a2,b2,c2,col);
      }
}


void draw_circles(window& W, bool anim=false)
{
  double d = 0.004*Thickness;

  circle c1(0,-1,sqrt(5.0));
  circle c2(0,-1,sqrt(5.0)-d);
  circle c3(0,0,1);
  circle c4(0,0,1-d);

/*
  draw_circle_arrows(W,c1,c2,-0.5,line_clr,anim);
  draw_circle_arrows(W,c3,c4,-0.3,line_clr,anim);
*/

  draw_circle_arrows(W,c1,c2,-0.4,line_clr,anim);
  if (anim) sleep(0.25);

  draw_circle_arrows(W,c3,c4,-0.4,line_clr,anim);
  if (anim) sleep(0.25);
}


void draw_lines(window& W, bool anim=false)
{ 
  double d = 0.004*Thickness;
  double eps = W.pix_to_real(int(0.3*Distance));

  double x[6], y[4];

  x[0] = -1-2*eps;
  x[1] = -eps;
  x[2] = eps;
  x[3] = d+eps;
  x[4] = 1+2*eps;
  x[5] = 1+2*eps+d;
 
  y[0] = 0;
  y[1] = d;
  y[2] = 2*d;
  y[3] = 3*d;

  draw_harrows(W,4,y,line_clr,anim);
  if (anim) sleep(0.25);

  draw_varrows(W,6,x,line_clr,anim);
  if (anim) sleep(0.25);
}


void clip_circle(window& W, circle c, int mode)
{ double x = c.center().xcoord();
  double y = c.center().ycoord();
  double r = c.radius();
  W.clip_mask_ellipse(x,y,r,r,mode);
}



void draw_box(window& W, double x0, double y0, double x1, double y1, 
                                    color col1, int i, int steps)
{
   color col0 = W.get_bg_color();

   int r0,g0,b0;
   col0.get_rgb(r0,g0,b0);
  
   int r1,g1,b1;
   col1.get_rgb(r1,g1,b1);

   float rd = float(r1-r0)/steps;
   float gd = float(g1-g0)/steps;
   float bd = float(b1-b0)/steps;

   color col(int(r0+i*rd),int(g0+i*gd),int(b0+i*bd));

   W.draw_box(x0,y0,x1,y1,col);
   draw_circles(W);
   draw_lines(W);

   W.flush_buffer(x0,y0,x1,y1);
}


void draw_text(window& W, 
               double x0, double y0, string txt, color col1, bool anim=false)
{
   if (anim == false)
   { W.draw_ctext(x0,y0,txt,col1);
     return;
    }

   int steps = int(1.0/anim_d);

   color col0 = W.get_bg_color();

   int r0,g0,b0;
   col0.get_rgb(r0,g0,b0);
  
   int r1,g1,b1;
   col1.get_rgb(r1,g1,b1);

   color col(1,1,1);
  
   float rd = float(r1-r0)/steps;
   float gd = float(g1-g0)/steps;
   float bd = float(b1-b0)/steps;
  
   for(int i=0; i< steps; i++)
   { col.set_rgb(int(r0+i*rd),int(g0+i*gd),int(b0+i*bd));
     W.draw_ctext(x0,y0,txt,col);
     W.flush_buffer();
     //W.flush_buffer(x0-2.50, W.ymax()-1, x0+2.50, W.ymax());
   }
}





void fill_logo(window& W, bool anim=false )
{
   double d = 0.004*Thickness;
   double eps = W.pix_to_real(int(0.3*Distance));
   double eps2 = 2*eps;
   //double eps2 = 1.75*eps;

   circle c1(0,-1,sqrt(5.0));
   circle c2(0,-1,sqrt(5.0)-d);
   circle c3(0,0,1);
   circle c4(0,0,1-d);

   W.clip_mask_rectangle(W.xmin(),W.ymin(),W.xmax(),W.ymax(),0);

   clip_circle(W,c1,1);
   clip_circle(W,c2,0);

   W.clip_mask_rectangle(-1-eps2,-4,1+eps2,4,0);

   clip_circle(W,c3,1);
   clip_circle(W,c4,0);


   // L
   W.clip_mask_rectangle(-2+d,0,-1-eps2,d,1);

   // E
   W.clip_mask_rectangle(-1+1.25*d,2*d,-eps,3*d,1);
   W.clip_mask_rectangle(-1+d,0,-eps,d,1);

   // D
   W.clip_mask_rectangle(eps,0,eps+d,1-d,1);
   W.clip_mask_rectangle(d+eps,0,1-d/2,d,1);

   // A
   W.clip_mask_rectangle(1+d+eps2,d,2-1.5*d,2*d,1);
   W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1-1.25*d,1);


   W.clip_mask_rectangle(-4,-4,4,0,0);
   W.clip_mask_rectangle(-eps,-4,eps,4,0);

   //draw_box(W,-2-eps,0,2,1,color_L,anim);

   if (anim)
   { 
     //int steps = int(1.5/anim_d);
     //if (window::display_type() == "xx") steps = int(8.5/anim_d);

     int steps = int(0.1/anim_d);
     if (window::display_type() == "xx") steps = int(15.0/anim_d);

     for(int i=0; i <= steps; i++)
     { draw_box(W,-2-eps,0,-1,1,color_L,i,steps);
       draw_box(W,-1-eps,0, 0,1,color_E,i,steps);
       draw_box(W, 0+eps,0, 1,1,color_D,i,steps);
       draw_box(W, 1+eps,0, 2,1,color_A,i,steps);
      }
   }
   else
   { draw_box(W,-2-eps,0,-1,1,color_L,1,1);
     draw_box(W,-1-eps,0, 0,1,color_E,1,1);
     draw_box(W, 0+eps,0, 1,1,color_D,1,1);
     draw_box(W, 1+eps,0, 2,1,color_A,1,1);
    }


   W.clip_mask_window(1);
}


void draw_title(window& W, bool anim=false)
{ 
  //float dpi = window::screen_dpi();
  //W.set_font(string("T%d",W.real_to_pix(0.25*192/dpi)));
  //W.set_font(string("T%d",int(64*dpi/192)));

  W.set_font("T64");
  draw_text(W,0,2.15,"Algorithmic  Solutions",text_clr,anim);

  //W.set_font(string("T%d",W.real_to_pix(0.20*192/dpi)));
  //W.set_font(string("T%d",int(48*dpi/192)));

  W.set_font("T48");
  draw_text(W,0,1.9,"presents",text_clr,anim);
}

void draw_copyright(window& W)
{ 
  //W.set_font(string("I%d",W.real_to_pix(0.085));
  W.set_font("I30");

  string text = "LEDA Logo  (sn 1995)";

  double xt = W.xmin() + W.pix_to_real(10);
  double yt = W.ymin() + W.text_height(text) + W.pix_to_real(10);
  W.draw_text(xt,yt,text,0x333333);
  W.flush_buffer();
}



void redraw(window* wp)
{ window& W = *wp;

  W.start_buffering();
  W.clear();

  if (frame) draw_title(W);
  fill_logo(W);
  draw_circles(W);
  draw_lines(W);
  draw_copyright(W);

  W.flush_buffer();
  W.stop_buffering();
}


void setup(window& W, int)
{ 
  panel P("LOGO");
  P.color_item("L-color",color_L);
  P.color_item("E-color",color_E);
  P.color_item("D-color",color_D);
  P.color_item("A-color",color_A);
  P.color_item("background",bg_clr);
  P.color_item("lines",line_clr);

  P.int_item("Thickness",Thickness,0,100);
  P.int_item("Distance",Distance,0,100);

  P.button("ok",0);
  P.button("quit",2);

  bool cont = (P.open() == 0);

  W.set_bg_color(bg_clr);

  redraw(&W);
}



int main()
{
  leda::copyright_window_string = "";

  if (window::display_type() == "xx") anim_d = 0.02;

  int w = 8*window::screen_dpi();
  int wmax = int(0.9*window::screen_width());
  if (w > wmax) w = wmax;
  int h = int(0.55*w);

  if (getenv("LEDA_OPEN_MAXIMIZED")) {
    w = window::screen_width();
    h = window::screen_height();
    frame = false;
  }

  window W(w,h,version_string);

  W.enable_close_button(true);

  W.set_line_width(1);
  W.set_bg_color(bg_clr);
  W.set_clear_on_resize(false);

W.set_text_mode(opaque);

  W.init(-3.0,3.0,-0.9);

  if (frame)
    W.display(window::center,window::center);
  else
    W.display(W,window::center,window::center);

  W.set_redraw(redraw);

  int but = 0;

  while (but != CLOSE_BUTTON)
  {
    if (but == MOUSE_BUTTON(3)) {
       //break;
       setup(W,0);
    }

    W.start_buffering();
    W.clear();
    W.flush_buffer();
  
    if (frame) draw_title(W,true);

    draw_lines(W,true);
    draw_circles(W,true);
    fill_logo(W,true);
    draw_copyright(W);
  
    W.stop_buffering();

    do { but = W.read_mouse();
    } while (but == MOUSE_BUTTON(4) || but == MOUSE_BUTTON(5));
  }


  return 0;
}
