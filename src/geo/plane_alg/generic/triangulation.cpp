/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  triangulation.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE


inline int left_bend(const POINT& p, 
                     const GRAPH<POINT,int>& G, 
                     const edge& e)   
{ return (orientation(p,G[source(e)],G[target(e)]) > 0); }

edge TRIANGULATE_POINTS(const list<POINT>& L0, 
                        GRAPH<POINT,int>& G)
{ 
  G.clear();
  if (L0.empty()) return nil;

  list<POINT> L = L0;

  L.sort(); 

  if ( L.empty() ) return nil; 

  // initizialize G with a single edge starting at the first point

  POINT last_p = L.pop();            // last visited point
  node  last_v = G.new_node(last_p); // last inserted node

  while (!L.empty() && last_p == L.head()) L.pop();

  if (!L.empty())
  { last_p = L.pop();
    node v = G.new_node(last_p);
    edge x = G.new_edge(last_v,v,0);
    edge y = G.new_edge(v,last_v,0);
    G.set_reversal(x,y);
    last_v = v;
   }

  
  POINT p;
  forall(p,L) 
  { if (p == last_p) continue; 

    edge e =  G.last_adj_edge(last_v);

    last_v = G.new_node(p);
    last_p = p;

    // walk up to upper tangent
    do e = G.face_cycle_pred(e); while (left_bend(p,G,e));
    // now e = e_up

    // walk down to lower tangent and triangulate
    do { edge succ_e = G.face_cycle_succ(e);
         edge x = G.new_edge(succ_e,last_v,leda::behind,0);
         edge y = G.new_edge(last_v,source(succ_e),0);
         G.set_reversal(x,y);
         e = succ_e;
       } while (left_bend(p,G,e));
  }

  
  edge hull_dart = G.last_edge();

  if (hull_dart)
  { edge e = hull_dart;
    do { G[e] = HULL_DART;
         e = G.face_cycle_succ(e);
       } while (e != hull_dart); 
  }

  return hull_dart;


}


// point location in triangulations (sn. September 2010)


inline bool is_hull_edge(const graph& G, const node_array<POINT>& pos, edge x)
{ edge y = G.face_cycle_succ(x);
  POINT a = pos[source(x)];
  POINT b = pos[target(x)];
  POINT c = pos[target(y)];
  return orientation(a,b,c) <= 0;
}

inline bool is_hull_edge1(const graph& G, const node_array<POINT>& pos, edge e)
{ edge x = G.cyclic_adj_succ(e);
  edge y = G.face_cycle_succ(e);
  if (target(x) != target(y)) return true; // no triangle
  return orientation(pos[source(e)], pos[target(e)], pos[target(x)]) <= 0;
}

static void check_locate(const graph& G, const node_array<POINT>& pos, 
                                         POINT p, edge x)
{
/*
  if (G.number_of_edges() == 0) 
  { assert(x == nil);
    return;
   }

  assert(x && graph_of(x) == &G);
*/

  edge y = G.face_cycle_succ(x);

  POINT a = pos[source(x)];
  POINT b = pos[target(x)];
  POINT c = pos[target(y)];

  int orient = orientation(a,b,p);

  assert(orient >= 0);

  SEGMENT seg(a,b);

  if (orientation(a,b,c) <= 0)
    { //hull edge
      assert (orient > 0  || seg.contains(p) ||
              (y == G.reversal(x) && side_of_halfspace(a,b,p) > 0));
     }
  else
    { // non-hull edge
      if (orient == 0)
       { assert(seg.contains(p));
         edge r = G.reversal(x);
         edge z = G.face_cycle_succ(r);
         POINT d = pos[target(z)];
         assert(orientation(b,a,d) > 0); // !is_hull_edge(G,pos,r)
        }
      else
      { //orient > 0
        assert(orientation(b,c,p) > 0);
        assert(orientation(c,a,p) > 0);
       }
    }
}


bool CHECK_LOCATE_IN_TRIANGULATION(const graph& G, 
                                   const node_array<POINT>& pos, 
                                   POINT p, edge x)
{
  if (G.number_of_edges() == 0)
  { if (x)
    { LEDA_EXCEPTION(1,"check locate: non-null edge for empty graph");
      return false;
     }
    return true;
   }

  if (x == 0)
  { LEDA_EXCEPTION(1,"check locate: null edge for non-empty graph");
    return false;
   }


  if (graph_of(x) != &G)
  { LEDA_EXCEPTION(1,"check locate: edge does not belong to garph");
    return false;
   }

  edge y = G.face_cycle_succ(x);

  POINT a = pos[source(x)];
  POINT b = pos[target(x)];
  POINT c = pos[target(y)];

  int orient = orientation(a,b,p);

  if (orient < 0)
  { LEDA_EXCEPTION(1,"check locate: orientation(e,p) < 0");
    return false;
   }

  SEGMENT seg(a,b);

  if (orientation(a,b,c) <= 0)
    { //hull edge
      bool ok = (orient > 0  || seg.contains(p) ||
               (y == G.reversal(x) && side_of_halfspace(a,b,p) > 0));
      if (!ok)
      { LEDA_EXCEPTION(1,"check locate: wrong hull edge");
        return false;
       }
     }
  else
    { // non-hull edge
      if (orient == 0)
       { if (!seg.contains(p))
         { LEDA_EXCEPTION(1,"check locate: edge should contain p");
           return false;
          }
         edge r = G.reversal(x);
         edge z = G.face_cycle_succ(r);
         POINT d = pos[target(z)];
         if(orientation(b,a,d) <= 0) // hull_edge
         { LEDA_EXCEPTION(1,"check locate: reversal must not be hull edge");
           return false;
         }
       }
      else
      { // orient > 0
        if ( orientation(b,c,p) <= 0 || orientation(c,a,p) <= 0)
        { LEDA_EXCEPTION(1,"check locate: triangle should contain p");
          return false;
        }
      }
    }

  return true;
}
  
  

