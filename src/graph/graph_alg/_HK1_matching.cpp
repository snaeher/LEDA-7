/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _HK1_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h> 
#include <LEDA/system/assert.h>

#include <LEDA/core/b_queue.h>
#include <LEDA/graph/node_list.h>

#ifndef LEDA_CHECKING_OFF
LEDA_CHECKING_OFF NOT DEFINED
#endif

LEDA_BEGIN_NAMESPACE

static int dist_start = 0; 
static int dist_max = 0;

static bool bfs(const graph& G,const node_list& free_in_A, 
                               const node_array<bool>& free,
                               node_array<int>& dist,
                               node_array<edge>& pred)       
{
  int n = G.number_of_nodes();

  b_queue<node> Q(n);

  node v;
  forall(v,free_in_A)
  { Q.append(v);
    dist[v] = dist_start;
   }

  bool augmenting_path_found = false;

  while (!Q.empty())
  { node v = Q.pop();
    int d = dist[v]+1;

    if (d > dist_max) dist_max = d;

    edge e;
    forall_out_edges(e,v)
    { node w = G.target(e);

      if (dist[w] >= dist_start ) continue; // reached before

      dist[w] = d; 
      pred[w] = nil;

      if (free[w]) augmenting_path_found = true;

      if (!augmenting_path_found) Q.append(w);
    }
  }

  return augmenting_path_found;
}



static edge find_augmenting_path(const graph& G, edge x, 
                                                 const node_array<bool>& free,
                                                 const node_array<int>& dist,
                                                 node_array<edge>& pred)
{ 
  // find augmenting path using dfs

  node v = G.target(x);
  pred[v] = x;

  if (free[v]) return x;

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    if (dist[w] < dist_start || dist[w] != dist[v]+1 || pred[w] != nil) continue;
    edge y = find_augmenting_path(G,e,free,dist,pred);
    if (y) return y;
  }

  return nil;
} 



list<edge> MAX_CARD_BIPARTITE_MATCHING_HK1(graph& G, const list<node>& A, 
                                                     const list<node>& B,
                                                     node_array<bool>& NC,
                                                     bool use_heuristic)
{
  node_array<bool> free(G,true);

  //check that all edges are directed from A to B
  node v;  
  forall(v,B) assert(G.outdeg(v) == 0); 

  if (use_heuristic) 
  { edge e;
    forall_edges(e,G)
    { node v = G.source(e);
      node w = G.target(e);
      if (free[v] && free[w])
      { free[v] = false;
        free[w] = false;
        G.rev_edge(e);
       }
     }
  } 
 
  node_list free_in_A;
  forall(v,A) if (free[v]) free_in_A.append(v);

  node_array<int> dist(G,-1);
  node_array<edge> pred(G,nil);

  while (bfs(G,free_in_A,free,dist,pred))  
  { 
    list<edge> EL;

    node v;
    forall(v,free_in_A)
    { edge e;
      forall_out_edges(e,v)
      { node w = G.target(e);
        if (dist[w] >= dist_start && dist[w] == dist[v]+1 && pred[w] == nil)
        { edge f = find_augmenting_path(G,e,free,dist,pred);
          if (f) { EL.append(f); break; }
         }
      }
    }

    while (!EL.empty())
    { edge e = EL.pop(); 
      free[G.target(e)] = false;
      node z=0;
      while (e)
      { G.rev_edge(e);
        z = G.target(e);
        e = pred[z];
      }
      free[z] = false;
      free_in_A.del(z);
    } 


    // prepare for next phase
    dist_start = dist_max+1;
  } 
    
  
  list<edge> result;

  forall(v,B) {
    edge e;
    forall_out_edges(e,v) result.append(e);
  }


  // compute vertex cover NC

  forall_nodes(v,G) NC[v] = false;
  node_array<bool> reachable(G,false);

  forall(v,A) {
    if (free[v]) DFS(G,v,reachable);
  }

  edge e;
  forall(e,result)
  { node v = G.source(e);
    node w = G.target(e);
    if (reachable[v])
      NC[v] = true;
    else
      NC[w] = true;
   }


  // restore original edge directions
  forall(e,result) G.rev_edge(e);

  return result;

}

LEDA_END_NAMESPACE

