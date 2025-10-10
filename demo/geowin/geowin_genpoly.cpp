/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_genpoly.c
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
{ 
  if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout =p1.unite(p2);
}

static void f_intersec( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{
  if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];  
  Lout=p1.intersection(p2);
}

static void f_diff( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{
  if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout=p1.diff(p2);
}

static void f_sym_diff( const list<rat_gen_polygon>& Lin, rat_gen_polygon& Lout)
{
  if( Lin.length() < 2 ) return;
  rat_gen_polygon p1=Lin[Lin[0]];
  rat_gen_polygon p2=Lin[Lin[1]];
  Lout=p1.sym_diff(p2);
}

int main()
{
  GeoWin GW("Boolean operations on generalized polygons");
  
  list<rat_gen_polygon> L;

  geo_scene input = GW.new_scene(L); 
  GW.set_limit((GeoEditScene<list<rat_gen_polygon> >*)input,2);
  GW.set_fill_color(input, invisible);
  
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > Unite(f_unite);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > Inter(f_intersec);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > Diff(f_diff);
  geowin_update<list<rat_gen_polygon>, list<rat_gen_polygon> > SymDiff(f_sym_diff);
       
  geo_scene output1=  GW.new_scene(Unite,input,"Union");
  GW.set_color( output1, blue );
  GW.set_color2( output1, blue ); 
  GW.set_fill_color( output1, green);

  geo_scene output2=  GW.new_scene(Inter,input, "Intersection");
  GW.set_color( output2, blue );
  GW.set_color2( output2, blue ); 
  GW.set_fill_color( output2, blue);

  geo_scene output3=  GW.new_scene(Diff,input, "Difference");
  GW.set_color( output3, violet );
  GW.set_color2( output3, violet ); 
  GW.set_fill_color( output3, violet);

  geo_scene output4=  GW.new_scene(SymDiff,input, "SymDiff");  
  GW.set_color( output4, violet );
  GW.set_color2( output4, violet ); 
  GW.set_fill_color( output4, violet);

  GW.set_all_visible(true);
  GW.edit(input);

  return 0;
}
