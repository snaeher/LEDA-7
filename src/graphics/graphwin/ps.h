#ifndef _PS_H_
#define _PS_H_

#include <LEDA/system/stream.h>
#include <LEDA/graphics/color.h>
#include <LEDA/core/list.h>
#include <LEDA/geo/point.h>


#define PSPixelPerCM	28.35


LEDA_BEGIN_NAMESPACE

class PostScript {

  string_ostream ostr;
  
  double x_min;
  double y_min;
  double x_max;
  double y_max;
  double scale;
  color      col;
  line_style lstyle;
  double     lwidth;

  double font_size;

  string lstyle_name[4];
  
  void header(ostream& out);
  void prolog(ostream& out);
  void footer(ostream& out);

  PostScript& operator<<(const char* s);
  PostScript& operator<<(double d);
  PostScript& operator<<(int i);  
  PostScript& operator<<(color c);

  point draw_arrow_head(const point& q, double a, color c);
  
 public:

  PostScript();
 ~PostScript() {}

  void set_origin(double x, double y);
  void set_scale(double f);
  void set_color(color c);
  void set_line_width(double w);
  void set_line_style(line_style s);
  void set_font(const char* name, double size);

  double get_xmin();
  double get_xmax();
  double get_ymin();
  double get_ymax();
  
  void set_bounding_box(double x0, double y0, double x1, double y1);
  
  void draw_text(double x, double y, string s, color c);
  void draw_ctext(double x, double y, string s, color c);

  void draw_rectangle(double x0, double y0, double x1, double y1, color c);
  void draw_filled_rectangle(double x0,double y0,double x1,double y1,color c);
  void draw_hatched_rectangle(double x0,double y0,double x1,double y1,int h, color c);

  void draw_roundrect(double x0, double y0, double x1, double y1, double rnd,
                                                                  color c);
  void draw_filled_roundrect(double x0, double y0, double x1, double y1, 
                                                              double rnd,
                                                              color c);

  void draw_ellipse(double x, double y, double r1, double r2, color c);
  void draw_filled_ellipse(double x, double y, double r1, double r2, color c);
  void draw_hatched_ellipse(double x, double y, double r1, double r2, int h, color c);

  void draw_circle(const point& C, double r, color c);
  void draw_filled_circle(const point& C, double r, color c);

  void draw_polygon(const list<point>& P, color c);
  void draw_filled_polygon(const list<point>& P, color c);
  void draw_hatched_polygon(const list<point>& P, int h, color c);

  void draw_polygon_edge(const list<point>& P, int arrow, color c);
  void draw_bezier_edge(const list<point>& P, int arrow, color c);
  void draw_spline_edge(const list<point>& P, int arrow, color c);
  void draw_circle_edge(point p, point q, point r, int arrow, color c);

  void draw_filled_arrow(const point& a, const point& b, int arrow, color c);
  void draw_filled_arrow(const list<point>& L, int arrow, color c);

  void draw_line(double x0, double y0,double x1, double y1);
  void draw_grid(double xmin, double ymin, double xmax, double ymax, double d);

  void clip_reset();
  void clip_ellipse(double x, double y, double r1, double r2);
  void clip_rectangle(double x0, double y0, double x1, double y1);
  void clip_polygon(const list<point>& P);
  void clip_polygon(double x0, double y0, double x1, double y1);


  void put_pixrect(char* pmap, double x, double y, double w1, double h1);
  void include_xpm(string fname, double x, double y, double w1, double h1);

  bool print(ostream& ostr);
  bool print(string fname);

};

LEDA_END_NAMESPACE

#endif
