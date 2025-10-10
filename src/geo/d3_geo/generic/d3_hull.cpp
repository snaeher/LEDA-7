/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_hull.h>
#include <LEDA/system/assert.h>
#include <LEDA/core/slist.h>

LEDA_BEGIN_NAMESPACE

bool CHECK_HULL(const GRAPH<D3_POINT,int>& H)
{  
   VECTOR c_vec(3);  // zero vector

   int n = 0;

   node v;
   forall_nodes(v,H) 
     if (H.degree(v) > 0) 
     { c_vec += H[v].to_vector();
       n++;
      }

   c_vec = c_vec/n;

   edge e;
   forall_edges(e,H)
    { edge e1 = H.cyclic_adj_succ(e);
      edge e2 = H.cyclic_adj_pred(e);
      D3_POINT a = H[source(e)];
      D3_POINT b = H[target(e)];
      D3_POINT c = H[target(e1)];
      D3_POINT d = H[target(e2)];

      if (orientation(a,b,c,c_vec) > 0)
      { LEDA_EXCEPTION(0,"orientation error");
        return false;
       }

      if (orientation(a,b,c,d) > 0)
      { LEDA_EXCEPTION(0,"local non-convexity");
        return false;
       }

     }

   return true;
}
     


void del_visible_faces(GRAPH<D3_POINT,int>& H, edge& border_e, D3_POINT p,
                                                            int& vis_count)
{ 
  slist<edge> S;
  slist<edge> del_edges;

  vis_count++;

  // initialize S with a visible face cycle

  edge x = border_e;
  do { S.append(x);
       H[x] = vis_count;
       x = H.face_cycle_succ(x);
  } while (x != border_e);

  while (!S.empty())
  { edge e = S.pop();
    edge r = H.reversal(e);
    if (H[r] == vis_count) continue;
    edge r1 = H.face_cycle_succ(r);
    D3_POINT A = H[source(r)];
    D3_POINT B = H[source(r1)];
    D3_POINT C = H[target(r1)];
    if (orientation(A,B,C,p) >= 0)
    { edge x = r;
      do { H[x] = vis_count;
           edge y = H.reversal(x);
           if (H[y] != vis_count)
              S.append(x);
           else 
            { del_edges.append(x);
              del_edges.append(y);
             }
           x = H.face_cycle_succ(x);
       } while (x != r);
     }
    else border_e = e;
   }

   // remove visible edges

  edge e;
  forall(e,del_edges)
  { node x = source(e);
    node y = target(e);
    H.del_edge(e);
    if (H.degree(x) == 0) H.del_node(x);
    if (H.degree(y) == 0) H.del_node(y);
  }
}


void join_coplanar_faces(GRAPH<D3_POINT,int>& H)
{
  list<edge> L;
  edge_array<bool> considered(H,false);

  edge e;
  forall_edges(e,H)
  { if (considered[e]) continue;
    edge r = H.reversal(e);
    edge e1 = H.face_cycle_succ(e);
    edge r1 = H.face_cycle_succ(r);

    D3_POINT A = H[source(e)];
    D3_POINT B = H[source(e1)];
    D3_POINT C = H[target(e1)];
    D3_POINT D = H[target(r1)];

    if (orientation(A,B,C,D) == 0) 
    { L.append(e);
      L.append(r);
      considered[e] = considered[r] = true;
     }
   }

   forall(e,L) H.del_edge(e);
}

