/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>


using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


int main() 
{
  //window::do_not_open_display(1);

  GraphWin gw("Leda Graph Editor");
  graph& G = gw.get_graph();

  //gw.set_agd_host("leda.uni-trier.de");
  gw.set_agd_host("localhost");

  gw.set_node_color(red);
  gw.set_node_width(48);

  gw.display(window::center,window::center);

  while ( gw.edit() )
  { if (Is_Planar(G))
      cout << "This graph is planar." << endl;
    else
      cout << "This graph is non-planar." << endl;
    gw.run_agd("Sugiyama");
   }

  cout << "exit gw" << endl;

  return 0;
}

