/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  rat_point_set_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:13 $


#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/geo/rat_point_set.h>
#include <assert.h>
#include <LEDA/core/IO_interface.h>

/*
#include <LEDA/geo/plane_alg.h>
#include <LEDA/system/stream.h>
#include <LEDA/geo/rat_point_set.h>
#include <LEDA/geo/rat_kernel_names.h>
#include "POINT_SET.c"
#include <LEDA/geo/kernel_names_undef.h>
*/

#include <LEDA/geo/rat_kernel_names.h>

using namespace leda;



int main(){ 

IO_interface I("Point Sets and Dynamic Delaunay Triangulations");

I.write_demo("We illustrate the speed of point sets and \
dynamic Delaunay triangulations.");

I.write_demo("We use three different kind of points sets: points in the unit \
square, points in the unit disk, and points (almost) on the unit circle.");
I.write_demo("Please start with n around thousand.");

int N = I.read_int("n = ",1000);
int n = N;

bool first_line = true;
for (int i = 0; i < 3; i++)  // the kind of input 
{  

#ifdef BOOK
for (n = N; n <= 4*N; n *= 2)
{ first_line = (n == N);
#endif

I.write_demo(" " );

float T0 = used_time(); float UT;

I.write_table("\n");

list<POINT> L, LQ;
 switch (i){
 case 0: if (first_line) I.write_table("S");
         I.write_demo("POINTS IN UNIT SQUARE\n");
        random_points_in_unit_cube(n, L);
        random_points_in_unit_cube(n, LQ); break;

 case 1: if (first_line) I.write_table("D");
        I.write_demo("POINTS IN UNIT BALL\n");
         random_points_in_unit_ball(n, L); 
         random_points_in_unit_ball(n, LQ); break;
 
 case 2: if (first_line) I.write_table("C");
         I.write_demo("POINTS ON UNIT CIRCLE\n");
         random_points_on_unit_circle(n,L);
         random_points_on_unit_circle(n,LQ); break;
 }

UT = used_time(T0); 

I.write_table(" & ", n);

I.write_demo("time for generation of n random points = ",UT);

POINT_SET T;

POINT p;

forall(p,L) T.insert(p);

UT = used_time(T0); 

I.write_demo("time for insertions: ",UT);
I.write_table(" & ", UT);

forall(p,LQ) T.lookup(p);

UT = used_time(T0); 

I.write_demo("time for lookup: ",UT);
I.write_table(" & ", UT);

forall(p,LQ) T.nearest_neighbor(p);

UT = used_time(T0); 

I.write_demo("time for nearest neighbor of n points: ",UT);
I.write_table(" & ", UT);

node v;
forall_nodes(v,T) T.nearest_neighbor(v);

UT = used_time(T0); 

I.write_demo("time for nearest neighbor of all nodes: ",UT);
I.write_table(" & ", UT);

forall(p,LQ) T.nearest_neighbors(p,10);

UT = used_time(T0); 

I.write_demo("time for 10 nearest neighbors of n points: ",UT);
I.write_table(" & ", UT);

forall_nodes(v,T) T.nearest_neighbors(v,10);

UT = used_time(T0); 

I.write_demo("time for 10 nearest neighbors of all nodes: ",UT);
I.write_table(" & ", UT);

forall(p,L) T.del(p);

UT = used_time(T0);

I.write_demo("time for deletes: ",UT);
I.write_table(" & ", UT);

I.write_table(" \\\\ \\hline ");
#ifdef BOOK
if (n == 4*N) I.write_table (" \\hline ");
}
#endif
}
return 0;
}



