/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mcb_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/string.h>

LEDA_BEGIN_NAMESPACE

static bool return_false(string s)
{ cerr << "CHECK_MCB: " + s +"\n"; return false; }

bool CHECK_MCB(const graph& G,const list<edge>& M, 
                 const node_array<bool>& NC)
{ node v; edge e;
  // check that M is a matching
  node_array<int> deg_in_M(G,0);
  forall(e,M) 
  { deg_in_M[G.source(e)]++;
    deg_in_M[G.target(e)]++;
  }
  forall_nodes(v,G) 
   if ( deg_in_M[v] > 1 ) return_false("M is not a matching");
  // check size(M) = size(NC)
  int K = 0;
  forall_nodes(v,G) if (NC[v]) K++;
  if ( K != M.size() ) return_false("M is smaller than node cover");
  // check that NC is a node cover

  int count = 0;
  forall_edges(e,G) 
    if ( ! (NC[G.source(e)] || NC[G.target(e)]) ) count++;

  if (count > 0)
   return_false(string("NC is not a node cover (%d non-covered edges)",count));

  return true;
}


list<edge> MAX_CARD_BIPARTITE_MATCHING(graph& G, const list<node>& A, 
                                                 const list<node>& B, 
                                                 node_array<bool>& NC)
{ 

  edge_array<int> edge_number(G); 
  int i = 0;
  node v;
  forall_nodes(v,G)  
  { edge e;
    forall_out_edges(e,v) edge_number[e] = i++;
   }

  list<edge> M = MAX_CARD_BIPARTITE_MATCHING_ABMP(G,A,B,NC,false);

   G.sort_edges(edge_number); 

   i = 0;
   forall_nodes(v,G) 
   { edge e;
     forall_out_edges(e,v) assert(edge_number[e] == i++);
    }


  return M; 
}


list<edge> MAX_CARD_BIPARTITE_MATCHING(graph& G, const list<node>& A, 
                                                 const list<node>& B)
{ node_array<bool> NC(G);
  list<edge> M = MAX_CARD_BIPARTITE_MATCHING(G,A,B,NC);
  CHECK_MCB(G,M,NC);
  return M; 
}


list<edge> MAX_CARD_BIPARTITE_MATCHING(graph& G, node_array<bool>& NC)
{ 
  list<node> A,B;

  if ( !Is_Bipartite(G,A,B) )
    LEDA_EXCEPTION(1,"MAX_CARD_BIPARTITE_MATCHING: G is not bipartite");

  list<edge> result = MAX_CARD_BIPARTITE_MATCHING(G,A,B,NC); 

  return result;
}


list<edge> MAX_CARD_BIPARTITE_MATCHING(graph& G)
{ node_array<bool> NC(G);
  return MAX_CARD_BIPARTITE_MATCHING(G,NC);
}


LEDA_END_NAMESPACE
