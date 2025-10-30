/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  kura_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main(int argc,char** argv)
{

  graph G;
  int n = read_int("n = ");
  int m = read_int("m = ");

  for(;;)
  { random_graph(G,n,m);
    list<edge> L;
    float T = used_time();
    PLANAR(G,L,false); 
    cout << string("|Ek| = %3d   %5.2f sec", L.length(), used_time(T)) << endl; 
  }

  
  return 0;
}
