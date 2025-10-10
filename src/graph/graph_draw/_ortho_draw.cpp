/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ortho_draw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>
#include <LEDA/core/array2.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE


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
        LEDA_EXCEPTION(1,"grid_graph: missing reversals");

  return G.last_adj_edge(A(0,0));
}




bool ORTHO_DRAW(const graph& G0, node_array<double>& xpos,
                                 node_array<double>& ypos,
                                 node_array<double>& xrad,
                                 node_array<double>& yrad,
                                 edge_array<list<double> >& xbends,
                                 edge_array<list<double> >& ybends,
                                 edge_array<double>& xsanch,
                                 edge_array<double>& ysanch,
                                 edge_array<double>& xtanch,
                                 edge_array<double>& ytanch)
{

  int n = G0.number_of_nodes();

  if (n == 0) return true;

  if (n == 1)
  { node v = G0.first_node();
    xpos[v] = ypos[v] = 0;
    xrad[v] = yrad[v] = 10;
    return true;
   }

  if (n == 2)
  { node v = G0.first_node();
    node w = G0.last_node();
    xpos[v] = ypos[v] = 0;
    xrad[v] = yrad[v] = 10;
    xpos[w] = 0;
    ypos[w] = 100;
    xrad[w] = yrad[w] = 10;
    return true;
   }
    



  GRAPH<node,edge> G;
  node_array<node> v_copy(G0);
  edge_array<edge> e_copy(G0);

  CopyGraph(G,G0);

  node v;
  forall_nodes(v,G) v_copy[G[v]] = v;
  edge e;
  forall_edges(e,G) e_copy[G[e]] = e;

  node_map<node> v_next(G,nil);

  Make_Connected(G);
  Make_Bidirected(G);

  if (!PLANAR(G,true))
     LEDA_EXCEPTION(1,"ORTHO_DRAW: graph must be planar.");


  list<edge> straight;

  forall_nodes(v,G)
  {  
    int d = G.outdeg(v);

    if (d <= 4) continue;

    node v_orig = G[v];
    int  lx = 2;
    int  ly = 1;

    if (d > 6)
    { int r = ((d-8) + 1)/2;
      ly = 2 + r/2;
      lx = 2 + r - r/2;
     }

    edge e = grid_graph(G,lx,ly,v_orig);

    edge x = e;
    do { node u = source(x);
         edge y  = G.face_cycle_pred(x);

         v_next[target(x)] = u;

         if (G.reversal(x) != e) straight.append(x);

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

  int genus = Genus(G);

  if (genus != 0) 
  { //LEDA_EXCEPTION(0,"ortho_draw: genus != 0");
    LEDA_EXCEPTION(0,string("ortho_draw: genus = %d",genus));
    return false;
   }

  G.compute_faces();

  node_array<double> xpos1(G);
  node_array<double> ypos1(G);
  edge_array<list<double> > xbends1(G);
  edge_array<list<double> > ybends1(G);
  edge_array<int> bound(G,16);

  node_array<bool> crossing(G,false);

  forall_nodes(v,G)
  { node v_orig = G[v];
    crossing[v] = xpos[v_orig] == -1;
   }

  forall(e,straight)
  { edge r = G.reversal(e);
    bound[e] = bound[r] = 0;
   }

  if (ORTHO_EMBEDDING(G,crossing,bound,xpos1,ypos1,xbends1,ybends1) < 0)
    return false;

  forall_nodes(v,G0)
  { node u = v_copy[v];
    double xmin =  MAXINT;
    double ymin =  MAXINT;
    double xmax = -MAXINT;
    double ymax = -MAXINT;
    while (u)
    { double x = xpos1[u];
      double y = ypos1[u];
      if (x > xmax) xmax = x;
      if (x < xmin) xmin = x;
      if (y > ymax) ymax = y;
      if (y < ymin) ymin = y;
      u = v_next[u];
     }
    xpos[v] = (xmin + xmax)/2;
    ypos[v] = (ymin + ymax)/2;
/*
    xrad[v] = 0.25 + (xmax - xmin)/2;
    yrad[v] = 0.25 + (ymax - ymin)/2;
*/
    xrad[v] = 0.15 + (xmax - xmin)/2;
    yrad[v] = 0.15 + (ymax - ymin)/2;
  }

  forall_edges(e,G0)
  { xbends[e] = xbends1[e_copy[e]];
    ybends[e] = ybends1[e_copy[e]];
   }

  forall_nodes(v,G)
  { node v_orig = G[v];
    double r1 = xrad[v_orig];
    double r2 = yrad[v_orig];
    forall_out_edges(e,v)
    { if (G[e] == nil) continue;
      xsanch[G[e]] = (xpos1[v] - xpos[v_orig])/r1;
      ysanch[G[e]] = (ypos1[v] - ypos[v_orig])/r2;
     }
    forall_in_edges(e,v)
    { if (G[e] == nil) continue;
      xtanch[G[e]] = (xpos1[v] - xpos[v_orig])/r1;
      ytanch[G[e]] = (ypos1[v] - ypos[v_orig])/r2;
     }
   }

  return true;
}

LEDA_END_NAMESPACE
