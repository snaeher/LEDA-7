/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  euler_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/euler_tour.h>
#include <LEDA/core/array.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



typedef two_tuple<edge,int> dart;

void Eulerian_Graph(graph& G, int n, int m)
{ array<node> V(n);
  int i;
  for (i = 0; i < n; i++) V[i] = G.new_node();
  for (i = 0; i < m; i++)
  { int v = rand_int(0,n-1);
    int w = rand_int(0,n-1);
    G.new_edge(V[v],V[w]);
  }
  Make_Connected(G);
  i = 0;
  while ( true )
  { while ( i < n && G.degree(V[i]) % 2 == 0 ) i++;
    if ( i == n ) return;
    int j = i + 1;
    while ( j < n && G.degree(V[j]) % 2 == 0 ) j++;
    leda_assert( j < n,"Make Eulerian Graph",1);
    G.new_edge(V[i],V[j]);
    i = j + 1;
  }
}



int main(int argc, char* argv[]) {

  int n_max = 0;
  if (argc > 1) n_max = atoi(argv[1]);

  for (int n = 1; n != n_max; n++) {
    for (int m = 0; m < 2*n; m++) {
      cout << "\nn = " << n << " m = " << m;
      for (int i = 0; i < 10; i++) { 
        graph G;
        Eulerian_Graph(G,n,m);

        list<dart> T;
        Euler_Tour(G,T);
        leda_assert(Check_Euler_Tour(G,T),"HUCH 1",1);

        list<edge> E;
        Euler_Tour(G,E);  
        leda_assert(Check_Euler_Tour(G,E),"HUCH 2",1);
       }
     }
   }
  return 0;
}

