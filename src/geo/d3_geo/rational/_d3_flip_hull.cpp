/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_flip_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_hull.h>

LEDA_BEGIN_NAMESPACE

enum { BLUE, RED, BLACK };

inline bool equal_xy(const d3_rat_point& p, const d3_rat_point& q)
{ return d3_rat_point::cmp_x(p,q) == 0 && d3_rat_point::cmp_y(p,q) == 0; }

static edge TRIANGULATE(GRAPH<d3_rat_point,int>& G, list<node>& L, int orient)
{ 
  if (L.empty()) return nil;

  node          last_v = L.pop();
  d3_rat_point  last_p = G[last_v];

  while (!L.empty() && equal_xy(last_p,G[L.head()])) 
  { node v = L.pop();
    G.del_node(v);
   }

  if (!L.empty())
  { node v = L.pop();
    last_p = G[v];
    edge x = G.new_edge(last_v,v,0);
    edge y = G.new_edge(v,last_v,0);
    G[x] = G[y] = BLACK;
    G.set_reversal(x,y);
    last_v = v;
   }

  // scan remaining points

  node v;
  forall(v,L) 
  { 
    d3_rat_point p = G[v];
    
    if (equal_xy(p,last_p)) 
    { G.del_node(v);
      continue; 
     }


    // walk up to upper tangent
    edge e = G.last_edge();
    do e = G.pred_face_edge(e); 
    while (orientation_xy(p,G[source(e)],G[target(e)]) == orient);

    // walk down to lower tangent and triangulate
    do { edge succ_e = G.succ_face_edge(e);
         edge x = G.new_edge(succ_e,v,0,leda::behind);
         edge y = G.new_edge(v,source(succ_e),0);
         G[x] = G[y] = RED;
         G.set_reversal(x,y);
         e = succ_e;
       } while (orientation_xy(p,G[source(e)],G[target(e)]) == orient);

    last_p = p;
   }

   edge e_hull = G.last_edge();
   edge e = e_hull;
   do { edge r = G.reversal(e);
        G[e] = G[r] = BLUE;
        e = G.face_cycle_succ(e);
   } while (e != e_hull);

  return G.last_edge();
}


static void init_hull(list<d3_rat_point>& L, GRAPH<d3_rat_point,int>& H, 
                                             bool filter = false)
{ 
   L.sort();

   d3_rat_point min_z;
   d3_rat_point max_z;

   if (filter)
   { list<node> V;
     d3_rat_point p;
     forall(p,L) V.append(H.new_node(p));
     TRIANGULATE(H,V,+1);
     min_z = H[H.first_node()];
     max_z = min_z;
     list<d3_rat_point> L1;
     edge e_hull = H.last_edge();
     edge e = e_hull;
     do { d3_rat_point p = H[source(e)];
          if (p.ZD()*min_z.WD() < min_z.ZD()*p.WD()) min_z = p;
          if (p.ZD()*max_z.WD() > max_z.ZD()*p.WD()) max_z = p;
          e = H.face_cycle_succ(e);
     } while (e != e_hull);
   }

   H.clear();
   list<node> V1;
   list<node> V2;

   d3_rat_point p;
   forall(p,L)
   { if (!filter || p.ZD()*max_z.WD() <= max_z.ZD()*p.WD()) 
           V1.append(H.new_node(p.translate(0,0,-40,1)));
     if (!filter || p.ZD()*min_z.WD() >= min_z.ZD()*p.WD()) 
           V2.push(H.new_node(p));
    }

/*
   cout << endl;
   cout << string("filter: %.2f sec",used_time(T)) << endl; 
   cout << "|V1| = " << V1.length() << endl; 
   cout << "|V2| = " << V2.length() << endl; 
*/

   edge h_edge1 = TRIANGULATE(H,V1,+1);
   edge h_edge2 = TRIANGULATE(H,V2,-1);

   edge e2 = h_edge2;
   do { if (equal_xy(H[source(e2)],H[source(h_edge1)])) break;
        e2 = H.face_cycle_succ(e2);
   } while (e2 != h_edge2);


   edge e1 = h_edge1;

   do { edge e11 = H.face_cycle_succ(e1);
        edge e22 = H.face_cycle_pred(e2);
        edge x = H.new_edge(e1,source(e2),0,leda::behind);
        edge y = H.new_edge(e2,source(e1),0,leda::behind);
        H.set_reversal(x,y);
        H[x] = H[y] = BLUE;
        e1 = e11;
        e2 = e22;
   } while (source(e1) != source(h_edge1));
}


