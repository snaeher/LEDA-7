/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  mst_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


int main()
{
   int N = read_int("N = ");

   list<point>         L;
   list<rat_point>     L1;
   random_source ran(0,10000);

   ran.set_seed(12345*N);

   for(int i=0; i<N; i++) 
   { int x,y;
     ran >> x >> y;
     L.append(point(x,y));
     L1.append(rat_point(x,y));
    }



{ GRAPH<point,int> G;
  cout << "MIN_SPANNING_TREE_OLD  " << flush;
  float T = used_time();
  MIN_SPANNING_TREE_OLD(L,G);
  float t = used_time(T);
  double c = 0;
  edge e;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G.number_of_nodes(), G.number_of_edges(),c,t) << endl;
}


{ GRAPH<point,int> G;
  cout << "MIN_SPANNING_TREE      " << flush;
  float T = used_time();
  MIN_SPANNING_TREE(L,G);
  float t = used_time(T);
  double c = 0;
  edge e;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G.number_of_nodes(), G.number_of_edges(),c,t) << endl;
}




/*
{ GRAPH<rat_point,int> G;
  cout << "MIN_SPANNING_TREE_OLD  " << flush;
  float T = used_time();
  MIN_SPANNING_TREE_OLD(L1,G);
  float t = used_time(T);
  rational c = 0;
  edge e;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G.number_of_nodes(), G.number_of_edges(),c.to_double(),t) << endl;
}
*/


{ GRAPH<rat_point,int> G;
  cout << "MIN_SPANNING_TREE      " << flush;
  float T = used_time();
  MIN_SPANNING_TREE(L1,G);
  float t = used_time(T);
  rational c = 0;
  edge e;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G.number_of_nodes(), G.number_of_edges(),c.to_double(),t) << endl;
}


 return 0;
}
