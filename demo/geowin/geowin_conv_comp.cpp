/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_conv_comp.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/geo/float_geo_alg.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/map.h>

using namespace leda;


/*
typedef rat_point     POINT;
typedef rat_segment   SEGMENT;
typedef rat_polygon   POLYGON;
*/
typedef point     POINT;
typedef segment   SEGMENT;
typedef polygon   POLYGON;

void constr_poly(const list<POINT>& L, list<POLYGON>& P)
{ 
  P.clear();
  if (L.length() > 1) P.append(POLYGON(L,POLYGON::NO_CHECK)); 
}

void convex_components(const list<POLYGON>& L, list<SEGMENT>& S)
{ S.clear();
  POLYGON P;
  forall(P,L)
  { if (!P.is_simple()) continue;
  
    GRAPH<POINT,SEGMENT> G;
    list<edge> in,bound;
    edge e;
    CONVEX_COMPONENTS(P,G,in,bound);
    
    forall(e,in) S.append(SEGMENT(G[source(e)],G[target(e)]));
  } 
}

int main()
{
  GeoWin GW("Convex components of a polygon");
 
  // Inputscene
  list<POINT> L;
  geo_scene sc_input =  GW.new_scene(L); 
  GW.set_color(sc_input,black);
  GW.set_fill_color(sc_input,yellow);
  GW.set_point_style(sc_input,circle_point);
  
  geowin_update<list<POINT>,   list<POLYGON> > construct(constr_poly);
  geowin_update<list<POLYGON>, list<SEGMENT> > convcomp(convex_components);

  geo_scene sc_poly= GW.new_scene(construct, sc_input, "Polygon");
  GW.set_fill_color(sc_poly,grey2);
  GW.set_color(sc_poly,black); 
  GW.set_line_width(sc_poly,2); 
  GW.set_visible(sc_poly,true);

  geo_scene sc_cc = GW.new_scene(convcomp,sc_poly,"Convex components"); 
  GW.set_color(sc_cc,blue);
  GW.set_line_width(sc_cc,1);
  GW.set_all_visible(true);
  
  GW.edit(sc_input);
  
  return 0;  
}
