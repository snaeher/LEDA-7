/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  sweep_time.c
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

using std::cerr;
using std::endl;



enum { RANDOM = 0, DIFFI = 1 };

int main()
{ IO_interface I("LINE SEGMENT INTERSECTION");

  I.write_demo("We illustrate the plane sweep algorithm for line segment \
intersection.\n\n\
We use two kinds of inputs:\n\
  N random segments whose endpoints have k bit random coordinates.\n\
  N segments whose endpoints have k bit coordinates and which intersect \
near the origin. \n\n\
We use the algorithm with rational arithmetic, with rational arithmetic with \
floating point filter, and with floating point arithmetic. In each case we \
run the algorithm without and with the optimization described \
in the LEDA book.\n\n\
Observe that in either case the number of intersections is about N*N.");

int mode; 
#ifdef BOOK
#ifdef RANDOM_INPUT
mode = RANDOM;
#else
mode = DIFFI;
#endif
#else
mode = I.read_int("Type zero for random inputs (any other number for the difficult input): ");
if ( mode != RANDOM ) mode = DIFFI;
#endif

int N  = I.read_int("number of segments = ",200);
int k  = I.read_int("endpoints have k bit coordinates, where k = ",10);
int s = 10;

if (mode == DIFFI)  s = I.read_int("define a non-negative perturbation s; \
for s = 0 all segments pass through the origin and hence the graph will have \
exactly 2N + 1 nodes and 2N edges: s = ",10);

#ifdef BOOK
for (k = 10; k <= 100; k+=10)
#endif
  {
    list<rat_segment> seglist;
    list<segment> seglist1;

    GRAPH<rat_point,rat_segment> G0;
    GRAPH<rat_point,rat_segment> G1;
    GRAPH<point,segment> G2;

   integer size = integer(1) << k;
   integer y = size;
   integer d = (N <= 1 ? size : 2*size/(N-1));
   
   if (mode == DIFFI)
     for(int i=0; i < N; i++)
     { integer x1 = size + rand_int(-s,s);
       integer y1 = size + y + rand_int(-s,s);
       integer x2 = 3*size + rand_int(-s,s);
       integer y2 = 3*size -y + rand_int(-s,s);
       integer w1 = 1;
       integer w2 = 1;
       y += d;
  
       rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
       seglist.append(s);
  
       segment s1(point(x1.to_double(),y1.to_double()),
                  point(x2.to_double(),y2.to_double()));
       seglist1.append(s1);
  
      }
   
    if (mode == RANDOM)
    for(int i=0; i < N; i++)
      { integer x1 = integer::random(k)-size;
        integer y1 = integer::random(k)-size;
        integer w1 = 1;
        integer x2 = integer::random(k)-size;
        integer y2 = integer::random(k)-size;
        integer w2 = 1;
        rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
        seglist.append(s);
        segment s1(point(x1.to_double(),y1.to_double()),
                   point(x2.to_double(),y2.to_double()));
        seglist1.append(s1);
       }
    

I.write_table("\n ",k);

float T = used_time(); float UT;

rat_point::use_filter = false;
rat_segment::use_filter = false;


SWEEP_SEGMENTS(seglist,G0,false,false);

UT = used_time(T);

int n0 = G0.number_of_nodes();
int m0 = G0.number_of_edges();

I.write_table(" & ", n0);
I.write_table(" & ", m0);
I.write_table(" & ", UT);

I.write_demo("number of nodes of G = ",n0);
I.write_demo("number of edges of G = ",m0);
I.write_demo("running time with rational kernel, no filter, no optimization = ",UT);

SWEEP_SEGMENTS(seglist,G0,false,true);

if (n0 != G0.number_of_nodes() || m0 != G0.number_of_edges() )
cerr << "ERROR: RATIONAL KERNEL WITH OPTIMIZATION";

UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("running time with rational kernel, no filter, with optimization = ",UT);

rat_point::use_filter = true;
rat_segment::use_filter = true;

SWEEP_SEGMENTS(seglist,G1,false,false);

UT = used_time(T);

I.write_table(" & ", UT);

I.write_demo("running time with rational kernel, with filter, no optimization = ",UT);

SWEEP_SEGMENTS(seglist,G1,false,true);

if (G1.number_of_nodes() != n0 ||
    G1.number_of_edges() != m0 ) 
cerr << "\n\nFILTER INCORRECT: RESULT DIFFERS FROM RESULT WITHOUT FILTER: \
NO OPTIMIZATION.";


UT = used_time(T);
I.write_table(" & ", UT);
I.write_demo("running time with rational kernel, with filter, with optimization = ",UT);

if (G1.number_of_nodes() != n0 ||
    G1.number_of_edges() != m0 ) 
cerr << "\n\nFILTER INCORRECT: RESULT DIFFERS FROM RESULT WITHOUT FILTER: \
WITH OPTIMIZATION";


SWEEP_SEGMENTS(seglist1,G2,false,false);
UT = used_time(T);

if (G2.number_of_nodes() != n0 ||
    G2.number_of_edges() != m0)
{ I.write_table(" & error" );
I.write_demo("floating point kernel without optimization gives incorrect result.");
I.write_demo("additional number of nodes = ",G2.number_of_nodes() - n0);
I.write_demo("additional number of edges = ",G2.number_of_edges() - m0);
}
else
  { I.write_table(" & ", UT);
I.write_demo("running time with floating point kernel, no optimization = ",UT);
  }

SWEEP_SEGMENTS(seglist1,G2,false,true);
UT = used_time(T);

if (G2.number_of_nodes() != n0 ||
    G2.number_of_edges() != m0 )
{ I.write_table(" & error" );
I.write_demo("floating point kernel with optimization gives incorrect result.");
I.write_demo("additional number of nodes = ",G2.number_of_nodes() - n0);
I.write_demo("additional number of edges = ",G2.number_of_edges() - m0);

}
else
  { I.write_table(" & ", UT);
I.write_demo("running time with floating point kernel, with optimization = ",UT);
  }
I.write_table("\\\\ ");
}
return 0;
}




