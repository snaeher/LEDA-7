/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_triang_poly.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/core/map.h>
#include <LEDA/core/tuple.h>

using namespace leda;

using std::cout;
using std::endl;


#define POLYGON     rat_polygon
#define SEGMENT     rat_segment
#define TRIANGLE    rat_triangle
#define POINT       rat_point
#define VECTOR      rat_vector
#define NB          rational

/*
#define POLYGON     polygon
#define SEGMENT     segment
#define TRIANGLE    triangle
#define POINT       point
#define VECTOR      vector
#define NB          double
*/

void triang_poly_outer(const list<POLYGON>& LP, list<SEGMENT>& LS)
{
  LS.clear();
  POLYGON poly;
  forall(poly,LP) {
     list<edge> inner, outer, boundary;
     GRAPH<POINT,SEGMENT>  G;
     TRIANGULATE_POLYGON(poly, G, inner, outer, boundary);
     edge e;
     forall(e,outer) {
      node v = G.source(e);
      node w = G.target(e);
      LS.append(SEGMENT(G[v],G[w]));
     }
  }
}


void triang_poly_inner(const list<POLYGON>& LP, list<SEGMENT>& LS)
{
  LS.clear();
  POLYGON poly;
  forall(poly,LP) {
     list<edge> inner, outer, boundary;
     GRAPH<POINT,SEGMENT>  G;
     TRIANGULATE_POLYGON(poly, G, inner, outer, boundary);
     edge e;
     forall(e,inner) {
      node v = G.source(e);
      node w = G.target(e);
      LS.append(SEGMENT(G[v],G[w]));
     }
  }
}


void triang_poly_boundary(const list<POLYGON>& LP, list<SEGMENT>& LS)
{
  LS.clear();
  POLYGON poly;
  forall(poly,LP) 
  { list<edge> inner, outer, boundary;
    GRAPH<POINT,SEGMENT>  G;
    TRIANGULATE_POLYGON(poly, G, inner, outer, boundary);
    edge e;
    forall(e,boundary) {
      node v = G.source(e);
      node w = G.target(e);
      LS.append(SEGMENT(G[v],G[w]));
     }
  }
}

window& arrow_seg(window& w, const SEGMENT& seg, int i)
{ point a = seg.source().to_float();
  point b = seg.target().to_float();
  w.draw_arrow(a,b);
  return w;
}

int main()
{
  GeoWin gw("Triangulation of generalized polygons");
  
  list<POLYGON> LP;
  
  geo_scene sc_input = gw.new_scene(LP);
  gw.set_fill_color(sc_input, grey1);
 
  geowin_update<list<POLYGON>, list<SEGMENT> >  triang_outer(triang_poly_outer);
  geowin_update<list<POLYGON>, list<SEGMENT> >  triang_inner(triang_poly_inner);
  geowin_update<list<POLYGON>, list<SEGMENT> >  boundary(triang_poly_boundary);
  
  geo_scene result = gw.new_scene(triang_outer, sc_input, "Outer Triangulation of generalized polygon");
  gw.set_color(result, blue2);
  gw.set_line_width(result,1);
  
  geo_scene result2 = gw.new_scene(triang_inner, sc_input, "Inner Triangulation of generalized polygon");
  gw.set_color(result2, red);
  gw.set_line_width(result2,2);

/*
  GeoBaseScene<list<SEGMENT> >* result3 = gw.new_scene(boundary, sc_input, "Boundaries");
  //gw.set_draw_object_fcn(result3, arrow_seg);
*/

  geo_scene result3 = gw.new_scene(boundary, sc_input, "Boundary Edges");
  gw.set_color(result3, black);
  gw.set_line_width(result3,2);
  
  gw.set_all_visible(true);  
  
  gw.add_special_help_text("geowin_triang_poly", true);

  gw.display();
  
  gw.edit();
  
  return 0;
}


