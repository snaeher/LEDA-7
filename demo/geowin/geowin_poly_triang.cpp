/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_poly_triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/graphics/bitmaps/button21.h>

using namespace leda;


static void constr_poly(const list<rat_point>& L, list<rat_polygon>& P)
{ 
  P.clear();
  if (L.length() > 1) P.append(rat_polygon(L,rat_polygon::NO_CHECK)); 
}

static void triang_poly1(const list<rat_polygon>& L, list<rat_segment>& S)
{ S.clear();
  rat_polygon P;
  forall(P,L)
  { if (!P.is_simple()) continue;
    GRAPH<rat_point,rat_segment> G;
    list<edge> out,in,bound;
    TRIANGULATE_POLYGON(P,G,out,in,bound);
    edge e;
    forall(e,in) S.append(rat_segment(G[source(e)],G[target(e)]));
  } 
}

static void triang_poly2(const list<rat_polygon>& L, list<rat_segment>& S)
{ S.clear();
  rat_polygon P;
  forall(P,L)
  { if (!P.is_simple()) continue;
    GRAPH<rat_point,rat_segment> G;
    list<edge> out,in,bound;
    TRIANGULATE_POLYGON(P,G,out,in,bound);
    edge e;
    forall(e,out) S.append(rat_segment(G[source(e)],G[target(e)]));
  } 
}

int main()
{
  GeoWin GW("Triangulate Polygons");

  // Inputscene
  list<rat_point> L;
  geo_scene sc_input =  GW.new_scene(L); 
  GW.set_color(sc_input,black);
  GW.set_fill_color(sc_input,yellow);
  GW.set_point_style(sc_input,circle_point);
  
  geowin_update<list<rat_point>,   list<rat_polygon> > construct(constr_poly);
  geowin_update<list<rat_polygon>, list<rat_segment> > triang1(triang_poly1);
  geowin_update<list<rat_polygon>, list<rat_segment> > triang2(triang_poly2);   

  geo_scene sc_poly=GW.new_scene(construct, sc_input, "Polygon");
  GW.set_fill_color(sc_poly,grey2);
  GW.set_color(sc_poly,black); 
  GW.set_line_width(sc_poly,2); 
  GW.set_visible(sc_poly,true);

  geo_scene sc_triang1 = GW.new_scene(triang1,sc_poly,"Exterior triangulation"); 
  GW.set_color(sc_triang1,grey2);
  
  geo_scene sc_triang2 = GW.new_scene(triang2,sc_poly,"Interior triangulation"); 
  GW.set_color(sc_triang2,blue);
  GW.set_all_visible(true);
  
  GW.set_button_width(21); GW.set_button_height(21);  
  GW.set_bitmap(sc_triang1, triangle_21_bits);
  GW.set_bitmap(sc_triang2, triangle_21_bits);  
  

  GW.display();

  GW.message("The demo shows the triangulation of the interior and exterior of a polygon.");

  GW.edit(sc_input);
  
  return 0;  
}
