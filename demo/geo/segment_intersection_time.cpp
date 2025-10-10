/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  segment_intersection_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:15 $


#include <LEDA/geo/plane_alg.h>
#include <LEDA/core/IO_interface.h>
#include <assert.h>

using namespace leda;



int int_count;

void count(const rat_segment& s1, const rat_segment& s2)
{ int_count++; }



int main()
{ IO_interface I("LINE SEGMENT INTERSECTION");

I.write_demo("We illustrate the various solutions for the segment \
intersection problem. \n\n\
The input consists of N random segment chosen as follows:\n\
For the first endpoint we chose random k bit coordinates.\n\
The second endpoint is obtained from the first by adding a vector with \
random s bit coordinates.\n\n\
The number of intersections grows with s.");


int N  = I.read_int("N = ",2000);
int k  = I.read_int("k = ",30);
int s  = I.read_int("s = ",5);

#ifdef BOOK
for (s = k - 8; s <= k; s++)
#endif
  {
    list<rat_segment> seglist;

    GRAPH<rat_point,rat_segment> G0;
    GRAPH<rat_point,rat_segment> G1;
    GRAPH<point,segment> G2;

   integer size = integer(1) << k;
   integer S    = integer(1) << s;

   for(int i=0; i < N; i++)
      { integer x1 = integer::random(k);
        integer y1 = integer::random(k);
        integer w1 = 1;
        integer x2 = x1 + integer::random(s);
        integer y2 = y1 + integer::random(s);
        integer w2 = 1;
        rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
        seglist.append(s);
      }
    

I.write_table("\n ",N);

I.write_table(" & ",s);

float T = used_time(); float UT;

SWEEP_SEGMENTS(seglist,G0,false,true);

UT = used_time(T);

int n0 = G0.number_of_nodes();
int m0 = G0.number_of_edges();

I.write_table(" & ", n0);
I.write_table(" & ", m0);
I.write_table(" & ", UT);

I.write_demo("number of segments = ",N);
I.write_demo("number of nodes of G = ",n0);
I.write_demo("number of edges of G = ",m0);
I.write_demo("SWEEP SEGMENTS, no embedding = ",UT);

SWEEP_SEGMENTS(seglist,G0,true,true);

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("SWEEP SEGMENTS, with embedding",UT);

MULMULEY_SEGMENTS(seglist,G0,false);

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("MULMULEY SEGMENTS, no embedding = ",UT);

MULMULEY_SEGMENTS(seglist,G0,true);

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("MULMULEY SEGMENTS, with embedding",UT);

int_count = 0;

TRIVIAL_SEGMENTS(seglist,count);

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("TRIVIAL SEGMENTS ",UT);

int_count = 0; 
BALABAN_SEGMENTS(seglist,count);

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("BALABAN SEGMENTS ",UT);

I.write_table("\\\\ ");

}
}

