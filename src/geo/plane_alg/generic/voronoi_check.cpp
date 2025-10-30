/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  voronoi_check.c
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
#include <LEDA/core/d_array.h>


LEDA_BEGIN_NAMESPACE


//static 
inline VECTOR edge_vector(const GRAPH<CIRCLE,POINT>& G, const edge& e)
{ const CIRCLE& C = G[G.source(e)];
  const CIRCLE& D = G[G.target(e)];
  if ( D.is_degenerate() )
    { POINT a = D.point1();
      POINT c = D.point3();
      POINT m = midpoint(a,c);
      return m - a.rotate90(m);
    }
  if ( C.is_degenerate() ) 
    { POINT a = C.point1();
      POINT c = C.point3();
      POINT m = midpoint(a,c);
      return a.rotate90(m) - m;
    }
  // both circles are non-degenerate
  return D.center() - C.center();
}


static bool False_IVD(const string& s)
{ cerr << "Is_Voronoi_Diagram: " << s; return false; }

bool Is_Voronoi_Diagram(const GRAPH<CIRCLE,POINT>& G, 
                        delaunay_voronoi_kind kind)
{ if ( G.number_of_nodes() == 0 ) return true;
  node v,w; edge e;
  
  if ( !Is_CCW_Ordered_Plane_Map(G) )
     return False_IVD("G is not CCW-ordered plane map");
  
  forall_edges(e,G)
  { if ( G.outdeg(target(e)) != 1 )
    { // e does not end at a vertex at infinity
      if ( G[e] != G[G.face_cycle_succ(e)] )
        return False_IVD("inconsistent site labels");
    }
    if ( G[e] == G[G.reversal(e)] )
        return False_IVD("same site on both sides");
  }

  forall_nodes(v,G)
  { CIRCLE C = G[v];
    if ( C.is_degenerate() ) 
    { // vertex at infinity 
     
      if ( G.outdeg(v) != 1 )
        return  False_IVD("degree of vertex at inf");

      edge e = G.first_adj_edge(v); edge r = G.reversal(e);
      POINT a = C.point1(); POINT c = C.point3();
      if ( (kind == NEAREST && (c != G[e] || a != G[r])) ||
          (kind == FURTHEST && (a!= G[e] || c != G[r])))
        return False_IVD("vertex at inf: wrong edge labels");
    }
    else
    { // finite vertex

      if ( G.outdeg(v) < 3 )
        return False_IVD("degree of proper vertex");
      forall_adj_edges(e,v)
      { if ( !C.contains(G[e]) )
          return False_IVD("label of proper vertex");
      }
      for (int i = 1; i <= 3; i++)
      { POINT a;
        switch (i) {
        case 1: a = C.point1();
                break;
        case 2: a = C.point2();
                break;
        case 3: a = C.point3();
                break;
        }

        bool found_a = false;
        forall_adj_edges(e,v)
          if ( a == G[e] ) found_a = true;
        if ( !found_a ) 
          return False_IVD("wrong cycle");
      }
      forall_adj_edges(e,v)
      { w = G.target(e);
        if ( G.outdeg(w) == 1 ) continue;
        if ( C.center() == G[w].center() ) 
          return False_IVD("zero lenght edge");
        int orient = orientation(C.center(),G[w].center(),G[e]);
        if ((kind == NEAREST  && orient <= 0) || 
            (kind == FURTHEST && orient >= 0))
        return False_IVD("orientation");
      }
    }
  }

  if ( Is_Connected(G) )
    { 
      cmp_edges_by_angle<GRAPH<CIRCLE,POINT> > cmp(G);

      node v; 

      forall_nodes(v,G) 
        if ( G.outdeg(v) == 1 ) break;

      edge_array<bool> considered(G,false);
      list<edge> rays;

      edge e = G.first_adj_edge(v);
      do
      { rays.push(e);

        list<edge> D;
        do 
        { considered[e] = true;
          D.append(e);
          e = G.face_cycle_succ(e);
        } while ( G.outdeg(source(e)) != 1);

        if ( !Is_C_Increasing(D,cmp) ) 
          return False_IVD(": wrong order");

      } while ( G.source(e) != v);

      if ( !Is_C_Nondecreasing(rays,cmp) ) 
         return False_IVD("wrong order, rays");

      forall_edges(e,G)
      { if ( !considered[e] )
        { edge e0 = e;
          do 
          { considered[e] = true;
            if ( G.outdeg(target(e)) == 1 ) 
              return False_IVD("unexpected vertex of degree one");
            e = G.face_cycle_succ(e);
          } while ( e != e0);
          if ( !Is_CCW_Convex_Face_Cycle(G,e) )  
            return False_IVD("wrong order");
        }
      }
 }
  else 
    { 
      forall_nodes(v,G) 
        if ( G.outdeg(v) > 1 ) return False_IVD("degree larger than 1");

      d_array<POINT,int> count(0);
      int n_dual = 0;
      edge e = G.first_edge();
      LINE l(G[e],G[G.reversal(e)]);
      forall_edges(e,G)
      { if ( !l.contains(G[e]) )
          return False_IVD("non-collinear sites");
        int& pc = count[G[e]];
        if (pc == 0) n_dual++;
        pc++;
        if (pc == 3) 
          return False_IVD(": site mentioned thrice");
      }
      if ( n_dual != (G.number_of_edges()/2 + 1) )
          return False_IVD(": two many sites");
 }

  return true;
}


LEDA_END_NAMESPACE

