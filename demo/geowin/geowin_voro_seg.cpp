/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geowin_voro_seg.c
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

typedef list<SEGMENT>           segment_list;
typedef GRAPH<CIRCLE,POINT>     voro_seg_graph;

int SAMPLE_NUMBER = 16;


static void segment_points(list<rat_point>& L, rat_segment s, int n)
{
  L.clear();
  rat_segment_points(L,s,n);
  L.append(s.target());
}


void VORONOI_SEGMENTS(const list<SEGMENT>& LS, voro_seg_graph& VS)
{

  // get point samples ...
  list<POINT> LP;
  list<POINT> all;
  map<POINT,SEGMENT> MS;
  SEGMENT iter;
  POINT  piter;
  
  VS.clear();
  
  forall(iter,LS){
    segment_points(LP,iter,SAMPLE_NUMBER);
    forall(piter,LP){
      all.append(piter);
      MS[piter] = iter;
    }
  }
  
  GRAPH<CIRCLE,POINT> VD;
  VORONOI(all,VD);

/*
  list<edge> remove;
  
  edge e;
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
*/
 
  VS=VD;
}


geowin_graph_update<segment_list, voro_seg_graph>  vseg(VORONOI_SEGMENTS);


static GeoWin* geo_win = 0;
void update_func(int) { geo_win->redraw(); }

int main()
{
  GeoWin GW(800,800,"Approximated voronoi diagram of line segments");
  geo_win = &GW;
  window& W = GW.get_window();
  
  list<SEGMENT> Lseg;
  
  geo_scene segs = GW.new_scene(Lseg);
  
  geo_scene sc1 = GW.new_scene(vseg, segs, "VORONOI"); 
  GW.set_color(sc1,blue);

  GW.set_all_visible(true);

  GW.init_menu();
  W.int_item(" Accuracy:",SAMPLE_NUMBER,1,64,update_func);
  GW.edit(segs);

  return 0;
}



