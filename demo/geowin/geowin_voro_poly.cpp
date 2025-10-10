/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_voro_poly.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geowin.h>
#include <LEDA/graphics/geowin_gen.h>

#include <LEDA/geo/plane_alg.h>
#include <LEDA/core/map.h>

using namespace leda;


#define SEGMENT rat_segment
#define POINT   rat_point
#define CIRCLE  rat_circle
#define POLYGON rat_gen_polygon
//#define POLYGON rat_polygon
#define RATIONAL


/*
#define SEGMENT segment
#define POINT   point
#define CIRCLE  circle
#define POLYGON polygon
*/

typedef list<POLYGON>           polygon_list;
typedef GRAPH<CIRCLE,POINT>     voro_poly_graph;

int SAMPLE_NUMBER = 30;


#if defined(RATIONAL)
void polygon_points(list<rat_point>& L, rat_polygon p, int n)
{
  L.clear();
  rat_polygon_points(L,p,n);
}

void polygon_points(list<rat_point>& L, rat_gen_polygon p, int n)
{
  list<point> LP;
  L.clear();
  list<rat_polygon> Lpol = p.polygons();
  rat_polygon pol_iter;
  forall(pol_iter,Lpol){
   list<point> LPH;
   polygon_points(LPH,pol_iter.to_float(),n);
   LP.conc(LPH);
  }
  point piter;
  forall(piter,LP) L.append(rat_point(piter));
}
#endif

void VORONOI_POLYGONS(const list<POLYGON>& LS, voro_poly_graph& VS)
{
  // get point samples ...
  list<POINT> LP;
  list<POINT> all;
  map<POINT,POLYGON> MS;
  POLYGON iter;
  POINT  piter;
  
  VS.clear();
  
  forall(iter,LS){
    polygon_points(LP,iter,SAMPLE_NUMBER);
    forall(piter,LP){
      all.append(piter);
      MS[piter] = iter;
    }
  }
  
  GRAPH<CIRCLE,POINT> VD;
  VORONOI(all,VD);
  
  edge e;
  list<edge> remove;
  
  forall_edges(e,VD){
    POINT p1 = VD[e];
    POINT p2 = VD[VD.reversal(e)];
    
    //cout << MS[p1] << "     " << MS[p2] << "\n\n";
    
    if (identical(MS[p1],MS[p2])){
       //cout << "remove edge!\n";
       remove.append(e);
    }
    //else cout << "Don't remove edge!\n";
  }
  
  forall(e,remove) VD.del_edge(e);
  
  // remove nodes with outdegree 0
  node v;
  list<node> remove_node;
  
  forall_nodes(v,VD) { if (VD.outdeg(v) == 0) remove_node.append(v); }
  forall(v,remove_node) VD.del_node(v);
 
  VS=VD;
}

geowin_graph_update<polygon_list, voro_poly_graph>  vseg(VORONOI_POLYGONS);


int main()
{
  GeoWin GW("Approximated voronoi diagram of polygons");
  
  list<POLYGON> Lpoly;
  
  geo_scene polys = GW.new_scene(Lpoly);
  
  geo_scene sc1 = GW.new_scene(vseg, polys, "VORONOI"); 
  GW.set_color(sc1,blue);
  GW.set_z_order(sc1, 1);
 
  GW.set_all_visible(true);

  GW.init_menu();
  GW.get_window().int_item(" Accuracy:",SAMPLE_NUMBER,5,200);
  GW.edit(polys);

  return 0;
}



