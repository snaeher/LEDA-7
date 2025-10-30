/*******************************************************************************
+
+  LEDA 7.2.2  
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


#if defined(__win32__)
static window W(120,90,"Algorithmic Solutions GmbH");
#else
static window W(window::screen_width()-10, window::screen_height()-26,
                                        "Algorithmic Solutions GmbH");
#endif

static char* window_pr = 0;

static bool cont = true;


static int  Distance    = 50;
static int  Thickness   = 45;

static color color_L    = blue2;
static color color_E    = yellow;
static color color_D    = color(0,196,128); //green2;
static color color_A    = orange;

/*
static color color_L  = blue2;
static color color_E  = blue2;
static color color_D  = blue2;
static color color_A  = blue2;
*/

static color text_col   = black;

static color bg_col     = grey2;  // grey3;

static color hline_col  = black;
static color vline_col1 = black;
static color circle_col = black;


//const double step = 0.0025;
const double step = 0.004;


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

  for(double d = 0.02; d < phi1; d += step)
  { point bb = C1.point_on_circle(adir1-d/2);
    point cc = C1.point_on_circle(adir1-d);
    W.draw_arc(a1,bb,cc,col);
    if (d+step <= phi2)
    { bb = C2.point_on_circle(adir2-d/2);
      cc = C2.point_on_circle(adir2-d);
      W.draw_arc(a2,bb,cc,col);
     }
    W.flush_buffer(-2.3,-0.5,2.3,1.3);
  }
  W.draw_arc_arrow(a1,b1,c1,col);
  W.draw_arc_arrow(a2,b2,c2,col);
  W.flush_buffer();
}



void draw_varrows(int n, double* x, color c, bool anim = false) 
{ if (anim)
  { for(double d = -0.6; d <= 1.6; d += step)
    { for (int i=0; i<n; i++) W.draw_segment(x[i],-0.6,x[i],d,c);
      W.flush_buffer(-1.5,-0.6,1.5,1.6);
     }
   }
  for (int i=0; i<n; i++) W.draw_arrow(x[i],-0.6,x[i],1.6,c);
  if (anim) W.flush_buffer();
}


void draw_harrows(int n, double* y, color c, bool anim = false) 
{ if (anim)
  { for(double d = -2.5; d <= 2.5; d += step)
    { for (int i=0; i<n; i++) W.draw_segment(-2.5,y[i],d,y[i],c);
      W.flush_buffer(-2.5,-0.1,2.5,0.75);
     }
   }
  for (int i=0; i<n; i++) W.draw_arrow(-2.5,y[i],2.5,y[i],c);
  if (anim) W.flush_buffer();
}


void draw_circle_arrows(circle C1, circle C2, double y, color col, bool anim = false)
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


void draw_circles(bool anim=false)
{
  double d = 0.004*Thickness;

  circle c1(0,-1,sqrt(5.0));
  circle c2(0,-1,sqrt(5.0)-d);
  circle c3(0,0,1);
  circle c4(0,0,1-d);

  draw_circle_arrows(c1,c2,-0.5,circle_col,anim);
  draw_circle_arrows(c3,c4,-0.3,circle_col,anim);
}


void draw_lines(bool anim=false)
{ 
  double d = 0.004*Thickness;
  double eps = W.pix_to_real(int(0.2*Distance));

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

  draw_harrows(4,y,hline_col,anim);
  draw_varrows(6,x,vline_col1,anim);
}


void clip_circle(circle c, int mode)
{ double x = c.center().xcoord();
  double y = c.center().ycoord();
  double r = c.radius();
  W.clip_mask_ellipse(x,y,r,r,mode);
}



void draw_box(double x0, double y0, double x1, double y1, 
                                    color col1, bool anim=false)
{
   color col0 = W.get_bg_color();

#if defined(__win32__)
   //int steps = 256;
   int steps = 1024;
#else
   //int steps = 128;
   int steps = 1024;
#endif


   if (anim)
   { int r0,g0,b0;
     col0.get_rgb(r0,g0,b0);
  
     int r1,g1,b1;
     col1.get_rgb(r1,g1,b1);

     color col(1,1,1);
  
     float rd = float(r1-r0)/steps;
     float gd = float(g1-g0)/steps;
     float bd = float(b1-b0)/steps;
  
     for(int i=0; i< steps; i++)
     { col.set_rgb(int(r0+i*rd),int(g0+i*gd),int(b0+i*bd));
       //if (col.is_good())
        { W.draw_box(x0,y0,x1,y1,col);
          draw_circles();
          draw_lines();
          W.flush_buffer(x0,y0,x1,y1);
         }
/*
       else
         { W.draw_box(x0,y0,x1,y1,col1);
           draw_circles();
           draw_lines();
           W.flush_buffer(x0,y0,x1,y1);
           leda_wait(1);
           break;
          }
*/
     }
   }
    
   W.draw_box(x0,y0,x1,y1,col1);
   if (anim)
   { draw_circles();
     draw_lines();
     W.flush_buffer(); 
    }
}


void draw_text(double x0, double y0, string txt, color col1, bool anim=false)
{
   if (anim == false)
   { W.draw_ctext(x0,y0,txt,col1);
     return;
    }

   int steps = 64;

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
     if (window_pr)
        W.put_pixrect(window_pr);
     else
        W.clear();
     W.draw_ctext(x0,y0,txt,col);
     double tw = 0.60 * W.text_width(txt);
     double th = 0.75 * W.text_height(txt);
     W.flush_buffer(x0-tw,y0-th,x0+tw,y0+th);
   }
}





