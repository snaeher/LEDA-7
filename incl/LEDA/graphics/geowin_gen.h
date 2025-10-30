/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  geo_gen.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graphics/geowin_init.h>

GEOWIN_BEGIN_NAMESPACE

__exportGeoW 
void random_points(list<point>& L, int N, double xmin, double ymin,
		   double xmax, double ymax, int grid);

__exportGeoW 
void disc_points(list<point>& L, int N, circle C);


__exportGeoW 
void lattice_points(list<point>& L, int N, double xmin,double ymin,
		   double xmax, double ymax);

__exportGeoW 
void segment_points( list<point>& L, segment s, int n);

__exportGeoW 
void circle_points(list<rat_point>& L, circle C, int n);

__exportGeoW 
void circle_segments(list<segment>& LS, circle C, int n);

__exportGeoW 
void rectangle_points(list<point>& L, rectangle R, int n);

__exportGeoW 
void polygon_points(list<point>& L, polygon P, int n);

__exportGeoW 
void hilbert(list<point>& L, int n, double xmin, double ymin,
	     double xmax, double ymax, int grid);

#if !defined(NO_RAT_ALGORITHMS)

__exportGeoW 
void rat_segment_points( list<rat_point>& L, rat_segment s, int n);

__exportGeoW 
void rat_polygon_points(list<rat_point>& L, rat_polygon P, int n);

/*
__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_point>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_segment>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_ray>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_line>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_circle>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_polygon>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_gen_polygon>& Pl);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_triangle>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<rat_rectangle>& L);

__exportGeoW 
void geowin_generate_objects(GeoWin& gw, list<d3_rat_point>& L);
*/

#endif

GEOWIN_END_NAMESPACE
