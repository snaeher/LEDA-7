/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  draw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/geo/plane.h>
#include <LEDA/graphics/window.h>
#include <LEDA/core/map.h>


using namespace leda;
using std::cout;
using std::endl;

enum { POINT, SEGMENT, RAY, LINE, CIRCLE, RECTANGLE, POLY};
//enum { SEGMENT, RAY, LINE, CIRCLE, RECTANGLE, POLY};


class geo_object_base {
public:
virtual void draw(window& W) = 0;
virtual ~geo_object_base() {}
};


template <class T>
class geo_object : public geo_object_base {

T obj;
color col;
color fcol;
int   lwidth;
int   lstyle;

public:

geo_object(const T& x, color c, color fc, int lw, int ls) : 
                  obj(x), col(c), fcol(fc), lwidth(lw), lstyle(ls) {}

void draw(window& W)
{ W.set_fg_color(col);
  W.set_fill_color(fcol);
  W.set_line_width(lwidth);
  W.set_line_style((line_style)lstyle);
  //W << obj;
  W.draw(obj);
}

~geo_object() {}

};



class geo_scene {

  window& W;

  list<geo_object_base*> L;

public:

  window& get_window() const { return W; }

  void insert(geo_object_base* p) { L.append(p); }
  void draw()  { geo_object_base* p; forall(p,L) p->draw(W); }
  void clear() { while (!L.empty()) delete L.pop(); }
  geo_scene(window& win) : W(win) {} 


  template <class T> void add()     
  { T x; W >> x;
  
    color line_clr = W.get_fg_color();
    color fill_clr = W.get_fill_color();
    int lwidth = W.get_line_width();
    int lstyle = W.get_line_style();
  
    geo_object<T>* p = new geo_object<T>(x,line_clr,fill_clr,lwidth,lstyle);
    p->draw(W);
    L.append(p); 
  }

};




void redraw(window* wp)  {
  wp->clear();
  geo_scene* sc_ptr = (geo_scene*)wp->get_client_data();
  sc_ptr->draw();
}


void scroll_start(window* wp, double x0, double y0, double x1, double x2)
{ geo_scene* sc_ptr = (geo_scene*)wp->get_client_data();
  sc_ptr->draw();
}

void handler(int x) {}


int main()
{

  color fg_col   = blue;
  color fill_col = yellow;
  color bg_col   = white;

  point_style pstyle = circle_point;
  line_style  lstyle = solid;

  int lwidth = 1;

  int shape = CIRCLE;

//int   x_origin = 0;
//int   y_origin = 0;

  list<string> shapes;
  shapes.append("point");
  shapes.append("seg");
  shapes.append("ray");
  shapes.append("line");
  shapes.append("circle");
  shapes.append("box");
  shapes.append("poly"); 

  window W("A simple drawing program");

  W.enable_close_button(true);

  geo_scene sc(W);

  W.set_client_data(&sc);

  W.choice_item("Drawing Shape ",shape,shapes);
  W.color_item ("Line Color    ",fg_col,handler);
  W.color_item ("Fill Color    ",fill_col,handler);
//W.color_item ("Background    ",bg_col);

  W.lwidth_item("Line Width",lwidth);

/*
  W.lstyle_item("Line Style",lstyle);
  W.pstyle_item("Point Style",pstyle);
*/

  W.button("zoom +",100);
  W.button("zoom -",101);

  W.button("clear",1);
  W.button("exit", 2);

  W.init(0,500,0);
  W.set_clear_on_resize(false);
  W.set_redraw(redraw);

  W.display(window::center,window::center);

W.set_frame_label(string("screen: %d x %d   window:  %d x %d",  
                          window::screen_width(),window::screen_height(),
                          W.width(),W.height()));

/*
  BASE_WINDOW* sw = W.open_status_window(30,grey1);
  sw->set_fixed_font();
*/
  
  W.clear();

/*
  W.clip_mask_window(0);
  polygon P;
  W >> P;
  W.draw_polygon(P,grey3);
  W.clip_mask_polygon(P,1);
*/

/*
int sz = 35;
for(int i=0; i<10; i++)
{ W.set_font(string("T%d",sz));
  W.draw_text(20,50*(i+1),string("Hello World %d",sz));
  sz = W.real_to_pix(W.text_height("H"));
}
*/


  for(;;)
  { 
    point p;
    int k = W.read_mouse(p);

    // window close button
    if (k == CLOSE_BUTTON) break;

    // quit button
    if (k == 2) break;

    if (k == MOUSE_BUTTON(4)) {
       W.zoom(2.0);
       continue;
    }

    if (k == MOUSE_BUTTON(5)) {
       W.zoom(0.5);
       continue;
    }


    W.set_fg_color(fg_col);
    W.set_fill_color(fill_col);
    W.set_bg_color(bg_col);
    W.set_line_width(lwidth);
    W.set_line_style(lstyle);
    W.set_point_style(pstyle);


    switch (k) {

     case   1: W.clear();
               sc.clear();
               continue;
     case 100: W.zoom(2.0);
               continue;
     case 101: W.zoom(0.5);
               continue;
    }

    bool dblclck,drag;
    W.read_mouse(p,300,300,dblclck,drag);

/*
cout << "p = " << p << "  drag = " << drag << " dblclck = " << dblclck << endl;
*/

    if (dblclck)
    { point a,b;
      W.read_zoom_rect(p,a,b);
      W.zoom_area(a,b);
      continue;
     }

    if (drag)
    { //W.set_cursor(XC_fleur);
      W.scroll_window(p,scroll_start);
      continue;
     }

    W.set_point_buffer(p);
  

    switch (shape) {

        case POINT:   sc.add<point>();
                      break;
  
        case SEGMENT: sc.add<segment>();
                      break;

        case RAY:     sc.add<ray>();
                      break;

        case LINE:    sc.add<line>();
                      break;
  
        case CIRCLE:  sc.add<circle>();
                      break;

        case RECTANGLE: sc.add<rectangle>();
                      break;
  
        case POLY:    sc.add<polygon>();
                      break;
       }

  }

  return 0;
}
