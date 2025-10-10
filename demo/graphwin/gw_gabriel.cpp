/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_gabriel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/b_stack.h>

using namespace leda;

static node moving_node = 0;

node test_edge(const GraphWin& gw, node u, node v)
{
  graph& G = gw.get_graph();

  point a = gw.get_position(u);
  point b = gw.get_position(v);
  point c = midpoint(a,b);
  circle circ(c,a);

  node result = 0;

  node x;
  forall_nodes(x,G) 
  { point p = gw.get_position(x);
    if (x != u && x != v && circ.inside(p)) {
      result = x; 
      break;
    }
  }
  return result;
}


void draw_gabriel(GraphWin& gw)
{ 
  window& W = gw.get_window();
  graph&  G = gw.get_graph();

  if (moving_node != 0) return;

  node v;
  forall_nodes(v,G) gw.set_color(v,ivory);

  edge e;
  forall_edges(e,G)  gw.set_color(e,blue);

  forall_edges(e,G) 
  { node u = G.source(e);
    node v = G.target(e);

    point a = gw.get_position(u);
    point b = gw.get_position(v);
    point c = midpoint(a,b);
    circle circ(c,a);
    W.set_line_width(1);
    W.draw_circle(circ,grey3);

    node x = test_edge(gw,u,v);

    if (x)
    { gw.set_color(e,red);
      gw.set_color(x,red);
      W.draw_circle(circ,red);
     }
  }

  // draw missing edges

  //W.set_line_width(2);
  W.set_line_width(1);

  double r = gw.get_node_radius();
  W.set_node_width(W.real_to_pix(r));

  forall_nodes(v,G) 
  { node_array<bool> adjacent(G,false);
    adjacent[v] = true;
    edge x;
    forall_inout_edges(x,v) {
      node w = G.opposite(x,v);
      adjacent[w] = true;
    }
    node w;
    forall_nodes(w,G) {
      if (adjacent[w]) continue;
      if (test_edge(gw,v,w) == 0)
      { point a = gw.get_position(v);
        point b = gw.get_position(w);
        W.draw_edge(a,b,green2);
      }
    }
  }

}

void bg_redraw(window* wp, double xmin, double ymin, double xmax, double ymax)
{ GraphWin* gwp = wp->get_graphwin();
  draw_gabriel(*gwp);
}

void init_graph_handler(GraphWin& gw)   { draw_gabriel(gw); }
void new_node_handler(GraphWin& gw,node){ draw_gabriel(gw); }
void new_edge_handler(GraphWin& gw,edge){ draw_gabriel(gw); }
void del_edge_handler(GraphWin& gw)     { draw_gabriel(gw); }
void del_node_handler(GraphWin& gw)     { draw_gabriel(gw); }


bool start_move_node_handler(GraphWin& gw, node u)
{ graph& G = gw.get_graph();

  moving_node = u;

  node v;
  forall_nodes(v,G) gw.set_color(v,ivory);

  edge e;
  forall_edges(e,G) gw.set_color(e,black);

  return true;
}

bool end_move_node_handler(GraphWin& gw, node v, const point & pos)
{ moving_node = 0;
  return true;
}




int main()
{
  GraphWin gw("Gabriel Demo");

  gw.set_bg_redraw(bg_redraw);

  gw.set_start_move_node_handler(start_move_node_handler);
  gw.set_end_move_node_handler(end_move_node_handler);

  gw.set_init_graph_handler(init_graph_handler);
  gw.set_new_edge_handler(new_edge_handler);
  gw.set_del_edge_handler(del_edge_handler);
  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_directed(false);

  gw.set_action(A_LEFT | A_DRAG | A_FRAME,NULL);
  gw.set_action(A_LEFT | A_DRAG | A_EDGE,NULL);

  gw.set_node_label_type(user_label);
  gw.set_node_radius(6);
  gw.set_edge_width(2);

  gw.display();
  gw.edit();

  return 0;
}

