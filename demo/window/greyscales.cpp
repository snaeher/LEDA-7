/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  greyscales.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <LEDA/core/array.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

int main()
{
  array<color> grey(256);

  for(int i=0; i<256; i++) grey[i] = color(i,i,i);

  return 0;
}
