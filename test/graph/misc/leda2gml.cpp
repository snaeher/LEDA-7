/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  leda2gml.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// convert graph in LEDA format to GML format

#include<LEDA/graph/graph.h>
#include<LEDA/numbers/integer.h>

using namespace leda;

using std::cerr;
using std::endl;



int main(int argc, char** argv)
{
  if(argc != 3)
  {
    cerr << argv[0] << ": usage is\n\t";
    cerr << argv[0] << " <file1> <file2>\nwhere <file1> contains ";
    cerr << "a graph in LEDA format. It is coverted and written\n";
    cerr << "to file <file2> in GML format.\n";
    return 1;
  }

  GRAPH<integer,double> G;

  G.read(argv[1]);

  G.write_gml(argv[2]);

  return 0;
}
