/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  view.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef VIEW_H
#define VIEW_H

#include <LEDA/graphics/anim/view_base.h>


LEDA_BEGIN_NAMESPACE

class standard_view_menu : public base_action
{ view&   V;
  window* M;
    
  public:  
  
  standard_view_menu(view& v); 
 ~standard_view_menu() { delete M; }  
  
  void redraw();  
  int handle_event(int event, int but, double x, double y);  
  window* get_window() const { return M; }
};


class standard_view_action : public base_action
{ view& V;  

  public:

  standard_view_action(view& v) : V(v) {}   

  int handle_event(int event, int but, double x, double y);  
  window* get_window() const { return &V.get_window(); }  
};

LEDA_END_NAMESPACE

#endif
