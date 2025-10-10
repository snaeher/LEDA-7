/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  poly_triang_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/rat_window.h>
#include <LEDA/graphics/gw_observer.h>
#include <LEDA/core/param_handler.h>
#include <LEDA/system/file.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::endl;



typedef point   POINT;
typedef segment SEGMENT;


void TRIANGULATE_POLY(const list<POINT>& poly, GRAPH<point,SEGMENT>& G, 
                                                  list<edge>& in_edges,
                                                  list<edge>& out_edges)
{ list <SEGMENT> S;
  list_item it1 = poly.last();
  list_item it2;
  forall_items(it2,poly)
  { S.append(SEGMENT(poly[it1],poly[it2]));
    it1 = it2;
   }

  SWEEP_SEGMENTS(S,G,true);

  assert(Genus(G) == 0);

  edge e0 = G.first_edge();

  TRIANGULATE_PLANE_MAP(G);
  assert(Genus(G) == 0);

  in_edges.clear();
  out_edges.clear();
  
  edge e = e0;
  do { edge r = G.reversal(e);
       edge x = G.cyclic_adj_succ(r);
       while (G[x] == SEGMENT(0,0,0,0))
       { in_edges.append(x);
         in_edges.append(G.reversal(x));
         x = G.cyclic_adj_succ(x);
        }
       x = G.cyclic_adj_pred(r);
       while (G[x] == SEGMENT(0,0,0,0))
       { out_edges.append(x);
         out_edges.append(G.reversal(x));
         x = G.cyclic_adj_pred(x);
        }
       e = x;
  } while (e != e0);
}
   

void TRIANGULATE_POLY(const list<POINT>& poly, GRAPH<point,SEGMENT>& G)
{ list<edge> in_edges;
  list<edge> out_edges;
  TRIANGULATE_POLY(poly,G,in_edges,out_edges);
  G.del_edges(out_edges);
}



      


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  window W(500,500);
  W.set_node_width(2);
  W.set_grid_mode(4);
  W.set_show_coordinates(true);
  W.display();


  GRAPH<POINT,SEGMENT> G;
  graph_node_position<POINT,SEGMENT> pos(G);


  polygon P;
  W >> P;
  W << P;


  gw_observer Obs(G,pos,500,500);
  //Obs.set_edge_distance(0);
  //Obs.set_edge_width(2);
  //Obs.set_edge_direction(undirected_edge);
  Obs.set_node_width(8);
  Obs.set_node_height(8);
  Obs.set_node_label_type(no_label);
  Obs.win_init(W.xmin(),W.xmax(),W.ymin());
  Obs.display(window::min,window::min);


  list<edge> in_edges;
  list<edge> out_edges;
  TRIANGULATE_POLY(P.vertices(),G,in_edges,out_edges);

  Obs.zoom_graph();

  edge e;
  forall(e,in_edges) Obs.set_color(e,blue);
  forall(e,out_edges) Obs.set_color(e,red);

  Obs.wait();

  forall(e,out_edges) 
  { edge r = G.reversal(e);
    G.del_edge(e);
    G.del_edge(r);
    assert(Genus(G) == 0);
    Obs.wait();
   }
  

  return 0;
}