void d2_add_point(GRAPH<D3_POINT,int>& H, edge& border_e, const D3_POINT& D, 
                                                       const D3_POINT& p)
{
  node v = H.new_node(p);

  // construct upper tangent
  edge up = border_e;

  while (orientation(p,H[source(up)],H[target(up)],D) <= 0)
     up = H.face_cycle_succ(up); 

  edge x = H.new_edge(up,v,0,leda::behind);
  edge y = H.new_edge(v,source(up));
  H.set_reversal(x,y);


  // construct lower tangent
  edge down = H.face_cycle_pred(border_e);
  while (orientation(p,H[source(down)],H[target(down)],D) <= 0)
     down = H.face_cycle_pred(down);

  down = H.face_cycle_succ(down);

  x = H.new_edge(down,v,0,leda::behind);
  y = H.new_edge(v,source(down));
  H.set_reversal(x,y);

  // remove visible edges
  while (target(down) != v)
  { edge e = down;
    down = H.face_cycle_succ(down);
    node u = source(e);
    H.del_edge(H.reversal(e));
    H.del_edge(e);
    if (H.degree(u) == 0) H.del_node(u);
   }

  border_e = H.first_adj_edge(v);
}




void compute_hull(list<D3_POINT>& L, GRAPH<D3_POINT,int>& H)
{
  int vis_count = 0;

  D3_POINT A = L.pop();
  node a = H.new_node(A);

  if (L.empty()) return;


  D3_POINT B = L.pop();
  while ( !L.empty() && collinear(A,B,L.head()) ) B = L.pop();
  node b = H.new_node(B);

  if (L.empty()) // all points are collinear
  { edge x = H.new_edge(a,b,0);
    edge y = H.new_edge(b,a,0);
    H.set_reversal(x,y);
    return;
   }


  // construct triangle (A,B,C)

  D3_POINT C = L.pop();
  node c = H.new_node(C);


  D3_POINT D = point_on_positive_side(A,B,C);

  H.set_reversal(H.new_edge(a,b,0),H.new_edge(b,a,0));
  H.set_reversal(H.new_edge(b,c,0),H.new_edge(c,b,0));
  H.set_reversal(H.new_edge(c,a,0),H.new_edge(a,c,0));

  edge border_e = H.last_adj_edge(c);

  assert(target(border_e) == a);

  assert(orientation(A,B,C,D) > 0);

  if (L.empty()) return;

  int  dim = 2;

  while (!L.empty())
  {
    D3_POINT p = L.pop();

    if (dim == 2) 
    { int orient = orientation(A,B,C,p);
      if (orient == 0)
      { d2_add_point(H,border_e,D,p);
        continue;
       }
      if (orient < 0) border_e = H.reversal(border_e);
      dim = 3;
    }

    // 3-dimensional case
    // remove all faces visible from p

    del_visible_faces(H,border_e,p,vis_count);

    node v = H.new_node(p);

    // and re-triangulate


    D3_POINT q = p;
    if (!L.empty()) q = L.head(); 

    // node stop = source(border_e);

    edge e = border_e;
    do { edge x = H.new_edge(e,v,0,leda::behind);
         edge y = H.new_edge(v,source(e),0);
         H.set_reversal(x,y);
         if (orientation(p,H[source(e)],H[target(e)],q) > 0) border_e = e;
         e = H.face_cycle_succ(e);
     } while (target(e) != v);


/*
    D3_POINT q = p;
    if (!L.empty()) q = L.head(); 

    bool vflag = orientation(p,H[source(border_e)],H[target(border_e)],q) > 0;
    edge be = H.face_cycle_succ(border_e);


    edge e = be;
    do { bool construct_edge;
         if (orientation(p,H[source(e)],H[target(e)],q) > 0)
         { construct_edge = !vflag;
           vflag = true;
           border_e = e;
          }
         else
         { construct_edge = true;
           vflag = false;
          }

         if (construct_edge)
         { edge x = H.new_edge(e,v,0,leda::behind);
           edge y = H.new_edge(v,source(e),0);
           H.set_reversal(x,y);
          }
         e = H.face_cycle_succ(e);
     } while (source(e) != source(be));

    if (H.outdeg(v) == 0) H.del_node(v);
*/

  }

  if (dim == 3) join_coplanar_faces(H);
}


