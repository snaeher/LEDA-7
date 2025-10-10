/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  read_mouse2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif


int main() 
{ 
  window W;

  W.button("button 0");
  W.button("button 1");
  W.button("button 2");
  W.button("button 3");

  int exit_but = W.button("exit");

  W.display();

  for(;;)
  { int but =  W.read_mouse();

    if (but == exit_but) break;

    switch (but) {
      case MOUSE_BUTTON(1): cout << "left button click" << endl;
                            break;
      case MOUSE_BUTTON(2): cout << "middle button click" << endl;
                            break;
      case MOUSE_BUTTON(3): cout << "right button click" << endl;
                            break;
      default:  cout << string("panel button: %d",but) << endl;
                break;
    }
  }

  W.screenshot("read_mouse2");
  return 0;
}
