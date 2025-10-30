/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _min_spanning.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/core/basic_alg.h>


LEDA_BEGIN_NAMESPACE


template<class NT> 
class cmp_edges : public leda_cmp_base<edge> {
   const edge_array<NT>* edge_cost;
public:

   cmp_edges(const edge_array<NT>& cost) : edge_cost(&cost) {}

   int operator()(const edge& x, const edge& y) const
   { return compare((*edge_cost)[x],(*edge_cost)[y]); }
};

template<class NT> 
class edge_smaller {
   const edge_array<NT>* edge_cost;
public:

   edge_smaller(const edge_array<NT>& cost) : edge_cost(&cost) {}

   bool operator()(const edge& x, const edge& y) const
   { return (*edge_cost)[x] < (*edge_cost)[y]; }
};



template<class NT>
void KRUSKAL(list<edge>& L, const edge_array<NT>& cost, node_partition& P, 
                                                        list<edge>& T,
                                                        NT /*dummy*/)
{ 
  cmp_edges<NT> cmp(cost);
  L.sort(cmp);

/*
  edge_smaller<NT> smaller(cost);
  L.sort(smaller);
*/

  edge e;
  forall(e,L)
  { node v = source(e);
    node w = target(e);
    if (! P.same_block(v,w))
    { T.append(e);
      P.union_blocks(v,w);
     }
   }
}



template <class NT>
list<edge> MIN_SPANNING_TREE_T(const graph& G, const edge_array<NT>& cost)
{ 
  list<edge> T;
  list<edge> L;
  node_partition P(G);
  edge e;

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  if (m == 0) return T;


/* Compute 3n-th biggest edge cost x by SELECT (from basic_alg.h)
 * and sort all edges with cost smaller than x in a list L
 */

  if (m > 3*n)
   { NT* c = new NT[m];
     NT* p = c;
     forall_edges(e,G) *p++ = cost[e];

     NT x = SELECT(c,p-1,3*n);

     delete[] c;

     forall_edges(e,G) 
        if (cost[e] < x) L.append(e);
    }
  else
    L = G.all_edges();


  KRUSKAL(L,cost,P,T,NT(0));


// collect and sort edges still connecting different trees into L
// and run the algorithm on L

  L.clear();

  forall_edges(e,G) 
     if (!P.same_block(source(e),target(e))) L.append(e);

  KRUSKAL(L,cost,P,T,NT(0));

  return T;
}



list<edge> MIN_SPANNING_TREE(const graph& G, const leda_cmp_base<edge>& cmp)
{ list<edge> L = G.all_edges();
  L.sort(cmp);
  node_partition P(G);
  list<edge> T;
  edge e;
  forall(e,L)
  { node v = source(e);
    node w = target(e);
    if (! P.same_block(v,w))
    { T.append(e);
      P.union_blocks(v,w);
     }
   }
  return T;
}


list<edge> MIN_SPANNING_TREE(const graph& G, 
                             int (*cmp_func)(const edge&, const edge&))
{ leda_cmp_base<edge> cmp(cmp_func);
  return MIN_SPANNING_TREE(G,cmp);
}


list<edge> MIN_SPANNING_TREE(const graph& G, const edge_array<int>& cost)
{ return MIN_SPANNING_TREE_T(G,cost); }


list<edge> MIN_SPANNING_TREE(const graph& G, const edge_array<double>& cost)
{ return MIN_SPANNING_TREE_T(G,cost); }

LEDA_END_NAMESPACE

