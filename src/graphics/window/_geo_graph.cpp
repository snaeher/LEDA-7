/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _geo_graph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/geo_graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

void geo_graph_rep::copy_graph(graph& H, const graph& G, 
                           node_map<node>& V, edge_map<edge>& E) 
{ 
  H.clear();
  V.init(H);
  E.init(H);

  node_array<node> v_in_H(G); 

  node v;
  forall_nodes(v,G) 
  { node u = H.new_node();
    v_in_H[v] = u;
    V[u] = v;
   }

  forall_nodes(v,G)
  { node u = v_in_H[v];
    edge e;
    forall_adj_edges(e,v) 
    { edge x = H.new_edge(u,v_in_H[target(e)]);
      E[x] = e;
     }
   }
}


geo_graph_rep::geo_graph_rep() {}

/*
geo_graph_rep::geo_graph_rep(const geo_graph_rep& gg) 
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,gg.gr,V,E);

  node v;
  forall_nodes(v,gr) 
  { gr[v] = gg.gr[V[v]];
    kind[v] = gg.kind[V[v]];
   }

  edge e;
  forall_edges(e,gr) gr[e] = gg.gr[E[e]];
}
*/

geo_graph_rep::geo_graph_rep(const graph& G, const node_array<point>& pos,
                                     const edge_array<int>& lab)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  node v;
  forall_nodes(v,gr) gr[v] = pos[V[v]];
  edge e;
  forall_edges(e,gr) gr[e] = lab[E[e]];
 }


geo_graph_rep::geo_graph_rep(const graph& G, const node_array<rat_point>& pos,
                                     const edge_array<int>& lab)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  node v;
  forall_nodes(v,gr) gr[v] = pos[V[v]].to_float();
  edge e;
  forall_edges(e,gr) gr[e] = lab[E[e]];
 }




geo_graph_rep::geo_graph_rep(const GRAPH<point,int>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  node v;
  forall_nodes(v,gr) gr[v] = G[V[v]];
  edge e;
  forall_edges(e,gr) gr[e] = G[E[e]];
 }

geo_graph_rep::geo_graph_rep(const GRAPH<rat_point,int>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  node v;
  forall_nodes(v,gr) gr[v] = G[V[v]].to_float();
  edge e;
  forall_edges(e,gr) gr[e] = G[E[e]];
 }


geo_graph_rep::geo_graph_rep(const GRAPH<point,segment>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  segment triv_seg(point(0,0),point(0,0));
  node v;
  forall_nodes(v,gr) gr[v] = G[V[v]];
  edge e;
  forall_edges(e,gr) gr[e] = (G[E[e]] == triv_seg);
 }


geo_graph_rep::geo_graph_rep(const GRAPH<rat_point,rat_segment>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);
  rat_segment triv_seg(rat_point(0,0,1),rat_point(0,0,1));
  node v;
  forall_nodes(v,gr) gr[v] = G[V[v]].to_float();
  edge e;
  forall_edges(e,gr) gr[e] = (G[E[e]] == triv_seg);
 }


geo_graph_rep::geo_graph_rep(const GRAPH<circle,point>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);

  node v;
  forall_nodes(v,gr)
  { circle C = G[V[v]];
    if (C.is_degenerate())
      kind[v] = infinite;
    else
      gr[v] = C.center();
  }

  forall_nodes(v,gr)
  {  
    if (kind[v] == finite) continue;

    assert(gr.outdeg(v) == 1);

    circle C = G[V[v]];

    node  u = gr.target(gr.first_adj_edge(v));
    point a = C.point1();
    point b = C.point2();
    point c = C.point3();

    vector vec = (c-a).rotate90();
    if (kind[u] == infinite) 
       gr[v] = b + vec;
    else
       gr[v] = gr[u] + vec;
  }

  edge e;
  forall_edges(e,gr) gr[e] = 0;
}


