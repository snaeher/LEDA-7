/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_tam2.c
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

edge two_node(graph& G)
{ node a = G.new_node();
  node b = G.new_node();
  edge e = G.new_edge(a,b);
  edge r = G.new_edge(b,a);
  G.set_reversal(e,r);
  return e;
}

edge eight_node(graph& G)
{
   node a = G.new_node();
   node b = G.new_node();
   node c = G.new_node();
   node d = G.new_node();

   edge e1 = G.new_edge(a,b);
   edge e2 = G.new_edge(b,c);
   edge e3 = G.new_edge(c,d);
   edge e4 = G.new_edge(d,a);
   edge r1 = G.new_edge(b,a);
   edge r2 = G.new_edge(c,b);
   edge r3 = G.new_edge(d,c);
   edge r4 = G.new_edge(a,d);

   G.set_reversal(e1,r1);
   G.set_reversal(e2,r2);
   G.set_reversal(e3,r3);
   G.set_reversal(e4,r4);

   return e1;
}



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

    list<edge> straight;
 
    int clr_count = 3;

    node v;
    forall_nodes(v,G)
    {  
      int d = G.outdeg(v);
 
      if (d <= 4) continue;
 
      edge e;
      int dmax;
 
      if (d > 6)
      { dmax = 2; 
        e  = eight_node(G);
       }
      else
      { dmax = 3; 
        e  = two_node(G);
       }

      list<node> dummy_nodes;
 
      edge x = e;
 
      do { node u = target(x);
           edge y = G.face_cycle_succ(x);
           dummy_nodes.append(u);
 
           if (G.reversal(x) != e)
           { straight.append(x);
             bi_edges.append(G.reversal(x));
            }
 
           for(int i = 0; i < dmax; i++)
           { if (G.outdeg(v) == 0) break;
             edge adj_e = G.first_adj_edge(v);
             edge adj_r = G.reversal(adj_e);
             G.move_edge(adj_r,source(adj_r),u);
             G.move_edge(adj_e,u,target(adj_e));
            }
            x = y;
          } while (x != e);

       GW.update_graph();
       node u;
       forall(u,dummy_nodes) GW.set_color(u,color(clr_count));
       clr_count++;
     }

    forall_nodes(v,G)
        if (G.outdeg(v) == 0) G.del_node(v);

    GW.update_graph();

    edge e;
    forall(e,straight) 
    { GW.set_color(e,red);
      GW.set_width(e,2);
      GW.set_direction(e,undirected_edge);
     }

    gw_spring_embed(GW);
    GW.redraw();
    GW.edit();

    G.del_edges(bi_edges);
    GW.update_graph();
    bi_edges.clear();
    Make_Connected(G,bi_edges);
    Make_Bidirected(G,bi_edges);
    G.make_planar_map();

/*
face f;
forall_faces(f,G) { G.print_face(f); cout << endl; }
cout << endl;
*/

    GW.update_graph();

    node_array<double> xpos(G);
    node_array<double> ypos(G);
    edge_array<list<double> > xbends(G);
    edge_array<list<double> > ybends(G);
    edge_array<int> bound(G,16);

    forall(e,straight)
    { edge r = G.reversal(e);
      bound[e] = 0;
      bound[r] = 0;
      GW.set_color(e,blue);
      GW.set_color(r,blue);
     }

    ORTHO_EMBEDDING(G,bound,xpos,ypos,xbends,ybends);

    G.del_edges(bi_edges);

    GW.update_graph();
    GW.adjust_coords_to_win(xpos,ypos,xbends,ybends);
    GW.set_flush(true);
    GW.set_layout(xpos,ypos,xbends,ybends);
  }

  return 0;
}
