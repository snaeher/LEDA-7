/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  planarity_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:24 $


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <assert.h>
#include <LEDA/graph/node_map2.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main() { {  // I want two blocks so that statistics is outside

IO_interface I("Planarity");

 I.write_demo("We illustrate the speed of algorithms related to planarity. \
There are two planarity tests in LEDA, one based on Lempel, Even, Cederbaum \
and one based on Hopcroft and Tarjan. The former is the faster and uses \
less space.");

 int HT = I.read_int("please type 1 if you also want to see the Hopcroft \
and Tarjan algorithm.",1);

 
 I.write_demo("Please start with n and m around ten-thousand.");

 

graph G;
list<edge> K;

int N = I.read_int("n = ",5000);
int M = I.read_int("m = ",10000);
int n = N; int m = M;
bool first_line = true;
bool is_planar;
int l = 0; 
/*
int L = 1;
*/

for (int i = 0; i < 5; i++)
{ 
float GT=0, BLT=0, BLE=0, BLC=0, HTT=0, HTE=0;



#ifdef BOOK
for (n = N; n <= 4*N; n  *= 2)
{ first_line = (n == N);
GT = BLT = BLE = BLC = HTT = HTE = 0;
m = 2*n;
for (l = 0; l < L; l++)
  {
#endif

float T = used_time();

if (l == 0 )
{ I.write_demo(" " );
  I.write_table("\n");
}

if (i < 3)
{

random_planar_map(G,n,m);



array<node> V(6);
for (int j = 0; j < 6; j++) V[j] = G.choose_node();


if (i % 3 == 0 ) 
{ I.write_demo("Planar Graph");
  if (first_line && l == 0) I.write_table("P");
}  
 
if (i % 3 == 1)
{ I.write_demo("Planar graph + K_3,3 on six random nodes");
  if (first_line && l == 0) I.write_table("P + $K_{3,3}$");
  for (int j = 0; j < 3; j++)
  for (int k = 3; k < 6; k++) 
  { edge e = G.new_edge(V[j],V[k]);
    edge r = G.new_edge(V[k],V[j]);
    G.set_reversal(e,r);
  }

}
if ( i % 3 == 2 )
{ I.write_demo("Planar graph + K_5 on five random nodes");
  if (first_line && l == 0) I.write_table("P + $K_{5}$");
  for (int j = 0; j < 5; j++)
  for (int k = j + 1; k < 5; k++) 
  { edge e = G.new_edge(V[j],V[k]);
    edge r = G.new_edge(V[k],V[j]);
    G.set_reversal(e,r);
  }
}
}
else
{ maximal_planar_map(G,n);

  
  node v = G.choose_node();
  node w; bool OK; edge e;

  do { OK = true;
       w = G.choose_node();
       forall_adj_edges(e,v)
         if ( G.target(e) == w ) OK = false; 
  } while ( OK == false );

  if ( i == 3 ) 
  { I.write_demo("Maximal planar graph");
    if (first_line && l == 0) I.write_table("MP");
  }
  else
  { G.set_reversal(G.new_edge(v,w), G.new_edge(w,v));
    I.write_demo("Maximal planar graph plus one edge");
    if (first_line && l == 0) I.write_table("MP + e");
  }



}

{
list<edge> E = G.all_edges();
E.permute();
G.sort_edges(E);
}


float UT = used_time(T); GT += UT;
I.write_demo("time for generation of graph = ",UT);

BL_PLANAR(G);

UT = used_time(T); BLT +=UT;


I.write_demo("time for planarity test, BL_PLANAR(G): ",UT);
is_planar = BL_PLANAR(G,K,true);

UT = used_time(T); BLE += UT;

I.write_demo("time for planarity test + embedding or Kuratowski, BL_PLANAR(G,K,true): ",UT);


if ( is_planar )
{ assert( Genus(G) == 0 );
  UT = used_time(T); BLC += UT;

  I.write_demo("time for check, Genus(G) == 0: ",UT);


}
else
{ assert( CHECK_KURATOWSKI(G,K) );
  UT = used_time(T); BLC += UT;

  I.write_demo("time for check, CHECK_KURATOWSKI: ",UT); 

}

if (HT == 1)
{ is_planar = HT_PLANAR(G);

  UT = used_time(T); HTT += UT;

  I.write_demo("time for planarity test, HT_PLANAR(G): ",UT);


  if ( is_planar)
  { HT_PLANAR(G,K,true);

    UT = used_time(T); HTE += UT;

    I.write_demo("time for planarity test + embedding, HT_PLANAR(G,K,true): ",UT);

    assert ( Genus(G) == 0 );
  }
  else
  { //HT_PLANAR(G,K,true);

    //UT = used_time(T);

    I.write_demo("no efficient Kuratowski finder in HT_PLANAR ");
  }
}

I.stop();


#ifdef BOOK
}
I.write_table(" & ",GT/L);
I.write_table(" & ",BLT/L);
I.write_table("& ",BLE/L);
I.write_table("& ",BLC/L);
I.write_table("& ",HTT/L);
if (is_planar ) I.write_table("& ", HTE/L);
else I.write_table("& -- ");
I.write_table("\\\\ \\hline");
}
#endif
}

}
#ifdef ASCII
print_statistics();
#endif

return 0;
}

