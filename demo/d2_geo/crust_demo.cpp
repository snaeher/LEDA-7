#include <LEDA/graphics/window.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/map.h>
#include <LEDA/geo/float_kernel.h>
#include <LEDA/geo/geo_alg.h>

using namespace leda;


void crust(const list<point>& S, GRAPH<point,int>& G) 
{
  list<point> L = S;

  GRAPH<circle,point> VD;

  VORONOI(L,VD);

  // add Voronoi vertices and mark them

  map<point,bool> voronoi_vertex(false);

  node v;
  forall_nodes(v,VD)
  { if (VD.outdeg(v) < 2) continue; 
    point p = VD[v].center();
    voronoi_vertex[p] = true;
    L.append(p);
  }

  DELAUNAY_TRIANG(L,G);

  list<node> vlist;
  forall_nodes(v,G)
     if (voronoi_vertex[G[v]]) vlist.append(v);

  G.del_nodes(vlist);
}


int main()
{ 
  window W; 
  W.display(); 

  point p;
  list<point> S;
  GRAPH<point,int> G;
  
  while ( W >> p )
  { S.append(p);
    crust(S,G);
    
    W.clear();
    W.set_line_width(1);
    W.set_node_width(2); 

    node v;
    forall_nodes(v,G) W.draw_node(G[v]);

    edge e;
    forall_edges(e,G) W.draw_segment(G[source(e)], G[target(e)]);


    edge_array<bool> visited(G,false);

    forall_edges(e,G)
    {
      if (visited[e]) continue;

      list<point> L;

      edge x = e;
      do { edge r = G.reversal(x);
           visited[x] = visited[r] = true;
           L.append(G[source(x)]);
           x = G.face_cycle_succ(x);
          } while (x != e);

      if (L.length() < 2) continue;

      W.set_line_width(1);
      W.draw_closed_spline(L,16,red);
    }

  }

 return 0;
}
      
