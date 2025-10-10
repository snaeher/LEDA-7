/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ABMP_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/b_stack.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/system/assert.h>

#include <LEDA/core/queue.h>

LEDA_BEGIN_NAMESPACE

static int number_of_augmentations;


static bool find_aug_path_by_bfs(graph& G, 
                                   node a, 
                   node_array<bool>& free, 
                   node_array<edge>& pred,
                   node_array<int>&  mark)
{ queue<node> Q; 

  Q.append(a); 
  mark[a] = number_of_augmentations;   

  while ( !Q.empty() )
  { node v = Q.pop();  // v is a node in A

    edge e;
    forall_adj_edges(e,v)
    { node w = G.target(e); // w is a node in B
      if (mark[w] == number_of_augmentations) continue;
      // w has not been reached before in this search
      pred[w] = e; mark[w] = number_of_augmentations;
      if (free[w])
      { // augment path from a to w
        free[w] = free[a] = false;
        while ( w != a) 
        { edge e = pred[w];
          w = G.source(e); 
          G.rev_edge(e); 
        }
        return true;
      }
      // w is not free
      edge f = G.first_adj_edge(w);
      node x = G.target(f);
      pred[x] = f; mark[x] = number_of_augmentations;
      Q.append(x);
    }
  }
  return false;
}
  // for the basic algorithm

edge ce(const node v, const graph& G, 
        const node_array<int>& layer, node_array<edge>& ce_it)
{ 
  edge e = ce_it[v];
  if ( e == nil ) e = G.first_adj_edge(v);
  while (e && layer[G.target(e)] != layer[v] - 1) e = G.adj_succ(e);
  ce_it[v] = e;
  return e;
 }


list<edge> MAX_CARD_BIPARTITE_MATCHING_ABMP(graph& G, 
                            const list<node>& A, 
                            const list<node>& B,
                            node_array<bool>& NC,
                            bool use_heuristic,
                            int Lmax)
{ node v; 

  //check that all edges are directed from A to B
  forall(v,A) assert(G.indeg(v) == 0);
  forall(v,B) assert(G.outdeg(v) == 0);

    node_array<bool> free(G,true);
    node_array<int> layer(G);

    if (use_heuristic) 
    { edge e;
      forall_edges(e,G)
      { node v = G.source(e);
        node w = G.target(e);
        if ( free[v] && free[w] )
        { free[v] = free[w] = false;
          G.rev_edge(e);
         }
       }
     }


    list<node> free_in_A;

    forall(v,B) layer[v] = 0;
    forall(v,A) 
    { layer[v] = 1;
      if (free[v]) free_in_A.append(v);
    }
    int L = 1;


  node_array<edge> ce_it(G,nil); // current edge iterator

  if (Lmax == -1) Lmax = (int)(0.1*::sqrt((double)G.number_of_nodes()));

  b_stack<edge> p(G.number_of_nodes());

  while ( L <= Lmax && free_in_A.size() > 50 * L)
  { node v = free_in_A.pop();
    L = layer[v];

    
    node w = v;
     
    while (true)
    { if ( free[w] && layer[w] == 0 )
      { // breakthrough
        free[w] = free[v] = false;
        while ( !p.empty() )
        {  edge e = p.pop(); 
           
           if (e == ce_it[G.source(e)]) 
             ce_it[G.source(e)] = G.adj_succ(e);

           G.rev_edge(e); 
        } 
        break;
      }
      else
      { edge e = ce(w,G,layer,ce_it);
        if (e)
        { // advance
          p.push(e); 
          w = G.target(e); 
        }
      else
        { // retreat
          layer[w] += 2;
          
          ce_it[w] = nil;
 
          if (p.empty()) 
          { free_in_A.append(w); 
            break; 
          }
          w = G.source(p.pop()); 
        }
      }
    }

  }

  
  node_array<int> mark(G,-1);
  node_array<edge> pred(G);
  number_of_augmentations = 0;
  forall(v,free_in_A)
  { if ( find_aug_path_by_bfs(G,v,free,pred,mark) ) 
     number_of_augmentations++; 
  }


  
  list<edge> result;

  forall(v,B) 
  { edge e;
    forall_adj_edges(e,v) result.append(e);
   }

  forall_nodes(v,G) NC[v] = false;
  node_array<bool> reachable(G,false);

  forall(v,A) {
    if (free[v]) DFS(G,v,reachable);
  }

  edge e;

  forall(e,result) {
    if ( reachable[G.source(e)] )
      NC[G.source(e)] = true;
    else
      NC[G.target(e)] = true;
  }

  forall(e,result) G.rev_edge(e);

  return result;

}

LEDA_END_NAMESPACE
