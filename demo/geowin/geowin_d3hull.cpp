/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_d3hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/geo/d3_hull.h>

using namespace leda;


#define D3_POINT    d3_point
#define D2_POINT    point
#define D2_POLYGON  polygon

static void hull_2d( const list<D3_POINT>& Lin, D2_POLYGON& Pol)
{
  D3_POINT p;
  list<D2_POINT> inp;
  list<D2_POINT> Lout;
  forall(p,Lin) inp.append(p.project_xy());
  Lout=CONVEX_HULL(inp);
  if( Lout.length() >= 2 ) Pol = D2_POLYGON(Lout );
}

static void show_d3_points(geo_scene sc, d3_window& W, GRAPH<d3_point,int>& H)
{
 GeoWin* gw = get_geowin(sc);
 list<D3_POINT> L;
 gw->get_objects(sc,L);

 GRAPH<D3_POINT,int> G;
 D3_HULL(L,G); 
 H.join(G);
}

geowin_update<list<D3_POINT>, list<polygon> > HULL2d(hull_2d);

int main()
{
  GeoWin GW( "D3 - Hulldemo");
  
  list<D3_POINT> L;
  geo_scene input =   GW.new_scene(L, show_d3_points); 
  GW.set_fill_color(input, blue );
  GW.set_point_style(input, circle_point); 

  geo_scene output1 =  GW.new_scene(HULL2d, input, "Convex hull");
  GW.set_color(output1,red);
  GW.set_description(output1, "This scene holds the 2d convex hull\nof the projection of the input into the\nxy-plane. To show the 3d hull use\n'Show d3 output' from the window menu.\n"); 
  GW.set_fill_color(output1, invisible);
  GW.set_visible(output1,true );  

  GW.add_special_help_text("geowin_d3hull", true);
  GW.edit(input);
  return 0;
}
