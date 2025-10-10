/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_panel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>

using namespace leda;

gw_node_shape triangle[4] = 
{ triang0_node, triang1_node, triang2_node, triang3_node};

#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  GraphWin gw;

  window& W = gw.get_window();

  string fname = "graph1.gw";
  color  col   = blue;

  W.string_item("",fname);
  W.color_item("",col);

  gw.set_node_width(48);
  gw.set_node_height(48);

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

  for(int i=1; i<12; i++)
  { gw.set_node_shape(triangle[i%4]);
    leda_wait(0.3);
   }

  return 0;
}

