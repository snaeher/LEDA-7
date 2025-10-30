/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  spanning_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/node_partition.h>
#include <LEDA/core/quicksort.h>
#include <LEDA/core/tuple.h>


LEDA_BEGIN_NAMESPACE

// new implementation of MIN_SPANNING_TREE (s.n.) 

typedef three_tuple<COORD,node,node> node_pair;

int compare(const node_pair& x, const node_pair& y)
{ return compare(x.first(),y.first()); }



void MIN_SPANNING_TREE(const list<POINT>& L, GRAPH<POINT,int>& T)
{ 
  DELAUNAY_TRIANG(L,T);

  node_partition P(T);

  int n = T.number_of_edges()/2;
  int i = 0;

  array<node_pair> A(n);

  edge e;
  forall_edges(e,T)
  { edge r = T.reversal(e);
    if (T[r] == MAXINT) continue; 
    T[e] = MAXINT;
    node v = source(e);
    node w = target(e);
    A[i].first() = T[v].sqr_dist(T[w]);
    A[i].second() = v;
    A[i].third() = w;
    i++;
   }

  A.sort();

  T.del_all_edges();

  for(i=0; i<n; i++)
  { node v = A[i].second();
    node w = A[i].third();
    if (!P.same_block(v,w))
    { P.union_blocks(v,w);
      T.new_edge(v,w,0);
     }
   }
}



// old implementation

extern list<edge> MIN_SPANNING_TREE(const graph&, 
                                    int(*cmp)(const edge&,const edge&));

static const GRAPH<POINT,int>* T_tmp;

static int cmp_edge_length(const edge& e1, const edge& e2)
{ COORD l1 = T_tmp->inf(source(e1)).sqr_dist(T_tmp->inf(target(e1))); 
  COORD l2 = T_tmp->inf(source(e2)).sqr_dist(T_tmp->inf(target(e2))); 
  return compare(l1,l2);
 }
  
void MIN_SPANNING_TREE_OLD(const list<POINT>& L, GRAPH<POINT,int>& T)
{ DELAUNAY_TRIANG(L,T);
  edge_array<bool> tree_edge(T,false);
  list<edge> E = MIN_SPANNING_TREE(T,cmp_edge_length);
  edge e;
  forall(e,E) 
  { tree_edge[e] = true;
    T[e] = 0;
   }
  E = T.all_edges();
  forall(e,E) 
     if (!tree_edge[e]) T.del_edge(e);
}



LEDA_END_NAMESPACE
