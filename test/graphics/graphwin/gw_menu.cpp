/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_menu.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/pixmaps/button32.h>

using namespace leda;

void about(GraphWin& gw)
{ panel P("About gw_menu");
  string naeher("N%cher",uml_a);
  P.text_item("\\bf\\blue gw_menu\\c8 \\rm\\black written by S. " + naeher);
  P.button("ok");
  P.open(gw.get_window());
}

void show_msg(GraphWin& gw, string msg)
{ gw.message(msg);
  leda_wait(1);
  gw.message("");
}

void func1(GraphWin& gw) { show_msg(gw,"\\bf Function 1 called."); }
void func2(GraphWin& gw) { show_msg(gw,"\\bf Function 2 called."); }
void func3(GraphWin& gw) { show_msg(gw,"\\bf Function 3 called."); }
void func4(GraphWin& gw) { show_msg(gw,"\\bf Function 4 called."); }
void func5(GraphWin& gw) { show_msg(gw,"\\bf Function 5 called."); }
void func6(GraphWin& gw) { show_msg(gw,"\\bf Function 6 called."); }
void func7(GraphWin& gw) { show_msg(gw,"\\bf Function 7 called."); }
void func8(GraphWin& gw) { show_msg(gw,"\\bf Function 8 called."); }
void func9(GraphWin& gw) { show_msg(gw,"\\bf Function 9 called."); }
void func10(GraphWin& gw){ show_msg(gw,"\\bf Function 10 called.");}
void func11(GraphWin& gw){ show_msg(gw,"\\bf Function 11 called.");}



int main() {

  GraphWin gw;

  // add selected items to the default menu bar (before gw.display() !)

  gw.set_default_menu(M_FILE | M_EDIT | M_GRAPH | M_WINDOW | M_HELP | M_DONE);


  // add a user defined menu 

  int user_menu = gw.add_menu("My Functions");
  gw.add_simple_call(func1, "func1", user_menu);
  gw.add_simple_call(func2, "func2", user_menu);
  gw.add_simple_call(func3, "func3", user_menu);


  // add some pixmap buttons

  char* pr1  = gw.get_window().create_pixrect(hand_xpm);
  char* pr2  = gw.get_window().create_pixrect(person_xpm);
  char* pr3  = gw.get_window().create_pixrect(people_xpm);
  char* pr4  = gw.get_window().create_pixrect(pencil_xpm);
  char* pr5  = gw.get_window().create_pixrect(solids_xpm);
  char* pr6  = gw.get_window().create_pixrect(blocks_xpm);
  char* pr7  = gw.get_window().create_pixrect(computer_xpm);
  char* pr8  = gw.get_window().create_pixrect(graph_xpm);
  char* pr9  = gw.get_window().create_pixrect(watch_xpm);
  char* pr10 = gw.get_window().create_pixrect(maze_xpm);
  char* pr11 = gw.get_window().create_pixrect(books_xpm);

  gw.add_simple_call(func1, "func1", 0,pr1);
  gw.add_simple_call(func2, "func2", 0,pr2);
  gw.add_simple_call(func3, "func3", 0,pr3);
  gw.add_simple_call(func4, "func4", 0,pr4);
  gw.add_simple_call(func5, "func5", 0,pr5);
  gw.add_simple_call(func6, "func6", 0,pr6);
  gw.add_simple_call(func7, "func7", 0,pr7);
  gw.add_simple_call(func8, "func8", 0,pr8);
  gw.add_simple_call(func9, "func9", 0,pr9);
  gw.add_simple_call(func10,"func10",0,pr10);
  gw.add_simple_call(func11,"func11",0,pr11);


  gw.display();

  // add an item to graphwin's "Help" menu (after gw.display() !)

  int help_menu = gw.get_menu("Help");
  if (help_menu)
     gw.add_simple_call(about,"about gw_menu", help_menu);

  gw.edit();

  return 0;
}