static int FLIPPING(GRAPH<d3_rat_point,int>& G)
{
  if (G.number_of_nodes() <= 3) return 0;

  int flip_count = 0;

  list<edge> S;
  edge e;
  forall_edges(e,G)
     if (G[e] == RED) S.append(e);

  while ( !S.empty() )
  { 
    edge e = S.pop();
    edge r = G.reversal(e);

    if (G[e] == BLUE) continue;

    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    d3_rat_point a = G[source(e1)];
    d3_rat_point b = G[target(e1)];
    d3_rat_point c = G[source(e3)];
    d3_rat_point d = G[target(e3)];


    if (orientation(a,b,c,d) <= 0) 
      G[e] = G[r] = BLACK;
    else
      { G[e] = G[r] = RED;
        int orient_bda = orientation_xy(b,d,a);
        int orient_bdc = orientation_xy(b,d,c);
        if (orient_bda != orient_bdc
        && (orient_bda != 0 || G.outdeg(source(e1)) == 4 || G[e1] == BLUE)
        && (orient_bdc != 0 || G.outdeg(source(e3)) == 4 || G[e3] == BLUE))
          { 
            edge e2 = G.face_cycle_succ(e1);
            edge e4 = G.face_cycle_succ(e3);
      
            S.push(e1); 
            S.push(e2); 
            S.push(e3); 
            S.push(e4); 
        
            // flip
            G.move_edge(e,e2,source(e4));
            G.move_edge(r,e4,source(e2));
            if ((orient_bda==0 && G[e1]==BLUE)||(orient_bdc==0 && G[e3]==BLUE))
               G[e] = G[r] = BLUE;
            else
               G[e] = G[r] = BLACK;
            flip_count++;
          }
      }

  }

  return flip_count;
}

static void simplify_face(GRAPH<d3_rat_point,int>& G, node v)
{ 
  // flip edges adjacent to v until no flips possible anymore or degree(v)
  // == 3 appends all flipped edges to E 

  if (G.outdeg(v) == 3) return;

  // e1,e2,e3 :  three consecutive edges incident to v
  edge e1 = G.first_adj_edge(v);
  edge e2 = G.cyclic_adj_succ(e1);
  edge e3 = G.cyclic_adj_succ(e2);

  // count : number of traversed edges since last flip
  int count = 0; 

  while (count++ < G.outdeg(v) && G.outdeg(v) > 3)
  { int orient1 = orientation_xy(G[target(e1)],G[target(e3)],G[source(e2)]);
    int orient2 = orientation_xy(G[target(e1)],G[target(e3)],G[target(e2)]);
    if (orient1 != orient2 && orient2 != 0)
      { edge r2 = G.reversal(e2);
        G.move_edge(e2,G.reversal(e1),target(e3),leda::before);
        G.move_edge(r2,G.reversal(e3),target(e1));
        G[e2] = G[r2] = RED;
        count = 0;
       }
    else 
       e1 = e2;

    e2 = e3;
    e3 = G.cyclic_adj_succ(e2);
  }
}


static int CLIPPING(GRAPH<d3_rat_point,int>& G) 
{ 
  node_list L;

  node_array<int> rdegree(G,0); 

  edge e;
  forall_edges(e,G)
  { if (G[e] != RED) continue;
    node v = source(e);
    if (++rdegree[v] == 3) L.append(v);
   }

  int clip_count = 0;

  node v;
  forall(v,L)
  { if (rdegree[v] != G.outdeg(v)) continue;

    forall_adj_edges(e,v)
    { edge x = G.face_cycle_succ(e);
      if (G[x] != BLUE) G[x] = G[G.reversal(x)] = RED;
     }

    if (G.outdeg(v) > 3) simplify_face(G,v);
    L.del(v);
    G.del_node(v);
    clip_count++;
  }


  forall(v,L)
  { 
    int deg  = G.outdeg(v);
    int rdeg = 0;
    int bdeg = 0;

    list<edge> E;
    edge e;
    forall_adj_edges(e,v)
    { if (G[e] == BLUE) bdeg++;
      if (G[e] == RED) rdeg++;
      edge x = G.face_cycle_succ(e);
      if (G[x] != BLUE) E.append(x);
     }

    if (bdeg > 0) continue;

    if (G.outdeg(v) > 3) simplify_face(G,v);

    if (rdeg == deg)
    { G.del_node(v);
      clip_count++;
     }
    else
    { edge e1 = G.first_adj_edge(v);
      edge e2 = G.cyclic_adj_succ(e1);
      edge e3 = G.cyclic_adj_succ(e2);
      int orient = orientation(G[target(e1)],G[target(e2)],G[target(e3)],G[v]);
      if (orient <= 0)
        { G.del_node(v);
          clip_count++;
         }
      else
        { edge e;
          forall_adj_edges(e,v) E.append(e);
         }
     }
   
    forall(e,E) G[e] = G[G.reversal(e)] = RED;
   }

  return clip_count;
}


void D3_HULL_FLIP(const list<d3_rat_point>& L0, GRAPH<d3_rat_point,int>& H, bool
filter)
{ 
  list<d3_rat_point> L = L0;
  init_hull(L,H,filter);
  int i = 0;
  int f = 0;
  int c = 0;

  do { f = FLIPPING(H);
       c = CLIPPING(H);
       if (i++ > 500) 
       { LEDA_EXCEPTION(0,"D3_HULL_FLIP: flip/clip loop.");
         break;
        }
  } while (f > 0 || c > 0);
}

LEDA_END_NAMESPACE
