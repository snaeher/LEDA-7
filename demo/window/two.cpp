#include <LEDA/graphics/graphwin.h>

using namespace leda;


int main()
{
   GraphWin gw1(1200,1500,"GraphWin 1");
   gw1.display(30,50);

   GraphWin gw2(1200,1500,"GraphWin 2");
   gw2.display(-(window::screen_width()-30), 50);

   for(;;)
   { window* wp;
     double x,y;
     int val;

     if (read_event(wp,val,x,y) != button_press_event) continue;

     if (val == MOUSE_BUTTON(3)) break;

     put_back_event();

     if (wp == gw1.get_window()) { 
       gw1.edit();
/*
       gw1.handle_edit_event();
       gw1.edit_loop(int event, window*& wp, int& but, double& x, double& y);
*/
     }

     if (wp == gw2.get_window()) { 
       gw2.edit();
     }

    }

  return 0;
}
