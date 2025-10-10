/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  flip_delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/geo_global_enums.h>

LEDA_BEGIN_NAMESPACE

int DELAUNAY_FLIPPING(GRAPH<POINT,int>& G, 
                      delaunay_voronoi_kind k)
{
  if (G.number_of_nodes() <= 3) return 0;

  int f = ( k == NEAREST ? +1 : -1);

  list<edge> S = G.all_edges();

  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);

    
    if (G[e] == HULL_DART || G[r] == HULL_DART) continue;

    G[e] = DIAGRAM_DART;
    G[r] = DIAGRAM_DART;

    // e1,e2,e3,e4: edges of quadriliteral with diagonal e
    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    // flip test
    POINT a = G[source(e1)];
    POINT b = G[target(e1)];
    POINT c = G[source(e3)];
    POINT d = G[target(e3)];

    if ( left_turn(d,a,b) && left_turn(b,c,d) )
    { // the quadrilateral is convex

      
      int soc = f * side_of_circle(a,b,c,d);

      if (soc == 0) // co-circular quadriliteral(a,b,c,d) 
      { G[e] = NON_DIAGRAM_DART;
        G[r] = NON_DIAGRAM_DART;
      }

      if (soc > 0) // flip
      { edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);
        
        S.push(e1); 
        S.push(e2); 
        S.push(e3); 
        S.push(e4); 
        
        // flip diagonal
        G.move_edge(e,e2,source(e4));
        G.move_edge(r,e4,source(e2));
        flip_count++;
      }

    }

  }

  return flip_count;
}


int DELAUNAY_FLIPPING(GRAPH<POINT,int>& G, list<edge> S, 
                      delaunay_voronoi_kind k)
{
  // Transforms graph G into a Delaunay triangulation by flipping edges.
  // Diagonals of co-circular convex quadriliterals are marked as 
  // NON_DIAGRAM_DART
  // We maintain a stack $S$ of edges containing diagonals which might
  // have to be flipped. 

  if (G.number_of_nodes() <= 3) return 0;

    int f = ( k == NEAREST ? +1 : -1);

  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);

    if (G[e] == HULL_DART || G[r] == HULL_DART) continue;

    G[e] = DIAGRAM_DART;
    G[r] = DIAGRAM_DART;

    // e1,e2,e3,e4: edges of quadriliteral with diagonal e
    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    // flip test
    POINT a = G[source(e1)];
    POINT b = G[target(e1)];
    POINT c = G[source(e3)];
    POINT d = G[target(e3)];

    if (left_turn(b,d,a) && right_turn(b,d,c))
    { // the quadrilateral is convex

      int soc = f*side_of_circle(a,b,c,d);

      if (soc == 0) // co-circular quadirliteral(a,b,c,d) 
      { G[e] = NON_DIAGRAM_DART;
        G[r] = NON_DIAGRAM_DART;
       }

      if (soc > 0) // flip
      { edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);
  
        S.push(e1); 
        S.push(e2); 
        S.push(e3); 
        S.push(e4); 
  
        // flip diagonal
        G.move_edge(e,e2,source(e4));
        G.move_edge(r,e4,source(e2));
        flip_count++;
      }
    }
  }

 return flip_count;

}


int DELAUNAY_FLIP(const list<POINT>& L, GRAPH<POINT,int>& G)
{ TRIANGULATE_POINTS(L,G);
  if (G.number_of_edges() == 0) return 0;
  return DELAUNAY_FLIPPING(G,NEAREST);
}


int F_DELAUNAY_FLIP(const list<POINT>& L, GRAPH<POINT,int>& G)
{ 
  list<POINT> H = CONVEX_HULL(L);
  TRIANGULATE_POINTS(H,G);
  if (G.number_of_edges() == 0) return 0;
  return DELAUNAY_FLIPPING(G,FURTHEST);
}




// flip constrained triangulations

int DELAUNAY_FLIPPING(GRAPH<POINT,SEGMENT>& G)
{
  if (G.number_of_nodes() <= 3) return 0;

  // int f = ( k == NEAREST ? +1 : -1);

  int f = 1;

  list<edge> S;
  edge e;
  forall_edges(e,G)
    if (G[e].is_trivial()) S.append(e);

  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);

    
    // e1,e2,e3,e4: edges of quadriliteral with diagonal e
    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    node A = source(e1);
    node B = target(e1);
    node C = source(e3);
    node D = target(e3);

    // flip test
    POINT a = G[A];
    POINT b = G[B];
    POINT c = G[C];
    POINT d = G[D];

    if ( left_turn(d,a,b) && left_turn(b,c,d) )
    { // the quadrilateral is convex

      int soc = f * side_of_circle(a,b,c,d);

      if (soc > 0) // flip
      { edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);

        // flip diagonal
        G.move_edge(e,e2,source(e4));
        G.move_edge(r,e4,source(e2));
        if (G[e1].is_trivial()) S.push(e1); 
        if (G[e2].is_trivial()) S.push(e2); 
        if (G[e3].is_trivial()) S.push(e3); 
        if (G[e4].is_trivial()) S.push(e4); 
        flip_count++;
      }

    }

  }

  return flip_count;
}

LEDA_END_NAMESPACE
