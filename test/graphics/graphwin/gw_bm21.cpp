/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_bm21.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/bitmaps/button21.h>

using namespace leda;

void about(GraphWin& gw)
{ panel P("About gw_menu");
  string naeher("N%cher",uml_a);
  P.text_item("\\bf\\blue gw_menu\\c8 \\rm\\black written by S. " + naeher);
  P.button("ok");
  P.open(gw.get_window());
}


void func1(GraphWin& gw) { gw.message("Function 1 called."); }
void func2(GraphWin& gw) { gw.message("Function 2 called."); }
void func3(GraphWin& gw) { gw.message("Function 3 called."); }
void func4(GraphWin& gw) { gw.message("Function 4 called."); }
void func5(GraphWin& gw) { gw.message("Function 5 called."); }
void func6(GraphWin& gw) { gw.message("Function 6 called."); }
void func7(GraphWin& gw) { gw.message("Function 7 called."); }
void func8(GraphWin& gw) { gw.message("Function 8 called."); }
void func9(GraphWin& gw) { gw.message("Function 9 called."); }
void func10(GraphWin& gw){ gw.message("Function 10 called.");}
void func11(GraphWin& gw){ gw.message("Function 11 called.");}



int main() {

  GraphWin gw;

 gw.get_window().set_bitmap_colors(black,blue);

  // add selected items to the default menu bar (before gw.display() !)

  gw.set_default_menu(M_FILE | M_EDIT | M_GRAPH | M_WINDOW | M_HELP | M_DONE);


  // add a user defined menu 

  int user_menu = gw.add_menu("My Functions");
  gw.add_simple_call(func1, "func1", user_menu);
  gw.add_simple_call(func2, "func2", user_menu);
  gw.add_simple_call(func3, "func3", user_menu);


  // add some pixmap buttons


  gw.add_simple_call(func1, "func1", 0,21,21,down_21_bits);
  gw.add_simple_call(func2, "func2", 0,21,21,left_21_bits);
  gw.add_simple_call(func3, "func3", 0,21,21,right_21_bits);
  gw.add_simple_call(func4, "func4", 0,21,21,up_21_bits);
  gw.add_simple_call(func5, "func5", 0,21,21,ps_21_bits);
  gw.add_simple_call(func6, "func6", 0,21,21,stop_21_bits);
  gw.add_simple_call(func7, "func7", 0,21,21,help_21_bits);
  gw.add_simple_call(func8, "func8", 0,21,21,tree_21_bits);
  gw.add_simple_call(func9, "func9", 0,21,21,union_21_bits);
  gw.add_simple_call(func10,"func10",0,21,21,poly_21_bits);
  gw.add_simple_call(func11,"func11",0,21,21,inside_21_bits);


  gw.display();

  // add an item to graphwin's "Help" menu (after gw.display() !)

  int help_menu = gw.get_menu("Help");
  if (help_menu)
     gw.add_simple_call(about,"about gw_menu", help_menu);

  gw.edit();

  return 0;
}


