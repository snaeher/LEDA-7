/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _visrep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graph/graph_alg.h>
#include<LEDA/graph/face_map.h>


LEDA_BEGIN_NAMESPACE

static int compute_depth(const graph& G, node s, node_array<int>& depth, 
                                                                  int offset=0)
{
  // computes the depth of each node v with respect to node s. The depth
  // of v is the length of the longest path from s to v. The offset is added
  // to all depths. The maximal depth is returned.
  // PRECONDITION: s is the single source of the acyclic graph G.
 
  node_array<int>  degree(G);
  list<node>       zero_deg;
  node v;
  edge e;

  int max_depth = offset;
 
  forall_nodes(v,G)
  {
    depth[v] = offset;
    degree[v] = G.indeg(v);
  }

  zero_deg.append(s);
 
  while(!zero_deg.empty())
  {
    v = zero_deg.pop();
    forall_adj_edges(e,v)
    {
      int  d = depth[v]+1;
      node w = G.target(e);
      if (depth[w] < d) 
      {
        depth[w] = d;
        if(max_depth < d) max_depth = d;
      }
      if(--degree[w] == 0) zero_deg.append(w);
    }
  }

  return max_depth;
}



static int visibility_rep(graph& G, node_array<int>& leftx, 
                                    node_array<int>& rightx, 
                                    node_array<int>& ypos,
                                    edge_array<int>& xpos, 
                                    edge_array<int>& lowy, 
                                    edge_array<int>& topy)
{
  // computes a visibility representation of the graph G, that is, each node is
  // represented by a horizontal segment (or box) and each edge is represented
  // by a vertical segment. We store the left and right x-coordinate
  // of the segments representing nodes in the arrays leftx and rightx.
  // We store the y-coordinate of a node segment in ypos. We store the
  // lower and upper y-coordinate of edges in lowy and topy. We store the
  // the x-coordinate of an edge segment in xpos.
  //
  // Precondition: 
  // G is simple, biconnected, planar and contains at least three nodes.


  graph D;                           // dual graph
  node v,s,t;
  edge e;
  face f;

  face_map<node> corr_node(G);       // maps between primal faces
  node_map<face> corr_face(D);       // and dual nodes
  edge_map<edge> corr_dual(G);       // maps between primal and
  edge_map<edge> corr_primal(D);     // dual edges

  edge_map<face> edge_left_face(G);  // left face of a primal edge
  node_map<face> node_left_face(G);  // left face of a primal node
  node_map<face> node_right_face(G); // right face of a primal node
                // left and right are according to st-orientation

  // make graph bidirected
  list<edge> bd_edges = Make_Bidirected(G);
  edge_array<bool> is_bd_edge(G,false);
  forall(e,bd_edges) is_bd_edge[e] = true;

  // compute embedding and dual graph
  G.make_planar_map();

  forall_faces(f,G)
  {
    node d = D.new_node();
    corr_node[f] = d;
    corr_face[d] = f;
  }

  forall_edges(e,G)
  {
    face f1 = G.adj_face(e);
    face f2 = G.adj_face(G.reversal(e));
    if( (!is_bd_edge[e]) && (f1 != f2) )
    {
      edge d =
      D.new_edge(corr_node[f1],corr_node[f2]);
      corr_dual[e] = d;
      corr_primal[d] = e;
    }
    else corr_dual[e] = nil;   // e is a bridge
  }

  // compute primal ST-numbering
  node_array<int> st_num(G);
  list<node> st_list;
  ST_NUMBERING(G,st_num,st_list);

  // find s,t and (s,t)
  s = st_list.pop();
  t = st_list.Pop();
  edge st = is_bd_edge[G.first_adj_edge(t)] ?
              G.reversal(G.first_adj_edge(t)) : G.first_adj_edge(t);

  // compute edge_left_face, node_left_face, node_right_face
  forall_faces(f,G)
  {
    forall_face_edges(e,f)
    {
      edge succ = G.face_cycle_succ(e);
      int e_max = leda_max(st_num[source(e)],st_num[target(e)]);
      int e_min = leda_min(st_num[source(e)],st_num[target(e)]);
      int s_max = leda_max(st_num[source(succ)],st_num[target(succ)]);
      int s_min = leda_min(st_num[source(succ)],st_num[target(succ)]);
      if(e_max == s_min) node_left_face[target(e)] = f;
      if(e_min == s_max) node_right_face[target(e)] = f;

      if(!is_bd_edge[e] && (e_min == st_num[source(e)]))
        edge_left_face[e] = f;
      else
        edge_left_face[e] = G.adj_face(G.reversal(e));
    }
  }
  if(source(st) == t)
  {
    node_left_face[t] = node_left_face[s] = G.adj_face(G.reversal(st));
    node_right_face[t] = node_right_face[s] = G.adj_face(st);
  }
  else
  {
    node_right_face[t] = node_right_face[s] = G.adj_face(G.reversal(st));
    node_left_face[t] = node_left_face[s] = G.adj_face(st);
  }

  // remove auxiliary edges
  G.del_edges(bd_edges);


  // orient all edges according to the st-numbering
  list<edge> reversed_edges;
  forall_edges(e,G)
    if(st_num[target(e)] < st_num[source(e)]) 
    { G.rev_edge(e);
      reversed_edges.append(e);
     }

  // compute primal depths
  node_array<int> primal_depth(G);
  compute_depth(G,s,primal_depth);

  // orient dual edges
  forall_edges(e,D)
  {
    edge p = corr_primal[e];
    if(corr_node[edge_left_face[p]] != source(e)) D.rev_edge(e);
  }
  D.rev_edge(corr_dual[st]);

  // compute dual depths
  node_array<int> dual_depth(D);
  compute_depth(D,corr_node[node_right_face[s]],dual_depth);

  // compute left, right, top, bottom
  forall_edges(e,G)
  {
    xpos[e] = dual_depth[corr_node[edge_left_face[e]]];
    lowy[e] = primal_depth[source(e)];
    topy[e] = primal_depth[target(e)];
  }
  forall_nodes(v,G)
  {
    int xmin = D.number_of_nodes()+1;
    int xmax = -1;

    // search for leftmost and rightmost edge of v
    forall_inout_edges(e,v)
    {
      int eleft = dual_depth[corr_node[edge_left_face[e]]];
      if(eleft < xmin) xmin = eleft;
      if(eleft > xmax) xmax = eleft;
    }

    leftx[v] = xmin;
    rightx[v] = xmax;
    ypos[v] = primal_depth[v];
  }

  forall(e,reversed_edges) G.rev_edge(e);

  return leda_max(primal_depth[t],dual_depth[corr_node[node_left_face[s]]]);
}



