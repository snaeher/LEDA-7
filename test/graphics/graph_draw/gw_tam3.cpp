/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_tam3.c
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
#include <LEDA/core/array2.h>
#include <assert.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cin;
using std::endl;
#endif


static edge grid_graph(GRAPH<node,edge>& G, int n, int m, node v_orig) 
{
  array2<node> A(n,m);
  int x,y;
  for(y=0; y<m; y++)
    for(x=0; x<n; x++)
        A(x,y) = G.new_node(v_orig);

  for(x=0; x<n; x++)
    for(y=0; y<m; y++)
       { node u = A(x,y);
         if (x < n-1) G.new_edge(u, A(x+1,y), edge(nil));
         if (y < m-1) G.new_edge(u, A(x,y+1), edge(nil));
         if (x > 0)   G.new_edge(u, A(x-1,y), edge(nil));
         if (y > 0)   G.new_edge(u, A(x,y-1), edge(nil));
        }

  if (!G.make_map()) 
        error_handler(1,"grid_graph: missing reversals");

  return G.last_adj_edge(A(0,0));
}



void show_faces(GraphWin& gw)
{
  graph& G = gw.get_graph();

  G.compute_faces();

  int c = 2;
  face f;
  forall_faces(f,G)
  { edge e;
    forall_face_edges(e,f) gw.set_color(e,color(c));
    c++;
   }

  node v;
  forall_nodes(v,G)
  { edge e;
    c = 0;
    forall_adj_edges(e,v) gw.set_label(e,string("%d",c++));
  }

  gw.redraw();
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

    graph& G0 = GW.get_graph();

    GRAPH<node,edge> G;
    node_array<node> v_copy(G0);
    edge_array<edge> e_copy(G0);

    CopyGraph(G,G0);

    node v;
    forall_nodes(v,G) v_copy[G[v]] = v;
    edge e;
    forall_edges(e,G) e_copy[G[e]] = e;


    node_map<node> v_next(G,nil);

    list<edge> bi_edges;

    Make_Connected(G);

    Make_Bidirected(G,bi_edges);

    if (!PLANAR(G,true))
    { GW.message("\\bf Graph must be planar !");
      leda_wait(1.5);
      GW.message("");
      continue;
     }

    GraphWin gw1(G);
    gw1.set_edge_label_pos(central_pos);
    gw1.display(0,0);
    gw1.set_flush(false);
    forall_nodes(v,G) 
    { point pos = GW.get_position(G[v]);
      gw1.set_position(v,pos);
     }
    show_faces(gw1);
    gw1.edit();

    list<edge> straight;

    forall_nodes(v,G)
    {  
      int d = G.outdeg(v);
 
      if (d <= 4) continue;

      node v_orig = G[v];
 
      int lx = 2;
      int ly = 1;

      if (d > 6)
      { int r = ((d-8) + 1)/2;
        ly = 2 + r/2;
        lx = 2 + r - r/2;
       }

      edge e = grid_graph(G,lx,ly,v_orig);

      edge x = e;
      do { node u = source(x);
           edge y = G.face_cycle_pred(x);

           v_next[target(x)] = u;
 
           if (G.reversal(x) != e) 
           { straight.append(x);
             bi_edges.append(G.reversal(x));
            }
 
           while (G.outdeg(u) < 4 && G.outdeg(v) > 0)
           { edge adj_e = G.first_adj_edge(v);
             edge adj_r = G.reversal(adj_e);
             node w = target(adj_e);
             G.move_edge(adj_r,w,u);
             G.move_edge(adj_e,x,w);
             x = adj_e;
            }
            x = y;
          } while (x != e);

      v_copy[v_orig] = source(e);
      v_next[target(e)] = nil;

      G.del_node(v);
    }


    node_array<double> xpos(G);
    node_array<double> ypos(G);
    edge_array<list<double> > xbends(G);
    edge_array<list<double> > ybends(G);
    edge_array<int> bound(G,16);


    gw1.update_graph();


    gw1.set_edge_color(black);
    gw1.set_edge_label_type(no_label);

    forall(e,straight)
    { edge r = G.reversal(e);
      bound[e] = bound[r] = 0;
      gw1.set_color(e,blue);
      gw1.set_width(e,2);
     }


    gw_spring_embed(gw1);


    G.compute_faces();

    gw1.redraw();
    gw1.edit();

    if (Genus(G) != 0) error_handler(0,"genus != 0");

G.print();

    show_faces(gw1);
    gw1.edit();

  forall(e,straight) gw1.set_color(e,blue);

  gw1.redraw();


    //G.restore_all_edges();
    //gw1.update_graph();


    if (!Is_Planar_Map(G)) error_handler(1,"G is not a planar map.");

    ORTHO_EMBEDDING(G,bound,xpos,ypos,xbends,ybends);

    gw1.adjust_coords_to_win(xpos,ypos,xbends,ybends);
    gw1.set_layout(xpos,ypos,xbends,ybends);

    leda_wait(1);
    gw1.close();
    GW.redraw();

    node_array<double>        xpos0(G0);
    node_array<double>        ypos0(G0);
    node_array<double>        rad1(G0);
    node_array<double>        rad2(G0);
    edge_array<list<double> > xbends0(G0);
    edge_array<list<double> > ybends0(G0);


    double r1 = GW.get_node_radius1();
    double r2 = GW.get_node_radius2();

    forall_nodes(v,G0)
    { node u = v_copy[v];
      double xmin =  MAXINT;
      double ymin =  MAXINT;
      double xmax = -MAXINT;
      double ymax = -MAXINT;
      while (u)
      { double x = xpos[u];
        double y = ypos[u];
        if (x > xmax) xmax = x;
        if (x < xmin) xmin = x;
        if (y > ymax) ymax = y;
        if (y < ymin) ymin = y;
        u = v_next[u];
       }
      xpos0[v] = (xmin + xmax)/2;
      ypos0[v] = (ymin + ymax)/2;
      rad1[v] = r1 + (xmax-xmin)/2;
      rad2[v] = r2 + (ymax-ymin)/2;
     }

    forall_edges(e,G0)
    { xbends0[e] = xbends[e_copy[e]];
      ybends0[e] = ybends[e_copy[e]];
     }

    GW.set_flush(true);
    GW.set_layout(xpos0,ypos0,xbends0,ybends0,false);
    GW.set_flush(false);

    forall_nodes(v,G)
    { node v_orig = G[v];
      double r1 = rad1[v_orig];
      double r2 = rad2[v_orig];
      GW.set_radius1(v_orig,r1);
      GW.set_radius2(v_orig,r2);
      forall_out_edges(e,v)
      { double dx = (xpos[v] - xpos0[v_orig])/r1;
        double dy = (ypos[v] - ypos0[v_orig])/r2;
cout << "source: dx = " << dx << endl;
cout << "source: dy = " << dy << endl;
        if (G[e]) GW.set_source_anchor(G[e],point(dx,dy));
       }
      forall_in_edges(e,v)
      { double dx = (xpos[v] - xpos0[v_orig])/r1;
        double dy = (ypos[v] - ypos0[v_orig])/r2;
cout << "target: dx = " << dx << endl;
cout << "target: dy = " << dy << endl;
        if (G[e]) GW.set_target_anchor(G[e],point(dx,dy));
       }
     }

    GW.redraw();
  }

  return 0;
}
