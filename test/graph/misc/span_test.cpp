/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  span_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



int main(int argc, char** argv)
{

GRAPH<int,int>  G;

cmdline_graph(G,argc,argv);

edge x;
forall_edges(x,G) G[x] = rand_int(0,1000000);

edge_array<int>     cost1(G);
edge_array<double>  cost2(G);

forall_edges(x,G) cost2[x] = cost1[x] = G[x];

list<edge> el;

float T;

cout << "SPANNING_TREE:             ";
cout.flush();
T = used_time();
el = SPANNING_TREE(G);
cout << string(" %5.2f sec   |T| = %d",used_time(T),el.length());
cout << endl;

if (Yes("Print tree edges ? ")) 
{ edge x;
  forall(x,el) { G.print_edge(x); cout << endl; }
 }

el.clear();

cout << "MIN_SPANNING_TREE(int):    ";
cout.flush();
T = used_time();
el = MIN_SPANNING_TREE(G,cost1);
cout << string(" %5.2f sec   |T| = %d",used_time(T),el.length());

int total1 = 0;
forall(x,el)  total1 += cost1[x];

cout << string("   total cost %d\n",total1);

if (Yes("Print tree edges ? ")) 
{ edge x;
  forall(x,el) { G.print_edge(x); cout << endl; }
 }


el.clear();

cout << "MIN_SPANNING_TREE(ugraph): ";
cout.flush();
T = used_time();
el = MIN_SPANNING_TREE(G,cost1);
cout << string(" %5.2f sec   |T| = %d",used_time(T),el.length());

total1 = 0;
forall(x,el)  total1 += cost1[x];

cout << string("   total cost %d\n",total1);

if (Yes("Print tree edges ? ")) 
{ edge x;
  forall(x,el) { G.print_edge(x); cout << endl; }
 }



cout << "MIN_SPANNING_TREE(double): ";
cout.flush();
T = used_time();
el = MIN_SPANNING_TREE(G,cost2);
cout << string(" %5.2f sec   |T| = %d",used_time(T),el.length());

double total2 = 0;
forall(x,el)  total2 += cost2[x];

cout << string("   total cost %f\n",total2);

if (Yes("Print tree edges ? ")) 
{ edge x;
  forall(x,el) { G.print_edge(x); cout << endl; }
 }


return 0;
}
