/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delaunay_check.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE

#ifndef LEDA_EDGE_VECTOR_DEFINED
#define LEDA_EDGE_VECTOR_DEFINED
// static 
inline VECTOR edge_vector(const GRAPH<POINT,int>& G, const edge& e)
{ return G[G.target(e)] - G[G.source(e)]; }
#endif

LEDA_END_NAMESPACE


#include <LEDA/geo/generic/geo_check.h>

LEDA_BEGIN_NAMESPACE

static bool False_IDT(const string& s)
{ cerr << "Is_Delaunay_Triangulation: " << s; return false; }

bool Is_Delaunay_Triangulation(const GRAPH<POINT,int>& G, 
                               delaunay_voronoi_kind kind)
{ if ( !Is_Triangulation(G) )
    return False_IDT("G is no triangulation");

  if (G.number_of_edges() == 2*G.number_of_nodes() - 2) 
    return true;

  
  edge e;
  edge_array<bool> considered(G,false);
  forall_edges(e,G)
  { if (!considered[e])
    { // check the faces incident to e and reversal(e)
      considered[e] = considered[G.reversal(e)] = true;

      POINT a = G[source(e)];
      POINT b = G[target(G.cyclic_adj_pred(e))];
      POINT c = G[target(e)];
      POINT d = G[target(G.face_cycle_succ(e))]; 
        
      if (left_turn(a,b,c) && left_turn(b,c,d) &&
            left_turn(c,d,a) && left_turn(d,a,b) )
      { // the faces to the left and right of e are bounded

        int s = side_of_circle(a,b,c,d);

        /* +1 for inside, -1 for outside */
        if ( (kind == NEAREST && s > 0) || 
            (kind == FURTHEST && s < 0) )
          return False_IDT("violated Delaunay property");
      }
    }
  }


  return true;
}


static bool False_IDD(const string& s)
{ cerr << "Is_Delaunay_Diagram: " << s; return false; }

bool Is_Delaunay_Diagram(const GRAPH<POINT,int>& G, 
                         delaunay_voronoi_kind kind)
{ 
  if ( !Is_Convex_Subdivision(G) )
  return False_IDD("G is no convex subdivision");

  edge e;
  edge_array<bool> considered(G,false);
  forall_edges(e,G)
  { if (!considered[e])
    { // check the face to the left of e

      POINT a = G[source(e)];
      POINT c = G[target(e)];
      POINT d = G[target(G.face_cycle_succ(e))]; 

      if ( left_turn(a,c,d) )
      { // face is bounded
        CIRCLE C(a,c,d);
        edge e0 = e;
        do
        { considered[e] = true;
          if ( !C.contains(G[source(e)]) )
          return False_IDD("face with non-co-circular vertices");
          e = G.face_cycle_succ(e);
        } while ( e != e0 );
      }
      else 
      { // face is unbounded
        edge e0 = e;
        do
        { considered[e] = true;
          e = G.face_cycle_succ(e);
        } while ( e != e0 );
      }
    }
  }
  { 
    edge e;
    edge_array<bool> considered(G,false);
    forall_edges(e,G)
    { if (!considered[e])
      { // check the faces incident to e and reversal(e)
        considered[e] = considered[G.reversal(e)] = true;

        POINT a = G[source(e)];
        POINT b = G[target(G.cyclic_adj_pred(e))];
        POINT c = G[target(e)];
        POINT d = G[target(G.face_cycle_succ(e))]; 
          
        if (left_turn(a,b,c) && left_turn(b,c,d) &&
              left_turn(c,d,a) && left_turn(d,a,b) )
        { // the faces to the left and right of e are bounded

          int s = side_of_circle(a,b,c,d);

          /* +1 for inside, -1 for outside */
          if ( (kind == NEAREST && s > 0) || 
              (kind == FURTHEST && s < 0) )
            return False_IDD("violated Delaunay property");
        }
      }
    }
 }
  return true;
}

LEDA_END_NAMESPACE

