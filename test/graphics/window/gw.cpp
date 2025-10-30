/*******************************************************************************
+
+  LEDA 7.2.2  
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

GraphWin gw;

list<string> agd_list;


void run_agd(int i) 
{ 
  list_item it = agd_list.get_item(i);

  string alg = agd_list[it]; 

  string ifile = "/tmp/agd.in"; 
  string ofile = "/tmp/agd.out"; 

  gw.save(ifile);
  system("agd_server " + alg + " " + ifile + " " + ofile);
  gw.read(ofile);
  system("rm -f " + ifile);
  system("rm -f " + ofile);
  gw.fill_window();
}


int main()
{
   // get agd_list from server
   // system("agd_server  list  /tmp/agd.list");
   // agd_list.read(/tmp/agd.list);
   // system("rm -f /tmp/agd.list");

   // testing
   agd_list.append("Layout1");
   agd_list.append("Layout2");
   agd_list.append("Layout3");
   agd_list.append("Layout4");
   agd_list.append("Layout5");

   menu M;
   int i=0;
   string s;
   forall(s,agd_list) M.button(s, i++, run_agd);

   gw.add_menu ("AGD",M);

   gw.display();
   gw.edit();

   return 0;
}

