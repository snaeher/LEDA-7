/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_poly_choice.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/plane_alg.h>

using namespace leda;


static void f_unite( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{ if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout =p1.unite(p2);
}

static void f_inter( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{ if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout=p1.intersection(p2);
}

static void f_diff( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{ if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout=p1.diff(p2);
}

static void f_sdiff( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{ if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout=p1.sym_diff(p2);
}

int main()
{
  GeoWin gw("Polygon Demo");
  //gw.message("We show the result of boolean operations on two polygons.");

  geo_scene  sc[4];
  list<rat_gen_polygon> L;
  geo_scene input = gw.new_scene(L); 
  gw.set_fill_color(input, invisible);
  gw.set_limit((GeoEditScene<list<rat_gen_polygon> >*)input, 2);
  
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > unite(f_unite);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > inter(f_inter);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > diff(f_diff);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > sdiff(f_sdiff);
       
  sc[0] = gw.new_scene(unite, input, "Union");
  gw.set_color(sc[0], blue);
  gw.set_fill_color(sc[0], blue);

  sc[1] = gw.new_scene(inter, input, "Intersection");
  gw.set_color(sc[1], red);
  gw.set_fill_color(sc[1], red);

  sc[2] = gw.new_scene(diff, input, "Difference");
  gw.set_color(sc[2], green );
  gw.set_fill_color(sc[2], green);

  sc[3] = gw.new_scene(sdiff, input, "Symmetric Difference");  
  gw.set_color(sc[3], yellow );
  gw.set_fill_color(sc[3], yellow);
  
  gw.set_all_visible(true);

  list<geo_scene> vis_scenes;
  vis_scenes.append(sc[0]); vis_scenes.append(sc[1]);
  vis_scenes.append(sc[2]); vis_scenes.append(sc[3]);
  
  list<string> choices;
  choices.append("union"); choices.append("inter");
  choices.append("diff"); choices.append("sdiff");

  gw.add_scene_buttons(vis_scenes, choices);
  gw.edit(input);

  return 0;
}
