/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_dc_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/core/array.h>
#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/geo/d3_hull.h>

LEDA_BEGIN_NAMESPACE

static void join_coplanar_faces(GRAPH<d3_rat_point, int>& H)
{
  edge_array<bool> considered(H,false);
  list<edge> L;

  d3_rat_point A, B, C, D;
  edge e, e1, r, r1;

  forall_edges (e, H) {
    if (considered[e]) continue;
    r = H.reversal(e);
    e1 = H.cyclic_adj_pred(r);
    r1 = H.cyclic_adj_pred(e);

    A = H[source(e)];
    B = H[source(e1)];
    C = H[target(e1)];
    D = H[target(r1)];

    if (orientation(A,B,C,D) == 0) 
    { 
      d3_rat_point P=point_on_positive_side(A, B, C);
      if (orientation(B, A, D, P) < 0) {
	L.append(e);
	L.append(r);
      }
      considered[e] = considered[r] = true;
     }
   }
   forall(e,L) H.del_edge(e);

   node v=H.first_node();
   while (v) {
     node v1=H.succ_node(v);
     if (H.degree(v)==0) H.del_node(v);
     if (H.outdeg(v)==2) {
       e=H.first_adj_edge(v);
       edge e1=H.adj_succ(e);
       d3_rat_point A=H[v];
       d3_rat_point B=H[H.target(e)];
       d3_rat_point C=H[H.target(e1)];
       if (collinear(A, B, C)) {
	 edge r=H.reversal(e);
	 edge r1=H.reversal(e1);
	 H.set_reversal(H.new_edge(r, e1), H.new_edge(r1, e));
	 H.del_node(v);
       }
     }
     v=v1;
   }
}

static int inner_point(GRAPH<d3_rat_point, int>& H, d3_rat_point& pi)
{
  node v=H.first_node();
  if (!v) return 0;
  d3_rat_point p1, p2, p3, p4;
  pi=p1=p2=p3=p4=H[v];
  
  while (v && collinear(p1, p2, p3)) {
    v=H.succ_node(v);
    if (v) {p2=p3; p3=H[v];}
  }
  pi=midpoint(p1, p2);
  if (!v) return 1;
  while (v && coplanar(p1, p2, p3, p4)) {
    v=H.succ_node(v);
    if (v) {p3=p4; p4=H[v];}
  }
  pi=midpoint(pi, p3);
  if (!v) return 2;
  pi=midpoint(pi, p4);
  return 3;
}

static void tangent(GRAPH<d3_rat_point, int>& H, node& v1, node& v2, 
		    int plane=0)
{
  rat_point p1, p2, p3, p;
  edge e=0, e1=0;
  bool t1=false;
  bool t2=false;

  if (plane==0) {
    p1=H[v1].project_xy();
    p2=H[v2].project_xy();
  }
  else {
    p1=H[v1].project_xz();
    p2=H[v2].project_xz();
  }
  if (p1==p2) {tangent(H, v1, v2, 1); return;}

  while (!t1 || !t2) {
    t1=true;
    e=H.first_adj_edge(v1);
    while (e && t1) {
      if (plane==0) p3=H[H.target(e)].project_xy();
      else p3=H[H.target(e)].project_xz();
      if (right_turn(p2, p1, p3)) {e1=e; t1=false;}
      /*      if (collinear(p2, p1, p3) && (p2.sqr_dist(p3)>p2.sqr_dist(p1)))
	{e1=e; t1=false;}*/
      e=H.adj_succ(e);
    }
    while (!t1) {
      v1=H.target(e1);
      p1=p3;
      t1=true;
      e=H.first_adj_edge(v1);
      while (e && t1) {
	if (plane==0) p3=H[H.target(e)].project_xy();
	else p3=H[H.target(e)].project_xz();
	if (right_turn(p2, p1, p3)) {e1=e; t1=false;}
	/*	if (collinear(p2, p1, p3) && (p2.sqr_dist(p3)>p2.sqr_dist(p1)))
	  {e1=e; t1=false;}*/
	e=H.adj_succ(e);
      }
    }
    t2=true;
    e=H.first_adj_edge(v2);
    while (e && t2) {
      if (plane==0) p3=H[H.target(e)].project_xy();
      else p3=H[H.target(e)].project_xz();
      if (left_turn(p1, p2, p3)) {e1=e; t2=false;}
      /*      if (collinear(p2, p1, p3) && (p1.sqr_dist(p3)>p1.sqr_dist(p2)))
	{e1=e; t2=false;}
	*/
      e=H.adj_succ(e);
    }
    while (!t2) {
      v2=H.target(e1);
      p2=p3;
      t2=true;
      e=H.first_adj_edge(v2);
      while (e && t2) {
	if (plane==0) p3=H[H.target(e)].project_xy();
	else p3=H[H.target(e)].project_xz();
	if (left_turn(p1, p2, p3)) {e1=e; t2=false;}
	/*	if (collinear(p2, p1, p3) && (p1.sqr_dist(p3)>p1.sqr_dist(p2)))
	  {e1=e; t2=false;}
	  */
	e=H.adj_succ(e);
      }
      t1=false;
    }
  }
}

