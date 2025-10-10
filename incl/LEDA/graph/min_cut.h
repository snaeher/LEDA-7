/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  min_cut.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_MINCUT_H
#define LEDA_MINCUT_H

#include <LEDA/graph/graph.h>
#include <LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage{min_cut}{}{Minimum Cut}
}*/

/*{\Mtext 
A cut $C$ in a network is a set of nodes that is neither 
empty nor the entire set of nodes. The weight of a cut is the sum of the 
weights of the edges having exactly one endpoint in $C$.
}*/


// Stoer/Wagner algorithm
extern __exportF int MIN_CUT_SW(const graph& G, const edge_array<int>& weight, 
                                list<node>& C, bool use_heuristic = true);

// new in LEDA-6.2: maxflow algorithm (by sn)
extern __exportF int MIN_CUT_MF(const graph& G, const edge_array<int>& weight, 
                                list<node>& C, bool use_heuristic = true);



extern __exportF int MIN_CUT(const graph& G, const edge_array<int>& weight, 
                             list<node>& C, bool use_heuristic = true);
/*{\Mfuncl
MIN\_CUT takes a graph $G$ and an edge\_array |weight| that gives for 
each edge a \emph{non-negative} integer weight.
The algorithm (\cite{SW94}) computes
a cut of minimum weight. A cut of minimum weight is returned in $C$ 
and the value of the cut is the return value of the function.
The running time is 
$O(nm + n^2\log n)$. The function uses a heuristic to speed up its computation.\\
\precond The edge weights are non-negative.
}*/ 


inline list<node> MIN_CUT(const graph& G, const edge_array<int>& weight)
{ list<node> C;
  MIN_CUT(G,weight,C);
  return C;
}
/*{\Mfunc as above, but the cut $C$ is returned.}*/


extern __exportF int CUT_VALUE(const graph& G,const edge_array<int>& weight, 
const list<node>& C);
/*{\Mfuncl
returns the value of the cut $C$.
}*/ 



LEDA_END_NAMESPACE

#endif

