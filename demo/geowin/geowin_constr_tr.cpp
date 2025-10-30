/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_constr_tr.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/geo/rat_geo_alg.h>

using namespace leda;

class geo_constr_tr : public geowin_update<list<rat_segment>,list<rat_segment> >
{
public:
  void update(const list<rat_segment>& Lin,list<rat_segment>& T)
  {
    T.clear();

    GRAPH<rat_point,rat_segment> G;
    TRIANGULATE_SEGMENTS(Lin,G); 
    
    edge_array<bool> drawn(G,false);
    edge e;
    forall_edges(e,G)
    { if (drawn[e]) continue;
      T.append(rat_segment(G[source(e)],G[target(e)]));
      edge r = G.reversal(e);
      if (r) drawn[r] = true;
    } 
   }
};

class geo_constr_delau_tr : public geowin_update<list<rat_segment>,list<rat_segment> >
{
public:
  void update(const list<rat_segment>& Lin,list<rat_segment>& T)
  {
    T.clear();

    GRAPH<rat_point,rat_segment> G;
    DELAUNAY_TRIANG(Lin,G); 
    
    edge_array<bool> drawn(G,false);
    edge e;
    forall_edges(e,G)
    { if (drawn[e]) continue;
      T.append(rat_segment(G[source(e)],G[target(e)]));
      edge r = G.reversal(e);
      if (r) drawn[r] = true;
    } 
   }
};

int main()
{
  GeoWin GW("Constrained Triangulation Demo");
  
  list<rat_segment> L;

  geo_scene sc_input = GW.new_scene(L); 
  GW.set_color(sc_input,green);
  GW.set_line_width(sc_input,3);
  GW.set_active_line_width(sc_input,3);
  GW.set_visible(sc_input,true);

  geo_constr_tr c_triang;
  
  geo_scene sc1 = GW.new_scene(c_triang, sc_input, "CONSTRAINED TRIANGULATION");
  GW.set_color(sc1,blue);
  GW.set_color2(sc1,blue);
  GW.set_visible(sc1,false);

  geo_constr_delau_tr c_del_triang;

  geo_scene sc2 = GW.new_scene(c_del_triang, sc_input, "CONSTRAINED DELAUNAY TRIANGULATION");
  GW.set_color(sc2,red);
  GW.set_color2(sc2,red);
  GW.set_visible(sc2,true);
 
  GW.edit(sc_input);
  
  return 0;
}

