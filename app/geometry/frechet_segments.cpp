/*******************************************************************************
+
+  LEDA 7.2.3  
+
+
+  frechet_segments.cpp
+
+
+  Copyright (c) 1995-2026
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;

const int light_grey = 0xfafafa;

const int light_red    = 0xffe0e0;
const int light_green  = 0xe0ffe0;
const int light_blue   = 0xe0e0ff;

//const int light_yellow = 0xffffe0;
const int light_yellow = 0xffffc0;



static double mouse_x = -1;
static double mouse_y = -1;

static segment s1;
static segment s2;

static double leash_distance = 0;
static int move_seg = 0;



double relative_distance(point p, segment s)
{ double f = p.distance(s.start())/s.length();
  if (side_of_halfspace(s.start(),s.end(),p) < 0) f = -f;
  return f;
}

point project_to_segment(double f, segment s)
{ vector vec = s.end() - s.start();
  return s.start().translate(f*vec);
}



void draw_ellipse(window& W, segment s1, segment s2, double dist,
                                         double x0, double y0,
                                         double x1, double y1,
                                         color clr1, color clr2, color clr3)
{
  int pix_min = 0;
  int pix_max = W.width();

  if (clr3 == invisible) {
    pix_min = W.xpix(x0);
    pix_max = W.xpix(x1);
  }

  double dx = x1 - x0;
  double dy = y1 - y0;

  int w = W.real_to_pix(dx);

/*
  line line1(s1);
  line line2(s2);

  double d = leash_distance;
  vector v = d*s2.rotate(-LEDA_PI/2).to_vector().norm();

  point p_left;
  if (line1.intersection(line2+v,p_left))
  { double f_left = relative_distance(p_left,s1);
    point p_right;
    line1.intersection(line2-v,p_right);
    double f_right = relative_distance(p_right,s1);

    //W.draw(line2+v,grey1);
    //W.draw(line2-v,grey1);
    //W.set_node_width(5);
    //W.draw_filled_node(p_left,yellow);
    //W.draw_filled_node(p_right,yellow);

    if (f_left > f_right) swap(f_left,f_right);
    pix_min = W.xpix(x0) + int(f_left * w) - 1;
    pix_max = W.xpix(x0) + int(f_right * w) + 1;
  }
*/

  for(int x = pix_min; x <= pix_max; x++)
  {
    double fx =  double(x-W.xpix(x0))/w;

    point c = project_to_segment(fx,s1);

    list<point>  L = circle(c,dist).intersection(line(s2));

    if (L.empty()) continue;

    point q1 = L.front();
    point q2 = L.back();

    double f1 = relative_distance(q1,s2);
    double f2 = relative_distance(q2,s2);

    if (f1 > f2) swap(f1,f2);

    fx = x0 + fx*dx;
    f1 = y0 + f1*dy;
    f2 = y0 + f2*dy;

    if (fx < x0 || fx > x1) 
    { W.draw_segment(fx,f1,fx,f2,clr2);
      W.draw_pixel(fx,f1,clr3);
      W.draw_pixel(fx,f2,clr3);
      continue;
    }

    if (f1 < y0) {
      W.draw_segment(fx,f1,fx,min(0.0,f2),clr2);
      W.draw_pixel(fx,f1,clr3);
      W.draw_pixel(fx,f2,clr3);
      f1 = y0;
    }

    if (f2 > y1) {
      W.draw_segment(fx,max(1.0,f1),fx,f2,clr2);
      W.draw_pixel(fx,f1,clr3);
      W.draw_pixel(fx,f2,clr3);
      f2 = y1;
    }

    if (f1 <= f2)
    { W.draw_segment(fx,f1,fx,f2,clr1);
      W.draw_pixel(fx,f1,clr3);
      W.draw_pixel(fx,f2,clr3);
    }
  }

  if (clr3 != invisible)
  { // draw center
    point c;
    if (s1.intersection_of_lines(s2,c))
    { double f1 = relative_distance(c,s1);
      double f2 = relative_distance(c,s2);
      int nw = W.set_node_width(2);
      W.draw_filled_node(f1,f2,grey3);
      W.set_node_width(nw);
     }
  }

}




