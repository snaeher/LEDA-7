/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mw_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/mw_matching.h>
#include <LEDA/graph/templates/mw_matching.h>

LEDA_BEGIN_NAMESPACE

#if defined (__EXPLICIT_INSTANTIATION)

template
list<edge> MAX_WEIGHT_MATCHING_T(const graph& G, 
                                 const edge_array<int>& w, 
                                 bool check = true, int heur = 2);



template
list<edge> MAX_WEIGHT_MATCHING_T(const graph& G, 
                                 const edge_array<int>& w, 
				 node_array<int>& pot, 
				 array<two_tuple<int, int> >& BT, 
				 node_array<int>& b,
                                 bool check = true, int heur = 2);


template
bool CHECK_MAX_WEIGHT_MATCHING_T(const graph& G,
                                 const edge_array<int>& w,
				 const list<edge>& M,
				 const node_array<int>& pot,
				 const array<two_tuple<int, int> >& BT,
				 const node_array<int>& b);

template
list<edge> MAX_WEIGHT_PERFECT_MATCHING_T(const graph& G, 
                                         const edge_array<int>& w, 
					 bool check = true, int heur = 2);

template
list<edge> MAX_WEIGHT_PERFECT_MATCHING_T(const graph& G, 
                                         const edge_array<int>& w, 
					 node_array<int>& pot, 
					 array<two_tuple<int, int> >& BT, 
					 node_array<int>& b,
					 bool check = true, int heur = 2);

template
bool CHECK_MAX_WEIGHT_PERFECT_MATCHING_T(const graph& G,
                                         const edge_array<int>& w,
					 const list<edge>& M,
					 const node_array<int>& pot,
					 const array<two_tuple<int, int> >& BT,
					 const node_array<int>& b);


template
list<edge> MIN_WEIGHT_PERFECT_MATCHING_T(const graph& G, 
                                         const edge_array<int>& w, 
                                         bool check = true, int heur = 2);


template
list<edge> MIN_WEIGHT_PERFECT_MATCHING_T(const graph& G, 
                                         const edge_array<int>& w, 
				         node_array<int>& pot, 
				         array<two_tuple<int, int> >& BT, 
				         node_array<int>& b,
                                         bool check = true, int heur = 2);

template
bool CHECK_MIN_WEIGHT_PERFECT_MATCHING_T(const graph& G,
                                         const edge_array<int>& w,
					 const list<edge>& M,
					 const node_array<int>& pot,
					 const array<two_tuple<int, int> >& BT,
					 const node_array<int>& b);

template
bool CHECK_WEIGHTS(const graph& G, const edge_array<int>& w, bool perfect);

#endif



/*
list<edge> MAX_WEIGHT_MATCHING(const graph& G, 
                               const edge_array<int>& w, 
                               bool check, int heur)
{ 
  return MAX_WEIGHT_MATCHING_T(G,w,check,heur); 
}
*/



list<edge> MAX_WEIGHT_MATCHING(const graph& G, 
                               const edge_array<int>& w, 
                               node_array<int>& pot, 
                               array<two_tuple<int, int> >& BT, 
                               node_array<int>& b,
                               bool check, int heur)
{ 
  return MAX_WEIGHT_MATCHING_T(G,w,pot,BT,b,check,heur); 
}


bool CHECK_MAX_WEIGHT_MATCHING(const graph& G,
                               const edge_array<int>& w,
                               const list<edge>& M,
                               const node_array<int>& pot,
                               const array<two_tuple<int, int> >& BT,
                               const node_array<int>& b)
{ 
  return CHECK_MAX_WEIGHT_MATCHING_T(G,w,M,pot,BT,b);
}



/*
list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       bool check, int heur)
{ 
  return MAX_WEIGHT_PERFECT_MATCHING_T(G,w,check,heur);
}
*/


list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       node_array<int>& pot, 
                                       array<two_tuple<int, int> >& BT, 
                                       node_array<int>& b,
                                       bool check, int heur)
{ 
  return MAX_WEIGHT_PERFECT_MATCHING_T(G,w,pot,BT,b,check,heur);
}