static edge winner(GRAPH<d3_rat_point, int>& H, node v1, node v2,
		   d3_rat_point pi, int side)
{
  edge we;
  int out;
  d3_rat_point pt;
  d3_rat_point p1=H[v1];
  d3_rat_point p2=H[v2];

  if (side) {we=H.first_adj_edge(v2); out=-1; pt=p2;}
  else {we=H.first_adj_edge(v1); out=1; pt=p1;}
  d3_rat_point p3=H[H.target(we)];
  d3_rat_point p4;
  edge e=H.adj_succ(we);
  while (e && (orientation(p1, p2, p3, pi)<=0) ) {
    we=e;
    p3=H[H.target(e)];
    e=H.adj_succ(e);
  }
  while (e) {
    p4=H[H.target(e)];
    int c=orientation(p1, p2, p3, p4);
    if (c<0) {we=e; p3=p4;}
    if ( (c==0) && (orientation(p1, p2, p4, pi)<0) 
	 && (orientation(pt, p3, p4, pi)==out) )
      {we=e; p3=p4;}
    e=H.adj_succ(e);
  }
  return we;
}

static void wrap(GRAPH<d3_rat_point, int>& H, node& v1, node& v2,
		 list<edge>& L1, list<edge>& L2, d3_rat_point pi)
{
  d3_rat_point start1=H[v1];
  d3_rat_point start2=H[v2];
  d3_rat_point p1=start1;
  d3_rat_point p2=start2;
  d3_rat_point p3, p4;
  d3_rat_point last=p1;
  bool turn=false;
  edge e1, e2;
  do {
    e1=winner(H, v1, v2, pi, 0);
    e2=winner(H, v1, v2, pi, 1);
    p3=H[H.target(e1)];
    p4=H[H.target(e2)];
    int o=orientation(p1, p2, p3, p4);
    if ( (o<0) || (o==0 && orientation(p1, p2, p4, pi)>0) ) {
      if ( !coplanar(p1, p2, p4, last) && !turn ) {
	L1.clear(); L2.clear(); start1=p1; start2=p2; turn=true;
      }
      L2.append(e2);
      last=p2;
      p2=p4;
      v2=H.target(e2);
    }
    else {
      if ( !coplanar(p1, p2, p3, last) && !turn) {
	L1.clear(); L2.clear(); start1=p1; start2=p2; turn=true;
      }
      L1.append(e1);
      last=p1;
      p1=p3;
      v1=H.target(e1);
      }
  } while (!collinear(start1, start2, p1) || !collinear(start1, start2, p2) );

  while (p1 != start1) {
    e1=H.first_adj_edge(v1);
    p3=H[H.target(e1)];
    while (!collinear(p1, p3, start1) ||
	   start1.sqr_dist(p3)>start1.sqr_dist(p1) ) {
      e1=H.adj_succ(e1);
      p3=H[H.target(e1)];
    }
    L1.append(e1);
    p1=p3;
    v1=H.target(e1);
  }
  while (p2 != start2) {
    e2=H.first_adj_edge(v2);
    p4=H[H.target(e2)];
    while (!collinear(p2, p4, start2) ||
	   start2.sqr_dist(p4)>start2.sqr_dist(p2) ) {
      e2=H.adj_succ(e2);
      p4=H[H.target(e2)];
    }
    L2.append(e2);
    p2=p4;
    v2=H.target(e2);
  }
}

