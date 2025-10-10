/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/map.h>
#include <LEDA/core/list.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


int main()
{
  int N      = read_int("# keys = ");
  int i;

  map<int,list<int>*>  xx(0);

  map<int,string> M("xyz");

  for(i=0; i< N; i++) M[i] = string("%d",i);

//forall_defined(i,M) cout << i <<endl;

  return 0;
}

