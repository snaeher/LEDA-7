/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  file_panel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h>


using namespace leda;

using std::cout;
using std::cin;
using std::endl;


enum { FILE_LOAD, FILE_SAVE, FILE_ALL, FILE_EXIT };


//string dname = ".";
string dname = "/home/naeher";
string fname = "none";

window* win_ptr;


// dummy I/O handler

void read_file(string fn) { cout << "reading file " << fn << endl; }

void write_file(string fn) { cout << "writing file " << fn << endl; }



void file_handler(int what)
{ 
  file_panel FP(*win_ptr,fname,dname);

  FP.set_pattern("all files","*");
  FP.set_mswin(false);

  FP.set_xhost("chomsky.uni-trier.de");

  switch (what) {
  case FILE_LOAD: FP.set_load_handler(read_file);
                  break;
  case FILE_SAVE: FP.set_save_handler(write_file);
                  break;
  case FILE_ALL:  FP.set_load_handler(read_file);
                  FP.set_save_handler(write_file);
                  break;
  }
  FP.open();
}


int main()
{
  menu file_menu;
  file_menu.button("Load File",FILE_LOAD,file_handler);
  file_menu.button("Save File",FILE_SAVE,file_handler);
  file_menu.button("Load/Save",FILE_ALL, file_handler);
  file_menu.button("Exit",FILE_EXIT);

  menu help_menu;
  help_menu.button("button1");
  help_menu.button("button2");
  help_menu.button("button3");

  window W;
  win_ptr = &W;

  W.button("File", file_menu);
  W.button("Help", help_menu);
  W.button("Exit", FILE_EXIT);
  W.make_menu_bar();

  W.display(window::center,window::center);

  while (W.read_mouse() != FILE_EXIT);

  return 0;

}
