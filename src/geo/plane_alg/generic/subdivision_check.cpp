/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  subdivision_check.c
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
//static 
inline VECTOR edge_vector(const GRAPH<POINT,int>& G, const edge& e)
{ return G[G.target(e)] - G[G.source(e)]; }
#endif

LEDA_END_NAMESPACE

#include <LEDA/geo/generic/geo_check.h>


LEDA_BEGIN_NAMESPACE

static bool False_ICS(const string& s)
{ cerr << "Is_Convex_Subdivision: " << s; return false; }

bool Is_Convex_Subdivision(const GRAPH<POINT,int>& G, 
                           bool& is_triangulated)
{
  is_triangulated = true;
  
  if ( !Is_Connected(G) )
    return False_ICS("G is not connected");

  if ( !Is_CCW_Ordered_Plane_Map(G) )
    return False_ICS("G is not a CCW-ordered plane map");

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  cmp_edges_by_angle<GRAPH<POINT,int> > cmp(G);

  if ( m == 2*n - 2) { 
                       node v;

                       if ( n <= 1 ) return true;

                       forall_nodes(v,G) 
                       { if ( G.outdeg(v) > 2 ) 
                            return False_ICS("G is a tree but not a chain");
                         if (G.outdeg(v) == 1) continue;
                         edge e1 = G.first_adj_edge(v);
                         edge e2 = G.last_adj_edge(v);
                         node w = G.target(e1);
                         node u = G.target(e2);
                         if ( G[v] == G[w] || G[v] == G[u] ) 
                            return False_ICS("nodes at equal positions");
                         if ( cmp(e1,G.reversal(e2)) != 0 )
                            return False_ICS("direction not opposite");
                       }
                       return true;
 }
  
  edge e;
  edge_array<bool> considered(G,false);
  bool already_seen_unbounded_face = false;
  forall_edges(e,G)
  { if ( !considered[e] )
    { // check the face to the left of e
      POINT a = G[source(e)];
      POINT b = G[target(e)];
      POINT c = G[target(G.face_cycle_succ(e))];
      int orient = orientation(a,b,c);
      int n = 0;
      edge e0 = e;
      do { considered[e] = true;
           e = G.face_cycle_succ(e);
           n++;
      } while ( e != e0);

      if ( orient > 0 ) 
      { if ( n > 3 )
        { is_triangulated = false;
          if ( !Is_CCW_Convex_Face_Cycle(G,e) )
            return False_ICS("non-convex bounded face"); 
        }
      }
      else
      { if ( already_seen_unbounded_face )
          return False_ICS("two faces qualify for unbounded face");
        already_seen_unbounded_face = true;
        if ( !Is_CW_Weakly_Convex_Face_Cycle(G,e) )
          return False_ICS("unbounded face is not weakly convex"); 
      }
    }
  }
  return true;

}


bool Is_Convex_Subdivision(const GRAPH<POINT,int>& G)
{ bool is_triangulated;
  return Is_Convex_Subdivision(G, is_triangulated);
}

bool Is_Triangulation(const GRAPH<POINT,int>& G)
{ bool is_triangulated;
  return Is_Convex_Subdivision(G, is_triangulated) && is_triangulated;
}
 
LEDA_END_NAMESPACE 
 