static void dfs(node s, node_array<bool>& reached, list<node>& L)
{
  L.append(s);
  reached[s] = true; 
  node v;
  forall_adj_nodes(v,s)
    if ( !reached[v] ) dfs(v,reached,L);
}

static void remove_nodes(GRAPH<d3_rat_point,int>& H, node v1, node v2, 
			 list<edge>& L1, list<edge>& L2)
{
 edge e;
 edge e1;
 node v;
 node_array<bool> visited(H, false);
 edge_array<bool> cut(H, false);
 list<node> L;

 forall(e, L1) {
   visited[H.source(e)]=true; 
   cut[e]=cut[H.reversal(e)]=true;
 }
 forall(e, L1) {
   e1=H.cyclic_adj_succ(e);
   while (!cut[e1]) {
     v=H.target(e1);
     if (!visited[v]) dfs(v, visited, L);
     H.del_edge(H.reversal(e1));
     H.del_edge(e1);
     e1=H.cyclic_adj_succ(e);    
   }
 }

 forall(e, L2) {
   visited[H.source(e)]=true; 
   cut[e]=cut[H.reversal(e)]=true;
 }
 forall(e, L2) {
   e1=H.cyclic_adj_pred(e);
   while (!cut[e1]) {
     v=H.target(e1);
     if (!visited[v]) dfs(v, visited, L);
     H.del_edge(H.reversal(e1));
     H.del_edge(e1);
     e1=H.cyclic_adj_pred(e);    
   }
 }

 if (L1.empty()) {
   visited[v1]=true;
   e=H.first_adj_edge(v1);
   if (e) {
     v=H.target(e);
     dfs(v, visited, L);
     H.del_edge(H.reversal(e));
     H.del_edge(e);
   }
 }

 if (L2.empty()) {
   visited[v2]=true;
   e=H.first_adj_edge(v2);
   if (e) {
     v=H.target(e);
     dfs(v, visited, L);
     H.del_edge(H.reversal(e));
     H.del_edge(e);
   }
 }

 H.del_nodes(L);
}

static void new_edges(GRAPH<d3_rat_point,int>& H, node v1, node v2,
		      list<edge>& L1, list<edge>& L2, d3_rat_point pi)
{
  list_item it1=L1.first();
  list_item it2=L2.first();
  edge e1=0, e2=0;
  edge s1, s2;
  edge new1, new2;
  node next1=0, next2=0;
  d3_rat_point p1=H[v1];
  d3_rat_point p2=H[v2];
  d3_rat_point start1=p1;
  d3_rat_point start2=p2;
  d3_rat_point p3, p4;
  int next=0;

  if (it1) {
    e1=H.reversal(L1.tail());
    new1=H.new_edge(e1, v2, 0, leda::before);
  }
  else new1=H.new_edge(v1, v2, 0);
  if (it2) {
    e2=H.reversal(L2.tail());
    new2=H.new_edge(e2, v1, 0, leda::behind);
  }
  else new2=H.new_edge(v2, v1, 0);
  H.set_reversal(new1, new2);
  s1=new1; s2=new2;

  do {
    if (it1) {
      next=0;
      e1=H.reversal(L1[it1]);
      next1=H.source(e1);
      p3=H[next1];
    }
    if (it2) {
      next=1;
      e2=H.reversal(L2[it2]);
      next2=H.source(e2);
      p4=H[next2];
    }
    if (it1 && it2) {
      int c=orientation(p1, p2, p3, p4);
      if ( (c>0) || ((c==0) && (orientation(p1, p2, p4, pi)>=0)) ) next=0;
    }
    if (next==0) {
      v1=next1; p1=p3;
      new1=H.new_edge(e1, v2, 0, leda::before);
      new2=H.new_edge(new2, v1, 0, leda::behind);
      it1=L1.succ(it1);
    }
    else {
      v2=next2; p2=p4;
      new1=H.new_edge(new1, v2, 0, leda::before);
      new2=H.new_edge(e2, v1, 0, leda::behind);
      it2=L2.succ(it2);
    }
    H.set_reversal(new1, new2);
  } while (!collinear(start1, start2, p1) || !collinear(start1, start2, p2));

  if ((p1!=start1) || (p2!=start2)) {
    if (p1.sqr_dist(p2)<start1.sqr_dist(p2)) {e1=new1;}
    else {e1=s1; p1=start1;}
    if (p2.sqr_dist(p1)<start2.sqr_dist(p1)) {e2=new2;}
    else {e2=s2; p2=start2;}
    H.set_reversal(H.new_edge(e1, H.source(e2), 0, leda::behind), 
		   H.new_edge(e2, H.source(e1), 0, leda::behind));
    H.del_edge(new1); H.del_edge(new2);
  }
  H.del_edge(s1); H.del_edge(s2);
}

