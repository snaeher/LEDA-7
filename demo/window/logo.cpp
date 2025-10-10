/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  logo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;


/*
static window W(800,450,"LEDA");

static int  Distance    =  75;
static int  Distance    =  75;
*/

//static window W(1000,600,"LEDA");
static window W(8*window::screen_dpi(),5*window::screen_dpi(),"LEDA");

static int  Thickness   =  90;
static int  Distance   =  90;

static color color_L    = blue2;
static color color_E    = yellow;
static color color_D    = green2;
static color color_A    = orange;

/*
static color bg_col     = grey2;
static color hline_col  = black;
static color vline_col  = black;
static color circle_col = black;
*/

static color bg_col     = grey2;
static color hline_col  = grey3;
static color vline_col  = grey3;
static color circle_col = grey3;

static bool show_lines = true;
static bool show_arrows = true;

inline void draw_vertical_line(double x, color c) 
{ if (show_arrows)
    W.draw_arrow(x,-0.5,x,1.6,c); 
  else
    W.draw_segment(x,-0.5,x,1.6,c); 
}

inline void draw_horizontal_line(double y, color c) 
{ if (show_arrows)
   W.draw_arrow(-2.4,y,2.4,y,c);
  else
   W.draw_segment(-2.4,y,2.4,y,c);
}


void draw_circle(circle C, double y, color col)
{ line l(point(0,y),point(1,y));
  list<point> I = C.intersection(l);
  point a = I.pop();
  point b = C.point_on_circle(LEDA_PI/2);
  point c = I.pop();
  if (show_arrows)
    W.draw_arc_arrow(a,b,c,col);
  else
    W.draw_arc(a,b,c,col);
}

void draw_circles(circle c1, circle c2, circle c3, circle c4, color c)
{ draw_circle(c1,-0.5,c);
  draw_circle(c2,-0.5,c);
  draw_circle(c3,-0.3,c);
  draw_circle(c4,-0.3,c);
}


void draw_lines(circle c1, circle c2, circle c3, circle c4, double d,
                                                            double eps,
                                                            color hcol, 
                                                            color vcol)
{ draw_vertical_line(-1-2*eps, vcol);
  draw_vertical_line(-eps,     vcol);
  draw_vertical_line(eps,      vcol);
  draw_vertical_line(d+eps,    vcol);
  draw_vertical_line(1+2*eps,  vcol);
  draw_vertical_line(1+2*eps+d,vcol);

  for(int i=0; i<4; i++) draw_horizontal_line(i*d,hcol);

/*
  for(int i=0; i<4; i++) W.draw_arrow(-2.5,i*d,2.5,i*d,hcol);
*/
}


void clip_circle(circle c, int mode)
{ double x = c.center().xcoord();
  double y = c.center().ycoord();
  double r = c.radius();
  W.clip_mask_ellipse(x,y,r,r,mode);
}


void fill_logo(circle c1, circle c2, circle c3, circle c4, double d,double eps)
{
   double eps2 = 2*eps;

   W.clip_mask_rectangle(W.xmin(),W.ymin(),W.xmax(),W.ymax(),0);

   clip_circle(c1,1);
   clip_circle(c2,0);

   W.clip_mask_rectangle(-1-eps2,-4,1+eps2,4,0);

   clip_circle(c3,1);
   clip_circle(c4,0);


   // L
   W.clip_mask_rectangle(-2+d,0,-1-eps2,d,1);

   // E
   W.clip_mask_rectangle(-1+1.25*d,3*d,-eps,2*d,1);
   W.clip_mask_rectangle(-1+d,0,-eps,d,1);

   // D
   W.clip_mask_rectangle(eps,0,eps+d,1-d,1);
   W.clip_mask_rectangle(d+eps,0,1-d/2,d,1);

   // A
   W.clip_mask_rectangle(1+d+eps2,d,2-1.5*d,2*d,1);
   //W.clip_mask_rectangle(1+eps2,0,1+eps2+d,0.95-d,1);

   W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1.00-d-0.5*eps2,1);

   //W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1-1.25*d,1);
   //W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1-1.5*d,1);
   //W.clip_mask_rectangle(1+eps2,0,1+eps2+d,1-0.3*d-eps2,1);



   W.clip_mask_rectangle(-4,-4,4,0,0);
   W.clip_mask_rectangle(-eps,-4,eps,4,0);


   W.draw_box(-2-eps,0,-1,1,color_L);
   W.draw_box(-1-eps,0, 0,1,color_E);
   W.draw_box( 0+eps,0, 1,1,color_D);
   W.draw_box( 1+eps,0, 2,1,color_A);

   W.clip_mask_window(1);
}


void redraw()
{
  double d   = 0.002*Thickness;
  double eps = 0.001*Distance;

  //double eps = W.pix_to_real(int(0.1*Distance));


  double sqrt5 = sqrt(5.0);


  circle c1(0,-1,sqrt5);
  circle c2(0,-1,sqrt5-d);

/*
  circle c1(0,-1,sqrt5+d/2);
  circle c2(0,-1,sqrt5-d/2);
*/

  circle c3(0,0,1);
  circle c4(0,0,1-d);


  W.start_buffering();
  W.clear();

  fill_logo(c1,c2,c3,c4,d,eps);

  if (show_lines)
  { draw_circles(c1,c2,c3,c4,circle_col);
    draw_lines(c1,c2,c3,c4,d,eps,hline_col,vline_col);
   }
  
  W.set_font(string("T%d",W.real_to_pix(0.1)));
  //string text = "(c)  ullfan 1995";
  string text = " sn 1995";
  //double xt = W.xmax() - W.text_width(text);
  double xt = W.xmin() + W.pix_to_real(2);
  double yt = W.ymin() + W.text_height(text) + W.pix_to_real(1);
  W.draw_text(xt,yt,text,bg_col.text_color());

  W.flush_buffer();
  W.stop_buffering();

  W.redraw_panel();
 }


void draw_logo(int) { redraw(); }


int main()
{
  panel P("LOGO");

  P.color_item("L-color",color_L);
  P.color_item("E-color",color_E);
  P.color_item("D-color",color_D);
  P.color_item("A-color",color_A);
  P.color_item("background",bg_col);
  P.color_item("h-lines",hline_col);
  P.color_item("v-lines",vline_col);
  P.color_item("circles",circle_col);

  P.button("ok",0);
  P.button("quit",2);

  W.int_item("width",Thickness,0,100,draw_logo);
  W.int_item("space",Distance,0,200,draw_logo);
  W.bool_item("lines",show_lines,draw_logo);
  W.bool_item("arrows",show_arrows,draw_logo);

  W.set_item_width(175);

  W.display(window::center,window::center);
  W.set_redraw(redraw);
  W.set_font("F12");
  W.init(-2.75,2.75,-0.75);
  W.set_clear_on_resize(false);

  W.set_bg_color(bg_col);

/*
  for(int t=0; t<90; t += 4)
  { Thickness = t;
    redraw();
   }
*/

  for(;;)
  { W.set_bg_color(bg_col);
    redraw();
    W.read_mouse();
    int but = P.open(W);
    if (but == 2) break;
   }

  return 0;
}
