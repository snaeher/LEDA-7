/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_agd.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main() 
{
  GraphWin gw("Leda Graph Editor");

  //gw.set_agd_host("leda.informatik.uni-trier.de");
  gw.set_agd_host("localhost");

  gw.display(window::center,window::center);

  while ( gw.edit() )
    gw.run_agd("Sugiyama");

  return 0;
}

