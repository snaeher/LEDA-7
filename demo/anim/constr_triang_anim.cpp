/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  constr_triang_anim.c
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
#include <LEDA/system/param_handler.h>
#include <LEDA/system/file.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::ifstream;
using std::ofstream;
using std::endl;
#endif

typedef point   POINT;
typedef segment SEGMENT;


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  param_handler P(argc,argv,".epm",false);
  P.add_parameter("inputgraph:-i:string:");
  param_handler::init_all();

  string f;
  P.get_parameter("-i",f);

  window W(500,500);
  W.set_node_width(2);
  W.set_grid_mode(4);
  W.set_show_coordinates(true);
  W.display();

  SEGMENT s;
  list<SEGMENT> L;

  GRAPH<POINT,SEGMENT> G;
  graph_node_position<POINT,SEGMENT> pos(G);

  if (f != "")
  { ifstream from(f);
    while (from >> s) {
      W << s;
      if (s.is_trivial()) W << s.source();
      L.push_back(s);
    }
    from.close();
  }

  while (W >> s) 
  { if (s.is_trivial()) W << s.source();
    L.push_back(s);
  }


  ofstream to("ct.log");
  to << L; 
  to.close();




  gw_observer Obs(G,pos,500,500);
  Obs.set_edge_distance(0);
  Obs.set_edge_width(2);
  Obs.set_edge_direction(undirected_edge);
  Obs.set_node_width(8);
  Obs.set_node_height(8);
  Obs.set_node_label_type(no_label);
  Obs.win_init(W.xmin(),W.xmax(),W.ymin());
  Obs.display(window::min,window::min);

  SWEEP_SEGMENTS(L, G, true);

  Obs.zoom_graph();
  Obs.set_edge_color(green,false);
  edge e = TRIANGULATE_PLANE_MAP(G);
  Obs.set_color(e,red);
  Obs.wait();

  //forall_edges(e,G) G.set_reversal(e,nil);

  return 0;
}

