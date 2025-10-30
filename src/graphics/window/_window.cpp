/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>
#include <LEDA/core/string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>


LEDA_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// LEDA WINDOW BASICS
//------------------------------------------------------------------------------

color window::fgcol = DEF_COLOR;
color window::bgcol = DEF_COLOR;


window::window() : BASE_WINDOW(copyright_string)
{ 
  if (//display_type() != "xx" && 
      get_error_handler() == default_error_handler) {
    set_error_handler(win_error_handler);
  }

  std_buttons();
  status_win = 0;
  point_buffer = 0;
  normalize_rat = false;
  show_orientation = false;
 }

window::window(int w, int h) : BASE_WINDOW(w,h,copyright_string)
{ 
  if (//display_type() != "xx" && 
      get_error_handler() == default_error_handler) {
    set_error_handler(win_error_handler);
  }

  std_buttons();
  status_win = 0;
  point_buffer = 0;
  normalize_rat = false;
  show_orientation = false;
 }

window::window(int w, int h, const char* label) : BASE_WINDOW(w,h,label)
{ user_log();
  write_log(string("Window: %s",label));

  if (//display_type() != "xx" && 
      get_error_handler() == default_error_handler) {
    set_error_handler(win_error_handler);
  }

  std_buttons();
  status_win = 0;
  point_buffer = 0;
  normalize_rat = false;
  show_orientation = false;
 }

window::window(const char* label) : BASE_WINDOW(label)
{ user_log();
  write_log(string("Window: %s",label));

  if (display_type() != "xx" && get_error_handler() == default_error_handler) {
    set_error_handler(win_error_handler);
  }

  std_buttons();
  status_win = 0;
  point_buffer = 0;
  normalize_rat = false;
  show_orientation = false;
 }



void window::win_error_handler(int i, const char* s)
{
  string msg("%-40s",s); 

  string line;

  int p = msg.index("(");
  int q = msg.index(")",p);

  if (q != -1)
  { line = msg(p+1,q-1).trim();
    msg = msg(0,p-1).trim();
   }

  msg = msg.replace_all(" ","~");
  msg = msg.replace_all("\\","/");

  window W;
  int tw = W.real_to_pix(1.5*W.text_width(s));
  
  panel P("LEDA EXCEPTION HANDLER",tw,-1);

  P.text_item("");

  if (i == 0)
    { P.text_item("\\bf\\blue Warning:");
      P.text_item("");
      if (line != "") {
         P.text_item("\\tt " + line);
         P.text_item("");
      }
      P.text_item("\\tt " + msg);
      P.button("continue");
      P.open();
     }
  else
    { P.text_item("\\bf\\red Error:");
      P.text_item("");
      if (line != "") {
         P.text_item("\\tt " + line);
         P.text_item("");
      }
      P.text_item("\\tt " + msg);
      P.button("exit",0);
      P.button("abort",1);
      P.button("ignore",2);

#if defined(linux) || defined(__GNUC__) && defined(sparc) && defined(__svr4__)
      P.button("where",3);
#endif

      P.buttons_per_line(4);
      P.display(window::center,window::center);

      switch (P.read()) {
     
      case 0: P.close();
              close_display();
              exit(0);
              break;

      case 1: P.close();
              close_display();
              abort();
              break;

      case 2: return;

      case 3: { P.set_frame_label("Exploring Stack ...");
                P.flush();
                P.set_fixed_font();
                char* lines[256];
                string L[256];
                int n = trace_stack(lines);
                int width = 0;
                int i;
                for(i=1; i<n; i++) 
                { char* p = lines[i];
                  char* q = p + strlen(p);
                  while (*q != ' ') q--;
                  *q++ = '\0';
                  L[i] = string(p);
                  L[i] += ' ';
                  L[i] += cplus_filter(q);
                  delete[] lines[i];
                  int w = P.real_to_pix(P.text_width(L[i]));
                  if (w > width) width = w;
                 }
                P.close();

                width += 30;
                if (width > 800) width = 800; 

                panel pan(width,-1,"LEDA Stack Trace");
                pan.text_item("");
                pan.text_item(string("\\bf\\blue %s",s));
                pan.text_item("");

                for(i=1; i<n; i++) 
                   pan.text_item(string("\\tt %s\\n",~L[i]));

                pan.text_item("");
                pan.button("ok");
                pan.open(window::center,window::center);

                close_display();
                exit(0);

                break;
               }
      }
     }
 }



//------------------------------------------------------------------------------
// WINDOW OUTPUT
//------------------------------------------------------------------------------


// pixels

void window::draw_pix(double x, double y, color c ) 
{ BASE_WINDOW::draw_pix(x,y,c); }

void window::draw_pix(const point& p, color c ) 
{ draw_pix(p.xcoord(),p.ycoord(),c); }


void window::draw_pixels(const list<point>& L, color c ) 
{ int n = L.length();
  double* xcoord = new double[n];
  double* ycoord = new double[n];
  int i = 0;
  point p;
  forall(p,L)
  { xcoord[i] = p.xcoord();
    ycoord[i] = p.ycoord();
    i++;
   }

  BASE_WINDOW::draw_pixels(n,xcoord,ycoord,c);

  delete[] xcoord;
  delete[] ycoord;
}


void window::draw_pixels(int n, double* xcoord, double* ycoord, color c ) 
{ BASE_WINDOW::draw_pixels(n,xcoord,ycoord,c); }





void window::draw_text(double x, double y, string s, color c)
{ BASE_WINDOW::draw_text(x,y,s,c); }

void window::draw_text(const point& p, string s, color c)
{ draw_text(p.xcoord(),p.ycoord(),s,c); }

void window::draw_ctext(double x, double y, string s, color c)
{ BASE_WINDOW::draw_ctext(x,y,s,c); }

void window::draw_ctext(const point& p, string s, color c)
{ draw_ctext(p.xcoord(),p.ycoord(),s,c); }

void window::draw_ctext(string s, color c)
{ BASE_WINDOW::draw_ctext(s,c); }

double window::text_box(double x0, double x1, double y0, string s, bool draw)
{ return BASE_WINDOW::text_box(x0,x1,y0,s,draw); }

void window::text_box(string s)
{ double d = pix_to_real(10);
  BASE_WINDOW::text_box(xmin()+d,xmax()-d,ymax()-d,s,true); 
 }

// points

void window::draw_point(double x0,double y0,color c)
{ BASE_WINDOW::draw_point(x0,y0,c); }

void window::draw_point(const point& p,color c)
{ draw_point(p.xcoord(),p.ycoord(),c); }


// segments

void window::draw_segment(double x1, double y1, double x2, double y2, color c )
{  if (show_orientation) 
     draw_arrow(x1,y1,x2,y2,c);
  else 
     BASE_WINDOW::draw_segment(x1,y1,x2,y2,c); 
}

