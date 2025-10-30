/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  cg2.c
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

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cerr;
using std::endl;
#endif



int main(int argc, char** argv) 
{
  if (argc < 2)
  { cerr << "usage: " << argv[0] << " n" << endl;
    return 1;
   }

  GRAPH<int,int> G;
  edge_array<int>& cap = G.edge_data();
  node s,t;

  int n = atoi(argv[1]);

  max_flow_gen_CG2(G,s,t,n);
  Write_Dimacs_MF(cout,G,s,t,cap);
    
 return 0;
}
