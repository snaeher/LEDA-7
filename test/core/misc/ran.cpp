/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  ran.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

int main()
{

  char* ptr = NULL;

  

  random_source ran;
  bool b;

  for(int i=0; i< 100; i++)
  { ran >> b;
    cout << b <<endl;
   }

return 0;
}
