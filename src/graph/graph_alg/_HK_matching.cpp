/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _HK_matching.c
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

LEDA_BEGIN_NAMESPACE

static int phase_number; 

static bool bfs(graph& /*G*/,const node_list& free_in_A, 
                             const node_array<bool>& free,
                             edge_array<int>& useful,
                             node_array<int>& dist,
                             node_array<int>& reached)
{ 
  list<node> Q;
  node v,w;
  edge e;

  forall(v,free_in_A)
  { Q.append(v);
    dist[v] = 0;  reached[v] = phase_number;
  }
  bool augmenting_path_found = false;

  while (!Q.empty())
  { v = Q.pop();
    int dv = dist[v];
    forall_adj_edges(e,v)
    { w = target(e);
      if (reached[w] != phase_number ) 
      { dist[w] = dv + 1; reached[w] = phase_number;
        if (free[w]) augmenting_path_found = true;
        if (!augmenting_path_found) Q.append(w);
      }
      if (dist[w] == dv + 1) useful[e] = phase_number;
    }
  }
  return augmenting_path_found;
}


static edge find_aug_path(graph& G, edge f, 
                          const node_array<bool>& free,
                          node_array<edge>& pred,        
                          const edge_array<int>& useful)
{ node w = G.target(f);
  pred[w] = f;
  if (free[w]) return f;
  edge e;
  forall_adj_edges(e,w)
  { node z = G.target(e);
    if ( pred[z] != nil || useful[e] != phase_number ) 
      continue;
    edge g  = find_aug_path(G,e,free,pred,useful);
    if ( g ) return g;
  }
  return nil;
} 


list<edge> MAX_CARD_BIPARTITE_MATCHING_HK(graph& G, 
                               const list<node>& A, 
                               const list<node>& B,
                              node_array<bool>& NC,
                         bool use_heuristic)
{ node v;  
  edge e;
  node_array<bool> free(G,true);

  //check that all edges are directed from A to B
  forall(v,B) assert(G.outdeg(v) == 0); 

  if (use_heuristic) { 
                       forall_edges(e,G)
                       { node v = G.source(e);
                         node w = G.target(e);
                         if ( free[v] && free[w] )
                         { free[v] = free[w] = false;
                           G.rev_edge(e);
                         }
                       }
 } 

  node_list free_in_A;
  forall(v,A) if (free[v]) free_in_A.append(v);

  
  edge_array<int> useful(G,0);
  node_array<int> dist(G);
  node_array<int> reached(G,0);
  phase_number = 1; 


  while ( 
          bfs(G,free_in_A,free,useful,dist,reached)
 )  
  { 
    node_array<edge> pred(G,nil);
    list<edge> EL;

    forall(v,free_in_A)
    { forall_adj_edges(e,v)
        if (pred[G.target(e)] == nil && useful[e] == phase_number)
        { edge f = find_aug_path(G,e,free,pred,useful);
          if ( f )  { EL.append(f); break; }
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
    phase_number++;
 }  
    
  
  list<edge> result;

  forall(v,B) 
   forall_adj_edges(e,v) result.append(e);

  forall_nodes(v,G) NC[v] = false;
  node_array<bool> reachable(G,false);
  forall(v,A)
    if (free[v]) DFS(G,v,reachable);
  forall(e,result)
    if ( reachable[G.source(e)] )
      NC[G.source(e)] = true;
    else
      NC[G.target(e)] = true;

  forall(e,result) G.rev_edge(e);

  return result;


}

LEDA_END_NAMESPACE
