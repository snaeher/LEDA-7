/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_many.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cout;
using std::endl;
#endif

int main()
{
  GraphWin gw1(500,500,"GraphWin1");
  gw1.display(0,0);

  GraphWin gw2(500,500,"GraphWin2");
  gw2.display(520,0);


  for(;;)
  { window* wp; 
    int but;
    double x,y;
    int e = read_event(wp,but,x,y);

    if (wp == &gw1.get_window()) 
    { gw1.edit_loop(e,but,x,y);
      if (e == button_press_event) 
      { int res = gw1.get_edit_result();
        if (res == 0) break; //exit
        if (res == 1) cout << "gw1: done" << endl;
        gw1.reset_edit_result();
       }
    }

    if (wp == &gw2.get_window()) 
    { gw2.edit_loop(e,but,x,y);
      if (e == button_press_event) 
      { int res = gw2.get_edit_result();
        if (res == 0) break; //exit
        if (res == 1) cout << "gw2: done" << endl;
        gw2.reset_edit_result();
       }
     }
  }

  return 0;
}
   
