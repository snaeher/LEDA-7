/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _five_color.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/array.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

static void  find_independent_neighbors(const graph& G, 
                                        const node_array<int>& col,
                                        node v, node& u, node& w)
{ 
  array<node> A(G.degree(v));

  int d = 0;
  node x;
  forall_adj_nodes(x,v)
      if (col[x] != -1) A[d++] = x;

  bool indep = false;

  for(int i=0; i<d; i++)
  { u = A[i];
    for(int j=i+1; j<d; j++)
    { w = A[j];
      indep = true;
      forall_adj_nodes(x,w) 
      { if (x == u) 
        { indep = false;
          break;
         }
       }
      if (indep) i = j = d;
     }
   }

  if (!indep)
    LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (no independent neighbors).");
}



static int unused_adj_colors(node v, const node_array<int>& col)
{ int used[6];
  int c;
  for(c = 0; c < 6; c++) used[c] = 0;
  node x;
  forall_adj_nodes(x,v)
  { c = col[x];
    if (c != -1) used[c] = 1;
   }
  c = 0; 
  while(used[c]) c++;
  return c;
 }



void FIVE_COLOR(graph& G, node_array<int>& C)
{
/*
  if (!PLANAR(G))
    LEDA_EXCEPTION(1,"FIVE_COLOR: graph must be planar.");
*/

  // we work on a copy G1 of G

  graph G1 = G;

  // remove loops and parallel edges
  Delete_Loops(G1);
  Make_Simple(G1);

  // eliminated reversals
  edge_array<edge> rev(G1);
  Is_Bidirected(G1,rev);
  edge e;
  forall_edges(e,G1)
  { if (rev[e] == nil) continue;
    edge r = rev[e];
    G1.del_edge(e);
    rev[r] = nil;
   }
   

  G1.make_undirected();


  node_array<int>  C1(G1,0);       // C1[v] = color of node v in G1
  node_array<bool> mark(G1,false);
  node_array<int>  deg(G1);        // deg[v] = current degree of v

  list<node> small_deg;            // list of nodes with deg[v] <= 5
  node_array<list_item> I(G1,nil); // I[v] = location of v in small_deg

  node_array<list<node> > L(G1);   // L[v] = list of nodes of G represented by v

  stack<node>  removed;            // stack of (conceptually) removed nodes

  int N;                           // current number of valid nodes of G1


  // Initialization

  N = G1.number_of_nodes();

  node u,v,w=0,x;

  u = G.first_node();

  forall_nodes(v,G1)
  { deg[v] = G1.degree(v);
    if(deg[v] <= 5) I[v] = small_deg.append(v);
    L[v].append(u);
    u = G.succ_node(u);
   }

  // shrinking G1

  while (N > 0)
  {

//
//  forall_nodes(v,G1)
//  { int d = 0;
//    forall_adj_nodes(x,v) if (C1[x] != -1) d++;
//    assert(C1[v] == -1 || deg[v] == d); 
//   }
//


    if (small_deg.empty())
     LEDA_EXCEPTION(1,"FIVE_COLOR: no node with degree < 5 (graph not planar).");

    v = small_deg.pop();

    I[v] = nil;

    if (deg[v] == 5)
    {
      find_independent_neighbors(G1,C1,v,u,w);

      if (w == u) 
      { // parallel edges ?
        LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (merging identical nodes)");
       }

      forall_adj_nodes(x,u) mark[x] = true;

      forall_adj_nodes(x,w)
      { if (x == u) 
         LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (merging adjacent nodes)");
        if (mark[x]) 
           { deg[x]--;
             if (deg[x] == 5) 
                I[x] = small_deg.append(x);
            }
        else
           { G1.new_edge(u,x);
             if (C1[x] != -1) deg[u]++;  
            }
       }

      forall_adj_nodes(x,u) mark[x] = false;

      deg[v]--;


      if (deg[u] > 5 && I[u] != nil)
      { small_deg.del(I[u]);
        I[u] = nil;
       }

      L[u].conc(L[w]);
     
      if (I[w] != nil) small_deg.del(I[w]);

      G1.del_node(w);

      N--;

    }

    //  now deg[v] <= 4

    C1[v] = -1;
    removed.push(v);

    forall_adj_nodes(x,v)
       if ( --deg[x] == 5) 
         I[x] = small_deg.append(x);

    N--;
   }

   // now color the nodes in "removed" from top to bottom

   while ( ! removed.empty() )
   { v = removed.pop();

     int c = unused_adj_colors(v,C1);

     if (c == 5) 
       LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (more than 5 colors).");

     C1[v] = c;
     forall(x,L[v]) C[x] = c;

    }

}


LEDA_END_NAMESPACE