void window::draw_segment(const point& p, const point& q, color c )
{ window::draw_segment(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }

void window::draw_segment(const segment& s, color c )
{ draw_segment(s.start(),s.end(),c); }

void window::draw_segments(const list<segment>& L, color c)
{ 
  if (show_orientation)
  { segment s;
    forall(s,L) draw_arrow(s,c);
    return;
   }

  int n = L.length();
  double* xcoord1 = new double[n];
  double* ycoord1 = new double[n];
  double* xcoord2 = new double[n];
  double* ycoord2 = new double[n];

  int i = 0;
  segment s;
  forall(s,L)
  { xcoord1[i] = s.xcoord1();
    ycoord1[i] = s.ycoord1();
    xcoord2[i] = s.xcoord2();
    ycoord2[i] = s.ycoord2();
    i++;
   }

  BASE_WINDOW::draw_segments(n,xcoord1,ycoord1,xcoord2,ycoord2,c);

  delete[] xcoord1;
  delete[] ycoord1;
  delete[] xcoord2;
  delete[] ycoord2;
}


// lines

void window::draw_line(double x1, double y1, double x2, double y2, color c )
{ BASE_WINDOW::draw_line(x1,y1,x2,y2,c);
  if (show_orientation) draw_arrow(x1,y1,x2,y2,c);
}

void window::draw_line(const point& p, const point& q, color c )
{ window::draw_line(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }

void window::draw_line(const segment& s, color c )
{ draw_line(s.start(),s.end(),c); }

void window::draw_line(const line& l, color c )
{ draw_line(l.seg(),c); }



void window::draw_hline(double y, color c )
{ BASE_WINDOW::draw_segment(xmin(),y,xmax(),y,c); }

void window::draw_vline(double x, color c )
{ BASE_WINDOW::draw_segment(x,ymin(),x,ymax(),c); }


// rays

void window::draw_ray(double x1, double y1, double x2, double y2, color c )
{ BASE_WINDOW::draw_ray(x1,y1,x2,y2,c); 
  if (show_orientation) draw_arrow(x1,y1,x2,y2,c);
} 

void window::draw_ray(const point& p, const point& q, color c )
{ window::draw_ray(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }

void window::draw_ray(const segment& s, color c )
{ draw_ray(s.start(),s.end(),c); }

void window::draw_ray(const ray& l, color c )
{ draw_ray(l.seg(),c); }


// segments and rays with hint

void window::draw_segment(point p, point q, line l, color col)
{ 
  if ( compare(p,q) > 0 ) leda_swap(p,q);
 
  bool bp = contains(p);
  bool bq = contains(q);

  if ( bp && bq ) { draw_segment(p,q,col); return; }

  segment s;
  point llc(xmin(),ymin()); // left lower corner
  point rrc(xmax(),ymax()); // right upper corner
  
  if ( !l.clip(llc,rrc,s) ) return;

  if ( bp && !bq ) draw_segment(p,s.target(),col);
    
  if ( !bp && bq ) draw_segment(q,s.source(),col);

  if ( !bp && !bq )
  { point a = p;
    point b = q;
    if (compare(p,s.source()) <= 0) a = s.source();
    if (compare(s.target(),q) <= 0) b = s.target();
    draw_segment(a,b,col);
  }
}

void window::draw_ray(point p, point q, line l, color col)
{  
  if ( contains(p) ) { draw_ray(p,q,col); return; }

  segment s;
  point llc(xmin(),ymin()); // left lower corner
  point rrc(xmax(),ymax()); // right upper corner
  
  if ( !l.clip(llc,rrc,s) ) return;

  if ((compare(p,s.source()) < 0 && compare(p,q) < 0) ||
      (compare(s.target(),p) < 0 && compare(q,p) < 0) ||
      (compare(s.source(),p) <= 0 && compare(p,s.target()) <= 0))
    draw_segment(s,col);
}




// nodes

void window::draw_node(double x0,double y0,color c) 
{ BASE_WINDOW::draw_node(x0,y0,c); }

void window::draw_node(const point& p, color c)
{ window::draw_node(p.xcoord(),p.ycoord(),c); }

void window::draw_filled_node(double x0,double y0,color c)
{ BASE_WINDOW::draw_filled_node(x0,y0,c); }

void window::draw_filled_node(const point& p, color c)
{ window::draw_filled_node(p.xcoord(),p.ycoord(),c); }

void window::draw_text_node(double x,double y,string s,color c)
{ BASE_WINDOW::draw_text_node(x,y,~s,c); }

void window::draw_text_node(const point& p ,string s,color c)
{ window::draw_text_node(p.xcoord(),p.ycoord(),~s,c); }

void window::draw_int_node(double x,double y,int i,color c)
{ BASE_WINDOW::draw_int_node(x,y,i,c); }

void window::draw_int_node(const point& p ,int i,color c)
{ window::draw_int_node(p.xcoord(),p.ycoord(),i,c); }


//circles

void window::draw_circle(double x,double y,double r,color c)
{ BASE_WINDOW::draw_circle(x,y,r,c); }

void window::draw_circle(const point& p,double r,color c)
{ BASE_WINDOW::draw_circle(p.xcoord(),p.ycoord(),r,c); }

void window::draw_circle(const circle& C,color c)
{ point p = C.center();
  double r = C.radius();
  BASE_WINDOW::draw_circle(p.xcoord(),p.ycoord(),r,c); 
 }


// discs

void window::draw_disc(double x,double y,double r,color c)
{ BASE_WINDOW::draw_filled_circle(x,y,r,c); }

void window::draw_disc(const point& p,double r,color c)
{ window::draw_disc(p.xcoord(),p.ycoord(),r,c); }


void window::draw_disc(const circle& C,color c)
{ draw_disc(C.center(),C.radius(),c); }



//ellipses

void window::draw_ellipse(double x,double y,double r1,double r2,color c)
{ BASE_WINDOW::draw_ellipse(x,y,r1,r2,c); }

void window::draw_ellipse(const point& p, double r1, double r2, color c)
{ BASE_WINDOW::draw_ellipse(p.xcoord(),p.ycoord(),r1,r2,c); }

void window::draw_filled_ellipse(double x,double y,double r1,double r2,color c)
{ BASE_WINDOW::draw_filled_ellipse(x,y,r1,r2,c); }

void window::draw_filled_ellipse(const point& p, double r1, double r2, color c)
{ BASE_WINDOW::draw_filled_ellipse(p.xcoord(),p.ycoord(),r1,r2,c); }



// arcs

void window::draw_arc(point p, point q, point r, int arrow, double w, color col)
{ 
  circle C(p,q,r);

  //int orient = orientation(p,q,r);
  double orient = (p.xcoord() - q.xcoord()) * (p.ycoord() - r.ycoord())
                - (p.ycoord() - q.ycoord()) * (p.xcoord() - r.xcoord()); 

  if ((p.distance(q)+q.distance(r))*scale() < 10)
  { BASE_WINDOW::draw_segment(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),col);
    BASE_WINDOW::draw_segment(q.xcoord(),q.ycoord(),r.xcoord(),r.ycoord(),col);
    return;
   }

  if (fabs(orient) < 1e-5 || C.radius() > 100*p.distance(r))
  { list<point> L;
    L.append(p);
    L.append(q);
    L.append(r);
    draw_polyline(L,arrow,w,col);
    return;
   }

  point  cen = C.center();
  double rad = C.radius();

  if ((arrow & 4) || (arrow & 8))
  { double phi = cen.angle(p,r);
    if (orient < 0) phi -= 2*LEDA_PI;
    point a = p.rotate(cen,0.25*phi);
    point b = p.rotate(cen,0.50*phi);
    point c = p.rotate(cen,0.75*phi);

    int arr1 = 0;
    int arr2 = 0;
    if (arrow == 4) { arr1 = 1; arr2 = 0; }
    if (arrow == 5) { arr1 = 1; arr2 = 2; }
    if (arrow == 6) { arr1 = 1; arr2 = 1; }
    if (arrow == 7) { arr1 = 3; arr2 = 2; }
    if (arrow == 8) { arr1 = 0; arr2 = 1; }
    if (arrow == 9) { arr1 = 0; arr2 = 3; }
    if (arrow ==10) { arr1 = 2; arr2 = 1; }
    if (arrow ==11) { arr1 = 2; arr2 = 3; }
    if (arrow ==12) { arr1 = 1; arr2 = 1; }
    if (arrow ==13) { arr1 = 1; arr2 = 3; }
    if (arrow ==14) { arr1 = 3; arr2 = 1; }
    if (arrow ==15) { arr1 = 3; arr2 = 3; }

    draw_arc(p,a,b,arr1,w,col);
    draw_arc(r,c,b,arr2,w,col);
    return;
   }

  if (arrow & 1)
  { segment s(r,cen);
    double d = LEDA_PI/2;
    if (orient > 0) d = -d;
    r = draw_arrow_head(s.source(),s.angle()+d,w,col);
   }


  if (arrow & 2)
  { segment s(p,cen);
    double d = LEDA_PI/2;
    if (orient < 0) d = -d;
    p = draw_arrow_head(s.source(),s.angle()+d,w,col);
  }

  point  ref = cen.translate(1,0);
  double start = cen.angle(ref,p);
  double angle = cen.angle(p,r);

  if (orient < 0) angle -= 2*LEDA_PI;

  BASE_WINDOW::draw_arc(cen.xcoord(),cen.ycoord(),rad,rad,start,angle,col);

}

void window::draw_arc(const point& p, const point& q, const point& r, color col)
{ draw_arc(p,q,r,0,0,col); }

void window::draw_arc_arrow(const point& p, const point& q, const point& r, 
                                                            color col)
{ double d = pix_to_real(get_line_width());
  //d *= 1.5;
  d *= 1.25;
  draw_arc(p,q,r,1,d,col); 
 }





// polygons

void window::draw_polyline(const list<point>& lp, int arrow, double d, color c)
{ int n = lp.length();

  if (n < 2) 
  { // LEDA_EXCEPTION(1,"window::draw_polyline: |lp| < 2");
    return;
   }

  if (arrow == 4 || arrow == 8)
  { int m = n/2;
    list_item it = lp.first();
    while (--m) it = lp.succ(it);
    point a = lp[it];
    it = lp.succ(it);
    point b = lp[it];
    point s = midpoint(a,b);
    segment seg;
    if (arrow & 4) seg = segment(a,s);
    if (arrow & 8) seg = segment(b,s);
    draw_polyline(lp,0,d,c);
    draw_arrow_head(s,seg.angle(),d,c);
    return;
   }

  if ((arrow & 4) || (arrow & 8))
  { int m = n/2;
    list<point> L = lp;
    list_item it = L.first();
    while (--m) it = L.succ(it);
    list<point> lp1,lp2;
    L.split(it,lp1,lp2,leda::behind);
    point p = lp1.tail();
    point q = lp2.head();
    point s = midpoint(lp1.tail(),lp2.head());
    lp1.reverse_items();
    lp1.push(s);
    lp2.push(s);
    int arr1 = (arrow & 4) ? 2 : 0;
    int arr2 = (arrow & 8) ? 2 : 0;
    draw_polyline(lp1,arr1,d,c);
    draw_polyline(lp2,arr2,d,c);
    return;
  }

/*
  if (arrow == 2)
  { list<point> lpr = lp;
    lpr.reverse_items();
    draw_polyline(lpr,1,d,c);
    return;
   }
*/

  double* X = new double[n];
  double* Y = new double[n];
  n = 0;
  point p;
  forall(p,lp) 
  { X[n] = p.xcoord();
    Y[n] = p.ycoord();
    n++;
   }

/*
  BASE_WINDOW::adjust_polyline(n,X,Y);
*/

  if (arrow & 1)
  { segment s(X[n-2],Y[n-2],X[n-1],Y[n-1]);
    point q = draw_arrow_head(s.target(),s.angle(),d,c);
    X[n-1] = q.xcoord();
    Y[n-1] = q.ycoord();
   }

  if (arrow & 2)
  { segment s(X[1],Y[1],X[0],Y[0]);
    point q = draw_arrow_head(s.target(),s.angle(),d,c);
    X[0] = q.xcoord();
    Y[0] = q.ycoord();
   }


  BASE_WINDOW::draw_polyline(n,X,Y,c);

  delete[] X;
  delete[] Y;
}

void window::draw_polyline(const list<point>& lp, color c)
{ draw_polyline(lp,0,0,c); }

void window::draw_polyline_arrow(const list<point>& lp, color c)
{ double d = pix_to_real(get_line_width());
  draw_polyline(lp,2,d,c); 
 }

void window::draw_polyline(const list<point>& lp, const list<color>& clr)
{ int n = lp.length();
  double* X = new double[n];
  double* Y = new double[n];
  int* C = new int[n];

  list_item it = clr.first();
  n = 0;
  point p;
  forall(p,lp) 
  { X[n] = p.xcoord();
    Y[n] = p.ycoord();
    C[n] = clr[it];
    it = clr.succ(it);
    n++;
   }

  BASE_WINDOW::draw_polyline(n,X,Y,C);
  delete[] X;
  delete[] Y;
  delete[] C;
}



void window::draw_polygon(const list<point>& lp, color c)
{ int n = lp.length();
  double* X = new double[n];
  double* Y = new double[n];
  n = 0;
  point p;
  forall(p,lp) 
  { X[n] = p.xcoord();
    Y[n] = p.ycoord();
    n++;
   }
  BASE_WINDOW::draw_polygon(n,X,Y,c);
  delete[] X;
  delete[] Y;
}

void window::draw_filled_polygon(const list<point>& lp, color c)
{ int n = lp.length();
  double* X = new double[n];
  double* Y = new double[n];
  n = 0;
  point p;
  forall(p,lp) 
  { X[n] = p.xcoord();
    Y[n] = p.ycoord();
    n++;
   }
  BASE_WINDOW::draw_filled_polygon(n,X,Y,c);
  delete[] X;
  delete[] Y;
}

void window::draw_polygon(const polygon& P, color c )
{ if (show_orientation)
    draw_oriented_polygon(P.vertices(),c);
  else
    draw_polygon(P.vertices(),c); 
}

void window::draw_filled_polygon(const polygon& P,color c )
{ draw_filled_polygon(P.vertices(),c); }

void window::draw_oriented_polygon(const polygon& P, color c )
{ draw_oriented_polygon(P.vertices(),c); }


void window::draw_oriented_polygon(const list<point>& lp, color c)
{ draw_polygon(lp,c);
  if (lp.size() < 2) return;
  point p = lp[lp[0]];
  point q = lp[lp[1]];
  segment s(p,midpoint(p,q));
  draw_arrow(s);
}

void window::draw_oriented_polygon(const gen_polygon& GP, color c)
{ if ( GP.trivial() ) return;
  polygon P;
  forall(P, GP.polygons() ) draw_oriented_polygon(P,c);
}


void window::draw_polygon(const gen_polygon& GP, color c)
{ if ( GP.trivial() ) return;
  polygon P;
  if (show_orientation) {
    draw_oriented_polygon(GP,c);
  }
  else {
   forall(P, GP.polygons() ) draw_polygon(P,c);
  }  
}
  
void window::clip_mask_polygon(int n, double* xc, double* yc, int mode)
{ BASE_WINDOW::clip_mask_polygon(n,xc,yc,mode); }

void window::clip_mask_polygon(const list<point>& L,int mode)
{ 
  int n = L.length();
  double* xc = new double[n];
  double* yc = new double[n];
  int i = 0;
  point p;
  forall(p,L)
  { xc[i] = p.xcoord();
    yc[i] = p.ycoord();
    i++;
   }

  BASE_WINDOW::clip_mask_polygon(n,xc,yc,mode);

  delete[] xc;
  delete[] yc;
}


void window::clip_mask_polygon(const polygon& pol,int mode) { 
  clip_mask_polygon(pol.vertices(), mode);
}


void window::draw_filled_polygon(const gen_polygon& GP, color c)
{
  if (GP.empty()) return;

  if (GP.full()) 
  { clear(c); 
    return; 
   }

  list<polygon> p_list = GP.polygons();

  polygon pol = p_list.head();

  if (p_list.size() == 1 && pol.orientation() > 0) 
  { draw_filled_polygon(pol,c);
    return;
   }

  double x0 = xmin_no_grid();
  double y0 = ymin_no_grid();
  double x1 = xmax_no_grid();
  double y1 = ymax_no_grid();

  if (is_buffering())
  { x0 = xreal(0);
    y0 = yreal(height());
    x1 = xreal(width());
    y1 = yreal(0);
   }

  list<point> frame;
  frame.append(point(x0,y0));
  frame.append(point(x1,y0));
  frame.append(point(x1,y1));
  frame.append(point(x0,y1));

  if (pol.orientation() > 0) frame.reverse_items();

  //p_list.push(polygon(frame);
  p_list.push(polygon(frame,polygon::NO_CHECK)); // not weakly simple (sn) ?

  clip_mask_window(1);

  forall(pol,p_list) 
    clip_mask_polygon(pol,pol.orientation() > 0);

/*
  forall(pol,p_list) 
  { const list<point>& L = pol.vertices();
    int n = L.length();
    double* xc = new double[n];
    double* yc = new double[n];
    int i = 0;
    point p;
    forall(p,L)
    { xc[i] = p.xcoord();
      yc[i] = p.ycoord();
      i++;
     }

    clip_mask_polygon(n,xc,yc,pol.orientation() > 0);

    delete[] xc;
    delete[] yc;
   }
*/

  draw_box(x0,y0,x1,y1,c);
  clip_mask_window(1);
}


void window::draw_rectangle(double x0,double y0,double x1,double y1, color col)
{ BASE_WINDOW::draw_rectangle(x0,y0,x1,y1,col); }

void window::draw_rectangle(point p, point q, color col)
{ BASE_WINDOW::draw_rectangle(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),col); }

void window::draw_rectangle(const rectangle& r, color c)
{ draw_rectangle(r.xmin(),r.ymin(),r.xmax(),r.ymax(),c); }



void window::draw_filled_rectangle(double x0,double y0,double x1,double y1, color col)
{ BASE_WINDOW::draw_filled_rectangle(x0,y0,x1,y1,col); }

void window::draw_filled_rectangle(point p, point q, color col)
{ BASE_WINDOW::draw_filled_rectangle(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),col); }

void window::draw_filled_rectangle(const rectangle& r, color c)
{ draw_box(r.xmin(),r.ymin(),r.xmax(),r.ymax(),c); }



