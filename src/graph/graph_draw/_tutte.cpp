/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _tutte.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// ------------------------------------------------------------- //
// drawing of a graph using Tutte's algorithm                    //
// David Alberts (1996)                                          //
// ------------------------------------------------------------- //

#include <LEDA/graph/graph_alg.h>
#include <LEDA/numbers/vector.h>
#include <LEDA/numbers/matrix.h>


LEDA_BEGIN_NAMESPACE

bool TUTTE_EMBEDDING(const graph& G, const list<node>& fixed_nodes,
                     node_array<double>& xpos, node_array<double>& ypos)
{
  // computes a convex drawing of the graph G if possible. The list
  // fixed_nodes contains nodes with prescribed coordinates already
  // given in pos. The computed node positions of the other nodes are
  // stored in pos, too. If the operation is successful, true is returned.
  // Precondition: pos is valid for G.
 
  node v,w;
  edge e;
  node_array<bool> is_fixed(G,false);
  forall(v,fixed_nodes) is_fixed[v] = true;

  list<node> other_nodes;
  forall_nodes(v,G) if(!is_fixed[v]) other_nodes.append(v);
  node_array<int> ind(G);          // position of v in other_nodes and A
  int i = 0;
  forall(v,other_nodes) ind[v] = i++;

  int n = other_nodes.size();   // #other nodes
  vector coord(n);              // coordinates (first x then y)
  vector rhs(n);                // right hand side
  matrix A(n,n);                // equations

  // initialize non-zero entries in matrix A
  forall(v,other_nodes)
  {
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    {
      // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(!is_fixed[w]) A(ind[v],ind[w]) = one_over_d;
    }
    A(ind[v],ind[v]) = -1;
  }

  if(A.det() == 0) return false;

  // compute right hand side for x coordinates
  forall(v,other_nodes)
  {
    rhs[ind[v]] = 0;
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    {
      // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(is_fixed[w]) rhs[ind[v]] -= (one_over_d*xpos[w]);
    }
  }

  // compute x coordinates
  coord = A.solve(rhs);
  forall(v,other_nodes) xpos[v] = coord[ind[v]];

  // compute right hand side for y coordinates
  forall(v,other_nodes)
  {
    rhs[ind[v]] = 0;
    double one_over_d = 1.0/double(G.degree(v));
    forall_inout_edges(e,v)
    {
      // get second node of e
      w = (v == source(e)) ? target(e) : source(e);
      if(is_fixed[w]) rhs[ind[v]] -= (one_over_d*ypos[w]);
    }
  }

  // compute y coordinates
  coord = A.solve(rhs);
  forall(v,other_nodes) ypos[v] = coord[ind[v]];

  return true;
}

LEDA_END_NAMESPACE