bool VISIBILITY_REPRESENTATION(graph& G, node_array<double>& x_pos,
                                         node_array<double>& y_pos,
                                         node_array<double>& x_rad,
                                         node_array<double>& y_rad,
                                         edge_array<double>& x_sanch,
                                         edge_array<double>& y_sanch,
                                         edge_array<double>& x_tanch,
                                         edge_array<double>& y_tanch)
{
    if (G.empty()) return true;

    if (G.number_of_nodes() < 3)
    { node u = G.first_node();
      node v = G.last_node();
      y_pos[u] = 1;
      y_pos[v] = 0;
      x_pos[u] = x_pos[v] = 0;
      x_rad[u] = x_rad[v] = 1;
      y_rad[u] = y_rad[v] = 0.25;
      x_sanch.init(G,0);
      y_sanch.init(G,0);
      x_tanch.init(G,0);
      y_tanch.init(G,0);
      return true;
    }


    if (!PLANAR(G))
    { LEDA_EXCEPTION(0,"visibility_rep: Graph is not planer");
      return false;
     }

    list<edge> el;
    if (!Is_Simple(G,el))
    { edge x = nil;
      edge e;
      forall(e,el)
      { if (x && source(x) == source(e) && target(x) == target(e)) 
          G.hide_edge(e);
        else 
          x = e;
      }
    }

    list<edge> bi_edges;
    Make_Biconnected(G,bi_edges);

    node_array<int> leftx(G);
    node_array<int> rightx(G);
    node_array<int> ypos(G);
    edge_array<int> lowy(G);
    edge_array<int> topy(G);
    edge_array<int> xpos(G);
  
    //nt max_c = 
    visibility_rep(G,leftx,rightx,ypos,xpos,lowy,topy);

    G.del_edges(bi_edges);

    node v;
    forall_nodes(v,G)
    { double r = double(rightx[v] - leftx[v])/2;
      double x = double(rightx[v] + leftx[v])/2;
      x_rad[v] = r + 0.25;
      y_rad[v] = 0.25;
      x_pos[v] = x;
      y_pos[v] = ypos[v];
    }

    edge e;
    forall_edges(e,G)
    { node v = source(e);
      node w = target(e);
      x_sanch[e] = (xpos[e] - x_pos[v])/x_rad[v];
      y_sanch[e] = +1;
      x_tanch[e] = (xpos[e] - x_pos[w])/x_rad[w];
      y_tanch[e] = -1;
     }

     edge x = nil;
     forall(e,el)
     { if (x == nil || source(x) != source(e) || target(x) != target(e)) 
       { x = e;
         continue;
        }
       G.restore_edge(e);
       node v = source(e);
       node w = target(e);
       x_sanch[e] = x_sanch[x] + 0.1/x_rad[v];
       y_sanch[e] = y_sanch[x];
       x_tanch[e] = x_tanch[x] + 0.1/x_rad[w];
       y_tanch[e] = y_tanch[x];

	   /* Modification Thomas Ziegler 2001-04-17. Problem: 
	   loop does not work correctly for more than two parallel edges*/

	   x=e;

	   /*end of modification*/

      }

  return true;
}

LEDA_END_NAMESPACE