void window::draw_triangle(point a, point b, point c, color col)
{ list<point> L;
  L.append(a);
  L.append(b);
  L.append(c);
  draw_polygon(polygon(L),col);  
}

void window::draw_triangle(const triangle& T, color col)
{ draw_triangle(T.point1(), T.point2(), T.point3(),col); }   


void window::draw_filled_triangle(point a, point b, point c, color col)
{ list<point> poly;
  poly.append(a);
  poly.append(b);
  poly.append(c);
  draw_filled_polygon(poly,col);
}
   
void window::draw_filled_triangle(const triangle& T, color col)
{ draw_filled_triangle(T.point1(), T.point2(), T.point3(),col); }   

// functions

void window::plot_xy(double x0, double x1, win_draw_func f, color c)
{ BASE_WINDOW::plot_xy(x0,x1,f,c); }

void window::plot_yx(double y0, double y1, win_draw_func f, color c)
{ BASE_WINDOW::plot_yx(y0,y1,f,c); }



// arrows

//static
point window::arrow_head(const point& q,double a,double d,double* X,double* Y)
{ 
  double alpha = a-LEDA_PI; 

  point l = q.translate_by_angle(alpha+LEDA_PI/6, 3.5*d);
  point m = q.translate_by_angle(alpha,           2.0*d);
  point r = q.translate_by_angle(alpha-LEDA_PI/6, 3.5*d);

  X[0] = q.xcoord();
  Y[0] = q.ycoord();
  X[1] = l.xcoord();
  Y[1] = l.ycoord();
  X[2] = m.xcoord();
  Y[2] = m.ycoord();
  X[3] = r.xcoord();
  Y[3] = r.ycoord();

  return m;
}


//static
int window::arrow(const point& a, const point& b,double d,int arr,double* X,double* Y)
{ 
  if (arr == 1) return arrow(b,a,d,2,X,Y);

  vector vec = (d/2)*((b-a).rotate90().norm());

  if (arr == 0)
  { X[0] = a.xcoord() + vec[0];
    Y[0] = a.ycoord() + vec[1];
    X[1] = b.xcoord() + vec[0];
    Y[1] = b.ycoord() + vec[1];
    X[2] = b.xcoord() - vec[0];
    Y[2] = b.ycoord() - vec[1];
    X[3] = a.xcoord() - vec[0];
    Y[3] = a.ycoord() - vec[1];
    return 4;
   }

  if (arr == 3)
  { point q = midpoint(a,b);
    int sz = arrow(q,b,d,2,X,Y);
    X += sz;
    Y += sz;
    sz += arrow(q,a,d,2,X,Y);
    return sz;
   }

  double alpha = segment(a,b).angle()-LEDA_PI; 
  point l = b.translate_by_angle(alpha-LEDA_PI/6, 3.5*d);
  point m = b.translate_by_angle(alpha,           2.0*d);
  point r = b.translate_by_angle(alpha+LEDA_PI/6, 3.5*d);

  X[0] = a.xcoord() + vec[0];
  Y[0] = a.ycoord() + vec[1];
  X[1] = m.xcoord() + vec[0];
  Y[1] = m.ycoord() + vec[1];
  X[2] = l.xcoord();
  Y[2] = l.ycoord();
  X[3] = b.xcoord();
  Y[3] = b.ycoord();
  X[4] = r.xcoord();
  Y[4] = r.ycoord();
  X[5] = m.xcoord() - vec[0];
  Y[5] = m.ycoord() - vec[1];
  X[6] = a.xcoord() - vec[0];
  Y[6] = a.ycoord() - vec[1];
  return 7;
}


static int poly_parallel(const list<point>& L, double d,double* X,double* Y,
                                               bool omit_first = false)
{
  double *xp = X;
  double *yp = Y;

  if (d <=0)
  { point p;
    forall(p,L)
    { if (!omit_first || xp != X)
      { *xp = p.xcoord();
        *yp = p.ycoord();
       }
      xp++;
      yp++;
     }
     return int(xp-X);
   }

  point last;

  list_item it;
  forall_items(it,L)
  { point p = L[it];

    if (it == L.first())
    { if (omit_first)
      { xp++;
        yp++;
       }
      else
      { point q = L[L.succ(it)];
        vector vec = (d/2)*((q-p).rotate90().norm());
        *xp++ = p.xcoord() + vec[0];
        *yp++ = p.ycoord() + vec[1];
       }
      last = p;
      continue;
     }

    if (it == L.last())
    { vector vec = (d/2)*((p-last).rotate90().norm());
      *xp++ = p.xcoord() + vec[0];
      *yp++ = p.ycoord() + vec[1];
      last = p;
      continue;
     }

    point q = L[L.succ(it)];

    vector vec1 = (d/2)*((p-last).rotate90().norm());
    vector vec2 = (d/2)*((q-p).rotate90().norm());

    double phi = p.angle(q,last);

    if (fabs(phi) < 0.05 || fabs(phi-LEDA_PI) < 0.05) 
     { p = p + 0.5*(vec1+vec2);
       *xp++ = p.xcoord();
       *yp++ = p.ycoord();
      }
    else
     if (phi > LEDA_PI) 
     { double A = phi - LEDA_PI;
       int steps = int(16*A/LEDA_PI);
       if (steps < 2) steps = 2;
       if (steps % 2) steps++;
       double delta = -A/steps;
       point p1 = p + vec1;
       *xp++ = p1.xcoord();
       *yp++ = p1.ycoord();
       for(int i=1; i<steps; i++)
       {  point r = p1.rotate(p,i*delta);
         *xp++ = r.xcoord();
         *yp++ = r.ycoord();
        }
        point p2 = p + vec2;
       *xp++ = p2.xcoord();
       *yp++ = p2.ycoord();
      }
    else
      { segment seg1(last+vec1,p+vec1);
        segment seg2(p+vec2,q+vec2);
        seg1.intersection_of_lines(seg2,p);
        *xp++ = p.xcoord();
        *yp++ = p.ycoord();
       }

    last = L[it];
   }

  return int(xp-X);
}


int window::arrow(const list<point>& L, double d, int arr,double* X,double* Y)
{
  int n = poly_parallel(L,d,X,Y);

  if (arr & 1)
  { list_item it2 = L.last();
    list_item it1 = L.pred(it2);
    point a = L[it1];
    point b = L[it2];
    vector vec = (d/2)*((b-a).rotate90().norm());
    double alpha = segment(a,b).angle()-LEDA_PI; 
    point l = b.translate_by_angle(alpha-LEDA_PI/6, 3.5*d);
    point m = b.translate_by_angle(alpha,           2.0*d);
    point r = b.translate_by_angle(alpha+LEDA_PI/6, 3.5*d);
    n--;
    X[n] = m.xcoord() + vec[0];
    Y[n] = m.ycoord() + vec[1];
    X[n+1] = l.xcoord();
    Y[n+1] = l.ycoord();
    X[n+2] = b.xcoord();
    Y[n+2] = b.ycoord();
    X[n+3] = r.xcoord();
    Y[n+3] = r.ycoord();
    X[n+4] = m.xcoord() - vec[0];
    Y[n+4] = m.ycoord() - vec[1];
    n += 4;
   }

  list<point> R = L;
  R.reverse_items();
  n += poly_parallel(R,d,X+n,Y+n,(arr&1));

  if (arr & 2)
  { list_item it2 = R.last();
    list_item it1 = R.pred(it2);
    point a = R[it1];
    point b = R[it2];
    vector vec = (d/2)*((b-a).rotate90().norm());
    double alpha = segment(a,b).angle()-LEDA_PI; 
    point l = b.translate_by_angle(alpha-LEDA_PI/6, 3.5*d);
    point m = b.translate_by_angle(alpha,           2.0*d);
    point r = b.translate_by_angle(alpha+LEDA_PI/6, 3.5*d);
    n--;
    X[n] = m.xcoord() + vec[0];
    Y[n] = m.ycoord() + vec[1];
    X[n+1] = l.xcoord();
    Y[n+1] = l.ycoord();
    X[n+2] = b.xcoord();
    Y[n+2] = b.ycoord();
    X[n+3] = r.xcoord();
    Y[n+3] = r.ycoord();
    X[0] = m.xcoord() - vec[0];
    Y[0] = m.ycoord() - vec[1];
    n += 4;
   }

  return n;
}



point window::draw_arrow_head(const point& q, double a, double d, color c)
{ 
  // construct and draw arrow head

  double pix = pix_to_real(1);

//d *= 1.5*(screen_dpi()/96.0);
//d *= 1.2*(screen_dpi()/96.0);
//d *= 1.0*(screen_dpi()/96.0);

//d *= 2;
  d *= 1.75;

  if (d < pix) d = pix;


/*
  double X[4];
  double Y[4];
  point m = arrow_head(q,a,d,X,Y);
  BASE_WINDOW::draw_filled_polygon(4,X,Y,c);
  int lw = set_line_width(1);
  BASE_WINDOW::draw_polygon(4,X,Y,c);
  set_line_width(lw);
*/


  double alpha = a-LEDA_PI; 

//point l = q.translate_by_angle(alpha+LEDA_PI/6, 3.5*d);
  point l = q.translate_by_angle(alpha+LEDA_PI/6, 4*d);

  point m = q.translate_by_angle(alpha,           2.0*d);

//point r = q.translate_by_angle(alpha-LEDA_PI/6, 3.5*d);
  point r = q.translate_by_angle(alpha-LEDA_PI/6, 4*d);

  draw_filled_triangle(m,l,q,c);
  draw_filled_triangle(m,r,q,c);

  int lw = set_line_width(1);
  draw_triangle(m,l,q,c);
  draw_triangle(m,r,q,c);
  set_line_width(lw);

  return m;
}


point window::draw_arrow_head(const point& q, double a, color c)
{ int lw = get_line_width();
  double d = pix_to_real(lw);
  return draw_arrow_head(q,a,d,c);
}


void window::draw_arrow(const segment& s, color c)
{ point q = draw_arrow_head(s.end(),s.angle(),c);
  BASE_WINDOW::draw_segment(s.xcoord1(),s.ycoord1(),q.xcoord(),q.ycoord(),c);
}


void window::draw_filled_arrow(const list<point>& L, int arrow, double d, 
                                                     bool border, color c)
{ 

  if (arrow >= 4)
  { int n = L.length();
    int m = n/2;
    list<point> lp = L;
    list_item it = lp.first();
    while (--m) it = lp.succ(it);
    list<point> lp1,lp2;
    lp.split(it,lp1,lp2,leda::behind);
    point p = lp1.tail();
    point q = lp2.head();
    point s = midpoint(p,q);
    lp1.append(s);
    lp2.push(s);


    if (arrow == 4)
    { lp2.push(midpoint(p,s));
      draw_filled_arrow(lp2,0,d,border,c);
      draw_filled_arrow(lp1,1,d,border,c);
     }

    if (arrow == 5)
    { lp2.push(midpoint(p,s));
      draw_filled_arrow(lp2,1,d,border,c);
      draw_filled_arrow(lp1,1,d,border,c);
     }

    if (arrow == 6)
    { lp2.push(midpoint(p,s));
      draw_filled_arrow(lp2,0,d,border,c);
      draw_filled_arrow(lp1,3,d,border,c);
     }

    if (arrow == 7)
    { lp2.push(midpoint(p,s));
      draw_filled_arrow(lp2,1,d,border,c);
      draw_filled_arrow(lp1,3,d,border,c);
     }

    if (arrow == 8)
    { lp1.append(midpoint(s,q));
      draw_filled_arrow(lp1,0,d,border,c);
      draw_filled_arrow(lp2,2,d,border,c);
     }

    if (arrow == 9)
    { lp1.append(midpoint(s,q));
      draw_filled_arrow(lp1,0,d,border,c);
      draw_filled_arrow(lp2,3,d,border,c);
     }
    
    if (arrow == 10)
    { lp1.append(midpoint(s,q));
      draw_filled_arrow(lp1,2,d,border,c);
      draw_filled_arrow(lp2,2,d,border,c);
     }
    
    if (arrow == 11)
    { lp1.append(midpoint(s,q));
      draw_filled_arrow(lp1,2,d,border,c);
      draw_filled_arrow(lp2,3,d,border,c);
     }
    
    if (arrow == 12 )
    { draw_filled_arrow(lp1,1,d,border,c);
      draw_filled_arrow(lp2,2,d,border,c);
     }

    if (arrow == 13 )
    { draw_filled_arrow(lp1,1,d,border,c);
      draw_filled_arrow(lp2,3,d,border,c);
     }

    if (arrow == 14 )
    { draw_filled_arrow(lp1,3,d,border,c);
      draw_filled_arrow(lp2,2,d,border,c);
     }

    if (arrow == 15 )
    { draw_filled_arrow(lp1,3,d,border,c);
      draw_filled_arrow(lp2,3,d,border,c);
     }

    return;
  }

  if (!border)
  { draw_polyline(L,arrow,d,c);
    return;
  }

  double pix = pix_to_real(1);

  if (d < pix)
     d = 1.5*d + pix;
  else
     d =  d + 1.5*pix;


  int sz = 16 * L.size();
  double* X = new double[sz];
  double* Y = new double[sz];

  int n = window::arrow(L,d,arrow,X,Y);

  BASE_WINDOW::draw_filled_polygon(n,X,Y,c);

  if (border)
  { int lw = set_line_width(1);
    BASE_WINDOW::draw_polygon(n,X,Y,black);
    set_line_width(lw);
   }

  delete[] X;
  delete[] Y;

}




