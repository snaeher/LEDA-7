/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _sort_edges.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE


template <class NT>
inline int sign_of(const NT& x)
{ if ( x == 0 ) return 0;
  else if (x > 0 ) return 1;
       else return -1;
}

template <class NT>
inline int compare_vectors(const NT& x1, const NT& y1, 
                           const NT& x2, const NT& y2)
{ 
  if (x1 == x2 && y1 == y2) return 0;
  if (x1 == 0 && y1 == 0) return -1;
  if (x2 == 0 && y2 == 0) return +1;
  // vectors are distinct and non-zero

  int sy1 = sign_of(y1);  int sy2 = sign_of(y2); 

  int upper1 = ( sy1 != 0 ? sy1 : sign_of(x1) );
  int upper2 = ( sy2 != 0 ? sy2 : sign_of(x2) );

  if ( upper1 == upper2 ) return sign_of(x2*y1 - x1*y2);

  return upper2 - upper1;
}


template <class NT>
class cmp_edges_by_angle_edge_case : 
                public leda_cmp_base<edge> 
{
  const edge_array<NT>* DX;
  const edge_array<NT>* DY;

public:

cmp_edges_by_angle_edge_case(const edge_array<NT>& dx, 
                             const edge_array<NT>& dy)
: DX(&dx), DY(&dy){}


int operator()(const edge& e, const edge& f) const
{ if ( source(e) != source(f) ) 
    return compare(index(source(e)),index(source(f)));
  else
    return compare_vectors((*DX)[e],(*DY)[e],(*DX)[f],(*DY)[f]);
} 

};


template <class NT>
bool Is_CCW_Ordered_T(const graph &G, 
                      const edge_array<NT>& dx, 
                      const edge_array<NT>& dy)
{ node v; edge e;

  cmp_edges_by_angle_edge_case<NT> cmp(dx,dy);

  forall_nodes(v,G)
  { int number_of_greater = 0;
    forall_adj_edges(e,v)
    { edge f = G.cyclic_adj_succ(e); 
      if ( cmp(e,f) > 0 ) number_of_greater++;       
    }
    if (number_of_greater > 1) return false;
  }
  return true;
}

bool Is_CCW_Ordered(const graph &G, const edge_array<int>& dx, 
                                    const edge_array<int>& dy)
{ return Is_CCW_Ordered_T(G,dx,dy); }

bool Is_CCW_Ordered(const graph &G, const edge_array<double>& dx, 
                                    const edge_array<double>& dy)
{ return Is_CCW_Ordered_T(G,dx,dy); }



template <class NT>
inline bool SORT_EDGES_T(graph &G, const edge_array<NT>& dx, 
                                   const edge_array<NT>& dy)
{ 
  if (G.is_undirected())
  { LEDA_EXCEPTION(1,"SORT_EDGES: sorry, not implemented for ugraph.");
    return false;
   }

  cmp_edges_by_angle_edge_case<NT> cmp(dx,dy);

  list<edge> L = G.all_edges();
  L.sort(cmp);
  G.sort_edges(L); 
 
  // G.sort_edges(cmp) in the next release instead of 
  // the three lines above

  assert(Is_CCW_Ordered(G,dx,dy));
  return Is_Plane_Map(G);  
}

bool SORT_EDGES(graph &G, const edge_array<int>& dx, 
                          const edge_array<int>& dy)
{ return SORT_EDGES_T(G,dx,dy); }

bool SORT_EDGES(graph &G, const edge_array<double>& dx, 
                          const edge_array<double>& dy)
{ return SORT_EDGES_T(G,dx,dy); }



template <class NT>
class cmp_edges_by_angle_node_case : 
                public leda_cmp_base<edge> 
{
  const node_array<NT>* X;
  const node_array<NT>* Y;

public:

cmp_edges_by_angle_node_case(const node_array<NT>& x, 
                             const node_array<NT>& y)
: X(&x), Y(&y){}


int operator()(const edge& e, const edge& f) const
{ if ( source(e) != source(f) ) 
    return compare(index(source(e)),index(source(f)));
  else
  { node v = source(e); 
    node w1 = target(e); node w2 = target(f);
    NT x1 = (*X)[w1] - (*X)[v]; NT y1 = (*Y)[w1] - (*Y)[v];
    NT x2 = (*X)[w2] - (*X)[v]; NT y2 = (*Y)[w2] - (*Y)[v];
    return compare_vectors(x1,y1,x2,y2);
   }
} 

};

template <class NT>
bool Is_CCW_Ordered_T(const graph &G, 
                      const node_array<NT>& x, 
                      const node_array<NT>& y)
{ node v; edge e;

  cmp_edges_by_angle_node_case<NT> cmp(x,y);

  forall_nodes(v,G)
  { int number_of_greater = 0;
    forall_adj_edges(e,v)
    { edge f = G.cyclic_adj_succ(e); 
      if ( cmp(e,f) > 0 ) number_of_greater++;       
    }
    if (number_of_greater > 1) return false;
  }
  return true;
}


bool Is_CCW_Ordered(const graph &G, 
                    const node_array<int>& x, 
                    const node_array<int>& y)
{ return Is_CCW_Ordered_T(G,x,y); }


bool Is_CCW_Ordered(const graph &G, 
                    const node_array<double>& x, 
                    const node_array<double>& y)
{ return Is_CCW_Ordered_T(G,x,y); }



template <class NT>
bool SORT_EDGES_T(graph &G, const node_array<NT>& x, 
                                   const node_array<NT>& y)
{ 
  if (G.is_undirected())
  { LEDA_EXCEPTION(1,"SORT_EDGES: sorry, not implemented for ugraph.");
    return false;
   }

  cmp_edges_by_angle_node_case<NT> cmp(x,y);

  list<edge> L = G.all_edges();
  L.sort(cmp);
  G.sort_edges(L);

  assert(Is_CCW_Ordered(G,x,y));
  return Is_Plane_Map(G);  
}

bool SORT_EDGES(graph &G, const node_array<int>& x, 
                          const node_array<int>& y)
{ return SORT_EDGES_T(G,x,y); }
  

bool SORT_EDGES(graph &G, const node_array<double>& x, 
                          const node_array<double>& y)
{ return SORT_EDGES_T(G,x,y); }
  

LEDA_END_NAMESPACE

