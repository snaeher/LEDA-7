/*******************************************************************************
+
+  LEDA 7.2  
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
  int w = 1200;
  int h = 800;

  int disp_w = window::screen_width();
  int disp_h = window::screen_height();

  if (w > disp_w/2) w = disp_w/2;
  if (h > disp_h/2) h = disp_h/2;

  int sz = 5;

  GraphWin** GW = new GraphWin*[sz];


  for(int i=0; i<sz; i++)
  { GW[i] = new GraphWin(w,h,string("Graphwin %d",i+1));
    GW[i]->display(200*i+30,150*i+30);
   }

  while (sz > 0)
  {
    for(int i=0; i<sz; i++) {
      if (GW[i] && GW[i]->get_edit_result() == 1) { 
         //stop = true;
         delete GW[i];
         GW[i] = GW[--sz];
      }
      
    }

    for(int i=0; i<sz; i++) {
      if (GW[i]) GW[i]->reset_edit_result();
    }

    window* wp; 
    int but;
    double x,y;
    int e = read_event(wp,but,x,y);

    if (wp) {
      GraphWin* gwptr = wp->get_graphwin();
      if (gwptr) gwptr->edit_loop(e,but,x,y);
    }


/*
    if (wp == &gw1.get_window()) 
    { gw1.edit_loop(e,but,x,y);
      if (e == button_press_event) 
         cout << "gw1: " << gw1.get_edit_result() << endl;
     }

    if (wp == &gw2.get_window()) 
    { gw2.edit_loop(e,but,x,y);
      if (e == button_press_event) 
         cout << "gw2: " << gw2.get_edit_result() << endl;
     }
*/
  }

  for(int i=0; i<sz; i++) delete GW[i];
  delete[] GW;

  return 0;
}
   
