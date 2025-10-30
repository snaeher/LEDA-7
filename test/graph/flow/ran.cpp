/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  ran.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#define LEDA_CHECKING_OFF

#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/system/stream.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::cin;
using std::cerr;
using std::flush;
using std::endl;



int main(int argc, char** argv) 
{
  if (argc < 2)
  { cerr << "usage: " << argv[0] << " n m [seed]" << endl;
    return 1;
   }

  GRAPH<int,int> G;
  edge_array<int>& cap = G.edge_data();
  node s,t;

  int n = atoi(argv[1]);
  int m = int(sqrt((float)n)*n/2);

  if (argc >= 3) m = atoi(argv[2]);

  if (argc >= 4) rand_int.set_seed(atoi(argv[2]));

  rand_int.set_seed(n+m);

  max_flow_gen_rand(G,s,t,n,m);
  //Delete_Loops(G);
  Write_Dimacs_MF(cout,G,s,t,cap);
    
 return 0;
}
