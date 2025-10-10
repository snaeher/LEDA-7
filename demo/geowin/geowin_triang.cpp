/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/rat_window.h>
#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/graphics/bitmaps/button21.h>

using namespace leda;


static void conv_hull(const list<rat_point>& L, rat_polygon& P)
{
  list<rat_point> ch = CONVEX_HULL(L);
  if( ch.length() > 2 ) P=rat_polygon(ch);
}

static void triang_points(const list<rat_point>& L, list<rat_segment>& T)
{ T.clear();
  GRAPH<rat_point,int> G;
  TRIANGULATE_POINTS(L,G); 
  edge_array<bool> drawn(G,false);
  edge e;
  forall_edges(e,G)
  { if (drawn[e]) continue;
    T.append(rat_segment(G[source(e)],G[target(e)]));
    edge r = G.reversal(e);
    if (r) drawn[r] = true;
   }
}

static void closestpair(const list<rat_point>& L2, rat_segment& seg)
{ 
  if (L2.size() < 2) return;
  rat_point p1,p2;
  list<rat_point> L=L2;
  CLOSEST_PAIR(L, p1, p2);
  seg=rat_segment(p1,p2);
}

int main()
{
  GeoWin GW("Triangulation Demo");
  
  list<rat_point> L;
  L.append(rat_point(12,13)); L.append(rat_point(30,30));

  geo_scene sc_input = GW.new_scene(L); 
  
  geowin_update<list<rat_point>, list<rat_polygon> > CHULL(conv_hull);
  geowin_update<list<rat_point>, list<rat_segment> > TRIANG(triang_points);
  geowin_update<list<rat_point>, list<rat_segment> > CP(closestpair); 
  
  geo_scene sc0 = GW.new_scene(CHULL, sc_input, "CONVEX HULL");
  GW.set_color(sc0,grey1);
  
  geo_scene sc1 = GW.new_scene(TRIANG, sc_input, "TRIANGULATE");
  GW.set_color(sc1,blue);

  geo_scene sc2 = GW.new_scene(CP, sc_input, "CLOSEST_PAIR");
  GW.set_color(sc2,red);
  GW.set_line_width(sc2,3);
  
  GW.set_all_visible(true);
  
  GW.set_button_width(21); GW.set_button_height(21);
  GW.set_bitmap(sc0, hull_21_bits);
  GW.set_bitmap(sc1, triang_21_bits);
  GW.set_bitmap(sc2, line_21_bits);  
  
  GW.edit(sc_input);
  
  return 0;
}