bool CHECK_MAX_WEIGHT_PERFECT_MATCHING(const graph& G,
                                       const edge_array<int>& w,
                                       const list<edge>& M,
                                       const node_array<int>& pot,
                                       const array<two_tuple<int, int> >& BT,
                                       const node_array<int>& b)
{ 
  return CHECK_MAX_WEIGHT_PERFECT_MATCHING_T(G,w,M,pot,BT,b);
}


/*
list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       bool check, int heur)
{ 
  return MIN_WEIGHT_PERFECT_MATCHING_T(G,w,check,heur);
}
*/


list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       node_array<int>& pot, 
                                       array<two_tuple<int, int> >& BT, 
                                       node_array<int>& b,
                                       bool check, int heur)
{ 
  return MIN_WEIGHT_PERFECT_MATCHING_T(G,w,pot,BT,b,check,heur);
}

bool CHECK_MIN_WEIGHT_PERFECT_MATCHING(const graph& G,
                                       const edge_array<int>& w,
                                       const list<edge>& M,
                                       const node_array<int>& pot,
                                       const array<two_tuple<int, int> >& BT,
                                       const node_array<int>& b)
{ 
  return CHECK_MIN_WEIGHT_PERFECT_MATCHING_T(G,w,M,pot,BT,b);
}



bool CHECK_WEIGHTS(const graph& G, edge_array<int>& w, bool perfect)
{ return CHECK_WEIGHTS_T(G,w,perfect); }




// double versions



/*
list<edge> MAX_WEIGHT_MATCHING(const graph& G, 
                               const edge_array<double>& w, 
                               bool check, int heur)
{ 
  return MAX_WEIGHT_MATCHING_T(G,w,check,heur); 
}
*/



list<edge> MAX_WEIGHT_MATCHING(const graph& G, 
                               const edge_array<double>& w, 
                               node_array<double>& pot, 
                               array<two_tuple<double, int> >& BT, 
                               node_array<int>& b,
                               bool check, int heur)
{ 
  return MAX_WEIGHT_MATCHING_T(G,w,pot,BT,b,check,heur); 
}


bool CHECK_MAX_WEIGHT_MATCHING(const graph& G,
                               const edge_array<double>& w,
                               const list<edge>& M,
                               const node_array<double>& pot,
                               const array<two_tuple<double, int> >& BT,
                               const node_array<int>& b)
{ 
  return CHECK_MAX_WEIGHT_MATCHING_T(G,w,M,pot,BT,b);
}



/*
list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       bool check, int heur)
{ 
  return MAX_WEIGHT_PERFECT_MATCHING_T(G,w,check,heur);
}
*/


list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       node_array<double>& pot, 
                                       array<two_tuple<double, int> >& BT, 
                                       node_array<int>& b,
                                       bool check, int heur)
{ 
  return MAX_WEIGHT_PERFECT_MATCHING_T(G,w,pot,BT,b,check,heur);
}


bool CHECK_MAX_WEIGHT_PERFECT_MATCHING(const graph& G,
                                       const edge_array<double>& w,
                                       const list<edge>& M,
                                       const node_array<double>& pot,
                                       const array<two_tuple<double, int> >& BT,
                                       const node_array<int>& b)
{ 
  return CHECK_MAX_WEIGHT_PERFECT_MATCHING_T(G,w,M,pot,BT,b);
}


/*
list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       bool check, int heur)
{ 
  return MIN_WEIGHT_PERFECT_MATCHING_T(G,w,check,heur);
}
*/


list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       node_array<double>& pot, 
                                       array<two_tuple<double, int> >& BT, 
                                       node_array<int>& b,
                                       bool check, int heur)
{ 
  return MIN_WEIGHT_PERFECT_MATCHING_T(G,w,pot,BT,b,check,heur);
}

bool CHECK_MIN_WEIGHT_PERFECT_MATCHING(const graph& G,
                                       const edge_array<double>& w,
                                       const list<edge>& M,
                                       const node_array<double>& pot,
                                       const array<two_tuple<double, int> >& BT,
                                       const node_array<int>& b)
{ 
  return CHECK_MIN_WEIGHT_PERFECT_MATCHING_T(G,w,M,pot,BT,b);
}



bool CHECK_WEIGHTS(const graph& G, edge_array<double>& w, bool perfect)
{ return CHECK_WEIGHTS_T(G,w,perfect); }



// versions for undirected graphs (sn 02/2009)

