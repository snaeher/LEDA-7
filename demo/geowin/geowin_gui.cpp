/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  geowin_gui.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// demonstrates the usage for more than one window
// in the edit - loop
#include <LEDA/graphics/geowin.h>

using namespace leda;

using std::cout;


// -----------------------------------------------------------
// handlers for the added menus ...
// -----------------------------------------------------------

class my_handler : public window_handler {
  string s;
public:
  my_handler(string name) : s(name) { }
  my_handler(const my_handler& mh) { s = mh.s; }
  
  virtual ~my_handler() { }
  
  virtual void operator()() {
    cout << s << " was pressed.\n"; 
    cout << "get_int:" << get_int() << "\n";
  }  
};

menu M;
my_handler MH1(string("button number 1"));
my_handler MH2(string("button number 2"));
my_handler MH3(string("button number 3"));

void menu_fcn(window& win)
{
  M.button("button 1",1,MH1);
  M.button("button 2",2,MH2);
  M.button("button 3",3,MH3);
  win.button("Additional",M);
}

// -----------------------------------------------------------

double xlast=-1, ylast=-1;

bool event_fcn(window* win, int event, int but, double x, double y)
{
/*
   cout << "event:" << event << " button:" << but << " x:" << x << " y:" << y << "\n"; 
*/   
   if (event == motion_event){
    if (xlast != -1){
     win->draw_segment(xlast,ylast,x,y);
     xlast=x;
     ylast=y;
    }
   }
   
   if (event == button_release_event){
     xlast=-1;
     ylast=-1;
   }

   if (event == button_press_event){
    if (but == MOUSE_BUTTON(1)){
      win->draw_point(x,y);
      xlast=x; ylast=y;
    }
   
    if (but == 1) return true;
   
    if (but == 2){
     cout << "We show the usage of multiple windows in the GeoWin event loop !\n";
    }
   }
   
   return false;
}

int main() 
{
  GeoWin gw("Example for changing the GUI");
  
  // switch off two menus ...
  gw.set_show_algorithms_menu(false);
  gw.set_show_help_menu(false);
 
  // add a menu ...
  gw.set_menu_add_fcn(menu_fcn);  
  
  window other(300,300,"Draw with the mouse");
  other.button("Quit", 1);
  other.button("Message", 2);
  other.display();

  // registering of the other window
  // so that it is handled in the event loop 
  gw.register_window(other, event_fcn);

  gw.edit();
  
  return 0;
}
