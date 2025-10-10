/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _HK2_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_list.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/core/slist.h>

#include <LEDA/system/assert.h>

using std::cout;
using std::endl;
using std::flush;

LEDA_BEGIN_NAMESPACE

static int bfs(const graph& G,const node_list& free_in_A, 
                              const node_array<bool>& free,
                              node_array<int>& dist,
                              node_array<edge>& pred,
                              int dist_start)
{
  // bfs starting at free nodes in A with initial dist values = dist_start
  // returns the target level (dist value of first level with a free node)
  // (-1 if no free node in B can be reached)
  // Precondition: forall(v,G) dist[v] < dist_start 

  int n = G.number_of_nodes();

  b_queue<node> Q(n);

  node v;
  forall(v,free_in_A)
  { Q.append(v);
    dist[v] = dist_start;
   }

  int target_level = -1; // undefined

  while (!Q.empty())
  { node v = Q.pop();
    int dv = dist[v];

    if (dv == target_level) break;

    edge e;
    forall_out_edges(e,v)
    { node w = G.target(e);

      if (dist[w] >= dist_start ) continue; // reached before

      dist[w] = dv+1; 
      pred[w] = nil;

      if (target_level != -1) continue;

      if (free[w]) 
        target_level = dist[w];
      else
        Q.append(w);

    }
  }

  return target_level;
}



static edge find_augmenting_path(const graph& G, edge x, 
                                                 const node_array<bool>& free,
                                                 const node_array<int>& dist,
                                                 node_array<edge>& pred)
{ // find augmenting path using dfs

  node v = G.target(x);
  pred[v] = x;

  if (free[v]) return x;

  int d = dist[v] + 1;

  edge e;
  forall_out_edges(e,v)
  { node w = G.target(e);
    if (dist[w] != d || pred[w] != nil) continue;
    edge y = find_augmenting_path(G,e,free,dist,pred);
    if (y) return y;
  }

  return nil;
} 



list<edge> MAX_CARD_BIPARTITE_MATCHING_HK2(graph& G, const list<node>& A, 
                                                     const list<node>& B,
                                                     node_array<bool>& NC,
                                                     bool use_heuristic)
{
  //check that all edges are directed from A to B
  node v;  
  forall(v,B) assert(G.outdeg(v) == 0); 

  node_array<bool> free(G,true);

  if (use_heuristic) 
  { int count = 0;
    edge e;
    forall_edges(e,G)
    { node v = G.source(e);
      node w = G.target(e);
      if (free[v] && free[w])
      { free[v] = false;
        free[w] = false;
        G.rev_edge(e);
        count++;
       }
     }
  } 

 
  node_list free_in_A;
  forall(v,A) if (free[v]) free_in_A.append(v);

  node_array<int>  dist(G,-1);
  node_array<edge> pred(G,nil);

  int target_level = -1;

  while ((target_level = bfs(G,free_in_A,free,dist,pred,target_level+1)) != -1) 
  { 
    slist<edge> L;

    node v;
    forall(v,free_in_A)
    { int d = dist[v]+1;
      edge e;
      forall_out_edges(e,v)
      { node w = G.target(e);
        if (dist[w] != d || pred[w] != nil) continue;
        edge x = find_augmenting_path(G,e,free,dist,pred);
        if (x) { L.append(x); break; }
      }
    }

    while (!L.empty())
    { edge e = L.pop(); 
      free[G.target(e)] = false;
      node u;
      while (e)
      { G.rev_edge(e);
        u = G.target(e);
        e = pred[u];
      }
      free[u] = false;
      free_in_A.del(u);
    } 

    // prepare for next phase

    if (target_level > (1<<20))
    { // prevent overflow: reset dist values and target value
      dist.init(G,-1);
      target_level = -1;
     }
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


  // restore original edge orientation
  forall(e,result) G.rev_edge(e);

  return result;
}


LEDA_END_NAMESPACE
