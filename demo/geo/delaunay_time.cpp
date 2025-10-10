/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  delaunay_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:14 $


#include <LEDA/core/list.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/geo/rat_kernel_names.h>
#include <LEDA/core/IO_interface.h>
#include <assert.h>


using namespace leda;




int main(){

IO_interface I("Delaunay Triangulations");

I.write_demo("We illustrate the speed of Delaunay triangulation algorithms. \
There are three algorithms: the flipping algorithm and (_S) and the \
divide-and-conquer algorithms of Guibas-Stolfi and Dwyer.");

I.write_demo("We use three different kind of points sets: points in the unit \
square, points in the unit disk, and points (almost) on the unit circle.");
I.write_demo("Please start with n around ten-thousand.");

int N = I.read_int("Number of points = ",20000); int R = 16000;

for (int k = 0; k < 3; k++)
{ for (int n = N; n <= 4*N; n = 2*n) 
  { list<POINT> L; GRAPH<POINT,int> G;
    float T = used_time();
    float UT;
    switch (k){
     case 0: I.write_table("\nS"); 
             I.write_demo("POINTS IN SQUARE\n");
             random_points_in_square(n,R,L); break;
     case 1: I.write_table("\nD"); 
             I.write_demo("POINTS IN DISC\n");
             random_points_in_disc(n,R,L);   break;
     case 2: I.write_table("\nC"); 
             I.write_demo("POINTS (ALMOST) ON UNIT CIRCLE\n");
             random_points_on_circle(n,R,L); break;
    }
                     
    I.write_table(" & ",n);
    I.write_demo("number of points = ",n);

    UT = used_time(T);

    I.write_demo("time for generation of n random points = ",UT);

    DELAUNAY_FLIP(L,G);

    I.write_table(" & ",UT = used_time(T));

    I.write_demo("time for flipping algorithm = ",UT);

    assert(Is_Delaunay_Triangulation(G,NEAREST));

    used_time(T); // to set the clock

    DELAUNAY_STOLFI(L,G);

    I.write_table(" & ",UT = used_time(T));

    I.write_demo("time for Guibas and Stolfi = ",UT);

    assert(Is_Delaunay_Triangulation(G,NEAREST));

    used_time(T); // to set the clock

    DELAUNAY_DWYER(L,G);
    I.write_demo("time for Dwyer = ",UT);

    I.write_table(" & ",UT = used_time(T));

    assert(Is_Delaunay_Triangulation(G,NEAREST));

    I.write_table(" & ",UT = used_time(T));

    I.write_demo("time for checking = ",UT);
 
   I.write_table("\\\\ \\hline");
  }
}
}

