/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_halfplane_inter.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/geowin.h>
#include<LEDA/core/array.h>
#include<LEDA/core/map.h>
#include<LEDA/geo/rat_geo_alg.h>
#include<LEDA/geo/float_geo_alg.h>

using namespace leda;

#define POINT    rat_point
#define LINE     rat_line
#define SEGMENT  rat_segment
#define POLYGON  rat_gen_polygon 
#define SIMPLE_POLYGON rat_polygon
#define RAY      rat_ray
#define NUMBER2  rational
#define RATIONAL_KERNEL

/*
#define POINT    point
#define LINE     line
#define SEGMENT  segment
#define POLYGON  gen_polygon 
#define SIMPLE_POLYGON polygon
#define RAY      ray
#define NUMBER2  double
*/

bool compute_dual_point(const LINE& l,POINT& pd)
{ pd = l.dual();
  if (l.point1().xcoord() > l.point2().xcoord()) return false;  else return true;
}

class halfplane_intersection : public geowin_update<list<LINE>, list<SEGMENT> >
{
 void update(const list<LINE>& Lin, list<SEGMENT>& Lseg)
 {
  HALFPLANE_INTERSECTION(Lin,Lseg);
 }
};

window& draw_seg(window& w, const SEGMENT& s, int)
{
 w.draw_arrow(s.source().to_float(), s.target().to_float(), red);
 return w;
}

class chull_and_dual_points : public geowin_update<list<LINE>, list<SEGMENT> >,
                              public geowin_redraw
{
  SIMPLE_POLYGON Plower,Pupper;
  list<POINT> Hcand_lower, Hcand_upper;
  list<SEGMENT> arrow_lower, arrow_upper;
 
  void update(const list<LINE>& Lin, list<SEGMENT>& Lseg)
  {
   POINT Hp;
   LINE lakt;
   Hcand_lower.clear(); Hcand_upper.clear();
   arrow_lower.clear(); arrow_upper.clear();
   forall(lakt,Lin) { // compute the lists of dual points for upper and lower hull ...
     if (! lakt.is_vertical()) {
      bool flag = compute_dual_point(lakt,Hp);
      if (flag) { Hcand_lower.append(Hp); arrow_lower.append(SEGMENT(midpoint(lakt.point1(),lakt.point2()),Hp)); }
      else { Hcand_upper.append(Hp); arrow_upper.append(SEGMENT(midpoint(lakt.point1(),lakt.point2()),Hp)); }
     }
   }

   // compute the convex hull of dual points ...
   Plower = CONVEX_HULL_POLY(Hcand_lower);
   Pupper = CONVEX_HULL_POLY(Hcand_upper);  
  }
  
  void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    color cold = W.set_color(c1);
    W << Plower;
    W << Pupper;
    W.set_color(green2);
    POINT iter;
    forall(iter, Hcand_lower) { W << iter; }
    forall(iter, Hcand_upper) { W << iter; }
    SEGMENT siter;
    forall(siter, arrow_lower) W.draw_arrow(siter.source().to_float(),siter.target().to_float());
    forall(siter, arrow_upper) W.draw_arrow(siter.source().to_float(),siter.target().to_float());
    W.set_color(cold);
  }

public:
  virtual ~chull_and_dual_points() {}
};

int main()
{
 GeoWin gw;
 gw.init(-10,10,10);
 
 list<LINE> L1;
 geo_scene sc1= gw.new_scene(L1);

 halfplane_intersection HPINTER;
 GeoResultScene<list<LINE>, list<SEGMENT> >* res1 = gw.new_scene(HPINTER, sc1, "Halfplane intersection");
 gw.set_color(res1,red);
 gw.set_line_width(res1,4);
 gw.set_visible(res1,true);
 geowin_set_draw_object_fcn(res1, draw_seg);
 
 
 chull_and_dual_points CP;
 geo_scene res2 = gw.new_scene(CP, CP, sc1, "Convex hull of dual points");
 gw.set_color(res2,blue);
 gw.set_line_width(res2,2);
 gw.set_point_style(res2, disc_point);
 gw.set_visible(res2,true); 

 gw.add_special_help_text("geowin_halfplane_inter", true);  

 gw.edit(sc1);
 return 1;
}


