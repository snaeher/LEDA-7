/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  menu_bar.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  menu M;
  M.button("button 1");
  M.button("button 2");
  M.button("button 3");
  M.button("button 4");
  M.button("button 5");

  window W(400,300,"Menu Demo");

  W.button("File",M);
  W.button("Edit",M);
  W.button("Help",M);
  W.button("exit");

  if (argc > 1 && string(argv[1]) == "menu_bar")
     W.make_menu_bar();

  W.display();

  cout << endl;
  cout << "height of menu bar: " << W.menu_bar_height() << endl;
  cout << endl;

  W.read_mouse();

  W.screenshot("menu_bar");
  return 0;
}
