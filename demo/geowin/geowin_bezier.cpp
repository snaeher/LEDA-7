/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_bezier.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/float_geo_alg.h>

using namespace leda;


class geo_bezier : public geowin_update<list<point>,list<polygon> >,
                   public geowin_redraw_container<list<polygon> >
{
  list<point> act;

  public:
  
  virtual ~geo_bezier() { }
  
  void update(const list<point>& L, list<polygon>& LP)
  {
    act = L;
    LP.clear();
    LP.append(CONVEX_HULL_POLY(L));
  }
   
  bool draw(const list<polygon>& L,window& W,color c1,color c2,double x1,double y1,double x2,double y2) 
  {
    // draw the hull polygon and the bezier curve ...
    if (! L.empty() ) W << L.head();
    else return false;
    
    W.draw_bezier(act,100,red);
    return false;
  }
};

geo_bezier CHULL;

int main()
{
 GeoWin gw("Bezier curve");

 list<point> LP;
 
 geo_scene sc0 = gw.new_scene(LP, "Control points");
 gw.set_point_style(sc0, disc_point);
 
 geo_scene res = gw.new_scene(CHULL, CHULL, sc0, "Bezier curve");
 gw.set_line_width(res, 2);
 gw.set_color(res, blue);
 
 gw.set_all_visible(true);
 gw.message("We show the convex hull of the control points and the bezier curve defined by them");
 
 gw.edit(sc0);

 return 0;
}
