/*******************************************************************************
+
+  LEDA 7.2.2
+
+
+  _base_window.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



// defines the BASE_WINDOW operations declared in <LEDA/graphics/base_window.h>
// using the basic graphics routines from <LEDA/graphics/x_basic.h>

#include <LEDA/graphics/base_window.h>

#include <LEDA/system/file.h>

#if defined(__win32__)

#include <LEDA/graphics/pixmaps/win_icon2.xpm>
#define XPM_ICON win_icon2_xpm

#else

#include <LEDA/graphics/pixmaps/leda_icon.xpm>
#include <LEDA/graphics/pixmaps/leda_small_icon.xpm>
#define XPM_ICON leda_icon

#endif

#include <LEDA/core/string.h>
#include <LEDA/system/assert.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if defined(__unix__)
#include <signal.h>
#include <sys/time.h>
#endif

LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>

/*
#include <LEDA/graphics/x_base.h>
x_base* XP = new x_base_x11();
*/


const char* event_name[] = { 
 "key_press_event", 
 "key_release_event", 
 "button_press_event", 
 "button_release_event",
 "display_event",
 "configure_event", 
 "exposure_event", 
 "motion_event", 
 "destroy_event", 
 "timer_event",
 "device_connect_event",
 "volume_connect_event",
 "no_event" 
};


inline double HyPot(double x, double y) { return sqrt(x*x + y*y); }

inline bool IsSpace(char c) { return c == ' ' || c == '\n'; }


int  BASE_WINDOW::real_to_pix(double d) const
{ 
  return (d > 0) ? int(d*scaling + 0.5) : int(d*scaling - 0.5);
}

double BASE_WINDOW::pix_to_real(int p) const { return p*one_over_scaling; }


int BASE_WINDOW::xpix(double x) const
{ double xf = xorigin + xoffset + x*scaling; 
  return (xf > 0) ? int(xf + 0.5) : int(xf - 0.5);
}

int BASE_WINDOW::ypix(double y) const
{ double yf = yorigin + yoffset - y*scaling;
  return (yf > 0) ? int(yf + 0.5) : int(yf - 0.5);
}


double BASE_WINDOW::xreal(int pix) const    
{ pix -= xoffset;
  return double(pix-xorigin)*one_over_scaling; 
}

double BASE_WINDOW::yreal(int pix) const
{ pix -= yoffset;
  return double(yorigin-pix)*one_over_scaling; 
}



void BASE_WINDOW::REDRAW_FUNC(void* p, int x, int y, int wi, int he, int val)
{ 
  BASE_WINDOW* w = (BASE_WINDOW*)p;

  int event = w->current_event;

/*
  printf("REDRAW_FUNC w = %d  event = %s  x = %d  y = %d  width = %d (%d)  height = %d (%d) buf = %d\n", 
          w->draw_win, event_name[w->current_event], x, y,
          x_window_width(w->draw_win), w->window_width, 
          x_window_height(w->draw_win), w->window_height, w->is_buffering()); 
  printf("wi = %d  he = %d\n",wi,he);
  fflush(stdout);
*/


/*
  if (event == exposure_event &&
      x == 0 && y == 0 && wi == w->window_width && he == w->window_height)
  { // entire window exposed: NO REDRAW -- WHY ?
    w->reset_clipping();
    return;
  }
*/


  BASE_WINDOW::call_window = w;

  // call configure() if window size has changed

  if (w->window_width  != x_window_width(w->draw_win)  ||  
      w->window_height != x_window_height(w->draw_win))
   { w->configure();  
     w->redraw_panel();
     x_flush_display();
    }

  if (y <= w->panel_height) 
  { w->redraw_panel(x,y,x+wi+1,y+he+1);
    x_flush_display();
    y = w->panel_height+1;
   }

/*
  x_set_clip_rectangle(w->draw_win,x-1,y-1,wi+2,he+2);
*/
  w->reset_clipping();

  double x0 = w->xreal(x-1); 
  double y0 = w->yreal(y+he+1); 
  double x1 = w->xreal(x+wi+1); 
  double y1 = w->yreal(y-1); 

  if (w->redraw0 || w->redraw1 || w->redraw2 || w->redraw2_ptr ||w->redraw1_ptr) 
  { 
    drawing_mode save_mode = x_set_mode(w->draw_win,src_mode);

    if (w->grid_mode != 0) w->draw_grid(x0,y0,x1,y1);

    if (w->redraw2 || w->redraw2_ptr) {
      if (w->redraw2_ptr) {
         w->redraw2_ptr->set_params(w,NULL,0,x0,y0,x1,y1,val);
         w->redraw2_ptr->operator()();
      }
      else w->redraw2(w,x0,y0,x1,y1,val);
    }
    else 
      if (w->redraw1 || w->redraw1_ptr) {
        if (w->redraw1_ptr) {
	   w->redraw1_ptr->set_params(w, NULL);
	   w->redraw1_ptr->operator()();
	}
        else w->redraw1(w);
      }
      else 
        w->redraw0();
     x_set_mode(w->draw_win,save_mode);
    }
/*
   else 
   { //clear entire window if no redraw function defined
     //printf("clear(%f,%f,%f,%f)\n",x0,y0,x1,y1);
     //fflush(stdout);
     //w->clear(x0,y0,x1,y1);
     w->clear();
   }
*/

  if (w->mesg_count > 0) w->draw_messages();

  w->clipping(2);
}


void BASE_WINDOW::redraw()
{
  { // read all remaining events  (s.n. 10/2002)  and 
    // call configure() if window size has changed

    int ww,v1,v2,x,y;
    unsigned long t;
    while (x_get_next_event(ww, x, y, v1,v2,t,-1) != no_event);
    if (window_width  != x_window_width(draw_win)  ||  
        window_height != x_window_height(draw_win)) configure();  
   }

   // redraw panel area
   redraw_panel();

   // new - background redraw added
   if (clear_redraw_ptr) {
     clear_redraw_ptr->set_params(this,NULL,0,xmin(),ymin(),xmax(),ymax(),0);
     clear_redraw_ptr->operator()();
   }
   else if (clear_redraw) clear_redraw(this,xmin(),ymin(),xmax(),ymax(),0);

   if (redraw2 || redraw2_ptr) {
     if (redraw2_ptr) {
        redraw2_ptr->set_params(this,NULL,0,xmin(),ymin(),xmax(),ymax(),0);
        redraw2_ptr->operator()();
     }
     else redraw2(this,xmin(),ymin(),xmax(),ymax(),0);
   }
   else 
    if (redraw1 || redraw1_ptr) { 
        if (redraw1_ptr) {
	   redraw1_ptr->set_params(this,NULL);
	   redraw1_ptr->operator()();
	 }
         else redraw1(this);
    }	 
    else 
     if (redraw0) redraw0(); // new - check added
 }


char* BASE_WINDOW::access_str(void* p)
{ //fprintf(stderr, "Use of BASE_WINDOW::access_str() is illegal.");
  return (char*)p;
 }

void BASE_WINDOW::assign_str(void* p, const char* str)
{ //fprintf(stderr, "Use of BASE_WINDOW::assign_str() is illegal.");
  strcpy((char*)p,str);
}


void BASE_WINDOW::set_border_width(int w) { x_set_border_width(draw_win,w); }
void BASE_WINDOW::set_border_color(int c) { x_set_border_color(draw_win,c); }

char* BASE_WINDOW::set_bg_pixrect(char* pr, double xorig, double yorig)
{ char* save = bg_pixrect; 
  bg_pixrect = pr; 
  x_set_bg_pixmap(draw_win,pr);
  bg_xoff = xorig;
  bg_yoff = yorig;
  return save;
}

char* BASE_WINDOW::set_bg_pixrect(char* pr)
{ char* save = bg_pixrect;
  bg_pixrect = pr; 
  x_set_bg_pixmap(draw_win,pr);
  return save;
}


int BASE_WINDOW::set_bg_color(int c) 
{ int save = bg_color; 
  bg_color = c; 
  x_set_bg_color(draw_win,c);
  return save;
 }


int BASE_WINDOW::set_fg_color(int c)
{ int save = fg_color; 
  fg_color = c; 
  return save;
 }

int BASE_WINDOW::set_fill_color(int c)
{ int save = fill_color; 
  fill_color = c; 
  return save;
 }


void BASE_WINDOW::set_buttons(int b0, int b1, int b2, int b3, int b4, int b5)
{ button_table[0] = b0;
  button_table[1] = b1;
  button_table[2] = b2;
  button_table[3] = b3;
  button_table[4] = b4;
  button_table[5] = b5;
}



void BASE_WINDOW::std_buttons()
{
  set_buttons(NO_BUTTON, MOUSE_BUTTON(1), MOUSE_BUTTON(2), MOUSE_BUTTON(3), 
                                          MOUSE_BUTTON(4), MOUSE_BUTTON(5));
}

void BASE_WINDOW::flush() { x_flush_display(); }


void BASE_WINDOW::clipping(int mode)
{ 
   switch (mode) {

   case 0: // entire window
   //x_set_clip_rectangle(draw_win,xoffset,yoffset,window_width,window_height);
     x_set_clip_rectangle(draw_win,xoffset,yoffset,window_width+10,window_height+10);
     break;

   case 1: // panel area
     //x_set_clip_rectangle(draw_win,xoffset,yoffset,panel_width,panel_height);
     x_set_clip_rectangle(draw_win,xoffset,yoffset,panel_width,panel_height+1);
     break;

   case 2: // drawing area
     x_set_clip_rectangle(draw_win,xoffset,yoffset+panel_height,window_width,
                                                  window_height-panel_height);
     break;
   }
}



void BASE_WINDOW::set_color(int c) 
{ 
  if (!is_open() && buf_level == 0) 
  { fprintf(stderr,"Error: Window has to be displayed before drawing.\n");
    x_close_display();
    abort();
   }
  if (c == DEF_COLOR) c = fg_color;
  x_set_color(draw_win,c); 
 }


void BASE_WINDOW::set_stipple(char* bits, int c) 
{ x_set_stipple(draw_win,bits,c); }


void* BASE_WINDOW::get_handle() const
{ return x_window_handle(draw_win); }

int BASE_WINDOW::is_open() const
{ return x_window_opened(draw_win); }

int BASE_WINDOW::is_closed() const 
{ return x_window_opened(draw_win)==0; }

int BASE_WINDOW::is_minimized() const 
{ return x_window_minimized(draw_win); }


int BASE_WINDOW::set_cursor(int c) { 
  return x_set_cursor(draw_win,c); 
}

void BASE_WINDOW::set_frame_label(const char* label) 
{ 
/*
  strncpy(default_frame_label,label,128);
  default_frame_label[127] = '\0';
  x_set_label(draw_win,default_frame_label); 
  x_set_icon_label(draw_win,default_frame_label); 
*/
  x_set_label(draw_win,label); 
  x_set_icon_label(draw_win,label); 
}

void BASE_WINDOW::set_tmp_label(const char* label) 
{ x_set_label(draw_win,label); }


void BASE_WINDOW::set_icon_label(const char* label) 
{ x_set_icon_label(draw_win,label); }

/*
int BASE_WINDOW::load_text_font(const char* fname) 
{ return x_load_text_font(draw_win,fname); }

int BASE_WINDOW::load_italic_font(const char* fname) 
{ return x_load_italic_font(draw_win,fname); }

int BASE_WINDOW::load_bold_font(const char* fname) 
{ return x_load_bold_font(draw_win,fname); }

int BASE_WINDOW::load_fixed_font(const char* fname) 
{ return x_load_fixed_font(draw_win,fname); }

int BASE_WINDOW::load_button_font(const char* fname) 
{ return x_load_button_font(draw_win,fname); }
*/


void BASE_WINDOW::set_text_font()  { x_set_text_font(draw_win); }
void BASE_WINDOW::set_italic_font(){ x_set_italic_font(draw_win); }
void BASE_WINDOW::set_bold_font()  { x_set_bold_font(draw_win); }
void BASE_WINDOW::set_fixed_font() { x_set_fixed_font(draw_win); }
void BASE_WINDOW::set_button_font(){ x_set_button_font(draw_win); }

int BASE_WINDOW::set_font(const char* fn) { return x_set_font(draw_win,fn); }


void BASE_WINDOW::reset_frame_label() 
{ set_frame_label(default_frame_label); }

double BASE_WINDOW::set_grid_dist(double d) 
{ double d0 = grid_mode;
  grid_mode = d;
  return d0;
 }

grid_style BASE_WINDOW::set_grid_style(grid_style s) 
{ grid_style s0 = g_style;
  g_style = s;
  return s0;
 }

int BASE_WINDOW::set_grid_mode(int i) 
{ int save = (int)grid_mode;
  if (grid_mode != i) init(xmin(),xmax(),ymin(),i); 
  return save;
 }

int BASE_WINDOW::set_node_width(int w)
{ if (w < 1) w = 1;
  int save = node_width;
  node_width = w;
  return save;
 }


drawing_mode BASE_WINDOW::set_mode(drawing_mode m) 
{ return x_set_mode(draw_win,m); }

int BASE_WINDOW::set_line_width(int w)
{ if (w < 1) w = 1;
  return x_set_line_width(draw_win,w);
 }

point_style BASE_WINDOW::set_point_style(point_style ps)
{ point_style ps_old = pt_style;
  pt_style = ps;
  return ps_old;
}

line_style BASE_WINDOW::set_line_style(line_style ls)
{ return x_set_line_style(draw_win,ls); }


int BASE_WINDOW::set_join_style(int js)
{ return x_set_join_style(draw_win,js); }

text_mode BASE_WINDOW::set_text_mode(text_mode m)
{ return x_set_text_mode(draw_win,m); }



coord_handler_func BASE_WINDOW::set_coord_handler(coord_handler_func f) 
{ coord_handler_func old = coord_handler;
  coord_handler= f;  
  return old;
}

const window_handler* BASE_WINDOW::set_coord_object(const window_handler& obj)
{
  const window_handler* ptr = coord_handler_ptr;
  coord_handler_ptr = & (window_handler&) obj;
  return ptr;
}


win_close_handler_func BASE_WINDOW::set_window_close_handler(win_close_handler_func f) 
{ win_close_handler_func old = win_close_handler;
  win_close_handler= f;  
  return old;
}

const window_handler*   BASE_WINDOW::set_window_close_object(const window_handler& obj)
{
  const window_handler* prev = win_close_ptr;
  win_close_ptr = & (window_handler&) obj;
  return prev;
}

event_handler_func BASE_WINDOW::set_event_handler(event_handler_func f) 
{ event_handler_func old = user_event_handler;
  user_event_handler= f;  
  return old;
}

void BASE_WINDOW::set_redraw(win_redraw_func2 f) 
{ redraw2 = f;  
  redraw0 = 0;
  redraw1 = 0;
}

void BASE_WINDOW::set_redraw2(const window_handler& obj)
{
  redraw2_ptr = & (window_handler&) obj;
  redraw0 = 0;
  redraw1 = 0;  
}

void BASE_WINDOW::set_redraw(win_redraw_func1 f) 
{ redraw1 = f; 
  redraw0 = 0;
  redraw2 = 0;
  redraw2_ptr = 0;
}

void BASE_WINDOW::set_redraw(const window_handler& obj) 
{ 
  redraw1_ptr = & (window_handler&) obj;
  redraw0 = 0;
  redraw2 = 0;
  redraw2_ptr = 0;  
}

void BASE_WINDOW::set_redraw(win_redraw_func0 f) 
{ redraw0 = f;
  redraw1 = 0;
  redraw2 = 0;
  redraw2_ptr = 0;
}


int BASE_WINDOW::get_line_width() const
{ return x_get_line_width(draw_win); }

line_style BASE_WINDOW::get_line_style() const
{ return x_get_line_style(draw_win); }

text_mode BASE_WINDOW::get_text_mode() const
{ return x_get_text_mode(draw_win);  }

drawing_mode BASE_WINDOW::get_mode() const 
{ return x_get_mode(draw_win);       }

int BASE_WINDOW::get_cursor() const
{ return x_get_cursor(draw_win); }

int BASE_WINDOW::get_resizing() const
{ return x_get_resizing(draw_win); }


double  BASE_WINDOW::text_width(const char* s) const
{ 
  //return pix_to_real(x_text_width(draw_win,s));

  // multi-line text
  int tw = 0;
  const char* p = s;
  for(;;)
  { const char* q = p;
    while (*q && *q != '\n') q++;
    int w = x_text_width(draw_win,p,int(q-p));
    if (w > tw) tw = w;
    if (*q == '\0') break;
    p = q+1;
   }
  return pix_to_real(tw);
}

 
double  BASE_WINDOW::text_height(const char* s) const
{ int line_count = 1;
  const char* p = s;
  while (*p) {
    if (*p++ == '\n') line_count++;
  }
  return pix_to_real(line_count*x_text_height(draw_win,s));
}


int BASE_WINDOW::get_border_width() const
{ return x_get_border_width(draw_win); }

/*
int BASE_WINDOW::get_border_color() const
{ return x_get_border_color(draw_win); }
*/



// frame window (window manager)

void BASE_WINDOW::frame_box(int& x0, int& y0, int& x1, int& y1) const
{ x_window_frame(draw_win,x0,y0,x1,y1); }

int BASE_WINDOW::xpos() const
{ int x0,y0,x1,y1;
  x_window_frame(draw_win,x0,y0,x1,y1); 
  return x0;
}

int BASE_WINDOW::ypos() const
{ int x0,y0,x1,y1;
  x_window_frame(draw_win,x0,y0,x1,y1); 
  return y0;
}

int BASE_WINDOW::frame_width() const
{ int x0,y0,x1,y1;
  x_window_frame(draw_win,x0,y0,x1,y1); 
  return x1-x0+1;
}

int BASE_WINDOW::frame_height() const
{ int x0,y0,x1,y1;
  x_window_frame(draw_win,x0,y0,x1,y1); 
  return y1-y0+1;
}




