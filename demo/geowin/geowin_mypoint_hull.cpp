/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_mypoint_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/graphics/ps_file.h>
#include <LEDA/geo/point.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/internal/std/ctype.h>
#include "quick_hull.h"

using namespace leda;

using std::ostream;
using std::istream;

// convex hull template algorithm ...
// (uses quick hull)


LEDA_BEGIN_NAMESPACE

// -------------------------------------------------------------------------------
// rational kernel for quickhull ...
// -------------------------------------------------------------------------------

struct compare_xy_rat_point {
  typedef int        result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;  
  
  int operator()(const rat_point& p1, const rat_point& p2)
  { return rat_point::cmp_xy(p1,p2); }
};

struct compare_yx_rat_point {
  typedef int        result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;  
  
  int operator()(const rat_point& p1, const rat_point& p2)
  { return rat_point::cmp_yx(p1,p2); }
};

struct compare_signed_dist_to_line_rat_point {
  typedef int        result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;  
  typedef rat_point  third_argument_type;
  typedef rat_point  fourth_argument_type;   
  
  int operator()(const rat_point& l1, const rat_point& l2,
                 const rat_point& pa, const rat_point& pb)
  { return leda::cmp_signed_dist(l1,l2,pa,pb); }
};

struct orientation_special_rat_point {
  typedef int        result_type;
  typedef rat_point  first_argument_type;
  
  rat_segment s;
  
  orientation_special_rat_point(const rat_point& p1, const rat_point& p2) : s(p1,p2) { }
  
  inline int operator()(const rat_point& c)
  { return leda::orientation(s, c); }
};

struct right_turn_rat_point {
  typedef bool       result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;
  typedef rat_point  third_argument_type;  
  
  bool operator()(const rat_point& p1, 
                  const rat_point& p2,
		  const rat_point& p3)
  {  
    return leda::right_turn(p1,p2,p3); 
  }
};

struct collinear_rat_point {
  typedef bool       result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;
  typedef rat_point  third_argument_type;  
  
  bool operator()(const rat_point& p1, 
                  const rat_point& p2,
		  const rat_point& p3)
  {  
    return leda::collinear(p1,p2,p3); 
  }
};

struct orientation_rat_point {
  typedef int        result_type;
  typedef rat_point  first_argument_type;
  typedef rat_point  second_argument_type;
  typedef rat_point  third_argument_type;  
  
  int operator()(const rat_point& p1, 
                 const rat_point& p2,
		 const rat_point& p3)
  {  
    return leda::orientation(p1,p2,p3); 
  }
};


class qhull_rat_kernel {
public:
typedef rat_point                               POINT;
typedef rat_segment                             SEGMENT;
typedef compare_xy_rat_point                    COMPARE_XY_POINTS;
typedef compare_yx_rat_point                    COMPARE_YX_POINTS;
typedef compare_signed_dist_to_line_rat_point   COMPARE_SIGNED_DIST_TO_LINE;
typedef orientation_special_rat_point           ORIENTATION_SPECIAL;
typedef orientation_rat_point                   ORIENTATION;
typedef collinear_rat_point                     COLLINEAR;
typedef right_turn_rat_point                    RIGHTTURN;
};

// -------------------------------------------------------------------------------
// LEDA float kernel for quickhull ...
// -------------------------------------------------------------------------------

struct compare_xy_point {
  typedef int        result_type;
  typedef point      first_argument_type;
  typedef point      second_argument_type;  
  
  int operator()(const point& p1, const point& p2)
  {   
     return point::cmp_xy(p1,p2); 
  }
};

struct compare_yx_point {
  typedef int    result_type;
  typedef point  first_argument_type;
  typedef point  second_argument_type;  
  
  int operator()(const point& p1, const point& p2)
  { return point::cmp_yx(p1,p2); }
};

struct compare_signed_dist_to_line_point {
  typedef int        result_type;
  typedef point  first_argument_type;
  typedef point  second_argument_type;  
  typedef point  third_argument_type;
  typedef point  fourth_argument_type;   
  
  int operator()(const point& l1, const point& l2,
                 const point& pa, const point& pb)
  { return leda::cmp_signed_dist(l1,l2,pa,pb); }
};

struct orientation_special_point {
  typedef int        result_type;
  typedef point  first_argument_type;
  
