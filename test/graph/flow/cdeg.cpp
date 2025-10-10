/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  cdeg.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/dimacs.h>
#include <LEDA/system/stream.h>

using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::endl;


int main(int argc, char** argv)
{
  GRAPH<int,int> G;
  edge_array<int>& cap = G.edge_data();
  node s,t;

  int c = 0;
  if (argc > 2) c = atoi(argv[1]); 
      
  Read_Dimacs_MF(cin,G,s,t,cap);

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  int deg = 0;

  node v;
  forall_nodes(v,G)
     if (G.outdeg(v) > deg) deg = G.outdeg(v);

  int m1 = n*deg;

  cerr << endl;
  cerr << string("max degree = %d",deg) << endl;
  cerr << string("I insert %d new edges",m1-m) << endl;
  cerr << endl;
  

  forall_nodes(v,G)
  { while (G.outdeg(v) < deg) 
    { edge e = G.new_edge(v,(v != s) ? s : t);
      cap[e] = c;
     }
   }

  Write_Dimacs_MF(cout,G,s,t,cap);

  return 0;
}