int BASE_WINDOW::read_event(int& k, double& x, double& y)
{ 
  mouse_key = NO_BUTTON;

  BASE_WINDOW* wp = 0;
  int e = no_event;
  while (wp != this || e == no_event)  e = event_handler(wp,0); 

  x = mouse_xreal;
  y = mouse_yreal;
  k = mouse_key;
  return e;
 }

int BASE_WINDOW::read_event(int& k, double& x, double& y, unsigned long& t)
{ 
  mouse_key = NO_BUTTON;

  BASE_WINDOW* wp = 0;
  int e = no_event;
  while (wp != this || e == no_event) e = event_handler(wp,0);

  x = mouse_xreal;
  y = mouse_yreal;
  k = mouse_key;
  t = event_time;

  return e;
 }


int BASE_WINDOW::read_event(int& k, double& x, double& y, unsigned long& t,
                                                          int timeout)
{ 
  mouse_key = NO_BUTTON;

  BASE_WINDOW* wp=0;
  int e = event_handler(wp,timeout);

  if (wp == this) 
  { x = mouse_xreal;
    y = mouse_yreal;
    k = mouse_key;
    t = event_time;
    return e;
   }

  return no_event;
 }


int BASE_WINDOW::get_event(int& k, double& x, double& y)
{ // non blocking
  BASE_WINDOW* wp=0;
  int e = -1;
  while (e != no_event && wp != this) e = event_handler(wp,-1);
  x = mouse_xreal;
  y = mouse_yreal;
  k = mouse_key;
  return e;
 }




int BASE_WINDOW::event_handler(BASE_WINDOW*& wp, int timeout)
{
  // timeout (msec)
  //  0: blocking
  // -1: non-blocking

  wp = 0;

  int win = 0;
  int e = no_event;
  int x=0,y=0,val1=0,val2=0;
  unsigned long t=0;

  if (timeout >= 0) win = 1;

  if (active_button_win) 
  { if (active_button != last_active_button)
    { active_button_win->redraw_panel(); 
      last_active_button = active_button;
     }
    if (active_button == 0) active_button_win = 0;
  }

  e = x_get_next_event(win,x,y,val1,val2,t,timeout);

/*
//if (e == button_press_event || e == button_release_event) 
if (e != no_event)
{ printf("event: w = %d e = %s x = %d y = %d v1 = %d v2 = %d t = %lu \n",
          win,event_name[e],x,y,val1,val2,t);
  fflush(stdout);
}
*/


  if (e == display_event && win == 0)
  { 
    if (val1 == 0)
      cout << "DISPLAY CLOSED" << endl;
    else 
      cout << string ("DISPLAY SIZE CHANGED: %d x %d (%d)",x,y,val1) <<endl;

   return e;
  }
 
  

  if (e == device_connect_event || e == volume_connect_event) 
  { wp = (BASE_WINDOW*)x_window_inf(win);
    if (wp->user_event_handler) {
      //cout << "user_event_handler: " << event_name[e] << endl;
      e = wp->user_event_handler(wp,e,val1,x,y,t);
    }
    return e;
   }


  if (e == destroy_event && win == 0) 
  { // global destroy event (only in websocket interface xx)
    if (display_type() == "xx") 
    { cout << "GLOBAL DESTROY: CLOSE & EXIT" << endl;
      //x_close_display();
      exit(0);
    }
  }

  if (e == no_event || win == 0) return no_event;


  // event happened in window win

  wp = (BASE_WINDOW*)x_window_inf(win);

   if (wp == 0) {
     // this happens only for exposure events of tooltip windows
/*
     cout << "EVENT " << event_name[e] << " win = " << win << " wp = 0" << endl;
*/
     return no_event;
   }


  wp->current_event = e;

  if (e == timer_event) 
  { if (wp->timer_action || wp->timer_action_ptr) 
    { drawing_mode save = x_set_mode(wp->draw_win,src_mode);
    
      if (wp->timer_action_ptr) {
        wp->timer_action_ptr->set_params(wp,NULL);
        wp->timer_action_ptr->operator()();
      }
      else wp->timer_action(wp);
      
      x_set_mode(wp->draw_win,save);
     }
    return e;
   }

  int alt_down = (val1 >> 8) == 4;


  if (alt_down && (e == key_press_event || e == key_release_event))
  { int key_char = (val1 & 0xFF);
    if (e == key_press_event)
    { // wait for key release event and put it back
      int w,v1,v2,x,y;
      unsigned long t;
      while (x_get_next_event(w,x,y,v1,v2,t) != key_release_event);
      x_put_back_event();
     }
    for(int i = 0; i < wp->item_count; i++)
    { panel_item it = wp->Item[i];
      if (it->kind != Button_Item || it->shortcut < 0) continue;
      char c = (char)tolower(it->label_str[it->shortcut]);
      if ( c == key_char)
      { x = it->xcoord + wp->button_w/2;
        y = it->ycoord + wp->button_h/2;
        if (e == key_press_event)
          e = button_press_event;
        else
           e = button_release_event;
        break;
       }
     }
   }
    

  if (e != exposure_event && wp->panel_enabled == 0 
                          && y > 0 && y < wp->panel_height
                          && x > 0 && x < wp->window_width)  {
      return (timeout > 0) ? e : no_event;
  }


  BASE_WINDOW* swp = wp->scroll_bar;
  if (swp && x > wp->window_width - swp->window_width)
  { // compute coordinates relativ to window sw
    x_window_to_screen(wp->draw_win,x,y);
    x_screen_to_window(swp->draw_win,x,y);
    wp = swp;
   }


  if (e != configure_event && e != exposure_event &&  e != destroy_event &&
      (e != key_press_event || val1 != KEY_PRINT))
  { 
    int panel_closed = 0;

    BASE_WINDOW* ap = BASE_WINDOW::active_window;

    while (ap && ap->p_win)
    { 
      if (e == motion_event && ap->p_win->menu_mode == 0) break;

      if (x > 0 && x < wp->window_width && y > 0 && y < wp->window_height) break;
      if (e != button_press_event) break;

      panel_closed++;
      x_window_to_screen(ap->draw_win,x,y);
      ap = ap->p_win;
      x_screen_to_window(ap->draw_win,x,y);
      if (ap->p_root != ap) x_grab_pointer(ap->draw_win);
      BASE_WINDOW::active_window = ap;
      wp = ap;
    }

    int v = wp->panel_event_handler(wp->draw_win,e,val1,x,y,t); 

    if (v >= 0)
    { 
      wp = wp->p_root;

      //assert(wp != 0);
      if (wp == 0) return no_event; // can this happen ?

      wp->mouse_key = v;
      wp->mouse_press_time = t;
      return button_press_event;
    }

    if (e != motion_event && e != button_release_event) // changed: 11/2010
     if (panel_closed || ((y > 0 && y < wp->panel_height) && 
                          (x > 0 && x < wp->window_width))) 
       return (timeout > 0) ? e : no_event;

  }


  // win may have been changed in panel event handler ?  YES !!
  //assert(wp == (BASE_WINDOW*)x_window_inf(win));

  wp = (BASE_WINDOW*)x_window_inf(win);



  switch (e) {

  case exposure_event:
      { 
        int wi = val1;
        int he = val2;
/*
printf("EXPOSURE win = %ld x = %d y = %d wp = %d h = %d \n",(unsigned long)wp,x,y,wi,he);
fflush(stdout);
*/
        if (display_type() != "mswin") REDRAW_FUNC(wp,x,y,wi,he,0);
        break; 
       }


 case configure_event: // window position or size has changed
/*
     wp->window_xpos = x;
     wp->window_ypos = y;
*/

/*
     printf("CONFIGURE win = %ld  x = %d  y = %d  wp = %d  h= %d \n",(unsigned long)wp,x,y,val1,val2);
*/


#if !defined(__win32__)
    // call redraw function if window changed 
    // maybe not necessary if size is enlarged --> EXPOSURE EVENT ?
    if (wp->window_width != x_window_width(wp->draw_win) ||
    wp->window_height != x_window_height(wp->draw_win))
    REDRAW_FUNC(wp,x,y,0,0,0);
#endif

           break; 

  case key_press_event:
  case key_release_event:

           if (e == key_press_event && val1 == KEY_PRINT) 
           { char fname[256];
             bool full_color = true;
#if defined(__win32__)
             strcpy(fname,"screenshot.wmf");
#else
             strcpy(fname,"screenshot.ps");
#endif

             BASE_WINDOW P(-1,-1,"Screenshot Panel");
             P.text_item("\\bf\\blue Save Window Screenshot");
             P.string_item("to file",fname,0,"");
/*
             P.bool_item("full color",&full_color,panel_action_func(0),"");
*/
             int b = P.button("write",0,panel_action_func(0));
             P.set_focus_button(b);
             P.button("cancel",1,panel_action_func(0));

             char* pr = wp->get_window_pixrect();

             int but = P.panel_open(BASE_WINDOW::center,BASE_WINDOW::center,wp);

             wp->set_pixrect(pr);
             wp->del_pixrect(pr);
             wp->redraw_panel();

             if (but == 0) {
               wp->screenshot(fname,full_color);
               //cout << "fname = " << fname << endl;
               open_file(fname);
             }
           }

           wp->shift_key_state = val1 >> 8;
           wp->mouse_key = val1 & 0xFF;
           wp->mouse_xpix = x;
           wp->mouse_ypix = wp->window_height-y-1;
           break;


  case destroy_event: {

           bool log = (display_type() == "xx");

           if (log)
             cout << string("DESTROY EVENT: win = %d",wp->draw_win) << endl;

           if (wp->win_close_handler || wp->win_close_ptr) 
           { if (log) cout << "CLOSE HANDLER" << endl;

             BASE_WINDOW::call_window = wp; 
	     if (wp->win_close_ptr) {
    	       wp->win_close_ptr->set_params(wp, NULL);
  	       wp->win_close_ptr->operator()();
             }
             else  wp->win_close_handler(wp);
             break;
            }

           if (wp->close_button_enabled)
           { // close window and return CLOSE_BUTTON 
             if (log) cout << "CLOSE_BUTTON" << endl;

             e = button_press_event;
             wp->mouse_key = CLOSE_BUTTON;
             wp->shift_key_state = 0;
             wp->mouse_xpix = 0;
             wp->mouse_ypix = 0;
             wp->mouse_press_time = t;
             break;
           }

           if (wp->focus_button)
           { // simulate button release of focus button
             if (log) cout << "FOCUS BUTTON" << endl;

             panel_item it = wp->focus_button;
             wp->last_sel_button = it;
             wp->draw_button(it,1);

             int ev= button_release_event;
             int x = it->xcoord + 10;
             int y = it->ycoord + 10;
             int v = wp->panel_event_handler(wp->draw_win,ev,1,x,y,t); 

             if (v >= 0)
             { wp->mouse_key = v;
               wp->mouse_press_time = t;
               e = button_press_event;
              }
             else
               e = no_event;

             break;
           }

           // in all other cases: close and exit

           if (log) cout << "DEFAULT: CLOSE & EXIT" << endl;

           wp->close();
           x_close_display();
           exit(0);

           break;
         }


  case button_press_event:
           wp->mouse_key = wp->button_table[val1 & 0xFF];
           wp->shift_key_state = val1 >> 8;
           wp->mouse_xpix = x;
           wp->mouse_ypix = wp->window_height-y-1;
           wp->mouse_press_time = t;
           break;

  case button_release_event:
           wp->mouse_key = wp->button_table[val1 & 0xFF];
           wp->shift_key_state = val1 >> 8;
           wp->mouse_xpix = x;
           wp->mouse_ypix = wp->window_height-y-1;
           wp->mouse_release_time = t;
           break;

  case motion_event:
         wp->mouse_xpix = x;
         wp->mouse_ypix = wp->window_height-y-1;
         break;
   }


  if (e==motion_event || e==button_press_event || e==button_release_event)
  { if (wp->grid_mode > 0) wp->cursor();

    wp->mouse_xreal =  wp->min_xcoord + 
                      ((double)wp->mouse_xpix)*wp->one_over_scaling;

    wp->mouse_yreal =  wp->min_ycoord + 
                      ((double)wp->mouse_ypix)*wp->one_over_scaling;
          
    if (timeout >= 0) // blocking
    { 
      if (wp->grid_mode > 0)
      { int g = (int)wp->grid_mode;
        wp->mouse_xreal=g*(int)(wp->mouse_xreal/g+((wp->mouse_xreal>0)?0.5:-0.5));
        wp->mouse_yreal=g*(int)(wp->mouse_yreal/g+((wp->mouse_yreal>0)?0.5:-0.5));
        wp->cursor();
      }
       
      // coordinate handling ...
      if (wp->coord_handler_ptr){ 
       wp->coord_handler_ptr->set_params(wp,NULL,0,wp->mouse_xreal, wp->mouse_yreal);
       wp->coord_handler_ptr->operator()();
      }
      else { 
       if (wp->coord_handler) 
        (wp->coord_handler)(wp,wp->mouse_xreal, wp->mouse_yreal);
      }
            
      if (wp == read_window && wp->mouse_action) // user defined action 
      { wp->mouse_action(wp->mouse_last_xreal,wp->mouse_last_yreal);
        wp->mouse_action(wp->mouse_xreal,wp->mouse_yreal);
      }
    }
    
    wp->mouse_last_xreal = wp->mouse_xreal;
    wp->mouse_last_yreal = wp->mouse_yreal;
  }


  wp->event_time = t;

  if (wp->user_event_handler) {
   e = wp->user_event_handler(wp,e,wp->mouse_key,wp->mouse_xreal,wp->mouse_yreal,t);
  }


  return e;
}



int BASE_WINDOW::read_mouse(int kind, double xstart, double ystart, double& x, 
                                                                    double& y)
{ int result = 0;

  int buf = x_test_buffer(draw_win);
  if (buf && kind != 0) x_stop_buffering(draw_win);

/*
  drawing_mode dm = x_set_mode(draw_win,xor_mode);
*/

  drawing_mode dm = x_get_mode(draw_win);

  if (kind != 0) x_set_mode(draw_win,xor_mode);

  switch(kind) {

  case  0: // point
           result = read_mouse_action(mouse_default_action,xstart,ystart,x,y);
           break;

  case  1: // segment
           result = read_mouse_action(mouse_segment_action,xstart,ystart,x,y);
           break;

  case  2: // ray
           result = read_mouse_action(mouse_ray_action,xstart,ystart,x,y);
           break;

  case  3: // line
           result = read_mouse_action(mouse_line_action,xstart,ystart,x,y);
           break;

  case  4: // circle
           result = read_mouse_action(mouse_circle_action,xstart,ystart,x,y);
           break;

  case  5: // rectangle
           result = read_mouse_action(mouse_rect_action,xstart,ystart,x,y);
           break;

  case  6: // arc
           result = read_mouse_action(mouse_arc_action,xstart,ystart,x,y);
           break;

  default: result = read_mouse_action(mouse_default_action,xstart,ystart,x,y);
           break;
  }

  // window may have been deleted in action function ?

  x_set_mode(draw_win,dm);

  if (buf && kind != 0) x_start_buffering(draw_win);

  return result;
}

int BASE_WINDOW::read_mouse(int kind, double x0,double y0, double x1,double y1,
                                                           double& x, double& y)
{
  mouse_additional_xreal = x1;
  mouse_additional_yreal = y1;
  return read_mouse(kind, x0, y0, x, y);
}


int BASE_WINDOW::get_mouse(double& x, double& y)
{ 
  if (!is_open() && buf_level == 0) 
  { //fprintf(stderr,"Error: Window has to be displayed before mouse input.\n");
    return NO_BUTTON;
   }

  int but = NO_BUTTON;

  BASE_WINDOW* wp = 0;
  int e = -1;
  while ((wp != this || e != button_press_event) && e != no_event) {
    e = event_handler(wp,-1);
  }

  if (e != no_event) but = mouse_key;

  x = mouse_xreal;
  y = mouse_yreal;

  return but;
 }



int BASE_WINDOW::get_mouse(double&x, double& y, unsigned long timeout)
{ unsigned long t_stop = epoche_time() + timeout;
  unsigned long t = 0;
  int val = NO_BUTTON;

  while (t < t_stop)
  { unsigned long tt;
    int e = read_event(val,x,y,tt,25);
    if (e == button_press_event) break;
    t = epoche_time();
  }

  return (t < t_stop) ? val : NO_BUTTON;
}


unsigned long BASE_WINDOW::button_press_time()  { return mouse_press_time; }
unsigned long BASE_WINDOW::button_release_time(){ return mouse_release_time; }

int BASE_WINDOW::shift_key_down() { return shift_key_state & 1; }
int BASE_WINDOW::ctrl_key_down()  { return shift_key_state & 2; }
int BASE_WINDOW::alt_key_down()   { return shift_key_state & 4; }




#if defined(__unix__)

static BASE_WINDOW* timer_win;
 
#if defined(__DECCXX)
#define leda_sig_pf __sigfp
#elif defined(SIG_PF) || defined(sgi) || (defined(__SUNPRO_CC) && !defined(linux))
#define leda_sig_pf SIG_PF
#else
typedef void (*leda_sig_pf)(int);
#endif

void BASE_WINDOW::timer_handler(int)
{ drawing_mode save_mode = timer_win->set_mode(src_mode);

  if (timer_win->timer_action_ptr) {
    timer_win->timer_action_ptr->set_params(timer_win,NULL);
    timer_win->timer_action_ptr->operator()();
  }
  else timer_win->timer_action(timer_win);
  
  timer_win->set_mode(save_mode);

/*
  signal(SIGALRM,(leda_sig_pf)timer_handler);
*/
 }
#endif


void BASE_WINDOW::start_timer(int msec, win_redraw_func1 F)
{ timer_action = F;
  x_start_timer(draw_win,msec); 
#if defined(__unix__)
  timer_win = this;
  signal(SIGALRM,(leda_sig_pf)timer_handler);

  static itimerval it;
  float fsec  = msec/1000.0;
  float usec  = 1000000*(fsec - int(fsec));
  it.it_interval.tv_sec  = int(fsec);
  it.it_interval.tv_usec = int(usec);
  it.it_value.tv_sec  = int(fsec);
  it.it_value.tv_usec = int(usec);
  setitimer(ITIMER_REAL, &it, NULL);
#endif
}

