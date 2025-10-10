/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  constr_triang.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/gen_plane_sweep.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/map.h>

#include "constr_triang_traits.h"

LEDA_BEGIN_NAMESPACE


#if defined(__hpuxcc__)
int compare(const SEGMENT&, const SEGMENT&)
{ LEDA_EXCEPTION(1,"constr_triang: compare called.");
  return 0;
}
#endif


edge TRIANGULATE_PLANE_MAP(GRAPH<POINT,SEGMENT>& G)
{ if (G.empty()) return nil;
  typedef leda_constr_triang_traits<POINT,SEGMENT,LINE,VECTOR,COORD> ct_traits;
/*
  typedef gen_plane_sweep<ct_traits> ct_sweep;
*/
  typedef gen_plane_sweep<ct_traits,ct_traits::INPUT_ITERATOR,ct_traits::OUTPUT>                                                                       ct_sweep;
  ct_sweep CTR(G); 
  CTR.sweep();
  return G.last_adj_edge(G.first_node());
 }



edge TRIANGULATE_SEGMENTS(const list<SEGMENT>& L, GRAPH<POINT,SEGMENT>& G)
{ G.clear(); 
  if (L.empty()) return nil;
  SWEEP_SEGMENTS(L, G, true); 
  Make_Simple(G); 
  G.make_map();
  return TRIANGULATE_PLANE_MAP(G); 
 }


edge DELAUNAY_TRIANG(GRAPH<POINT,SEGMENT>& G)
{ edge e_hull = TRIANGULATE_PLANE_MAP(G);
  DELAUNAY_FLIPPING(G);
  return e_hull;
}

edge DELAUNAY_TRIANG(const list<SEGMENT>& L, GRAPH<POINT,SEGMENT>& G)
{ edge e_hull = TRIANGULATE_SEGMENTS(L,G);
  DELAUNAY_FLIPPING(G);
  return e_hull;
}






edge TRIANGULATE_POLYGON(const GEN_POLYGON& poly, GRAPH<POINT,SEGMENT>& G, 
                                                  list<edge>& inner_edges,
                                                  list<edge>& outer_edges,
                                                  list<edge>& boundary_edges,
                                                  list<edge>& hole_edges)						  
{ 
  inner_edges.clear();
  outer_edges.clear();
  boundary_edges.clear();
  hole_edges.clear();

  if (poly.trivial()) return nil;

  list <SEGMENT> S;

  map<SEGMENT,int> orient;

  POLYGON pol;
  forall(pol,poly.polygons())
  { int ori = pol.orientation();
    const list<POINT>& vlist = pol.vertices();
    POINT last = vlist.tail();
    POINT p;
    forall(p,vlist)
    { SEGMENT seg(last,p);
      orient[seg] = ori;
      S.append(seg);
      last = p;
    }
  }
  
  
  SWEEP_SEGMENTS(S,G,true);

  Make_Simple(G); 
  G.make_map();

  edge hull_e = TRIANGULATE_PLANE_MAP(G);
  DELAUNAY_FLIPPING(G);
  
  // classify edges 

  edge e;
  forall_edges(e,G) 
  { 
    SEGMENT seg = G[e];

    if (seg.is_trivial() || seg.source() == G[source(e)]) continue; 

    inner_edges.append(G.reversal(e));

    if (orient[seg] > 0)
       boundary_edges.append(e); 
    else 
       hole_edges.append(e); 


    edge x = G.cyclic_adj_succ(e);
    while (G[x].is_trivial())
    { outer_edges.append(x); 
      x = G.cyclic_adj_succ(x);
     }

    edge y = G.cyclic_adj_pred(e);
    while (G[y].is_trivial())
    { inner_edges.append(y); 
      y = G.cyclic_adj_pred(y);
     }
   }

  return hull_e;
}



