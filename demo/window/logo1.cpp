/*******************************************************************************
+
+  LEDA 7.2.2  
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


static int width = 50;
static int distance = 50;

static color color_L    = blue2;
static color color_E    = yellow;
static color color_D    = green2;
static color color_A    = orange;

//static color bg_clr    = 0xbbbbbb;
//static color line_clr  = 0x888888;

static color bg_clr    = 0xd0d0d0;

//static color line_clr  = 0x555555;
static color line_clr  = 0x333333;
static color circle_clr = line_clr;

static bool show_lines = true;
static bool show_arrows = true;



inline void draw_vertical_line(window& W, double x, color c) 
{ if (show_arrows)
    W.draw_arrow(x,-0.5,x,1.6,c); 
  else
    W.draw_segment(x,-0.5,x,1.6,c); 
}

inline void draw_horizontal_line(window& W, double y, color c) 
{ if (show_arrows)
    W.draw_arrow(-2.4,y,2.4,y,c);
  else
    W.draw_segment(-2.4,y,2.4,y,c);
}


void draw_circle(window& W, circle C, double y, color col)
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


void clip_circle(window& W, circle c, int mode)
{ double x = c.center().xcoord();
  double y = c.center().ycoord();
  double r = c.radius();
  W.clip_mask_ellipse(x,y,r,r,mode);
}


void fill_logo(window& W, circle c1,circle c2,circle c3,circle c4,
                                             double w, double d1, double d2)
{
   // L
   W.clip_mask_window(0);
   clip_circle(W,c1,1);
   clip_circle(W,c2,0);
   W.clip_mask_rectangle(-2+w,0,-1,w,1);
   W.draw_box(-2,0,-1-d1,1,color_L);

   // E
   W.clip_mask_window(0);
   clip_circle(W,c3,1);
   clip_circle(W,c4,0);
   W.clip_mask_rectangle(-1+w,      0,-d2,  w,1);
   W.clip_mask_rectangle(-1+1.3*w,2*w,-d2,3*w,1);
   W.draw_box(-1,0,-d2,1,color_E);

   // D
   W.clip_mask_window(0);
   clip_circle(W,c3,1);
   clip_circle(W,c4,0);
   W.clip_mask_rectangle(d2,0,d2+w,1-w,1);
   W.clip_mask_rectangle(w+d2,0,1-w/2,w,1);
   W.draw_box(d2,0, 1,1,color_D);

   // A
   W.clip_mask_window(0);
   clip_circle(W,c1,1);
   clip_circle(W,c2,0);
   W.clip_mask_rectangle(1+w+d1,w,2-1.5*w,2*w,1);
   W.clip_mask_rectangle(1+d1,0,1+d1+w,1-w-0.5*d1,1);
   W.draw_box( 1+d1,0, 2,1,color_A);

   W.clip_mask_window(1);
}


void draw_logo(window& W, int width, int dist)
{
  double w  = 0.0035*width;
  double d1 = 0.0024*dist;
  double d2 = 0.0013*dist; 

  circle c1(0,-1,sqrt(5));
  circle c2(0,-1,sqrt(5)-w);

  circle c3(0,0,1);
  circle c4(0,0,1-w);

  W.set_bg_color(bg_clr);

  //W.set_line_width(2);
  W.set_line_width(1);

  W.start_buffering();
  W.clear();

  fill_logo(W,c1,c2,c3,c4,w,d1,d2);

  if (show_lines)
  { 
    draw_circle(W,c1,-0.5,circle_clr);
    draw_circle(W,c2,-0.5,circle_clr);

    draw_circle(W,c3,-0.3,circle_clr);
    draw_circle(W,c4,-0.3,circle_clr);
  
    draw_vertical_line(W,-1-d1, line_clr);
    draw_vertical_line(W,-d2,   line_clr);
    draw_vertical_line(W,d2,    line_clr);
    draw_vertical_line(W,d2+w,  line_clr);
    draw_vertical_line(W,1+d1,  line_clr);
    draw_vertical_line(W,1+d1+w,line_clr);
  
    for(int i=0; i<4; i++) draw_horizontal_line(W,i*w,line_clr);
  }


  string text = "(c) sn 1995";
  double xt = W.xmin() + W.pix_to_real(8);
  double yt = W.ymin() + W.text_height(text) + W.pix_to_real(4);
  W.draw_text(xt,yt,text,0x333333);

  W.flush_buffer();
  W.stop_buffering();

  W.redraw_panel();
}


void redraw() {
  window* wp = window::get_call_window();
  draw_logo(*wp,width,distance);
}

void update(int) {
  window* wp = window::get_call_window();
  draw_logo(*wp,width,distance);
}


void animation(window& W)
{ draw_logo(W,0,distance);
  int d = 1;
  if (window::display_type() == "xx") 
    d = 2;
  else
    leda_wait(0.5);
  for(int w=0; w < width; w += d) draw_logo(W,w,distance);
}


int main()
{
  int dpi = window::dpi();

  panel P("LOGO");

  P.color_item("L-color",color_L);
  P.color_item("E-color",color_E);
  P.color_item("D-color",color_D);
  P.color_item("A-color",color_A);
  P.color_item("background",bg_clr);
  P.color_item("lines",line_clr);
  P.color_item("circles",circle_clr);

  P.button("ok",0);
  P.button("quit",2);

  string title("LEDA - %.1f",float(__LEDA__)/100);
/*
  title += "   (";
  title += leda::compile_time;
  title += "  ";
  title += leda::compile_date;
  title += ")";
*/

  //int w = 10*dpi;
  int w = 7*dpi;
  int wmax = int(0.9*window::screen_width());

  if (w > wmax) w = wmax;

  window W(w,w/2,title);

/*
  W.int_item("  width",width,0,100,update);
  W.int_item("  space",distance,0,100,update);

  panel_item it1 = W.bool_item("     lines",show_lines,update);
  W.set_item_label_width(it1,dpi);

  panel_item it2 = W.bool_item("     arrows",show_arrows,update);
  W.set_item_label_width(it2,dpi);

  W.button("prefs",1);

  W.set_item_width(2*dpi);

  W.make_menu_bar(4);
*/

  W.display(window::center,window::center);
  W.set_redraw(redraw);

  W.init(-2.75,2.75,-0.85);
  W.set_clear_on_resize(false);

  if (window::display_type() != "xx") {
    if (window::screen_width() > window::screen_height()) animation(W);
  }

  for(;;)
  { draw_logo(W,width,distance);
    int but = W.read_mouse();

    if (window::screen_width() > window::screen_height()) {
      if (but == MOUSE_BUTTON(1)) animation(W);
    }

    if (but == MOUSE_BUTTON(3)) {
       if (P.open(W) == 2) break;
    }

    if (but == CLOSE_BUTTON) break;

   }

  return 0;
}
