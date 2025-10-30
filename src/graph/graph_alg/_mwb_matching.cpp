/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _mwb_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/mwb_matching.h>
#include <LEDA/graph/scale_weights.h>
#include <LEDA/graph/templates/mwb_matching.h>

LEDA_BEGIN_NAMESPACE

bool MWBM_SCALE_WEIGHTS(const graph& G, 
                        edge_array<double>& c)
{ 
  return scale_weights(G,c,3.0);
}

bool MWA_SCALE_WEIGHTS(const graph& G, 
                       edge_array<double>& c)
{ 
  return scale_weights(G,c,4*G.number_of_nodes());
}


list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G,
                                const edge_array<int>& c,
                                node_array<int>& pot)
{ int W = MAXINT/3;
  check_weights(G,c,-W,W,"MWBM<int>");
  return MAX_WEIGHT_BIPARTITE_MATCHING_T(G,c,pot); }

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G,
                                const edge_array<double>& c,
                                node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,3.0,"MWBM<double>");
  scale_weights(G,c,c1,3.0);
  return MAX_WEIGHT_BIPARTITE_MATCHING_T(G,c1,pot); 
}

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G,
                               const edge_array<int>& c)
{ node_array<int> pot(G);
  return MAX_WEIGHT_BIPARTITE_MATCHING(G,c,pot); }

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G,
                               const edge_array<double>& c)
{ node_array<double> pot(G);
  return MAX_WEIGHT_BIPARTITE_MATCHING(G,c,pot); }

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G, 
                               const list<node>& A, 
                               const list<node>& B,    
                         const edge_array<int>& c,  
                               node_array<int>& pot)
{ int W = MAXINT/3;
  check_weights(G,c,-W,W,"MWBM<int>");
  return MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,c,pot); 
}

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G, 
                               const list<node>& A, 
                               const list<node>& B,    
                         const edge_array<double>& c,  
                               node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,3.0,"MWBM<double>");
  scale_weights(G,c,c1,3.0);
  return MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,c1,pot); 
}

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G, 
                const list<node>& A, const list<node>& B, 
                const edge_array<int>& c)
{ node_array<int> pot(G);
  return MAX_WEIGHT_BIPARTITE_MATCHING(G,A,B,c,pot); 
}

list<edge> MAX_WEIGHT_BIPARTITE_MATCHING(graph& G, 
                const list<node>& A, const list<node>& B, 
                const edge_array<double>& c)
{ node_array<double> pot(G);
  return MAX_WEIGHT_BIPARTITE_MATCHING_T(G,A,B,c,pot); }

bool CHECK_MWBM(const graph& G, const edge_array<int>& c,
        const list<edge>& M, const node_array<int>& pot)
{ return CHECK_MWBM_T(G,c,M,pot); }

bool CHECK_MWBM(const graph& G, const edge_array<double>& c,
        const list<edge>& M, const node_array<double>& pot)
{ return CHECK_MWBM_T(G,c,M,pot); }

// MAX_WEIGHT_ASSIGNMENT


list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<int>& c,
                               node_array<int>& pot)
{ max_absolute_value(G,c);
  int W = MAXINT/(4*(1+G.number_of_nodes()));
  check_weights(G,c,-W,W,"MWA<int>");
  return MAX_WEIGHT_ASSIGNMENT_T(G,c,pot); 
}

list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<double>& c,
                               node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,4*G.number_of_nodes(),"MWA<double>");
  scale_weights(G,c,c1,4*G.number_of_nodes());
  return MAX_WEIGHT_ASSIGNMENT_T(G,c1,pot); }

list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G, 
                 const list<node>& A, const list<node>& B,    
                 const edge_array<int>& c, 
                                    node_array<int>& pot)
{ max_absolute_value(G,c);
  int W = MAXINT/(4*(1+G.number_of_nodes()));
  check_weights(G,c,-W,W,"MWA<int>");
  return MAX_WEIGHT_ASSIGNMENT_T(G,A,B,c,pot); 
}



list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G, 
                 const list<node>& A, const list<node>& B,    
                 const edge_array<double>& c, 
                                    node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,4*G.number_of_nodes(),"MWA<double>");
  scale_weights(G,c,c1,4*G.number_of_nodes());
  return MAX_WEIGHT_ASSIGNMENT_T(G,A,B,c1,pot); 
}

bool CHECK_MAX_WEIGHT_ASSIGNMENT(const graph& G, const edge_array<int>& c,
        const list<edge>& M, const node_array<int>& pot)
{ return CHECK_MAX_WEIGHT_ASSIGNMENT_T(G,c,M,pot); }

bool CHECK_MAX_WEIGHT_ASSIGNMENT(const graph& G, const edge_array<double>& c,
        const list<edge>& M, const node_array<double>& pot)
{ return CHECK_MAX_WEIGHT_ASSIGNMENT_T(G,c,M,pot); }


list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<int>& c)
{ node_array<int> pot(G);
  return MAX_WEIGHT_ASSIGNMENT(G,c,pot); }