  double ax, ay;
  double prec1;
  double prec2;
  
  orientation_special_point(const point& p1, const point& p2) : 
     ax(p1.xcoord()), ay(p1.ycoord()), prec1(p1.xcoord() - p2.xcoord()), prec2(p1.ycoord() - p2.ycoord()) 
  { }
  
  inline int operator()(const point& c)
  { double d1 = prec1 * (ay - c.ycoord());
    double d2 = prec2 * (ax - c.xcoord()); 
    if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
  }  
};

struct right_turn_point {
  typedef bool   result_type;
  typedef point  first_argument_type;
  typedef point  second_argument_type;
  typedef point  third_argument_type;  
  
  bool operator()(const point& p1, 
                  const point& p2,
		  const point& p3)
  {  
    return leda::right_turn(p1,p2,p3); 
  }
};

struct collinear_point {
  typedef bool   result_type;
  typedef point  first_argument_type;
  typedef point  second_argument_type;
  typedef point  third_argument_type;  
  
  bool operator()(const point& p1, 
                  const point& p2,
		  const point& p3)
  {  
    return leda::collinear(p1,p2,p3); 
  }
};

struct orientation_point {
  typedef int   result_type;
  typedef point  first_argument_type;
  typedef point  second_argument_type;
  typedef point  third_argument_type;  
  
  int operator()(const point& p1, 
                 const point& p2,
		 const point& p3)
  {  
    return leda::orientation(p1,p2,p3); 
  }
};

class qhull_float_kernel  {
public:
typedef point                               POINT;
typedef segment                             SEGMENT;
typedef compare_xy_point                    COMPARE_XY_POINTS;
typedef compare_yx_point                    COMPARE_YX_POINTS;
typedef compare_signed_dist_to_line_point   COMPARE_SIGNED_DIST_TO_LINE;
typedef orientation_special_point           ORIENTATION_SPECIAL;
typedef orientation_point                   ORIENTATION;
typedef collinear_point                     COLLINEAR;
typedef right_turn_point                    RIGHTTURN;

};

// -------------------------------------------------------------------------------

LEDA_END_NAMESPACE


class My_Point{
 double x,y;
public:
 My_Point(){ x=0; y=0; }
 My_Point(double xw, double yw) { x=xw; y=yw; }
 
 double xcoord() const { return x; }
 double ycoord() const { return y; }
 
 void set_xcoord(double val) { x = val; }
 void set_ycoord(double val) { y = val; }  
 
 static int cmp_xy(const My_Point& p1, const My_Point& p2)
 {
  int v = leda::compare(p1.xcoord(),p2.xcoord());
  if (v==0) return leda::compare(p1.ycoord(),p2.ycoord());
  else return v;
 } 
 
 static int cmp_yx(const My_Point& p1, const My_Point& p2)
 {
  int v = leda::compare(p1.ycoord(),p2.ycoord());
  if (v==0) return leda::compare(p1.xcoord(),p2.xcoord());
  else return v;
 }  
};

// -------------------------------------------------------------------------------
// My_Point traits for quickhull ...
// -------------------------------------------------------------------------------


int orientation(const My_Point& p1, const My_Point& p2, const My_Point& p3)
{
  double d1 = (p1.xcoord() - p2.xcoord()) * (p1.ycoord() - p3.ycoord());
  double d2 = (p1.ycoord() - p2.ycoord()) * (p1.xcoord() - p3.xcoord()); 
  if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
}

int cmp_signed_dist(const My_Point& p1, const My_Point& p2, 
                    const My_Point& p3, const My_Point& p4)
{
  double d1 = (p1.xcoord() - p2.xcoord()) * (p4.ycoord() - p3.ycoord());
  double d2 = (p1.ycoord() - p2.ycoord()) * (p4.xcoord() - p3.xcoord()); 
  if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
}


struct compare_xy_my_point {
  typedef int        result_type;
  typedef My_Point      first_argument_type;
  typedef My_Point      second_argument_type;  
  
  int operator()(const My_Point& p1, const My_Point& p2)
  {   
     return My_Point::cmp_xy(p1,p2); 
  }
};

struct compare_yx_my_point {
  typedef int    result_type;
  typedef My_Point  first_argument_type;
  typedef My_Point  second_argument_type;  
  