void draw_segments(window& W, segment s1, segment s2)
{
  W.set_line_width(1);
  W.draw_line(s1,grey2);
  W.draw_line(s2,grey2);

  W.set_line_width(3);
  W.draw_segment(s1,blue2);
  W.draw_segment(s2,green2);
}


  
void redraw(window* wp)
{
  window& W = *wp;

  double fx = -1;
  double fy = -1;

  double dist = leash_distance;

  W.start_buffering();
  W.clear();

  W.del_messages();
  W.message("Drag pointer along one of the segments or move it into the Free Space rectangle.");

  W.draw_box(0,0,1,1,0xe0e0e0);

  draw_ellipse(W,s1,s2,dist,0,0,1,1,light_yellow,light_grey,black);

  W.draw_rectangle(0,0,1,1,grey3);

  W.set_line_width(2);
  W.draw_arrow(0,0,1,0,blue2);
  W.draw_arrow(0,0,0,1,green2);


  point p(mouse_x,mouse_y);

  if (mouse_y <= 1.15) 
  { 
    double fx = mouse_x;
    double fy = mouse_y;

    if (fx < 0) fx = 0;
    if (fx > 1) fx = 1;
    if (fy < 0) fy = 0;
    if (fy > 1) fy = 1;

    W.set_line_width(1);
    W.draw_segment(fx,0,fx,1,grey3);
    W.draw_segment(0,fy,1,fy,grey3);

    W.draw_ctext(fx,-0.04,string("%.2f",fx),grey3);
    W.draw_ctext(-0.08,fy,string("%.2f",fy),grey3);
  
    point p = project_to_segment(fx,s1);
    point q = project_to_segment(fy,s2);

    color clr = (p.distance(q) > dist) ? red : green;

    W.draw_filled_node(fx,fy,clr);
    W.draw_node(fx,fy,black);

    color light_clr  = (p.distance(q) > dist) ? light_red : light_green;

    W.set_line_width(1);
    W.draw_disc(p,dist,light_clr);
    W.draw_circle(p,dist,grey3);

    draw_segments(W,s1,s2);

    W.set_line_width(2);
    W.draw_segment(p,q,grey3);

    W.set_line_width(1);

    W.draw_filled_node(p,white);
    W.draw_node(p,grey3);

    W.draw_filled_node(q,white);
    W.draw_node(q,grey3);

    double dist = p.distance(q);
    color dclr = (dist <= leash_distance) ? black : red;  
    W.draw_ctext(0.5,1.07,string("dist = %d",W.real_to_pix(dist)),dclr);

   }
  else
  {  
    draw_segments(W,s1,s2);

    line line1(s1);
    point p1 = line1.perpendicular(p).target(); 
    double dist1 = p.distance(p1);

    line line2(s2);
    point p2 = line2.perpendicular(p).target(); 
    double dist2 = p.distance(p2);

    if (dist1 > 1 && dist2 > 1) 
      move_seg = 0;
    else
    { list<point> L;
      double f = 0;

      if (move_seg == 0) {
        if (dist1 < dist2) 
           move_seg = 1;
        else
           move_seg = 2;
      }
      
      if (move_seg == 1) {
        if (dist2 < dist1 && dist1 > 0.10) {
          move_seg = 2;
        }
      }

      if (move_seg == 2) {
        if (dist1 < dist2 && dist2 > 0.10) {
          move_seg = 1;
        }
      }

  
      W.set_line_width(1);

      if (move_seg == 1)
      { f = relative_distance(p1,s1);
        L = circle(p1,dist).intersection(line2);
        W.draw_filled_node(p1,black);
        W.draw_circle(p1,dist,grey3);
        if (f >=0 && f <= 1) {
          W.draw_segment(f,0,f,1,grey3);
          W.draw_ctext(f,-0.04,string("%.2f",f),grey3);
         }
       }

      if (move_seg == 2)
      { f = relative_distance(p2,s2);
        L = circle(p2,dist).intersection(line1);
        W.draw_filled_node(p2,black);
        W.draw_circle(p2,dist,grey3);
        if (f >=0 && f <= 1) {
          W.draw_segment(0,f,1,f,grey3);
          W.draw_ctext(-0.08,f,string("%.2f",f),grey3);
         }
       }
  
      if (!L.empty())
      { point q1 = L.pop();
        point q2 = L.empty() ? q1 : L.pop();
  
        W.draw_filled_node(q1,red);
        W.draw_filled_node(q2,red);

        if (move_seg == 1)
        { double f1 = relative_distance(q1,s2);
          double f2 = relative_distance(q2,s2);
          W.set_line_width(2);
          W.draw_segment(f,f1,f,f2,green2);
          W.draw_filled_node(f,f1,red);
          W.draw_filled_node(f,f2,red);
          W.set_line_width(1);
          W.draw_segment(p1,q1,grey2);
          W.draw_segment(p1,q2,grey2);
         }

        if (move_seg == 2)
        { double f1 = relative_distance(q1,s1);
          double f2 = relative_distance(q2,s1);
          W.set_line_width(2);
          W.draw_segment(f1,f,f2,f,blue);
          W.draw_filled_node(f1,f,red);
          W.draw_filled_node(f2,f,red);
          W.set_line_width(1);
          W.draw_segment(p2,q1,grey2);
          W.draw_segment(p2,q2,grey2);
         }
  
      }
    }

  }
  W.flush_buffer();
  W.stop_buffering();
}


void set_leash(int x) 
{ window* wp = window::get_call_window();
  leash_distance = wp->pix_to_real(x);
  redraw(wp);
}

  

int main()
{   
  window W("The Frechet Distance of 2 Line Segments");

  int dpi = window::screen_dpi();

  int lpix = 120;
  W.int_item("  Length of Leash",lpix,0,300,set_leash);
  W.button("clear",1);
  W.make_menu_bar();

  W.init(-0.75,1.75,-0.25);

//W.display(50,50);
  W.display();


  leash_distance = W.pix_to_real(lpix);

  //W.set_node_width(5);
  W.set_node_width(dpi/32);

  for(;;)
  {
    W.disable_button(1);

    W.clear();
    W.message("Please draw two line segments in the upper part of the window.");

    W.set_line_width(1);
    W.draw_box(0,0,1,1,0xe0e0e0);
    W.draw_rectangle(0,0,1,1,grey3);

    W.set_line_width(3);

    W >> s1;
    if (s1.xcoord2() < s1.xcoord1()) s1 = s1.reverse();
    W.draw_segment(s1,blue2);

    W >> s2;
    if (s2.xcoord2() < s2.xcoord1()) s2 = s2.reverse();
    W.draw_segment(s2,green2);

    W.enable_button(1);

    mouse_x = s2.end().xcoord();
    mouse_y = s2.end().ycoord();

    redraw(&W);

    int but = 0;
    double x,y;
    unsigned long t = 0;
    while (W.read_event(but,x,y,t) != button_press_event || 
                                               (but != 1 && but != 2))   
    { 
      int dx = W.real_to_pix(x - mouse_x);
      int dy = W.real_to_pix(y - mouse_y);
      int d = dx*dx + dy*dy;

      if (d < 8) continue;

      mouse_x = x;
      mouse_y = y;
      redraw(&W);
    }

    if (but == 2) break;
  }

  return 0;
}
