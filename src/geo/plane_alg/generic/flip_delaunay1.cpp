/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  flip_delaunay1.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



LEDA_BEGIN_NAMESPACE


//------------------------------------------------------------------------------
//  DELAUNAY_FLIPPING
//------------------------------------------------------------------------------


// We use bidirected graph of type GRAPH<POINT,edge> for representing planar 
// subdvivisons 
// G[v] = position of v 
// G[e] in { DIAGRAM_EDGE, NON_DIGRAM_EDGE, HULL_EDGE }


int DELAUNAY_FLIPPING(GRAPH<POINT,int>& G, list<edge> S, int f)
{
  // Tranforms graph G into a Delaunay triangulation by flipping edges.
  // Diagonals of co-circular convex quadriliterals are marked as 
  // NON_DIAGRAM_EDGE
  // We maintain a stack $S$ of edges containing diagonals which might
  // have to be flipped. 

  if (G.number_of_nodes() <= 3) return 0;

  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);

    if (G[e] == HULL_EDGE || G[r] == HULL_EDGE) continue;

    G[e] = DIAGRAM_EDGE;
    G[r] = DIAGRAM_EDGE;

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
      { G[e] = NON_DIAGRAM_EDGE;
        G[r] = NON_DIAGRAM_EDGE;
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


int DELAUNAY_FLIPPING(GRAPH<POINT,int>& G, int f)
{  return DELAUNAY_FLIPPING(G,G.all_edges(),f); }



int DELAUNAY_FLIP(const list<POINT>& L, GRAPH<POINT,int>& G)
{ TRIANGULATE_POINTS(L,G);
  if (G.number_of_edges() == 0) return 0;
  return DELAUNAY_FLIPPING(G);
}


int F_DELAUNAY_FLIP(const list<POINT>& L, GRAPH<POINT,int>& G)
{ 
  list<POINT> H = CONVEX_HULL(L);
  TRIANGULATE_POINTS(H,G);
  if (G.number_of_edges() == 0) return 0;
  return DELAUNAY_FLIPPING(G,-1);
}


LEDA_END_NAMESPACE
