/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  voronoi.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



LEDA_BEGIN_NAMESPACE

void DELAUNAY_TO_VORONOI(const GRAPH<POINT,int>& DD, 
                         GRAPH<CIRCLE,POINT>& VD)
{
  VD.clear();

  if (DD.number_of_nodes() < 2) return;

  // determine a hull dart 
  edge e;
  forall_edges(e,DD) if (DD[e] == HULL_DART) break;

  edge hull_dart = e;
 
  edge_array<node> vnode(DD,nil);

  
  // create Voronoi nodes for outer face

  POINT a = DD[source(e)];
  do { POINT b = DD[target(e)];
       vnode[e] =  VD.new_node(CIRCLE(a,center(a,b),b));
       e = DD.face_cycle_succ(e);
       a = b;
     } while ( e != hull_dart );

  // and for all other faces

  forall_edges(e,DD)
  { 
    if (vnode[e]) continue;

    edge  x = DD.face_cycle_succ(e);
    POINT a = DD[source(e)];
    POINT b = DD[target(e)];
    POINT c = DD[target(x)];
    node  v = VD.new_node(CIRCLE(a,b,c));

    vnode[e] = v;

    do { vnode[x] = v;
         x = DD.face_cycle_succ(x);
       } while( x != e );
  }
 

  
  edge_array<edge> vedge(DD,nil);

  // construct Voronoi darts out of nodes at infinity

  e = hull_dart;
  do { edge r = DD.reversal(e); 
       POINT p = DD[target(e)];   
       vedge[e] = VD.new_edge(vnode[e],vnode[r],p);
       e = DD.cyclic_adj_pred(r); // same as DD.face_cycle_succ(e)
     } while ( e != hull_dart );


  // and out of all other nodes.

  forall_edges(e,DD)
  { node v = vnode[e]; 
    if (VD.outdeg(v) > 0) continue;
    edge x = e;
    do { edge  r = DD.reversal(x); 
         POINT p = DD[target(x)];
         vedge[x] = VD.new_edge(v,vnode[r],p);
         x = DD.cyclic_adj_pred(r);
       } while ( x != e);
  }

  // assign reversal edges

  forall_edges(e,DD)
  { edge r = DD.reversal(e);
    VD.set_reversal(vedge[e],vedge[r]);
  }
 
}


void VORONOI(const list<POINT>& L, GRAPH<CIRCLE,POINT>& VD)
{ GRAPH<POINT,int> DD;
  DELAUNAY_DIAGRAM(L,DD);
  DELAUNAY_TO_VORONOI(DD,VD);
}


void F_DELAUNAY_TO_VORONOI(const GRAPH<POINT,int>& DD, 
                           GRAPH<CIRCLE,POINT>& VD)
{
  VD.clear();

  if (DD.number_of_nodes() < 2) return;

  edge_array<node> vnode(DD,nil);

  // create Voronoi nodes for outer face

  edge e;
  forall_edges(e,DD)
   if (DD[e] == HULL_DART)
   { edge e1 = DD.face_cycle_succ(e);
     if (DD[e1] == HULL_DART
         && orientation(DD[source(e)],DD[target(e)],DD[target(e1)])<=0) break;
    }

  edge hull_edge = e;

  POINT a = DD[source(e)];

  e = hull_edge;
  do { POINT b = DD[target(e)];
       vnode[e] =  VD.new_node(CIRCLE(b,center(a,b),a));
       a = b;
       e = DD.face_cycle_succ(e);
     } while ( e != hull_edge);


  // for all other faces

  forall_edges(e,DD)
  { 
    if (vnode[e]) continue;

    edge  x = DD.face_cycle_succ(e);
    POINT a = DD[source(e)];
    POINT b = DD[target(e)];
    POINT c = DD[target(x)];
    node  v = VD.new_node(CIRCLE(a,b,c));

    vnode[e] = v;

    do { vnode[x] = v;
         x = DD.face_cycle_succ(x);
        } while( x != e);

   }


  edge_array<edge> vedge(DD,nil);

  // construct Voronoi edges for outer face

  e = hull_edge;
  do { edge r = DD.reversal(e); 
       POINT p = DD[target(e)];
       vedge[e] = VD.new_edge(vnode[e],vnode[r],p);
       e = DD.cyclic_adj_pred(r);
     } while ( e != hull_edge);


  // for all other faces

  forall_edges(e,DD)
  { node v = vnode[e]; 
    if (VD.outdeg(v) == 0)
    { edge x = e;
      do { edge r = DD.reversal(x); 
           POINT p = DD[target(x)];
           vedge[x] = VD.new_edge(v,vnode[r],p);
           x = DD.cyclic_adj_pred(r);
         } while ( x != e);
     }
   }

  // assign reversal edges

  forall_edges(e,DD)
  { edge r = DD.reversal(e);
    VD.set_reversal(vedge[e],vedge[r]);
   }

}



void F_VORONOI(const list<POINT>& L, GRAPH<CIRCLE,POINT>& VD)
{ GRAPH<POINT,int> DD;
  F_DELAUNAY_DIAGRAM(L,DD);
  F_DELAUNAY_TO_VORONOI(DD,VD);
}



LEDA_END_NAMESPACE
