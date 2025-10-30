/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_tamassia.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cin;
using std::endl;
#endif



int main()
{

  GraphWin GW("Drawing Planar Graphs Orthogonally (Tamassia)");


  GW.set_bg_color(grey2);
  GW.set_node_color(green2);
  GW.set_edge_color(violet);

/*
  GW.set_edge_width(2);
  GW.set_d3_look(true);
*/

  GW.set_node_shape(rectangle_node);
  GW.set_node_width(24);
  GW.set_node_height(24);

  GW.display();

  GW.message("\\bf\\blue Please construct a planar graph and press done.");
  leda_wait(1.75);
  GW.message("");

  while (GW.edit()) 
  {
    GW.set_flush(false);

    graph& G = GW.get_graph();

    if (!PLANAR(G))
    { GW.message("\\bf Graph must be planar !");
      leda_wait(1.5);
      GW.message("");
      continue;
     }

    list<edge> bi_edges;
    Make_Connected(G,bi_edges);
    Make_Bidirected(G,bi_edges);

    G.make_planar_map();

    node_array<int> xpos(G);
    node_array<int> ypos(G);
    edge_array< list<int> > xbends(G);
    edge_array< list<int> > ybends(G);

    edge_array<int> bound(G,8);

    edge e;
    forall_edges(e,G)
    { edge r = G.reversal(e);
      if (GW.get_color(e) == red || GW.get_color(r) == red)
      bound[e] = 0;
      bound[r] = 0;
     }
      

    ORTHO_EMBEDDING(G, bound, xpos, ypos, xbends, ybends);

    G.del_edges(bi_edges);

    GW.update_edges();

    node v;
    forall_nodes(v,G)
       GW.set_position(v,point(xpos[v],ypos[v]));

    forall_edges(e,G)
    { list<int> lx = xbends[e];
      list<int> ly = ybends[e];
      list<point> pol;
      while ( !lx.empty() )
      { double x = lx.pop();
        double y = ly.pop();
        pol.append(point(x,y));
       }
      GW.set_bends(e,pol);
    }

   GW.fill_window();
   GW.set_flush(true);
  }

  return 0;
}