void BASE_WINDOW::start_timer(int msec, const window_handler& obj)
{ timer_action_ptr = & (window_handler&) obj;
  x_start_timer(draw_win,msec); 
#if defined(__unix__)
  timer_win = this;
  signal(SIGALRM,(leda_sig_pf)timer_handler);

  static itimerval it;
  float fsec  = msec/1000.0;
  float usec  = 1000000*(fsec - int(fsec));
  it.it_interval.tv_sec  = int(fsec);
  it.it_interval.tv_usec = int(usec);
  it.it_value.tv_sec  = int(fsec);
  it.it_value.tv_usec = int(usec);
  setitimer(ITIMER_REAL, &it, NULL);
#endif
}

void BASE_WINDOW::stop_timer() 
{ timer_action = 0;
  timer_action_ptr = 0;
  x_stop_timer(draw_win); 
#if defined(__unix__)
  static itimerval it;
  setitimer(ITIMER_REAL, &it, NULL);
#endif
}


int BASE_WINDOW::read_mouse_action(mouse_action_func action, double xstart, 
                                                             double ystart, 
                                                             double& x, 
                                                             double& y)
{
  if (!is_open() && buf_level == 0)
  { fprintf(stderr,"Error: Window has to be displayed before mouse input.\n");
    return NO_BUTTON;
   }


  read_window = this;

  mouse_action = action;

  mouse_key = 0;

  mouse_start_xreal = xstart;
  mouse_start_yreal = ystart;

  //mouse_last_xreal = xstart;
  //mouse_last_yreal = ystart;

  if (grid_mode > 0) cursor();
  
  if (coord_handler_ptr) {
    coord_handler_ptr->set_params(this,NULL,0,mouse_xreal,mouse_yreal);
    coord_handler_ptr->operator()();
  }
  else { if (coord_handler) coord_handler(this,mouse_xreal,mouse_yreal); }

  if (mouse_action) mouse_action(mouse_last_xreal,mouse_last_yreal);

  BASE_WINDOW* wp;
  while (event_handler(wp,0) != button_press_event || wp != this);

  if (mouse_action) mouse_action(mouse_xreal,mouse_yreal);

  if (grid_mode > 0) cursor();

  x = mouse_xreal;
  y = mouse_yreal;

  mouse_action = mouse_default_action;

  return mouse_key;
}


void BASE_WINDOW::iconify() { x_minimize_window(draw_win); }
void BASE_WINDOW::maximize() { x_maximize_window(draw_win); }


void BASE_WINDOW::close() 
{ 
  if (draw_win == 0) return;

  if (active_button_win == this) active_button_win = 0;

  x_stop_buffering(draw_win);
  x_delete_buffer(draw_win);

  if (scroll_bar)
  { delete scroll_bar;
    scroll_bar = 0;
   }

  destroy_status_window();

  if (this == active_window) active_window = 0;
  // close sub-windows
  for(int i=0; i<item_count; i++)
  { panel_item it=Item[i];
    if (it->kind == Button_Item && it->ref)
    { BASE_WINDOW* wp = (BASE_WINDOW*)it->ref;
      if (wp != this && wp->is_open()) wp->close();
     }
   }
  if (is_open()) x_close_window(draw_win); 

/*
  if (win_parent)
  { // handle exposure events of parent
    int e,w,v1,v2,x,y;
    unsigned long t;
    while ((e = x_get_next_event(w,x,y,v1,v2,t,-1)) != no_event)
    { if (w == win_parent->draw_win && e == configure_event)
      { x_put_back_event();
        event_handler(win_parent,0);
       }
     }
  }
*/

}


BASE_WINDOW::BASE_WINDOW(int width, int height, const char* frame_label) { 
  create(width,height,frame_label); 
}

BASE_WINDOW::BASE_WINDOW(const char* frame_label) { 
  create(0,0,frame_label); 
}

BASE_WINDOW::BASE_WINDOW() { 
  create(0,0,""); 
}


BASE_WINDOW::~BASE_WINDOW() 
{ 
  //cout << "BASE_WINDOW DESTRUCTOR: win_count = " << win_count << endl;

  close();

  if (scroll_bar) delete scroll_bar;
  if (status_win) delete scroll_bar;

  if (status_str) delete[] status_str;

  if (owner_item) 
  { owner_item->menu_win = 0;
    owner_item->ref = 0;
   }

  for(int i = 0; i<item_count; i++) delete Item[i];

  x_destroy_window(draw_win);

  if (icon_pixrect) x_delete_pixrect(icon_pixrect);
  icon_pixrect = 0;

  if (--win_count == 0) x_close_display(); 
 }



void BASE_WINDOW::create(int w_width, int w_height, const char* label)
{
  //disp_fd = 0;
  //if (win_count==0) disp_fd = x_open_display();

  disp_fd = x_open_display();

  if (disp_fd == -1) return;

  int dpi = screen_dpi();

  int def_width;
  int def_height;
  default_size(def_width,def_height);

  if (w_width == 0) w_width = def_width;
  if (w_height == 0) w_height = def_height;

  win_count++;

/*
  cout << "create: " << w_width << " x " << w_height << endl;
  cout << "label = " << label << endl;
  cout << "win_count = " << win_count << endl;
  cout << endl;
*/

  grawin_ptr = 0;
  geowin_ptr = 0;

  for(int i=0; i<16; i++) data[i] = 0;

  active_window = this;
  p_root = this;
  p_win = 0;

  win_parent = 0;

  current_event = no_event;

  state = 1;
  window_width  = w_width;
  window_height = w_height;

  window_width_save = 0;
  window_height_save = 0;

  icon_pixrect = 0;

  bg_pixrect = 0;
  bg_xoff = 0;
  bg_yoff = 0;
  bg_color = white;
  fg_color = black;
  fill_color = invisible;


  strncpy(default_frame_label,label,128);
  default_frame_label[127] = '\0';


  draw_win = x_create_window(this, window_width, window_height, bg_color, 
                             default_frame_label,default_frame_label,0,
                             REDRAW_FUNC);

  panel_init();

  panel_win = 0;

  
  if (draw_win)
  { x_set_text_font(draw_win);
    x_set_color(draw_win,black);
    x_set_mode(draw_win,src_mode);
    x_set_line_style(draw_win,solid);
    x_set_line_width(draw_win,1);
    x_set_text_mode(draw_win,transparent);
   }

  pt_style = cross_point;
  
  show_grid_cursor = 1;

  clear_on_resize = 1;

//node_width = 11;
  node_width = dpi/12;


  redraw0 = 0;
  
  redraw1 = 0;
  redraw1_ptr = 0;
  
  redraw2 = 0;
  redraw2_ptr = 0;

  clear_redraw = 0;  
  clear_redraw_ptr = 0;

  coord_handler = 0;
  coord_handler_ptr = 0;
  
  win_close_handler = 0;
  win_close_ptr = 0;
  
  user_event_handler = 0;

  timer_action = 0;
  timer_action_ptr = 0;

  hotx1 = 0;
  hotx2 = 0;
  hoty1 = 0;
  hoty2 = 0;

  mesg_count = 0;
  win_flush = 1;

  mode_for_scaling = scale_x;
  scaling = 1;
  one_over_scaling = 1;
  scaling_prec = 12;

  xorigin = 0;
  yorigin = 0;

  xoffset = 0;
  yoffset = 0;

  min_xcoord = 0;
  min_ycoord = 0;
  max_xcoord = 100;
  max_ycoord = 100;
  mouse_xpix = 0;
  mouse_ypix = 0;
  mouse_xreal = 0;
  mouse_yreal = 0;
  mouse_last_xreal = 0;
  mouse_last_yreal = 0;
  shift_key_state = 0;
  grid_mode  = 0;
  g_style  = point_grid;
  owner_item = 0;
  panel_enabled = true;
  buf_level = 0;

  d3_view_pos_x = 0;
  d3_view_pos_y = 0;
  d3_view_pos_z = 100;
  d3_view_norm_x = 0;
  d3_view_norm_y = 0;
  d3_view_norm_z = 100;

  status_str = 0;
  status_win = 0;

  scroll_bar = 0;

  std_buttons();
/*
  set_buttons(NO_BUTTON,MOUSE_BUTTON(1),MOUSE_BUTTON(2),MOUSE_BUTTON(3),
              NO_BUTTON,MOUSE_BUTTON(1),MOUSE_BUTTON(2),MOUSE_BUTTON(3));
*/

  close_button_enabled = false;

  if (coord_handler_ptr) {
    coord_handler_ptr->set_params(this ,NULL,0,mouse_xreal, mouse_yreal);
    coord_handler_ptr->operator()();
  }
  else {
   if (coord_handler) 
    (coord_handler)(this,mouse_xreal,mouse_yreal);
  }

  show_notice = true;
}

void BASE_WINDOW::set_topmost() { x_set_topmost(draw_win); }

void BASE_WINDOW::resize(int xpos, int ypos,int width, int height)
{ /*
  window_xpos = xpos;
  window_ypos = ypos;
  */

/*
  double ratio =  double(height)/width;

  int max_width = int(0.9*screen_width());
  int max_height = int(0.9*screen_height());

  if (width > max_width) {
    width = max_width;
    height = int(ratio*width);
    xpos = (screen_width() - width)/2;
    ypos = (screen_height() - height)/2;
  }

  if (height > max_height) {
    height = max_height;
    width = int(height/ratio);
    xpos = (screen_width() - width)/2;
    ypos = (screen_height() - height)/2;
  }
*/


  x_resize_window(draw_win,xpos,ypos,width,height, (win_parent != 0));
  configure();
  clipping(0);
 }


void BASE_WINDOW::resize(int width, int height)
{ int x0,y0,x1,y1;
  x_window_frame(draw_win,x0,y0,x1,y1); 
  resize(x0,y0,width,height);
}

 


/*
void BASE_WINDOW::set_size_hints(int wmin, int wmax, int hmin, int hmax)
{
  x_set_size_hints(draw_win, wmin, wmax, hmin, hmax);
}
*/

void BASE_WINDOW::display(int xpos, int ypos, BASE_WINDOW* w, bool hidden)
{
  assert(draw_win > 0);

  int dpi = screen_dpi();

  if (x_window_opened(draw_win)) 
  { redraw_panel();
    return;
   }

  disp_fd = x_open_display();
  if (disp_fd == -1) return;

  win_parent = w;

  int pw = 0;

  if (w)
  { if (w != this)  
      pw = w->draw_win;
    else
     { pw = -1;
       w = 0;
      }
   }


  // open parent if closed
  if (w && w->is_closed()) 
      w->display(BASE_WINDOW::center,BASE_WINDOW::center,0);


  if (item_count > 0)
  { panel_width = window_width;
    place_panel_items();
    window_width  = panel_width;
    if (window_height < panel_height) window_height = panel_height;
   }


  int parent_width = (w==0) ? screen_width() : w->width();
  int parent_height= (w==0) ? screen_height() : w->height();

  int width  = window_width;
  int height = window_height;


  if (xpos > 0xFFFF)
  {  switch (xpos) {

     case BASE_WINDOW::min:    
                  xpos = 0;
                  break;

     case BASE_WINDOW::center: 
                  //xpos = -(parent_width+window_width)/2;
                  xpos = (parent_width-window_width)/2 - 1;
                  if (pw == 0) xpos -= (12*screen_dpi())/192;  // frame
                  if (xpos < 0) xpos = 0;
                  break;

     case BASE_WINDOW::max:    
                  xpos = -(parent_width-1);
                  break;
     }
   }
   


  if (ypos > 0xFFFF)
  {  switch (ypos) {

     case BASE_WINDOW::min:   
                 ypos = 0;
                 break;

     case BASE_WINDOW::center: { 
                 if (width > 0.75*screen_width())
                 { // portrait (xx display)
                   ypos = int(0.40*(parent_height-window_height));
                  }
                 else
                   ypos = int(0.5*(parent_height-window_height));

                 if (pw == 0) ypos -= (60*screen_dpi())/192;  // frame
                 if (ypos < 0) ypos = 0;
                 break;
                }

     case BASE_WINDOW::max:    
                 ypos = -(parent_height-1);
                 break;
     }
  }


  if (xpos < 0)
  { xpos  = -xpos;
    width = -width;
   }
    
  if (ypos < 0)
  { ypos   = -ypos;
    height = -height;
   }
    
/*
  window_xpos = xpos;
  window_ypos = ypos;
*/


  if (window_height == panel_height) // panel
     x_set_bg_color(draw_win,panel_bg_color);

/*
  char* pr = 0;
  if (pw > 0) pr = x_create_pixrect(pw,xpos,ypos,xpos+width,ypos+height);
  if (pr)  
  { //x_insert_pixrect(draw_win,pr); 
    x_set_bg_pixmap(draw_win,pr);
  }
*/

  x_open_window(draw_win,xpos,ypos,width,height,pw,hidden);


#if !defined(__unix__)
  x_set_focus(draw_win);
#endif

  configure();
  redraw_panel();

  clear();

/*
  if (focus_button) 
  { int x = focus_button->xcoord + button_w/2;
    int y = focus_button->ycoord + button_h/2 + 3;
    x_move_pointer(draw_win,x,y);
   }
*/


  // set icon

  if (win_parent == 0) 
  { if (icon_pixrect == 0) icon_pixrect = create_pixrect_from_xpm(XPM_ICON);
    x_set_icon_pixmap(draw_win,icon_pixrect); 

/*
    if (getenv("LEDA_OPEN_ICONIFIED") && window_height > panel_height) 
      x_minimize_window(draw_win);

    if (getenv("LEDA_OPEN_MAXIMIZED") && window_height > panel_height) 
    { x_maximize_window(draw_win);
      redraw();
    }
*/

  }

}


void BASE_WINDOW::move_pointer(double x, double y)
{ x_move_pointer(draw_win,xpix(x),ypix(y)); }


void BASE_WINDOW::close_display() { x_close_display(); }


int BASE_WINDOW::panel_open(int x, int y, BASE_WINDOW* w)
{ 
  display(x,y,w);
  set_focus();
  int b = read_mouse();
  close();
  return b;
 }


int BASE_WINDOW::menu_open(int xpos, int ypos, BASE_WINDOW* w)
{ 
  p_win = w;
  place_panel_items();

  if (xpos + panel_width > w->width()) xpos = w->width() - panel_width;
  if (ypos + panel_height > w->height()) ypos = w->height() - panel_height;


#if defined(__APPLE__)
  display(xpos,ypos,w);
  x_set_border_width(draw_win,0);
#else
  x_set_border_width(draw_win,0);
  display(xpos,ypos,w);
#endif


  p_win = 0;

  w->menu_mode = 1; /* why ? */

  // added by sn.
  x_grab_pointer(draw_win);

  BASE_WINDOW* wp;
  double xc,yc;
  int b = read_mouse(wp,xc,yc);


  // mouse click outside of menu ?

  if (wp != this) b = NO_BUTTON;
  if (xc < 0 || xc > w->width()) b = NO_BUTTON;
  if (yc < 0 || yc > w->height()) b = NO_BUTTON;


  // close and handle remaining events 

  int e,ww,v1,v2,x,y;
  unsigned long t;
  while (x_get_next_event(ww,x,y,v1,v2,t,-1) != no_event);

  close();

  if (!x_window_bits_saved(w->draw_win) && win_parent != this)
  { while (x_get_next_event(ww,x,y,v1,v2,t) != exposure_event) {}
    x_put_back_event();
    event_handler(wp,0);
  }
  else
    while ((e=x_get_next_event(ww,x,y,v1,v2,t,-1)) != no_event)
    { if (e == exposure_event)
      { x_put_back_event();
        event_handler(wp,0);
       }
     }

  return b;
}


void BASE_WINDOW::set_offset(double dx, double dy)
{  xoffset = real_to_pix(dx);
   yoffset = real_to_pix(dy);
}

int BASE_WINDOW::set_precision(int prec)
{ int old_prec = scaling_prec;
  scaling_prec = prec;

  scaling = double(window_width)/(max_xcoord-min_xcoord);
  scaling = truncate(scaling,scaling_prec);

  one_over_scaling = double(max_xcoord-min_xcoord)/window_width;
  one_over_scaling = truncate(one_over_scaling,scaling_prec);

  return old_prec;
}


void BASE_WINDOW::configure()
{
/*
  printf("CONFIGURE STARTED\n");
  fflush(stdout);
*/

  int panel_only = (panel_height == window_height); 

  int win_width  = x_window_width(draw_win);
  int win_height = x_window_height(draw_win);
  bool size_changed= (win_width != window_width || win_height != window_height);

  window_width = win_width;
  window_height = win_height;

  if (item_count > 0) 
  { panel_width = window_width;
    place_panel_items();
    //if (panel_only) panel_height = window_height;
    if (panel_only) window_height = panel_height;
   }

  scaling_mode smode = mode_for_scaling;

//smode = scale_xy;

  double scaling_x = double(window_width)/(max_xcoord-min_xcoord);
  double scaling_y = double(window_height-panel_height)/(max_ycoord-min_ycoord);

  if (smode == scale_xy) { 
    smode = scaling_x <= scaling_y ? scale_x : scale_y;
  }
  
  if (smode == scale_x)
    scaling = scaling_x;
  else
    scaling = scaling_y;

  one_over_scaling = 1/scaling;


  scaling = truncate(scaling,scaling_prec);
  one_over_scaling = truncate(one_over_scaling,scaling_prec);

  if ((grid_mode > 0) && (grid_mode*scaling < 4))
  { // at least grid distance of 4 pixels
    grid_mode=0; 
    fprintf(stderr,"warning: grid distance to small.\n");
   }

  if (grid_mode > 0)
  { max_xcoord = min_xcoord+int(window_width*one_over_scaling);
    max_ycoord = min_ycoord+int((window_height-panel_height)*one_over_scaling);
   }
  else
  { max_xcoord = min_xcoord+window_width*one_over_scaling;
    max_ycoord = min_ycoord+(window_height-panel_height)*one_over_scaling;
   }

  xorigin = (int)(-min_xcoord*scaling);
  yorigin = (int)(window_height+min_ycoord*scaling);

  //mouse_xreal = 0;
  //mouse_yreal = 0;


  if (!is_open() || !size_changed) {
/*
    printf("END OF CONFIGURE 1\n");
    fflush(stdout);
*/

   return;
  }

/*
  printf("SIZE CHANGED\n");
  fflush(stdout);
*/

  int buffering = x_test_buffer(draw_win);
  x_stop_buffering(draw_win);

  if (status_win)
  { int w = window_width-2;
    int h = status_win->window_height;
    int x = 0; 
    int y = window_height-h-2;
    status_win->resize(x,y,w,h);
   }

  if (scroll_bar)
  { panel_item it0 = scroll_bar->Item[0];
    panel_item it1 = scroll_bar->Item[1];
    panel_item it2 = scroll_bar->Item[2];
    panel_action_func f0 = it0->action;
    panel_action_func f1 = it1->action;
    panel_action_func f2 = it2->action;
    double h = scroll_bar->panel_height - it0->height - it1->height;
    double sz = it2->height/h;
    //double y = it2->ycoord - (it2->height - scroll_bar->yskip)/2;
    double y = it2->ycoord;
    double pos = (y - it0->height)/(h - it2->height);
    close_scrollbar();
    open_scrollbar(f0,f1,f2,sz,pos);
   }

  if (buffering) {
    // create buffer for new window size
    x_start_buffering(draw_win);
  }

/*
  printf("CONFIGURE FINISHED\n");
  fflush(stdout);
*/
}