void window::draw_arrow(const point& p, const point& q, color c)
{ draw_arrow(segment(p,q),c); }

void window::draw_arrow(double x0, double y0, double x1, double y1, color c)
{ draw_arrow(segment(x0,y0,x1,y1),c); }




// edges

void window::draw_edge(double x0, double y0, double x1, double y1, color c)
{ BASE_WINDOW::draw_edge(x0,y0,x1,y1,c); }

void window::draw_edge(const point& p, const point& q, color c)
{ draw_edge(p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),c); }

void window::draw_edge(const segment& s, color c)
{ draw_edge(s.start(),s.end(),c); }


void window::draw_edge_arrow(double x0,double y0,double x1,double y1,color c)
{ draw_edge_arrow(segment(x0,y0,x1,y1),c); }

void window::draw_edge_arrow(const point& p, const point& q, color c)
{ draw_edge_arrow(segment(p,q),c); }

void window::draw_edge_arrow(const segment& s, color c)
{ double  alpha = s.angle();
  point p = s.start().translate_by_angle(alpha,get_node_width()/scale());
  point q = s.end().translate_by_angle(alpha,-get_node_width()/scale());
  draw_arrow(p,q,c);
}


//------------------------------------------------------------------------------
// WINDOW INPUT
//------------------------------------------------------------------------------

int window::get_mouse(unsigned long timeout) 
{ return BASE_WINDOW::get_mouse(timeout); }

int window::get_mouse(double& x, double& y, unsigned long timeout) 
{ return BASE_WINDOW::get_mouse(x,y,timeout); }

int window::get_mouse() 
{ return BASE_WINDOW::get_mouse(); }

int window::get_mouse(double& x, double& y) 
{ return BASE_WINDOW::get_mouse(x,y); }

int  window::get_mouse(point& q)
{ double x,y;
  int but = BASE_WINDOW::get_mouse(x,y);
  q = point(x,y);
  return but;
 }


int window::read_mouse() 
{ return BASE_WINDOW::read_mouse(); }

int  window::read_mouse(double& x, double& y)
{ return BASE_WINDOW::read_mouse(0,0.0,0.0,x,y); }

int  window::read_mouse(point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(0,0.0,0.0,x,y);
  q = point(x,y);
  return but;
 }


int  window::read_mouse_seg(double x0, double y0, double& x, double& y)
{ return BASE_WINDOW::read_mouse(1,x0,y0,x,y); }

int  window::read_mouse_seg(const point& p, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(1,p.xcoord(),p.ycoord(),x,y);
  q = point(x,y);
  return but;
}


int  window::read_mouse_ray(double x0, double y0, double& x, double& y)
{ return BASE_WINDOW::read_mouse(2,x0,y0,x,y); }

int  window::read_mouse_ray(const point& p, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(2,p.xcoord(),p.ycoord(),x,y);
  q = point(x,y);
  return but;
}


int  window::read_mouse_line(double x0, double y0, double& x, double& y)
{ return BASE_WINDOW::read_mouse(3,x0,y0,x,y); }

int  window::read_mouse_line(const point& p, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(3,p.xcoord(),p.ycoord(),x,y);
  q = point(x,y);
  return but;
}


int  window::read_mouse_circle(double x0, double y0, double& x, double& y)
{ return BASE_WINDOW::read_mouse(4,x0,y0,x,y); }

int  window::read_mouse_circle(const point& p, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(4,p.xcoord(),p.ycoord(),x,y);
  q = point(x,y);
  return but;
}


int  window::read_mouse_rect(double x0, double y0, double& x, double& y)
{ return BASE_WINDOW::read_mouse(5,x0,y0,x,y); }

int  window::read_mouse_rect(const point& p, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(5,p.xcoord(),p.ycoord(),x,y);
  q = point(x,y);
  return but;
}


int  window::read_mouse_arc(double x0, double y0, double x1, double y1, double& x, double& y)
{ return BASE_WINDOW::read_mouse(6,x0,y0,x1,y1,x,y); }

int  window::read_mouse_arc(const point& p, const point& q, point& r)
{ double x,y;
  int but = BASE_WINDOW::read_mouse(6,p.xcoord(),p.ycoord(),q.xcoord(),q.ycoord(),x,y);
  r = point(x,y);
  return but;
}


int window::read_mouse_action(mouse_action_func f, double& x, double& y)
{ return BASE_WINDOW::read_mouse_action(f,0.0,0.0,x,y); }

int window::read_mouse_action(mouse_action_func f, point& q)
{ double x,y;
  int but = BASE_WINDOW::read_mouse_action(f,0.0,0.0,x,y);
  q = point(x,y);
  return but;
}


window& window::draw(const point& p, color c)
{ draw_point(p,c); 
  return *this;
}

window& window::draw(const segment& s, color c)
{ draw_segment(s,c); 
  return *this;
}

window& window::draw(const ray& r, color c)
{ draw_ray(r,c); 
  return *this;
}

window& window::draw(const line& l, color c)
{ draw_line(l,c); 
  return *this;
}

window& window::draw(const circle& C, color c)
{ if (! C.is_degenerate()) {
    draw_disc(C,get_fill_color());
    draw_circle(C,c);
  }
  else if (C.is_line()) draw_line(C.to_line(), c);
  else draw_point(C.point1(), c);
  return *this;
}

window& window::draw(const polygon& P, color c)
{ draw_filled_polygon(P,get_fill_color());
  draw_polygon(P,c); 
  return *this;
}

window& window::draw(const gen_polygon& P, color c)
{ draw_filled_polygon(P,get_fill_color());
  draw_polygon(P,c); 
  return *this;
}

window& window::draw(const rectangle& R, color c)
{ draw_box(R,get_fill_color());
  draw_rectangle(R,c); 
  return *this;
}

window& window::draw(const triangle& T, color c)
{ draw_filled_triangle(T,get_fill_color());
  draw_triangle(T,c); 
  return *this;
}

void window::set_point_buffer(const point p)
{ if (point_buffer) delete point_buffer;
  point_buffer = new point(p);
 }


window& window::read(point& p)
{ 
  if (point_buffer)
  { p = *point_buffer;
    delete point_buffer;
    point_buffer = 0;
    return *this;
   }

  double x,y;
  state = 1;
  if (read_mouse(x,y) == MOUSE_BUTTON(1)) 
     p = point(x,y);
  else
     state = 0;
  return *this;
 }


window& window::read(segment& s)
{ double x,y;
  point p;
  int key = 0;
  state = 1;

  if (!read(p).state) return *this;

  for(;;)
  { key = read_mouse_seg(p.xcoord(),p.ycoord(),x,y);
    if (key == MOUSE_BUTTON(3))  
     { state = 0;
       break; 
      }
    if (key == MOUSE_BUTTON(1) && !(shift_key_down() && read(p).state)) break; 
   }

  if (state) s = segment(p,point(x,y));

  return *this;
}

window& window::read(ray& r)
{ double x,y;
  point p;
  int key = 0;
  state = 1;

  if (!read(p).state) return *this;

  for(;;)
  { key = read_mouse_ray(p.xcoord(),p.ycoord(),x,y);
    if (key == MOUSE_BUTTON(3))  
     { state = 0;
       break; 
      }
    if (key == MOUSE_BUTTON(1) && !(shift_key_down() && read(p).state)) break; 
   }
  if (state) r = ray(p,point(x,y));
  return *this;
}


window& window::read(line& l)
{ double x,y;
  point p;
  int key = 0;
  state = 1;

  if (!read(p).state) return *this;

  while ((key=read_mouse_line(p.xcoord(),p.ycoord(),x,y)) != MOUSE_BUTTON(1))
  { if (key == MOUSE_BUTTON(3))  
     { state = 0;
       break; 
      }
    if (key == MOUSE_BUTTON(1) && shift_key_down() && !read(p).state) break;
   }

  if (state) l = line(p,point(x,y));

  return *this;
}


window& window::read(circle& c)
{ double x,y;
  point p;
  int key = 0;
  state = 1;

  if (!read(p).state) return *this;

  while ((key=read_mouse_circle(p.xcoord(),p.ycoord(),x,y)) != MOUSE_BUTTON(1))
  { if (key == MOUSE_BUTTON(3))  
     { state = 0;
       break; 
      }
    if (key == MOUSE_BUTTON(1) && shift_key_down() &&!read(p).state) break;
   }

  if (state) 
  { double dx = x-p.xcoord();
    double dy = y-p.ycoord();
    c = circle(p,sqrt(dx*dx+dy*dy));
   }

  return *this;
}


list<point> window::read_polygon()
{ double x,y;
  state = 1;
  point first,last,p;
  list<point> pl;

  if (!read(first).state) return pl;

  bool buf = is_buffering();
  if (buf) stop_buffering();

  pl.append(first);

  p = first;

  drawing_mode save = set_mode(xor_mode);

  while (read_mouse_seg(p.xcoord(),p.ycoord(),x,y) == MOUSE_BUTTON(1))
  { 
    if (!shift_key_down()) 
      { point q(x,y);
        draw_segment(p,q);
        pl.append(q);
        p = q;
       }
     else 
       if (pl.length() > 1 ) 
       { point l = pl.Pop();
         draw_segment(pl.tail(),l);
         p = pl.tail();
        }
  }

  //draw_segment(first,p);

  list_item it;
  forall_items(it,pl) 
  { list_item it1 = pl.succ(it);
    if (it1 ) draw_segment(pl[it],pl[it1]);
   }

  set_mode(save);

  if (buf) start_buffering();

  return pl;
}

window& window::read(gen_polygon& GP)
{
  GP = gen_polygon();
  polygon in, piter;
  list<polygon> polys;


  int but;
  unsigned long t;
  double x,y;

  bool buf = is_buffering();
  if (buf) stop_buffering();

  color fprev = set_fill_color(invisible);

  drawing_mode save = set_mode(xor_mode);

/*
  message("Input polygons of a gen_polygon, holes in clockwise order; Quit- Middle mouse button");
*/

  bool first_run = true;
  
  do {
   read_event(but, x, y, t);
 
   if (but != MOUSE_BUTTON(2)) {
    read(in); (*this) << in;
    polys.append(in);
    if (! in.empty() || in.orientation()==0) {
       if (in.orientation()==1) GP = GP.unite(gen_polygon(in));
       else {
        if (first_run) GP = gen_polygon(gen_polygon_rep::FULL);
        GP = GP.diff(gen_polygon(in.complement()));
       }
    }
   }
   first_run = false;
   
  } while (but != MOUSE_BUTTON(2));

  forall(piter, polys){
    (*this) << piter; 
  }
 
  set_mode(save);

  if (buf) start_buffering();

  
/*
  del_message();
*/
  set_fill_color(fprev);
  
  return *this;
}

window& window::read(polygon& P)
{ //P = read_polygon();
  P = polygon(read_polygon(), polygon::NO_CHECK);
  return *this;
 }


window& window::read(rectangle& r)
{
  double xw,yw;
  int key;
  point p1;

  state = 1;

  if (!read(p1).state) return *this;

  while ((key=read_mouse_rect(p1.xcoord(),p1.ycoord(),xw,yw))!=MOUSE_BUTTON(1))
  { if (key == MOUSE_BUTTON(3))  
    { state = 0;
      break; 
     }
    if (key == MOUSE_BUTTON(1) && shift_key_down() && !read(p1).state) break;
   }

  if (state) r = rectangle(p1,point(xw,yw));

  return *this;
}

