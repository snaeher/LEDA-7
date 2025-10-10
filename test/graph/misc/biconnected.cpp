/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  biconnected.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




int main(){

for (int n = 1; n < 100; n++)
  { cout << "n= " << n << endl;

for (int m = 0; m < 100; m++)
{ graph G;

random_graph(G,n,m);

/*
  Delete_Loops(G);
  G.make_undirected();
*/

edge_array<int> compnum(G);
int c_num = BICONNECTED_COMPONENTS(G, compnum);
 
//cout << "\n\nThe number of biconnected components of G is equal to " << c_num;

//cout << endl; cout << endl;

node_array<int> Comp(G);

int c = COMPONENTS(G,Comp);

for (int i = 0; i < c; i++)
{ GRAPH<node,edge> H;
  node_array<node> link(G,nil);
  node v;
  forall_nodes(v,G) 
    if (Comp[v] == i) link[v] = H.new_node();
  if (H.number_of_nodes() == 1) c_num--;
  else
  { edge e;
    forall_edges(e,G)
    { node v = G.source(e); node w = G.target(e);
      if ( Comp[v] == i && i == Comp[w] ) H.new_edge(link[v],link[w]);
    }
    edge_array<int> compnumH(H);
    c_num -= BICONNECTED_COMPONENTS(H,compnumH);
  }
}

 if (c_num !=  0) cout << "n = " << n << "  m = " << m << endl;

}
}

return 0;
}