void BASE_WINDOW::init(double x0, double x1, double y0, int g_mode, int erase)
{ 
  if (x0 >= x1)
  { fprintf(stderr,"illegal arguments in W.init: x0 (%f) >= x1 (%f)\n",x0,x1);
    return;
   }

  mode_for_scaling = scale_x;
  min_xcoord = x0;
  max_xcoord = x1;
  min_ycoord = y0;
  grid_mode  = g_mode;

  configure();

  if (erase)
  { //configure();
    if (is_open()) clear();
   }
}


void BASE_WINDOW::init(double x0, double x1, double y0, double y1, int g_mode, int erase)
{ 
  if (y0 >= y1)
  { fprintf(stderr,"illegal arguments in W.init: x0 (%f) >= x1 (%f) or y0 (%f) >= y1 (%f)\n",x0,x1,y0,y1);
    return;
   }

  mode_for_scaling = scale_xy;
  min_xcoord = x0;
  max_xcoord = x1;
  min_ycoord = y0;
  max_ycoord = y1;
  grid_mode  = g_mode;

  configure();

  if (erase)
  { //configure();
    if (is_open()) clear();
   }
}


void BASE_WINDOW::init0(double x0, double x1, double y0, int bg_off_restore)
{ 
  int bgx = xpix(bg_xoff);
  int bgy = ypix(bg_yoff);

  mode_for_scaling = scale_x;
  min_xcoord = x0;
  max_xcoord = x1;
  min_ycoord = y0;

  scaling = double(window_width)/(max_xcoord-min_xcoord);
  one_over_scaling = double(max_xcoord-min_xcoord)/window_width;

  scaling = truncate(scaling,scaling_prec);
  one_over_scaling = truncate(one_over_scaling,scaling_prec);

  if (grid_mode > 0)
   { max_xcoord = min_xcoord+int(window_width*one_over_scaling);
     max_ycoord = min_ycoord+int((window_height-panel_height)*one_over_scaling);
    }
  else
   { max_xcoord = min_xcoord+window_width*one_over_scaling;
     max_ycoord = min_ycoord+(window_height-panel_height)*one_over_scaling;
    }

  xorigin = (int)(-min_xcoord*scaling);
  yorigin = (int)(window_height+min_ycoord*scaling);

  //if (bg_pixrect)
  if (bg_off_restore)
  { bg_xoff = xreal(bgx);
    bg_yoff = yreal(bgy);
   }
}





int BASE_WINDOW::query_pix(double x, double y)
{ return x_get_pixel(draw_win,xpix(x),ypix(y)); }


void BASE_WINDOW::draw_pix(double x, double y, int col)
{ if (col == invisible) return;
  set_color(col);
  x_pixel(draw_win,xpix(x),ypix(y));
  if (win_flush) flush();
}

void BASE_WINDOW::draw_pixels(int n, double* xcoord, double* ycoord, int col)
{ if (col == invisible) return;
  set_color(col);
  int* x = new int[n];
  int* y = new int[n];
  int i;
  for(i=0;i<n;i++)
  { x[i] = xpix(xcoord[i]);
    y[i] = ypix(ycoord[i]);
   }
  x_pixels(draw_win,n,x,y); 
  delete[] x;
  delete[] y;
  if (win_flush) flush();
}



void BASE_WINDOW::draw_point(double x, double y, int col)
{ if (col == invisible) return;
  int X = xpix(x);
  int Y = ypix(y);
  int ws = x_set_line_width(draw_win,1);
  line_style ls = x_set_line_style(draw_win,solid);
  set_color(col);

  float dpi = (float)screen_dpi();

/*
  int d = node_width/2;
*/

  int d = int(0.5 + dpi/32);
  if (d < 4) d = 4;

  switch (pt_style) {

   case pixel_point:  x_pixel(draw_win,X,Y);
                      break;

   case cross_point:  d = int(0.75*d);
                      x_line(draw_win,X-d,Y-d,X+d,Y+d);
                      x_line(draw_win,X-d,Y+d,X+d,Y-d);
                      break;

   case plus_point:   d = int(0.75*d);
                      x_line(draw_win,X,Y-d,X,Y+d);
                      x_line(draw_win,X-d,Y,X+d,Y);
                      break;

   case circle_point: set_color(fill_color);
                      x_fill_ellipse(draw_win,X,Y,d,d);
                      set_color(col);
                      x_ellipse(draw_win,X,Y,d,d);
                      break;

   case rect_point:   set_color(fill_color);
                      x_box(draw_win,X-d,Y-d,X+d,Y+d);
                      set_color(col);
                      x_rect(draw_win,X-d,Y-d,X+d,Y+d);
                      break;

   case disc_point:   x_fill_ellipse(draw_win,X,Y,d,d);
                      x_ellipse(draw_win,X,Y,d,d);
                      break;

   case box_point:    x_box(draw_win,X-d,Y-d,X+d,Y+d);
                      break;

  }

  x_set_line_width(draw_win,ws);
  x_set_line_style(draw_win,ls);

  if (win_flush) flush();
}


void BASE_WINDOW::draw_segment(double x1, double y1, double x2, double y2, int col)
{ if (col == invisible) return;
  set_color(col);
  x_line(draw_win, xpix(x1), ypix(y1), xpix(x2), ypix(y2));
  if (win_flush) flush();
}


void BASE_WINDOW::draw_segments(int n, double* xcoord1, double* ycoord1, 
                                       double* xcoord2, double* ycoord2, int col)
{ if (col == invisible) return;
  set_color(col);

  int* x1 = new int[n];
  int* y1 = new int[n];
  int* x2 = new int[n];
  int* y2 = new int[n];
  int i;

  for(i=0;i<n;i++)
  { x1[i] = xpix(xcoord1[i]);
    y1[i] = ypix(ycoord1[i]);
    x2[i] = xpix(xcoord2[i]);
    y2[i] = ypix(ycoord2[i]);
   }

  x_lines(draw_win,n,x1,y1,x2,y2); 

  delete[] x1;
  delete[] y1;
  delete[] x2;
  delete[] y2;
  if (win_flush) flush();
}


void BASE_WINDOW::draw_line(double x1, double y1, double x2, double y2, int col)
{
  double dx = x2 - x1;
  double dy = y2 - y1;

  if (dx == 0 && dy == 0)
  { draw_pix(x1,y1,col);
    return;
   }

/*
  double xl = xmin();
  double yl = ymin();
  double xr = xmax();
  double yr = ymax();
*/

  double xl = xreal(0);
  double xr = xreal(window_width);
  double yl = yreal(window_height);
  double yr = yreal(0);


  if (fabs(dy) < fabs(dx))
  { yl = y1 + (xl-x1)*dy/dx;
    yr = y1 + (xr-x1)*dy/dx;
   }
  else
  { xl = x1 + (yl-y1)*dx/dy;
    xr = x1 + (yr-y1)*dx/dy;
   }

  BASE_WINDOW::draw_segment(xl,yl,xr,yr,col);

}



void BASE_WINDOW::draw_ray(double x1, double y1, double x2, double y2, int col)
{
  double dx = x2 - x1;
  double dy = y2 - y1;

  if (dx == 0 && dy == 0)
  { draw_pix(x1,y1,col);
    return;
   }

  double xmin = xreal(0);
  double xmax = xreal(window_width);
  double ymin = yreal(window_height);
  double ymax = yreal(0);

  double x,y;

  if (fabs(dy) < fabs(dx))
    { x = (x1 < x2) ? xmax : xmin;
      y = y1 + (x-x1)*dy/dx;
     }
  else
    { y = (y1 < y2) ? ymax : ymin;
      x = x1 + (y-y1)*dx/dy;
     }

  BASE_WINDOW::draw_segment(x1,y1,x,y,col);

}



void BASE_WINDOW::draw_arc(double x0, double y0, double r1, double r2, 
                                                            double start, 
                                                            double angle, 
                                                            int col)
{ if (col == invisible) return;
  set_color(col);
  int R1 = real_to_pix(r1);
  int R2 = real_to_pix(r2);
  x_arc(draw_win,xpix(x0),ypix(y0),R1,R2,start,angle);
  if (win_flush) flush();
}

void BASE_WINDOW::draw_filled_arc(double x0, double y0, double r1, double r2, 
                                                                   double start,
                                                                   double angle,
                                                                   int col)
{ if (col == invisible) return;
  set_color(col);
  int R1 = real_to_pix(r1);
  int R2 = real_to_pix(r2);
  x_fill_arc(draw_win,xpix(x0),ypix(y0),R1,R2,start,angle);
  if (win_flush) flush();
}


void BASE_WINDOW::draw_node(double x0, double y0, int col)
{ if (col == invisible) return;
  int save = x_set_line_width(draw_win,1);
  double R = pix_to_real(node_width);
  draw_circle(x0,y0,R,col);
  x_set_line_width(draw_win,save);
 }

void BASE_WINDOW::draw_filled_node(double x0, double y0, int col)
{ if (col == invisible) return;
  set_color(col);
  int X = xpix(x0);
  int Y = ypix(y0);
  x_fill_ellipse(draw_win,X,Y,node_width,node_width);
  int lw = x_set_line_width(draw_win,1);
  set_color(black);
  x_ellipse(draw_win,X,Y,node_width,node_width);
  x_set_line_width(draw_win,lw);
  if (win_flush) flush();
 }


void BASE_WINDOW::draw_text_node(double x0, double y0, const char *s, int col)
{ text_mode t_save = x_set_text_mode(draw_win,transparent);

  if (col == DEF_COLOR) col = bg_color;

  draw_filled_node(x0,y0,col);
  
  draw_ctext(x0,y0,s,col);

  if (col == black || col == blue || col == violet || col == brown) 
     draw_ctext(x0,y0,s,white);
  else
     draw_ctext(x0,y0,s,black);

  x_set_text_mode(draw_win,t_save);
}

void BASE_WINDOW::draw_int_node(double x0, double y0, int i, int col)
{ char buf[16];
  sprintf(buf,"%d",i);
  draw_text_node(x0,y0,buf,col);
 }


void BASE_WINDOW::draw_edge(double x1, double y1, double x2, double y2, int col)
{ 
  if (col == invisible) return;

  double dx = x2-x1;
  double dy = y2-y1;
  double L  = scaling*HyPot(dx,dy);

  if (L > 2*node_width)
  { set_color(col);
    double l  = double(node_width+1)/L;
    x1 += l*dx;
    x2 -= l*dx;
    y1 += l*dy;
    y2 -= l*dy;
    x_line(draw_win,xpix(x1),ypix(y1),xpix(x2),ypix(y2));
    if (win_flush) flush();
   }
}
 

void BASE_WINDOW::draw_circle(double x, double y, double r, int col)
{ if (col == invisible) return;
  set_color(col);
  int R = real_to_pix(r);
  x_ellipse(draw_win,xpix(x),ypix(y),R,R);
  if (win_flush) flush();
 }


void BASE_WINDOW::draw_filled_circle(double x, double y, double r, int col)
{ if (col == invisible) return;
  set_color(col);
  int R = real_to_pix(r);
  if (R > 0)
     x_fill_ellipse(draw_win,xpix(x),ypix(y),R,R);
  else
     x_pixel(draw_win,xpix(x),ypix(y));
  if (win_flush) flush();
 }


void BASE_WINDOW::draw_ellipse(double x0, double y0, double a, double b, int col)
{ if (col == invisible) return;
  set_color(col);
  int R1 = real_to_pix(a);
  int R2 = real_to_pix(b);
  x_ellipse(draw_win,xpix(x0),ypix(y0),R1,R2);
  if (win_flush) flush();
 }


void BASE_WINDOW::draw_filled_ellipse(double x0, double y0, double a, double b, int col)
{ if (col == invisible) return;
  set_color(col);
  int R1 = real_to_pix(a);
  int R2 = real_to_pix(b);
  x_fill_ellipse(draw_win,xpix(x0),ypix(y0),R1,R2);
  if (win_flush) flush();
 }



void BASE_WINDOW::plot_xy(double x0, double x1, win_draw_func f, int col)
{
  if (col == invisible) return;
  set_color(col);

  int *xcoord;
  int *ycoord;

  int x = xpix(x0);
  int y_old = ypix((*f)(x0));
  int i,y_new;
  int size = 0;
  int n = 0;


  for(x = xpix(x0)+1; x <= xpix(x1); x++)
  { y_new = ypix((*f)(xreal(x)));
    if (y_new > y_old)
       size += (y_new-y_old+1);
    else
       size += (y_old-y_new+1);
    y_old = y_new;
   }

  xcoord = new int[size];
  ycoord = new int[size];

  y_old = ypix((*f)(x0));

  for(x = xpix(x0)+1; x <= xpix(x1); x++)
  { y_new = ypix((*f)(xreal(x)));
    if (y_new > y_old)
      for(i=y_old; i<=y_new; i++) 
      { xcoord[n] = x;
        ycoord[n] = i;
        n++;
       }
    else
      for(i=y_old; i>=y_new; i--) 
      { xcoord[n] = x;
        ycoord[n] = i;
        n++;
       }
    y_old = y_new;
  }

 x_pixels(draw_win,size,xcoord,ycoord);
 
 delete[] xcoord;
 delete[] ycoord;

  if (win_flush) flush();
}


void BASE_WINDOW::plot_yx(double y0, double y1, win_draw_func f, int col)
{
  if (col == invisible) return;
  set_color(col);

  int *xcoord;
  int *ycoord;

  int y;
  int i,x_new;
  int x_old = xpix((*f)(y0));
  int size = 0;
  int n = 0;


  for(y = ypix(y0)-1; y >= ypix(y1); y--)
  { x_new = xpix((*f)(yreal(y)));
    if (x_new > x_old)
       size += (x_new-x_old+1);
    else
       size += (x_old-x_new+1);
    x_old = x_new;
   }

  xcoord = new int[size];
  ycoord = new int[size];

  x_old = xpix((*f)(y0));

  for(y = ypix(y0)-1; y >= ypix(y1); y--)
  {
    x_new = xpix((*f)(yreal(y)));
    if (x_new > x_old)
      for(i=x_old; i<=x_new; i++) 
      { xcoord[n] = i;
        ycoord[n] = y;
        n++;
       }
    else
      for(i=x_old; i>=x_new; i--) 
      { xcoord[n] = i;
        ycoord[n] = y;
        n++;
       }
    x_old = x_new;
  }

 x_pixels(draw_win,size,xcoord,ycoord);
 
 delete[] xcoord;
 delete[] ycoord;

 if (win_flush) flush();
}

void BASE_WINDOW::adjust_polyline(int n, double *xcoord, double *ycoord)
{
 int* x = new int[n];
 int* y = new int[n];
 int i;
 for(i=0;i<n;i++)
 { x[i] = xpix(xcoord[i]);
   y[i] = ypix(ycoord[i]);
  }
 x_polyline(draw_win,n,x,y,1);
 xcoord[0] = xreal(x[0]);
 ycoord[0] = yreal(y[0]);
 xcoord[n-1] = xreal(x[n-1]);
 ycoord[n-1] = yreal(y[n-1]);
 delete[] x;
 delete[] y;
}


void BASE_WINDOW::draw_polyline(int n, double *xcoord, double *ycoord, int col)
{
 if (col == invisible) return;
 set_color(col);

 int* x = new int[n];
 int* y = new int[n];
 int i;

 for(i=0;i<n;i++)
 { x[i] = xpix(xcoord[i]);
   y[i] = ypix(ycoord[i]);
  }

 x_polyline(draw_win,n,x,y);

 delete[] x;
 delete[] y;

 if (win_flush) flush();
}

void BASE_WINDOW::draw_polyline(int n, double *xcoord, double *ycoord, int* clr)
{
 int* x = new int[n];
 int* y = new int[n];

 for(int i=0;i<n;i++)
 { x[i] = xpix(xcoord[i]);
   y[i] = ypix(ycoord[i]);
  }

 x_polyline(draw_win,n,x,y,clr);

 delete[] x;
 delete[] y;

 if (win_flush) flush();
}



void BASE_WINDOW::draw_polygon(int n, double *xcoord, double *ycoord, int col)
{
 if (col == invisible) return;
 set_color(col);

 int* x = new int[n+1];
 int* y = new int[n+1];
 int i;

 for(i=0;i<n;i++)
 { x[i] = xpix(xcoord[i]);
   y[i] = ypix(ycoord[i]);
  }

 x[n] = x[0];
 y[n] = y[0];

 x_polyline(draw_win,n+1,x,y);

 delete[] x;
 delete[] y;

  if (win_flush) flush();
}




