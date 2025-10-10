/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dot.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/dot.h>

LEDA_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------
//  class dot_swap
// ---------------------------------------------------------------------------

dot_swap::dot_swap(dot& d1, dot& d2) : D1(d1), D2(d2), V(D1.get_view())
{ point p1 = D1.get_position();
  point p2 = D2.get_position(); 
    
  window& W = V.get_window();
  num = W.real_to_pix(Path1.get_path_length(p1,p2)); 
}
  
dot_swap::dot_swap(dot& d1, dot& d2, const path& P) 
  : D1(d1), D2(d2), V(D1.get_view())
{ point p1 = D1.get_position();
  point p2 = D2.get_position(); 
 
  Path1 = P;
  Path2 = P;
  
  window& W = V.get_window();
  num = W.real_to_pix(Path1.get_path_length(p1,p2));  
}

dot_swap::dot_swap(dot& d1, dot& d2, const path& P1, const path& P2) 
  : D1(d1), D2(d2), V(D1.get_view())
{ point p1 = D1.get_position();
  point p2 = D2.get_position(); 
 
  Path1 = P1;
  Path2 = P2;
  
  double l1 = Path1.get_path_length(p1,p2);  
  double l2 = Path2.get_path_length(p1,p2);  
  
  window& W = V.get_window();     
  num = (l1 > l2) ? W.real_to_pix(l1) : W.real_to_pix(l2);
}

void dot_swap::init(int steps)
{ point p1 = D1.get_position();
  point p2 = D2.get_position(); 
 
  Path1.get_path(p1,p2,steps,L1);
  Path2.get_path(p2,p1,steps,L2);
}

void dot_swap::start()
{ hD1 = D1;
  hD1.set_cell_border_color(invisible);
  hD1.set_cell_color(invisible);
  hD1.set_rank(D1.get_rank()-1);

  D1.set_value_color(invisible);
  D1.set_value_border_color(invisible);  

  hD2 = D2;
  hD2.set_cell_border_color(invisible);
  hD2.set_cell_color(invisible);    
  hD2.set_rank(D2.get_rank()-1);

  D2.set_value_color(invisible);
  D2.set_value_border_color(invisible);  
}
  
void dot_swap::step() 
{ hD1.set_position(L1.pop());
  hD2.set_position(L2.pop());
}
  
void dot_swap::finish()
{ D1.set_value(hD2.get_value());
  D1.set_value_color(hD1.get_value_color());
  D1.set_value_border_color(hD1.get_value_border_color());
  
  D2.set_value(hD1.get_value());
  D2.set_value_color(hD2.get_value_color());
  D2.set_value_border_color(hD2.get_value_border_color());
  
  delete this; 
}

// ---------------------------------------------------------------------------
//  class dot_assign
// ---------------------------------------------------------------------------

dot_assign::dot_assign(dot& d, double x) : D1(d), D2(d), V(d.get_view())
{ val = x;

  double y0 = d.get_position().ycoord();
  double dy = val < 0 ? y0 : V.ymax() - y0;  
  
  window& W = V.get_window();  
  num = W.real_to_pix(dy);
}

dot_assign::dot_assign(dot& d1, dot& d2) : D1(d1), D2(d2), V(D1.get_view())
{ val = D2.get_value();

  point p1 = D1.get_position();
  point p2 = D2.get_position(); 
 
  window& W = V.get_window();
  num = W.real_to_pix(p1.distance(p2)); 
}
  
dot_assign::dot_assign(dot& d1, dot& d2, const path& P) 
  : D1(d1), D2(d2), V(D1.get_view())
{ val = D2.get_value();

  point p1 = D1.get_position();
  point p2 = D2.get_position(); 
 
  Path = P;
 
  window& W = V.get_window();
  num = W.real_to_pix(Path.get_path_length(p1,p2)); 
}

void dot_assign::init(int steps)
{ 
  if (&D1 == &D2) // assign by a constant
  { point p1  = D1.get_position();    
  
    double x0 = p1.xcoord();    
    double y0 = val < 0 ? V.ymin() :  V.ymax();

    double x1 = p1.xcoord();    
    double y1 = p1.ycoord() + val * D1.get_stretch_factor();
    
    Path.get_path(point(x0,y0),point(x1,y1),steps,L);
  }
  else
  { point p1 = D1.get_position();
    point p2 = D2.get_position(); 
    Path.get_path(p2,p1,steps,L);
  }
}