static void random_sample(int n, const list<D3_POINT>& L, list<D3_POINT>& L1)
{
  int N        = L.length();
  list_item* A = new list_item[N];
  int i        = 0;

  list_item it;
  forall_items(it,L) A[i++] = it;

  L1.clear();

  while (n--)
    L1.append(L.inf(A[rand_int(0,N-1)]));

  delete[] A;
}
    


int simplify(list<D3_POINT>& L, int n)
{

  GRAPH<D3_POINT,int> G0;
  list<D3_POINT> L0;

  random_sample(n,L,L0);
  L0.sort();
  L0.unique();
  compute_hull(L0,G0);

  if (G0.number_of_nodes() >= 3*n/4) return 0;
  if (G0.number_of_nodes() <  6)     return 0;

  int M = G0.number_of_edges()/3;

  double* AX = new double[M];
  double* AY = new double[M];
  double* AZ = new double[M];
  double* AW = new double[M];
  double* NX = new double[M];
  double* NY = new double[M];
  double* NZ = new double[M];

  int m = 0;

  edge_array<bool>  considered(G0,false);
  edge e0;
  forall_edges(e0,G0)
  { if (considered[e0]) continue;

    edge x = e0;
    do { considered[x] = true;
         x = G0.face_cycle_succ(x);
    } while (x != e0);

    edge e1 = G0.face_cycle_succ(e0);
    edge e2 = G0.face_cycle_succ(e1);
    D3_POINT a = G0[source(e0)];
    D3_POINT b = G0[source(e1)];
    D3_POINT c = G0[source(e2)];
    AX[m] = a.XD();
    AY[m] = a.YD();
    AZ[m] = a.ZD();
    AW[m] = a.WD();
    double X1 = b.XD()*a.WD() - a.XD()*b.WD();
    double Y1 = b.YD()*a.WD() - a.YD()*b.WD();
    double Z1 = b.ZD()*a.WD() - a.ZD()*b.WD();
    double X2 = c.XD()*a.WD() - a.XD()*c.WD();
    double Y2 = c.YD()*a.WD() - a.YD()*c.WD();
    double Z2 = c.ZD()*a.WD() - a.ZD()*c.WD();
    NX[m] = Z1*Y2 - Y1*Z2;
    NY[m] = X1*Z2 - Z1*X2;
    NZ[m] = Y1*X2 - X1*Y2;
    m++;
  }

  int count = 0;

  list_item it = L.first();
  while (it)
  { 
    list_item next = L.succ(it);
    D3_POINT p = L[it];
    double px = p.XD();
    double py = p.YD();
    double pz = p.ZD();
    double pw = p.WD();
    int i = 0;
    while (i < m)
    { double aw = AW[i];
      if ( NX[i]*(px*aw-AX[i]*pw) +
           NY[i]*(py*aw-AY[i]*pw) +
           NZ[i]*(pz*aw-AZ[i]*pw) <= 0 ) break;
      i++;
     }
    if (i == m) 
    { count++;
      L.del_item(it); 
     }
    it = next;
   }

  delete[] AX;
  delete[] AY;
  delete[] AZ;
  delete[] AW;
  delete[] NX;
  delete[] NY;
  delete[] NZ;

//cout << string("simplify:  |L| = %d   %5.2f",L.length(),used_time(t));

  return count;
} 



void D3_HULL(const list<D3_POINT>& L0, GRAPH<D3_POINT,int>& H, int n)
{ H.clear();
  if (L0.empty()) return;
  list<D3_POINT> L = L0;

  if (n == -1)
  { double l = L0.length();
    n = int(::pow(l,0.6));
   }

  if (n > 6) simplify(L,n);

  L.sort();
  L.unique();
  compute_hull(L,H);
}



void D3_HULL0(list<D3_POINT>& L, GRAPH<D3_POINT,int>& H)
{ H.clear();
  if (L.empty()) return;
  compute_hull(L,H);
}

LEDA_END_NAMESPACE


