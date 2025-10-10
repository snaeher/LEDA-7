/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  rat_n_gon_time.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// $Revision: 1.1.1.1 $  $Date: 2005/09/12 10:00:14 $


#include <LEDA/core/IO_interface.h>
#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/geo/rat_transform.h>

#include <LEDA/geo/rat_kernel_names.h>
#define N_GON n_gon

using namespace leda;

using std::cout;
using std::endl;



float S;
float TI;
IO_interface I("Union of n-gons");


void report_time(string s)
{ S = used_time(TI);
  I.write_table(" & ",S);
  I.write_demo(s,S);
}

int main(){

I.write_demo("We compute the union of a regular n-gon P with a \
copy Q of P obtained by rotating P around its center by 2*pi/(2*n*m) \
degrees. We output the transformation matrix and the number of vertices \
of the union. The number of vertices of the output should be 4n.");

I.write_demo("There are two versions of this demo: n_gon_time and rat_n_gon_time.\
The rational version is guaranteed to give the correct result. The floating point \
version may give an incorrect result or may even crash. Try \
n = 5000 and m = 1.581e+09.");



int N = 5000;

/*
#if (N_GON == n_gon)
N = 5000; 
#else
N = 500;
#endif
*/

int n = N;

//double M = 2 * N;

double m;

POINT ORIGIN(0,0);
POINT X(1,0);
CIRCLE C(ORIGIN,X);

#ifdef BOOK
for (n = N; n <= 4 * N; n = 4 * n)
for (m = M = 1235*n; m < 1000*M; m = 4*m)
{
#else
while (true)
{ 
n = I.read_int("n = ",1000);
m = I.read_double("rotation by degree 2*pi/(2*n*m); m = ", 2000);
#endif

#ifdef FIXED_INPUT
cout <<"\n\nFIXED INPUT: n = 5000 and m = 1.581e+09";
n = 5000;
m = 1.581e+09;
#endif

I.write_table("\n ", n);
I.write_table(" & ", m);

TI = used_time(); 


double eps = 1/(10.0*n*m);

POLYGON P = N_GON(n,C,eps);

GEN_POLYGON PG(P,GEN_POLYGON::NO_CHECK);

report_time("time to generate P = ");

TRANSFORM T = rotation(ORIGIN, LEDA_PI/(n * m), eps);

report_time("time to generate the transformation T = ");

POLYGON Q = T(P); 
GEN_POLYGON QG(Q,GEN_POLYGON::NO_CHECK);

report_time("time to compute T(P) = ");

GEN_POLYGON R = PG.unite(QG);

report_time("time to compute P union T(P) = ");

  

#ifndef BOOK
cout << "\n\nThe transformation matrix\n\n" << T.T_matrix();
#endif

if ( R.empty() )
{ I.write_table(" &  empty");
  I.write_demo("R is empty");
}
if ( R.full() )
{ I.write_table(" &  full");
  I.write_demo("R is full");
}
if ( !R.trivial() )
{ I.write_table(" &  ",R.size());;
  I.write_demo("number of edges of R (should be 4n)",R.size());
}

#ifdef FULL_OUTPUT
cout << "\n\norientation of P = " << P.orientation();

cout << "\n\norientation of Q = " << Q.orientation(); 

cout.flush();


{ const GEN_POLYGON& P1 = QG;

  const GEN_POLYGON& P0 = PG;

  GRAPH<POINT,SEGMENT> G;

  //const GEN_POLYGON& P0 = *this;

  edge_array<int> label;

  construct_labeled_map(P0,P1,G,label);

  cout << "\nnumber of nodes = " << G.number_of_nodes();
    
  cout << "\nnumber of edges = " << G.number_of_edges();

  cout <<"\nGenus of G = " << Genus(G);  

  array<int> degree_count(10);
  int j;
  for (j = 0; j < 10; j++) degree_count[j] = 0;

  node v;
  forall_nodes(v,G) degree_count[G.outdeg(v)]++;

  for (j = 0; j < 10; j++) 
   cout <<"\n\nnumber of nodes of degree " << j << " = " <<degree_count[j];

  
    
  // label relevant edges, stage IV

  edge_array<bool> relevant(G,false);

  int d = not_P0_face + not_P1_face;
  edge e; int relevant_count = 0;
  forall_edges(e,G) 
    if (label[e] != d) { relevant_count++; relevant[e] = true; }
   
  cout <<"\n\nnumber of relevant edges = " << relevant_count;
  // stages V and VI
   

   bool non_trivial_result = simplify_graph(G,relevant);

  cout << "\n\n" << (non_trivial_result ? "full" : " empty");

   cout <<"\n\nnumber of nodes = " << G.number_of_nodes();
   cout << "\nnumber of edges = " << G.number_of_edges();


edge_array<bool> visited(G,false);

list<POLYGON> result;

/*

cmp_for_cycle_tracing<POINT,SEGMENT> cmp(G); 

list<edge> E = G.all_edges();
E.sort(cmp);

edge e0;
forall(e0,E)
{ edge e = e0;
  if ( visited[e] || visited[G.reversal(e)]) continue;
  int orient;
  if ( relevant[e] ) 
    { orient = +1; }
  else
    { e = G.reversal(e); orient = -1; }
  list<SEGMENT> pol;
  collect_polygon(G,e,visited,pol);
  POLYGON P(pol,orient);
  result.append(P);
} 

*/

}

cout.flush();


#endif
 
I.write_table(" \\\\ \\hline");



if ( I.read_char("Press Y to continue ",'Y') != 'Y' ) break;

}

I.write_table(" \\hline");

}