void BASE_WINDOW::draw_filled_polygon(int n, double *xcoord, double *ycoord, int col)
{
 if (col == invisible) return;
 set_color(col);

 int* x = new int[n];
 int* y = new int[n];
 int i;

 for(i=0;i<n;i++)
 { x[i] = xpix(xcoord[i]);
   y[i] = ypix(ycoord[i]);
  }

 x_fill_polygon(draw_win,n,x,y);

 delete[] x;
 delete[] y;

  if (win_flush) flush();
}



void BASE_WINDOW::draw_rectangle(double x1, double y1, double x2, double y2, int col)
{ if (col == invisible) return;
  set_color(col);
  x_rect(draw_win,xpix(x1),ypix(y1),xpix(x2),ypix(y2));
  if (win_flush) flush();
 }


void BASE_WINDOW::draw_filled_rectangle(double x1, double y1, double x2, double y2, int col)
{ if (col == invisible) return;
  set_color(col);
  x_box(draw_win,xpix(x1),ypix(y1),xpix(x2),ypix(y2));
  if (win_flush) flush();
 }



inline void swap_coords(int* X, int* Y, int i, int j)
{ int  tmp;
  tmp = X[i]; X[i] = X[j]; X[j] = tmp;
  tmp = Y[i]; Y[i] = Y[j]; Y[j] = tmp;
}

int circle_quadrant(int* X, int* Y, int x0, int y0, int r, int f1, int f2)
{
  int y = r;
  int x = 0;
  int e = 3-2*y;
  int n = 0;

  while (x < y)
  { X[n] = x;
    Y[n] = y;
    n++;
    x++;
    if (e >= 0) { y--; e -= 4*y; }
    e += 4*x+2;
   }

  int j = n-1;

  while(j >= 0)
  { X[n] = Y[j];
    Y[n] = X[j];
    n++;
    j--;
   }

  for(j=0; j<n; j++)
  { X[j] = x0 + f1*X[j];
    Y[j] = y0 + f2*Y[j];
   }

  if (f1 != f2)
    for(j=0; j<n/2; j++) swap_coords(X,Y,j,n-j-1);

  return n;
}



int BASE_WINDOW::construct_roundrect(int*& xc, int*& yc, double x0, double y0, 
                                                         double x1, double y1, 
                                                         double rndness)
{ if (x0 > x1) { double t = x0; x0 = x1; x1 = t; }
  if (y0 > y1) { double t = y0; y0 = y1; y1 = t; }
  double w = x1 - x0;
  double h = y1 - y0;
  double r;
  if (w > h)
     r = rndness*h/2;
  else
     r = rndness*w/2;

  int R = real_to_pix(r);

  int X0 = xpix(x0);
  int Y0 = ypix(y0);
  int X1 = xpix(x1);
  int Y1 = ypix(y1);

  xc = new int[8*(R+1)+1];
  yc = new int[8*(R+1)+1];

  int n = 0;

  n += circle_quadrant(xc+n,yc+n,X1-R,Y0-R,R,+1,+1);
  n += circle_quadrant(xc+n,yc+n,X1-R,Y1+R,R,+1,-1);
  n += circle_quadrant(xc+n,yc+n,X0+R,Y1+R,R,-1,-1);
  n += circle_quadrant(xc+n,yc+n,X0+R,Y0-R,R,-1,+1);

  xc[n] = xc[0];
  yc[n] = yc[0];

  return n+1;
}


void BASE_WINDOW::draw_roundbox(double x0, double y0, 
                                double x1, double y1, double rndness, int col)
{ if (col == invisible) return;
  int* xc;
  int* yc;
  int n = construct_roundrect(xc,yc,x0,y0,x1,y1,rndness);
  set_color(col);
  x_fill_polygon(draw_win,n,xc,yc);
  if (win_flush) flush();
  delete[] xc;
  delete[] yc;
}


void BASE_WINDOW::draw_roundrect(double x0, double y0, 
                                 double x1, double y1, double rndness, int col)
{ if (col == invisible) return;
  int* xc;
  int* yc;
  int n = construct_roundrect(xc,yc,x0,y0,x1,y1,rndness);
  set_color(col);
  x_polyline(draw_win,n,xc,yc);
  if (win_flush) flush();
  delete[] xc;
  delete[] yc;
}



char* BASE_WINDOW::create_bitmap(int w, int h, unsigned char* pr_data)
{ return x_create_bitmap(draw_win,w,h,pr_data,1); }


char* BASE_WINDOW::create_pixrect_from_bits(int w,int h, unsigned char* pr_data,
                                               int fcol, int bcol)
{ return x_create_pixrect_from_bits(draw_win,w,h,pr_data,fcol,bcol); }



// xpm functions

static char* xpm_mask(const char** xpm) 
{ 
  int width;
  int height;
  int colors;
  int chars;
  sscanf(*xpm,"%d %d %d %d",&width,&height,&colors, &chars);

  if (chars > 2)
  { fprintf(stderr,"xpm: sorry, chars_per_pixel > 2 not implemented.\n");
    return 0;
   }

  char black_c1 = 0;
  char black_c2 = 0;

  int i;
  for(i=0; i<colors; i++)
  { xpm++;
    char c1=0;
    char c2=0;
    char rgb_str[16];

    if (chars == 1) sscanf(*xpm,"%c c %s",&c1,rgb_str);
    if (chars == 2) sscanf(*xpm,"%c%c c %s",&c1,&c2,rgb_str);

    if (strcmp(rgb_str,"None") == 0)
    { black_c1 = c1;
      black_c2 = c2;
     }
   }

  int   bytes = height*width/8;
  char* bits = new char[bytes];

  for(i=0; i<bytes; i++) bits[i] = 0;

  int pos = 0;
  for(int y=0; y<height; y++)
  { const char* line = *++xpm;
    for(int x=0; x<width; x++)
    { char c1 = line[x*chars];
      char c2 = 0;
      if (chars == 2) c2 = line[x*chars+1];
      if (c1 == black_c1 && c2 == black_c2)  bits[pos/8] |= (1 << (7-(pos%8)));
      pos++;
     }
   }

  return bits;
}


static unsigned int* xpm_to_bgra(const char** xpm, int& width, int& height) 
{
  int colors;
  int chars;
  sscanf(*xpm,"%d %d %d %d",&width,&height,&colors, &chars);

  if (chars > 2)
  { fprintf(stderr,"xpm: sorry, chars_per_pixel > 2 not implemented.\n");
    return 0;
   }

  unsigned color_table[1<<16]; 
  int i = 1<<16; 
  while (i--) color_table[i] = 0xffffff00; // rgba

  for(i=0; i<colors; i++)
  { xpm++;
    char c1=0;
    char c2=0;
    char rgb_str[16];

    //"xy c #rrggbb"

    if (chars == 1) sscanf(*xpm,"%c c %s",&c1,rgb_str);
    if (chars == 2) sscanf(*xpm,"%c%c c %s",&c1,&c2,rgb_str);

    int len = (int)strlen(rgb_str);

    if (strcmp(rgb_str,"None") == 0)
    { color_table[256*c2 + c1] = 0x00000000; // alpha = 0 (transparent)
      //color_table[256*c2 + c1] = 0xffffffff;
     }
    else
      { if (len == 13)
        { rgb_str[3] = rgb_str[5];
          rgb_str[4] = rgb_str[6];
          rgb_str[5] = rgb_str[9];
          rgb_str[6] = rgb_str[10];
          rgb_str[7] = 0;
         }
    
        unsigned clr;
        sscanf(rgb_str+1,"%x",&clr);
        color_table[256*c2 + c1] = clr + 0xff000000; // alpha = 255
      }
   }

  xpm++;

  unsigned int* bgra = new unsigned int[width*height];

  for(int y=0; y<height; y++)
  { for(int x=0; x<width; x++)
    { const char* line = xpm[y];
      int index = line[x*chars];
      if (chars == 2) index += 256*line[x*chars+1];
      bgra[y*width+x] = color_table[index];
    }
  }

  return bgra;
}


char* BASE_WINDOW::create_pixrect_from_xpm(const char** xpm)
{ int width = 0;
  int height = 0;
  unsigned int* bgra = xpm_to_bgra(xpm,width,height);

  float f = screen_dpi()/96.0f;

  char* prect = 
    x_create_pixrect_from_bgra(draw_win,width,height, (unsigned char*)bgra,f);

  delete[] bgra;
  return prect;
}




unsigned char* BASE_WINDOW::pixrect_to_bgra(char* prect) { 
   return x_pixrect_to_bgra(draw_win,prect); 
}


char* BASE_WINDOW::create_pixrect_from_color(int w,int h, unsigned int clr)
{ int sz = w*h;
  unsigned int* data = new unsigned int[sz];
  for(int i=0; i<sz; i++) data[i] = clr;
  char* prect = x_create_pixrect_from_bgra(draw_win,w,h, (unsigned char*)data);
  delete[] data;
  return prect;
}



char* BASE_WINDOW::create_pixrect_from_rgba(int w,int h, unsigned char* rgba,
                                                         float f)
{ int len = w*h;
  unsigned char* bgra = new unsigned char[4*len];
/*
  for(int i=0; i<4*len; i++) bgra[i] = 0xff;
*/

  for(int i=0; i<len; i++)
  { int j = 4*i;
    bgra[j+0] = rgba[j+2];
    bgra[j+1] = rgba[j+1];
    bgra[j+2] = rgba[j+0];
    bgra[j+3] = rgba[j+3];
  }

  char * prect = x_create_pixrect_from_bgra(draw_win,w,h,bgra,f);
  //delete[] bgra;
  return prect;
}



char* BASE_WINDOW::create_pixrect_from_rgb(int w,int h, unsigned char* data, 
                                                        float f)
{ int len = w*h;
  unsigned char* bgra = new unsigned char[4*len];

  unsigned int* p = (unsigned int*) bgra;

  for(int i=0; i<len; i++)
  { unsigned char r = data[3*i+0];
    unsigned char g = data[3*i+1];
    unsigned char b = data[3*i+2];
    unsigned char a = 255;
   *p++ = (a<<24) + (r<<16) + (g<<8) + b;
  }

  char* prect = x_create_pixrect_from_bgra(draw_win,w,h,bgra,f);
  delete[] bgra;
  return prect;
}

char* BASE_WINDOW::create_pixrect_from_bgra(int w, int h, unsigned char* data, 
                                                                       float f) 
{ 
  return x_create_pixrect_from_bgra(draw_win,w,h,data+4,f);
}



char* BASE_WINDOW::create_pixrect_from_bgra(unsigned char* data, float f)
{ int w = data[0] + 256*data[1];
  int h = data[2] + 256*data[3];
  return x_create_pixrect_from_bgra(draw_win,w,h,data+4,f);
}


char* BASE_WINDOW::get_pixrect(double x1, double y1, double x2, double y2)
{ clipping(0);
  char* pr = x_create_pixrect(draw_win, xpix(x1), ypix(y2), xpix(x2), ypix(y1));
  clipping(2);
  return pr;
 }


char* BASE_WINDOW::get_window_pixrect()
{ clipping(0);
  //char* pr = x_create_pixrect(draw_win,0,0, window_width, window_height);
  char* pr = x_create_pixrect(draw_win,0,0, window_width-1, window_height-1);
  clipping(2);
  return pr;
 }


void BASE_WINDOW::set_pixrect_alpha(char* prect, int a) {
  x_set_pixrect_alpha(prect,a);
}

void BASE_WINDOW::set_pixrect_phi(char* prect, float phi) {
  x_set_pixrect_phi(prect,phi);
}

void BASE_WINDOW::set_pixrect_anchor(char* prect, int x, int y) {
  x_set_pixrect_anchor(prect,x,y);
}

void BASE_WINDOW::get_pixrect_anchor(char* prect, int& x, int& y) {
  x_get_pixrect_anchor(prect,x,y);
}


/*
static BASE_WINDOW* progress_win;
static int progress_val = 0;

static void open_progress_win(void* win, int max)
{ BASE_WINDOW* P = new BASE_WINDOW(-1,-1,"pixmap2ps");
  progress_val = 0;
  P->text_item("\\bf\\blue Converting X11 Pixmap to Postscript");
  P->slider_item("line",&progress_val,0,max,0,"");
  P->display(BASE_WINDOW::center,BASE_WINDOW::center,(BASE_WINDOW*)win);
  progress_win = P;
}

static void close_progress_win()
{ leda_wait(1.5);
  delete progress_win; 
}

static void show_progress(int i)
{ if (progress_val != i)
  { progress_val = i;
    progress_win->redraw_panel();
   }
 }
*/



void BASE_WINDOW::pixrect_to_ps(char* pmap, ostream& psout, bool full_color)
{ 
  int w = get_pixrect_width(pmap);
  int h = get_pixrect_height(pmap);
  int sz = w*h;

  unsigned char* buf = x_pixrect_to_bgra(draw_win,pmap);
  unsigned int* p = (unsigned int*)buf;

  for(int i=0; i<sz; i++)
  { int clr = p[i];

    int b = clr & 0xff;
    int g = (clr>>8) & 0xff;
    int r = (clr>>16) & 0xff;

    if (full_color)
      psout << string("%02x%02x%02x",r,g,b);
    else
      psout << string("%02x",(r+g+b)/3);

    if (i > 0 && (i%72) == 0) psout << endl;
  }
  psout << endl;

  delete[] buf;
}


  
void BASE_WINDOW::screenshot(const char* fn, bool full_color)
{
  int fnn = (int)strlen(fn);

  char fname[256];
  strcpy(fname,fn);

  char* p = fname + fnn;

  while (p != fname && *p != '.') p--;

  if (strcmp(p,".wmf") != 0 && strcmp(p,".ps") != 0)
  { p =  fname + fnn;
#if defined(__win32__)
    sprintf(p,".wmf");
#else
    sprintf(p,".ps");
#endif
   }

  if (strcmp(p,".wmf") == 0) screenshot_wmf(fname,full_color);
  if (strcmp(p,".ps") == 0) screenshot_ps(fname,full_color);
}
  



void BASE_WINDOW::screenshot_ps(const char* fname, bool full_color)
{ 
  ofstream o(fname);

  if (o.fail()) 
  { acknowledge("Cannot write file",fname);
    return;
   }

  int x0,y0,x1,y1;

  if (win_parent == 0)
    x_window_frame(draw_win,x0,y0,x1,y1);
  else
  { x0 = 0; 
    y0 = 0;
    x_window_to_screen(draw_win,x0,y0);
    x1 = window_width;
    y1 = window_height;
    x_window_to_screen(draw_win,x1,y1);
   }

  char* pmap = x_root_pixrect(x0,y0,x1,y1);

  int w = get_pixrect_width(pmap);
  int h = get_pixrect_height(pmap);

  int dw = 20;
  int dh = 20;

  time_t t;
  time(&t);

  o << "%!PS-Adobe-2.0" << endl;
  o << "%%Creator: LEDA Window" << endl;
  o << "%%CreationDate: " << ctime(&t);
  o << "%%Pages:  1" << endl;
  o << "%%BoundingBox: " << " 0 0 " << (w+2*dw) << " " << (h+2*dh) << endl; 
  o << "%%EndComments" << endl;
  o << endl;

  o << "/draw_pixmap {" << endl;
  o << " 3 dict begin" << endl;
  o << "   /h exch def /w exch def" << endl;
  o << "   /pix w 3 mul string def" << endl;
  o << "   w h scale" << endl;
  o << "   w h 8 [w 0 0  0 h sub  0 h]" << endl;
  o << "   {currentfile pix readhexstring pop}" << endl;
  o << "   false 3 colorimage" << endl;
  o << "} def" << endl;
  o << endl;

  o << "/draw_grey_pixmap {" << endl;
  o << " 3 dict begin" << endl;
  o << "   /h exch def /w exch def" << endl;
  o << "   /pix w string def" << endl;
  o << "   w h scale" << endl;
  o << "   w h 8 [w 0 0  0 h sub  0 h]" << endl;
  o << "   {currentfile pix readhexstring pop}" << endl;
  o << "   image" << endl;
  o << "} def" << endl;
  o << endl;


  o << "gsave" << endl;
  o << endl;
  o << dw << " " << dh << " translate" <<endl;
  o << endl;

  if (full_color)
     o << w << " " << h << " draw_pixmap" << endl;
  else
     o << w << " " << h << " draw_grey_pixmap" << endl;

  pixrect_to_ps(pmap,o,full_color);


  o << endl;
  o << "grestore" << endl;
  o << "showpage" << endl;
  o << endl;

  x_delete_pixrect(pmap);
}

void BASE_WINDOW::screenshot_wmf(const char* fname, bool full_color)
{ 
#if !defined(__win32__)
  ofstream out(fname);
  out << "WMF-format available only for MS-Windows." << endl;
#else

  int x0,y0,x1,y1;

  if (win_parent == 0)
    x_window_frame(draw_win,x0,y0,x1,y1);
  else
  { x0 = 0; 
    y0 = 0;
    x_window_to_screen(draw_win,x0,y0);
    x1 = window_width;
    y1 = window_height;
    x_window_to_screen(draw_win,x1,y1);
   }

  char* pmap = x_root_pixrect(x0,y0,x1,y1);

  open_metafile(fname);
  put_pixrect(0,0,pmap);
  close_metafile();
  x_delete_pixrect(pmap);

#endif
}



void BASE_WINDOW::text_to_clipboard(const char* txt)
{ x_text_to_clipboard(draw_win,txt); }

char* BASE_WINDOW::text_from_clipboard()
{ return x_text_from_clipboard(draw_win); }

void BASE_WINDOW::pixrect_to_clipboard(char* pmap)
{ x_pixrect_to_clipboard(draw_win,pmap); }


char* BASE_WINDOW::pixrect_from_clipboard()
{ return x_pixrect_from_clipboard(draw_win); }


void BASE_WINDOW::open_metafile(const char* fname)
{ if (strlen(fname) > 0) 
     x_open_metafile(draw_win,fname); 
  else
     x_open_metafile(draw_win,NULL); 
}

