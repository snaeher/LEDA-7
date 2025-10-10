/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  TextLoader.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef TextLoader_H
#define TextLoader_H

#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>
#include <LEDA/system/stream.h>

LEDA_BEGIN_NAMESPACE

class __exportC TextLoader {

  private:

  window *wp;
  window *w_parent;
    
  string  marked_style;
  string  search_string;
  string  tab_string;
  int     marked_line;
  int     at_line;
  int     to_line;
  int     max_lines;

  double  ymin_TextBox;
  double  xmin_TextBox;
  double  xmax_TextBox;

  int     button_space;
  int     button_counter;
  
  double  window_width;
  double  window_height;
  string   window_label;

  int     x_WindowPos;
  int     y_WindowPos;
  
  bool    ShowScrollBar;
  bool    ShowTexture;
  bool    setScrollPosition;
  
  color   bg_color;
  
  static void show_buffer(window*);
  static void draw_buffer(window*);
  static void scroll_drag(int);
  static void scroll_down(int);
  static void scroll_up(int);

  double ypos;
  double ylow;
  double yhigh;
  double txt_height;
  
  list<string> buffer;
  void init_parameters();
    
  public:

  TextLoader(int x = 200, int y = 200, string label = "") : window_width(x), window_height(y), window_label(label)
  { wp = new window(x, y, label); init_parameters();  }

  ~TextLoader() { delete wp; } 

  void load_text(string); 

  void load_text_with_display(string fname)
  { load_text(fname); 
    display( w_parent, x_WindowPos, y_WindowPos ); 
   }

  void load_text_with_display(string fname, int x, int y)
  { load_text(fname); display( w_parent, x, y ); }

  void load_text_with_display(string fname, int x, int y, window *w)
  { load_text(fname); display( w, x, y); }

  void extract_frame_label(string);
  void create_menu(string);
  void update_all_button_labels(string);
  void open_with_timeout(int, int, int);
  int  open_with_read_button(int, int);
  int  open_with_read_button();
  
  void display( window *, int, int );         
  void display(int, int);         
  void display();         
  void clear() { wp->clear(); } 
  void close() { wp->close(); }         

  color set_bg_color( color bg ) 
  { return wp->set_bg_color( bg_color=bg ); }

  void set_window_label(string label) 
  { wp->set_frame_label(window_label = label); }
  

  void set_read_intervall(int at, int to)
  { if(at < 1 || at > to || to < 0) return;
    at_line = at; to_line = to;
  }

  void set_textbox_position(double x1, double x2, double y1) 
  { xmin_TextBox = x1; xmax_TextBox = x2;  ymin_TextBox = y1;  }

  void set_string_style(string style, int line, string stg) 
  { marked_style = style;  marked_line = line; search_string = stg;  }


  void set_scroll_position( int i, bool status )
  { setScrollPosition=status; 
    
		if( !setScrollPosition ) return;
	
	  ypos =  ylow + double(((double) i/1000) * (yhigh - ylow)); 
	
    wp->set_scrollbar_pos( ypos - ylow );
  }

  void set_tabulator_stops(int number)
  { if(number < 1) return;
    tab_string = "";
    for(int i=1; i<number; i++)
      tab_string += "~";
  }

  void set_button_space(int space)
  { if(space < 0) return; 
    button_space = space;
  }

  void set_button_label(int button, string label)
  { if(button < 0 || button >= button_counter) return;
    wp->set_button_label(button, label);
  }

  void set_display_position(int x, int y)
  { x_WindowPos = x; y_WindowPos = y;  }

  
  void set_scrollbar(bool show)
  { ShowScrollBar = show;  }


  void enable_all_buttons()
  { if(button_counter < 1) return;
    for(int i = 0; i < button_counter; i++)
      wp->enable_button(i);
  }

  void disable_all_buttons()
  { if(button_counter < 1) return;
    for(int i = 0; i < button_counter; i++)
      wp->disable_button(i);
  }

  void enable_button(int button)
  { if(button < 0 || button >= button_counter) return;
    wp->enable_button(button);
  }
    

  void disable_button(int button)
  { if(button < 0 || button >= button_counter) return;
    wp->disable_button(button);
  }

  void set_texture(const bool status) { ShowTexture = status; }

  window* get_parent_window_ptr() { return w_parent; }
  window* get_window_ptr() { return wp; }
  int get_file_lenght(string);
  int get_number_of_buttons() { return button_counter; }
  int get_window_height() { return wp->height(); }    
  int get_window_width() { return wp->width(); }

  string get_button_label(int button)
  { if(button < 0 || button >= button_counter) return "";
    return wp->get_button_label(button);
  }

  int wait_of_mouse_event() { return wp->read_mouse(); }
  void wait_with_timeout(int);   
  int read_button() { return wp->read(); }
};

LEDA_END_NAMESPACE

#endif
