/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sweep_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>

using namespace leda;


using std::cout;
using std::flush;
using std::endl;




enum { RANDOM, DIFFI };

int main(int argc, char** argv)
{ 
  int mode = RANDOM;

  if (argc > 1) 
  { if (string(argv[1]) == "-d") mode = DIFFI;
    else if (string(argv[1]) == "-r") mode = RANDOM;
         else error_handler(1,"usage: sweep_test [-r|-d]");
   }

  int N  = read_int("N  = ");
  int s = 0;

rand_int.set_seed(N);

  if (mode == DIFFI)  s = read_int("s = ");

  for (int k = 10; k <= 300; k+=10)
  {
    list<rat_segment> seglist;
    list<segment> seglist1;

    GRAPH<rat_point,rat_segment> G0;
    GRAPH<rat_point,rat_segment> G1;
    GRAPH<point,segment> G2;

   integer size = integer(1) << k;
   integer y = size;
   integer d = 2*size/(N-1);
   
   if (mode == DIFFI)
     for(int i=0; i < N; i++)
     { integer x1 = -size + rand_int(-s,s);
       integer y1 = y + rand_int(-s,s);
       integer x2 = size + rand_int(-s,s);
       integer y2 = -y + rand_int(-s,s);
       integer w1 = 1;
       integer w2 = 1;
       y += d;
  
       rat_segment s(rat_point(x1,y1,w1),rat_point(x2,y2,w2));
       seglist.append(s);
  
       segment s1(point(x1.to_float(),y1.to_float()),
                  point(x2.to_float(),y2.to_float()));
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
        segment s1(point(x1.to_float(),y1.to_float()),
                   point(x2.to_float(),y2.to_float()));
        seglist1.append(s1);
       }
    

cout << string("%4d&",k) << flush;

float T;

rat_point::use_filter = false;
rat_segment::use_filter = false;
T = used_time();
SWEEP_SEGMENTS(seglist,G0);
float t1 = used_time(T);

cout << string("  %5d&  %5d&  %6.2f&",
                G0.number_of_nodes(), G0.number_of_edges(), t1) << flush;

rat_point::use_filter = true;
rat_segment::use_filter = true;
T = used_time();
SWEEP_SEGMENTS(seglist,G1);
float t2 = used_time(T);

if (G1.number_of_nodes() != G0.number_of_nodes() ||
    G1.number_of_edges() != G0.number_of_edges()) t2 = -99999;

if (G1.number_of_nodes() != G0.number_of_nodes() ||
    G1.number_of_edges() != G0.number_of_edges())
   cout << "  error&" << flush;
else
   cout << string("  %6.2f&", t2) << flush;


T = used_time();
SWEEP_SEGMENTS(seglist1,G2);
float t3 = used_time(T);

if (G2.number_of_nodes() != G0.number_of_nodes() ||
    G2.number_of_edges() != G0.number_of_edges())
   cout << "    error \\\\" << endl;
else
   cout << string("  %6.2f \\\\", t3) << endl;

 }
  
  return 0;
}