void BASE_WINDOW::close_metafile()
{ x_close_metafile(draw_win); }

void BASE_WINDOW::metafile_to_clipboard()
{ x_metafile_to_clipboard(draw_win); }


void BASE_WINDOW::load_metafile(double x0, double y0, double x1, 
                                                      double y1,
                                                      const char* fname)
{ x_load_metafile(draw_win,xpix(x0),ypix(y1),xpix(x1),ypix(y0),fname); }






void BASE_WINDOW::put_bitmap(double x, double y, char* bm, int col)
{ x_set_color(draw_win,col);
  x_insert_bitmap(draw_win, xpix(x), ypix(y), bm); 
  if (win_flush) flush();
 }


void  BASE_WINDOW::put_pixrect(double x, double y, char* prect)
{ x_insert_pixrect(draw_win, xpix(x), ypix(y), prect); 
  if (win_flush) flush();
 }

void  BASE_WINDOW::set_pixrect(char* prect)
{ x_insert_pixrect(draw_win,prect); 
  if (win_flush) flush();
 }


void  BASE_WINDOW::fit_pixrect(char* prect)
{ //x_fit_pixrect(draw_win,prect); 
  //if (win_flush) flush();
  scale_pixrect(xmin(),ymin(),window_width, window_height,prect);
 }



void  BASE_WINDOW::center_pixrect(double x, double y, char* prect)
{ int w,h;
  x_get_pixrect_size(prect,w,h);
  int X = xpix(x - pix_to_real(w)/2);
  int Y = ypix(y - pix_to_real(h)/2);
  x_insert_pixrect(draw_win,X,Y,prect); 
/*
  x_set_pixrect_anchor(prect,w/2,h/2);
  x_insert_pixrect(draw_win,xpix(x),ypix(y),prect); 
*/
  if (win_flush) flush();
 }

void  BASE_WINDOW::center_pixrect(char* prect)
{ // fit into window
/*
  x_insert_pixrect(draw_win,prect); 
  if (win_flush) flush();
*/
  double x = (xmin() + xmax())/2;
  double y = (ymin() + ymax())/2;
  center_pixrect(x,y,prect);
 }



void  BASE_WINDOW::put_pixrect(double x, double y, char* rect, int x0, 
                                                               int y0, 
                                                               int width, 
                                                               int height)
{ x_insert_pixrect(draw_win, xpix(x), ypix(y), rect, x0, y0, width,height);
  if (win_flush) flush();
 }


void  BASE_WINDOW::scale_pixrect(double x, double y, int w, int h, char* rect)
{ x_insert_pixrect(draw_win, xpix(x), ypix(y),w,h,rect);
  if (win_flush) flush();
 }


void  BASE_WINDOW::del_bitmap(char* rect) { x_delete_bitmap(rect); }
void  BASE_WINDOW::del_pixrect(char* rect) { x_delete_pixrect(rect); }

int BASE_WINDOW::get_pixrect_width(char* rect) const
{ int w,h;
  x_get_pixrect_size(rect,w,h);
  return w;
}

int BASE_WINDOW::get_pixrect_height(char* rect) const
{ int w,h;
  x_get_pixrect_size(rect,w,h);
  return h;
}


void BASE_WINDOW::copy_rect(double x1, double y1, double x2, double y2, 
                                                  double x, double y)
{ 
  int X1 = xpix(x1);
  int Y1 = xpix(y1);
  int X2 = xpix(x2);
  int Y2 = xpix(y2);
  int X  = xpix(x);
  int Y  = xpix(y);

//x_copy_pixrect(draw_win,X1,Y1,X2,Y2,X,Y);

  char* prect = x_create_pixrect(draw_win,X1,Y1,X2,Y2);
  x_insert_pixrect(draw_win,X,Y,prect);
  x_delete_pixrect(prect);

  if (win_flush) flush();
 }


void BASE_WINDOW::draw_grid_lines(double xmin,double ymin, 
                                  double xmax, double ymax, 
                                  double xorig, double yorig, 
                                  double d, bool axis)
{ 
  if (xmin == xmax || ymin == ymax) return;

  if (d < 0) d = -d;

  if (xmin > xmax)
  { double t = xmin;
    xmin = xmax;
    xmax = t;
   }

  if (ymin > ymax)
  { double t = ymin;
    ymin = ymax;
    ymax = t;
   }

  int nx = int((xmax - xmin)/d) + 2;
  int ny = int((ymax - ymin)/d) + 2;

  double x0 = xorig;
  while (x0 >  xmin) x0 -= d;
  while (x0 <  xmin) x0 += d;

  double y0 = yorig;
  while (y0 >  ymin) y0 -= d;
  while (y0 <  ymin) y0 += d;

  int N = nx + ny;

  int* x1 = new int[N];
  int* y1 = new int[N];
  int* x2 = new int[N];
  int* y2 = new int[N];

  int xorig_pix = xpix(xorig);
  int yorig_pix = ypix(yorig);

  int n = 0;

  for(double i = x0; i < xmax; i += d) 
  { x1[n] = xpix(i); y1[n] = ypix(ymin);
    x2[n] = xpix(i); y2[n] = ypix(ymax);
    n++;
   }
  for(double j = y0; j < ymax; j += d)
  { x1[n] = xpix(xmin); y1[n] = ypix(j);
    x2[n] = xpix(xmax); y2[n] = ypix(j);
    n++;
   }

  int r,g,b;
  color(grey1).get_rgb(r,g,b);

/*
  // darker for xx ?
  if (display_type() == "xx") { r -= 8; g -= 8; b -= 8; }
*/

  color c(r+15,g+15,b+15);
  x_set_color(draw_win,c);
  x_lines(draw_win,n,x1,y1,x2,y2); 

  if (axis) 
  { color c = white;
    if (bg_color == white)  c = color(r-15,g-15,b-15);
    x_set_color(draw_win,c);

    if (xorig_pix > xpix(xmin) && xorig_pix < xpix(xmax))
       x_line(draw_win,xorig_pix,ypix(ymin),xorig_pix,ypix(ymax)); 

    if (yorig_pix > ypix(ymax) && yorig_pix < ypix(ymin))
       x_line(draw_win,xpix(xmin),yorig_pix,xpix(xmax),yorig_pix); 


    x_set_color(draw_win,grey3);

    if (xorig_pix > xpix(xmin) && xorig_pix < xpix(xmax)  &&
        yorig_pix > ypix(ymax) && yorig_pix < ypix(ymin))
    { x_pixel(draw_win,xorig_pix,yorig_pix);
      x_pixel(draw_win,xorig_pix-1,yorig_pix);
      x_pixel(draw_win,xorig_pix+1,yorig_pix);
      x_pixel(draw_win,xorig_pix,yorig_pix-1);
      x_pixel(draw_win,xorig_pix,yorig_pix+1);
    }
   }

  x_flush_display();

  delete[] x1;
  delete[] y1;
  delete[] x2;
  delete[] y2;
}



void BASE_WINDOW::draw_grid_points(double xmin, double ymin, 
                                   double xmax, double ymax, 
                                   double xorig, double yorig, 
                                   double d, bool axis)
{

  if (xmin == xmax || ymin == ymax) return;

  if (d < 0) d = -d;

  if (xmin > xmax)
  { double t = xmin;
    xmin = xmax;
    xmax = t;
   }

  if (ymin > ymax)
  { double t = ymin;
    ymin = ymax;
    ymax = t;
   }

  int nx = int((xmax - xmin)/d) + 1;
  int ny = int((ymax - ymin)/d) + 1;

  double x0 = xorig;
  while (x0 <= xmin) x0 += d;
  while (x0 >= xmin) x0 -= d;

  double y0 = yorig;
  while (y0 <= ymin) y0 += d;
  while (y0 >= ymin) y0 -= d;

  int xorig_pix = xpix(xorig);
  int yorig_pix = ypix(yorig);

  int N = (nx+2)*(ny+2);

  int* xc = new int[N];
  int* yc = new int[N];

  int n = 0;
  double i,j;

  for(i = x0; i < xmax; i += d)
    for(j = y0; j < ymax; j += d)
    { xc[n] = xpix(i); 
      yc[n] = ypix(j); 
      n++;
     }

  set_color(color(bg_color).text_color());

  x_pixels(draw_win,n,xc,yc);

  if (axis) 
  { n = 0;
    for(i = x0; i < xmax; i += d)
    { int xp = xpix(i); 
      if (xp > xorig_pix) { xc[n] = xp+1; yc[n] = yorig_pix; n++; }
      if (xp < xorig_pix) { xc[n] = xp-1; yc[n] = yorig_pix; n++; }
     }
  
    for(j = y0; j < ymax; j += d)
    { int yp = ypix(j); 
      if (yp > yorig_pix) { xc[n] = xorig_pix; yc[n] = yp+1; n++; }
      if (yp < yorig_pix) { xc[n] = xorig_pix; yc[n] = yp-1; n++; }
     }

    x_pixels(draw_win,n,xc,yc);
    //x_point(draw_win,xorig_pix,yorig_pix);
    x_line(draw_win,xorig_pix-2,yorig_pix-2,xorig_pix+2,yorig_pix+2);
    x_line(draw_win,xorig_pix-2,yorig_pix+2,xorig_pix+2,yorig_pix-2);
  }

  delete[] xc;
  delete[] yc;

  x_flush_display();
}



void BASE_WINDOW::draw_grid(double xmin, double ymin, double xmax, double ymax, 
                            double xorig, double yorig, double d)
{ 
  bool axis = true;

  int lw = x_set_line_width(draw_win,1);
  line_style ls = x_set_line_style(draw_win,solid);
  color clr = x_set_color(draw_win,black);

  if (g_style == point_grid)
     draw_grid_points(xmin,ymin,xmax,ymax,xorig,yorig,d,axis);

  if (g_style == line_grid)
     draw_grid_lines(xmin,ymin,xmax,ymax,xorig,yorig,d,axis);

  x_set_line_width(draw_win,lw);
  x_set_line_style(draw_win,ls);
  x_set_color(draw_win,clr);
 }


void BASE_WINDOW::draw_grid(double xmin, double ymin, double xmax, double ymax)
{ if (grid_mode != 0) 
    draw_grid(xmin,ymin,xmax,ymax,bg_xoff,bg_yoff,grid_mode); 
}


void BASE_WINDOW::draw_grid()
{ draw_grid(xmin(),ymin(),xmax(),ymax()); }


void BASE_WINDOW::draw_copyright()
{
/* 
   This function generates a copyright  message in every LEDA window 
   (and the classes derived from it). It is part of the LEDA license 
   conditions that this function is neither modified nor disabled. 
*/
  bool maximized = getenv("LEDA_OPEN_MAXIMIZED");

  if (!maximized) {
    if (window_height <= panel_height+50) return;
    if (window_width < 500 || window_height < 500) return;
  }

  //if (is_buffering()) return;

  x_save_font(draw_win);

  const char*  msg = copyright_window_string;

  int col = color(bg_color).text_color();
  //if (bg_color == white) col = grey3; // 0x737373
  if (bg_color == white) col = 0x939393;

  int save_col = x_set_color(draw_win,col);
  x_set_color(draw_win,col);

  if (maximized)
    x_set_font(draw_win,"T28");
  else
    x_set_button_font(draw_win);

  int dx = x_text_width(draw_win,msg) + 6;
  int dy = x_text_height(draw_win,msg) + 2;

  if (status_win && status_win->is_open()) 
      dy += status_win->window_height;
      
  if (scroll_bar) 
      dx += (scroll_bar->window_width + 3);

  x_text(draw_win,window_width-dx, window_height-dy, msg);

  x_restore_font(draw_win);
  x_set_color(draw_win,save_col);
}



void BASE_WINDOW::clear(double x0, double y0, double x1, double y1)
{ int X0 = xpix(x0);
  int Y0 = ypix(y1);
  int X1 = xpix(x1);
  int Y1 = ypix(y0);

  if (X0 > X1) { int tmp = X0; X0 = X1; X1 = tmp; }
  if (Y0 > Y1) { int tmp = Y0; Y0 = Y1; Y1 = tmp; }

  x_clear_window(draw_win,X0,Y0,X1,Y1,xpix(bg_xoff),ypix(bg_yoff));

  if (grid_mode != 0) 
  { x_set_clip_rectangle(draw_win,X0,Y0,X1-X0+1,Y1-Y0+2);
    draw_grid(x0,y0,x1,y1);
    x_set_clip_rectangle(draw_win,0,panel_height,window_width, 
                                            window_height-panel_height);
   }

  if (clear_redraw_ptr) {
     clear_redraw_ptr->set_params(this,NULL,0,x0,y0,x1,y1);
     clear_redraw_ptr->operator()();
  }
  else if (clear_redraw) clear_redraw(this,x0,y0,x1,y1,0);
  
  if (is_open() && show_notice 
                && (win_parent == 0 || win_parent == this)) draw_copyright();
  
  flush();
}


void BASE_WINDOW::clear(double x0, double y0, double x1, double y1, double xo,
                                                                    double yo)
{ bg_xoff = xo;
  bg_yoff = yo;
  clear(x0,y0,x1,y1);
}

void BASE_WINDOW::clear0()
{ int X0 = xoffset;
  int Y0 = yoffset+panel_height;
  int X1 = window_width-1;
  int Y1 = window_height-1;
  x_clear_window(draw_win,X0,Y0,X1,Y1,xpix(bg_xoff),ypix(bg_yoff));
  flush();
}



void BASE_WINDOW::clear()
{ del_messages();

  int X0 = xoffset;
  int Y0 = yoffset+panel_height;
  int X1 = window_width-1;
  int Y1 = window_height-1;

  int h = window_height;

/*
  if (x_test_buffer(draw_win)) 
  { Y0 = 0;
    h += (panel_height+1);
  }
*/

  x_clear_window(draw_win,X0,Y0,X1,Y1,xpix(bg_xoff),ypix(bg_yoff));

  if (grid_mode != 0) 
  { x_set_clip_rectangle(draw_win,X0,Y0,window_width, h);
    draw_grid(xreal(X0),yreal(Y1),xreal(X1),yreal(Y0));
    x_set_clip_rectangle(draw_win,0,panel_height,window_width, 
                                          window_height-panel_height);
  }

  if (clear_redraw_ptr) {
    clear_redraw_ptr->set_params(this,NULL,0,xmin(),ymin(),xmax(),ymax());
    clear_redraw_ptr->operator()();
  }
  else if (clear_redraw) clear_redraw(this,xmin(),ymin(),xmax(),ymax(),0);
  
  if (is_open() && show_notice 
                && (win_parent == 0 || win_parent == this)) draw_copyright();
  
  flush();
}


void BASE_WINDOW::clear(double xo, double yo)
{ bg_xoff = xo;
  bg_yoff = yo;
  clear();
}


void BASE_WINDOW::clear(int col)
{ color bg_col   = set_bg_color(col); 
  char* bg_prect = set_bg_pixrect(0); 
  clear();
  set_bg_color(bg_col);
  set_bg_pixrect(bg_prect);
}



static void DRAW_MESSAGE(BASE_WINDOW* wp, const char* s, int i)
{ double th = 1.2 * wp->text_height("H");
  double x0 = wp->xmin() + 0.5*th;
  double x1 = wp->xmax() - 0.5*th;
  double y  = wp->ymax() - 0.3*th;
  //wp->draw_text(x0,y+i*th*i,s); 
  wp->text_box(x0,x1,y+i*th,s);
}


void BASE_WINDOW::message(const char *s)
{ if (mesg_count >= 64) return;
  mesg_list[mesg_count] = new char[strlen(s)+1];
  strcpy(mesg_list[mesg_count],s);
  mesg_count++;
  draw_messages();
}

void BASE_WINDOW::draw_messages()
{ 
  drawing_mode save_dm  = x_set_mode(draw_win,xor_mode);
  text_mode    save_tm  = x_set_text_mode(draw_win,transparent);
  color        save_clr = x_set_color(draw_win,black);

  x_save_font(draw_win);
  x_set_text_font(draw_win);
  for(int i=0; i<mesg_count; i++) DRAW_MESSAGE(this,mesg_list[i],i);

  x_restore_font(draw_win);
  x_set_mode(draw_win,save_dm);
  x_set_text_mode(draw_win,save_tm);
  x_set_color(draw_win,save_clr);

  flush();
}


void BASE_WINDOW::del_messages()
{ if (mesg_count > 0) draw_messages();
  while(mesg_count > 0) delete[] mesg_list[--mesg_count];
 }



/*
void BASE_WINDOW::draw_text(double x, double y, const char *s, int col)
{ if (col == invisible) return;
  set_color(col);
  x_text(draw_win,xpix(x),ypix(y),s);
  if (win_flush) flush();
}
void BASE_WINDOW::draw_ctext(double x, double y, const char *s, int col)
{ if (col == invisible) return;
  set_color(col);
  x_ctext(draw_win,xpix(x),ypix(y),s);
  if (win_flush) flush();
}
*/


void BASE_WINDOW::draw_text(double x, double y, const char *s, int col)
{ if (col == invisible) return;
  set_color(col);
  double tht = pix_to_real(x_text_height(draw_win,s));
  const char* p = s;
  for(;;)
  { const char* q = p;
    while (*q && *q != '\n') q++;
    x_text(draw_win,xpix(x),ypix(y),p,int(q-p));
    y -= tht; 
    if (*q == '\0') break;
    p = q+1;
   }
  if (win_flush) flush();
}



void BASE_WINDOW::draw_ctext(double x, double y, const char *s, int col)
{ double tw1 = text_width(s);
  double th1 = text_height(s);
  draw_text(x-tw1/2,y+th1/2,s,col);
}

void BASE_WINDOW::draw_ctext(const char* s, int col)
{ double x = (xmax() - xmin())/2;
  double y = (ymax() - ymin())/2 + pix_to_real(1);
  draw_ctext(x,y,s,col); 
}