window& window::read(triangle& t)
{ double x,y;
  state = 1;
  point first,last,p;
  list<point> pl;

  if (!read(first).state) return *this;

  bool buf = is_buffering();
  if (buf) stop_buffering();

  pl.append(first);

  p = first;

  drawing_mode save = set_mode(xor_mode);

  while ((pl.size()!=3) && (read_mouse_seg(p.xcoord(),p.ycoord(),x,y) == MOUSE_BUTTON(1)))
  { 
    if (!shift_key_down()) 
      { point q(x,y);
        draw_segment(p,q);
        pl.append(q);
        p = q;
       }
     else 
       if (pl.length() > 1 ) 
       { point l = pl.Pop();
         draw_segment(pl.tail(),l);
         p = pl.tail();
        }
  }

  list_item it;
  forall_items(it,pl) 
  { list_item it1 = pl.succ(it);
    if (it1 ) draw_segment(pl[it],pl[it1]);
   }

  set_mode(save);

  if (buf) start_buffering();

  if (pl.size() != 3) return *this;
  
  point p1 = pl.pop(), p2 = pl.pop(), p3 = pl.pop();
  t = triangle(p1,p2,p3);
  
  return *this;
}

int window::read_mouse_arc_with_fixed_src(const point& src, point& tgt, point& middle)
{
	int num_points = 1;
	int but = 0;
	do {
		switch (num_points) {
		case 1: but = read_mouse_seg(src, tgt);
				if (! shift_key_down()) { 
					++num_points;
					if (src == tgt || ctrl_key_down()) { middle = src; ++num_points; }
						// create trivial segment / straight line segment
				} 
				else return but;
				break;
		case 2: but = read_mouse_arc(src, tgt, middle);
				if (! shift_key_down()) ++num_points; else --num_points;
				break;
		}
	} while (num_points < 3 && but == MOUSE_BUTTON(1));

	return but;
}

window& window::read_arc(point& src, point& tgt, point& middle)
{
	int num_points = 0;
	int but = 0;
	do {
		switch (num_points) {
		case 0:	but = read_mouse(src); 
				if (! shift_key_down()) ++num_points;
				break;
		case 1: but = read_mouse_arc_with_fixed_src(src, tgt, middle);
				if (! shift_key_down()) num_points += 2; else --num_points;
				break;
		}
	} while (num_points < 3 && but == MOUSE_BUTTON(1));

	if (but == MOUSE_BUTTON(1)) set_state(1); else set_state(0);
	return *this;
}


window& window::operator>>(point& p)    
{ set_tmp_label(">> POINT");
  read(p); 
  reset_frame_label();
  return *this; 
 }

window& window::operator>>(segment& s)  
{ set_tmp_label(">> SEGMENT");
  read(s); 
  reset_frame_label();
  return *this; 
 }


window& window::operator>>(ray& r)  
{ set_tmp_label(">> RAY");
  read(r); 
  reset_frame_label();
  return *this; 
 }

window& window::operator>>(line& l)     
{ set_tmp_label(">> LINE");
  read(l); 
  reset_frame_label();
  return *this; 
 }

window& window::operator>>(circle& C)   
{ set_tmp_label(">> CIRCLE");
  read(C); 
  reset_frame_label();
  return *this; 
 }

window& window::operator>>(polygon& P)  
{ set_tmp_label(">> POLYGON");
  read(P); 
  reset_frame_label();
  return *this; 
 }
/*
window& window::operator>>(gen_polygon& P)
{ polygon Q;
  operator>>(Q);
  P = gen_polygon(Q);
  return *this;
}
*/

window& window::operator>>(gen_polygon& P)
{ set_tmp_label(">> GEN_POLYGON");
  read(P); 
  reset_frame_label();
  return *this;
}


window& window::operator>>(rectangle& r) 
{ set_tmp_label(">> RECTANGLE");
  read(r);
  reset_frame_label();
  return *this;
}

window& window::operator>>(triangle& t) 
{ set_tmp_label(">> TRIANGLE");
  read(t);
  reset_frame_label();
  return *this;
}

bool window::confirm(string s)
{ panel p;
  p.text_item("");
  p.text_item("\\bf\\blue " + s);
  p.text_item("");
  p.fbutton("yes",1);
  p.button("no",0);
  return p.open(*this) == 1;
}


void window::acknowledge(string s)
{ panel p;
  p.text_item("");
  p.text_item("\\bf\\blue " + s);
  p.text_item("");
  p.fbutton("ok");
  p.open(*this);
}

void window::notice(string s) { acknowledge(s); }


int  window::read_panel(string header, int n, string* L)
{ panel P("LEDA PANEL");
  P.text_item(header);
  for(int i = 0; i < n; i++) P.button(L[i]);
  return P.open(*this);
 }


int  window::read_vpanel(string header, int n, string* L)
{ panel P("LEDA PANEL");
  P.buttons_per_line(1);
  P.text_item(header);
  for(int i = 0; i < n; i++)  P.button(L[i]);
  return P.open(*this);
 }

void  window::panel_read(string prompt, string& x)
{ panel P("STRING INPUT PANEL");
  P.string_item(prompt,x);
  P.fbutton("continue");
  P.open(*this);
 }

void  window::panel_read(string prompt, int& x)
{ panel P("INT INPUT PANEL");
  P.int_item(prompt,x);
  P.fbutton("continue");
  P.open(*this);
}

void  window::panel_read(string prompt, double& x)
{ panel P("DOUBLE INPUT PANEL");
  P.real_item(prompt,x);
  P.fbutton("continue");
  P.open(*this);
 }



string  window::read_string(string prompt)
{ string s;
  panel_read(prompt,s);
  return s;
 }

int  window::read_int(string prompt)
{ int i = 0;
  panel_read(prompt,i);
  return i;
}

double  window::read_real(string prompt)
{ double x = 0;
  panel_read(prompt,x);
  return x;
 }



//------------------------------------------------------------------------------
//   PANEL OPERATIONS
//------------------------------------------------------------------------------


