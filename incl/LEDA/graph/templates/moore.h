/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  moore.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721216
#include <LEDA/internal/PREAMBLE.h>
#endif

//------------------------------------------------------------------------------
// MOORE  (single source shortest paths)
//
// last modified: 1997)
//------------------------------------------------------------------------------
 
#include <LEDA/graph/graph_alg.h>
#include <limits>

LEDA_BEGIN_NAMESPACE

template <class NT>
void  MOORE_T(const graph& g, node s, const edge_array<NT>& cost,
                                            node_array<NT>& dist, node t) 
{
  // use a double ended queue of candidate nodes (node_list)
  // and a node_array<NT> for dist values
  // stop when t reached (if t != nil)

  int max_num = std::numeric_limits<int>::max();

  //dist.init(g,max_num);

  node v;
  forall_nodes(v,g) dist[v] = max_num;
  dist[s] = 0;

  node_list labeled;  // deque of candidate nodes
  labeled.append(s);

  while (! labeled.empty()) 
  { 
    node v = labeled.pop();
    NT dv = dist[v];

    if (t && dv > dist[t]) continue;

    edge e;
    forall_adj_edges(e,v)
    { node w = g.opposite(v,e);
      NT d = dv + cost[e];
      if (d < dist[w]) 
      { if ( ! labeled(w) ) 
        { if (dist[w] == max_num)
	       labeled.append(w);
	    else
	       labeled.push(w);
	   }
	  dist[w] = d;
       }
     }
  }
}

#if LEDA_ROOT_INCL_ID == 721216
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

LEDA_END_NAMESPACE

