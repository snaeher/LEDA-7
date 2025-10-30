/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_circles.c
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


geowin_update<list<rat_point>,list<rat_polygon> > CHULL(CONVEX_HULL_POLY);
geowin_update<list<rat_point>,list<rat_circle> >  AEC(ALL_EMPTY_CIRCLES);
geowin_update<list<rat_point>,list<rat_circle> >  SMEC(SMALLEST_ENCLOSING_CIRCLE);
geowin_update<list<rat_point>,list<rat_circle> >  LEC(LARGEST_EMPTY_CIRCLE); 


int main()
{
  GeoWin GW("Largest empty and smallest enclosing circle demo");
  GW.message("See the descriptions for the scenes in the menu Scenes->description");
  
  list<rat_point> L;
  geo_scene sc_input =  GW.new_scene(L); 
  GW.set_description(sc_input, "The input point set. \n");
  GW.set_color(sc_input,black);
  
  geo_scene sc_hull=GW.new_scene(CHULL, sc_input, "Convex Hull");
  GW.set_fill_color(sc_hull,invisible);
  GW.set_line_width(sc_hull,2);
  GW.set_color(sc_hull,black); 
  GW.set_description(sc_hull, "This scene holds the convex hull of the input points.\n");
  GW.set_visible(sc_hull,true);

  geo_scene sc_sec = GW.new_scene(SMEC,  sc_input, "Smallest enclosing circle");
  GW.set_color(sc_sec,blue );
  GW.set_fill_color(sc_sec, invisible);
  GW.set_description(sc_sec, "This scene holds the smallest circle\ncontaining all points of the input.\n");
  GW.set_visible(sc_sec,true);

  geo_scene sc_lec = GW.new_scene(LEC,  sc_input, "Largest empty circle");
  GW.set_color(sc_lec,red);
  GW.set_fill_color(sc_lec, invisible);
  GW.set_description(sc_lec, "This scene holds the largest circle\nwhose center lies inside the convex\nhull of the input that contains\nno point of the input in its interior.\n");
  GW.set_visible(sc_lec,true);

  geo_scene sc_aec = GW.new_scene(AEC,  sc_input, "All empty circles"); 
  GW.set_color(sc_aec,grey1);
  GW.set_description(sc_aec, "This scene contains all empty circles\npassing through three or more points\nof the input.\n");
  GW.set_fill_color(sc_aec, invisible);
 
  //buttons
  GW.set_button_width(21); GW.set_button_height(21);   
  GW.set_bitmap(sc_hull, hull_21_bits);
  GW.set_bitmap(sc_sec, encl_circle_21_bits);
  GW.set_bitmap(sc_lec, empty_circle_21_bits);
  
  GW.edit(sc_input);
  //GW.edit();
  
  return 0;  
}
