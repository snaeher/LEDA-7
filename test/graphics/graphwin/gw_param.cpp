/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_param.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

template <class vtype, class etype>
class MyGraphWin : public GraphWin {

  GRAPH<vtype,etype>& g_ref;

public:

 GRAPH<vtype,etype>& get_graph() { return g_ref; }

 MyGraphWin(GRAPH<vtype,etype>& G, const char* L="") : GraphWin(G,L),g_ref(G){}
~MyGraphWin() {}
};

  

int main()
{
   GRAPH<string,string> G;

   node v = G.new_node("node 1");
   node w = G.new_node("node 2");
 //edge e = 
            G.new_edge(v,w,"edge 1");

   MyGraphWin<string,string> gw(G);

   gw.display();
   gw.edit();

   return 0;
}

 

