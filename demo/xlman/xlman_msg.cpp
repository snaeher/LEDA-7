/*******************************************************************************
+
+  LEDA 6.6.1  
+
+
+  xlman_msg.c
+
+
+  Copyright (c) 1995-2020
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>

using namespace leda;



void client_text_redraw(window* win) 
{ win->draw_ctext((char*)win->get_client_data()); }

void alarm_msg(string msg)
{ window W(1,1,"Xlman Alarm Message");
  W.display(window::center,window::center);
  W.set_client_data(msg.cstring());
  W.set_redraw(client_text_redraw);
  //W.set_window_delete_handler(client_text_redraw);
  W.set_font("T48");
  int xpos = W.xpos();
  int ypos = W.ypos();
  double tw = 1.2*W.text_width(msg);
  double th = 2.0*W.text_height(msg);
  int ww = W.real_to_pix(tw);
  int wh = W.real_to_pix(th);
  W.resize(xpos-ww/2,ypos-wh/2,ww,wh);
  client_text_redraw(&W);
  W.read_mouse();
}


#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

  string msg; 
  for(int i = 1; i <argc; i++) 
  { msg += argv[i];
    msg += " ";
   }
  alarm_msg(msg);
  return 0;
}

