/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  IC_versus_RIC_time.c
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

IO_interface I("Convex Hulls: IC versus RIC");

I.write_demo("We illustrate the difference between incremental construction \
and randomized incremental construction on the example \
of convex hull algorithms. \
We time CONVEX_HULL_IC and CONVEX_HULL_RIC.");

I.write_demo("We use three different kind of points sets: points in the unit \
square, points in the unit disk, and points (almost) on the unit circle. \
For each point set we make two experiments: once with a sorted version and \
once with a randomly permuted version.");
I.write_demo("Please start with n around thousand.");

int N = I.read_int("n = ",4000);
int n = N;

float GT=0, SR=0, ICR=0, ICS=0, RICR=0, RICS=0, K=0;

int l = 0;
#ifdef BOOK
int L = 1;  // number of repetitions.
#endif

for (int i = 0; i < 3; i++)  // the kind of input 
{  

#ifdef BOOK
for (n = N; n <= 4*N ; n *= 2)
{ if (n == N) first_line = true; 
GT = SR = SS = ICR = ICS = RICR = RICS = K = 0;
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

 UT = used_time(T); GT += UT;

I.write_demo("time for generation of n random points = ",UT);

list<rat_point> CH;

CH = CONVEX_HULL_S(L);

int k = CH.length();

K += k;

I.write_demo("number of vertices of hull = ",k);

UT = used_time(T); SR += UT;

/*
I.write_demo("time for CONVEX_HULL_S: random input",UT);

CH = CONVEX_HULL_S(LS);

UT = used_time(T); SS += UT;

I.write_demo("time for CONVEX_HULL_S: sorted input",UT);
*/

CH = CONVEX_HULL_IC(L);

UT = used_time(T); ICR += UT;

I.write_demo("time for CONVEX_HULL_IC: random input",UT);

CH = CONVEX_HULL_IC(LS);

UT = used_time(T); ICS += UT;

I.write_demo("time for CONVEX_HULL_IC: sorted input",UT);
  

CH = CONVEX_HULL_RIC(L);

UT = used_time(T); RICR += UT;

I.write_demo("time for CONVEX_HULL_RIC: random input",UT);

CH = CONVEX_HULL_RIC(LS);

UT = used_time(T); RICS += UT;

I.write_demo("time for CONVEX_HULL_RIC: sorted input",UT);

#ifdef BOOK
}
I.write_table(" & ",n);
I.write_table(" & ",GT/L);
I.write_table(" & ",K/L);
//I.write_table(" & ",SR/L);
//I.write_table(" & ",SS/L);
I.write_table(" & ",ICR/L);
I.write_table(" & ",ICS/L);
I.write_table(" & ",RICR/L);
I.write_table(" & ",RICS/L);
I.write_table(" \\\\ \\hline ");
}
#endif
}

}
#ifdef ASCII
print_statistics();
#endif

}