int window::button(string s, const char* hlp)   
{ panel_action_func f= 0;
  return BASE_WINDOW::button(s,-1,f,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, const char* hlp)
{ panel_action_func f= 0;
  return BASE_WINDOW::button(w,h,bm,s,-1,f,hlp); }


int window::button(string s,int v, const char* hlp)  
{ panel_action_func f= 0;
  return BASE_WINDOW::button(s,v,f,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, int v, 
                                                              const char* hlp)
{ panel_action_func f= 0;
  return BASE_WINDOW::button(w,h,bm,s,v,f,hlp); }


int window::button(string s, panel_action_func F, const char* hlp)   
{ return BASE_WINDOW::button(s,-1,F,hlp); }

int window::button(string s, const window_handler& obj, const char* hlp)   
{ panel_action_func f= 0;
  int bt = BASE_WINDOW::button(s,-1,f,hlp);
  BASE_WINDOW::set_action(bt,obj);
  return bt; 
}



int window::button(int w, int h, unsigned char* bm, string s, 
                                                    panel_action_func F, 
                                                    const char* hlp)   
{ return BASE_WINDOW::button(w,h,bm,s,-1,F,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, 
                                                    const window_handler& obj, 
                                                    const char* hlp)   
{ 
  panel_action_func f= 0;
  int bt = BASE_WINDOW::button(w,h,bm,s,-1,f,hlp); 
  BASE_WINDOW::set_action(bt,obj);
  return bt;
}




int window::button(string s, int v, panel_action_func F, const char* hlp)   
{ return BASE_WINDOW::button(s,v,F,hlp); }

int window::button(string s, int v, const window_handler& obj, const char* hlp)   
{ panel_action_func f= 0;
  int bt = BASE_WINDOW::button(s,v,f,hlp); 
  BASE_WINDOW::set_action(bt,obj);
  return bt;
}


int window::button(int w, int h, unsigned char* bm, string s, int v, 
                                                         panel_action_func F,
                                                         const char* hlp)
{ return BASE_WINDOW::button(w,h,bm,s,v,F,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, int v, 
                                                         const window_handler& obj,
                                                         const char* hlp)
{ panel_action_func f= 0;
  int bt = BASE_WINDOW::button(w,h,bm,s,v,f,hlp); 
  BASE_WINDOW::set_action(bt,obj);
  return bt;
}


int window::button(string s, window& p, const char* hlp)   
{ return BASE_WINDOW::button(s,-1,(BASE_WINDOW*)&p,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, window& p, 
                                                              const char* hlp)
{ return BASE_WINDOW::button(w,h,bm,s,-1,(BASE_WINDOW*)&p,hlp); }


int window::button(string s, int val, window& p, const char* hlp)   
{ return BASE_WINDOW::button(s,val,(BASE_WINDOW*)&p,hlp); }

int window::button(int w, int h, unsigned char* bm, string s, int val, 
                                                              window& p,
                                                              const char* hlp)
{ return BASE_WINDOW::button(w,h,bm,s,val,(BASE_WINDOW*)&p,hlp); }


int window::menu_button(string s, int val, window& p, const char* hlp)   
{ return BASE_WINDOW::menu_button(s,val,(BASE_WINDOW*)&p,hlp); }

int window::menu_button(string s, window& p, const char* hlp)   
{ return BASE_WINDOW::menu_button(s,-1,(BASE_WINDOW*)&p,hlp); }


// pixmaps

/*
int window::button(char* pr0, char* pr1, string s, const char* hlp)
{ panel_action_func F=0;
  return BASE_WINDOW::button(pr0,pr1,s,-1,F,hlp); }
*/

int window::button(char* pr0, char* pr1, string s, int v, const char* hlp)
{ panel_action_func F=0;
  return BASE_WINDOW::button(pr0,pr1,s,v,F,hlp); }

int window::button(char* pr0, char* pr1, string s, panel_action_func F,
                                                          const char* hlp)
{ return BASE_WINDOW::button(pr0,pr1,s,-1,F,hlp); }

int window::button(char* pr0, char* pr1, string s, const window_handler& obj,
                                                          const char* hlp)
{ panel_action_func f= 0;
  int bt = BASE_WINDOW::button(pr0,pr1,s,-1,f,hlp); 
  BASE_WINDOW::set_action(bt, obj);
  return bt;
}



int window::button(char* pr0, char* pr1, string s, int v, panel_action_func F, 
                                                          const char* hlp)
{ return BASE_WINDOW::button(pr0,pr1,s,v,F,hlp); }

int window::button(char* pr0, char* pr1, string s, int v, const window_handler& obj, 
                                                          const char* hlp)
{ panel_action_func f= 0;
  int bt = BASE_WINDOW::button(pr0,pr1,s,v,f,hlp); 
  BASE_WINDOW::set_action(bt, obj);
  return bt;
}



int window::button(char* pr0, char* pr1, string s, window& p, const char* hlp)
{ return BASE_WINDOW::button(pr0,pr1,s,-1,(BASE_WINDOW*)&p,hlp); }


int window::button(char* pr0, char* pr1, string s, int val, window& p, 
                                                            const char* hlp)
{ return BASE_WINDOW::button(pr0,pr1,s,val,(BASE_WINDOW*)&p,hlp); }


// focus buttons

int window::fbutton(string s, int n, const char* hlp)
{ int b = button(s,n,hlp);
  set_focus_button(b);
  return b;
}

int window::fbutton(string s, const char* hlp)
{ int b = button(s,hlp);
  set_focus_button(b);
  return b;
}

int window::fbutton(string s, int n, void (*F)(int), const char* hlp)
{ int b = button(s,n,F,hlp);
  set_focus_button(b);
  return b;
}

int window::fbutton(string s, int n, const window_handler& obj, const char* hlp)
{ int b = button(s,n,obj,hlp);
  set_focus_button(b);
  return b;
}


window* window::get_window(int but)
{ return (window*)BASE_WINDOW::get_window(but); }

window* window::set_window(panel_item it, window* M)
{ return (window*)BASE_WINDOW::set_window(it, M); }

window* window::set_window(int but, window* M)
{ return (window*)BASE_WINDOW::set_window(but, M); }



panel_item window::text_item(string s)   
{ return BASE_WINDOW::text_item(s); }

void window::set_text(panel_item it, string s)   
{ BASE_WINDOW::set_text(it,s); }

panel_item window::int_item(string s,int& x,const char* hlp) 
{ return BASE_WINDOW::int_item(s,&x,hlp);}

panel_item window::int_item(string s,int& x, int l, int h,const char* hlp) 
{ return BASE_WINDOW::slider_item(s,&x,l,h,0,hlp);}

panel_item window::int_item(string s,int& x, int l, int h, void (*F)(int),const char* hlp) 
{ return BASE_WINDOW::slider_item(s,&x,l,h,F,hlp);}

panel_item window::int_item(string s,int& x, int l, int h, const window_handler& obj,const char* hlp) 
{ panel_item it = BASE_WINDOW::slider_item(s,&x,l,h,0,hlp);
  BASE_WINDOW::set_item_object(it,obj);
  return it;
}


panel_item window::double_item(string s, double& x,const char* hlp) 
{ return BASE_WINDOW::float_item(s,&x,hlp);}

panel_item window::real_item(string s, double& x,const char* hlp)
{ return BASE_WINDOW::float_item(s,&x,hlp); }

panel_item window::string_item(string label, string& x,const char* hlp)
{ x = ~x; //disconnect
  return BASE_WINDOW::string_item(label,&x,0,hlp);
 }

panel_item window::string_item(string label, string& x, void (*F)(char*), 
                                                        const char* hlp)
{ x = ~x; //disconnect
  return BASE_WINDOW::string_item(label,&x,F,hlp);
 }

panel_item window::string_item(string label, string& x, const window_handler& obj, 
                                                        const char* hlp)
{ x = ~x; //disconnect
  panel_item it = BASE_WINDOW::string_item(label,&x,0,hlp);
  BASE_WINDOW::set_item_object_str(it, obj);
  return it;
 } 



panel_item  window::string_item(string label,string& x, const list<string>& L,
                                const char* hlp)
{ return string_item(label,x,L,0,0,hlp); }


panel_item  window::string_item(string label,string& x, const list<string>& L,
                                void (*F)(char*), const char* hlp)
{ return string_item(label,x,L,0,F,hlp); }


panel_item  window::string_item(string label,string& x, const list<string>& L,
                                const window_handler& obj, const char* hlp)
{ panel_item it = string_item(label,x,L,0,0,hlp); 
  BASE_WINDOW::set_item_object_str(it, obj);
  return it;
}



// with sz param

panel_item  window::string_item(string label,string& x, const list<string>& L,
                                int sz, const char* hlp)
{ return string_item(label,x,L,sz,0,hlp); }


panel_item  window::string_item(string label,string& x, const list<string>& L,
                                int sz, void (*F)(char*), const char* hlp)
{ x = x.cstring(); //disconnect
  const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) 
    /*if (s.length() > 0)*/ p[i++] = s;
  panel_item it = BASE_WINDOW::string_menu_item(label,&x,"",i,p,sz,F,hlp); 
  delete[] p;
  return it;
}

panel_item  window::string_item(string label,string& x, const list<string>& L,
                                int sz, const window_handler& obj, const char* hlp)
{ x = x.cstring(); //disconnect
  const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) 
     if (s.length() > 0) p[i++] = s;
  panel_item it = BASE_WINDOW::string_menu_item(label,&x,"",i,p,sz,0,hlp); 
  delete[] p;
  BASE_WINDOW::set_item_object_str(it, obj);
  return it;
}


void window::set_menu(panel_item it, const list<string>& L, int sz)
{ const char** argv = new const char*[L.length()];
  int argc = 0;
  string s;
  forall(s,L) 
     if (s.length() > 0) argv[argc++] = s;
  BASE_WINDOW::set_menu(it,argc,argv,sz); 
  delete[] argv;
}



// choice items

panel_item  window::choice_item(string label,int& x, const list<string>& L,
                                void (*F)(int), const char* hlp)
{ const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) p[i++] = s;
  panel_item it = BASE_WINDOW::choice_item(label,&x,i,p,1,0,F,hlp); 
  delete[] p;
  return it;
}

panel_item  window::choice_item(string label,int& x, const list<string>& L,
                                const window_handler& obj, const char* hlp)
{ const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) p[i++] = s;
  panel_item it = BASE_WINDOW::choice_item(label,&x,i,p,1,0,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  delete[] p;
  return it;
}


panel_item  window::choice_mult_item(string label,int& x, const list<string>& L,
                                                          const char* hlp)
{ return choice_mult_item(label,x,L,0,hlp); }


panel_item  window::choice_mult_item(string label,int& x, string s1, 
                                                          const char* hlp)
{ list<string> tmp;
  tmp.append(s1);
  return choice_mult_item(label,x,tmp,hlp);
}

panel_item  window::choice_mult_item(string label,int& x, string s1, string s2,
                                                          const char* hlp)
{ list<string> tmp;
  tmp.append(s1);
  tmp.append(s2);
  return choice_mult_item(label,x,tmp,hlp);
}


panel_item  window::choice_mult_item(string label,int& x, const list<string>& L,
                                                    void (*F)(int),const char* hlp)
{ const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) p[i++] = s;
  panel_item it = BASE_WINDOW::choice_mult_item(label,&x,i,p,F,hlp); 
  delete[] p;
  return it;
}

panel_item  window::choice_mult_item(string label,int& x, const list<string>& L,
                                                    const window_handler& obj,const char* hlp)
{ const char** p = new const char*[L.length()];
  int    i = 0;
  string s;
  forall(s,L) p[i++] = s;
  panel_item it = BASE_WINDOW::choice_mult_item(label,&x,i,p,0,hlp); 
  delete[] p;
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}


// bitmap choice item

panel_item window::choice_item(string label, int& x, int n, int w, int h, 
                               unsigned char** bm,const char* hlp) 
{ return BASE_WINDOW::bitmap_choice_item(label,&x,n,w,h,bm,0,hlp); }

panel_item window::choice_item(string label, int& x, int n, int w, int h, 
                               unsigned char** bm, void (*F)(int),
                                                   const char* hlp) 
{ return BASE_WINDOW::bitmap_choice_item(label,&x,n,w,h,bm,F,hlp); }

panel_item window::choice_item(string label, int& x, int n, int w, int h, 
                               unsigned char** bm, const window_handler& obj,
                                                   const char* hlp) 
{ panel_item it = BASE_WINDOW::bitmap_choice_item(label,&x,n,w,h,bm,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}



panel_item window::choice_mult_item(string label, int& x, int n, int w, int h, 
                                    unsigned char** bm, const char* hlp) 
{ return BASE_WINDOW::bitmap_choice_mult_item(label,&x,n,w,h,bm,0,hlp); }

panel_item window::choice_mult_item(string label, int& x, int n, int w, int h, 
                                    unsigned char** bm, void (*F)(int),
                                                        const char* hlp) 
{ return BASE_WINDOW::bitmap_choice_mult_item(label,&x,n,w,h,bm,F,hlp); }

panel_item window::choice_mult_item(string label, int& x, int n, int w, int h, 
                                    unsigned char** bm, const window_handler& obj,
                                                        const char* hlp) 
{ panel_item it = BASE_WINDOW::bitmap_choice_mult_item(label,&x,n,w,h,bm,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}


panel_item  window::choice_item(string label,int& x,const char* hlp,int n, ...)
{ const char* p[32];
  va_list arg_list;
  va_start(arg_list,n);
  for(int i=0; i<n; i++)
    p[i] = va_arg(arg_list,char*);
  va_end(arg_list);
  return BASE_WINDOW::choice_item(label,&x,n,p,1,0,0,hlp);
 }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2)
{ return choice_item(label,x,0,2,s1,s2); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3)
{ return choice_item(label,x,0,3,s1,s2,s3); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3, const char* s4)
{ return choice_item(label,x,0,4,s1,s2,s3,s4); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3, const char* s4, const char* s5)
{ return choice_item(label,x,0,5,s1,s2,s3,s4,s5); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6)
{ return choice_item(label,x,0,6,s1,s2,s3,s4,s5,s6); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6, const char* s7)
{ return choice_item(label,x,0,7,s1,s2,s3,s4,s5,s6,s7); }

panel_item  window::choice_item(string label,int& x,const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6, const char* s7, const char* s8)
{ return choice_item(label,x,0,8,s1,s2,s3,s4,s5,s6,s7,s8); }



panel_item window::int_item(string s,int& x,int low, int high, int step,const char* hlp)   
{ return int_item(s,x,low,high,step,0,hlp); }


panel_item window::int_item(string s,int& x,int low, int high, int step,
                                            void (*F)(int), const char* hlp)   
{ int n = (high-low)/step +1;
  char** p = new char*[n];
  for(int i = 0; i < n; i++) 
  { p[i] =  new char[16];
    sprintf(p[i],"%d",low+i*step);
   }
  panel_item it = 
   BASE_WINDOW::choice_item(s,&x,n,(const char**)p,step,low,F,hlp);
  for(int j = 0; j < n; j++)  delete[] p[j];
  delete[] p;
  return it;
 }
 
panel_item window::int_item(string s,int& x,int low, int high, int step,
                                            const window_handler& obj, const char* hlp)   
{ int n = (high-low)/step +1;
  char** p = new char*[n];
  for(int i = 0; i < n; i++) 
  { p[i] =  new char[16];
    sprintf(p[i],"%d",low+i*step);
   }
  panel_item it = 
   BASE_WINDOW::choice_item(s,&x,n,(const char**)p,step,low,0,hlp);
  for(int j = 0; j < n; j++)  delete[] p[j];
  delete[] p;
  BASE_WINDOW::set_item_object(it, obj);
  return it;
 } 
 


panel_item window::bool_item(string s, bool& x,void (*F)(int),const char* hlp)
{ return BASE_WINDOW::bool_item(s,&x,F,hlp); }

panel_item window::bool_item(string s, bool& x,const char* hlp)
{ return BASE_WINDOW::bool_item(s,&x,0,hlp); }

panel_item window::bool_item(string s, bool& x,const window_handler& obj,const char* hlp)
{ panel_item it = BASE_WINDOW::bool_item(s,&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}


panel_item window::color_item(string s, color& x,const char* hlp)
{ return BASE_WINDOW::color_item(s,(int*)&x,0,hlp); }

panel_item window::color_item(string s, color& x,void (*F)(int),const char* hlp)
{ return BASE_WINDOW::color_item(s,(int*)&x,F,hlp); }

panel_item window::color_item(string s, color& x,const window_handler& obj,const char* hlp)
{ panel_item it = BASE_WINDOW::color_item(s,(int*)&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}



panel_item window::pstyle_item(string s, point_style& x, const char* hlp)
{ return BASE_WINDOW::pstyle_item(s,&x,0,hlp); }

panel_item window::pstyle_item(string s, point_style& x, void (*F)(int),
                                                        const char* hlp)
{ return BASE_WINDOW::pstyle_item(s,&x,F,hlp); }

panel_item window::pstyle_item(string s, point_style& x, const window_handler& obj,
                                                        const char* hlp)
{ panel_item it = BASE_WINDOW::pstyle_item(s,&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}




panel_item window::lstyle_item(string s, line_style& x, const char* hlp)
{ return BASE_WINDOW::lstyle_item(s,&x,0,hlp); }

panel_item window::lstyle_item(string s, line_style& x, void (*F)(int),
                                                        const char* hlp)
{ return BASE_WINDOW::lstyle_item(s,&x,F,hlp); }

panel_item window::lstyle_item(string s, line_style& x, const window_handler& obj,
                                                        const char* hlp)
{ panel_item it = BASE_WINDOW::lstyle_item(s,&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}



panel_item window::lwidth_item(string s, int& x, const char* hlp)
{ return BASE_WINDOW::lwidth_item(s,&x,0,hlp); }

panel_item window::lwidth_item(string s, int& x, void (*F)(int),
                                                        const char* hlp)
{ return BASE_WINDOW::lwidth_item(s,&x,F,hlp); }

panel_item window::lwidth_item(string s, int& x, const window_handler& obj,
                                                        const char* hlp)
{ panel_item it = BASE_WINDOW::lwidth_item(s,&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}


/*
panel_item window::lwidth42_item(string s, int& x, const char* hlp)
{ return BASE_WINDOW::lwidth42_item(s,&x,0,hlp); }

panel_item window::lwidth42_item(string s, int& x, void (*F)(int),
                                                        const char* hlp)
{ return BASE_WINDOW::lwidth42_item(s,&x,F,hlp); }

panel_item window::lwidth42_item(string s, int& x, const window_handler& obj,
                                                        const char* hlp)
{ panel_item it = BASE_WINDOW::lwidth42_item(s,&x,0,hlp); 
  BASE_WINDOW::set_item_object(it, obj);
  return it;
}
*/



static void casteljau(double t, int n, double* ax, double* ay,
                                       double& x, double& y)
{ double* bx = new double[n];
  double* by = new double[n];

  for(int j=0; j<n; j++)
  { bx[j] = ax[j];
    by[j] = ay[j];
   }

  for(int k=1; k<n; k++)
  { for(int i=n-1; i>=k; i--)
    { bx[i] = (1-t)*bx[i-1] + t*bx[i];
      by[i] = (1-t)*by[i-1] + t*by[i];
     }
  }

  x = bx[n-1];
  y = by[n-1];

  delete[] bx;
  delete[] by;
}


void window::compute_bezier(const list<point>& L, int m, double* X, double* Y)
{ 
  int n = L.length();

  double* bx = new double[n];
  double* by = new double[n];

  int i = 0;
  point p;
  forall(p,L)
  { bx[i] = p.xcoord();
    by[i] = p.ycoord();
    i++;
   }

  double d = 1.0/(m-1);

  X[0] = bx[0];
  Y[0] = by[0];

  for(int j=1; j<m-1; j++) casteljau(j*d,n,bx,by,X[j],Y[j]);

  X[m-1] = bx[n-1];
  Y[m-1] = by[n-1];

  delete[] bx;
  delete[] by;
}


void window::draw_bezier(const list<point>& L, int m, int arrow, double d, 
                                                      bool border, color c)
{ 
  double* X = new double[m];
  double* Y = new double[m];

  compute_bezier(L,m,X,Y);

  list<point> P;
  for(int i=0; i<m; i++) P.append(point(X[i],Y[i]));
  draw_filled_arrow(P,arrow,d,border,c);

  delete[] X;
  delete[] Y;
}


void window::bezier_segments(const list<point>& L, int m, segment& s1, 
                                                          segment& s2)
{ double* X = new double[m];
  double* Y = new double[m];
  compute_bezier(L,m,X,Y);
  s1 = segment(X[1],Y[1],X[0],Y[0]);
  s2 = segment(X[m-2],Y[m-2],X[m-1],Y[m-1]);
  delete[] X;
  delete[] Y;
}



void window::draw_bezier(const list<point>& L, int m, color c)
{ draw_bezier(L,m,0,0,false,c); }

void window::draw_bezier_arrow(const list<point>& L, int m, color c)
{ double d = pix_to_real(get_line_width());
  draw_bezier(L,m,2,d,false,c); 
 }

  


// splines

#include <LEDA/core/array.h>

void window::draw_spline(const list<point>& L, int f, int arrow, double d, 
                                                      bool border, color c)
{
  // Diese Funktion berechnet aus einem vorgegebenen Polygon, welches
  // verfeinert werden soll, ein Bezier Kontrollpolygon. Bisher wird
  // davon ausgegangen, da3 das eingegebene Polygon einfach ist. Das
  // Bezierpolygon mu3 diese Eigenschaft natuerlich nicht haben.
  // Gro3e Abstandsunterschiede bei aufeinanderfolgenden Polygonpunkten
  // koennen zu entarteten Polygonzuegen fuehren, etwa bei einem
  // fehlerhaften Doppelklick bei Mauseingabe. Dies koennte man durchaus
  // abfangen durch einen vorgegebenen Mindestabstand, oder, noch besser,
  // durch ein vorgegebenes Mindestverhaeltnis zwischen 
  // aufeinanderfolgenden Sementlaengen.

    int size = L.size(); 

    array<point>  pol(size+2);
    array<double> seglength(size+2);
    array<vector> tangent(size+2);


    if (L.head() == L.tail()) 
    { // closed spline
      pol[0]      = L[L.pred(L.last())];
      pol[size+1] = L[L.succ(L.first())];
     }
    else
    { // open spline
      pol[0]      = L.head();
      pol[size+1] = L.tail();
     }

    int i=1;

    point pt;
    forall(pt,L) pol[i++] = pt;


    // Hier wird die Tangente an das Polygon in einem Punkt berechnet als
    // Parallele zu den beiden benachbarten Punkten. Da das Polygon
    // rundum geschlossen ist, geht das auch fuer die Randpunkte.
    // In der Formel 8.12 ist ein Fehler, durch die Laenge des Segments mu3 
    // nicht dividiert werden.

    for (i=1; i <= size; i++) tangent[i] = pol[i+1] - pol[i-1];

    for (i=0; i <= size; i++) seglength[i] = pol[i].distance(pol[i+1]);


    // Nun werden die sogenannten inneren Bezierpunkte berechnet nach den
    // Formeln 8.13, 8.14.

    for (i=1; i<size; i++)
    { 
      double q  = 3*(seglength[i-1] + seglength[i]);
      double h1 = seglength[i-1]/q;
      double h2 = seglength[i]/q;

      list<point> A;
      A.append(pol[i]);
      A.append(pol[i] + h1*tangent[i]);
      A.append(pol[i+1] - h2*tangent[i+1]);
      A.append(pol[i+1]);

      if (i == 1 && (arrow & 2))
        draw_bezier(A,f,2,d,border,c);
      else 
       if (i == size-1 && (arrow & 1))
         draw_bezier(A,f,1,d,border,c);
       else
         draw_bezier(A,f,0,d,border,c);
    } 
}



void window::draw_spline(const list<point>& L, int f, color c)
{ draw_spline(L,f,0,0,false,c); }

void window::draw_spline_arrow(const list<point>& L, int f, color c)
{ double d = pix_to_real(get_line_width());
  draw_spline(L,f,2,d,false,c); 
}

void window::draw_closed_spline(const list<point>& L, int f, color c)
{ list<point> tmp = L;
  tmp.append(tmp.head());
  draw_spline(tmp,f,0,0,false,c); 
}

void window::draw_spline(const polygon& P, int f, color c)
{ list<point> tmp = P.vertices();
  tmp.append(tmp.head());
  draw_spline(tmp,f,0,0,false,c); 
}


// pixmaps

char* window::create_pixrect_from_xpm(string fname)
{
   ifstream in(fname);

   if (!in.good())
   { LEDA_EXCEPTION(0,
     string("window::create_pixrect: Cannot open %s",~fname));
     return 0;
    }

   list<string> L;
   string line;

   char c = ' ';

   while (in)
   { while (in && c != '"') in >> c;
     if (c == '"')
     { line.read(in,'"');
       in >> c;
       in >> c;
       L.append(line);
      }
    }

   int n = L.length();

   char** xpm = new char*[n];

   char** p = xpm;

   string s;
   forall(s,L) *p++ = s.cstring();

   return create_pixrect_from_xpm((const char**)xpm);
}


// edit string

void window::string_edit(point p, string& s)
{ BASE_WINDOW::string_edit(p.xcoord(),p.ycoord(),&s); }


void window::draw_text_with_cursor(double x, double y, string s, int cursor, 
                                                                 color col)
{  draw_text(x,y,s,col);

  if (s == "" && cursor >= 0)
  { double dx =  text_width("|");
    double dy =  text_height("|");
    draw_text(x,y,"| |",col);
    BASE_WINDOW::draw_text_cursor(x+dx,y-dy,col);
    return;
   }

  if (cursor < 0) return;

  double tht = text_height(s.replace_all("\n"," "));

  int len = 0;

  string line;
  forall_lines(line,s)
  { y -= tht; 
    if (cursor >= len && cursor <= len + line.length()) 
    { cursor -= len;
      x += text_width(line(0,cursor-1));
      BASE_WINDOW::draw_text_cursor(x,y,col);
      break;
     }
    len += line.length()+1;
   }

}


int window::string_edit(double x0, double y1, string& s, int& curs_pos)
{ int timeout = 0;
  return string_edit(x0,y1,s,curs_pos,timeout);
 }


int window::string_edit(double x0, double y1, string& s, int& curs_pos,
                                                          int& timeout)
{ 
  int val;
  unsigned long t0 = 0;
  unsigned long t  = 0;
  double x,y;
  int k;

  do { if (timeout > 0)
         { k = read_event(val,x,y,t,timeout);
           if (t0 == 0) 
             t0 = t;
           else
             if (int(t-t0) > timeout) k = no_event;
          }
       else
          k = read_event(val,x,y);
   } while (k != no_event && k != button_press_event && k != key_press_event);

  if (k == no_event)
  { timeout = 0;
    return 1;
   }

  if (k == button_press_event)
  { double w = text_width(s);
    double h = text_height(s);
    double x1 = x0 + w;
    double y0 = y1 - h;
    if (y > y0 && y < y1 && x > x0 && x < x1) 
    { double l = x - x0;
      int j = s.length()-1;
      while (text_width(s(0,j)) > l) j--;
      curs_pos = j+1;
      return 0;
     }
    return -1;
   }

  //if (val == KEY_RETURN) return -1;
  if (val == KEY_ESCAPE) return -1;

  char c    = (char)val;
  int  j    = curs_pos;
            
#if defined(__BORLANDC__)
  if (isprint(c))
#else
  if (!iscntrl(c))
#endif
  { s = s.insert(j,string(c));
    j++;
   }
  else
  { 
    switch (c) {

case KEY_RETURN:    s = s.insert(j++,"\n");
                    break;

     case KEY_DOWN:    s = s.insert(j++,"\n");
                         break;

     case KEY_BACKSPACE: if (j > 0) s = s.del(--j);
                         break;
  
     case KEY_LEFT:      if (j > 0) j--;
                         break;
  
     case KEY_RIGHT:     if (j < s.length()) j++;
                         break;
  
     case KEY_HOME:      j = 0;
                         break;
  
     case KEY_END:       j = s.length();
                         break;
     }
   }

  curs_pos = j;
  return c;
}


/*
void window::set_clip_polygon(const list<point>& lp)
{ int n = lp.length();
  double* X = new double[n];
  double* Y = new double[n];
  n = 0;
  point p;
  forall(p,lp) 
  { X[n] = p.xcoord();
    Y[n] = p.ycoord();
    n++;
   }
  BASE_WINDOW::set_clip_polygon(n,X,Y);
  delete[] X;
  delete[] Y;
}
*/




void window::screenshot(string fname, bool full_color)
{ BASE_WINDOW::screenshot(fname,full_color); }


bool window::contains(const point& p) const
{ return ( p.xcoord() >= xmin() && p.xcoord() <= xmax() &&
           p.ycoord() >= ymin() && p.ycoord() <= ymax() );
 }



void window::adjust_zoom_rect(double& x0, double& y0, double& x1, double& y1) 
{ 
  double wdx = xmax() - xmin();
  double wdy = ymax() - ymin();

  if (x0 > x1) wdy = -wdy;
  if (y0 > y1) wdx = -wdx;

  line L(point(x0,y0),point(x0+wdx,y0+wdy));

  if (orientation(L,point(x1,y1)) < 0)
  { double dy = L.y_proj(x1)-y1; 
    //y1 += dy;
    y1 += dy/2;
    y0 -= dy/2;
   }
  else
  { double dx = L.x_proj(y1)-x1; 
    //x1 += dx;
    x1 += dx/2;
    x0 -= dx/2;
   }
 }


static void adjust_zoom_rect2(window& W, 
                              double x0, double y0, double x, double y, 
                              double& x1, double& y1, double& x2, double& y2)
{ 
  W.adjust_zoom_rect(x0,y0,x,y);
  point p0(x0,y0);
  point p1(x,y);
  point p2 = p1.reflect(p0);
  x1 = p2.xcoord();
  y1 = p2.ycoord();
  x2 = x;
  y2 = y;
  if (x1 > x2) { double tmp = x1; x1 = x2; x2 = tmp; } 
  if (y1 > y2) { double tmp = y1; y1 = y2; y2 = tmp; } 
 }





static void draw_zoom_rect(window& W, char* buf, 
                           double x1, double y1, double x2, double y2, 
                           double x01,double y01,double x02,double y02) 
{ double d = W.pix_to_real(2);
  if (x01 > x1) x01 = x1;
  if (y01 > y1) y01 = y1;
  if (x02 < x2) x02 = x2;
  if (y02 < y2) y02 = y2;
  W.set_pixrect(buf);
  W.draw_point((x1+x2)/2,(y1+y2)/2);
  W.draw_rectangle(x1,y1,x2,y2);
  W.flush_buffer(x01-d,y01-d,x02+d,y02+d);
}



bool window::read_zoom_rect(const point& cent, point& p, point& q)
{
  point_style p_style = set_point_style(cross_point); 

  void (*c_handler)(window*,double,double) = get_show_coord_handler();
  set_show_coord_handler(nil);

  const char* msg2 = "~~~\\bf size:    \\rm move ~~~\
                         \\bf position:\\rm click & drag~~~\
                         \\bf zoom:    \\rm click & release~~~\
                         \\bf cancel:  \\rm click right";

  string st_str = get_status_string();
  set_status_string(msg2);

  //start_buffering();
  //redraw(this);

  char* buf = get_window_pixrect();

  start_buffering();

  bool canceled = false;

  double x0 = cent.xcoord(); 
  double y0 = cent.ycoord(); 

  double x1 = x0;
  double y1 = y0;
  double x2 = x0;
  double y2 = y0;

  double xpos = x0 + pix_to_real(8);
  double ypos = y0 - pix_to_real(8);

  adjust_zoom_rect2(*this,x0,y0,xpos,ypos,x1,y1,x2,y2);
  draw_zoom_rect(*this,buf,x1,y1,x2,y2,x1,y1,x2,y2);

  draw_point(x0,y0);
  flush_buffer();

  for(;;)
  { double x01 = x1;
    double y01 = y1;
    double x02 = x2;
    double y02 = y2;

    int event,but;
    while ( (event = read_event(but,xpos,ypos)) != button_press_event)
    { if (event != motion_event) continue;
      adjust_zoom_rect2(*this,x0,y0,xpos,ypos,x1,y1,x2,y2);
      draw_zoom_rect(*this,buf,x1,y1,x2,y2,x01,y01,x02,y02);
      x01 = x1;
      y01 = y1;
      x02 = x2;
      y02 = y2;
     }
    if (but == MOUSE_BUTTON(3)) 
    { canceled = true;
      break;
     }

    //if (but == MOUSE_BUTTON(1)) 
    { double xpos0 = xpos;
      double ypos0 = ypos;
      double x,y;
      while (read_event(but,x,y) != button_release_event)
      { double dx = x - xpos;
        double dy = y - ypos;
        draw_zoom_rect(*this,buf,x1+dx,y1+dy,x2+dx,y2+dy,x1,y1,x2,y2);
        x0 += dx;
        y0 += dy;
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
        xpos = x;
        ypos = y;
       }
       x -= xpos0;
       y -= ypos0;
       if (real_to_pix(x*x + y*y) < 16) break; // no dragging
     }
  }

  stop_buffering();

  set_point_style(p_style); 
  set_show_coord_handler(c_handler);
  set_status_string(st_str);

  if (canceled) return false;

  p = point(x1,y1);
  q = point(x2,y2);

  return true;
}



bool window::read_zoom_rect(point& p, point& q )
{
  set_cursor(XC_dotbox);

  const char* msg1 = "~ZOOM AREA~~~~~\\bf select center: \\rm click left~~~~~\
                                     \\bf cancel: \\rm click right";

  void (*c_handler)(window*,double,double) = get_show_coord_handler();
  set_show_coord_handler(nil);

  string st_str = get_status_string();
  set_status_string(msg1);

/*
  point cent;
  int but = read_mouse(cent);
*/

  double x,y;
  int but = 0;
  while (read_event(but,x,y) != button_press_event);
  point cent(x,y);

  set_cursor(-1);

  set_status_string(st_str);

  set_show_coord_handler(c_handler);

  if (but != MOUSE_BUTTON(1)) return false;

  return read_zoom_rect(cent,p,q);
}
   

  

void window::zoom_area(double x0, double y0, double x1, double y1, int steps,
                       void (*redraw_func)(window*))
{  

   if (!is_open())
     LEDA_EXCEPTION(1,"zoom: window must be displayed first.");

   double wx0 = xmin();
   double wy0 = ymin();
   double wx1 = xmax();
   double wy1 = ymax();

   adjust_zoom_rect(x0,y0,x1,y1);

   if (x0 > x1) { double tmp = x0; x0 = x1; x1 = tmp; }
   if (y0 > y1) { double tmp = y0; y0 = y1; y1 = tmp; }

   if (status_win)
   { double f = (x1-x0)/(wx1-wx0);
     y0 -= f*pix_to_real(status_win->height());
    }

   double dx0 = (x0 - wx0)/steps;
   double dy0 = (y0 - wy0)/steps;
   double dx1 = (x1 - wx1)/steps;
   double dy1 = (y1 - wy1)/steps;

   double g_dist = get_grid_dist();

   //grid_style g_style = get_grid_style();

   char* bg_pr = get_bg_pixrect();

   if (g_dist == 0 && bg_pr == 0 && real_to_pix(20) >= 8)
   { double gd = -20;
     //double gd = pix_to_real(25);
     set_grid_dist(-gd);
     set_grid_style(line_grid);
    }

   start_buffering();

   while (steps--)
   { wx0 += dx0;
     wy0 += dy0;
     wx1 += dx1;
     wy1 += dy1;
     init0(wx0,wx1,wy0,get_bg_pixrect() ? 1:0);
     if (redraw_func) 
        redraw_func(this); 
     else
        redraw();

     flush_buffer();

   //clear0();
     clear();
    }

   stop_buffering();

   if (g_dist == 0)
   { 
/*
leda_wait(0.75);
*/
     set_grid_dist(0);
    }

    if (redraw_func) 
      redraw_func(this); 
    else
      redraw();
}



void window::zoom(double f, int steps, void (*redraw_func)(window*))
{
  if (f == 0) // default window
  { double x0 = 0;
    double x1 = 100;
    double y0 = 0;
    double ratio = (ymax() - ymin())/(xmax() - xmin());
    double y1 = y0 + ratio * (x1-x0);

    if (status_win) 
    { double f = (x1-x0)/(xmax()-xmin());
      y0 += f*pix_to_real(status_win->height());
     }

    zoom_area(x0,y0,x1,y1,steps,redraw_func);
    return;
   }


  double xmi=xmin();
  double xma=xmax();
  double ymi=ymin();
  double yma=ymax();
  double xd=(xma-xmi)*(1/f-1)/2;
  double yd=(yma-ymi)*(1/f-1)/2;
  zoom_area(xmi-xd,ymi-yd,xma+xd,yma+yd,steps,redraw_func);
}



void window::scroll_window(const point& p, 
                void (*redraw_func)(window*,double,double,double,double)) 
{
  scroll_window(p,redraw_func,0,redraw_func);
}



void window::scroll_window(const point& p, 
              void (*redraw_func_start)(window*,double,double,double,double),
              void (*redraw_func_drag)(window*,double,double,double,double), 
              void (*redraw_func_end)(window*,double,double,double,double)) 
{
  window& W = *this;

  W.grab_mouse();

  unsigned long t = W.button_press_time();

  double xmin = W.xmin();
  double ymin = W.ymin();
  double xmax = W.xmax();
  double ymax = W.ymax();

  double x = p.xcoord();
  double y = p.ycoord();

  int buffer_w = 2*W.width();
  int buffer_h = 2*W.height();
  W.start_buffering(buffer_w, buffer_h);

  double x_off = xmax - x;
  double y_off = y - ymin;


  double x0 = xmin - xmax + x;
  double x1 = xmax - xmin + x;
  double y0 = ymin - ymax + y;
  double y1 = ymax - ymin + y;

/*
y_off += W.pix_to_real(1);
y0 += W.pix_to_real(1);
y1 += W.pix_to_real(1);
*/

  // draw_background

  W.set_offset(x_off,y_off);

  double gd0 = W.get_grid_dist();
  char* bpr = W.get_bg_pixrect();
  grid_style gs = W.get_grid_style();
  double gd = -20;
  //double gd = -W.pix_to_real(25);
  y1 += W.pix_to_real(100);

  if (gd0 == 0 && bpr == 0 && W.real_to_pix(gd) <= -8) 
  { W.set_grid_dist(gd);
    W.set_grid_style(line_grid);
   }

  W.clear(x0,y0,x1,y1,0,0);

  if (redraw_func_start)
    redraw_func_start(&W,x0,y0,x1,y1);
  else
    W.redraw();

  W.set_grid_dist(gd0);
  W.set_grid_style(gs);

  W.set_offset(0,0);

  char* buffer;
  W.stop_buffering(buffer);


  W.start_buffering();

  int dir = 0;

/*
  double d = W.pix_to_real(30);
  if (fabs(ymax-y) < d || fabs(ymin-y) < d) dir = 1;
  if (fabs(xmax-x) < d || fabs(xmin-x) < d) dir = 2;
*/

  switch (dir) {
  case 0:  W.set_cursor(XC_fleur);
           break;
  case 1:  W.set_cursor(XC_sb_h_double_arrow);
           break;
  case 2:  W.set_cursor(XC_sb_v_double_arrow);
           break;
  }


  double dx = 0;
  double dy = 0;

  double px = W.xmin() - x_off;
  double py = W.ymin() - W.pix_to_real(buffer_h/2) + y_off;

  py -= W.pix_to_real(panel_height);
  py += W.pix_to_real(1);
//px -= W.pix_to_real(1);

  W.put_pixrect(px,py,buffer);

  if (redraw_func_drag) redraw_func_drag(&W,dx,dy,x_off,y_off);
  W.flush_buffer();


  void (*c_handler)(window*,double,double) = get_show_coord_handler();
  set_show_coord_handler(nil);

  string st_str = get_status_string();

  int event;
  do { 
/*
       string s(" %6.2f / %6.2f", xmin-dx,ymin-dy);
       W.set_status_string(s);
*/
/*
      (*c_handler)(this,xmin-dx,ymin-dy);
*/
       unsigned long t1;
       int val;
       event = W.read_event(val,x1,y1,t1);

       if (event == motion_event && t1 - t < 50) {
          continue;
       }

       if (x1 < xmin || x1 > xmax || y1 < ymin || y1 > ymax) continue;

       t  = t1;
       dx = x1-x;
       dy = y1-y;

       if (dir == 1) dy = 0;
       if (dir == 2) dx = 0;


       //W.flush_buffer(x_off-dx,y_off+dy);

       double p_xpos = W.xmin() - (x_off-dx);
       double p_ypos = W.ymin() - W.pix_to_real(buffer_h/2) + y_off + dy;

       p_ypos -= W.pix_to_real(panel_height);
       p_ypos += W.pix_to_real(1);
     //p_xpos -= W.pix_to_real(1);

       W.put_pixrect(p_xpos,p_ypos,buffer);

       if (redraw_func_drag) redraw_func_drag(&W,dx,dy,x_off,y_off);
       W.flush_buffer();

  } while (event != button_release_event);


  W.stop_buffering();
  W.delete_buffer();
  W.del_pixrect(buffer);

  W.set_cursor(-1);

  W.init0(xmin-dx,xmax-dx,ymin-dy,0);
  W.set_grid_dist(gd0);

  W.start_buffering();

  if (redraw_func_end)
    redraw_func_end(&W,W.xmin(),W.ymin(),W.xmax(),W.ymax());
  else
    W.redraw();

  W.flush_buffer();
  W.stop_buffering();

  W.set_status_string(st_str);

  W.set_show_coord_handler(c_handler);

  W.ungrab_mouse();
}


int window::read_mouse(double& x0, double& y0, int timeout1, int timeout2, 
                       bool& double_click, bool& drag) 
{
  double x = x0;
  double y = y0;
  int key,event,val;

  unsigned long t0 = button_press_time();
  unsigned long t,t1;

  drag = false;
  double_click = false;

  do { 

     //event = read_event(val,x,y,t1);
       event = read_event(val,x,y,t1,timeout1);

/*
       int dt = int(t1-t0);
       if (dt > timeout1) break;
*/

       if (event == no_event)
       { // timeout: drag
         break;
        }

       //if (dt > timeout1/2)
       if (event == motion_event)
       { 
/*
         double f = pix_to_real(1);
         double dx = (x-x0)/f;
         double dy = (y-y0)/f;
         if (dx >= 2 || dx <= -2 || dy >= 2 || dy <= -2) break;
*/
         double dx = real_to_pix(x-x0);
         double dy = real_to_pix(y-y0);
         if (dx*dx + dy*dy > 10) break;
        }

  } while (event != button_release_event);


  if (event != button_release_event)
   { drag = true;
     x0 = x;
     y0 = y;
    }
  else
    { double_click = ( int(t1-t0) < timeout1 && 
                      read_event(key,x,y,t,timeout2)==button_press_event );
      if (double_click)
      { do { event = read_event(val,x,y,t1);
             if (int(t1-t0) > timeout1) break;
            } while (event != button_release_event);
        drag = (event != button_release_event);
      }
    }

  return val;
}


// help texts

static void close_help_win(int but)
{ window* wp = window::get_call_window();
  wp->close();
#if !defined(__win32__)
  delete wp;
#endif
}

static void help_win_close_handler(window* wp) { close_help_win(0); }

void window::display_help_text(string fname)
{
  fname += ".hlp";

  if (!is_file(fname))
  { char* lroot = getenv("LEDAROOT");
    if (lroot)
       fname = string(lroot) + "/incl/HELP/" + fname;
    if (!is_file(fname))
    { LEDA_EXCEPTION(0,string("Cannot open file %s.",~fname));
      return;
     }
   }

  ifstream in(fname);
  string msg;
  while (in)
  { string str;
    str.read_line(in);
    str = str.replace_all("\\\\","\\n");
    if (str == "") str = "\\6";
    msg += str;
    msg += " ";
   }

  
  double l = real_to_pix(text_width(msg));
  double h = real_to_pix(text_height(msg));

  int w = int(1.5*sqrt(l*h));

  panel* pp = new panel(w,-1,fname);
  pp->set_window_close_handler(help_win_close_handler);
  pp->text_item(msg);

  pp->button("continue");

  if (w > 0.9*width())
    pp->open(window::center,window::center);
  else
    pp->open(*this,window::center,window::center);
}


LEDA_END_NAMESPACE