  int operator()(const My_Point& p1, const My_Point& p2)
  { return My_Point::cmp_yx(p1,p2); }
};

struct compare_signed_dist_to_line_my_point {
  typedef int    result_type;
  typedef My_Point  first_argument_type;
  typedef My_Point  second_argument_type;  
  typedef My_Point  third_argument_type;
  typedef My_Point  fourth_argument_type;   
  
  int operator()(const My_Point& l1, const My_Point& l2,
                 const My_Point& pa, const My_Point& pb)
  { return cmp_signed_dist(l1,l2,pa,pb); }
};

struct orientation_special_my_point {
  typedef int        result_type;
  typedef My_Point  first_argument_type;
  
  double ax, ay;
  double prec1;
  double prec2;
  
  orientation_special_my_point(const My_Point& p1, const My_Point& p2) : 
     ax(p1.xcoord()), ay(p1.ycoord()), 
     prec1(p1.xcoord() - p2.xcoord()), prec2(p1.ycoord() - p2.ycoord()) 
  { }
  
  inline int operator()(const My_Point& c)
  { double d1 = prec1 * (ay - c.ycoord());
    double d2 = prec2 * (ax - c.xcoord()); 
    if (d1 == d2) return 0; else return (d1 > d2) ? +1 : -1;
  }  
};

struct right_turn_my_point {
  typedef bool   result_type;
  typedef My_Point  first_argument_type;
  typedef My_Point  second_argument_type;
  typedef My_Point  third_argument_type;  
  
  bool operator()(const My_Point& p1, 
                  const My_Point& p2,
		  const My_Point& p3)
  {  
    return orientation(p1,p2,p3)==-1; 
  }
};

struct collinear_my_point {
  typedef bool   result_type;
  typedef My_Point  first_argument_type;
  typedef My_Point  second_argument_type;
  typedef My_Point  third_argument_type;  
  
  bool operator()(const My_Point& p1, 
                  const My_Point& p2,
		  const My_Point& p3)
  {  
    return orientation(p1,p2,p3)==0; 
  }
};

struct orientation_my_point {
  typedef int   result_type;
  typedef My_Point  first_argument_type;
  typedef My_Point  second_argument_type;
  typedef My_Point  third_argument_type;  
  
  int operator()(const My_Point& p1, 
                 const My_Point& p2,
		 const My_Point& p3)
  {  
    return orientation(p1,p2,p3); 
  }
};

class My_Segment{
 My_Point st,en;
public:
 My_Segment(){ st = My_Point(0,0); en = My_Point(0,0); }
 My_Segment(double x1, double y1, double x2, double y2) 
 { st=My_Point(x1,y1); en=My_Point(x2,y2); }
 My_Segment(const My_Point& p1, const My_Point& p2)
 { st=p1; en=p2; }
 
 My_Point start() const { return st; }
 My_Point end() const { return en; }
 
 void set_start(const My_Point& p) { st = p; }
 void set_end(const My_Point& p) { en = p; }  
};

class my_kernel  {
public:
typedef My_Point                               POINT;
typedef My_Segment                             SEGMENT;
typedef compare_xy_my_point                    COMPARE_XY_POINTS;
typedef compare_yx_my_point                    COMPARE_YX_POINTS;
typedef compare_signed_dist_to_line_my_point   COMPARE_SIGNED_DIST_TO_LINE;
typedef orientation_special_my_point           ORIENTATION_SPECIAL;
typedef orientation_my_point                   ORIENTATION;
typedef collinear_my_point                     COLLINEAR;
typedef right_turn_my_point                    RIGHTTURN;

};

// -------------------------------------------------------------------------------

