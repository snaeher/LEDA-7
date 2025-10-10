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

static int width = 100;
static int distance = 50;

static color color_L  = blue2;
static color color_E  = yellow;
static color color_D  = green2;
static color color_A  = orange;

//static color line_clr  = 0x333333;

static color bg_clr = 0xdddddd;
static color line_clr = 0x333333;

static bool show_lines = true;
static bool show_arrows = true;


static double compute_strip_width(double D,double r)
{
  // outer radius: r = sqrt(5);
  // center: xc = 0 and yc = -1

  // compute strip width w

  // innder Radius R = r-w
  // circle equation: (x-xc)^2 + (y-yc)^2  = R^2

  // x^2 +(y+1)^2 = (r-w)^2 
  // x^2 + y^2 +2*y + 1 = r^2 -2rw + w^2

  // for y = 3*w and x = D+w

  //(D+w)^2 + 9w^2 + 6w + 1 = r^2 - 2rw + w^2
  // D^2 + 2Dw + w^2 + 9w^2 + 6w + 1 = r^2 - 2rw + w^2
  // D^2 + 2Dw + 9w^2 + 6w + 1 = r^2 - 2rw
  // 9w^2 + 2(D+r+3)w + (D^2-r^2+1) = 0  

   double a = 9;
   double b = 2*(D+r+3);
   double c = D*D - r*r + 1;

   double dis = b*b - 4*a*c;

   double w = 0;
   if (dis > 0) w = (sqrt(dis)-b)/(2*a);
/*
   point p(D+w,3*w);
   double rr = p.distance(point(0,-1)) + w;
   cout << "sqr(r) = " << rr*rr << endl;
*/
   return w;
}


  


inline void draw_vertical_line(window& W, double x, double y, color c) 
{ 
  double ytop = 1.6;
  if (show_arrows)
    W.draw_arrow(x,y,x,ytop,c); 
  else
    W.draw_segment(x,y,x,ytop,c); 
}

inline void draw_horizontal_line(window& W, double y, color c) 
{ if (show_arrows)
    W.draw_arrow(-2.4,y,2.4,y,c);
  else
    W.draw_segment(-2.4,y,2.4,y,c);
}


