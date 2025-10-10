/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  stick.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/stick.h>

LEDA_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------
//  stick swap animation
// ---------------------------------------------------------------------------

stick_swap::stick_swap(stick& s1, stick& s2) : S1(s1), S2(s2), V(S1.get_view())
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
  
  window& W = V.get_window();
  num = W.real_to_pix(Path1.get_path_length(p1,p2)); 
}
  
stick_swap::stick_swap(stick& s1, stick& s2, const path& P) : S1(s1), S2(s2), V(S1.get_view())
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
 
  Path1 = P;
  Path2 = P;
  
  window& W = V.get_window();
  num = W.real_to_pix(Path1.get_path_length(p1,p2));  
}

stick_swap::stick_swap(stick& s1, stick& s2, const path& P1, const path& P2) : S1(s1), S2(s2), V(S1.get_view())
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
 
  Path1 = P1;
  Path2 = P2;
  
  double l1 = Path1.get_path_length(p1,p2);  
  double l2 = Path2.get_path_length(p1,p2);  
  
  window& W = V.get_window();     
  num = (l1 > l2) ? W.real_to_pix(l1) : W.real_to_pix(l2);
}

void stick_swap::init(int steps)
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
 
  Path1.get_path(p1,p2,steps,L1);
  Path2.get_path(p2,p1,steps,L2);
}


void stick_swap::start()
{ hS1 = S1;
  hS1.set_cell_border_color(invisible);
  hS1.set_cell_color(invisible);
  hS1.set_rank(S1.get_rank()-1);
    
  S1.set_value(0);

  hS2 = S2;
  hS2.set_cell_border_color(invisible);
  hS2.set_cell_color(invisible);    
  hS2.set_rank(S2.get_rank()-1);
    
  S2.set_value(0);
}

  
void stick_swap::step() 
{ hS1.set_position(L1.pop());
  hS2.set_position(L2.pop());
}

  
void stick_swap::finish()
{ S1.set_value(hS2.get_value());
  S2.set_value(hS1.get_value());
  delete this; 
}


// ---------------------------------------------------------------------------
//  stick assign animation
// ---------------------------------------------------------------------------

stick_assign::stick_assign(stick& s, double x) : S1(s), S2(s), V(s.get_view())
{
  val = x;

  double y0 = s.get_position().ycoord();
  double dy = val < 0 ? y0 : V.ymax() - y0;  
  
  window& W = V.get_window();  
  num = W.real_to_pix(dy);
}

stick_assign::stick_assign(stick& s1, stick& s2) : S1(s1), S2(s2), V(S1.get_view())
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
 
  window& W = V.get_window();
  num = W.real_to_pix(p1.distance(p2)); 
}
  
stick_assign::stick_assign(stick& s1, stick& s2, const path& P) : S1(s1), S2(s2), V(S1.get_view())
{ point p1 = S1.get_position();
  point p2 = S2.get_position(); 
 
  Path = P;
 
  window& W = V.get_window();
  num = W.real_to_pix(Path.get_path_length(p1,p2)); 
}


void stick_assign::init(int steps)
{ if (&S1 == &S2)
  { point p1 = S1.get_position();    
  
    double x = p1.xcoord();
    double y = val < 0 ? V.ymin() : V.ymax();

    Path.get_path(point(x,y),p1,steps,L);
  }
  else
  { point p1 = S1.get_position();
    point p2 = S2.get_position(); 
    Path.get_path(p2,p1,steps,L);
  }
}

void stick_assign::start() 
{
  hS1 = S2;     
  if (&S1 == &S2) 
  { point p1 = S1.get_position();    
  
    double x = p1.xcoord();
    double y = val < 0 ? V.ymin() : V.ymax();

    hS1.set_position(point(x,y));
    hS1.set_value(val);
  }
  
  hS1.set_cell_border_color(invisible);
  hS1.set_cell_color(invisible);    
  hS1.set_rank(S1.get_rank()-1);
  
  col1 = S1.set_value_border_color(grey1);
  col2 = S1.set_value_color(invisible);
}

void stick_assign::step() { hS1.set_position(L.pop()); }

void stick_assign::finish() 
{ S1.set_value(hS1.get_value());
  S1.set_value_border_color(col1);
  S1.set_value_color(col2);
  delete this; 
}


// ---------------------------------------------------------------------------
//  class stick
// ---------------------------------------------------------------------------

stick::stick(point p) 
{ 
  set_default_attr();
  
  position  = p;
  
  set_value_layout();  
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;  
}

