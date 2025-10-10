/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  minimum_ratio_cycle_demo.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:22 $


#include <LEDA/core/IO_interface.h>
#define RATIO_CYCLE_DEMO

using namespace leda;


IO_interface I("Minimum Ratio Cycle Arithmetic Demand");


#include <LEDA/graph/templates/shortest_path.h>
#include <LEDA/numbers/rational.h>


bool greater_than_lambda_star(const graph& G, node s,
                              const edge_array<int>& c,
                              const edge_array<int>& p,
                              rational lambda)
{ edge_array<integer> cost(G);
  
  edge e;
  integer num = lambda.numerator();
  integer denom = lambda.denominator();
  forall_edges(e,G) cost[e] = denom*c[e] - num*p[e];

  node_array<integer> dist(G);
  node_array<edge> pred(G);

  return !BELLMAN_FORD_T(G,s,cost,dist,pred);
}


rational MIN_RATIO_CYCLE(graph& G, const edge_array<int>& c,
                                   const edge_array<int>& p,
                                   list<edge>& C_opt)
{ node v; edge e;

  
  #ifdef RATIO_CYCLE_DEMO  
  int number_of_rounds = 0;    
  #endif
 // for minimum ratio cycle demo

  C_opt.clear();
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
  rational one_over_n(integer(1),integer(n));

  if (greater_than_lambda_star(G,s,c1,unit_cost,one_over_n))
     error_handler(1,"cycle of cost zero or less wrt c");
  if (greater_than_lambda_star(G,s,p1,unit_cost,one_over_n))
     error_handler(1,"cycle of cost zero or less wrt p");


  integer int_n(n);
  integer int_Pmax(Pmax);
  rational lambda_min(integer(0));
  rational lambda_max(int_n * integer(Cmax) + integer(1));

  rational delta(1,int_n * int_n * int_Pmax * int_Pmax);

  while (lambda_max - lambda_min > delta)
  { rational lambda = (lambda_max + lambda_min)/2;
    lambda.normalize();   // important

    
    #ifdef RATIO_CYCLE_DEMO  
        I.write_demo("lambda_min = " + lambda_min.to_string());
        I.write_demo(1,"lambda_max = " + lambda_max.to_string());
        if (number_of_rounds% 10 == 0 ) I.stop();
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
    do { P += p[pred[z]]; C += c[pred[z]];
         C_opt.append(pred[z]);
         z = G.source(pred[z]);
       } while ( z != v);

    lambda_opt = rational(C)/rational(P);

 }

  G.del_node(s);

  return lambda_opt;

}


#include <LEDA/graph/graph_alg.h>

int main(){

graph G;


I.write_demo("This demo illustrates the arithmetic demand of the minimum \
ratio cycle algorithm.");

I.write_demo("We generate a random graph with n nodes and \
m edges and choose for each edge a cost and a profit. \
The costs are random numbers in [701 .. 901] \
and the profits are random numbers in [1001 .. 1004].");

I.write_demo("We will determine the \
cycle where the ratio of total cost to total profit is minimal.");

I.write_demo("Let lambda be some \
real parameter. We consider the edge costs c[e] = cost[e] - lambda*profit[e]. \
Lambda_opt is the largest value of lambda for which the graph has \
no negative cycle.");

I.write_demo("We search for this value by binary search in the interval \
[ 1/(n*P) .. C*n], where C is the \
maximal cost and P is the maximal profit. Each step of the binary search is a \
negative cycle detection problem.");





int n = I.read_int("n = ");
int m = I.read_int("m = ");

random_simple_loopfree_graph(G,n,m);

edge_array<int> p(G);
edge_array<int> c(G); 


edge e;
forall_edges(e,G) { p[e] = rand_int(1001,1004); c[e] = rand_int(701,901); }

list<edge> C_opt;

rational lambda_star = MIN_RATIO_CYCLE(G,c,p,C_opt);

I.write_demo("lambda_star = " + lambda_star.to_string());
I.write_demo("The optimal cycle contains ",C_opt.length()," edges.");

return 0;
}