//------------------------------------------------------------------------------


edge LOCATE_IN_TRIANGULATION(const graph& G, const node_array<POINT>& pos, 
                                             POINT p, edge e) 
{ 
  const bool check = false;

  if (G.number_of_edges() == 0) return nil;

  if (e == nil) e = G.first_edge();

  assert(graph_of(e) == &G);

  if (is_hull_edge(G,pos,e)) e = G.reversal(e);  

  int dim = (is_hull_edge(G,pos,e)) ? 1 : 2;

  if (dim == 1)
  { int orient = orientation(pos[source(e)],pos[target(e)],p);
    if (orient != 0)
    { if (orient < 0) e = G.reversal(e);
      if (check) check_locate(G,pos,p,e);
      return e;
    }

    // p is colinear with the points in S 

    if (side_of_halfspace(pos[source(e)],pos[target(e)],p) < 0)
      e = G.reversal(e);

    // we walk in the direction of e, we know that p lies in half-space(e) 
         
    edge e1 = G.face_cycle_succ(e);
    while ( e1 != G.reversal(e) && 
            side_of_halfspace(pos[source(e1)],pos[target(e1)],p) >= 0)
    { e = e1;  
      e1 = G.face_cycle_succ(e); 
    }
         
    if (check) check_locate(G,pos,p,e);

    return e;
  }


  // dim = 2

  POINT a = pos[source(e)];
  POINT b = pos[target(e)];

  if (p == a) return G.reversal(e);

  int orient = orientation(a,b,p);

  if (orient == 0) 
  { e = G.face_cycle_pred(e);
    a = pos[source(e)];
    b = pos[target(e)];
    orient = orientation(a,b,p);
  }

  if (orient < 0) 
  { e = G.reversal(e);
    swap(a,b);
    orient = -orient;
   }

  assert(orient > 0);
   
  SEGMENT s(a,p);

/*
  int ss = 50;

  bool hull = false;

  while (!hull && orient > 0)
  { int steps = --ss/10;
    if (steps < 1) break;
    for(int i=0; i<steps; i++)
    { edge e_next1 = G.cyclic_adj_succ(e);
      node u = target(e_next1);
      edge e_next2 = G.reversal(G.face_cycle_succ(e));
      node v = source(e_next2);
  
      if (u != v) { hull = true; break; }

      e = (s.orientation(pos[u]) < 0) ? e_next1 : e_next2;
     }
    orient = orientation(pos[source(e)],pos[target(e)],p);
    steps--;
  }

  if (orient == 0) return e;

  a = pos[source(e)];
  b = pos[target(e)];

  if (orient < 0)
  { e = G.reversal(e);  
    orient = -orient;
    swap(a,b);
   }

  s = SEGMENT(a,p);

  assert(orient > 0);
*/

  while (orient > 0)
  { 
    //assert(a == pos[source(e)]);
    //assert(b == pos[target(e)]);

    edge e_next = G.cyclic_adj_succ(e);

    POINT c = pos[target(e_next)];

    if (orientation(a,b,c) <= 0) break; // e is hull_edge, return

    int d = s.orientation(c);

    if (d >= 0) 
    { e_next = G.reversal(G.face_cycle_succ(e));
      a = c;
     }
    else 
      b = c;

    orient = orientation(a,b,p);

    if (orient >= 0)
        e = e_next;
    else
      if (d == 0) 
      { edge x = G.cyclic_adj_succ(e);
        if (orientation(pos[source(x)],pos[target(x)],p) == 0) e = x;
       }
  }

  if (check) check_locate(G,pos,p,e);

  return e;

}

edge LOCATE_IN_TRIANGULATION(const GRAPH<POINT,int>& G, POINT p, edge start) 
{ return LOCATE_IN_TRIANGULATION(G,G.node_data(),p,start); }


edge LOCATE_IN_TRIANGULATION(const GRAPH<POINT,SEGMENT>& G, POINT p,edge start) 
{ return LOCATE_IN_TRIANGULATION(G,G.node_data(),p,start); }



LEDA_END_NAMESPACE