stick::stick(double v, point p)
{ 
  set_default_attr();
  
  position  = p;
  v_height  = v;
  
  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;  
}

stick::stick(double v, double stretch, point p)
{ set_default_attr();
  
  position  = p;
  v_height  = v;
  v_stretch = stretch;
  
  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;  
}

stick::stick(const stick& s) : base_element(s) { *this = s; }

stick& stick::operator=(const stick& s)
{ if (base_element::is_attached()) V->del_element(*this);

  base_element::operator=(s);
  
  position  = s.position;
   
  c_height  = s.c_height;
  c_width   = s.c_width;
  c_col     = s.c_col;
  c_bcol    = s.c_bcol;
  
  v_yoff    = s.v_yoff;
  v_height  = s.v_height;
  v_stretch = s.v_stretch;
  v_width   = s.v_width;
  v_col     = s.v_col;
  v_bcol    = s.v_bcol;
  
  set_value_layout();
  set_cell_layout();
  
  c_cur_pos = c_new_pos;
  v_cur_pos = v_new_pos;
  
  if (!s.is_attached()) return *this;
  
  // attach function !
  view& V = s.get_view();
  double sf = V.get_scale_factor();
  
  c_height *= sf;
  c_width  *= sf;
  v_yoff   *= sf;
  v_height *= sf;
  v_width  *= sf;
  
  V.add_element(*this);
  
  return *this; 
}

stick::~stick() 
{ if (!base_element::is_attached()) return;
  V->del_element(*this);
}


void stick::attach(view& v)
{ base_element::attach(v);
  
  if (v.get_scale_factor() != 1.0) 
  { double sf = v.get_scale_factor();
    c_height /= sf;
    c_width  /= sf;
    v_yoff   /= sf;
    v_height /= sf;
    v_width  /= sf;
    
    set_value_layout();
    set_cell_layout();    
    
    c_cur_pos = c_new_pos;
    v_cur_pos = v_new_pos; 
  }
  
  draw();
}


void stick::detach(view& v)
{ clear();
  base_element::detach(v);
}

void stick::set_default_attr()
{ c_height  = 4;
  c_width   = 30;
  c_col     = grey1;
  c_bcol    = black;

  v_yoff    = 3;
  v_height  = 0;
  v_stretch = 1.0;
  v_width   = 22;
  v_col     = ivory;
  v_bcol    = black;
}

void stick::set_cell_layout()
{ double xc = position.xcoord();
  double yc = position.ycoord();  
  double dx = c_width / 2;
  double dy = c_height / 2;    
  c_new_pos = rectangle(xc - dx, yc - dy, xc + dx, yc + dy);   
}

void stick::set_value_layout()
{ double x0 = position.xcoord() - v_width / 2;
  double x1 = x0 + v_width;          
  double dy = c_height / 2 + v_yoff;
  double ym = position.ycoord();
  double y0 = (v_height > 0) ?  ym + dy : ym - dy - fabs(v_height * v_stretch);    
  double y1 = y0 + fabs(v_height * v_stretch);
  v_new_pos = rectangle(x0,y0,x1,y1);    
}

void stick::clear(rectangle r0)
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


bool stick::intersect(segment s, list<point>& L) const
{ L = c_cur_pos.intersection(s);
  list<point> ltmp = v_cur_pos.intersection(s);
  L.conc(ltmp);  
  return L.empty();   
}


void stick::redraw() 
{ window& W = V->get_window();
  
  if (c_height > 0 && c_width > 0)
  { W.draw_filled_rectangle(c_cur_pos,c_col);
    W.draw_rectangle(c_cur_pos,c_bcol);
  }
          
  if (v_height != 0 && v_width > 0)
  { W.draw_filled_rectangle(v_cur_pos,v_col);
    W.draw_rectangle(v_cur_pos,v_bcol); 
  }
}


void stick::draw()
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
  
  if (c_height > 0 && c_width > 0)
  { W.draw_filled_rectangle(c_cur_pos,c_col);
    W.draw_rectangle(c_cur_pos,c_bcol);
  }
            
  if (v_height != 0 && v_width > 0)
  { W.draw_filled_rectangle(v_cur_pos,v_col);
    W.draw_rectangle(v_cur_pos,v_bcol); 
  }

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


void stick::clear()
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
 

rectangle stick::get_bounding_box()
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
  


point stick::set_position(double x, double y)
{ return set_position(point(x,y)); }

