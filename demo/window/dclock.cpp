/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dclock.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/window.h>
#include <time.h>

using namespace leda;


bool seconds = false;

void display_time(window* wp)
{ 
  window& W = *wp;

  // get the current time
  time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 
  string s("%d:%02d:%02d",T->tm_hour,T->tm_min,T->tm_sec);

  // and display it (centered in W)

//W.start_buffering();

  W.clear();
  double  x = (W.xmax() - W.xmin())/2;
  double  y = (W.ymax() - W.ymin())/2;
  y += W.pix_to_real(int(0.07*W.height()));
  W.draw_ctext(x,y,s);

  W.flush_buffer();
//W.stop_buffering();

  W.set_frame_label(s);
}

void display_ftime(window* wp) 
{ 
  window& W = *wp;

  // get the current time

  time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 
  string s("%d:%02d:%02d",T->tm_hour,T->tm_min,T->tm_sec);
  int len = s.length();

  // and display it (centered in W)

  double c_width = 0.75 * W.text_width("H");
  double c_height = 1.1 * W.text_height("H");

  double x = (W.xmax() - W.xmin())/2 - len*c_width/2;
  double y = (W.ymax() - W.ymin())/2 + c_height/2;

  W.set_node_width(W.real_to_pix(c_width)/8);

//W.start_buffering();

  W.clear();

  //W.draw_point(x,y,red);


  for(int i=0; i<len; i++)
  { char c = s[i];
    //W.draw_rectangle(x,y,x+c_width,y-c_height,blue2);
    if (c == ':')
     { //W.draw_text(x,y+0.075*c_height,string(c));
       double cw = 0.70*c_width;
       double x0 = x + 0.75*cw;
       double y1 = y - 0.35*c_height;
#if defined(__win32__)
       double y2 = y - 0.50*c_height; 
#else
       double y2 = y - 0.60*c_height; 
#endif
       W.draw_filled_node(x0,y1);
       W.draw_filled_node(x0,y2);
       x += cw;
      }
    else
      { W.draw_text(x,y,string(c));
        x += c_width;
       }
   }

  W.flush_buffer();
//W.stop_buffering();

  W.set_frame_label(s);

}



int main()
{
  //window W(1500,750, "dclock");
  //window W(1500,750, "dclock");
  window W(600,300, "dclock");
  
/*
  W.set_bg_color(black);
  W.set_fg_color(white);
*/
  W.set_bg_color(ivory);
  W.set_fg_color(black);

  W.display(window::center,window::center);
  W.set_font("T150");

  W.set_redraw(display_ftime);

W.start_buffering();

  W.start_timer(1000,display_ftime);
  W.read_mouse();

W.stop_buffering();

  //W.screenshot("dclock");
  return 0;
}
