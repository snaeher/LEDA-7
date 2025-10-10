/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  size.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>

using namespace leda;

using std::cout;
using std::endl;



int main()
{
  cout << "node: " << sizeof(node_struct) << endl;
  cout << "edge: " << sizeof(edge_struct) << endl;
  cout << "face: " << sizeof(face_struct) << endl;

  return 0;
}