void draw_circle(window& W, circle C, double y, color col)
{ // draw part of circle lying above y
  line l(point(0,y),point(1,y));
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


void fill_logo(window& W, circle c1,circle c2,circle c3,circle c4,double w, 
                                                                  double d, 
                                                                  double d_mid)
{
   // w width of rings and strips
   // d left/right distance
   // d_mid  middle distance

   double d2 = d_mid/2;

   // L
   W.clip_mask_window(0);
   clip_circle(W,c1,1);
   clip_circle(W,c2,0);
 //W.clip_mask_rectangle(-2+w,0,-1,w,1);
   W.clip_mask_rectangle(-2+0.5*w,0,-1,w,1);

 //W.draw_box(-2.0,0,-1-d,1.0,color_L);
   W.draw_box(-2.1,0,-1-d,1.1,color_L);

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

   W.clip_mask_rectangle(1+w+d,w,2-1.3*w,2*w,1);
   W.clip_mask_rectangle(1+d,0,1+d+w,1-w-0.5*d,1);

 //W.draw_box(1+d,0,2.0,1.0,color_A);
   W.draw_box(1+d,0,2.1,1.1,color_A);

   W.clip_mask_window(1);
}


void draw_logo(window& W, int dist, double scale)
{
  // scale in [0..1] for animation

  double d = 0.003*dist;  // letter distance
  double d_mid = 1.15*d;    // middle letter distance (larger)

  double r1 = sqrt(5); // radius of outer large circle c1
  double r2 = 1.00;    // radius of outer small circle c3

  // computer strip width
  double w = scale * compute_strip_width(d+r2,r1);

//double w = 0.003255*scale*width; // width of rings and stripes

  circle c1(0,-1,r1);
  circle c2(0,-1,r1-w);

/*
  circle c1(0,-0.97,r1);
  circle c2(0,-0.97,r1-w);
*/

  circle c3(0,0,r2);
  circle c4(0,0,r2-w);

  W.set_bg_color(bg_clr);

  W.set_line_width(1);

  W.start_buffering();
  W.clear();

  fill_logo(W,c1,c2,c3,c4,w,d,d_mid);

  if (show_lines)
  { 
/*
    draw_circle(W,c1,-0.5,line_clr);
    draw_circle(W,c2,-0.5,line_clr);

    draw_circle(W,c3,-0.3,line_clr);
    draw_circle(W,c4,-0.3,line_clr);
*/

    draw_circle(W,c1,-0.4,line_clr);
    draw_circle(W,c2,-0.4,line_clr);

    draw_circle(W,c3,-0.4,line_clr);
    draw_circle(W,c4,-0.4,line_clr);
  
    draw_vertical_line(W,-1-d,      -0.4, line_clr);
    draw_vertical_line(W,-d_mid/2,  -0.4, line_clr);
    draw_vertical_line(W, d_mid/2,  -0.4, line_clr);
    draw_vertical_line(W, d_mid/2+w,-0.4, line_clr);
    draw_vertical_line(W, 1+d,      -0.4, line_clr);
    draw_vertical_line(W, 1+d+w,    -0.4, line_clr);
  
    for(int i=0; i<4; i++) draw_horizontal_line(W,i*w,line_clr);
/*
    draw_horizontal_line(W,r2,line_clr);
    draw_horizontal_line(W,r2-w,line_clr);
*/
  }

  W.flush_buffer();
  W.stop_buffering();

  W.redraw_panel();
}

void draw_copyright(window& W)
{ W.start_buffering();

  if (true)
  { if (getenv("LEDA_OPEN_MAXIMIZED"))
      W.set_font("I25");
    else
      W.set_font("I30");
  }

  if (window::display_type() == "x11") W.set_font("I22");

//string text = "LEDA Logo (c) sn 1995.";
  string text = "LEDA Logo (sn 1995)";

  double xt = W.xmin() + W.pix_to_real(8);
  double yt = W.ymin() + W.text_height(text) + W.pix_to_real(4);
  W.draw_text(xt,yt,text,0x555555);
  W.set_text_font();
  W.flush_buffer();
  W.stop_buffering();
}


void redraw() {
  window* wp = window::get_call_window();
  draw_logo(*wp,distance,1);
  draw_copyright(*wp);
}

void update(int) {
  window* wp = window::get_call_window();
  draw_logo(*wp,distance,1);
  draw_copyright(*wp);
}


void animation(window& W, float delay=0)
{ draw_logo(W,distance,0);
  leda::sleep(delay);
  double fmax = width/100.0;
  double d = fmax/100;
//if (window::display_type() == "xx") d *= 4;
  if (window::display_type() == "xx") d *= 3.5;

  for(double f=d; f < fmax; f += d)
  { draw_logo(W,distance,f);
    W.get_mouse(); // allow interaction
   }

  draw_logo(W,distance,fmax);

  leda::sleep(delay/2);
  draw_copyright(W);
}


int main()
{
  leda::copyright_window_string = "";

  int dpi = window::dpi();

  panel P("LEDA Logo");

  P.text_item("\\bf Settings");
  P.text_item("");

/*
  P.color_item("L-color",color_L);
  P.color_item("E-color",color_E);
  P.color_item("D-color",color_D);
  P.color_item("A-color",color_A);
*/

  P.color_item("Background",bg_clr);
  P.color_item("Line Color",line_clr);

  P.int_item("Width",width,0,100,update);
  P.int_item("Space",distance,0,100,update);

/*
  P.bool_item("Show Lines",show_lines,update);
*/
  P.bool_item("Arrows",show_arrows,update);

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

  //int w = 6*dpi;
  int w = int(6.5*dpi);
  int wmax = int(0.9*window::screen_width());
  if (w > wmax) w = wmax;

  int h = w/2;

  if (getenv("LEDA_OPEN_MAXIMIZED"))
  { w = window::screen_width() + 1;
    h = window::screen_height() + 1;
    //w = 0;
    //h = 0;
   }

  window W(w,h,title);

  W.enable_close_button(true);

  W.set_bg_color(bg_clr);

/*
  W.int_item("  width",width,0,200,update);
  W.int_item("  space",distance,0,100,update);

  panel_item it1 = W.bool_item("     lines",show_lines,update);
  W.set_item_label_width(it1,dpi);

  panel_item it2 = W.bool_item("     arrows",show_arrows,update);
  W.set_item_label_width(it2,dpi);

  W.button("prefs",1);

  W.set_item_width(2*dpi);

  W.make_menu_bar(4);
*/

  if (getenv("LEDA_OPEN_MAXIMIZED"))
    W.display(W,window::center,window::center); // frameless
  else
    W.display(window::center,window::center);

  double xmax = 2.8;
  double ymin = -1.5*h/w;

  W.init(-xmax,xmax,ymin);

//W.set_clear_on_resize(false);

  bool anim = true;

  if (anim)
    animation(W,0.5f);
  else
    draw_logo(W,distance,0.01*width);

  W.set_redraw(redraw); // after animation


  for(;;)
  { 
    int but = W.read_mouse();

    if (but == MOUSE_BUTTON(1)) animation(W,0.6f);

    if (but == MOUSE_BUTTON(3)) {
       if (P.open(W) == 2) break;
       animation(W,0.5);
    }

    if (but == CLOSE_BUTTON) break;

   }

  return 0;
}
