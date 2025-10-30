/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  ortho_test.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_draw.h>
#include <LEDA/graph/graph_alg.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;



main()
{
  int n = read_int("n = ");
  int m = read_int("m = ");

  int count = 0;

for(;;) {

 graph G;

 random_planar_graph(G,n,m);

 Make_Connected(G);
 Make_Simple(G);

 node_array<double>        xpos(G);
 node_array<double>        ypos(G);
 node_array<double>        xrad(G);
 node_array<double>        yrad(G);
 edge_array<list<double> > xbends(G);
 edge_array<list<double> > ybends(G);
 edge_array<double>        xsan(G);
 edge_array<double>        ysan(G);
 edge_array<double>        xtan(G);
 edge_array<double>        ytan(G);

 bool ok = ORTHO_DRAW(G,xpos,ypos,xrad,yrad,xbends,ybends,xsan,ysan,xtan,ytan);

 if (!ok)
 { G.write("ortho.lgr");
   break;
  }
 cout << count++ << endl;
}


}
    