edge CONVEX_COMPONENTS(const GEN_POLYGON& GP, GRAPH<POINT,SEGMENT>& G,
                                              list<edge>& inner_edges,
                                              list<edge>& boundary_edges,
                                              list<edge>& hole_edges)
{   
  G.clear();

  inner_edges.clear(); 
  boundary_edges.clear(); 
  hole_edges.clear();
  
  if (GP.area() < 0) return nil; // gp "open"
  
  list<edge> outer_edges;
  edge hull_e = TRIANGULATE_POLYGON(GP,G,inner_edges,outer_edges,boundary_edges,
                                                                 hole_edges);

  if (!hull_e) return nil; // trivial

  inner_edges.permute(); // sn    

  stack<edge> ST;
  list<edge>  tmp;
  edge_array<bool> marked(G,false);

  
  // guard against removing inner edges which are reversals of 
  // boundary of hole edges

  edge e;
  forall(e, boundary_edges) 
  { marked[e] = true; 
    tmp.append(G.reversal(e));
   }

  forall(e, hole_edges) 
  { marked[e] = true; 
    tmp.append(G.reversal(e));
   }


  forall(e,inner_edges) 
  { if (marked[G.reversal(e)]) continue;
    ST.push(e);
    marked[e] = true;
  }

  inner_edges = tmp;

  while (!ST.empty()) 
  {
   edge e = ST.pop();

   // get the points for the convexity test

   edge rev = G.reversal(e);

   node nb2 = G.source(e); 
   node na2 = G.target(e);
   node na3 = G.target(G.face_cycle_succ(e)); 
   node nb3 = G.source(G.face_cycle_pred(e));
   node nb1 = G.target(G.face_cycle_succ(rev)); 
   node na1 = G.source(G.face_cycle_pred(rev));

   POINT a1 = G[na1]; 
   POINT a2 = G[na2]; 
   POINT a3 = G[na3];

   POINT b1 = G[nb1]; 
   POINT b2 = G[nb2]; 
   POINT b3 = G[nb3];
  
   int o1 = orientation(a1,a2,b2);
   int o2 = orientation(a1,a2,a3);
  
   if (o2==0 || o1==o2)
   { o1 = orientation(b1,b2,a2); 
     o2 = orientation(b1,b2,b3);
     if (o2==0 || o1==o2)
     { // convex: we have to delete e and its reversal edge ...
       G.del_edge(e);
       G.del_edge(rev);
       continue;
      }
    }

   inner_edges.append(e); 
   inner_edges.append(rev); 
  }

  return hull_e;
}

   

edge TRIANGULATE_POLYGON(const POLYGON& poly, GRAPH<POINT,SEGMENT>& G, 
                                              list<edge>& inner_edges,
                                              list<edge>& outer_edges,
                                              list<edge>& boundary_edges)
{ 
   GEN_POLYGON gpoly(poly,GEN_POLYGON::NO_CHECK);
   list<edge> hole_edges;
   return TRIANGULATE_POLYGON(gpoly,G,inner_edges,outer_edges,
                                                  boundary_edges,
                                                  hole_edges);
}

   
edge CONVEX_COMPONENTS(const POLYGON& poly, GRAPH<POINT,SEGMENT>& G, 
                                            list<edge>& inner_edges,
                                            list<edge>& boundary_edges)
{ 
  GEN_POLYGON gpoly(poly,GEN_POLYGON::NO_CHECK);
  list<edge> hole_edges;
  return CONVEX_COMPONENTS(gpoly,G,inner_edges, boundary_edges, hole_edges);
}




static void collect_faces(GRAPH<POINT,SEGMENT>& G, 
                          const list<edge>& inner_edges, list<POLYGON>& parts)
{ 
  parts.clear();
  
  edge_array<bool> visited(G,true);
 
  edge e;
  forall(e,inner_edges) visited[e] = false;
  
  forall(e,inner_edges)
  { if (visited[e])  continue;
    list<POINT> L;
    edge x = e;
    do { L.append(G[source(x)]);
         visited[x] = true;
         x = G.face_cycle_succ(x);
       } while (x != e);
    parts.append(POLYGON(L));
  }
}



list<POLYGON> TRIANGLE_COMPONENTS(const GEN_POLYGON& P)
{ 
  if (P.size() < 3) return P.polygons();

  list<POLYGON> parts;

  GRAPH<POINT,SEGMENT> G;
  list<edge> inner_edges;
  list<edge> outer_edges;
  list<edge> boundary_edges;
  list<edge> hole_edges;
  TRIANGULATE_POLYGON(P,G,inner_edges,outer_edges,boundary_edges,hole_edges);

  collect_faces(G,inner_edges,parts);

  return parts;
}



list<POLYGON> CONVEX_COMPONENTS(const GEN_POLYGON& P)
{ 
  if (P.size() < 3) return P.polygons();

  list<POLYGON> parts;

  GRAPH<POINT,SEGMENT> G;
  list<edge> inner_edges;
  list<edge> boundary_edges;
  list<edge> hole_edges;
  CONVEX_COMPONENTS(P,G,inner_edges,boundary_edges,hole_edges);

  collect_faces(G,inner_edges,parts);

  return parts;
}

LEDA_END_NAMESPACE

