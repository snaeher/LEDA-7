/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3win_test_graph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window_stream.h>
#include <LEDA/geo/d3_hull.h>

using namespace leda;

int main()
{
 d3_window_stream DW("Graph test");

 DW.set_color(red);
 
 list<d3_rat_point> input;
 list<d3_point> L;
 GRAPH<d3_point, int>  G;
 
 //generate graph
 random_points_in_ball(300, 500, input);
 d3_rat_point iter;
 
 forall(iter, input) L.append(iter.to_float());
 D3_HULL(L,G);
 
 //DW << G;
 
 int cnt = 0;
 map<node,color> NM;
 map<edge,color> EM;
 edge e, rev;
 forall_edges(e,G){
   rev = G.reversal(e);
   if (EM.defined(rev)) EM[e] = EM[rev];
   else EM[e] = color(cnt%15);
   cnt++;
 }
 
 list<edge> LE;
 list<node> LN;
 d3_window_item n;
 DW.insert_graph(G,NM,EM,n);

 DW.show();
 
 return 0;
}