static void set_attrib(const char* s, int, char& attrib)
{ 
  char fnt = attrib & 0xf0;
  char clr = attrib & 0x0f;


  if (strncmp(s,"\\tf",3) == 0) fnt = 0x00;
  else
  if (strncmp(s,"\\rm",3) == 0) fnt = 0x00;
  else
  if (strncmp(s,"\\bf",3) == 0) fnt = 0x10;
  else
  if (strncmp(s,"\\tt",3) == 0) fnt = 0x20;
  else
  if (strncmp(s,"\\it",3) == 0) fnt = 0x30;
  else
  if (strncmp(s,"\\black", 6) == 0) clr = (char)color::index(black);
  else
  if (strncmp(s,"\\white", 6) == 0) clr = (char)color::index(white);
  else
  if (strncmp(s,"\\blue2", 6) == 0) clr = (char)color::index(blue2);
  else
  if (strncmp(s,"\\blue",  5) == 0) clr = (char)color::index(blue);
  else
  if (strncmp(s,"\\red",   4) == 0) clr = (char)color::index(red);
  else
  if (strncmp(s,"\\green2",7) == 0) clr = (char)color::index(green2);
  else
  if (strncmp(s,"\\green", 6) == 0) clr = (char)color::index(green);
  else
  if (strncmp(s,"\\yellow",7) == 0) clr = (char)color::index(yellow);
  else
  if (strncmp(s,"\\violet",7) == 0) clr = (char)color::index(violet);
  else
  if (strncmp(s,"\\orange",7) == 0) clr = (char)color::index(orange);
  else
  if (strncmp(s,"\\cyan",  5) == 0) clr = (char)color::index(cyan);
  else
  if (strncmp(s,"\\brown", 6) == 0) clr = (char)color::index(brown);
  else
  if (strncmp(s,"\\grey1", 6) == 0) clr = (char)color::index(grey1);
  else
  if (strncmp(s,"\\grey2", 6) == 0) clr = (char)color::index(grey2);
  else
  if (strncmp(s,"\\grey3", 6) == 0) clr = (char)color::index(grey3);


  attrib = fnt | clr;
}


/*
static void set_attrib(const char* s, int, int& attrib)
{ 
  int fnt = attrib & 0xff000000;
  int clr = attrib & 0x00ffffff;

  if (strncmp(s,"\\tf",3) == 0) fnt = 0x00000000;
  else if (strncmp(s,"\\rm",3) == 0) fnt = 0x00000000;
  else if (strncmp(s,"\\bf",3) == 0) fnt = 0x01000000;
  else if (strncmp(s,"\\tt",3) == 0) fnt = 0x02000000;
  else if (strncmp(s,"\\it",3) == 0) fnt = 0x03000000;

  else if (strncmp(s,"\\black", 6) == 0) clr = black;
  else if (strncmp(s,"\\white", 6) == 0) clr = white;
  else if (strncmp(s,"\\blue",  5) == 0) clr = blue;
  else if (strncmp(s,"\\blue2", 6) == 0) clr = blue2;
  else if (strncmp(s,"\\red",   4) == 0) clr = red;
  else if (strncmp(s,"\\green", 6) == 0) clr = green;
  else if (strncmp(s,"\\green2",7) == 0) clr = green2;
  else if (strncmp(s,"\\yellow",7) == 0) clr = yellow;
  else if (strncmp(s,"\\violet",7) == 0) clr = violet;
  else if (strncmp(s,"\\orange",7) == 0) clr = orange;
  else if (strncmp(s,"\\cyan",  5) == 0) clr = cyan;
  else if (strncmp(s,"\\brown", 6) == 0) clr = brown;
  else if (strncmp(s,"\\grey1", 6) == 0) clr = grey1;
  else if (strncmp(s,"\\grey2", 6) == 0) clr = grey2;
  else if (strncmp(s,"\\grey3", 6) == 0) clr = grey3;

  attrib = fnt | clr;
}
*/



int BASE_WINDOW::split_text(const char* s, int& argc, char**& argv)
{
  int wc = 1;

  //count words
  const char* x = s;
  while (*x)
  {
    while (*x && IsSpace(*x)) x++;

    const char* p=x;

    if (*p == '$' || *p == '.' /*|| *p == ','*/) 
       p++;
    else
      { if (*p == '\\') p++;
        while (*p != '\0')
        { 
          if (IsSpace(*p)) break;
          if (*p == '$' || (*p == '.' && (*p+1) == ' ') /*|| *p == ','*/) break;
          if (*p == '\\'  && *(p+1) !='~') break;
          p++;
         }
       }

    if (p > x) 
    { wc++;
      x = p;
     }
   }

  // split into words
  argc = 0;
  argv = new char*[wc];

  //int attrib = 0;
  char attrib = 0;

  while (*s)
  { 
    if (argc >= wc) 
    { fprintf(stderr,"BASE_WINDOW::error in split_text\n");
      break;
     }

    while (*s && IsSpace(*s)) s++;

    const char* p = s;

    if (*p == '$' || *p == '.' /* || *p == ',' */) 
       p++;
    else
      { if (*p == '\\') p++;
        while (*p)
        { if (IsSpace(*p)) break;
          if (*p == ' ') break;
          if (*p == '$' || (*p == '.'&& (*p+1) == ' ') /*|| *p == ','*/) break;
          if (*p == '\\'  && *(p+1) !='~') break;
          p++;
         }
       }

    if (p > s) 
    { int n = int(p-s);

      if (s[0] == '\\' && s[1] != '~')
      { // special text item ?
        if (s[1] == 'n' || isdigit(s[1]) || 
           (s[1] == 'c' && (s[2] == ' ' || isdigit(s[2]) || s[2] == '\0')))
        { 
          char* w = new char[2];
          w[0] = 0;
          w[1] = 0;
          int j = 1;
          if (s[1] == 'c')
          { w[0] = char(0x80);
            j = 2;
           }

          if ( isdigit(s[j]) ) w[0] |= ((char)atoi(s+j) & 0x7F);

          argv[argc++] = w;
         }
         else
           set_attrib(s,n,attrib);

         s = p;
         continue;
       }

      char* w = new char[n+2];
      w[0] = attrib;

/*
      char* w = new char[n+5];
      w[0] = char(attrib >> 24) && 0xff); // font
      w[1] = char(attrib >> 16) && 0xff); // red
      w[2] = char(attrib >>  8) && 0xff); // green
      w[3] = char(attrib >>  0) && 0xff); // blue
*/

      int j = 1;
      for(int i=0; i<n; i++) 
      { char c = s[i];
        if (c == '\\' && s[i+1] == '~' ) continue;
        if (c == '~'  && (i == 0 || s[i-1] != '\\')) c = ' ';
        w[j++] = c;
       }
      w[j++] = '\0';
      argv[argc++] = w;
      s = p;
     }
   }

  return argc;
}



int BASE_WINDOW::format_text(int argc, char** argv, int xmin, int xmax, 
                                                    int ymin, int dy, 
                                                    int win, bool fix_color) 
{
  // do not draw if win = 0

  int  y = ymin;
  int  i = 0;

  bool math = false;

  while (i < argc && (win==0 || y < window_height))
  { int j = i;
    int x = xmin;
    int x_off = 0;
    int w = x_text_width(draw_win,argv[i]+1);

    if (w == 0)  
       w = xmax-x;
    else
      if (i < argc-1) 
      { char c = argv[i+1][1];
        if (c != ',' && c != '.') w += 3;
        w += x_text_width(draw_win,".");
       }

    if (x+w > xmax) i++;

    while(x+w <= xmax)
    { x += w;
      if (++i >= argc) break;
      char* s = argv[i];

      if (!math)
      { char fnt = s[0] & 0xf0;

        switch (fnt) {
          case 0x10: x_set_bold_font(draw_win);
                     break;
          case 0x20: x_set_fixed_font(draw_win);
                     break;
          case 0x30: x_set_italic_font(draw_win);
                     break;
          default:   x_set_text_font(draw_win);
                     break;
         }
       }

      if (s[1] == '$') 
       { math = !math;
         if (math)
            //x_set_italic_font(draw_win);
            x_set_font(draw_win,"I12");
         else
            x_set_text_font(draw_win);
            //x_set_font(draw_win,"T12");
         w = 0;
        }
      else
       { w = x_text_width(draw_win,s+1);
         if (w==0)  // newline
         { w = xmax-x;
           if (s[0] & 0x80) // center
             x_off = w/2;
          }
        else 
          if (i < argc-1) 
          { char c = argv[i+1][1];
            if (c != ',' && c != '.') w += 3;
            w += x_text_width(draw_win,".");
           }
       }

      if (math)
      { char* p=s+1;
        while (*p && *p != '_') p++;
        if (*p == '_') w -= x_text_width(draw_win,"_"); 
       }

     }

    int d = 0;
    int r = 0;

    if (i < argc && i-j > 1)
    { d = (xmax-x)/(i-j-1);
      r = (xmax-x)%(i-j-1);
     }

 
    x = xmin + x_off;

    int dy1 = dy;

    for(int k=j; k < i; k++)
    { char* s = argv[k];
      if (s[1] == 0) // special text item 
      { dy1 += (s[0] & 0x7F);
        //dy = (s[0] & 0x7F);
        continue;
       }

      int dyfix = 0;

      if (!math)
      { char fnt = s[0] & 0xF0;

        switch (fnt) {
          case 0x10: x_set_bold_font(draw_win);
                     dyfix = 0;
                     break;
          case 0x20: x_set_fixed_font(draw_win);
                     dyfix = +4;
                     break;
          case 0x30: x_set_italic_font(draw_win);
                     dyfix = 0;
                     break;
          default:   x_set_text_font(draw_win);
                     dyfix = 0;
                     break;
         }
       }

      if (s[1] == '$') 
      { math = !math;
        if (math)
          //x_set_italic_font(draw_win);
          x_set_font(draw_win,"I12");
        else
          x_set_text_font(draw_win);
          //x_set_font(draw_win,"T12");
        continue;
       }

      int w = x_text_width(draw_win,s+1);
      w += x_text_width(draw_win,".");

      if (win && !fix_color) 
      { //int clr = s[0] & 0x0F;
        int clr = color::get(s[0] & 0x0f);
         if (clr == blue)
           x_set_color(win,color(0,0,150));
         else
           x_set_color(win,color(clr));
       }

      char* p=s+1;
      while (*p && *p != '_') p++;

      if (win) 
      { if (math && *p == '_') 
        { *p = '\0';
          x_text(win,x,y,s+1); 
          int y_sub = y + x_text_height(draw_win,s+1)/3;
          x_text(win,x+x_text_width(win,s+1),y_sub,p+1); 
          *p = '_';
          w -= x_text_width(draw_win,"_"); 
         }
        else
          x_text(win,x,y+dyfix,s+1); 
      }

      x += w;

      if (k < i-1) 
      { char c = argv[k+1][1];
        if (c != ',' && c != '.')
        { //x += x_text_width(draw_win,".");
          x += 3;
          x += d;
          if (r-- > 0) x++;
         }
       }
     }
    y += dy1;
   }

  x_set_text_font(draw_win);
  x_set_color(draw_win,black);

  return y;
}



double BASE_WINDOW::text_box(double x0, double x1, double y1, const char* s, bool draw)
{ char** argv;
  int    argc;
  int    win = draw ? draw_win : 0;
  x_set_text_font(draw_win);
  split_text(s,argc,argv);
  int dy = x_text_height(draw_win,s);
  int y0 = format_text(argc, argv, xpix(x0),xpix(x1),ypix(y1),dy,win);
  for(int i=0; i<argc; i++) delete[] argv[i];
  delete[] argv;
  return yreal(y0);
}
  




void BASE_WINDOW::cursor()
{ if (show_grid_cursor && this == read_window)
  { x_set_read_gc(draw_win);
    int X = xpix(mouse_xreal);
    int Y = ypix(mouse_yreal);
    x_line(draw_win, X,Y,X+8,Y);
    x_line(draw_win, X,Y,X-8,Y);
    x_line(draw_win, X,Y,X,Y+8);
    x_line(draw_win, X,Y,X,Y-8);
    x_reset_gc(draw_win);
   }
}

// define static members

void BASE_WINDOW::do_not_open_display(bool b)
{ x_do_not_open_display(b); }

char* BASE_WINDOW::root_pixrect(int x0, int y0, int x1, int y1)
{ return x_root_pixrect(x0,y0,x1,y1); }



BASE_WINDOW* BASE_WINDOW::active_window = 0;
BASE_WINDOW* BASE_WINDOW::read_window = 0;
int          BASE_WINDOW::win_count = 0;

panel_item   BASE_WINDOW::active_button = 0;
panel_item   BASE_WINDOW::last_active_button = 0;
BASE_WINDOW* BASE_WINDOW::active_button_win = 0;

/*
panel_item   BASE_WINDOW::help_last_item = 0;
*/

int  BASE_WINDOW::screen_width()
{ int w,h,dpi;
  x_display_info(w,h,dpi);
  return w;
}

int  BASE_WINDOW::screen_height()
{ int w,h,dpi;
  x_display_info(w,h,dpi);
  return h;
}

int  BASE_WINDOW::screen_dpi()
{ int w,h,dpi;
  x_display_info(w,h,dpi);
  return dpi;
}

string BASE_WINDOW::display_type()
{ int w,h,dpi;
  return x_display_info(w,h,dpi);
}

void BASE_WINDOW::upload(string fname)   { x_send_text("upload: " + fname); }
void BASE_WINDOW::download(string fname) { x_send_text("download: " + fname); }
void BASE_WINDOW::keyboard(int x) { x_send_text(string("keyboard: %d",x)); }



void BASE_WINDOW::default_size(int& w, int& h)
{
  int screen_w = screen_width();
  int screen_h = screen_height();

  if (getenv("LEDA_OPEN_MAXIMIZED")) {
    w = screen_w+1;
    h = screen_h+1;
    return;
  }

  int max_h = int(0.85*screen_h);
  int max_w = int(0.97*screen_w);

  h = int(7.75 * screen_dpi());
  if (h > max_h) h = max_h;

  w = int(0.87*h);
  if (w > max_w) w = max_w;
}


int BASE_WINDOW::default_width()
{ int w,h;
  default_size(w,h);
  return w;
}

int BASE_WINDOW::default_height()
{ int w,h;
  default_size(w,h);
  return h;
}



void BASE_WINDOW::mouse_default_action(double,double) 
{ /* do nothing */}

void BASE_WINDOW::mouse_segment_action(double x, double y) 
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  x_set_read_gc(read_window->draw_win);
  read_window->draw_segment(x0,y0,x,y,black); 
  x_reset_gc(read_window->draw_win);
 }

void BASE_WINDOW::mouse_line_action(double x, double y) 
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  int dw = read_window->draw_win;
  int X = read_window->xpix(x0);
  int Y = read_window->ypix(y0);
  x_set_read_gc(read_window->draw_win);
  read_window->draw_line(x0,y0,x,y,black); 
  int csave = x_set_color(dw,black);
  x_set_color(dw,black);
  x_ellipse(dw,X,Y,2,2);
  x_set_color(dw,csave);
  x_reset_gc(read_window->draw_win);
}

void BASE_WINDOW::mouse_ray_action(double x, double y) 
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  x_set_read_gc(read_window->draw_win);
  read_window->draw_ray(x0,y0,x,y,black); 
  x_reset_gc(read_window->draw_win);
}


void BASE_WINDOW::mouse_rect_action(double x, double y)
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  x_set_read_gc(read_window->draw_win);
  read_window->draw_rectangle(x0,y0,x,y,black);
  x_reset_gc(read_window->draw_win);
 }

void BASE_WINDOW::mouse_circle_action(double x, double y)
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  int dw = read_window->draw_win;
  int X = read_window->xpix(x0);
  int Y = read_window->ypix(y0);
  x_set_read_gc(read_window->draw_win);
  read_window->draw_circle(x0,y0,HyPot(x-x0,y-y0),black);
  int csave = x_set_color(dw,black);
  //x_point(dw,X,Y);
  x_line(dw,X-2,Y-2,X+2,Y+2);
  x_line(dw,X-2,Y+2,X+2,Y-2);
  x_set_color(dw,csave);
  x_reset_gc(read_window->draw_win);
 }

void BASE_WINDOW::mouse_arc_action(double x, double y)
{ double x0 = read_window->mouse_start_xreal;
  double y0 = read_window->mouse_start_yreal;
  double x1 = read_window->mouse_additional_xreal;
  double y1 = read_window->mouse_additional_yreal;

  double cx =   (x0*x0 + y0*y0)*(y1 - y )
	          + (x1*x1 + y1*y1)*(y  - y0)
              + (x *x  + y *y )*(y0 - y1);
 
  double cy = - (x0*x0 + y0*y0)*(x1 - x )
              - (x1*x1 + y1*y1)*(x  - x0)
              - (x *x  + y *y )*(x0 - x1);

  double cw = 2*( (x1 - x0)*(y  - y0) - (x  - x0)*(y1 - y0) );

  int dw = read_window->draw_win;
  x_set_read_gc(dw);
  if (cw != 0) {
    cx /= cw; cy /= cw;
	double r = sqrt( (x-cx)*(x-cx) + (y-cy)*(y-cy) );
	double start = compute_angle(cx, cy, cx+1, cy, x0, y0);
	double span  = compute_angle(cx, cy, x0, y0, x1, y1);
	if (cw > 0) span -= 2*LEDA_PI;
	read_window->draw_arc(cx,cy,r,r,start,span,black);
  }
  else read_window->draw_segment(x0,y0,x1,y1,black);
  x_reset_gc(dw);
 }

// copied from _point.c
double BASE_WINDOW::compute_angle(double px, double py, double qx, double qy, double rx, double ry)
{
  double dx1 = qx - px; 
  double dy1 = qy - py; 
  double dx2 = rx - px; 
  double dy2 = ry - py; 

  double norm  = (dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2);

  double cosfi = (dx1*dx2+dy1*dy2) / sqrt(norm);

  if (cosfi >=  1.0 ) return 0;
  if (cosfi <= -1.0 ) return LEDA_PI;
  
  double fi = acos(cosfi);

  if (dx1*dy2 < dy1*dx2) fi = -fi;

  if (fi < 0) fi += 2*LEDA_PI;

  return fi;
}

