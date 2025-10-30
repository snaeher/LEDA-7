/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_tooltip.c
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

void new_node_handler(GraphWin& gw, node v)
{ window& W = gw.get_window();
  point p = gw.get_position(v);
  double r1 = gw.get_radius1(v);
  double r2 = gw.get_radius2(v);
  double x0 = p.xcoord() - r1;
  double y0 = p.ycoord() - r2;
  double x1 = p.xcoord() + r1;
  double y1 = p.ycoord() + r2;
  W.set_tooltip(index(v),x0,y0,x1,y1,string("node ") + gw.get_label(v));
}

bool del_node_handler(GraphWin& gw, node v)
{ window& W = gw.get_window();
  W.del_tooltip(index(v));
  return true;
}


int main() 
{
  GraphWin gw(450,500,"Leda Graph Editor");

  graph& G = gw.get_graph();

  gw.set_agd_host("leda.informatik.uni-trier.de");

  gw.set_new_node_handler(new_node_handler);
  gw.set_del_node_handler(del_node_handler);

  gw.set_end_move_node_handler(new_node_handler);

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

