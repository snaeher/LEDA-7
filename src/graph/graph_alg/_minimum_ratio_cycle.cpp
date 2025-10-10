/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _minimum_ratio_cycle.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/shortest_path.h>
#include <LEDA/graph/templates/shortest_path.h>
#include <LEDA/numbers/rational.h>


LEDA_BEGIN_NAMESPACE

#if defined(__EXPLICIT_INSTANTIATION)
template bool BELLMAN_FORD_T(const graph&, node, const edge_array<integer>&,
                                                  node_array<integer>&,
                                                  node_array<edge>&);

template node_array<int> CHECK_SP_T(const graph&, node, 
                                    const edge_array<integer>&, 
                                    const node_array<integer>&,
                                    const node_array<edge>&);

#endif



bool greater_than_lambda_star(const graph& G, node s,
                              const edge_array<int>& c,
                              const edge_array<int>& p,
                              rational lambda)
{ edge_array<integer> cost(G);
  
  integer num = lambda.numerator();
  integer denom = lambda.denominator();
  edge e;
  forall_edges(e,G) cost[e] = denom*c[e] - num*p[e];

  node_array<integer> dist(G);
  node_array<edge> pred(G);

  return !BELLMAN_FORD_T(G,s,cost,dist,pred);
}


rational MINIMUM_RATIO_CYCLE(graph& G, 
                              const edge_array<int>& c,
                              const edge_array<int>& p,
                              list<edge>& C_opt)
{ node v; edge e;

  
  #if ( defined WINDOW || defined ASCII)  // only for demos
  int number_of_rounds = 0;
  #endif
 // for minimum ratio cycle demo

  if ( Is_Acyclic(G) ) return rational(0);  

  node s = G.new_node();
  forall_nodes(v,G) if (v != s) G.new_edge(s,v);
 
  edge_array<int> c1(G);
  edge_array<int> p1(G);

  int Cmax = 0; int Pmax = 0;

  forall_edges(e,G)
  { if (G.source(e) == s) { c1[e] = p1[e] = 0; }
    else
    { c1[e] = c[e]; p1[e] = p[e];
      Cmax = leda_max(Cmax,c[e]);
      Pmax = leda_max(Pmax,p[e]);
    }
  }

  int n = G.number_of_nodes();

  
  edge_array<int> unit_cost(G,1);
  rational one_over_n = 1/rational(n);

  if (greater_than_lambda_star(G,s,c1,unit_cost,one_over_n))
     LEDA_EXCEPTION(1,"cycle of cost zero or less in c");

  if (greater_than_lambda_star(G,s,p1,unit_cost,one_over_n))
     LEDA_EXCEPTION(1,"cycle of cost zero or less in p");


  rational rat_n = rational(n);
  rational lambda_min = rational(0);
  rational lambda_max = rat_n * rational(Cmax) + rational(1);

  rational delta = 1/(rat_n * rat_n * Pmax * Pmax);

  while (lambda_max - lambda_min > delta)
  { rational lambda = (lambda_max + lambda_min)/2;
    lambda.normalize();   // important

    
    #if ( defined WINDOW || defined ASCII )
        I.write_demo("lambda_min = " + rational_to_string(lambda_min));
        I.write_demo(1,"lambda_max = " + rational_to_string(lambda_max));
        if (number_of_rounds% 10 == 0 ) I.stop_window();
        number_of_rounds++;
    #endif




    if ( greater_than_lambda_star(G,s,c1,p1,lambda) )
     lambda_max = lambda;
    else
     lambda_min = lambda;
  }
  rational lambda_opt;
  { 
    edge_array<integer> cost(G);
      
    node v; edge e;
    integer num = lambda_max.numerator();
    integer denom = lambda_max.denominator();
    forall_edges(e,G) cost[e] = denom*c1[e] - num*p1[e];

    node_array<integer> dist(G);
    node_array<edge> pred(G);

    BELLMAN_FORD_T(G,s,cost,dist,pred);

    node_array<int> label = CHECK_SP_T(G,s,cost,dist,pred);

    forall_nodes(v,G) if (label[v] == -2) break;


    int P = 0; int C = 0;
    node z = v;
    do { P += p[pred[z]]; 
         C += c[pred[z]];
         C_opt.append(pred[z]);
         z = G.source(pred[z]);
       } while ( z != v);

    lambda_opt = rational(C)/rational(P);

 }

  G.del_node(s);

  return lambda_opt;

}

LEDA_END_NAMESPACE