/*
int BASE_WINDOW::text_color(color clr)
{ int r,g,b;
  clr.get_rgb(r,g,b);
  if (r+g+b < 385)
     return white;
  else
     return black;
}
*/


void BASE_WINDOW::start_buffering() 
{ 
  assert(buf_level >= 0);
//if (!is_open() || is_minimized()) return;

  if (buf_level++ > 0)  {
    assert(is_buffering());
    return; 
  }

  if (x_start_buffering(draw_win)) clear();

  window_width_save = window_width;
  window_height_save = window_height;
}


void BASE_WINDOW::start_buffering(int w, int h) 
{ 
  assert(buf_level==0);
//if (!is_open() || is_minimized()) return;

  if (window_height > panel_height) h += panel_height;

  if (x_start_buffering(draw_win,w,h) == -1)
  { LEDA_EXCEPTION(1,"window: start_buffering failed.");
    return;
   }
   
  buf_level = 1;

  window_width_save = window_width;
  window_height_save = window_height;
  window_width  = w;
  window_height = h;

  // extend clipping to w x h buffer
  clipping(0);
}



void BASE_WINDOW::stop_buffering()  
{ 
  assert(buf_level > 0);

  if (--buf_level != 0) return;

  x_stop_buffering(draw_win); 

//window_width  = x_window_width(draw_win);
//window_height = x_window_height(draw_win);

  window_width  = window_width_save;
  window_height = window_height_save;

  window_width_save  = 0;
  window_height_save = 0;
}


void BASE_WINDOW::stop_buffering(char*& pr)  
{ 
  assert(buf_level > 0);

  if (--buf_level != 0) return;

  x_stop_buffering(draw_win,&pr); 

//window_width  = x_window_width(draw_win);
//window_height = x_window_height(draw_win);

  window_width  = window_width_save;
  window_height = window_height_save;
  window_width_save = 0;
  window_height_save = 0;
}


void BASE_WINDOW::flush_pixels(int x0, int y0, int x1, int y1)
{ assert((buf_level > 0) == is_buffering());
  if (buf_level > 1) return; 
  clipping(0);
  x_flush_buffer(draw_win,x0,y0,x1,y1);
}


void BASE_WINDOW::flush_buffer(double x0, double y0, double x1, double y1)
{ assert((buf_level > 0) == is_buffering());

  if (buf_level > 1) return;

  int X0 = xpix(x0);
  int Y0 = ypix(y1);
  int X1 = xpix(x1);
  int Y1 = ypix(y0);
  if (Y0 < panel_height) Y0 = panel_height;

  x_flush_buffer(draw_win,X0,Y0,X1,Y1);
}


void BASE_WINDOW::flush_buffer() 
{ 
  assert((buf_level > 0) == is_buffering());

/*
  // this should not happen
  if (buf_level == 0) {
    cerr << "FLUSH_BUFFER: level == 0" << endl;
    return;
  }

  assert(buf_level > 0);
*/


  if (buf_level > 1) return;

  int x0 = 0;
  int y0 = panel_height;
  int x1 = window_width-1;
  int y1 = window_height-1;

  x_flush_buffer(draw_win,x0,y0,x1,y1,xoffset,yoffset);
}

void BASE_WINDOW::flush_buffer(double dx, double dy) 
{ assert((buf_level > 0) == is_buffering());
  if (buf_level > 1) return;

  int x0 = 0;
  int y0 = panel_height;
  int x1 = window_width-1;
  int y1 = window_height-1;

  x_flush_buffer(draw_win, x0, y0, x1, y1, xoffset+real_to_pix(dx),
                                           yoffset+real_to_pix(dy)); 
}

void BASE_WINDOW::flush_buffer(double dx, double dy, 
                               double x0, double y0, double x1, double y1)
{ assert((buf_level > 0) == is_buffering());
  if (buf_level > 1) return;
  x_flush_buffer(draw_win,xpix(x0),ypix(y0),xpix(x1),ypix(y1),real_to_pix(dx),
                                                              real_to_pix(dy)); 
 }

void BASE_WINDOW::set_buffer(char* pr) { x_set_buffer(draw_win,pr); }

bool BASE_WINDOW::is_buffering()  { return x_test_buffer(draw_win) != 0; }

void BASE_WINDOW::delete_buffer() { x_delete_buffer(draw_win); }  


// static members


void BASE_WINDOW::default_coord_handler(BASE_WINDOW* win, double x, double y)
{ 
  char s[128];
  sprintf(s,"     %6.1f %6.1f",x,y);

  x_save_font(win->draw_win);
  x_set_fixed_font(win->draw_win);

  text_mode save_tm = x_set_text_mode(win->draw_win,opaque);
  drawing_mode save_dm = x_set_mode(win->draw_win,src_mode);

  int save_col = x_set_color(win->draw_win, black);
  x_set_color(win->draw_win, black);

  int tw = x_text_width(win->draw_win,s);
  int th = x_text_height(win->draw_win,s);

  int xpos = win->window_width - tw - 10; 
  int ypos = win->panel_height + 5;


  x_start_buffering(win->draw_win);
  x_text(win->draw_win,xpos,ypos,s);
  x_flush_buffer(win->draw_win,xpos,ypos,xpos+tw,ypos+th);
  x_stop_buffering(win->draw_win);

  x_restore_font(win->draw_win);

  x_set_text_mode(win->draw_win,save_tm);
  x_set_mode(win->draw_win,save_dm);
  x_set_color(win->draw_win, save_col);
}


int BASE_WINDOW::read_event(BASE_WINDOW*& wp, int& k, double& x, double& y)
{ read_window = 0;
  int e = BASE_WINDOW::event_handler(wp,0);
  if (wp)
  { x = wp->mouse_xreal;
    y = wp->mouse_yreal;
    k = wp->mouse_key;
   }
  return e;
 }

int BASE_WINDOW::read_event(BASE_WINDOW*& wp, int& k, double& x, 
                                                      double& y, 
                                                      unsigned long& t,
                                                      int timeout)
{ read_window = 0;
  int e = BASE_WINDOW::event_handler(wp,timeout);
  if (wp) 
  { x = wp->mouse_xreal;
    y = wp->mouse_yreal;
    k = wp->mouse_key;
    t = wp->event_time;
    return e;
   }

  return no_event;
 }



int BASE_WINDOW::get_event(BASE_WINDOW*& wp, int& k, double& x, double& y)
{ read_window = 0;
  int e = BASE_WINDOW::event_handler(wp,-1);
  if (wp)
  { x = wp->mouse_xreal;
    y = wp->mouse_yreal;
    k = wp->mouse_key;
   }
  return e;
 }


void BASE_WINDOW::put_back_event() { x_put_back_event(); }


int BASE_WINDOW::read_mouse(BASE_WINDOW*& wp, double& x, double& y)
{
  read_window = 0;

  int e = no_event;
  while (e != button_press_event && e != key_press_event) {
    e = BASE_WINDOW::event_handler(wp,0);
  }

  x = wp->mouse_xreal;
  y = wp->mouse_yreal;
  return wp->mouse_key;
}


int BASE_WINDOW::get_mouse(BASE_WINDOW*& wp, double& x, double& y)
{ 
  int but = NO_BUTTON;

  if (BASE_WINDOW::event_handler(wp,-1) == button_press_event) {
    but = wp->mouse_key;
    x = wp->mouse_xreal;
    y = wp->mouse_yreal;
  }

  return but;
}


void BASE_WINDOW::grab_mouse()   { x_grab_pointer(draw_win); }

void BASE_WINDOW::ungrab_mouse() { x_grab_pointer(0); }


void* BASE_WINDOW::get_inf(int i) const { return data[i]; }

void* BASE_WINDOW::set_inf(void* x, int i) 
{ void* tmp = data[i]; 
  data[i] = x; 
  return tmp; 
}


void BASE_WINDOW::set_icon_pixrect(char* pr) { 
   if (icon_pixrect) x_delete_pixrect(icon_pixrect);
   icon_pixrect = pr;
   if (is_open()) x_set_icon_pixmap(draw_win,pr); 
}

void BASE_WINDOW::reset_clipping() 
{ x_set_clip_rectangle(draw_win,0,panel_height,window_width,
                                          window_height-panel_height);
}


void BASE_WINDOW::set_clip_rectangle(double x0, double y0, double x1, double y1)
{ int X0 = xpix(x0);
  int X1 = xpix(x1);
  int Y0 = ypix(y0);
  int Y1 = ypix(y1);
  if (Y1 < panel_height) Y1 = panel_height;
  x_set_clip_rectangle(draw_win,X0,Y1,X1-X0+1,Y0-Y1+1);
}


/*
void BASE_WINDOW::set_clip_ellipse(double x0, double y0, double r1, double r2)
{ int R1 = real_to_pix(r1);
  int R2 = real_to_pix(r2);
  x_set_clip_ellipse(draw_win,xpix(x0),ypix(y0),R1,R2);
}

void BASE_WINDOW::set_clip_polygon(int n, double *xc, double *yc)
{
 int* x = new int[n+1];
 int* y = new int[n+1];
 int i;

 for(i=0;i<n;i++)
 { x[i] = xpix(xc[i]);
   y[i] = ypix(yc[i]);
  }

 x[n] = x[0];
 y[n] = y[0];

 x_set_clip_polygon(draw_win,n+1,x,y);

 delete[] x;
 delete[] y;
}
*/

// clip masks

void BASE_WINDOW::clip_mask_window(int c) { 
  x_clip_mask_rectangle(draw_win,0,0,window_width-1,window_height-1,c); 
}


void BASE_WINDOW::clip_mask_polygon(int n, double* xc, double* yc, int c)
{ 
 int* x = new int[n+1];
 int* y = new int[n+1];

 for(int i=0;i<n;i++)
 { x[i] = xpix(xc[i]);
   y[i] = ypix(yc[i]);
  }

 x[n] = x[0];
 y[n] = y[0];

 x_clip_mask_polygon(draw_win,n,x,y,c);

 delete[] x;
 delete[] y;
}



void BASE_WINDOW::clip_mask_rectangle(double x0, double y0, double x1, 
                                                            double y1, int c)
{ 
  int X0 = xpix(x0);
  int Y0 = ypix(y0);
  int X1 = xpix(x1);
  int Y1 = ypix(y1);
  if (X0 > X1) { int tmp = X0; X0 = X1; X1 = tmp; }
  if (Y0 > Y1) { int tmp = Y0; Y0 = Y1; Y1 = tmp; }

  x_clip_mask_rectangle(draw_win,X0,Y0,X1,Y1,c);
}


void BASE_WINDOW::clip_mask_ellipse(double x,double y,double r1,double r2,int c)
{ int R1 = real_to_pix(r1);
  int R2 = real_to_pix(r2);
  x_clip_mask_ellipse(draw_win,xpix(x),ypix(y),R1,R2,c); 
}


void BASE_WINDOW::clip_mask_chord(double x0,double y0, double x1,double y1, double cx,double cy, double r, int c)
{ int R = real_to_pix(r);
  x_clip_mask_chord(draw_win, xpix(x0),ypix(y0), xpix(x1),ypix(y1), xpix(cx),ypix(cy), R, c); 
}




// status window

//static
void BASE_WINDOW::status_redraw(BASE_WINDOW* wp) 
{ 
  BASE_WINDOW* win = (BASE_WINDOW*)wp->get_inf();
  char* str = win->status_str;

  if (!wp->is_open() || str == 0) return;

  bool fstring = false;
  for(unsigned int i=0; i<strlen(str); i++)
       if (str[i] == '\\') fstring = true;

  double pix = wp->pix_to_real(1);

  double x0 = wp->xmin() + 5*pix;
  double x1 = wp->xmax() - 5*pix;
  double y0 = wp->ymin();
  double y1 = wp->ymax();


  wp->set_fixed_font();
  wp->clear();

  if (fstring) 
     wp->text_box(x0,x1,y1-2*pix,str);
  else
   { double th = wp->text_height(str);
     double y = (y1+y0+th)/2;
     wp->draw_text(x0,y,str);
    }

/*
  wp->set_color(grey3);
  wp->set_line_width(2);
  x_line(wp->draw_win,0,0,wp->width(),0);
*/

  // status_win is buffering !
  wp->flush_buffer();
}



BASE_WINDOW* BASE_WINDOW::create_status_window(int h, color col)
{ if (status_win == 0)
  { //status_win = new BASE_WINDOW(window_width-2,h,"");
    status_win = new BASE_WINDOW(window_width-1,h,"");
    status_win->set_inf(this);
    status_win->set_redraw(status_redraw);
    status_win->set_bg_color(col);
    status_win->set_border_color(grey3);
   }
  return status_win;
}


BASE_WINDOW* BASE_WINDOW::open_status_window(int h, color col)
{ 
  if (h == 0) 
  { set_fixed_font();
    h = real_to_pix(1.45*text_height("H"));
    set_text_font();
   }

  create_status_window(h,col);

  if (!status_win->is_open())
  { status_win->display(0,window_height-h,this);
    // status windows are always buffering
    status_win->start_buffering();
    status_redraw(status_win);
   }
  return status_win;
}


void BASE_WINDOW::close_status_window()
{ if (status_win && status_win->is_open())
  { status_win->close(); 
    status_win->stop_buffering();
   }
}


void BASE_WINDOW::destroy_status_window()
{ if (status_win) 
  { delete status_win;
    status_win = 0;
   }
}

void BASE_WINDOW::set_status_string(const char* str)
{ if (status_str) delete[] status_str;
  if (str == 0)  str = "";
  status_str = new char[strlen(str) +1]; 
  strcpy(status_str,str);
  if (status_win) status_redraw(status_win);
}



void BASE_WINDOW::draw_text_cursor(double x, double y, int col)
{ if (col == invisible) return;
  int xc = xpix(x);
  int yc = ypix(y) + 1;
  int X[3];
  int Y[3];
  int d = x_text_height(draw_win,"H")/4 + 2;
  X[0] = xc-d+1;
  Y[0] = yc;
  X[1] = xc+d-1;
  Y[1] = yc;
  X[2] = xc;
  Y[2] = yc-d;
  set_color(col);
  x_fill_polygon(draw_win,3,X,Y);
  if (win_flush) flush();
}



void BASE_WINDOW::set_focus()
{ x_set_focus(draw_win); }


void BASE_WINDOW::string_edit(double x , double y, void* s)
{ 
  int string_h0 = string_h;
  string_h = string_h+6;

  int string_w0 = string_w;
  string_w = 3*string_w/4;

  double x1 = x - pix_to_real(string_w/2+2);
  double y1 = y - pix_to_real(string_h/2);
  double x2 = x + pix_to_real(string_w/2+2);
  double y2 = y + pix_to_real(string_h/2);
  char* pm =  get_pixrect(x1,y1,x2,y2);

  panel_item it = string_item0(x1,y2,s);

  active_item = it;

  draw_string_item(it,0);
  while (panel_text_edit(it))
  { assign_str(it->ref,it->data_str);
    it->data_str = access_str(it->ref);
   }
  put_pixrect(x1,y1,pm);
  del_pixrect(pm);
  active_item = 0;
  delete it;
  string_h = string_h0;
  string_w = string_w0;
}
  


void BASE_WINDOW::acknowledge(const char* s1, const char* s2)
{ BASE_WINDOW P(-1,-1,s1);
  P.text_item("\\bf\\blue");
  P.text_item(s1);
  P.text_item("");
  if (strlen(s2) > 0)
  { P.text_item("\\tt");
    P.text_item(s2);
   }
  P.button("ok",0,panel_action_func(0));
  P.panel_open(BASE_WINDOW::center,BASE_WINDOW::center,this);
}



void BASE_WINDOW::set_special_event_handler(
                  void (*func)(void*,const char*,const char*,int,int),
                  unsigned long data)
{ x_set_special_event_handler(draw_win,func,data); }



void BASE_WINDOW::set_d3_view_point(double px, double py, double pz, 
                                    double nx, double ny, double nz)
{ d3_view_pos_x = px;
  d3_view_pos_y = py;
  d3_view_pos_z = pz;
  d3_view_norm_x = nx;
  d3_view_norm_y = ny;
  d3_view_norm_z = nz;
}

void BASE_WINDOW::get_d3_view_point(double& px, double& py, double& pz, 
                                    double& nx, double& ny, double& nz)
{ px = d3_view_pos_x;
  py = d3_view_pos_y;
  pz = d3_view_pos_z;
  nx = d3_view_norm_x;
  ny = d3_view_norm_y;
  nz = d3_view_norm_z;
}


void BASE_WINDOW::project_d3_point(double& x, double& y, double& z)
{
  //q = normal_project(p)

  double px = d3_view_pos_x;
  double py = d3_view_pos_y;
  double pz = d3_view_pos_z;
  double nx = d3_view_norm_x;
  double ny = d3_view_norm_y;
  double nz = d3_view_norm_z;

  // v = p - point1
  double vx = x - px;
  double vy = y - py;
  double vz = z - pz;

  double W = nx*nx + ny*ny + nz*nz;
  double A = (nx*vx + ny*vy + nz*vz)/W;

  double qx = x-A*nx;
  double qy = y-A*ny;
  double qz = z-A*nz;

  x = qx;
  y = qy;
  z = qz;
}


void BASE_WINDOW::set_tooltip(int id, double x0, double y0, 
                                      double x1, double y1, const char* text)
{ 
  x_set_tooltip(draw_win,id,xpix(x0),ypix(y1),xpix(x1),ypix(y0),text);
}


void BASE_WINDOW::del_tooltip(int id)
{ 
#if defined(__win32__)
  x_del_tooltip(draw_win,id);
#endif
}

void BASE_WINDOW::choose_color(int c)
{ 
#if defined(__win32__)
  x_choose_color(draw_win,c);
#endif
}



void BASE_WINDOW::set_alpha(int a)
{
#if defined(__win32__)
  x_set_alpha(draw_win,a);
#endif
}


LEDA_END_NAMESPACE
