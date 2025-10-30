/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  ac_dense.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>

#include <stdlib.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char** argv)
{
  if (argc < 2)
  { cerr << "usage: ac_dense [-ran|-rev] num_nodes [seed]" << endl;
    return 0;
   }

  int seed = rand_int.reinit_seed(); // generate a time-dependent seed

  bool rev=false;
  bool ran=false;
  int num_nodes;

  if (string(argv[1]) == "-rev") 
   { rev = true;
     num_nodes = atoi(argv[2]);
     if (argc >= 4) seed = atoi(argv[3]);
    }
  else
  if (string(argv[1]) == "-ran") 
   { ran = true;
     num_nodes = atoi(argv[2]);
     if (argc >= 4) seed = atoi(argv[3]);
    }
  else
   { rev = false;
     ran = false;
     num_nodes = atoi(argv[1]);
     if (argc >= 3) seed = atoi(argv[2]);
    }

  rand_int.set_seed(seed);


  int source = 1;
  int sink = num_nodes;

  int num_arcs = 0;
  for (int i = 1; i <= (num_nodes-1); i++) num_arcs += i;

  int max_cap = 10000;
 
  printf("c Fully Dense Acyclic Network\n");
  printf("c for Max-Flow\n");
  printf("c Arcs with random capacities\n");
  printf("p max %d %d\n",num_nodes,num_arcs);
  printf("n %d s\n",source);
  printf("n %d t\n",sink);

  int* V = new int[num_nodes];
  int* C = new int[num_nodes];


  for (int p = 1; p < num_nodes; p++)
  { int n = 0;
    for(int q = p+1; q <=  num_nodes; q++) 
    { V[n] = q;
      C[n] = rand_int(1,max_cap); 
      n++;
     }
    for(int i=0; i<n; i++)
    { int k = i;
      int r = rand_int(i,n-1);
      if (rev) 
         k = n-i-1;
      else
        if (ran)
        { swap(V[i],V[r]);
          swap(C[i],C[r]);
         }
      printf("a %d %d %d\n",p,V[k],C[k]);
     }
   }

  delete[] V;

  return 0;
}
