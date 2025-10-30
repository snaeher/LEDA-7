/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_convert.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main(int argc, char** argv) 
{
  GraphWin gw;
  gw.display();

  for(int i=1; i<argc; i++)
  { gw.set_filename(argv[i]);
    gw.read_gw(argv[i]);
    gw.save_gw(argv[i]);
   }

  return 0;
}