namespace leda {

ostream& operator<<(ostream& out, const My_Point& p)
{ out << "(" << p.xcoord() << "," << p.ycoord() << ")";
  return out;
}

ostream& operator<<(ostream& out, const My_Segment& s)
{ out << s.start() << s.end();
  return out;
}

int compare(const My_Point& p1, const My_Point& p2)
{
  int v = leda::compare(p1.xcoord(),p2.xcoord());
  if (v==0) return leda::compare(p1.ycoord(),p2.ycoord());
  else return v;
}

istream& operator>>(istream& in, My_Point& p) 
{
  double x,y; 
  char c;
  do in.get(c); while (in && isspace(c));
  if (!in) return in;
  if (c != '(') in.putback(c);
  in >> x;
  do in.get(c); while (isspace(c));
  if (c != ',') in.putback(c);
  in >> y; 
  do in.get(c); while (c == ' ');
  if (c != ')') in.putback(c);
  p.set_xcoord(x); p.set_ycoord(y);
  return in; 
} 

istream& operator>>(istream& in, My_Segment& s) 
{
 My_Point p1,p2;
 in >> p1; in >> p2;
 s = My_Segment(p1,p2);
 return in;
}

ps_file& operator<<(ps_file& F,const My_Point& o) 
{ 
  F << point(o.xcoord(),o.ycoord());
  return F; 
}  

ps_file& operator<<(ps_file& F,const My_Segment& o) 
{ 
  F.draw_segment(o.start().xcoord(),o.start().ycoord(), o.end().xcoord(),o.end().ycoord());
  return F; 
}  

const char* leda_tname(My_Point* p) {  return "My_Point"; }

const char* leda_tname(My_Segment* s) {  return "My_Segment"; }

leda_window& operator << (leda_window& w, const My_Point& obj)
{ w << point(obj.xcoord(),obj.ycoord()); return w; }

leda_window& operator << (leda_window& w, const My_Segment& o)
{ w.draw_segment(o.start().xcoord(),o.start().ycoord(), o.end().xcoord(),o.end().ycoord()); 
  return w; 
}

leda_window& operator >> (leda_window& w, My_Point& obj)
{
  point p1;
  if( w >> p1 ) {
    obj.set_xcoord(p1.xcoord());
    obj.set_ycoord(p1.ycoord());
  }
  return w;
}

leda_window& operator >> (leda_window& w, My_Segment& obj)
{
  segment s;
  if( w >> s) {
    obj = My_Segment(s.source().xcoord(), s.source().ycoord(), s.target().xcoord(), s.target().ycoord());
  }
  return w;
}


bool geowin_IntersectsBox(const My_Point& obj, 
                          double x1,double y1,double x2,double y2,bool f)
{
 if (obj.xcoord() < x2 && obj.xcoord() > x1 && obj.ycoord() < y2 && obj.ycoord() > y1) 
     return true;
 else 
     return false;
}

void geowin_BoundingBox(const My_Point& obj, 
                        double& x1, double& x2,double& y1, double& y2)
{ x1=obj.xcoord(); y1=obj.ycoord(); x2=obj.xcoord(); y2=obj.ycoord(); }

void geowin_Translate(My_Point& obj, double dx, double dy)
{
 obj.set_xcoord(obj.xcoord() + dx);
 obj.set_ycoord(obj.ycoord() + dy);
}

void geowin_Rotate(My_Point& obj, double dx, double dy,double a)
{ }

void geowin_generate_objects(GeoWin& gw, list<My_Point>& L)
{ }

} // end namespace leda

#include <LEDA/geowin_init.h>

//typedef qhull_rat_kernel   KERNEL;
//typedef qhull_float_kernel KERNEL;
typedef my_kernel          KERNEL;

typedef KERNEL::POINT   point_type;
typedef KERNEL::SEGMENT segment_type;


typedef list<point_type>::const_iterator  ITER;


void qhull(const list<point_type>& L, list<segment_type>& hull)
{ 
  hull.clear();
  list<point_type> result = QUICK_HULL_ITER<KERNEL,ITER>(L.begin(),L.end());
  
  point_type pt,last;
  
  if (result.size() < 2) return;
  last = result.tail();
  forall(pt,result){ hull.append(segment_type(pt,last)); last=pt; }
}

int main()
{
  //we have to use this for My_Point:
  geowin_init_default_type((list<My_Point>*)0,string("MyPointlist"));

  GeoWin gw;
  
  list<point_type> pts;
  
  geo_scene input = gw.new_scene(pts);
  
  geowin_update<list<point_type>, list<segment_type> > hull_update(qhull);
  
  geo_scene result = gw.new_scene(hull_update, input, "convex hull");
  gw.set_color(result, blue);
  
  gw.set_all_visible(true);
  gw.edit(input);

  return 0;
}
