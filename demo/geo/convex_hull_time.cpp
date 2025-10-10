/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  convex_hull_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:14 $


#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/core/IO_interface.h>
#include <assert.h>

using namespace leda;


int main(){ {  // I want two blocks so that statistics is outside

IO_interface I("Convex Hulls");

I.write_demo("We illustrate the speed of convex hull algorithms. \
There are two algorithms: a sweep algorithm (_S) and the \
randomized incremental algorithm (_RIC). ");

I.write_demo("We use three different kind of points sets: points in the unit \
square, points in the unit disk, and points (almost) on the unit circle. \
For each point set we make two experiments: once with a sorted version and \
once with a randomly permuted version.");
I.write_demo("Please start with n around ten-thousand.");

int N = I.read_int("n = ",20000);
int n = N;

float gt = 0; 
float sr = 0;
float ss = 0;
float ricr = 0;
float rics = 0;


int l = 0;
#ifdef BOOK
int L = 1;  // number of repetitions.
#endif

for (int i = 0; i < 3; i++)  // the kind of input 
{  

#ifdef BOOK
for (int n = N; n <= 4*N; n *= 2)
{ if ( n == N) first_line = true; 
gt = sr = ss = ICR = ICS = ricr = rics = K = 0;
for (l = 0; l < L; l++)
  {
#endif

float T = used_time();

I.write_demo(" " );

float UT = used_time(T); 

list<rat_point> L;
 switch (i){
 case 0: if (l == 0 ) I.write_table("\nS");
         I.write_demo("POINTS IN UNIT SQUARE\n");
        random_points_in_unit_cube(n, L); break;

 case 1: if (l == 0 ) I.write_table("\nD");
        I.write_demo("POINTS IN UNIT BALL\n");
         random_points_in_unit_ball(n, L); break;
 
 case 2: if ( l == 0 ) I.write_table("\nC");
         I.write_demo("POINTS (ALMOST) ON UNIT CIRCLE\n");
         random_points_on_unit_circle(n,L); break;
 }


list<rat_point> LS = L;

LS.sort();
L.permute();

 UT = used_time(T); gt += UT;

I.write_demo("time for generation of n random points = ",UT);

list<rat_point> CH;

CH = CONVEX_HULL_S(L);

int k = CH.length();

//K += k;

I.write_demo("number of vertices of hull = ",k);

UT = used_time(T); sr += UT;

I.write_demo("time for CONVEX_HULL_S: random input",UT);

CH = CONVEX_HULL_S(LS);

UT = used_time(T); ss += UT;

I.write_demo("time for CONVEX_HULL_S: sorted input",UT);

/*
CH = CONVEX_HULL_IC(L);

UT = used_time(T); ICR += UT;

I.write_demo("time for CONVEX_HULL_IC: random input",UT);

if ( n >= 4000 && i == 2 )
 I.write_demo("_IC takes too long on sorted inputs for points on circle.");
else
  { CH = CONVEX_HULL_IC(LS);

    UT = used_time(T); ICS += UT;

    I.write_demo("time for CONVEX_HULL_IC: sorted input",UT);
  }

  */
CH = CONVEX_HULL_RIC(L);

UT = used_time(T); ricr += UT;

I.write_demo("time for CONVEX_HULL_RIC: random input",UT);

CH = CONVEX_HULL_RIC(LS);

UT = used_time(T); rics += UT;

I.write_demo("time for CONVEX_HULL_RIC: sorted input",UT);

#ifdef BOOK
}
I.write_table(" & ",n);
I.write_table(" & ",gt/L);
I.write_table(" & ",K/L);
I.write_table(" & ",sr/L);
I.write_table(" & ",ss/L);
//I.write_table(" & ",ICR/L);
//I.write_table(" & ",ICS/L);
I.write_table(" & ",ricr/L);
I.write_table(" & ",ricrics/L);
I.write_table(" \\\\ \\hline ");
}
#endif
}

}
#ifdef ASCII
print_statistics();
#endif

}

