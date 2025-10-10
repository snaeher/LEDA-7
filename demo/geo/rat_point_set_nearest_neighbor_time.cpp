/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  rat_point_set_nearest_neighbor_time.c
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

IO_interface I("Nearest Neighbor Time");

I.write_demo("We illustrate the effect of the floating point filter in the \
nearest neighbor search.");

I.write_demo("We perform one nearest neighbor search from every node.");

int N = I.read_int("n = ",50000);
int n = N;

I.write_table("\n");

list<POINT> L;

I.write_demo("POINTS IN UNIT SQUARE\n");
random_points_in_unit_cube(n, L);


I.write_table(" ", n);

float T0 = used_time(); float UT;

POINT_SET T;

POINT p;

forall(p,L) T.insert(p);

UT = used_time(T0); 
I.write_demo("time for n insertions ",UT);
I.write_table(" & ", UT);

node v;

forall_nodes(v,T) T.nearest_neighbor(v);

UT = used_time(T0); 

I.write_demo("time for n nearest neighbor queries, clever way: ",UT);
I.write_table(" & ", UT);

forall_nodes(v,T) T.nearest_neighborA(v);

UT = used_time(T0); 

I.write_demo("time for n nearest neighbor queries, stupid way: ",UT);
I.write_table(" & ", UT);

return 0;
}



