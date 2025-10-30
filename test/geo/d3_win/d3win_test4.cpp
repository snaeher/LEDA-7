/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3win_test4.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window_stream.h>
#include <LEDA/geo/d3_rat_point.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

int main()
{
 d3_window_stream DW("Selection Test");
 list<d3_rat_point> PR;
 list<d3_point>     PF;
 
 DW.set_selection_color(red);
 
 // generate points ...
 random_d3_rat_points_in_cube(50,300,PR);
 d3_rat_point iter;
 forall(iter,PR) PF.append(iter.to_float());
 
 // generate d3_segments and insert them in DW
 list_item lit;
 d3_window_item n;

 forall_items(lit, PF) {
   if (lit != PF.last()){
     d3_point p1 = PF[lit];
     d3_point p2 = PF[PF.succ(lit)];
     d3_segment seg(p1, p2);
     
     DW << p1 << p2;
     DW.insert_d3_segment(seg,n);
     DW.set_color(n, blue2);
   }
 }
 cout << "first show !\n"; cout.flush();
 DW.show();
 
 list<d3_window_item> sel_items;
 
 // get the selected items ...
 DW.get_selected_items(sel_items);
 
 // mark the selected edges red...
 forall(n, sel_items) DW.set_color(n, orange);
 
 cout << "second show!\n"; cout.flush();
 DW.show();
 
 return 0;
}
