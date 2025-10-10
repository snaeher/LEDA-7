/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  circles.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/geo/geo_global_enums.h>
#include <LEDA/geo/geo_alg.h>

LEDA_BEGIN_NAMESPACE

static CIRCLE trivial_circle(POINT p)
{ return CIRCLE(p,p,p); }


static CIRCLE circle_with_diameter(POINT p, POINT q)
{ return CIRCLE(p,q,p.rotate90(center(p,q))); }

/*
static int compare_size(const CIRCLE& c1, const CIRCLE& c2)
{ POINT p0 = c1.point1();
  POINT p1 = c1.center();
  POINT q0 = c2.point1();
  POINT q1 = c2.center();
  return compare(p0.sqr_dist(p1), q0.sqr_dist(q1));
}
*/

static void search(GRAPH<POINT,int>& DT, edge e, POINT A, POINT B, 
                                                 edge_array<bool>& visited,
                                                 list<CIRCLE>& L)
{ 
  edge x = DT.face_cycle_succ(e);
  edge y = DT.face_cycle_succ(x);

  CIRCLE C(DT[source(x)],DT[target(x)],DT[target(y)]);

  if (orientation(A,B,C.center()) > 0) // center out of hull
  { 
    visited[e] = visited[x] = visited[y] = true;

    edge xr = DT.reversal(x);
    edge yr = DT.reversal(y);

    list<CIRCLE> L1,L2;
    if (DT[xr] != HULL_EDGE) search(DT,xr,A,B,visited,L1);
    if (DT[yr] != HULL_EDGE) search(DT,yr,A,B,visited,L2);
  
    if (L1.empty() && L2.empty())
    { POINT a = DT[source(x)];
      POINT b = DT[target(x)];
      POINT c = DT[target(y)];
      POINT d = b.reflect(A,B);
      L.append(CIRCLE(a,b,d));
      L.append(CIRCLE(b,c,d));
     }
    else
     { L.conc(L1);
       L.conc(L2);
      }
   }
}




void compute_empty_circles(const list<POINT>& L, list<CIRCLE>& CL)
{
  GRAPH<POINT,int> DT; 
  DELAUNAY_TRIANG(L,DT);

  CL.clear();

  int n = DT.number_of_nodes();

  if (n == 0) return;

  if (n == 1) { 
    node v = DT.first_node(); 
    CL.append(trivial_circle(DT[v]));
    return;
  }

  bool one_dimensional = false;

  node v;
  forall_nodes(v,DT)
    if (DT.outdeg(v) == 1) 
    { one_dimensional = true;
      break;
     }


  edge_array<bool> visited(DT,false);

  // compute hull edge

  edge e;
  forall_edges(e,DT)
   if (DT[e] == HULL_EDGE) break;
/*
   { edge e1 = DT.face_cycle_succ(e);
     if (DT[e1] == HULL_EDGE
         && orientation(DT[source(e)],DT[target(e)],DT[target(e1)])<=0) break;
    }
*/

  edge hull_edge = e;

  // for all hull edges

  do { edge  r = DT.reversal(e);
       visited[e] = true;
       POINT a = DT[source(e)];
       POINT b = DT[target(e)];
       if (one_dimensional)
         CL.append(circle_with_diameter(a,b));
       else
         search(DT,r,a,b,visited,CL);
       e = DT.face_cycle_succ(e);
     } while (e != hull_edge);


  forall_edges(e,DT)
  { if (visited[e]) continue;
    edge e1 = e;
    edge e2 = DT.face_cycle_succ(e1);
    edge e3 = DT.face_cycle_succ(e2);
    visited[e1] = visited[e2] = visited[e3] = true;
    CL.append(CIRCLE(DT[source(e1)], DT[source(e2)], DT[source(e3)]));
   }
}


CIRCLE  LARGEST_EMPTY_CIRCLE(const list<POINT>& L)
{
  CIRCLE trivial_circle;

  if (L.empty()) return trivial_circle;

  list<CIRCLE> CL;
  compute_empty_circles(L,CL);

  CIRCLE Cmax = CL.pop();
  COORD  max_r = Cmax.center().sqr_dist(Cmax.point1());

  CIRCLE C;
  forall(C,CL)
  { COORD r = C.center().sqr_dist(C.point1());
    if (r > max_r)
    { Cmax = C;
      max_r = r;
    }
  }
  return Cmax;
}




