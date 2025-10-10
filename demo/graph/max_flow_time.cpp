/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  max_flow_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:25 $


#include <LEDA/core/IO_interface.h>
#include <LEDA/core/array2.h>
#include <LEDA/graph/graph_alg.h>

#include <LEDA/graph/templates/max_flow_book.h>

using namespace leda;


int main()
{ IO_interface I("Max Flow Algorithm");

  I.write_demo("This demo illustrates the speed of the \
max flow algorithm on four different kinds of graph. \
The LEDA book gives details."); 

 I.write_demo("");
I.write_demo("GENERATORS");
I.write_demo("Random: random graph");
I.write_demo("CG1: Cherkassky -- Goldberg");
I.write_demo("CG2: Cherkassky -- Goldberg");
I.write_demo("AMO: Ahuja -- Orlin -- Magnanti");

I.write_demo("Try medium values of n and m, e.g, n = 1000, m = 3000.");

int n = I.read_int("n = ",1000); 
int m = I.read_int("m = ",3000);
int y = I.read_int("write one to get extensive statistics: ");
  
  GRAPH<int,int> G; node s,t;

  array2<double> A(168,2); 
  int C = 0; 
  int l = 0; 
  int j;

 
  for (j = 0; j < 4; j++)
  { switch (j)
    { case 0: I.write_demo("RANDOM GRAPH");
             max_flow_gen_rand(G,s,t,n,m);
             break;
      case 1:I.write_demo("CG1");
             max_flow_gen_CG1(G,s,t,n);
             break;
      case 2:I.write_demo("CG2");
             max_flow_gen_CG2(G,s,t,n);
             break;
      case 3:I.write_demo("AMO");
             max_flow_gen_AMO(G,s,t,n);
             break;
    }
 
   edge_array<int> flow(G);

   float T; 
   int num_pushes, num_edge_inspections,
                       num_relabels,num_global_relabels, num_gaps;
// int alg; 

   int k;
 
    T  = used_time();
    MAX_FLOW_T(G,s,t,G.edge_data(),flow,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,num_gaps,5); 
    C = 5; k = 0;
    
    if ( y == 1)
    { I.write_demo("number of pushes = ",num_pushes);
      I.write_demo("number of edge inspections = ",num_edge_inspections);
      I.write_demo("number of relabels = ",num_relabels);
      I.write_demo("number of global relabels = ",num_global_relabels);
      I.write_demo("number of gaps = ",num_gaps);
    }
    float UT = used_time(T);
    I.write_demo("running time = ",UT);
    A(42*j + 14*k + 2*C + l,0) = G.number_of_nodes();
    A(42*j + 14*k + 2*C + l,1) = UT;



   }
return 0;
}



