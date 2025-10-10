/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _rat_window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/rat_window.h>
#include <LEDA/geo/float_kernel.h>

LEDA_BEGIN_NAMESPACE

#if defined(__hpuxcc__)

#define def_input_op(type)\
window& operator>>(window& W, rat_##type &r)\
{ type q; W >> q; r = rat_##type(q); return W; }

def_input_op(point)
def_input_op(segment)
def_input_op(ray)
def_input_op(line)
def_input_op(circle)
def_input_op(polygon)
def_input_op(gen_polygon)
def_input_op(rectangle)
def_input_op(triangle)

#else

template <class T>
inline window& read_object(window& W, T& p)
{ typename T::float_type q;
  W >> q;
  p = T(q);
  return W;
}

window& operator>>(window& W, rat_point& p)
{ return read_object(W,p); }

window& operator>>(window& W, rat_segment& s)
{ return read_object(W,s); }

window& operator>>(window& W, rat_ray& r)
{ return read_object(W,r); }

window& operator>>(window& W, rat_line& l)
{ return read_object(W,l); }

window& operator>>(window& W, rat_circle& c)
{ return read_object(W,c); }

window& operator>>(window& W, rat_polygon& p)
{ return read_object(W,p); }

window& operator>>(window& W, rat_gen_polygon& p)
{ return read_object(W,p); }

window& operator>>(window& W, rat_rectangle& r)
{ return read_object(W,r); }

window& operator>>(window& W, rat_triangle& t)
{ return read_object(W,t); }

#endif

//-----------------------------------------------------------------------------
// window output 
// optional normalization
//-----------------------------------------------------------------------------

inline rat_point normalize(const rat_point& p)
{
 integer g = gcd((p.X(),p.Y()),p.W());
 return rat_point(p.X()/g, p.Y()/g, p.W()/g);
}

inline rat_segment normalize(const rat_segment& s)
{ return rat_segment(normalize(s.source()), normalize(s.target())); }

inline rat_ray normalize(const rat_ray& r)
{ return rat_ray(normalize(r.point1()), normalize(r.point2())); }

inline rat_line normalize(const rat_line& l)
{ return rat_line(normalize(l.point1()), normalize(l.point2())); }

inline rat_circle normalize(const rat_circle& c)
{ return rat_circle(normalize(c.point1()), normalize(c.point2()), normalize(c.point3()) ); }

inline rat_polygon normalize(const rat_polygon& p)
{ list<rat_point> Lp = p.vertices();
  list<rat_point> Lp2;
  rat_point iter;
  forall(iter,Lp) Lp2.append(normalize(iter));
  return rat_polygon(Lp2);  
}

inline rat_gen_polygon normalize(const rat_gen_polygon& p)
{ list<rat_point> Lp = p.vertices();
  list<rat_point> Lp2;
  rat_point iter;
  forall(iter,Lp) Lp2.append(normalize(iter));
  return rat_gen_polygon(Lp2);  
}

inline rat_rectangle normalize(const rat_rectangle& r)
{ return rat_rectangle(normalize(r.lower_left()), normalize(r.upper_right())); }

inline rat_triangle normalize(const rat_triangle& t)
{ return rat_triangle(normalize(t.point1()), normalize(t.point2()), normalize(t.point3()) ); }


// ------------------------------------------------------------------------------------------

template <class T>
inline window& draw_object(window& W, const T& p) 
{ 
  if (W.get_normalize_rational_objects()){
    T p2 = normalize(p);
    W << p2.to_float();
  }
  else W << p.to_float();
  return W;
}


window& operator<<(window& W, const rat_point& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const rat_segment& s)
{ return draw_object(W,s); }

window& operator<<(window& W, const rat_ray& r)
{ return draw_object(W,r); }

window& operator<<(window& W, const rat_line& l)
{ return draw_object(W,l); }

window& operator<<(window& W, const rat_circle& c)
{ return draw_object(W,c); }

window& operator<<(window& W, const rat_polygon& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const rat_gen_polygon& p)
{ return draw_object(W,p); }

window& operator<<(window& W, const rat_rectangle& r)
{ return draw_object(W,r); }

window& operator<<(window& W, const rat_triangle& t)
{ return draw_object(W,t); }

LEDA_END_NAMESPACE