void  compute_enclosing_circles(const list<POINT>& L, list<CIRCLE>& CL)
{
  GRAPH<POINT,int> FDT; 
  F_DELAUNAY_TRIANG(L,FDT);

  CL.clear();

  int n = FDT.number_of_nodes();

  if (n == 0) return;

  if (n == 1) {
    node v = FDT.first_node(); 
    CL.append(trivial_circle(FDT[v]));
  }

  if (n == 2) { 
    node v = FDT.first_node(); 
    node w = FDT.last_node(); 
    CL.append(circle_with_diameter(FDT[v],FDT[w]));
   }

  // compute all enclosing circles passing though 3 points

  edge_array<bool> visited(FDT,false);

  edge e1;
  forall_edges(e1,FDT)
  { if (visited[e1] || FDT[e1] == HULL_EDGE) continue;

    edge e2 = FDT.face_cycle_succ(e1);
    edge e3 = FDT.face_cycle_succ(e2);

    visited[e1] = visited[e2] = visited[e3] = true;

    POINT a = FDT[source(e1)];
    POINT b = FDT[source(e2)];
    POINT c = FDT[source(e3)];

    CL.append(CIRCLE(a,b,c));
  }

  // compute all enclosing circles with diameter e (forall edges e of FDT)

  visited.init(FDT,false);

  forall_edges(e1,FDT)
  { if (visited[e1] || FDT[e1] == HULL_EDGE) continue;

    edge r1 = FDT.reversal(e1);

    edge e2 = FDT.face_cycle_succ(e1);
    edge r2 = FDT.face_cycle_succ(r1);

    visited[e1] = true;
    visited[r1] = true;

    POINT a = FDT[source(e1)];
    POINT b = FDT[target(e1)];
    POINT c = FDT[target(e2)];
    POINT d = FDT[target(r2)];

    CIRCLE C = circle_with_diameter(a,b); // diameter e1

    if (C.inside(c) && C.inside(d)) CL.append(C);
  }


}



CIRCLE  SMALLEST_ENCLOSING_CIRCLE(const list<POINT>& L)
{
  CIRCLE Cmin;
  COORD  min_r;

  list<CIRCLE> CL; 
  compute_enclosing_circles(L,CL);

  if (CL.empty()) return Cmin;

  Cmin  = CL.pop();
  min_r = Cmin.center().sqr_dist(Cmin.point1());

  CIRCLE C;
  forall(C,CL)
  { COORD r = C.center().sqr_dist(C.point1());
    if (r < min_r)
    { Cmin = C;
      min_r = r;
    }
  }

  return Cmin;
}




void ALL_EMPTY_CIRCLES(const list<POINT>& L, list<CIRCLE>& CL)
{
  GRAPH<POINT,int> DT; 
  DELAUNAY_TRIANG(L,DT);

  CL.clear();

  int n = DT.number_of_nodes();

  if (n < 3) return;

  bool one_dimensional = false;

  node v;
  forall_nodes(v,DT)
    if (DT.outdeg(v) == 1) 
    { one_dimensional = true;
      break;
     }

  if (one_dimensional) return;

  edge_array<bool> visited(DT,false);

  edge e;
  forall_edges(e,DT)
  { if (visited[e] || DT[e] == HULL_EDGE) continue;
    edge e1 = e;
    edge e2 = DT.face_cycle_succ(e1);
    edge e3 = DT.face_cycle_succ(e2);
    visited[e1] = visited[e2] = visited[e3] = true;
    CL.append(CIRCLE(DT[source(e1)], DT[source(e2)], DT[source(e3)]));
   }
}


void ALL_ENCLOSING_CIRCLES(const list<POINT>& L, list<CIRCLE>& CL)
{
  GRAPH<POINT,int> FDT; 
  F_DELAUNAY_TRIANG(L,FDT);

  CL.clear();

  int n = FDT.number_of_nodes();

  if (n < 3) return;

  // compute all enclosing circles passing though 3 points

  edge_array<bool> visited(FDT,false);

  edge e1;
  forall_edges(e1,FDT)
  { if (visited[e1] || FDT[e1] == HULL_EDGE) continue;
    edge e2 = FDT.face_cycle_succ(e1);
    edge e3 = FDT.face_cycle_succ(e2);

    visited[e1] = visited[e2] = visited[e3] = true;

    POINT a = FDT[source(e1)];
    POINT b = FDT[source(e2)];
    POINT c = FDT[source(e3)];
    CL.append(CIRCLE(a,b,c));
  }
}


LEDA_END_NAMESPACE