void fill_logo(bool anim=false )
{
   double d = 0.004*Thickness;
   double eps = W.pix_to_real(int(0.2*Distance));
   double eps2 = 2*eps;

   circle c1(0,-1,sqrt(5.0));
   circle c2(0,-1,sqrt(5.0)-d);
   circle c3(0,0,1);
   circle c4(0,0,1-d);

   W.clip_mask_rectangle(W.xmin(),W.ymin(),W.xmax(),W.ymax(),0);

   clip_circle(c1,1);
   clip_circle(c2,0);

   W.clip_mask_rectangle(-1-eps2,-4,1+eps2,4,0);

   clip_circle(c3,1);
   clip_circle(c4,0);


   // L
   W.clip_mask_rectangle(-2+d,0,-1-eps2,d,1);

   // E
   //W.clip_mask_rectangle(-1+1.25*d,2*d,-eps,3*d,1);
   W.clip_mask_rectangle(-1+1.25*d,3*d,-eps,2*d,1);
   W.clip_mask_rectangle(-1+d,0,-eps,d,1);

   // D
   W.clip_mask_rectangle(eps,0,eps+d,1-d,1);
   W.clip_mask_rectangle(d+eps,0,1-d/2,d,1);

   // A
   W.clip_mask_rectangle(1+d+eps2,d,2-1.5*d,2*d,1);
   W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1-1.25*d,1);


   W.clip_mask_rectangle(-4,-4,4,0,0);
   W.clip_mask_rectangle(-eps,-4,eps,4,0);

   //draw_box(-2-eps,0,-1,1,color_L,anim);
   //draw_box(-1-eps,0, 0,1,color_E,anim);
   //draw_box( 0+eps,0, 1,1,color_D,anim);
   //draw_box( 1+eps,0, 2,1,color_A,anim);

   draw_box(-2-eps,0,2,1,color_L,anim);

   W.clip_mask_window(1);
}


void draw_title(bool anim=false)
{
#if defined(__win32__)
  //W.set_font("A200");
  W.set_font("A120");
#else
  W.set_font("T80");
#endif
  draw_text(0,2.15,"Algorithmic  Solutions",black,anim);
  W.del_pixrect(window_pr);
  window_pr = W.get_window_pixrect();
  W.set_font("I40");
//draw_text(0,1.8,"presents",black,anim);
  W.del_pixrect(window_pr);
  window_pr = W.get_window_pixrect();
}


void redraw()
{
  if (window_pr)
  { W.put_pixrect(window_pr);
    return;
   }

  W.start_buffering();
  W.clear();

  draw_title();
  fill_logo();
  draw_circles();
  draw_lines();
  draw_text(0,-0.80,"Version 6.0",text_col);
  
  W.set_font(string("I%d",W.real_to_pix(0.075)));
  string text = "by ullfan 1995";
  double xt = W.xmax() - W.text_width(text);
  double yt = W.ymin() + W.text_height(text) + W.pix_to_real(2);
  W.draw_text(xt,yt,text,bg_col.text_color());

  W.flush_buffer();
  window_pr = W.get_window_pixrect();

  W.stop_buffering();
 }


void draw_logo(int) { redraw(); }



void setup(int)
{ 
  panel P("LOGO");
  P.color_item("L-color",color_L);
  P.color_item("E-color",color_E);
  P.color_item("D-color",color_D);
  P.color_item("A-color",color_A);
  P.color_item("background",bg_col);
  P.color_item("h-lines",hline_col);
  P.color_item("v-lines",vline_col1);
  P.color_item("circles",circle_col);

  P.int_item("Thickness",Thickness,0,100);
  P.int_item("Distance",Distance,0,100);

  P.button("ok",0);
  P.button("quit",2);

  cont = P.open() == 0;

  W.set_bg_color(bg_col);

  if (window_pr)
  { W.del_pixrect(window_pr);
    window_pr = 0;
   }
  redraw();
}



int main()
{
  W.set_bg_color(bg_col);
  W.set_clear_on_resize(false);


#if defined(__win32__)
  int screen_w = window::screen_width();
  int screen_h = window::screen_height();

  int x0 = screen_w/2 - 4;
  int y0 = screen_h/2 - 8;

  W.display(500,400);


  for(int w=120; w < 0.85*screen_w; w += 5) 
  { int h = int(0.62*w - 20);
    W.resize(x0-w/2,y0-h/2,w,h);
  }
#else
  W.display(0,0);
#endif

  W.set_redraw(redraw);
  //W.init(-2.75,2.75,-1.15);
  W.init(-3.10,3.10,-0.95);

  W.start_buffering();

  W.clear();
  window_pr = W.get_window_pixrect();


  draw_title(true);
  draw_lines(true);
  draw_circles(true);
  fill_logo(true);

  W.del_pixrect(window_pr);
  window_pr = W.get_window_pixrect();
  draw_text(0,-0.80,"Version 6.0",text_col,true);

  W.stop_buffering();

  W.del_pixrect(window_pr);
  window_pr = W.get_window_pixrect();

  leda_wait(0.75);
  redraw();

  menu dummy;
  dummy.button("dummy1");
  dummy.button("dummy2");
  dummy.button("dummy3");

  panel P1(W.width(),-1,"LEDA 4.0");

  P1.button("&File",dummy);
  P1.button("&Install",dummy);
  P1.button("&Demos",dummy);
  P1.button("&Manual",dummy);
  P1.button("&Options",setup);
  P1.button("&Help");
  P1.button("&Exit",999);
  P1.make_menu_bar();

  P1.display(W,0,0);

  while(cont && P1.read_mouse() != 999);


  return 0;
}
