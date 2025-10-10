/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  menu_bar_obj.c
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



class my_handler : public window_handler {
  string s;
public:
  my_handler(string name) : s(name) { }
  my_handler(const my_handler& mh) { s = mh.s; }
  
  virtual void operator()() {
    cout << s << " was pressed.\n"; 
    cout << "get_int:" << get_int() << "\n";
  }
  
  
};


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif
  my_handler MH1(string("button number 1"));
  my_handler MH2(string("button number 2"));
  my_handler MH3(string("button number 3"));
  my_handler MH4(string("button number 4"));
  my_handler MH5(string("button number 5"));

  menu M;
  M.button("button 1",1,MH1);
  M.button("button 2",2,MH2);
  M.button("button 3",3,MH3);
  M.button("button 4",4,MH4);
  M.button("button 5",5,MH5);

  window W(400,300,"Menu Demo");

  W.button("File",M);
  W.button("Edit",M);
  W.button("Help",M);
  W.button("exit",6);

  if (argc > 1 && string(argv[1]) == "menu_bar")
     W.make_menu_bar();

  W.display();
  int bt;
  
  do {
   bt = W.read_mouse();
  }
  while (bt != 6);
  
  W.screenshot("menu_bar.ps");
  return 0;
}
