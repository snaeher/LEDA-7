/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  flip_segments.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/geo_graph.h>
#include <LEDA/graphics/gw_observer.h>

using namespace leda;



typedef point   POINT;
typedef segment SEGMENT;


int DELAUNAY_FLIPPING(GRAPH<POINT,SEGMENT>& G, list<edge>& aux_edges)
{
  aux_edges.clear();

  if (G.number_of_nodes() <= 3) return 0;

  // int f = ( k == NEAREST ? +1 : -1);

  int f = 1;

  list<edge> S;
  edge e;
  forall_edges(e,G)
    if (G[e].is_trivial()) S.append(e);

  int flip_count = 0;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = G.reversal(e);

    
    // e1,e2,e3,e4: edges of quadriliteral with diagonal e
    edge e1 = G.face_cycle_succ(r);
    edge e3 = G.face_cycle_succ(e);

    node A = source(e1);
    node B = target(e1);
    node C = source(e3);
    node D = target(e3);

    // flip test
    POINT a = G[A];
    POINT b = G[B];
    POINT c = G[C];
    POINT d = G[D];

    SEGMENT triv_seg;

    if ( left_turn(d,a,b) && left_turn(b,c,d) )
    { // the quadrilateral is convex

      
      int soc = f * side_of_circle(a,b,c,d);

      if (soc > 0) // flip
      { edge e2 = G.face_cycle_succ(e1);
        edge e4 = G.face_cycle_succ(e3);

        if (G[e].is_trivial())
        { // flip diagonal
          G.move_edge(e,e2,source(e4));
          G.move_edge(r,e4,source(e2));
          flip_count++;
         }
        else
        { // split edge
          SEGMENT s = G[e];
          G.del_edge(e);
          G.del_edge(r);
          node u = G.new_node(center(a,c));
          edge x1 = G.new_edge(u,A,s);
          edge x2 = G.new_edge(u,B,triv_seg);
          edge x3 = G.new_edge(u,C,s);
          edge x4 = G.new_edge(u,D,triv_seg);

          edge y1 = G.new_edge(e1,u,s);
          edge y2 = G.new_edge(e2,u,triv_seg);
          edge y3 = G.new_edge(e3,u,s);
          edge y4 = G.new_edge(e4,u,triv_seg);

          G.set_reversal(x1,y1);
          G.set_reversal(x2,y2);
          G.set_reversal(x3,y3);
          G.set_reversal(x4,y4);

          aux_edges.append(x2);
          aux_edges.append(y2);
          aux_edges.append(x4);
          aux_edges.append(y4);
        }

        
        S.push(e1); 
        S.push(e2); 
        S.push(e3); 
        S.push(e4); 
      }

    }

  }

  return flip_count;
}



int main() 
{
  window W;
  W.display(window::center,window::center);

  list<SEGMENT> L;
  segment s;
  while (W >> s) L.append(s);

  GRAPH<POINT,SEGMENT> G;
  graph_node_position<POINT,SEGMENT> pos(G);

  gw_observer Obs(G,pos,500,500);
  //Obs.set_edge_distance(0);
  //Obs.set_edge_width(2);
  //Obs.set_edge_direction(undirected_edge);
  Obs.set_node_width(8);
  Obs.set_node_height(8);
  Obs.set_node_label_type(no_label);
  Obs.win_init(W.xmin(),W.xmax(),W.ymin());
  Obs.display(window::min,window::min);

  edge hull_e = TRIANGULATE_SEGMENTS(L,G);
  geo_graph gg1(G);
  W.set_color(blue);
  W << gg1;
  W.read_mouse();

  edge_array<bool> hull(G,false);
 
  list<edge> aux_edges;
  int flip_count = DELAUNAY_FLIPPING(G,aux_edges);


/*
  int flip_count = DELAUNAY_FLIPPING(G,segment(0,0,0,0));
*/

  W.clear();
  W.set_color(black);

  edge e;
  forall_edges(e,G) 
  { if (G[e] != segment(0,0,0,0)) W.set_line_width(3);
    W.draw_segment(G[source(e)],G[target(e)]);
    W.set_line_width(1);
   }

/*
  forall(e,aux_edges) 
     W.draw_segment(G[source(e)],G[target(e)],red);
*/

  W.read_mouse();

  return 0;
}

