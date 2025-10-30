/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_tam1.c
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


int main()
{

  GraphWin GW("Drawing Planar Graphs Orthogonally (Tamassia)");

  GW.set_node_shape(rectangle_node);
  GW.set_node_width(24);
  GW.set_node_height(24);

  GW.display();

  GW.message("\\bf\\blue Please construct a planar graph and press done.");
  leda_wait(1.0);
  GW.message("");

  while (GW.edit()) 
  {
    GW.set_flush(false);

    graph& G = GW.get_graph();

    list<edge> bi_edges;
    Make_Connected(G,bi_edges);
    Make_Bidirected(G,bi_edges);

    if (!PLANAR(G,true))
    { GW.message("\\bf Graph must be planar !");
      leda_wait(1.5);
      GW.message("");
      continue;
     }

    list<node> high_deg;
    list<edge> straight;

    node v;
    forall_nodes(v,G)
       if (G.outdeg(v) > 4) high_deg.append(v);

   list<node> dummy_nodes;

    while (!high_deg.empty())
    { 
      node v = high_deg.pop();

      if (G.outdeg(v) < 5) continue;

      int d = G.outdeg(v)/2;

      node u = G.new_node();
      dummy_nodes.append(u);

      edge e = G.first_adj_edge(v);
      edge x = G.new_edge(v,u);
      edge y = G.new_edge(u,v);
      G.set_reversal(x,y);

      straight.append(x);
      straight.append(y);
      bi_edges.append(y);

      forall_adj_edges(e,v)
      { if (d-- <= 0) break;
        edge r = G.reversal(e);
        G.move_edge(r,source(r),u);
        G.move_edge(e,u,target(e));
       }

      if (G.outdeg(u) > 4) high_deg.append(u);
      if (G.outdeg(v) > 4) high_deg.append(v);
     }

    G.del_edges(bi_edges);

    GW.update_graph();

    forall(v,dummy_nodes)
    { GW.set_label_type(v,no_label);
      GW.set_color(v,grey2);
     }

    if (!PLANAR(G))
    { GW.message("\\bf Graph is not planar !");
      GW.edit();
      return 1;
     }


    bi_edges.clear();
    Make_Connected(G,bi_edges);
    Make_Bidirected(G,bi_edges);
    G.make_planar_map();

    node_array<double> xpos(G);
    node_array<double> ypos(G);
    edge_array< list<double> > xbends(G);
    edge_array< list<double> > ybends(G);
    edge_array<int> bound(G,MAXINT);

    edge e;
    forall(e,straight)
    { edge r = G.reversal(e);
      bound[e] = 0;
      bound[r] = 0;
     }

    ORTHO_EMBEDDING(G, bound, xpos, ypos, xbends, ybends);

    G.del_edges(bi_edges);

    GW.update_graph();

    forall(e,straight) GW.set_color(e,red);

    GW.adjust_coords_to_win(xpos,ypos,xbends,ybends);
    GW.set_flush(true);
    GW.set_layout(xpos,ypos,xbends,ybends);
  }

  return 0;
}
