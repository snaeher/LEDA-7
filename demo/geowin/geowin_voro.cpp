/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_voro.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/geowin.h>
#include<LEDA/geo/rat_geo_alg.h>
#include<LEDA/geo/float_geo_alg.h>
#include <LEDA/graphics/bitmaps/button21.h>

using namespace leda;



typedef list<rat_point>             point_list;
typedef list<rat_polygon>           polygon_list;
typedef list<rat_circle>            circle_list;

typedef GRAPH<rat_circle, rat_point> voronoi_graph;
typedef GRAPH<rat_point,int>       delaunay_graph;

geowin_update<point_list, polygon_list>         hull(CONVEX_HULL_POLY);
geowin_graph_update<point_list, voronoi_graph>  voronoi(VORONOI);
geowin_graph_update<point_list, voronoi_graph>  f_voro(F_VORONOI);
geowin_graph_update<point_list, delaunay_graph> delaunay(DELAUNAY_TRIANG);
geowin_graph_update<point_list, delaunay_graph> f_delau(F_DELAUNAY_TRIANG);
geowin_graph_update<point_list, delaunay_graph> ms_tree(MIN_SPANNING_TREE);
geowin_graph_update<point_list, delaunay_graph> crust(CRUST);

class voro_circles : public geowin_update<point_list, circle_list>, public geowin_redraw
{
  circle_list LC;

  void draw(window& W,color c1,color c2,double x1,double y1,double x2,double y2)
  {
    color cold = W.set_color(c1);
    rat_circle iter;
    forall(iter,LC) {
     W.set_color(c1);
     W << iter;
/*
     // draw arrows to center ...
     rat_point p = iter.point1(), c = iter.center();
     W.set_color(red);
     W.draw_arrow(p.xcoord().to_double(), p.ycoord().to_double(), \
                  c.xcoord().to_double(), c.ycoord().to_double());
*/
    }
    W.set_color(cold);
  }

  void update(const point_list& LP, circle_list&)
  {
    LC.clear();
    voronoi_graph VG;
    VORONOI(LP,VG);
    node v;
    forall_nodes(v,VG) if (! (VG[v].is_degenerate())) LC.append(VG[v]);
  }

public:
  virtual ~voro_circles() {}
};


int main()
{
  //GeoWin gw("Voronoi Demo");
  int dpi = window::screen_dpi();

  GeoWin gw(8*dpi,9*dpi,"Voronoi Demo");

  list<rat_point> L;
  GeoEditScene<list<rat_point> >* sc_input =  gw.new_scene(L); 
  
  gw.set_point_style(sc_input,circle_point);
  gw.set_color(sc_input,black);
  gw.set_z_order(sc_input,0);
  gw.set_description(sc_input,"This scene holds the input points for the computation \
  of the various geometric data structures shown in this demo.");

  geo_scene sc1 = gw.new_scene(voronoi, sc_input, "VORONOI"); 
  gw.set_color(sc1,blue);
  gw.set_visible(sc1,true);
  gw.set_z_order(sc1,1);
  gw.set_description(sc1, "This scene holds the Voronoi diagram of the input points. \
  The Voronoi diagram assigns to every input point p a part of the plane containing \
  the points of the plane with closer distance to p than to any other input point.");
  
  geo_scene sc2 = gw.new_scene(f_voro,  sc_input, "F_VORONOI");
  gw.set_color(sc2,green);
  gw.set_z_order(sc2,2);
  gw.set_description(sc2, "This scene holds the furthest point Voronoi diagram. This \
  diagram assigns to every input point p that is a vertex of the convex hull a part \
  of the plane containing all points with greater distance to p than to every other \
  vertex of the convex hull.");

  geo_scene sc3 = gw.new_scene(ms_tree, sc_input, "MINSPAN_TREE");
  gw.set_color(sc3,black);
  gw.set_line_width(sc3,2);
  gw.set_z_order(sc3,3);
  gw.set_description(sc3, "The Euclidean Minimum Spanning Tree (EMST) of the input \
  points is the graph with minimum edge length connecting all input points. Notice that \
  this graph is a subgraph of the Delaunay triangulation.");

  geo_scene sc4 = gw.new_scene(delaunay,sc_input, "DELAUNAY");
  gw.set_color(sc4,orange);
  gw.set_z_order(sc4,4);
  gw.set_description(sc4, "The Delaunay triangulation of the set of input points S is \
  a triangulation T of S with the special property, that all triangles t=(a,b,c) of \
  T have no points of the input inside of their circumcircles. Notice that the Delaunay \
  triangulation is the dual graph of the Voronoi diagram.");

  geo_scene sc5 = gw.new_scene(f_delau, sc_input, "F_DELAUNAY");
  gw.set_color(sc5,red);
  gw.set_z_order(sc5,5);
  gw.set_description(sc5, "The furthest point Delaunay triangulation is the dual graph of \
  the furthest point Voronoi diagram.");

  geo_scene sc6 = gw.new_scene(hull,    sc_input, "CONVEX HULL");
  gw.set_color(sc6,grey2);
  gw.set_fill_color(sc6,grey1);
  gw.set_z_order(sc6,6);
  gw.set_description(sc6, "The convex hull of the input points is the smallest convex set \
  containing all input points.");

  geo_scene sc7 = gw.new_scene(crust,   sc_input, "Crust"); 
  gw.set_color(sc7,pink);
  gw.set_z_order(sc7,7);
  gw.set_description(sc7, "The crust of the input points is a data structure used for \
  reconstruction algorithms."); 
  
  voro_circles vc;
  geo_scene sc8 = gw.new_scene(vc, vc, sc_input, "Circles in voronoi diagramm");
  gw.set_color(sc8,green2);
  gw.set_fill_color(sc8,invisible);
  gw.set_line_width(sc8,2);
  gw.set_z_order(sc8,8);
  gw.set_description(sc8, "This scene shows the circumcircles of the triangles in the Delaunay \
  triangulation and the centers of these circles. Notice that these centers are the Voronoi nodes.");   

  gw.set_button_width(21); 
  gw.set_button_height(21);
  gw.set_bitmap(sc1, voro_21_bits);
  gw.set_bitmap(sc2, f_voro_21_bits);
  gw.set_bitmap(sc3, tree_21_bits);
  gw.set_bitmap(sc4, triang_21_bits);
  gw.set_bitmap(sc5, f_triang_21_bits);
  gw.set_bitmap(sc6, hull_21_bits); 

//gw.add_special_help_text("geowin_voro", true);
  gw.display(window::center,window::center);
  gw.edit(sc_input);
  
  return 0;
}

