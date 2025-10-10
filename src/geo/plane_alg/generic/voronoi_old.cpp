/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  voronoi_old.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// VORONOI DIAGRAMS
//
// DELAUNAY_TO_VORONOI(DT,VD)  : compute VD from Delaunay Triangulation DT
// VORONOI(L,VD)   : compute VD for a list L of points
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

void DELAUNAY_TO_VORONOI(const GRAPH<POINT,int>& DT, GRAPH<CIRCLE,POINT>& VD)
{
  VD.clear();

  if (DT.number_of_nodes() < 2) return;

  edge_array<node> vnode(DT,nil);

  // create Voronoi nodes for outer face

  edge e;
  forall_edges(e,DT)
   if (DT[e] == HULL_EDGE)
   { edge e1 = DT.face_cycle_succ(e);
     if (DT[e1] == HULL_EDGE
         && orientation(DT[source(e)],DT[target(e)],DT[target(e1)])<=0) break;
    }


  edge hull_edge = e;

  POINT a = DT[source(e)];
  do { POINT b = DT[target(e)];
       vnode[e] =  VD.new_node(CIRCLE(a,center(a,b),b));
       e = DT.face_cycle_succ(e);
       a = b;
     } while (e != hull_edge);


  // and for all other faces

  forall_edges(e,DT)
  { 
    if (vnode[e]) continue;

    edge  x = DT.face_cycle_succ(e);
    POINT a = DT[source(e)];
    POINT b = DT[target(e)];
    POINT c = DT[target(x)];
    node  v = VD.new_node(CIRCLE(a,b,c));

    vnode[e] = v;

    do { vnode[x] = v;
         x = DT.face_cycle_succ(x);
        } while( x != e);

   }



  edge_array<edge> vedge(DT,nil);

  // construct Voronoi edges for outer face

  e = hull_edge;
  do { edge r = DT.reversal(e); 
       POINT p = DT[target(e)];
       vedge[e] = VD.new_edge(vnode[e],vnode[r],p);
       e = DT.cyclic_adj_pred(r);
     } while ( e != hull_edge);


  // and for all other faces

  forall_edges(e,DT)
  { node v = vnode[e]; 
    if (VD.outdeg(v) > 0) continue;
    edge x = e;
    do { edge  r = DT.reversal(x); 
         POINT p = DT[target(x)];
         vedge[x] = VD.new_edge(v,vnode[r],p);
         x = DT.cyclic_adj_pred(r);
       } while ( x != e);
   }

  // assign reversal edges

  forall_edges(e,DT)
  { edge r = DT.reversal(e);
    VD.set_reversal(vedge[e],vedge[r]);
   }

}



void F_DELAUNAY_TO_VORONOI(const GRAPH<POINT,int>& DT, GRAPH<CIRCLE,POINT>& VD)
{
  VD.clear();

  if (DT.number_of_nodes() < 2) return;

  edge_array<node> vnode(DT,nil);

  // create Voronoi nodes for outer face

  edge e;
  forall_edges(e,DT)
   if (DT[e] == HULL_EDGE)
   { edge e1 = DT.face_cycle_succ(e);
     if (DT[e1] == HULL_EDGE
         && orientation(DT[source(e)],DT[target(e)],DT[target(e1)])<=0) break;
    }

  edge hull_edge = e;

  POINT a = DT[source(e)];

  e = hull_edge;
  do { POINT b = DT[target(e)];
       vnode[e] =  VD.new_node(CIRCLE(b,center(a,b),a));
       a = b;
       e = DT.face_cycle_succ(e);
     } while ( e != hull_edge);


  // for all other faces

  forall_edges(e,DT)
  { 
    if (vnode[e]) continue;

    edge  x = DT.face_cycle_succ(e);
    POINT a = DT[source(e)];
    POINT b = DT[target(e)];
    POINT c = DT[target(x)];
    node  v = VD.new_node(CIRCLE(a,b,c));

    vnode[e] = v;

    do { vnode[x] = v;
         x = DT.face_cycle_succ(x);
        } while( x != e);

   }


  edge_array<edge> vedge(DT,nil);

  // construct Voronoi edges for outer face

  e = hull_edge;
  do { edge r = DT.reversal(e); 
       POINT p = DT[target(e)];
       vedge[e] = VD.new_edge(vnode[e],vnode[r],p);
       e = DT.cyclic_adj_pred(r);
     } while ( e != hull_edge);


  // for all other faces

  forall_edges(e,DT)
  { node v = vnode[e]; 
    if (VD.outdeg(v) == 0)
    { edge x = e;
      do { edge r = DT.reversal(x); 
           POINT p = DT[target(x)];
           vedge[x] = VD.new_edge(v,vnode[r],p);
           x = DT.cyclic_adj_pred(r);
         } while ( x != e);
     }
   }

  // assign reversal edges

  forall_edges(e,DT)
  { edge r = DT.reversal(e);
    VD.set_reversal(vedge[e],vedge[r]);
   }

}




void VORONOI(const list<POINT>& L, GRAPH<CIRCLE,POINT>& VD)
{ GRAPH<POINT,int> DT;
  DELAUNAY_DIAGRAM(L,DT);
  DELAUNAY_TO_VORONOI(DT,VD);
}


void F_VORONOI(const list<POINT>& L, GRAPH<CIRCLE,POINT>& VD)
{ GRAPH<POINT,int> DT;
  F_DELAUNAY_DIAGRAM(L,DT);
  F_DELAUNAY_TO_VORONOI(DT,VD);
}


LEDA_END_NAMESPACE