point stick::set_position(point p)
{ if (p == position) return p;  
  leda_swap(position,p);  
  set_cell_layout();
  set_value_layout();        
  draw();
  return p;
}

point stick::get_position() const { return position; }



  
color stick::set_cell_color(color c) 
{ if (c == c_col) return c;
  leda_swap(c_col,c);
  if (c_col == invisible) clear();
  draw();
  return c;
}

color stick::get_cell_color() const { return c_col; }


color stick::set_cell_border_color(color c)
{ if (c == c_bcol) return c;
  leda_swap(c_bcol,c);
  if (c_bcol == invisible) clear();  
  draw();
  return c; 
}

color stick::get_cell_border_color() const { return c_bcol; }



double stick::set_cell_height(double height) 
{ if (c_height == height) return c_height;  
  leda_swap(c_height,height);   
  set_cell_layout();
  set_value_layout(); 
  draw();  
  return height;
}

double stick::get_cell_height() const { return c_height; }



double stick::set_cell_width(double width) 
{ if (c_width == width) return c_width; 
  leda_swap(c_width,width);   
  set_cell_layout();
  draw();  
  return width;
}

double stick::get_cell_width() const { return c_width; }



color stick::set_value_color(color c) 
{ if (c == v_col) return c;
  leda_swap(v_col,c);
  if (v_col == invisible) clear();  
  draw();
  return c; 
}

color stick::get_value_color() const { return v_col; }



color stick::set_value_border_color(color c)
{ if (c == v_bcol) return c;
  leda_swap(v_bcol,c);
  if (v_bcol == invisible) clear();  
  draw();
  return c; 
}

color stick::get_value_border_color() const { return v_bcol; }



double stick::set_value(double height) 
{ if (v_height == height) return v_height;  
  leda_swap(v_height,height);  
  set_value_layout();
  draw();  
  return height;
}

double stick::get_value() const { return v_height; }


double stick::set_stretch_factor(double stretch) 
{ if (v_stretch == stretch) return stretch;  
  leda_swap(v_stretch,stretch);  
  set_value_layout();
  draw();  
  return stretch;
}

double stick::get_stretch_factor() const { return v_stretch; }


double stick::set_value_width(double width) 
{ if (v_width == width) return v_width;  
  leda_swap(v_width,width);  
  set_value_layout();
  draw();  
  return width;
}

double stick::get_value_width() const { return v_width; }


double stick::set_value_yoffset(double offset)
{ if (v_yoff == offset) return v_yoff;  
  leda_swap(v_yoff,offset);  
  set_value_layout();
  draw();  
  return offset;
}

double stick::get_value_yoffset() const { return v_yoff; }



void stick::swap(stick& S) 
{ if (!base_element::is_attached() || !S.is_attached(*V)) return; 
  V->add_animation(new stick_swap(*this,S)); 
}

void stick::swap(stick& S, const path& p) 
{ if (!base_element::is_attached() || !S.is_attached(*V)) return; 
  V->add_animation(new stick_swap(*this,S,p)); 
}

void stick::swap(stick& S, const path& p1, const path& p2) 
{ if (!base_element::is_attached() || !S.is_attached(*V)) return; 
  V->add_animation(new stick_swap(*this,S,p1,p2)); 
}


void stick::move(point p) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new base_move<stick>(*this,p));
}

void stick::move(point p, const path& P) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new base_move<stick>(*this,p,P));
}

void stick::translate(double dx, double dy)
{ if (!base_element::is_attached()) return;  
  point p = point(position.xcoord() + dx, position.ycoord() + dy);
  V->add_animation(new base_move<stick>(*this,p));
}

void stick::translate(double dx, double dy, const path& P)
{ if (!base_element::is_attached()) return;  
  point p = point(position.xcoord() + dx, position.ycoord() + dy);
  V->add_animation(new base_move<stick>(*this,p,P));
}


void stick::assign(double i) 
{ if (!base_element::is_attached()) return;
  V->add_animation(new stick_assign(*this,i)); 
}

void stick::assign(stick& S) 
{ if (!is_attached() || !S.is_attached(*V) || this == &S) return;
  V->add_animation(new stick_assign(*this,S)); 
}

void stick::assign(stick& S, const path& P) 
{ if (!is_attached() || !S.is_attached(*V) || this == &S) return;
  V->add_animation(new stick_assign(*this,S,P)); 
}

LEDA_END_NAMESPACE