list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<double>& c)
{ node_array<double> pot(G);
  return MAX_WEIGHT_ASSIGNMENT(G,c,pot); }

list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,                                   
                      const list<node>& A, const list<node>& B, 
                      const edge_array<int>& c)
{ node_array<int> pot(G);
  return MAX_WEIGHT_ASSIGNMENT(G,A,B,c,pot); 
}

list<edge> MAX_WEIGHT_ASSIGNMENT(graph& G,                                   
                      const list<node>& A, const list<node>& B, const edge_array<double>& c)
{ node_array<double> pot(G);
  return MAX_WEIGHT_ASSIGNMENT(G,A,B,c,pot); 
}

// MIN WEIGHT ASSIGNMENT

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<int>& c,
                               node_array<int>& pot)
{ max_absolute_value(G,c);
  int W = MAXINT/(4*(1 + G.number_of_nodes()));
  check_weights(G,c,-W,W,"MWA<int>");
  return MIN_WEIGHT_ASSIGNMENT_T(G,c,pot); 
}

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<double>& c,
                               node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,4*G.number_of_nodes(),"MWA<double>");
  scale_weights(G,c,c1,4*G.number_of_nodes());
  return MIN_WEIGHT_ASSIGNMENT_T(G,c1,pot); }

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G, 
                 const list<node>& A, const list<node>& B,    
                 const edge_array<int>& c, 
                                    node_array<int>& pot)
{ max_absolute_value(G,c);
  int W = MAXINT/(4*(1 + G.number_of_nodes()));
  check_weights(G,c,-W,W,"MWA<int>");
  return MIN_WEIGHT_ASSIGNMENT_T(G,A,B,c,pot); 
}

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G, 
                 const list<node>& A, const list<node>& B,    
                 const edge_array<double>& c, 
                                    node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,4*G.number_of_nodes(),"MWA<double>");
  scale_weights(G,c,c1,4*G.number_of_nodes());
  return MIN_WEIGHT_ASSIGNMENT_T(G,A,B,c1,pot); 
}

bool CHECK_MIN_WEIGHT_ASSIGNMENT(const graph& G, const edge_array<int>& c,
        const list<edge>& M, const node_array<int>& pot)
{ return CHECK_MIN_WEIGHT_ASSIGNMENT_T(G,c,M,pot); }

bool CHECK_MIN_WEIGHT_ASSIGNMENT(const graph& G, const edge_array<double>& c,
        const list<edge>& M, const node_array<double>& pot)
{ return CHECK_MIN_WEIGHT_ASSIGNMENT_T(G,c,M,pot); }


list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<int>& c)
{ node_array<int> pot(G);
  return MIN_WEIGHT_ASSIGNMENT(G,c,pot); }

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,
                               const edge_array<double>& c)
{ node_array<double> pot(G);
  return MIN_WEIGHT_ASSIGNMENT(G,c,pot); }

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,                                   
                      const list<node>& A, const list<node>& B, 
                      const edge_array<int>& c)
{ node_array<int> pot(G);
  return MIN_WEIGHT_ASSIGNMENT(G,A,B,c,pot); 
}

list<edge> MIN_WEIGHT_ASSIGNMENT(graph& G,                                   
                      const list<node>& A, const list<node>& B, const edge_array<double>& c)
{ node_array<double> pot(G);
  return MIN_WEIGHT_ASSIGNMENT(G,A,B,c,pot); 
}


// MAX WEIGHT MAX CARDINALITY

list<edge> MWMCB_MATCHING(graph& G, 
                                         const list<node>& A, 
                                         const list<node>& B,  
                                  const edge_array<int>& c, 
                                        node_array<int>& pot)
{ max_absolute_value(G,c);
  int W = MAXINT/(4*(1 + G.number_of_nodes()));
  check_weights(G,c,-W,W,"MWMCB<int>");
  return MWMCB_MATCHING_T(G,A,B,c,pot); 
}

list<edge> MWMCB_MATCHING(graph& G, 
                                         const list<node>& A, 
                                         const list<node>& B,  
                                  const edge_array<double>& c, 
                                        node_array<double>& pot)
{ edge_array<double> c1(G);
  //scale_weights(G,c,c1,4*G.number_of_nodes(),"MWMCB<double>");
  scale_weights(G,c,c1,4*G.number_of_nodes());
  return MWMCB_MATCHING_T(G,A,B,c,pot);
 }

list<edge> MWMCB_MATCHING(graph& G, 
                          const list<node>& A, 
                          const list<node>& B,
                          const edge_array<int>& c)
{ node_array<int> pot(G);
  return MWMCB_MATCHING(G,A,B,c,pot); 
}

list<edge> MWMCB_MATCHING(graph& G, 
                          const list<node>& A, 
                          const list<node>& B,
                          const edge_array<double>& c)
{ node_array<double> pot(G);
  return MWMCB_MATCHING_T(G,A,B,c,pot); }

LEDA_END_NAMESPACE


