/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mst2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/plane_alg.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/templates/quicksort.t>

#include <LEDA/core/tuple.h>

using namespace leda;

using std::cout;
using std::endl;



typedef two_tuple<node,node> node_pair;

typedef point  POINT;
typedef double COORD;

void MIN_SPANNING_TREE1(const list<POINT>& L, GRAPH<POINT,int>& T)
{ 
float t = used_time();
  DELAUNAY_TRIANG(L,T);
cout << string("delaunay: %.2f",used_time(t)) << endl;


t = used_time();
  node_partition P(T);

  int n = T.number_of_edges()/2;
  int i = 0;

  node_pair* A = new node_pair[n];
  COORD*     C = new COORD[n];


  edge e;
  forall_edges(e,T)
  { edge r = T.reversal(e);
    if (T[r] == MAXINT) continue; 
    T[e] = MAXINT;
    node v = source(e);
    node w = target(e);
    A[i].first() = v;
    A[i].second()= w;
    C[i] = T[v].sqr_dist(T[w]);
    i++;
   }
cout << string("structures: %.2f",used_time(t)) << endl;

t = used_time();
  QUICKSORT_P(C,A,0,n-1);
cout << string("sorting: %.2f",used_time(t)) << endl;

t = used_time();
  T.del_all_edges();
cout << string("del_edges: %.2f",used_time(t)) << endl;

t = used_time();
  for(int i=0; i<n; i++)
  { node v = A[i].first();
    node w = A[i].second();
    if (!P.same_block(v,w))
    { P.union_blocks(v,w);
      T.new_edge(v,w);
     }
   }
cout << string("partition: %.2f",used_time(t)) << endl;

 
  delete[] A;
  delete[] C;
}


int main()
{
   int N = read_int("N = ");

   list<point>     L;
   random_source ran(0,100000);

   ran.set_seed(12345*N);

   for(int i=0; i<N; i++) 
   { int x,y;
     ran >> x >> y;
     L.append(point(x,y));
    }


  GRAPH<point,int> G;
  GRAPH<point,int> G1;
  edge e;


  float T = used_time();
  MIN_SPANNING_TREE(L,G);
  float t = used_time(T);
  double c = 0;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G.number_of_nodes(), G.number_of_edges(),c,t) << endl;

  T = used_time();
  MIN_SPANNING_TREE1(L,G1);
  t = used_time(T);
  c = 0;
  forall_edges(e,G) c += G[source(e)].sqr_dist(G[target(e)]);
  cout << string("|V| = %d |E| = %d  cost = %.2f   %.2f sec",
             G1.number_of_nodes(), G1.number_of_edges(),c,t) << endl;


 return 0;
}