void dot_assign::start() 
{
  hD1 = D2;     
  if (&D1 == &D2) 
  { point p1 = D1.get_position();    
  
    double x = p1.xcoord();
    double y = val < 0 ? V.ymin() :  V.ymax();

    hD1.set_position(point(x,y));
    hD1.set_value(0);
  }
  
  D1.set_value_border_color(invisible);
  D1.set_value_color(invisible);
  
  hD1.set_cell_border_color(invisible);
  hD1.set_cell_color(invisible);    
  hD1.set_rank(D1.get_rank()-1);
}

void dot_assign::step() { hD1.set_position(L.pop()); }

void dot_assign::finish() 
{ D1.set_value(val);
  D1.set_value_border_color(hD1.get_value_border_color());
  D1.set_value_color(hD1.get_value_color());
  delete this; 
}

// ---------------------------------------------------------------------------
//  class dot
// ---------------------------------------------------------------------------

dot::dot() 
{
  pos = point(0,0);
  
  c_height  = 6.0;
  c_width   = 30.0;
  c_col     = grey1;
  c_bcol    = black;

  v_height  = 0.0;
  v_radius  = 2.5;
  v_stretch = 1.0;
  v_col     = ivory;
  v_bcol    = black;

  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;
}

dot::dot(double v, point p) 
{
  pos = p;
  
  c_height  = 6.0;
  c_width   = 30.0;
  c_col     = grey1;
  c_bcol    = black;

  v_height  = v;
  v_radius  = 2.5;
  v_stretch = 1.0;
  v_col     = ivory;
  v_bcol    = black;

  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;
}

dot::dot(double v, double stretch, point p) 
{
  pos = p;
  
  c_height  = 6.0;
  c_width   = 30.0;
  c_col     = grey1;
  c_bcol    = black;

  v_height  = v;
  v_radius  = 2.5;
  v_stretch = stretch;
  v_col     = ivory;
  v_bcol    = black;

  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;
}

dot::dot(const dot& d) : base_element(d) { *this = d; }

dot& dot::operator=(const dot& d)
{ if (base_element::is_attached()) V->del_element(*this);

  base_element::operator =(d);
  
  pos = d.pos;
  
  c_height  = d.c_height;
  c_width   = d.c_width;
  c_col     = d.c_col;
  c_bcol    = d.c_bcol;
  
  v_height  = d.v_height;
  v_radius  = d.v_radius;
  v_stretch = d.v_stretch;
  v_col     = d.v_col;
  v_bcol    = d.v_bcol;

  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;
  
  if (!d.is_attached()) return *this;
  
  view& V = d.get_view();
  double sf = V.get_scale_factor();
  
  c_height *= sf;
  c_width  *= sf;
  v_height *= sf;
  v_radius *= sf;
  
  V.add_element(*this);
  return *this;    
}

dot::~dot()
{ if (!base_element::is_attached()) return;
  V->del_element(*this);
}

void dot::attach(view& v)
{ base_element::attach(v);

  if (v.get_scale_factor() != 1.0)
  { double sf = v.get_scale_factor();
  
    c_height /= sf;
    c_width  /= sf;
    v_height /= sf;
    v_radius /= sf;
  
    set_value_layout();
    set_cell_layout();
    
    c_cur_pos = c_new_pos;
    v_cur_pos = v_new_pos;
  }

  draw();
}

void dot::detach(view& v)
{ clear();
  base_element::detach(v);
}

void dot::set_cell_layout() 
{ double xc = pos.xcoord();
  double yc = pos.ycoord();  
  double dx = c_width / 2;
  double dy = c_height / 2;    
  c_new_pos = rectangle(xc - dx, yc - dy, xc + dx, yc + dy);
}

bool dot::intersect(segment s, list<point>& L) const
{ list<point> L_cell  = c_cur_pos.intersection(s);
  
  double x = v_cur_pos.xmin() + v_cur_pos.width() / 2;
  double y = v_cur_pos.ymin() + v_cur_pos.height() / 2;
  list<point> L_value = circle(x,y,v_radius).intersection(s);
  
  L.clear();  
  if (L_cell.empty() && L_value.empty()) return false;
  
  L = L_cell;
  L.conc(L_value);   
  
  return true;
}

void dot::set_value_layout() 
{ double x0 = pos.xcoord() - v_radius - 1;
  double x1 = pos.xcoord() + v_radius + 1;
  double y0 = pos.ycoord() + (v_height * v_stretch) - v_radius - 1;    
  double y1 = pos.ycoord() + (v_height * v_stretch) + v_radius + 1;
  v_new_pos = rectangle(x0,y0,x1,y1);    
}

