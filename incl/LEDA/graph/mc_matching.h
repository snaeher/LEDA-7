/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mc_matching.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#ifndef LEDA_MC_MATCHING
#define LEDA_MC_MATCHING

#include <LEDA/graph/graph.h>
#include <LEDA/graph/mc_matching_gabow.h>

LEDA_BEGIN_NAMESPACE

/*{\Manpage{mc_matching}{}{Maximum Cardinality Matchings in General Graphs}}*/

/*{\Mtext


A \emph{matching} in a graph $G$ is a subset $M$ of the 
edges of $G$ such that no two share an endpoint. 

An odd-set cover |OSC| of $G$ is a labeling of the nodes of $G$ with
non-negative integers such that every edge of $G$ (which is not a self-loop) 
is either incident to a node labeled $1$ or 
connects two nodes labeled with the same $i$, $i \ge 2$. 

Let $n_i$ be the number of nodes labeled $i$ and consider any matching
$N$. For $i$, $i \ge 2$, let $N_i$ be the edges in $N$ that connect 
two nodes labeled $i$.
Let $N_1$ be the remaining edges in $N$. Then 
$\Labs{N_i} \le \lfloor n_i/2 \rfloor$ and $\Labs{N_1} \le n_1$
and hence
\[ \Labs{N} \le n_1 + \sum_{i \ge 2} \lfloor n_i/2 \rfloor \]
for any matching $N$ and any odd-set cover |OSC|.

It can be shown that for a maximum cardinality matching $M$
there is always an odd-set cover |OSC| with 
\[ \Labs{M} = n_1 + \sum_{i \ge 2} \lfloor n_i/2 \rfloor, \]
thus proving the optimality of $M$. In such a cover all $n_i$ with $i \ge 2$
are odd, hence the name.

\settowidth{\typewidth}{|list<edge>|}
\addtolength{\typewidth}{\colsep}
\settowidth{\callwidth}{MAX}
\computewidths
}*/




extern __exportF list<edge> MAX_CARD_MATCHING_EDMONDS(const graph& G, 
                                                  node_array<int>& OSC, 
                                                  int heur=0);
/*{\Mfunc computes a maximum cardinality matching $M$ in a general graph $G$ 
and returns it as a list of edges. The original algorithm was developed
by Edmond in \cite{E65}. An efficient implementation was presented
by Gabow and Edmond in \cite{Gabow: Edmonds}. It has running 
time $O(nm\cdot\alpha(n,m))$. With $|heur| = 1$ the algorithm
 uses a greedy heuristic to find an initial matching.
An odd-set cover that proves the maximality of $M$ is returned in |OSC|.
}*/

extern __exportF list<edge> MAX_CARD_MATCHING_KECECIOGLU(const graph& G, 
                                                      node_array<int>& OSC,
                                                      int heur=0);
/*{\Mfunc a variant of Gabow/Edmond's algorithm using an heuristic
 proposed by J. Kececioglu and J. Pecquer. }*/


extern __exportF list<edge> MAX_CARD_MATCHING_KECECIOGLU1(const graph& G, 
                                                      node_array<int>& OSC, 
                                                      int heur=0);


inline list<edge> MAX_CARD_MATCHING_GABOW(const graph& G, node_array<int>& OSC)
{ G_card_matching CM(G);
  int num_phases = 0;
  return CM.solve(OSC,num_phases);
}
/*{\Mfunc a new algorithm by Gabow (\cite{Gab17}) with running time 
$O(\sqrt{n}\cdot m)$. The implementation was done by Ansaripour,Danaei 
and Mehlhorn (\cite{ADM24}). }*/



extern __exportF bool CHECK_MAX_CARD_MATCHING(const graph& G, 
                                    const list<edge>& M,
                                    const node_array<int>& OSC);
/*{\Mfunc checks whether |M| is a maximum cardinality matching in $G$ and
|OSC| is a proof of optimality. Aborts if this is not the case.}*/



inline list<edge> MAX_CARD_MATCHING(const graph& G, 
                                    node_array<int>& OSC, 
                                    int heur = 1)
{ 
  return MAX_CARD_MATCHING_GABOW(G,OSC);
}

/*{\Mfunc computes a maximum cardinality matching in a general $G$
and an odd-set cover |OSC| by calling |MAX_CARD_MATCHING_GABOW|. }*/


inline list<edge> MAX_CARD_MATCHING(const graph& G, int heur = 0)
{ node_array<int> OSC(G);
  return MAX_CARD_MATCHING_GABOW(G,OSC);
}
/*{\Mfunc as above, but no proof of optimality is returned. }*/




LEDA_END_NAMESPACE

#endif

