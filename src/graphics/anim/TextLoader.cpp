/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+ TextLoader.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/graphics/anim/TextLoader.h>

LEDA_BEGIN_NAMESPACE

void TextLoader::load_text(string fn)
{ ifstream in(fn);
  if( !in ) return; 

  int line_number = 1;
  buffer.clear();   

  while(in || line_number <= 12) 
  {
    string s = read_line(in);    

    if(line_number == marked_line) 
      s = s.insert(s.index(search_string), marked_style);
    s = s.replace_all(char(9), tab_string);   
    s = s.replace_all("  ", "~~");        
    if(at_line <= line_number)
    { if(to_line >= line_number)
         buffer.append(s);
      else 
         break;
    }
    line_number++;
  } 
  max_lines = line_number;
}


void TextLoader::display( window * W, int x, int y) 
{ 
  if(!wp->is_open()) {
    wp->set_bg_color(bg_color);
    x_WindowPos = x;  y_WindowPos = y; w_parent=W;       
    W ? wp->display( *W, x, y) : wp->display( x, y );  
  }

  buffer.empty() ? wp->clear() : show_buffer(wp);    
}

void TextLoader::display( int x, int y ) 
{ display( 0, x, y ); }


void TextLoader::display() 
{ display( 0, x_WindowPos, y_WindowPos); }



void TextLoader::extract_frame_label(string fname)
{ 
  ifstream in(fname);

  if (in) 
     wp->set_frame_label(window_label = read_line(in));  
  else
     wp->set_frame_label(window_label = ""); 
}



int TextLoader::get_file_lenght(string fname)
{
  ifstream in(fname);
  if( !in ) return -1;

  int count = 0;
  while(in) {
    read_line(in);
    ++count;
  }
  return --count;
}



void TextLoader::wait_with_timeout(int time)
{ double x = 0, y=0;
  unsigned long t = 0;
  int val = 0, event = 0;
  while((event = wp->read_event(val, x, y, t, time)) != no_event &&
         event != button_press_event &&  event != key_press_event); 
}



void TextLoader::create_menu(string fname)
{ ifstream in(fname);
  if( !in ) return;

  if(button_counter != 0) return;

  button_counter = get_file_lenght(fname);
  for(int count = 0; count < button_counter; count++)
    wp->button(read_line(in), count); 

  if(button_counter > 0) {
    wp->set_button_space(button_space);
    wp->make_menu_bar();
  } 
}



void TextLoader::update_all_button_labels(string fname)
{ ifstream in(fname);
  if( !in ) return;

  if(window_label == "") wp->set_frame_label(fname);
            
  if(button_counter == 0) return;

  for(int count = 0; count < button_counter; count++)
    wp->set_button_label(count, read_line(in)); 
}



void TextLoader::open_with_timeout(int x, int y, int time)
{ display(x, y);
  wait_with_timeout(time);
  close();    
}
                                                                              

int TextLoader::open_with_read_button(int x, int y)
{ display(x, y);
  if(button_counter < 1) return -1;
  int but = wp->read();
  return but;
}



int TextLoader::open_with_read_button()
{ display();
  if(button_counter < 1) return -1;
  int but = wp->read();
  return but;
}


void TextLoader::scroll_down(int)   
{ window *wpt_scrollbar = window::get_call_window();
  window *wpt = (window*)wpt_scrollbar->get_inf();
  TextLoader& TL = *(TextLoader*)wpt->get_client_data();
  
  if (TL.ypos < TL.yhigh)  TL.ypos += TL.txt_height;

  wpt->flush_buffer(0, TL.ypos - TL.ylow);
  double dy = (TL.ypos - TL.ylow) / (TL.yhigh - TL.ylow);

  wpt->set_scrollbar_pos(dy); 
}


void TextLoader::scroll_up(int)
{ window *wpt_scrollbar = window::get_call_window();
  window *wpt = (window*)wpt_scrollbar->get_inf();
  TextLoader& TL = *(TextLoader*)wpt->get_client_data();

  if(TL.ypos > TL.ylow) TL.ypos -= TL.txt_height;

  wpt->flush_buffer(0, TL.ypos - TL.ylow);
  double dy = (TL.ypos - TL.ylow) / (TL.yhigh - TL.ylow);

  wpt->set_scrollbar_pos(dy);
 }


void TextLoader::scroll_drag(int i) 
{ window *wpt_scrollbar = window::get_call_window();
  window *wpt = (window*)wpt_scrollbar->get_inf();
  TextLoader& TL = *(TextLoader*)wpt->get_client_data();

  if(i >= 0) { 
    double dy = i/1000.0; 
    TL.ypos = TL.ylow + dy * (TL.yhigh - TL.ylow); 
  } else if(i == -2) { 
      int li = int (0.5 + (TL.ypos - TL.ylow) / TL.txt_height);
      TL.ypos = TL.ylow + li * TL.txt_height;
    }

  wpt->flush_buffer(0, TL.ypos - TL.ylow);
}

void TextLoader::draw_buffer(window *wp)
{ 
  TextLoader& TL = *(TextLoader*)wp->get_client_data();

  double y = wp->ymax() - TL.ymin_TextBox;	
  double pix = wp->pix_to_real(1);

  wp->start_buffering();
  wp->clear();

  string s;
  forall(s, TL.buffer)
  { y = wp->text_box(TL.xmin_TextBox, TL.xmax_TextBox, y, s);
    y -= pix;
  }   

  TL.ylow  = y; 

  wp->flush_buffer();
  wp->stop_buffering();
}


void TextLoader::show_buffer(window *wp)
{ 
  TextLoader& TL = *(TextLoader*)wp->get_client_data();

  TL.txt_height = wp->text_height("Z");


  double y0    = wp->ymax() - TL.ymin_TextBox;	
  double y1    = wp->ymin();
  TL.yhigh = y0 - int((y0 - y1) / TL.txt_height) * TL.txt_height;

  draw_buffer(wp);

  if( TL.setScrollPosition==false ) TL.ypos=TL.ylow;
	

  double yt    = TL.txt_height * TL.buffer.length() + TL.ymin_TextBox;

  double f = (y0 - y1) / yt;


  if (f < 1 && TL.ShowScrollBar) 
    wp->open_scrollbar(scroll_up,scroll_down,scroll_drag,f,TL.ypos-TL.ylow );
  else 
    wp->close_scrollbar();  
}


void TextLoader::init_parameters()
{ 
  ShowTexture = false;

  bg_color=white;
  
  x_WindowPos = window::center;
  y_WindowPos = window::center;
  
  xmin_TextBox = wp->pix_to_real(3);
  ymin_TextBox = wp->pix_to_real(3);
  xmax_TextBox = wp->xmax() - wp->pix_to_real(5);

  max_lines = 300;
  at_line = 1;
  to_line = max_lines;
  
  marked_style = "";
  search_string = "";
  marked_line = max_lines + 1;
  
  ShowScrollBar = false;
  setScrollPosition=false;
    
  ypos=0;
  
  set_tabulator_stops(2);

  button_space = 5;
  button_counter = 0;
  
  w_parent=0;

  wp->set_redraw(draw_buffer);
  wp->set_client_data(this);
}

LEDA_END_NAMESPACE
