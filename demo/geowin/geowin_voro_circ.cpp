/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_voro_circ.c
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

LEDA_BEGIN_NAMESPACE
void circle_points(list<rat_point>& L, circle c, int n);
LEDA_END_NAMESPACE


using namespace leda;


#define SEGMENT rat_segment
#define POINT   rat_point
#define CIRCLE  rat_circle
#define RATIONAL


/*
#define SEGMENT segment
#define POINT   point
#define CIRCLE  circle
*/

typedef list<CIRCLE>            circle_list;
typedef GRAPH<CIRCLE,POINT>     voro_circ_graph;

int SAMPLE_NUMBER = 45;

void circle_points(list<rat_point>& L, rat_circle c, int n)
{
  list<rat_point> LP;
  L.clear();
  circle_points(LP,c.to_float(),n);
  rat_point piter;
  forall(piter,LP) L.append(piter);
}

void VORONOI_CIRCLES(const list<CIRCLE>& LS, voro_circ_graph& VS)
{
  // get point samples ...
  list<POINT> LP;
  list<POINT> all;
  map<POINT,CIRCLE> MS;
  CIRCLE iter;
  POINT  piter;
  
  VS.clear();
  
  forall(iter,LS){
    circle_points(LP,iter,SAMPLE_NUMBER);
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

geowin_graph_update<circle_list, voro_circ_graph>  vseg(VORONOI_CIRCLES);


int main()
{
  GeoWin GW("Approximated voronoi diagram of circles");
  
  list<CIRCLE> Lcirc;
  
  geo_scene circ = GW.new_scene(Lcirc);
  
  geo_scene sc1 = GW.new_scene(vseg, circ, "VORONOI"); 
  GW.set_color(sc1,blue);
  GW.set_z_order(sc1, 1);

  GW.set_all_visible(true);

  GW.init_menu();
  GW.get_window().int_item(" Accuracy:",SAMPLE_NUMBER,5,200);
  GW.edit(circ);

  return 0;
}