geo_graph_rep::geo_graph_rep(const GRAPH<rat_circle,rat_point>& G)
{ node_map<node> V;
  edge_map<edge> E;
  copy_graph(gr,G,V,E);
  kind.init(gr,finite);

  node v;
  forall_nodes(v,gr)
  { rat_circle C = G[V[v]];
    if (C.is_degenerate())
      kind[v] = infinite;
    else
      gr[v] = C.center().to_float();
  }

  forall_nodes(v,gr)
  {  
    if (kind[v] == finite) continue;

    assert(gr.outdeg(v) == 1);

    rat_circle C = G[V[v]];

    node  u = gr.target(gr.first_adj_edge(v));
    rat_point a = C.point1();
    rat_point b = C.point2();
    rat_point c = C.point3();

    vector vec = (c-a).rotate90().to_float();
    if (kind[u] == infinite) 
       gr[v] = b.to_float() + vec;
    else
       gr[v] = gr[u] + vec;
  }

  edge e;
  forall_edges(e,gr) gr[e] = 0;
}





bool geo_graph_rep::get_point(node v, point& p) const
{ bool b = (kind[v] == finite);
  if (b) p = gr[v];
  return b;
 }

bool geo_graph_rep::get_segment(edge e, segment& s) const
{ node v = source(e);
  node w = target(e);
  bool b = (is_finite(v) && is_finite(w));
  if (b) s = segment(gr[v],gr[w]);
  return b;
 } 

bool geo_graph_rep::get_ray(edge e, ray& r) const
{ node v = source(e);
  node w = target(e);
  bool b = (is_finite(v) && !is_finite(w));
  if (b) r = ray(gr[v],gr[w]);
  return b;
 } 

bool geo_graph_rep::get_line(edge e, line& l) const
{ node v = source(e);
  node w = target(e);
  bool b = (!is_finite(v) && !is_finite(w));
  if (b) l = line(gr[v],gr[w]);
  return b;
 }

void geo_graph_rep::get_objects(list<segment>& slist, list<ray>& rlist, 
                                       list<line>& llist) const
{ edge e;
  forall_edges(e,gr)
  { node v = source(e);
    node w = target(e);
    if (is_finite(v) && is_finite(w))
      slist.append(segment(gr[v],gr[w]));
    else
      if (is_finite(v))
         rlist.append(ray(gr[v],gr[w]));
      else
        if (is_finite(w))
           rlist.append(ray(gr[w],gr[v]));
        else
           llist.append(line(gr[v],gr[w]));
    }
 }


void geo_graph_rep::translate(double dx, double dy)
{ node v;
  forall_nodes(v,gr) gr[v] = gr[v].translate(dx,dy);
 }

void geo_graph_rep::rotate(const point& p, double phi)
{ node v;
  forall_nodes(v,gr) gr[v] = gr[v].rotate(p,phi);
 }



window& operator<<(window& W, const geo_graph_rep& gg)
{
  list<segment> slist;
  list<ray>     rlist;
  list<line>    llist;

  gg.get_objects(slist,rlist,llist);

  segment s;
  forall(s,slist) W << s;

  ray r;
  forall(r,rlist) W << r;

  line l;
  forall(l,llist) W << l;

  return W;
}

window& operator>>(window& W, geo_graph_rep& gg) 
{ GRAPH<point,int> G;
  GraphWin gw(G,W);
  gw.edit();
  node v;
  forall_nodes(v,G) G[v] = gw.get_position(v);
  gg.set_graph(G);
  gg.kind.init(gg.gr,geo_graph_rep::finite);
  return W; 
}



ostream& operator<<(ostream& o, const geo_graph_rep& gg)
{ const GRAPH<point,int>& G = gg.get_graph(); 
  o << G;
  node v;
  forall_nodes(v,G) o << gg.kind[v] << endl;
  return o;
}

istream& operator>>(istream& i, geo_graph_rep& gg)
{ GRAPH<point,int> G;
  i >> G; 
  gg.set_graph(G);
  gg.kind.init(gg.gr,geo_graph_rep::finite);
  node v;
  forall_nodes(v,gg.gr) 
  { int vis;
    i >> vis;
    gg.kind[v] = vis;
   }
  return i;
 }

LEDA_END_NAMESPACE
