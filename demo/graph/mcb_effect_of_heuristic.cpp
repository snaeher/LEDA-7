/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  mcb_effect_of_heuristic.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:25 $


#include <LEDA/core/list.h>
#include <LEDA/graph/graph.h>
#include <assert.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;



static int use_heuristic;
IO_interface I("Maximum Cardinality Matching: Effect of Heuristic");




double MCB_EFFECT_OF_HEURISTIC(graph& G, 
                               const list<node>& A, const list<node>& B)
{ node v; edge e;
  node_array<bool> free(G,true);

  forall(v,B) assert(G.outdeg(v) == 0);
  
  if (use_heuristic == 0) return 0;

  
  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if ( free[v] && free[w] )
    { free[v] = free[w] = false;
      G.rev_edge(e);
    }
  }


  int n = 0;
  forall(v,A) if (!free[v]) n++;
 
  return double(n)/A.size();
}


int main(){
I.set_precision(3);

I.write_demo("We generate random bipartite graph and use different \
heuristics to compute a matching");

int n = I.read_int("n = ",10000);

for (int m = n; m <= 10*n; m = m + n)

{ graph G;
  list<node> A,B;
  random_bigraph(G,n,n,m,A,B);

  I.write_table("\n", n); I.write_table(" & ",m);
  I.write_demo("n = ",n); I.write_demo("m = ",m);

  float T = used_time();
 
  for (use_heuristic = 0; use_heuristic < 2; use_heuristic++)
  { double H = MCB_EFFECT_OF_HEURISTIC(G,A,B); 
    float UT = used_time(T);
    I.write_table(" & ",H); I.write_table(" & ",UT); 
    I.write_demo("percentage of matched nodes " + string((use_heuristic == 0 ? "without" : "with   ")) + " heuristic  = ",H);
    I.write_demo("used time = ",UT);

    



}
  I.stop(); 
  I.write_table(" \\\\ \\hline ");

}
return 0;
}
 
