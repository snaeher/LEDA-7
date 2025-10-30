/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/d3_hull.h>
#include <LEDA/graphics/d3_window.h>

using namespace leda;

int main()
{
  // construct a random set of points L
  list<d3_rat_point> L;
  random_d3_rat_points_in_ball(50,75,L);

  // construct the convex hull H of L
  GRAPH<d3_rat_point,int> H;
  CONVEX_HULL(L,H);

  // open a window W

  window W("d3 hull demo");
  W.init(-100,+100,-100);
  W.display(window::center,window::center);

  // extract the node positions into an array of vectors
  node_array<rat_vector> pos(H);
  node v;
  forall_nodes(v,H) pos[v] = H[v].to_vector();

  // and display H in a d3_window for window W
  d3_window d3win(W,H,pos);

  d3win.read_mouse();

  W.screenshot("d3_hull");
  return 0;
}