void dot::redraw()
{ window& W = V->get_window();

  if (c_height > 0 && c_width > 0)
  { double x0 = c_cur_pos.xmin();
    double x1 = c_cur_pos.xmax();
    double dx = c_cur_pos.width() / 2;
    double y0 = c_cur_pos.ymin();
    double y1 = c_cur_pos.ymax();
    double dy = c_cur_pos.height() / 2;
/*   
    point p0(x0 + dx, y0);
    point p1(x0 + dx, y1);
    W.draw_line(p0,p1, c_col);
*/
    W.draw_segment(x0 + dx, y0, x0 + dx, y1, c_bcol);
    W.draw_segment(x0, y0 + dy, x1, y0 + dy, c_bcol);  
  }
   
  if (v_radius > 0)
  { double x = v_cur_pos.xmin() + v_cur_pos.width() / 2;
    double y = v_cur_pos.ymin() + v_cur_pos.height() / 2;
    W.draw_filled_circle(x,y,v_radius,v_col);    
    W.draw_circle(x,y,v_radius,v_bcol); 
  }
}


void dot::clear(rectangle r0)
{ window& W = V->get_window();
  
  double x0 = r0.xmin();
  double y0 = r0.ymin();
  double x1 = r0.xmax();
  double y1 = r0.ymax();
  
  W.clear(x0,y0,x1,y1);
  
  elem_item it;
  forall(it,V->all_elements())
  { if (it == this) continue;
    rectangle r1 = it->get_bounding_box();    
    double de  = W.pix_to_real(1);
    double X0 = r1.xmin() - de;
    double Y0 = r1.ymin() - de;
    double X1 = r1.xmax() + de;
    double Y1 = r1.ymax() + de;    
    if (x1 < X0 || X1 < x0 || y1 < Y0 || Y1 < y0) continue;    
    it->redraw();
  }
}

void dot::draw()
{ if (!base_element::is_attached() || !V->get_window().is_open()) return;
  
  window& W = V->get_window();
  W.start_buffering();
  
  rectangle r0;
  if (c_new_pos != c_cur_pos && v_new_pos != v_cur_pos)
  { r0 = get_bounding_box();

    clear(r0);
    
    c_cur_pos = c_new_pos;
    v_cur_pos = v_new_pos;
  } 
  else
  { if (c_new_pos != c_cur_pos)
    { clear(c_cur_pos); 
      r0 = c_cur_pos;
      c_cur_pos = c_new_pos;
    }
    if (v_new_pos != v_cur_pos)
    { clear(v_cur_pos);
      r0 = v_cur_pos;
      v_cur_pos = v_new_pos;
    }
  }
  
  redraw();
  
  rectangle r1 = get_bounding_box();
  double x0 = r1.xmin();
  double y0 = r1.ymin();
  double x1 = r1.xmax();
  double y1 = r1.ymax();
  
  list<elem_item>& L = V->all_elements();
  if (base_element::get_item() != L.last_item()) 
  { list_item it = L.succ(base_element::get_item());
            
    for (; it; it = L.succ(it))
    { elem_item x = L[it];

      rectangle r1 = x->get_bounding_box();    
      double de = W.pix_to_real(1);
      double X0 = r1.xmin() - de;
      double Y0 = r1.ymin() - de;
      double X1 = r1.xmax() + de;
      double Y1 = r1.ymax() + de;    
      if (x1 < X0 || X1 < x0 || y1 < Y0 || Y1 < y0) continue;          
      for (; it; it = L.succ(it)) L[it]->redraw();
      break;
    } 
  }
  
  if (x0 > r0.xmin()) x0 = r0.xmin();
  if (y0 > r0.ymin()) y0 = r0.ymin();
  if (x1 < r0.xmax()) x1 = r0.xmax();
  if (y1 < r0.ymax()) y1 = r0.ymax();


  double de = W.pix_to_real(50);
  x0 -= de; x1 += de;
  y0 -= de; y1 += de;

  W.flush_buffer(x0,y0,x1,y1);        
  W.stop_buffering();    
}


void dot::clear()
{ if (!base_element::is_attached() || !V->get_window().is_open()) return;  
  window& W = V->get_window();
  W.start_buffering();  
  
  rectangle r = get_bounding_box();
  double x0 = r.xmin();
  double y0 = r.ymin();
  double x1 = r.xmax();
  double y1 = r.ymax();
  
  clear(r);  
  
  W.flush_buffer(x0,y0,x1,y1);
  W.stop_buffering();  
}
 

