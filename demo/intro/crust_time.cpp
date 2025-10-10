/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  crust_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




#include <LEDA/graph/graph.h>
#include <LEDA/core/map.h>
#include <LEDA/geo/float_kernel.h>
#include <LEDA/geo/geo_alg.h>

using namespace leda;

using std::cout;


void crust(const list<point>& S, GRAPH<point,int>& G) 
{
  list<point> L = S;

  GRAPH<circle,point> VD;

  VORONOI(L,VD);

  // add Voronoi vertices and mark them

  map<point,bool> voronoi_vertex(false);

  node v;
  forall_nodes(v,VD)
  { if (VD.outdeg(v) < 2) continue; 
    point p = VD[v].center();
    voronoi_vertex[p] = true;
    L.append(p);
  }

  DELAUNAY_TRIANG(L,G);

  list<node> vlist;
  forall_nodes(v,G)
     if (voronoi_vertex[G[v]]) vlist.append(v);

  G.del_nodes(vlist);
}


int main()
{ int n = read_int("number of points = ");
  
  list<point> S;
  random_points_in_unit_square(n,S);
  GRAPH<point,int> G;
   
  float T = used_time();
  crust(S,G);
  cout << "\n\nThe crust computation took " << 
          used_time(T) << " seconds.\n\n";

  return 0;
}

