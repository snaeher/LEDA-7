#include <LEDA/system/stream.h>
#include <LEDA/graphics/color.h>
#include <LEDA/core/list.h>
#include <LEDA/geo/point.h>
#include <LEDA/graphics/graphwin.h>


#ifndef _SVG_H_
#define _SVG_H_


LEDA_BEGIN_NAMESPACE

class ScalableVectorGraphics {
  
 protected: 

  string_ostream ostr;

  // GraphWin& gw; 

  string title;
  
  double x_min;
  double y_min;
  double x_max;
  double y_max;
  double width;
  double height;
  double scale;

  double edge_transparency; 

  // variables for shapes
  color        shape_color; 
  color        shape_border_color; 
  double       shape_border_width; 
  // variables for lines
  color        line_color; 
  int          line_style; 
  double       line_thickness; 
  // varialbes for labels
  color        label_color;
  color        label_bg_color; 
  gw_position  label_pos; 
  bool         orient; 

  gw_font_type node_label_font; 
  double       node_font_size; 
  bool         node_italic; 

  gw_font_type edge_label_font; 
  double       edge_font_size; 
  bool         edge_italic; 


  bool comma; 


  double pix_to_real(int arg)
    { return arg / scale; }

  void header(ostream& out);
  void prolog(ostream& out);
  void footer(ostream& out);


  point draw_arrow_head(const point& q, double a);

  ScalableVectorGraphics& operator<<(const char* s);
  ScalableVectorGraphics& operator<<(double d);
  ScalableVectorGraphics& operator<<(int i);  
  ScalableVectorGraphics& operator<<(color c);
  ScalableVectorGraphics& operator<<(ostream& (*f)(ostream&));
  ScalableVectorGraphics& operator<<(point p);
  ScalableVectorGraphics& operator<<(const list<point>& P); 

  ScalableVectorGraphics& operator<<(ScalableVectorGraphics& (*f)(ScalableVectorGraphics& a)); 

  void  writeSpaces(int soace); 
  void  insertBoth(int space); 
  void  insertFill(int space); 
  void  insertBorder(int spaces); 
  void  insertShapeAtr(int space, int mode); 
  void  insertLineAtr(int space); 
  void  insertLineAtr(); 
  color labelColor();

  point init_node_label( point pos , 
			 double r1 , double r2 , 
			 gw_position lable_pos , 
			 gw_node_shape shape , 
			 string label , int index , 
			 list<string>& L,
			 double& yshift, 
			 bool& start, bool& middle, bool& end); 
  void init_edge_label( string label , int index , list<string>& l); 
  point calculate_pos_node(point p, double r1, double r2,
			   gw_position lable_pos, 
			   gw_node_shape shape); 
  void draw_text_node(point pos, double r1, double r2, 
		      list<string> labels, 
		      double yshift, 
		      bool start, bool middle, bool end);
  void draw_text_node(point pos, 
		      string label, 
		      bool start, bool middle, bool end);
  void draw_text_edge(point , point , 
		      list<string> labels, 
		      double yshift);  
  void draw_text_edge(point , point , 
		      string label); 
  





 public:

  ScalableVectorGraphics() {}
  virtual ~ScalableVectorGraphics() {}
  
  virtual void userProlog(ostream& o) {}
  

  void set_bounding_box( double, double, double, double, double );

  void set_title(string s) { title = s; }

  void set_node_label_font(gw_font_type, int);
  void set_edge_label_font(gw_font_type, int);

  void set_transparency(double arg)
    { edge_transparency = arg; }
  

  void set_shape_attr(color f, color b, double bw)
    {
      shape_color        = f;
      shape_border_color = b; 
      shape_border_width = bw; 
    }
  void set_line_attr(double thickness, int style, color c)
    {
      line_thickness = thickness; 
      line_style     = style; 
      line_color     = c; 
    }
  void set_label_attr(color lc, color bc)
    {
      label_color    = lc; 
      label_bg_color = bc; 
    }


  void draw_node(gw_node_shape , 
		 double , double , 
		 double , double , 
		 double , 
		 gw_position , gw_label_type ,
		 string, 
		 int); 
  void draw_edge(const list<point>& P, 
		 int arrow, 
		 gw_edge_shape sh, 
                 bool draw_border,
		 gw_position lpos, 
		 string, int); 
 
  
  // void draw_text(double x, double y, string s);
  void draw_ctext(point p, string s);
  
  // methods for shapes 
  void draw_rectangle(double x0, double y0, 
		      double x1, double y1, 
		      double rd=0); 
  void draw_filled_rectangle(double x0, double y0, 
			     double x1, double y1, 
			     double rd=0); 
  void draw_bounded_rectangle(double x0, double y0, 
			      double x1, double y1, 
			      double rd=0); 
  void draw_ellipse(double x, double y, 
		    double r1, double r2); 
  void draw_filled_ellipse(double x, double y, 
			   double r1, double r2);
  void draw_bounded_ellipse(double x, double y, 
			    double r1, double r2); 
  void draw_bounded_polygon(const list<point>& P); 
  void draw_polygon(const list<point>& P); 
  void draw_filled_polygon(const list<point>& P); 

  // methods for lines
  void draw_polygon_edge(const list<point>& P, int arrow); 
  void draw_filled_arrow(const list<point>& P, int arrow); 
  void draw_circle_edge(point p, point q, point r, int arrow); 
  void draw_bezier_edge(const list<point>& P, int arrow); 
  void draw_spline_edge(const list<point>& P, int arrow); 

  void draw_circle(point center, double r); 
  void draw_filled_circle(point center, double r); 

  void background_circles(point center , 
			  const list<double>& r, 
			  color border, double width);
  /* 
     void draw_line(double x0, double y0,double x1, double y1);
     void draw_grid(double xmin, double ymin, 
     double xmax, double ymax, double d);
     
     void clip_reset();
     void clip_ellipse(double x, double y, double r1, double r2);
     void clip_rectangle(double x0, double y0, double x1, double y1);
     void clip_polygon(const list<point>& P);
     void clip_polygon(double x0, double y0, double x1, double y1);
     
     
     void put_pixrect(char* pmap, double x, double y, double w1, double h1);
     void include_xpm(string fname, double x, double y, double w1, double h1);
  */ 
  
  bool print(ostream& ostr);
  bool print(string fname);

  friend ScalableVectorGraphics& commaInPoint(ScalableVectorGraphics& a);

  friend ScalableVectorGraphics& noCommaInPoint(ScalableVectorGraphics& a);

};

inline ScalableVectorGraphics& commaInPoint(ScalableVectorGraphics& a)
    { a.comma = true; return a; }

inline ScalableVectorGraphics& noCommaInPoint(ScalableVectorGraphics& a)
    { a.comma = false; return a; }


LEDA_END_NAMESPACE



#endif
