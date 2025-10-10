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


#include <LEDA/graphics/graphwin.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::cin;
using std::endl;
#endif


int main(int argc, char** argv) 
{
  GraphWin gw;

  window& W = gw.get_window();

  string fname = "graph1.gw";
  color  col   = blue;

  W.string_item("",fname);
  W.color_item("",col);

  gw.set_node_width(32);
  gw.set_node_height(32);

  gw.set_node_shape(triang0_node);

  gw.display();

  if (argc > 1) 
  { string fname = argv[1];
    gw.set_filename(fname);
    if (fname.tail(4) == ".gml")
       gw.read_gml(fname);
    else
       gw.read_gw(fname);
   }

  gw.edit();

  gw.set_node_shape(triang1_node);
  gw.edit();

  gw.set_node_shape(triang2_node);
  gw.edit();

  gw.set_node_shape(triang3_node);
  gw.edit();

  return 0;
}

