/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  max_flow_large_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:24 $


#include <LEDA/templates/max_flow.t>
#include <LEDA/core/IO_interface.h>
#include <LEDA/core/array2.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;


int main()
{ IO_interface I("Max Flow Algorithms: All Heuristic");

  I.write_demo("This demo illustrates the speed of the generic preflow-push \
with different heuristics \
on four different kinds of graph \
and with three different implementations of the set of active nodes. \
The LEDA book gives details.");

 
I.write_demo("VERSIONS OF THE ALGORITHM");
I.write_demo("GRH: local and global relabeling heuristc, two-phases, low-high nodes");
I.write_demo("GAP: the above and gap heuristic");
I.write_demo("");
I.write_demo("SELECTION RULES");
I.write_demo("FIFO: first in-first out selection rule");
I.write_demo("MFIFO: modified first in-first out selection rule");
I.write_demo("HL: highest selection rule");
I.write_demo("");
I.write_demo("GENERATORS");
I.write_demo("Random: random graph");
I.write_demo("CG1: Cherkassky -- Goldberg");
I.write_demo("CG2: Cherkassky -- Goldberg");
I.write_demo("AMO: Ahuja -- Orlin -- Magnanti");



  I.write_demo("Try medium values of n and m, say n = 400, m = 1000.");

  int n = I.read_int("n = ",2000); int m = I.read_int("m = ",6000);
  int y = I.read_int("write one to get extensive statistics: ");
  array2<double> A(192,2); int C = 0; int i,j,k,l;

for (l = 0; l < 3; l++)
{ if (l >= 1) { n *= 2; m *= 2; }
   
  
  GRAPH<int,int> G; node s,t;
 
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

   float T; int f; int num_pushes, num_edge_inspections,
                       num_relabels,num_global_relabels, num_gaps;
   int alg; int k;
 
   
   {  I.write_demo("MAX_FLOW_GRH + FIFO");
     T  = used_time();
     fifo_set U;
     f = MAX_FLOW_GRH_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,5); 
     C = 0; k = 0;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }
   {  I.write_demo("MAX_FLOW_GAP + FIFO");
     T  = used_time();
     fifo_set U;
     f = MAX_FLOW_GAP_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,num_gaps,5); 
     C = 1; k = 0;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }



 I.stop(); 

  
   {  I.write_demo("MAX_FLOW_GRH + Highest Level");
     T  = used_time();
     hl_set U(2*G.number_of_nodes() - 1);
     f = MAX_FLOW_GRH_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,5); 
     C = 0; k =1;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }
   {  I.write_demo("MAX_FLOW_GAP + Highest Level");
     T  = used_time();
     hl_set U(2*G.number_of_nodes() - 1);
     f = MAX_FLOW_GAP_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,num_gaps,5); 
     C = 1; k =1;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }

   {  I.write_demo("MAX_FLOW");
     T  = used_time();
     f = MAX_FLOW_T(G,s,t,G.edge_data(),flow,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,num_gaps,5); 
     C = 2; k = 1;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }
   /*
  {  I.write_demo("MAX_FLOW_LEDA");
     T  = used_time();
     f = MAX_FLOW(G,s,t,G.edge_data(),flow); 
     C = 3; k = 1;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }
   */
   
   I.stop();
 

  
   {  I.write_demo("MAX_FLOW_GRH + MFIFO");
     T  = used_time();
     mfifo_set U;
     f = MAX_FLOW_GRH_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,5); 
     C = 0; k =2;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }
   {  I.write_demo("MAX_FLOW_GAP + MFIFO");
     T  = used_time();
     mfifo_set U;
     f = MAX_FLOW_GAP_T(G,s,t,G.edge_data(),flow,U,num_pushes,num_edge_inspections,num_relabels,num_global_relabels,num_gaps,5); 
     C = 1; k =2;
     
     if ( y == 1)
     { I.write_demo("number of pushes = ",num_pushes);
       I.write_demo("number of edge inspections = ",num_edge_inspections);
       I.write_demo("number of relabels = ",num_relabels);
       I.write_demo("number of global relabels = ",num_global_relabels);
       I.write_demo("number of gaps = ",num_gaps);
     }
     float UT = used_time(T);
     I.write_demo("running time = ",UT);
     A(48*j + 16*k + 4*C + l,0) = G.number_of_nodes();
     A(48*j + 16*k + 4*C + l,1) = UT;



   }

 
   
   I.stop();


}
 }
#ifdef BOOK
   
   for (j = 0; j < 4; j++)
   { if ( j == 0) I.write_table("rand");
     if ( j == 1) I.write_table("CG1");
     if ( j == 2) I.write_table("CG2");
     if ( j == 3) I.write_table("AMO"); 
     { for (k = 0; k < 3; k++)
       { I.write_table(" & " );
         if (k == 0) I.write_table("FF");
         if (k == 1) I.write_table("HL");
         if (k == 2) I.write_table("MF");
         for (int C = 0; C < 3; C++)
         { for (l = 0; l < 3; l++)
           if ( k != 1 && C >= 2) I.write_table(" & --- ");
             else  I.write_table(" & ", A(48*j + 16*k + 4*C + l,1));
         }
         I.write_table("\\\\ \\hline "); 
         if (k == 2) I.write_table(" \\hline");  
         I.write_table("\n");
       }   
     }
   }


#endif
return 0;
}