rectangle dot::get_bounding_box()
{ double ax0 = c_cur_pos.xmin(), ay0 = c_cur_pos.ymin();
  double ax1 = c_cur_pos.xmax(), ay1 = c_cur_pos.ymax();
  
  double bx0 = v_cur_pos.xmin(), by0 = v_cur_pos.ymin();
  double bx1 = v_cur_pos.xmax(), by1 = v_cur_pos.ymax();
  
  double x0 = (ax0 < bx0) ? ax0 : bx0;
  double y0 = (ay0 < by0) ? ay0 : by0;
  double x1 = (ax1 > bx1) ? ax1 : bx1;
  double y1 = (ay1 > by1) ? ay1 : by1;
  
  return rectangle(x0,y0,x1,y1);
}
  
color dot::set_cell_color(color c) 
{ if (c == c_col) return c;
  leda_swap(c_col,c);
  if (c_col == invisible) clear();
  draw();
  return c;
}

color dot::get_cell_color() const { return c_col; }

color dot::set_cell_border_color(color c)
{ if (c == c_bcol) return c;
  leda_swap(c_bcol,c);
  if (c_bcol == invisible) clear();  
  draw();
  return c; 
}

color dot::get_cell_border_color() const { return c_bcol; }


point dot::set_position(double x, double y)
{ return set_position(point(x,y)); }

point dot::set_position(point p)
{ if (p == pos) return p;  
  leda_swap(pos,p);  
  set_cell_layout();
  set_value_layout();        
  draw();
  return p;
}

point dot::get_position() const { return pos; }


double dot::set_cell_height(double height) 
{ if (c_height == height) return c_height;  
  leda_swap(c_height,height);   
  set_cell_layout();
  draw();  
  return height;
}

double dot::get_cell_height() const { return c_height; }



double dot::set_cell_width(double width) 
{ if (c_width == width) return c_width; 
  leda_swap(c_width,width);   
  set_cell_layout();
  draw();  
  return width;
}

double dot::get_cell_width() const { return c_width; }



color dot::set_value_color(color c) 
{ if (c == v_col) return c;
  leda_swap(v_col,c);
  if (v_col == invisible) clear();  
  draw();
  return c; 
}

color dot::get_value_color() const { return v_col; }

color dot::set_value_border_color(color c)
{ if (c == v_bcol) return c;
  leda_swap(v_bcol,c);
  if (v_bcol == invisible) clear();  
  draw();
  return c; 
}

color dot::get_value_border_color() const { return v_bcol; }



double dot::set_value(double height) 
{ if (v_height == height) return v_height;  
  leda_swap(v_height,height);  
  set_value_layout();
  draw();  
  return height;
}

double dot::get_value() const { return v_height; }


double dot::set_stretch_factor(double stretch) 
{ if (v_stretch == stretch) return stretch;  
  leda_swap(v_stretch,stretch);  
  set_value_layout();
  draw();  
  return stretch;
}

double dot::get_stretch_factor() const { return v_stretch; }



double dot::set_value_radius(double radius) 
{ if (v_radius == radius) return v_radius;
  leda_swap(v_radius,radius);
  set_value_layout();
  draw();  
  return radius;
}

double dot::get_value_radius() const { return v_radius; }


void dot::swap(dot& D) 
{ if (!base_element::is_attached() || !D.is_attached(*V)) return; 
  V->add_animation(new dot_swap(*this,D)); 
}

void dot::swap(dot& D, const path& p) 
{ if (!base_element::is_attached() || !D.is_attached(*V)) return; 
  V->add_animation(new dot_swap(*this,D,p)); 
}

void dot::swap(dot& D, const path& p1, const path& p2) 
{ if (!base_element::is_attached() || !D.is_attached(*V)) return; 
  V->add_animation(new dot_swap(*this,D,p1,p2)); 
}


void dot::move(point p) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new base_move<dot>(*this,p));
}

void dot::move(point p, const path& P) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new base_move<dot>(*this,p,P));
}

void dot::translate(double dx, double dy)
{ if (!base_element::is_attached()) return;  
  point p = point(pos.xcoord() + dx, pos.ycoord() + dy);
  V->add_animation(new base_move<dot>(*this,p));
}

void dot::translate(double dx, double dy, const path& P)
{ if (!base_element::is_attached()) return;  
  point p = point(pos.xcoord() + dx, pos.ycoord() + dy);
  V->add_animation(new base_move<dot>(*this,p,P));
}


void dot::assign(double i) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new dot_assign(*this,i)); 
}

void dot::assign(dot& D) 
{ if (!is_attached() || !D.is_attached(*V) || this == &D) return;
  V->add_animation(new dot_assign(*this,D)); 
}

void dot::assign(dot& D, const path& P) 
{ if (!is_attached() || !D.is_attached(*V) || this == &D) return;
  V->add_animation(new dot_assign(*this,D,P)); 
}

LEDA_END_NAMESPACE