template<class NT>
inline list<edge> MAX_WEIGHT_MATCHING_UNDIR(const graph& U, 
                                            const edge_array<NT>& uweights,
                                            bool check, int heur, bool perfect)
{
  assert(U.is_undirected());
  assert(uweights.get_owner() == &U);

  // construct directed copy G of U

  GRAPH<node,edge> G;
  node_array<node> V(U,NULL);
  edge_array<edge> E(U,NULL);

  node u;
  forall_nodes(u,U) V[u] = G.new_node(u);

  edge x;
  forall_edges(x,U) 
  { node a = V[U.source(x)];
    node b = V[U.target(x)];
    E[x] = G.new_edge(a,b,x);
   }


  // copy weights

  edge_array<NT> weights(G);
  edge e;
  forall_edges(e,G) weights[e] = uweights[G[e]]; 


  // run matching on directed graph
  
  list<edge> M; 

  if (perfect)
     M = MAX_WEIGHT_PERFECT_MATCHING(G,weights,check,heur);
  else
     M = MAX_WEIGHT_MATCHING(G,weights,check,heur);

  // translate result back to original undirected graph

  list<edge> result;
  forall(e,M) result.append(G[e]);
  return result;
}



list<edge> MAX_WEIGHT_MATCHING(const graph& G, const edge_array<int>& w, 
                                               bool check, int heur)
{ if (G.is_directed())
    return MAX_WEIGHT_MATCHING_T(G,w,check,heur);
  else
    return MAX_WEIGHT_MATCHING_UNDIR(G,w,check,heur,false);
}


list<edge> MAX_WEIGHT_MATCHING(const graph& G, const edge_array<double>& w, 
                                               bool check, int heur)
{ if (G.is_directed())
    return MAX_WEIGHT_MATCHING_T(G,w,check,heur);
  else
    return MAX_WEIGHT_MATCHING_UNDIR(G,w,check,heur,false); 
}



list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       bool check, int heur)
{ if (G.is_directed())
    return MAX_WEIGHT_PERFECT_MATCHING(G,w,check,heur); 
  else
    return MAX_WEIGHT_MATCHING_UNDIR(G,w,check,heur,true);
}


list<edge> MAX_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       bool check, int heur)
{ if (G.is_directed())
    return MAX_WEIGHT_PERFECT_MATCHING(G,w,check,heur); 
  else
    return MAX_WEIGHT_MATCHING_UNDIR(G,w,check,heur,true);
}






template<class NT>
inline list<edge> MIN_WEIGHT_PERFECT_MATCHING_UNDIR(const graph& U, 
                                            const edge_array<NT>& uweights,
                                            bool check, int heur)
{
  assert(U.is_undirected());
  assert(uweights.get_owner() == &U);

  // construct directed copy G of U

  GRAPH<node,edge> G;
  node_array<node> V(U,NULL);
  edge_array<edge> E(U,NULL);

  node u;
  forall_nodes(u,U) V[u] = G.new_node(u);

  edge x;
  forall_edges(x,U) 
  { node a = V[U.source(x)];
    node b = V[U.target(x)];
    E[x] = G.new_edge(a,b,x);
   }


  // copy weights

  edge_array<NT> weights(G);
  edge e;
  forall_edges(e,G) weights[e] = uweights[G[e]]; 


  // run matching on directed graph
  
  list<edge> M = MIN_WEIGHT_PERFECT_MATCHING(G,weights,check,heur);

  // translate result back to original undirected graph

  list<edge> result;
  forall(e,M) result.append(G[e]);
  return result;
}



list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<int>& w, 
                                       bool check, int heur)
{ if (G.is_directed())
    return MIN_WEIGHT_PERFECT_MATCHING(G,w,check,heur); 
  else
    return MIN_WEIGHT_PERFECT_MATCHING_UNDIR(G,w,check,heur);
}


list<edge> MIN_WEIGHT_PERFECT_MATCHING(const graph& G, 
                                       const edge_array<double>& w, 
                                       bool check, int heur)
{ if (G.is_directed())
    return MIN_WEIGHT_PERFECT_MATCHING(G,w,check,heur); 
  else
    return MIN_WEIGHT_PERFECT_MATCHING_UNDIR(G,w,check,heur);
}






LEDA_END_NAMESPACE

