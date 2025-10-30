/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_triang_gen_poly.c
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

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


#define POLYGON     rat_polygon
#define GEN_POLYGON rat_gen_polygon
#define SEGMENT     rat_segment
#define TRIANGLE    rat_triangle
#define POINT       rat_point
#define VECTOR      rat_vector
#define NB          rational

/*
#define POLYGON     polygon
#define GEN_POLYGON gen_polygon
#define SEGMENT     segment
#define TRIANGLE    triangle
#define POINT       point
#define VECTOR      vector
#define NB          double
*/

void triang_gen_poly_outer(const list<GEN_POLYGON>& LGP, list<SEGMENT>& LS)
{
  LS.clear();
  GEN_POLYGON politer;
  
  forall(politer,LGP) {
     list<edge> inner, outer;
     list<edge> hull, hole;
     GRAPH<POINT,SEGMENT>  G;
     TRIANGULATE_POLYGON(politer, G, inner, outer, hull, hole);
     edge e;
     forall(e, outer) LS.append(SEGMENT(G[G.source(e)], G[G.target(e)]));
  }
}


void triang_gen_poly_inner(const list<GEN_POLYGON>& LGP, list<SEGMENT>& LS)
{
  GRAPH<POINT,SEGMENT> triang;
  list<edge> inner_edges;
  list<edge> outer_edges;

  LS.clear();
  GEN_POLYGON politer;
  
  forall(politer,LGP) {
     inner_edges.clear(); outer_edges.clear(); triang.clear();
     list<edge> hull, hole;
     TRIANGULATE_POLYGON(politer, triang, inner_edges, outer_edges, hull, hole);
     edge e;
     forall(e, inner_edges) LS.append(SEGMENT(triang[triang.source(e)], triang[triang.target(e)]));
     cout << "inner/outer/hole/boundary:" << inner_edges.size() << " " << outer_edges.size() << " " << hole.size() << " " << hull.size() << "\n";
  }
}


void triang_gen_poly_hole(const list<GEN_POLYGON>& LGP, list<SEGMENT>& LS)
{
  LS.clear();
  GEN_POLYGON politer;
  
  forall(politer,LGP) {
     list<edge> inner, outer;
     list<edge> hull, hole;
     GRAPH<POINT,SEGMENT>  G;
     TRIANGULATE_POLYGON(politer, G, inner, outer, hull, hole);
     edge e;
     forall(e, hole) LS.append(SEGMENT(G[G.source(e)], G[G.target(e)]));
  }
}

void triang_gen_poly_boundary(const list<GEN_POLYGON>& LGP, list<SEGMENT>& LS)
{
  LS.clear();
  GEN_POLYGON politer;
  
  forall(politer,LGP) {
     list<edge> inner, outer;
     list<edge> hull, hole;
     GRAPH<POINT,SEGMENT>  G;
     TRIANGULATE_POLYGON(politer, G, inner, outer, hull, hole);
     edge e;
     forall(e, hull) LS.append(SEGMENT(G[G.source(e)], G[G.target(e)]));
  }
}

window& arrow_seg(window& w, const SEGMENT& seg, int i)
{ w.draw_arrow(seg.source().to_float(), seg.target().to_float() ); return w;}

int main()
{
  GeoWin gw("Triangulation of generalized polygons");
  
  list<GEN_POLYGON> LGP;
  
  geo_scene sc_input = gw.new_scene(LGP);
  gw.set_fill_color(sc_input, invisible);
  
  geowin_update<list<GEN_POLYGON>, list<SEGMENT> >  triang_outer(triang_gen_poly_outer);
  geowin_update<list<GEN_POLYGON>, list<SEGMENT> >  triang_inner(triang_gen_poly_inner);
  geowin_update<list<GEN_POLYGON>, list<SEGMENT> >  boundary(triang_gen_poly_boundary);
  geowin_update<list<GEN_POLYGON>, list<SEGMENT> >  hole(triang_gen_poly_hole);
  
  geo_scene result = gw.new_scene(triang_outer, sc_input, "Outer triangulation of generalized polygon");
  gw.set_color(result, grey2);
  gw.set_line_width(result,2);
  
  geo_scene result2 = gw.new_scene(triang_inner, sc_input, "Inner triangulation of generalized polygon");
  gw.set_color(result2, blue);
  gw.set_line_width(result2,2);

  GeoBaseScene<list<SEGMENT> >* result3 = gw.new_scene(boundary, sc_input, "Boundaries");
  //gw.set_draw_object_fcn(result3, arrow_seg);
  gw.set_color(result3, black);
  gw.set_line_width(result3, 2);
  
  GeoBaseScene<list<SEGMENT> >* result4 = gw.new_scene(hole, sc_input, "Holes");
  gw.set_draw_object_fcn(result4, arrow_seg);  
  gw.set_color(result4, pink);
  gw.set_line_width(result4, 2);
  
  gw.set_all_visible(true);  
  
  gw.add_special_help_text("geowin_triang_gen_poly", true);
  
  gw.edit();
  
  return 0;
}