static void merge(GRAPH<d3_rat_point, int>& H, GRAPH<d3_rat_point, int>& H2)
{
  node v1=H.last_node();
  node v2=H2.first_node();
 
  H.join(H2);

  d3_rat_point pi;
  int dim=inner_point(H, pi);

  // all points are collinear
  if (dim==1) {
    H.del_all_edges();
    node v1=H.first_node();
    node v2=H.last_node();
    node v;
    forall_nodes(v, H) {
      if (v!=v1 && v!=v2) H.del_node(v);
    }
    H.set_reversal(H.new_edge(v1, v2), H.new_edge(v2, v1));
    return;
  }
  // coplanar
  if (dim==2) {
    list<d3_rat_point> L;
    node v;
    forall_nodes(v, H) L.append(H[v]);
    D3_HULL0(L, H); 
    return;
  }
  
  // compute upper tangent
  tangent(H, v1, v2);
  
  // compute lists of cut edges L1 and L2
  list<edge> L1, L2;
  wrap(H, v1, v2, L1, L2, pi);

  // remove everything inside the cut edge cycle
  remove_nodes(H, v1, v2, L1, L2);

  // insert new edges
  new_edges(H, v1, v2, L1, L2, pi);
}

static void dc_d3_hull(array<d3_rat_point>& A, int l, int r, GRAPH<d3_rat_point, int>& H1, int n)
{
  int sz = r-l+1;

  if (sz > n) {

    int m = (l+r)/2;
    
    GRAPH<d3_rat_point, int> H2;

    dc_d3_hull(A,l,m,H1,n);
    dc_d3_hull(A,m+1,r,H2,n);
    merge(H1, H2); 
  }
  else {
/*
    d3_rat_point pa = A[l];
    d3_rat_point pb = A[l+1];
    d3_rat_point pc = A[r];

    node a = H1.new_node(pa);

    while ( r > l+1 && collinear(pa, pb, pc) ) pb=pc;

    node b = H1.new_node(pb);

    H1.set_reversal(H1.new_edge(a,b,0), H1.new_edge(b,a,0));

    if (sz == 3) {
      node c = H1.new_node(pc);
      H1.set_reversal(H1.new_edge(a,c,0), H1.new_edge(c,a,0));
      H1.set_reversal(H1.new_edge(b,c,0), H1.new_edge(c,b,0));
    }
*/
      list<d3_rat_point> L;
      for(int i=l; i<=r; i++) L.append(A[i]);
      D3_HULL0(L, H1);
  }
}

void D3_DC_HULL(const list<d3_rat_point>& L0, GRAPH<d3_rat_point, int>& H, int n)
{
  H.clear();
  if (L0.empty()) return;

  list<d3_rat_point> L=L0;
  L.sort();
  L.unique();

  d3_rat_point p;
  int i = 0;
  array<d3_rat_point> A(L.length()); 
  forall(p,L) A[i++] = p;
 
  dc_d3_hull(A,0,i-1,H,n);
  join_coplanar_faces(H);
}

LEDA_END_NAMESPACE



