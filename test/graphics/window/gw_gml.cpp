/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_gml.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include<LEDA/graphics/graphwin.h>
#include<LEDA/graphics/panel.h>

using namespace leda;

string read_filename("graph.gml");
string write_filename("new_graph.gml");

void read(GraphWin& GW)
{
  panel P("read GML...");
  P.string_item("file name",read_filename,
                "Enter the name of a file containing a graph in GML format.");
  P.open();
  GW.read_gml(read_filename);
}

void save(GraphWin& GW)
{
  panel P("save GML...");
  P.string_item("file name",write_filename,
                "Enter a file name for saving the graph in GML format.");
  P.open();
  GW.save_gml(write_filename);
}


int main()
{
  GraphWin GW;

  gw_add_simple_call(GW,read,"read GML...");
  gw_add_simple_call(GW,save,"save GML...");

  GW.open();
}


