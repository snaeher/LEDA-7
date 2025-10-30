/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _base_panel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/base_window.h>
#include <LEDA/system/assert.h>

#if defined(_MSC_VER)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#endif

#include "bitmaps/pstyle.h"

#include <LEDA/core/string.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>


// static members

BASE_WINDOW* BASE_WINDOW::call_window = 0;
panel_item   BASE_WINDOW::call_item = 0;


BASE_WINDOW* BASE_WINDOW::get_call_window() 
{ return BASE_WINDOW::call_window; }

panel_item BASE_WINDOW::get_call_item()   
{ return BASE_WINDOW::call_item;   }

int BASE_WINDOW::get_call_button() 
{ panel_item it = BASE_WINDOW::call_item;  
  return (it && it->kind == Button_Item) ? it->dat1 : -1;
}



void BASE_WINDOW::scroll_down_action(int)
{ ((BASE_WINDOW*)call_window->get_inf())->scroll(-1); }

void BASE_WINDOW::scroll_up_action(int)
{ ((BASE_WINDOW*)call_window->get_inf())->scroll(+1); }


void BASE_WINDOW::scroll_drag_action(int i)
{ 
  BASE_WINDOW* wp = (BASE_WINDOW*)call_window->get_inf();
  char* scroll_buf;

  wp->clipping(0);

  int num_items = wp->item_count;

  if (display_type() != "mswin") {
    if (num_items > 500) num_items = 500;
  }

  int menu_h = num_items * wp->yskip;

  int pan_h  = wp->panel_height;
  int pan_w  = wp->panel_width;

  panel_item p0 = wp->Item[0];

  if (i == -1) // start
  { // init buffer
   int y0 = p0->ycoord;
   int j;
   for(j=0; j<wp->item_count; j++) wp->Item[j]->ycoord = j*wp->yskip;

   wp->start_buffering(pan_w,menu_h+pan_h);

   wp->panel_height = menu_h;
   wp->clear();
   wp->draw_panel();
   wp->stop_buffering(scroll_buf);
   wp->panel_height  = pan_h;
   wp->window_height = pan_h;
   for(j=0; j<wp->item_count; j++) wp->Item[j]->ycoord = y0 + j*wp->yskip;
   wp->set_inf(scroll_buf);
   return;
  }

  scroll_buf= (char*)wp->get_inf();

  if (i == -2) // finish
  { // round to next button position and delete buffer
    int y0 = p0->ycoord;
    int dy = y0 % wp->yskip;
    int y1 = y0 - dy;
    int d  = 1;

    if (dy < -wp->yskip/2)
    { y1 = y0 - (wp->yskip + dy);
      d = -1;
     }

    while (y0 != y1+d)
    { x_insert_pixrect(wp->draw_win,0,menu_h+y0+pan_h-1,scroll_buf);
      y0 += d;
     }

    y0 -= d;

    x_delete_pixrect(scroll_buf);
    for(int j=0; j < wp->item_count; j++) 
       wp->Item[j]->ycoord = y0 + j*wp->yskip;
    return;
   }

  // drag

  double f = i/1000.0; 
  p0->ycoord = -int(f*(menu_h-pan_h));

  int yy = menu_h + p0->ycoord + pan_h - 1;
  x_insert_pixrect(wp->draw_win,0,yy,scroll_buf);
}



//------------------------------------------------------------------------------
// some auxiliary functions for string manipulation
//------------------------------------------------------------------------------

inline int str_length(const char* p) { return (int)strlen(p); }


static char* string_dup(const char* p)
{ if (p == 0) return 0;
  char* q = new char[str_length(p)+1];
  if (q==0) 
  { fprintf(stderr,"string_dup: out of memory");
    abort();
   }
  strcpy(q,p);
  return q;
}

static char* make_string(int x)
{ char str[256];
  sprintf(str,"%d",x);
  return string_dup(str);
 }

static char* make_string(double x, string format)
{ char str[256];
  sprintf(str,format,x);
  return string_dup(str);
 }


//------------------------------------------------------------------------------
// construction and destruction of panel items
//------------------------------------------------------------------------------


Panel_Item::Panel_Item(int k, char* s, void* addr, int h, int i)
{ kind = k;
  enabled = true;
  secret = false;
  label_str = s;
  label_clr = black;
  text_clr = black;

  label_width = -1;
  button_width = -1;

  item_width = 0;
  width = 0;

  ref = addr;
  height  = h;
  index = i;
  data_str = 0;
  help_str = 0;
  xcoord = 0;
  ycoord = 0;

  dat1 = 0;
  dat2 = 0;
  offset = 0;
  argc = 0;
  argv = 0;

  menu_bar = 0;
  
  action = 0;
  str_action = 0;
  release_action = 0;
  
  action_ptr = 0;
  str_action_ptr = 0;

  str_selected = 0;
  
  menu_win = 0;
  menu_but = 0;
  shortcut = -1;
}


Panel_Item::~Panel_Item()
{ 
  if (label_str) delete[] label_str;
  if (help_str)  delete[] help_str;

  if (kind != String_Item && kind != String_Menu_Item && data_str) 
    delete[] data_str;

  if (argc > 0 || argc == -1 /*pmap*/)
  { for(int i = 0; i < argc; i++)
    { if (kind == Button_Item || 
          kind == Bitmap_Choice_Item || kind == Bitmap_Choice_Mult_Item) 
         x_delete_bitmap(argv[i]);
      else
         delete[] argv[i];
     }
    delete[] argv;
   }

  if (kind == Button_Item && ref) 
  { ((BASE_WINDOW*)ref)->owner_item = 0;
    //if (menu_win) delete menu_win;
   }

  if (menu_win) delete menu_win;

}



//------------------------------------------------------------------------------
// panel member functions
//------------------------------------------------------------------------------

void BASE_WINDOW::set_tooltip(panel_item it)
{
  if (it->help_str == 0 || str_length(it->help_str) == 0) return;

  if (it->help_str && it->help_str[0] != '#')
  { int x1 = it->xcoord;
    int y1 = it->ycoord;
    int x2 = x1 + it->width;
    int y2 = y1 + it->height;
    x_set_tooltip(draw_win,it->index,x1,y1,x2,y2,it->help_str);
    return;
   }

  char* hstr = string_dup(it->help_str);
  char* p = hstr+1;
  char* H[256];

  int n = 0;

  while (*p != '\0')
  { H[n++] = p++;
    while (*p != '#' && *p != '\0') p++;
    if (*p == '#') *p++ = '\0';
   }

  if (n > it->argc) n = it->argc;

  int lw = get_item_label_width(it);

  if (it->kind == Choice_Item || it->kind == Choice_Mult_Item)
  {
    int x0 = it->xcoord + lw;
    int y0 = it->ycoord;
    int y1 = y0 + choice_h;
    //int n = it->argc;
  
    for(int j=0; j<n; j++)
    { 
      int x1 = x0 + get_item_button_width(it)-1;
      //int x1 = x0 + choice_w-1;

      x_set_tooltip(draw_win, 100*it->index + j, x0, y0, x1, y1,H[j]);

      x0 += get_item_button_width(it);
      //x0 += choice_w;
     }
   }
 
  if (it->kind == Bitmap_Choice_Item || it->kind == Bitmap_Choice_Mult_Item)
  {
    //int n = it->argc;
    int w = it->dat1;
    int h = it->dat2;
    int x0 = it->xcoord + lw;
    int y0 = it->ycoord;
    int y1 = it->ycoord + h;
 
    for(int j=0; j<n; j++) 
    { int x1 = x0 + w + 1;
      x_set_tooltip(draw_win, 100*it->index + j, x0, y0, x1, y1, H[j]);
      x0 += (w+3);
     }
   }
 
  delete[] hstr;
}


void BASE_WINDOW::panel_init()
{ 
  if (display_fd() == -1)  return;

  x_save_font(draw_win);

  panel_float_format = "%f";

  auto_button_layout = true;

  item_count = 0;
  but_count = 0;

  float scaling = float(screen_dpi()/96.0);

  //panel_line_width = 1;
  panel_line_width = int(0.5 + scaling);


  item_bg_color = white;
  bitmap_color0 = black;
  bitmap_color1 = black;

  panel_bg_color = grey1;
  press_color    = grey2;
  shadow_color   = grey3;
//disable_color  = grey2;
  disable_color  = color(150,150,150);

  //min_panel_width = int(450*screen_dpi_scaling());
  min_panel_width = int(550*screen_dpi_scaling());
  panel_width = 0;
  panel_height = 0;

  x_set_button_font(draw_win);
  tw = x_text_width(draw_win,"H");
  th = x_text_height(draw_win,"H");

  ytskip   = int(1.05*th);
  yskip    = int(1.6*th);

  yoff     = int(0.80*th);
  xoff     = int(1.50*tw);

  slider_h = int(0.80*th);
  color_h  = int(0.90*th);
  string_h = int(1.05*th);
  button_h = int(1.20*th);

  choice_h = button_h;

  button_d = th/2;

  label_w   = 10;          // minimal label length
  sl_num_w  = 4*tw;        // slider num field width
  slider_w  = 16*color_h;  // minimal slider length
  string_w  = slider_w;    // minimal string item length
  number_w  = string_w;    // minimal int/float item length
  choice_w  = 30;          // minimal choice field width
  button_w  = 20;          // minimal button width

/*
  scrollbar_w = int(0.95*th);
*/


  //buts_per_line = 0;  
  buts_per_line = 3;  

  center_button_label = 1;
  menu_style = 1;   // -1: scrollbar  (0,1,2: ....)
  menu_size = 0;

  active_item = 0;
  last_sel_button = 0;
  last_sel_item = 0;
  menu_mode = 0;
  menu_bar_mode = 0;
  menu_bar_height = 0;
  focus_button = 0;

  x_set_fixed_font(draw_win);
  twf = x_text_width(draw_win,"HHHHHHHHHHHHHHHHHHHH")/20.0;
  tw = x_text_width(draw_win,"H");
  th = x_text_height(draw_win,"H");
  x_restore_font(draw_win);

  scrollbar_w = int(0.85*th);

  panel_items_fixed = false;

}


void BASE_WINDOW::set_item_height(int h)  { yskip = h; }

void BASE_WINDOW::set_button_height(int h)  { button_h = h; }

void BASE_WINDOW::set_item_width(int w)
{ slider_w = w;
  string_w = w;
  number_w = w;
 }

void BASE_WINDOW::set_item_margins(int x, int y)   { xoff = x; yoff = y; }

void BASE_WINDOW::set_item_space(int s)   { yoff = s; }

void BASE_WINDOW::set_button_space(int d) { button_d = d; }


void BASE_WINDOW::item_error()
{ error_handler(1,"panel: too many panel items.");
  //exit(0);
  abort();
 }


//------------------------------------------------------------------------------
// adding items
//------------------------------------------------------------------------------

panel_item BASE_WINDOW::new_panel_item(int k, const char* s, void* addr, int h,
                                                             const char* hlp)
{ 
  if (item_count >= MAX_ITEM_NUM) item_error();

  char* s1 = string_dup(s);
  char* spos = s1;
  while (*spos != '\0' && *spos != '&') spos++;
  if (*spos) {
    for(char* p = spos; *p; p++) *p = *(p+1);
   }

  panel_item p = new Panel_Item(k,s1,addr,h,item_count);

  p->shortcut = (*spos && *spos != '&') ? int(spos - s1) : -1;

  p->help_str = string_dup(hlp);

  Item[item_count++] = p;

  if (k != Text_Item && k != Button_Item)
  { x_set_button_font(draw_win);
    int w = x_text_width(draw_win,s) + tw;
    if (k == Slider_Item) 
       w += sl_num_w;
    else 
       if (k == String_Menu_Item) 
         w += 4*tw;
/*
       else 
         w += 2*tw;
*/

    if (w > label_w) label_w = w;
    x_set_text_font(draw_win);
   }
  return p;
}


void BASE_WINDOW::hspace(int d)
{ //panel_item p = new_panel_item(Text_Item,"",0,0,0);
  panel_item p = new_panel_item(Space_Item,"space",0,0,0);
  p->width = d;
  p->height = 0;
}

void BASE_WINDOW::vspace(int d)
{ panel_item p = new_panel_item(Text_Item,"",0,d,0);
  p->width = 0;
  p->height = d;
}


void BASE_WINDOW::new_line()
{ new_panel_item(Newline_Item,"",0,0,0); 
  but_count++;
 }

void BASE_WINDOW::separator()
{ new_panel_item(Newline_Item,"",0,0,0); 
  but_count++;
 }


void BASE_WINDOW::fill_line()
{ new_panel_item(Fill_Item,"",0,0,0); }



panel_item BASE_WINDOW::text_item(const char* s)
{ // create new text item if s == "" or previous item is not a text item
  // otherwise append text to last text item

  panel_item it = (item_count > 0)  ? Item[item_count-1] : 0;

  if (s==0) s = "";

  if (str_length(s) == 0 || it == 0 || it->kind != Text_Item || it->argc == 0)
  { it = new_panel_item(Text_Item,"",0,ytskip,0); 
    it->argv = new char*[2];
    it->argc = 0;
   }

  // split into words and append

  int    argc1 = 0;
  char** argv1;

  split_text(s,argc1,argv1);

  int    argc0 = it->argc;
  char** argv0 = it->argv;

  it->argc = argc0 + argc1;

/*
cout << "s = " << s << endl;
cout << "argc = " << it->argc << endl;
*/

  if (it->argc > 0)
    it->argv = new char*[it->argc];
  else
    it->argv = 0;

  int i;
  int n = 0;
  for(i = 0; i < argc0; i++) it->argv[n++] = argv0[i]; 
  for(i = 0; i < argc1; i++) it->argv[n++] = argv1[i]; 

  delete[] argv0;
  delete[] argv1;

  return it;
}

void BASE_WINDOW::set_text(panel_item it, const char* s)
{
  if (it->kind != Text_Item)
  { fprintf(stderr,"illegal item in window::set_text\n");
    fflush(stderr);
    return;
   }

  delete[] it->argv;

  split_text(s,it->argc,it->argv);
}


panel_item BASE_WINDOW::string_item0(double x, double y, void* s)
{ panel_item p = new Panel_Item(String_Item,(char*)"",s,string_h,-1);
  p->xcoord = xpix(x) - label_w;
  p->ycoord = ypix(y);
  p->help_str = 0;
  p->data_str = access_str(s);
  p->offset = str_length(p->data_str);
  p->dat1 = 0;
  //p->dat2 = string_w/tw;
  p->dat2 = int(0.5 + string_w/twf) - 1;
  p->str_action = 0;
  return p;
 }
 



panel_item BASE_WINDOW::string_item(const char* s, void* x, 
                                    panel_str_action_func F, const char* hlp)
{ panel_item p = new_panel_item(String_Item,s,x,string_h,hlp);
  p->data_str = access_str(x);
  p->offset = str_length(p->data_str);
  p->dat1 = 0;
  //p->dat2 = string_w/tw;
  p->dat2 = int(0.5 + string_w/twf) - 1;
  //if (p->offset > p->dat2) p->offset = p->dat2;
  p->str_action = F;
  return p;
 }




void str_menu_selection(int code)
{ 
  int sel = code & 0xFFFF;
  int i   = (code>>16) & 0xFFFF;

  // I assume that str_menu_selection is called after closing the 
  // sub-menu and that at this moment active_window points to
  // the window that contains the string item to be changed


  BASE_WINDOW* str_menu_win = BASE_WINDOW::active_window;
  panel_item   str_menu_it = str_menu_win->Item[i];

  if (str_menu_it && sel >= 0)
  { str_menu_win->assign_str(str_menu_it->ref,str_menu_it->argv[sel]);
    str_menu_it->data_str = str_menu_win->access_str(str_menu_it->ref);
    str_menu_it->offset = str_length(str_menu_it->data_str);
    str_menu_it->dat1 = 0;

    str_menu_it->dat2 = 
     str_menu_win->get_default_width(str_menu_it)/str_menu_win->tw;

    str_menu_win->clipping(1);

    int draw_w = str_menu_win->draw_win;

    int save_lw = x_set_line_width(draw_w,str_menu_win->panel_line_width);

    line_style   save_ls = x_set_line_style(draw_w,solid);
    text_mode    save_tm = x_set_text_mode(draw_w,transparent);
    drawing_mode save_mo = x_set_mode(draw_w,src_mode);
    
    //str_menu_win->draw_string_item(str_menu_it);

    str_menu_win->activate_string_item(str_menu_it,0);

    x_set_line_width(draw_w,save_lw);
    x_set_line_style(draw_w,save_ls);
    x_set_text_mode(draw_w,save_tm);
    x_set_mode(draw_w,save_mo);

    if (str_menu_it->str_action || str_menu_it->str_action_ptr) 
    { 
      BASE_WINDOW::call_window = str_menu_win;
      BASE_WINDOW::call_item = str_menu_it;

      str_menu_win->clipping(2);
      if (str_menu_it->str_action_ptr){
        str_menu_it->str_action_ptr->set_params(str_menu_win,(char*)str_menu_it->argv[sel] );
        str_menu_it->str_action_ptr->operator()();
      }
      else str_menu_it->str_action((char*)str_menu_it->argv[sel]);
      str_menu_win->clipping(1);
     }

   }
}


void BASE_WINDOW::set_menu(panel_item p, int argc,const char** argv, int sz)
{ 
  if (p->kind != String_Menu_Item)
  { fprintf(stderr,"illegal item %s in window::set_menu\n",p->label_str);
    return;
   }

  const char* none_str = "none";

  if (argc == 0) 
  { argc = 1; 
    argv = &none_str;
   }

  if (p->argc > 0) 
  { for(int i = 0; i < p->argc; i++) delete[] p->argv[i];
    delete[] p->argv;
   }

/*
  if (argc == 0) 
  { p->argc = 0;
    p->argv = 0;
    p->menu_but->ref = 0;
    if (p->menu_win) delete p->menu_win;
    p->menu_win = 0;
    return;
   }
*/

  p->argc = argc;
  p->argv = new char*[argc];

  //int max_choice_len = 0;
  for(int i = 0; i < argc; i++) 
  { p->argv[i] = string_dup(argv[i]);
/*
    int l = str_length(argv[i]);
    if (l > max_choice_len) max_choice_len = l;
*/
   }

  // build menu panel

  int cols = 1;
  int rows = argc;

  if (sz == 0)
  { if (argc < 20) cols = 1;
    else if (argc < 40) cols = 2;
    else if (argc < 80) cols = 3;
    else cols = 4;
    rows = (argc + cols -1)/cols;
   }


  BASE_WINDOW* menu = new BASE_WINDOW(-1,-1);

  menu->buttons_per_line(cols);

//menu->menu_style = (sz > 0) ? 2 : 1;
  menu->menu_style = (sz > 0) ? 2 : 0;

  menu->menu_size = sz;

  for(int r = 0; r<rows; r++)
    for(int c = 0; c<cols; c++) 
    { int i = c*rows + r;
      if (i < argc)
         menu->button(p->argv[i],(p->index<<16) + i,str_menu_selection);
      else
         menu->button("",0,(BASE_WINDOW*)0);
     }

  if (argv == &none_str)
  { panel_item it = menu->get_item(argv[0]);
    it->enabled = 0;
   }

  if (p->menu_win) delete p->menu_win;

  p->menu_win = menu;
  p->menu_but->ref = menu;
  menu->owner_item = p->menu_but;

}



panel_item BASE_WINDOW::string_menu_item(const char* s, void* x, const char*,
                                  int argc,const char** argv, int size,
                                  panel_str_action_func F, const char* hlp)
{ 
/*
  if (argc == 0) return string_item(s,x,0,hlp);
*/

  panel_item p = new_panel_item(String_Menu_Item,s,x,string_h,hlp);
  p->data_str = access_str(x);
  p->offset = str_length(p->data_str);
  p->dat1 = 0;
  //p->dat2 = string_w/tw;
  p->dat2 = int(0.5 + string_w/twf) - 1;

  if (p->offset > p->dat2) p->offset = p->dat2;

  button("DOWN",-1,(BASE_WINDOW*)0);
  but_count--;

  panel_item q = Item[item_count-1];
  q->width = string_h;
  q->height = string_h;
  q->menu_but = q;

  p->menu_but = q;

  //p->menu_win->button_w += 2*string_h-8; 

  p->str_action = F;

  p->argc = 0;
  p->argv = 0;
  set_menu(p,argc,argv,size);

  return p;
}




panel_item BASE_WINDOW::int_item(const char* s, int* x, const char* hlp)
{ panel_item p = new_panel_item(Int_Item,s,x,string_h,hlp);
  p->data_str = make_string(*x);
  p->offset = str_length(p->data_str);
  p->dat1 = 0;
  //p->dat2 = string_w/tw;
  p->dat2 = int(0.5 + string_w/twf) - 1;
  if (p->offset > p->dat2) p->offset = p->dat2;
  return p;
 }

panel_item BASE_WINDOW::float_item(const char* s, double* x, const char* hlp)
{ panel_item p = new_panel_item(Float_Item,s,x,string_h,hlp);
  p->data_str = make_string(*x,panel_float_format);
  p->offset = str_length(p->data_str);
  p->dat1 = 0;
  //p->dat2 = string_w/tw;
  p->dat2 = int(0.5 + string_w/twf) - 1;
  if (p->offset > p->dat2) p->offset = p->dat2;
  return p;
 }


panel_item BASE_WINDOW::slider_item(const char* s, int* x, int low, int high, 
                                                      panel_action_func F,
                                                      const char* hlp)
{ panel_item p = new_panel_item(Slider_Item,s,x,slider_h,hlp);
  p->data_str = make_string(*x);
  p->dat1 = low;
  p->dat2 = high;
  p->action = F;
  return p;
 }

panel_item BASE_WINDOW::choice_item(const char* s, int* x, int argc,
                             const char** argv, int step, int off,
                             panel_action_func F, const char* hlp)
{ 
  panel_item p = new_panel_item(Choice_Item,s,x,choice_h,hlp);
  p->data_str = 0;
  p->dat2 = step;
  p->offset = off;
  p->argc = argc;
  p->argv = new char*[argc];
  x_set_button_font(draw_win);
  for(int i=0; i<argc; i++) 
  { //int w = x_text_width(draw_win,argv[i]) + int(1.5*th);
    int w = x_text_width(draw_win,argv[i]) + 2*tw;
    if (w > choice_w) choice_w = w;
    p->argv[i] = string_dup(argv[i]);
   }
  x_set_text_font(draw_win);
  p->action = F;
  return p;
 }


panel_item BASE_WINDOW::choice_mult_item(const char* s, int* x, int argc,
                                   const char** argv, panel_action_func F,
                                   const char* hlp)
{ choice_item(s,x,argc,argv,0,0,F,hlp); 
  panel_item p = Item[item_count-1];
  p->kind = Choice_Mult_Item;
  return p;
 }


panel_item BASE_WINDOW::bitmap_choice_item(const char* label, int *x, 
                                           int argc, int width, int height, 
                                                      unsigned char **argv, 
                                                      panel_action_func F,
                                                      const char* hlp)
{ int h = choice_h;
  if (height > th) h = choice_h + (height-th);
  panel_item p = new_panel_item(Bitmap_Choice_Item,label,x,h,hlp);  

  int f = int(0.5 + screen_dpi()/96.0);

  p->argc=argc;		   // number of bitmaps
  p->argv=new char*[argc]; // bitmaps
  for(int i=0; i < argc; i++) 
     p->argv[i] = x_create_bitmap(draw_win,width,height,argv[i],f);

/*
  p->dat1=width+3;
  p->dat2=height+th/2;
*/
  char* prect = p->argv[0];
  
  int pw,ph;
  x_get_pixrect_size(prect,pw,ph);

  p->dat1 = pw + 3;
  p->dat2 = ph + th/4;

  if (ph > th) h = choice_h + (ph-th);
  p->height = h;

  p->action = F;
  return p;
}


panel_item BASE_WINDOW::bitmap_choice_mult_item(const char* label, int *x,
                                          int argc, int width, int height, 
                                          unsigned char **argv, 
                                          panel_action_func F,
                                          const char* hlp)
{ 
  bitmap_choice_item(label,x,argc,width,height,argv,F,hlp); 
  panel_item p = Item[item_count-1];
  p->kind = Bitmap_Choice_Mult_Item;
  return p;
 }



panel_item BASE_WINDOW::bool_item(const char* s, bool* x, panel_action_func F, 
                                                          const char* hlp)
{ panel_item p = new_panel_item(Bool_Item,s,x,color_h,hlp);
  p->action = F;
  return p;
}



panel_item BASE_WINDOW::color_item(const char* s, int* x, panel_action_func F,
                                                          const char* hlp)
{ panel_item p = new_panel_item(Color_Item,s,x,color_h,hlp);

  //int w = 18*(color_h+2) - 2;
  int w = 18*(color_h + 2*panel_line_width) - 2*panel_line_width;

  p->width = w;

  if (w > slider_w)
  { slider_w  = w;
    string_w  = w;
    number_w  = w;
  }

/*
  button("",0,&color_menu);
  panel_item q = Item[item_count-1];
  p->menu_but = q;
  q->menu_but = q;
  but_count--;
*/

  p->action = F;
  return p;
}

 
panel_item BASE_WINDOW::pstyle_item(const char* label, point_style* x, 
                                        panel_action_func F, const char* hlp)
{
  unsigned char* pstyle_bits[7]; 
  pstyle_bits[0] = pixel_point_bits; 
  pstyle_bits[1] = cross_point_bits; 
  pstyle_bits[2] = plus_point_bits; 
  pstyle_bits[3] = circle_point_bits; 
  pstyle_bits[4] = disc_point_bits; 
  pstyle_bits[5] = rect_point_bits; 
  pstyle_bits[6] = box_point_bits;


  return bitmap_choice_item(label,(int*)x,pstyle_num,pstyle_width,
                                                     pstyle_height,
                                                     pstyle_bits,
                                                     F,hlp);
}



/*
static unsigned char* create_line_bitmap(int w, int h, int lw)
{ int line_bytes = w/8;
  int bytes = h*line_bytes;

  unsigned char* bmp = new unsigned char[bytes];
  for(int i=0; i<bytes; i++) bmp[i] = 0;

  int y1 = (h - lw + 1)/2;
  int y2 = y1 + lw - 1;

  for(int y=y1; y<=y2; y++)
  { int x = y*line_bytes;
    bmp[x] = 0xc0;
    for(int j=1; j<line_bytes-1; j++) bmp[x+j] = 0xff;
    bmp[x+line_bytes-1] = 0x03;
   }

  return bmp;
}
*/

static unsigned char* create_line_bitmap(int w, int h, int lw)
{ 
  int x1 = 6;
  int x2 = w-6;
  int y1 = (h - lw + 1)/2;
  int y2 = y1 + lw - 1;

  int line_bytes = w/8;
  if (w % 8) line_bytes++;

  int bytes = h*line_bytes;

  unsigned char* bmp = new unsigned char[bytes];
  for(int i=0; i<bytes; i++) bmp[i] = 0;

  for(int i=y1; i<=y2; i++) {
    unsigned char* p = bmp + i*line_bytes;
    for(int j = x1; j<x2; j++) *(p+j/8) |= (1 << (j%8));
  }

  return bmp;
}



panel_item BASE_WINDOW::lwidth_item(const char* label, int* x, 
                                    panel_action_func F, const char* hlp)
{ const int n = 5;
  unsigned char* lwidth_bits[n];

  int cw = 18*(color_h + 2*panel_line_width) - 2*panel_line_width;
  double f = screen_dpi()/91.0;
  int w =  int(cw/(f*n));
  int h =  int(0.7*color_h/f);

  for(int i=0; i<n;i++) lwidth_bits[i] = create_line_bitmap(w,h,i);
  return bitmap_choice_item(label,x,n,w,h,lwidth_bits,F,hlp);
}


static unsigned char* create_style_bitmap(int w, int h, int style)
{ int line_bytes = w/8;
  int bytes = h*line_bytes;
  unsigned char* bmp = new unsigned char[bytes];
  for(int i=0; i<bytes; i++) bmp[i] = 0;

  int y1 = h/2;
  int y2 = y1+1;

  unsigned char pat[4];

  switch (style) {

    case 0: //solid
            pat[0] = 0xff;
            pat[1] = 0xff;
            pat[2] = 0xff;
            pat[3] = 0xff;
            break;

    case 1: //dashed
            pat[0] = 0x00;
            pat[1] = 0xff;
            pat[2] = 0x00;
            pat[4] = 0xff;
            break;

    case 2: //dotted
            pat[0] = 0x60;
            pat[1] = 0x60;
            pat[2] = 0x60;
            pat[3] = 0x60;
            break;

    case 3: //dash-dot
            pat[0] = 0x18;
            pat[1] = 0xff;
            pat[2] = 0x18;
            pat[3] = 0xff;
            break;
   }

  for(int y=y1; y<=y2; y++) { 
    for (int x=1; x<line_bytes-1; x++) bmp[y*line_bytes+x] = pat[x%4];
  }

  return bmp;
}




panel_item BASE_WINDOW::lstyle_item(const char* label, line_style* x, 
                                    panel_action_func F, const char* hlp)
{ const int n = 4;
  unsigned char* lwidth_bits[n];

  double f = screen_dpi()/91.0;

  int w = 56; // muliple of 8
  int h = int(0.8*color_h/f);
  
  for(int i=0; i<n;i++)
      lwidth_bits[i] = create_style_bitmap(w,h,i);

  return bitmap_choice_item(label,(int*)x,n,w,h,lwidth_bits,F,hlp);
}




// buttons

int BASE_WINDOW::button(const char* s, int val, panel_action_func F, 
                                                const char* hlp)
{
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);
  p->dat1 = (val == -1) ? but_count : val;
  p->action = F;
  x_set_button_font(draw_win);
  int w = x_text_width(draw_win,s) + x_text_height(draw_win,s);
  x_set_text_font(draw_win);
  if (w  > button_w) button_w = w;
  p->width = w;
  but_count++;
  return p->dat1;
 }


void BASE_WINDOW::set_focus_button(int but)
{ int i;
  for(i = 0; i<item_count; i++)
  { panel_item p = Item[i];
    if (p->kind != Button_Item || p->menu_but == p) continue;
    if (p->dat1 == but) break;
   }
  focus_button =  (i < item_count) ? Item[i] : 0;
}


int BASE_WINDOW::button(const char* s, int val, BASE_WINDOW* wp,const char* hlp)
{ 
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);

  p->dat1 = val;
  if (val == -1)
  { p->dat1 = but_count;
    p->dat2 = 2;  // second bit = 1: return result of sub-window wp
   }

  p->ref = wp;
  if (wp) wp->owner_item = p;

  x_set_button_font(draw_win);
  //int w = x_text_width(draw_win,s) + 28;
  int w = x_text_width(draw_win,s) + 20*panel_line_width;
  x_set_text_font(draw_win);

  if (w > button_w) button_w = w;
  p->width = w;
  but_count++;
  return p->dat1;
 }


int BASE_WINDOW::button(int w, int h, unsigned char* bits, const char* s, 
                                                           int val, 
                                                           panel_action_func F,
                                                           const char*hlp)
{ if (h+1 > button_h) button_h = h+1;
  if (w+2 > button_w) button_w = w+2;

  if (hlp == 0) hlp = s;
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);
  p->argv = new char*[1];
  p->argv[0] = x_create_bitmap(draw_win,w,h,bits,1);
  p->argc = 1; // bitmap
  p->dat1 = (val == -1) ? but_count : val;
  p->action = F;
  p->width = w-1;
  but_count++;
  return p->dat1;
 }


int BASE_WINDOW::button(int w, int h, unsigned char* bits, const char* s, 
                                                           int val, 
                                                           BASE_WINDOW* wp,
                                                             const char*hlp)
{ if (h+2 > button_h) button_h = h+2;
  if (w+2 > button_w) button_w = w+2;

  if (hlp == 0) hlp = s;
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);
  p->argv = new char*[1];
  p->argv[0] = x_create_bitmap(draw_win,w,h,bits,1);
  p->argc = 1; // bitmap

  p->dat1 = val;
  if (val == -1)
  { p->dat1 = but_count;
    p->dat2 = 2;  // second bit = 1: return result of sub-window wp
   }

  p->ref = wp;
  if (wp) wp->owner_item = p;
  p->width = w-1;
  but_count++;
  return p->dat1;
 }


int BASE_WINDOW::button(char* pmap0, char* pmap1, const char* s, int val, 
                                                            panel_action_func F,
                                                            const char*hlp)
{ int w,h;
  x_get_pixrect_size(pmap0,w,h);

  if (button_h < h+1) button_h = h+1;
  if (button_w < w+3) button_w = w+3;

  if (hlp == 0) hlp = s;
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);
  p->argv = new char*[2];
  p->argv[0] = pmap0;
  p->argv[1] = pmap1;
  p->argc = -1; // pmap
  p->dat1 = (val == -1) ? but_count : val;
  p->action = F;
  p->width = w-1;
  but_count++;
  return p->dat1;
 }


int BASE_WINDOW::button(char* pmap0, char* pmap1, const char* s, int val, 
                                                               BASE_WINDOW* wp,
                                                               const char*hlp)
{ int w,h;
  x_get_pixrect_size(pmap0,w,h);
  if (h+2 > button_h) button_h = h+2;
  if (w+2 > button_w) button_w = w+2;
  if (hlp == 0) hlp = s;
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);
  p->argv = new char*[2];
  p->argv[0] = pmap0;
  p->argv[1] = pmap1;
  p->argc = -1; // pmap

  p->dat1 = val;
  if (val == -1)
  { p->dat1 = but_count;
    p->dat2 = 2;  // second bit = 1: return result of sub-window wp
   }

  p->ref = wp;
  if (wp) wp->owner_item = p;
  p->width = w-1;
  but_count++;
  return p->dat1;
 }




int BASE_WINDOW::menu_button(const char* s, int val, BASE_WINDOW* wp, 
                                                     const char* hlp)
{
  panel_item p = new_panel_item(Button_Item,s,0,yskip,hlp);

  p->dat1 = val;
  p->dat2 = 0x1;  // first bit = 1:  menu (bar) button

  if (val == -1)
  { p->dat1 = but_count;
    p->dat2 |= 0x2;  // second bit = 1: return result of sub-window wp
   }

  p->ref = wp;
  if (wp) wp->owner_item = p;

  x_set_button_font(draw_win);
//int w = x_text_width(draw_win,s) + 10;
  int w = x_text_width(draw_win,s) + 10*panel_line_width;
  x_set_text_font(draw_win);

//if (w  > button_w) button_w = w; // why not ?

  p->width = w;
  menu_bar_mode = 1;
  but_count++;
  return p->dat1;
 }



//------------------------------------------------------------------------
// drawing buttons & items
//------------------------------------------------------------------------


void BASE_WINDOW::draw_label(panel_item it)
{ char* str = it->label_str;
  int slen = str_length(str);
  if (slen == 0) return;

  x_set_button_font(draw_win);

  int x = it->xcoord;
  int y = it->ycoord;

  if (str[0] == '!' || str[0] == '%') {
     str++;
     slen--;
   }

  if (strncmp(str,"\\tt ",4) == 0)
  { x_set_fixed_font(draw_win);
    str += 4;
    slen -= 4;
   }

  int lw = get_item_label_width(it);

  x_set_color(draw_win,panel_bg_color); 
  x_box(draw_win,x,y,x+lw,y+it->height);

  x_set_color(draw_win,it->enabled ? it->label_clr : disable_color);

  int i = 0;
  while (i < slen && str[i] != '@') i++;

  bool arrow = (i < slen && strcmp(str+i,"@arrow") == 0);

  int dy = it->height - x_text_height(draw_win,str);

  int yt = y + dy/2;

  //yt -= 2;
  yt -= 1;

  if (i < slen) str[i] = '\0';
  x_text(draw_win,x,yt,str);
  if (i < slen) str[i] = '@';

  if (arrow)
  { int w = x_text_width(draw_win,str);
    int x1 = x + w + 3;
    int x2 = x+lw - 5;
    int yy = y+it->height/2;
    x_line(draw_win,x1,yy,x2,yy);
    x_line(draw_win,x2-9,yy-4,x2-1,yy);
    x_line(draw_win,x2-9,yy+4,x2-1,yy);
   }

  x_set_text_font(draw_win);
}


void BASE_WINDOW::draw_box_with_shadow(int x1,int y1,int x2,int y2,int c,int w)
{ // below and right of box(x1,y1,x2,y2)
  x_set_color(draw_win,shadow_color);
  x_box(draw_win,x1+3,y1+3,x2+w,y2+w);
  x_set_color(draw_win,c);
  x_box(draw_win,x1,y1,x2,y2);
/*
  x_set_color(draw_win,black);
  x_rect(draw_win,x1,y1,x2,y2);
*/
 }


void BASE_WINDOW::draw_d3_box(int x1,int y1,int x2,int y2, int pressed, 
                                                           int enabled)
{
  if (x2 <= x1 || y2 <= y1) return;

  int save_color = press_color;

  if (press_color == invisible) press_color = panel_bg_color;

//color dark = color(64,64,64);
  color dark = color("#333333");

  color c1 = (pressed) ? dark : white;
  color c2 = (pressed) ? shadow_color : panel_bg_color;
  color c3 = (pressed) ? white : shadow_color;
  color c4 = (pressed) ? press_color : dark;


  if (!enabled) 
  { c1 = c3 = shadow_color;
    c2 = c4 = panel_bg_color;
   }

  x_set_color(draw_win,panel_bg_color);

  //x_box(draw_win,x1,y1,x2+1,y2+1);
  x_box(draw_win,x1,y1,x2+1,y2);


  if (pressed) 
   { if (enabled)
       x_set_color(draw_win,press_color);
     else
       x_set_color(draw_win,color(200,200,200));
     x_box(draw_win,x1,y1,x2+1,y2+1);
    }


  x_set_color(draw_win,c2);
  x_line0(draw_win,x1+1,y1+1,x2+(pressed?1:0),y1+1);  // inner top
  x_line0(draw_win,x1,y1+1,x2+(pressed?1:0),y1+1);    // inner top (high dpi)
  //x_line0(draw_win,x1+1,y1+1,x1+1,y2);                // inner left

  x_set_color(draw_win,c1);
  x_line0(draw_win,x1,y1,x2,y1);	              // outer top
  //x_line0(draw_win,x1,y1,x1,y2);		      // outer left
  x_line0(draw_win,x1,y1,x1,y2+1);		      // outer left (high dpi)


  x_set_color(draw_win,c3);
  x_line0(draw_win,x1+(pressed?1:0),y2+1,x2+1,y2+1);  // outer bottom 
  x_line0(draw_win,x2+1,y1+(pressed?1:0),x2+1,y2+1);  // outer right

  x_set_color(draw_win,c4);
  x_line0(draw_win,x1+(pressed?2:1),y2,x2,y2);	      // inner bottom
  x_line0(draw_win,x2,y1+(pressed?2:1),x2,y2);        // inner right

/*
  // lower left and upper right pixel
  x_set_color(draw_win,grey2);
  x_pixel(draw_win,x1,y2+1);
  x_pixel(draw_win,x2+1,y1);
*/

  press_color = save_color;
}


int BASE_WINDOW::draw_text_item(panel_item it, int win)
{ 
  // compute only height of text box if win == 0

  if (it->argc == 0)
  { // empty text: new paragraph
    return th/2;
   }

  int xpos = it->xcoord;

  x_set_text_font(draw_win);

  if (win)
  { x_set_color(draw_win,panel_bg_color);
    x_box(draw_win,xpos,it->ycoord,panel_width,it->ycoord+it->height+1);
    if (it->enabled) 
      x_set_color(draw_win,black);
    else
      x_set_color(draw_win,disable_color);
   }
  
  if (win) clipping(1);

  int y = format_text(it->argc,it->argv,xpos,panel_width-xpos,
                      it->ycoord, it->height,win, !it->enabled);

  if (win) clipping(2);

  return y - it->ycoord + th/2;
}


void BASE_WINDOW::draw_string_item(panel_item it, const char* s)
{ 
  if (it->kind == String_Item || it->kind == String_Menu_Item)  
      it->data_str = access_str(it->ref);

  int active = (it == active_item);
  int selected = it->str_selected;

  int lw = get_item_label_width(it);
  int w = get_default_width(it);

  int x1 = it->xcoord + lw;
  int y1 = it->ycoord;
  int x2 = x1 + w;
  int y2 = y1 + string_h;

  if (it->index < 0)
  { x_set_color(draw_win,grey1);
    x_box(draw_win,x1-1,y1-1,x2+1,y2+1);
    x_box(draw_win,x1-1,y1-1,x2+1,y2+1);
    x_set_color(draw_win,black);
    x_rect(draw_win,x1-1,y1-1,x2+1,y2+1);
   }

  start_buffering();
  clipping(1);

  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,x1-lw-1,y1-2,x2+1,y2+4);

  draw_label(it);
  
  if (s == 0)  s = it->data_str;

  if (it->index >=0)
  { 
    if (it->kind != String_Menu_Item)
    { int xr = x1-8;
      int yr = it->ycoord+it->height/2;
  
      if (it->dat1 > 0)
        draw_left_menu_button(xr,yr,-1,it->enabled);
      else
       { x_set_color(draw_win,panel_bg_color);
         x_box(draw_win,xr-6,yr-8,xr+6,yr+8);
        }
  
      xr = x2+8;

/*
      int slen = str_length(s);
  
      // overflow arrow button
      if (slen > it->dat2)
        draw_right_menu_button(xr,yr,-1,it->enabled);
      else
       { x_set_color(draw_win,panel_bg_color);
         x_box(draw_win,xr-6,yr-8,xr+6,yr+8);
        }
*/

     }
  
    int save = press_color;
    //press_color = (selected) ? blue : ivory;
    press_color = ivory;
    if (selected)
      draw_box_with_shadow(x1,y1,x2,y2+2,blue,1);
    else
      draw_d3_box(x1,y1,x2,y2,1,it->enabled);
    press_color = save;
  }


  // write text into box

  x_set_fixed_font(draw_win);
  int th = x_text_height(draw_win,"H");

//int yt = (y1 + y2 - th)/2;
  int yt = (y1 + 3* panel_line_width + y2 - th)/2;

  while (it->offset > it->dat2)
  { it->dat1++;
    it->dat2++;
   }

  s += it->dat1;

  //int xtoff = active ? 5 : 4;
  //int ytoff = active ? 1 : 0;

  int xtoff = 4*panel_line_width;
  int ytoff = panel_line_width/2;

  int len = it->dat2 - it->dat1;

  x_set_fixed_font(draw_win);
  x_set_color(draw_win, it->text_clr);


  if (!it->enabled) {
  //x_set_color(draw_win, disable_color);
  //use black even if string item is disabled
    x_set_color(draw_win, black);
  }
  else
    if (selected) x_set_color(draw_win,white);


  if (it->secret)
  { int slen = str_length(s);
    char* sx = new char[slen+1];
    for(int i=0; i<slen; i++) sx[i] = '*';
    sx[slen] = '\0';
    x_text(draw_win,x1+xtoff,yt+ytoff,sx,len);
    delete[] sx;
   }
  else
    x_text(draw_win,x1+xtoff,yt+ytoff,s,len);

/*
x_set_color(draw_win,red);
x_line(draw_win,x1,y1,x1,y1+100);
x_line(draw_win,x2,y1,x2,y1+100);

cout << "draw_string_item: s = " << s << endl;
cout << string("draw_string_item: chars = %d   width = %d", 
                             len,x_text_width(draw_win,s,len)) << endl;
*/

  //x_set_text_font(draw_win);

  if (active && it->enabled)
  { // draw cursor

    int len = it->offset - it->dat1;
    int twidth = x_text_width(draw_win,s,len);

    int xc = x1 + xtoff + twidth;
    int X[4];
    int Y[4];

    int d = tw/2;

    X[0] = xc;
    Y[0] = y2 - 2*d + panel_line_width;

    X[1] = xc + d;
    Y[1] = y2 + panel_line_width;

    X[2] = xc - d;
    Y[2] = y2 + panel_line_width;

    X[3] = X[0];
    Y[3] = Y[0];

    x_set_color(draw_win,0x333333);
    x_fill_polygon(draw_win,4,X,Y);
   }

  if (it->kind == String_Menu_Item)
  { x_set_button_font(draw_win);
    int xl1 = it->xcoord;
    int xl2 = xl1 + x_text_width(draw_win,it->label_str);
    flush_pixels(xl1,y1-1,xl2,y2+3);
    flush_pixels(x1,y1-1,x2+1,y2+3);
   }
  else
  { if (it->index < 0)
       flush_pixels(x1-1,y1-1,x2+1,y2+3);
    else
       flush_pixels(x1-lw-1,y1-1,x2+1,y2+3);
   }

  stop_buffering();
  clipping(2);

  x_flush_display();
}



void BASE_WINDOW::activate_string_item(panel_item it, int x, int but)
{ 
  if (!it->enabled) return;

  x_set_focus(draw_win);

  keyboard(1);

  int lw = get_item_label_width(it);

  if (but > 0)
  { if (active_item != it) 
    { it->offset = str_length(it->data_str);
      if (it->offset > it->dat2) it->offset = it->dat2;
     }
    else
    { const char* s = it->data_str + it->dat1;
      int x0 = it->xcoord + lw;
      int xlen = x - x0;

      x_set_fixed_font(draw_win);
      int j = str_length(s);
      while (x_text_width(draw_win,s,j) > xlen) j--; 

      j += it->dat1;
      if (j > it->dat2) j = it->dat2;
      it->offset = j;
    }
  }

  //if (active_item && active_item != it)
  if (active_item != it)
  { 
    panel_item active_old = active_item;

    active_item = it;

    if (active_old)
    { if (active_old->kind == Int_Item)
        { delete[] active_old->data_str;
          active_old->data_str = make_string(*(int*)active_old->ref);
         }
      if (active_old->kind == Float_Item)
        { delete[] active_old->data_str;
          active_old->data_str = 
                make_string(*(double*)active_old->ref,panel_float_format);
         }
      draw_string_item(active_old);
    /*
      if (active_old->str_action)
      { clipping(2);
        x_set_text_font(draw_win);
        call_window = this;
        call_item = active_old;
        active_old->str_action(active_old->data_str);
        clipping(1);
       }
      */
     }
   }

  draw_string_item(it);

  if (but == 3 && (it->kind == String_Item || it->kind == String_Menu_Item))
  {
    // right button click
    // open paste/copy/delete menu

    it->str_selected = 1;
    draw_string_item(it);

    char* p = x_text_from_clipboard(draw_win);

    BASE_WINDOW menu(-1,-1);
    menu.buttons_per_line(1);
    menu.menu_mode = 1;
    //menu.set_panel_bg_color(ivory);
    BASE_WINDOW* wp = 0;
    menu.button("cut",0,wp,0);
    menu.button("copy",1,wp,0);
    menu.button("paste",2,wp,0);
    menu.button("delete",3,wp,0);

    if (p == NULL) 
       menu.disable_item(menu.get_button_item(2));


    menu.p_win = this;
    menu.place_panel_items();

#if defined(__APPLE__)
    menu.display(x,it->ycoord + string_h + 4,this);
    x_set_border_width(menu.draw_win,0);
#else
    x_set_border_width(menu.draw_win,0);
    menu.display(x,it->ycoord + string_h + 4,this);
#endif

    int win,val1,val2,xx,yy;
    unsigned long t;
    while (x_get_next_event(win,xx,yy,val1,val2,t) != button_press_event);

    int sel = -1;

    if (win == menu.draw_win)
    { x_put_back_event();
      menu.p_win = 0;
      sel = menu.read_mouse();
     }

    menu.close();
    it->str_selected = 0;
    active_window = this;

    switch (sel) {

    case 0: { // cut
              x_text_to_clipboard(draw_win,it->data_str);
              assign_str(it->ref,"");
              it->data_str = access_str(it->ref);
              it->offset = 0;
              break;
            }

    case 1: { // copy	 
              x_text_to_clipboard(draw_win,it->data_str);
              break;
            }

    case 2: { // paste
/*
              int len = str_length(it->data_str) + str_length(p) + 1;
              char* buf = new char[len];
              int off = it->offset;
              strncpy(buf,it->data_str,off);
              strcpy(buf+off,p);
              strcpy(buf+off+str_length(p),it->data_str+off);
              buf[len-1] = '\0';
              assign_str(it->ref,buf);
              it->data_str = access_str(it->ref);
              it->offset = off + str_length(p);
              delete[] buf;
*/
              assign_str(it->ref,p);
              it->data_str = access_str(it->ref);
              it->offset = str_length(p);
              break;
             }

    case 3: { // delete	 
              assign_str(it->ref,"");
              it->data_str = access_str(it->ref);
              it->offset = 0;
              break;
            }

     }

   // redraw all items
   for(int i=0; i<item_count; i++) draw_item(Item[i]);
  }


}
   
   

bool BASE_WINDOW::panel_text_edit(panel_item it, int key_val)
{ 
  int len = 2*str_length(it->data_str);
  if (len < 256) len = 256;

  int lw = get_item_label_width(it);
  int iw = get_default_width(it);

  char*  str = new char[len];

  strcpy(str,it->data_str);

  int k,val1,val2;
  int xc,yc;
  int w;
  unsigned long t;

  if (key_val != 0)
  { k = key_press_event;
    val1 = key_val;
   }
  else
  { do k = x_get_next_event(w,xc,yc,val1,val2,t);
    while (w != draw_win || (k != key_press_event && k != button_press_event));
   }

  if (k == button_press_event) 
  { if (it->index < 0)
    { int x1 = it->xcoord + lw;
      int y1 = it->ycoord;
      //int x2 = x1 + string_w;
      int x2 = x1 + iw;
      int y2 = y1 + string_h;
      bool res = false;
      if (yc > y1 && yc < y2 && xc > x1 && xc < x2) 
      { activate_string_item(it,xc);
        res = true;
       }
      delete[] str;
      return res;
     }
    x_put_back_event();
    delete[] str;
    return false;
   }

  if (val1 == KEY_RETURN || val1 == KEY_UP || val1 == KEY_DOWN)
  { if (it->index < 0) return false;
    if (it->kind == String_Menu_Item && (it->str_action || it->str_action_ptr))
    { clipping(2);
      x_set_text_font(draw_win);
      call_window = this;
      call_item = it;
      
      if (it->str_action_ptr) {
        it->str_action_ptr->set_params(this,str);
        it->str_action_ptr->operator()();
      }
      else it->str_action(str);
      
      clipping(1);
     }
    x_put_back_event();
    delete[] str;
    return false;
   }


  if (it->kind != String_Item && it->kind != String_Menu_Item) 
   delete[] it->data_str;


  char c    = (char)val1;
  int  j    = it->offset;
  int  strl = str_length(str);

  //if (isprint(c))
  if (!iscntrl(c))
  { for(int i=strl; i>=j; i--) str[i+1] = str[i];
    str[j]=c;
    j++;
    if (j > it->dat2)
    { it->dat1++;
      it->dat2++;
     }
   }
  else
   switch (c) {

   case KEY_BACKSPACE:    
                   if (j > 0) 
                   { for(int i=j; i<=strl; i++) str[i-1] = str[i];
                     if (--j < it->dat1)
                     { it->dat1--;
                       it->dat2--;
                      }
                    }
                   break;

   case KEY_LEFT:  if (j == 0) break;
                   if (--j < it->dat1) 
                   { it->dat1--;
                     it->dat2--;
                   }
                   break;

   case KEY_RIGHT: if (j == strl) break;
                   if (++j > it->dat2)
                   { it->dat1++;
                     it->dat2++;
                   }
                   break;

   case KEY_HOME:  j = 0;
                   it->dat2 -= it->dat1;
                   it->dat1 = 0;
                   break;

   case KEY_END:   j = strl;
                   if (it->dat2 < j) 
                   { it->dat1 += (j-it->dat2);
                     it->dat2 = j;
                   }
                   break;
   }


 it->offset = j;

 draw_string_item(it,str);

 it->data_str = string_dup(str);


 if ((it->str_action || it->str_action_ptr) && /* it->kind == String_Item && */
     (!iscntrl(c) || c == KEY_BACKSPACE))
 { clipping(2);
   x_set_text_font(draw_win);
   call_window = this;
   call_item = it;
   
   if (it->str_action_ptr) {
     it->str_action_ptr->set_params(this,str);
     it->str_action_ptr->operator()();
   }
   else it->str_action(str);
   
   clipping(1);
  }

 delete[] str;

 return true;
}



void BASE_WINDOW::draw_choice_item(panel_item it)
{ 
  clear_item(it);

  draw_label(it);

  int val = *(int*)it->ref;

  int lw = get_item_label_width(it);

  int x = it->xcoord + lw;
  int y = it->ycoord;
  int n = it->argc;
  int c;

  if (it->kind == Choice_Mult_Item) 
     c = val;
  else
    { c = (val - it->offset)/it->dat2;
      if (val != it->offset + c*it->dat2) c = -1;
     }

  int bw = get_item_button_width(it);

  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,x,y,x+n*bw,y+choice_h+3);
  x_set_color(draw_win,black);


  for(int j=0; j<n; j++)
  { bool pressed = (j == c); 

    if (it->kind == Choice_Mult_Item) pressed = ((c&(1 << j)) != 0);


    draw_d3_box(x,y,x+bw-3,y+choice_h,pressed,it->enabled);
    int xx = x + bw/2 - (pressed ? 1 : 2);
    int yy = y + choice_h/2 - (pressed ? 0 : 1);

    //yy++;

/*
    color text_clr = (str_length(it->label_str) == 0) ? it->label_clr : (color)black;
*/
    color text_clr = it->text_clr;

    if (!pressed)
    { int r,g,b;
      text_clr.get_rgb(r,g,b);
      text_clr = color((r+255)/2,(g+255)/2,(b+255)/2);
     }

    if (!it->enabled) text_clr = disable_color;

    x_set_color(draw_win,text_clr);
    x_set_button_font(draw_win);

    x_ctext(draw_win,xx,yy,it->argv[j]);

    x_set_text_font(draw_win);
    x_set_color(draw_win,black);

    x += bw;
   }

  flush_item(it);
}




void BASE_WINDOW::draw_bitmap_choice_item(panel_item it) 
{
  clear_item(it);
  draw_label(it);

  int lw = get_item_label_width(it);

  int n = it->argc;
  int w = it->dat1;
  int h = it->dat2;
  int x = it->xcoord + lw;
  int y = it->ycoord;
  int c = *(int*)it->ref;

  y += (it->height - h)/2;

  for(int j=0; j<n; j++) 
  { bool pressed = (j == c); 
    if (it->kind == Bitmap_Choice_Mult_Item) pressed = ((c&(1 << j)) != 0);

    draw_d3_box(x,y,x+w,y+h,pressed,it->enabled);

    if (it->enabled)
       x_set_color(draw_win,pressed ? bitmap_color1 : bitmap_color0);
    else
       x_set_color(draw_win, disable_color);

    int dx = pressed ? 3 : 2;
    int dy = pressed ? -3 : -4;

    x_insert_bitmap(draw_win,x+dx,y+h+dy,it->argv[j]);
    x += w;
    x += 3;
   }

  flush_item(it);
}



void BASE_WINDOW::draw_color_button(int x, int y, int j, int pressed,
                                                           int enabled)
{ 
  // (x,y) = center of button
  // j = -1..16

  if (j < -1 || j > 16) return;

  x += (j+1) * (color_h + 2*panel_line_width);

  color clr = color::get(j);

  int w = color_h/2;

  int save = press_color;
  press_color = clr;
  draw_d3_box(x-w,y-w,x+w,y+w,pressed,enabled);
  press_color = save;

  if (clr == invisible || (pressed && enabled)) return;

  int d = (2*w)/3 - 1;

  x_set_color(draw_win, enabled ? clr : disable_color);
  x_box(draw_win,x-d,y-d,x+d,y+d);

  int lw = x_set_line_width(draw_win,1);
  x_set_line_width(draw_win,1);
  x_set_color(draw_win,black);
  x_rect(draw_win,x-d,y-d,x+d,y+d);
  x_set_line_width(draw_win,lw);

}



void BASE_WINDOW::draw_color_item(panel_item it)
{ 
  clear_item(it);
  draw_label(it);

  int lw = get_item_label_width(it);

  int y = it->ycoord + it->height/2 + 1;
  int x = it->xcoord + lw +color_h/2;
  int c = *(int*)it->ref;
  for(int j=-1; j < 17; j++) {
    draw_color_button(x,y,j,c==color::get(j),it->enabled);
  }

  flush_item(it);
}



void BASE_WINDOW::draw_bool_item(panel_item it)
{  
  clear_item(it);

  int slen = str_length(it->label_str);
  int lw = get_item_label_width(it);

  if (lw > 0 && slen > 1) draw_label(it);

  int yt = it->ycoord + it->height/2;
  int xt = it->xcoord + lw + color_h/2;
  int c  = *(bool*)it->ref;
  int w  = color_h/2;

  if (strcmp(it->label_str,"^") == 0)
  { draw_up_menu_button(xt,yt,c,it->enabled);
    flush_item(it);
    return;
   }

  if (strcmp(it->label_str,"v") == 0)
  { draw_down_menu_button(xt,yt,c,it->enabled);
    flush_item(it);
    return;
   }

  draw_d3_box(xt-w,yt-w,xt+w+1,yt+w,c,it->enabled);

  if (slen == 1) 
  { color text_clr = it->label_clr;
    if (!it->enabled) text_clr = disable_color;
    int x = xt;
    int y = yt-1;
    if (c || !it->enabled) { x++; y++; }
  //x_set_color(draw_win,text_clr);
    x_set_color(draw_win,grey3);
    x_set_button_font(draw_win);
    x_ctext(draw_win,x,y,it->label_str);
    x_set_text_font(draw_win);
    x_set_color(draw_win,black);
   }

   flush_item(it);
}



void BASE_WINDOW::draw_slider_item(panel_item it, int x)
{ 
  clear_item(it);

  int lw = get_item_label_width(it);
  int w = get_default_width(it);

  int x0 = it->xcoord + lw;
  int y0 = it->ycoord;
  int x1 = x0 + w - 1;

//int y1 = y0 + slider_h;
  int y1 = y0 + it->height;

  int mi = it->dat1;
  int ma = it->dat2;

  double d = (ma > mi) ? float(w-1)/(ma-mi) : 1;

  draw_label(it);


  int val = *(int*)it->ref;

  bool undef = (val < mi || val > ma);

  if (x == 0) {
    // compute offset
    x = x0  + (int)(0.5 + d * (val - mi));
   }

  if (x < x0) x = x0;
  if (x > x1) x = x1;

  it->offset = x;

  val = mi + int(0.5 + (x-x0)/d);

  *(int*)it->ref = val;

  int val_w = sl_num_w;

  if (it->label_str[0] != '!')
  { char text[16];

    if (undef)
      sprintf(text,"   ?");
    else
    {
      char* format_p = strstr(it->label_str,"@");

      if (format_p)
      { sprintf(text,format_p+1,val);
        val_w = x_text_width(draw_win,text) + tw;
       }
      else
      if (it->label_str[0] == '%')
        sprintf(text,"%3d%%", (100*(val-mi))/(ma-mi));
      else
       if (mi < 0 && ma > 0)
        sprintf(text,"%+4d",val);
       else
        sprintf(text,"%4d",val);
     }

    x_set_color(draw_win,panel_bg_color);
    x_box(draw_win,x0-val_w,y0,x0-1,y1);

    x_set_fixed_font(draw_win);

    int dy = it->height - x_text_height(draw_win,"H");
    int yt = y0 + dy/2 - 1;

    x_set_color(draw_win,it->enabled ? it->label_clr : disable_color);
    x_text(draw_win,x0-val_w,yt,text);
    x_set_text_font(draw_win);
  }

  // draw slider

  int save_lw = x_set_line_width(draw_win,panel_line_width);
  draw_d3_box(x, y0,x1,y1,1,it->enabled);
  draw_d3_box(x0,y0,x, y1,0,it->enabled);
  x_set_line_width(draw_win,save_lw);

  it->offset = x;

  x_set_color(draw_win,black);

  flush_item(it);
}

void BASE_WINDOW::draw_separator(panel_item it)
{ clipping(1);
  x_set_color(draw_win,shadow_color);
  x_line(draw_win,1,it->ycoord,panel_width-1,it->ycoord);
  x_set_color(draw_win,white);
  x_line(draw_win,1,it->ycoord+1,panel_width-1,it->ycoord+1);
  clipping(2);
}


void BASE_WINDOW::draw_button(panel_item it, int pressed)
{ 
  clipping(1);

  //if (it->ref == this) 
  if (strcmp(it->label_str,"DOWN") == 0 || strcmp(it->label_str,"UP") == 0)
  { int x1 = it->xcoord;
    int y1 = it->ycoord;
    int d =  it->height/2;

    if (strcmp(it->label_str,"DOWN") == 0)
    { draw_down_menu_button(x1+d+1,y1+d-1,pressed,it->enabled);
      return;
     }

    if (strcmp(it->label_str,"UP") == 0)
    { draw_up_menu_button(x1+d+1,y1+d+1,pressed,it->enabled);
      return;
     }
   }


   // scrollbar slider
   if (strcmp(it->label_str,"SLIDER") == 0) pressed = false;


   // invisible button (label starts with ".")
   if (it->label_str != 0 && it->label_str[0] == '.') return;

   //if (it == active_button) pressed = 1;

   int enab = it->enabled;
   if (!enab) pressed = 0;


   bool is_menu = (panel_height >= window_height && item_count == but_count);

   if (it->dat2 % 2)
   { // sub window (menu)
     draw_menu_button(it,pressed);
     return;
    }

   if (menu_bar_mode && (it->argc != 0)) // before: == -1
   { int bar = menu_bar_mode;
     menu_bar_mode = 0;
     draw_menu_button(it,pressed);
     menu_bar_mode = bar;
     return;
   }

   if (it->menu_but == 0 && menu_style == 2) 
   { draw_menu_item(it,pressed);
     return;
    }

   clear_item(it);

   char* s = it->label_str;

   int x1 = it->xcoord;
   int y1 = it->ycoord;

   int x2 = x1 + it->width;
   int y2 = y1 + it->height;

   int xt = x1 + it->width/2;
   int yt = y1 + it->height/2;

   int dt = 1;

   x_set_color(draw_win, panel_bg_color);

/*
   if (!it->enabled)
      x_box(draw_win,x1,y1,x2+1,y2+1);
   else
*/
   { if (is_menu && menu_style == 1)
       { y1++; y2++;
         if (pressed) 
            draw_d3_box(x1+1,y1,x2,y2-2,0,it->enabled);
         else
            if (buts_per_line == 1)
              x_box(draw_win,x1+1,y1,x2,y2-1);
            else
              x_box(draw_win,x1+1,y1,x2+1,y2-1);
         dt = -1;
        }
     else
       if (it->menu_but == it && str_length(it->label_str) == 0) 
         { x_box(draw_win,x1,y1,x2+2,y2+2);
           if (pressed || it->ref == 0) 
             //draw_d3_box(x1,y1,x2,y2,pressed,it->enabled);
             draw_d3_box(x1,y1,x2,y2,pressed,it->ref != 0);
          }
       else
         draw_d3_box(x1,y1,x2,y2,pressed);
  
     if (!pressed && it == focus_button)
     { int r1,g1,b1,r2,g2,b2;
       color(grey1).get_rgb(r1,g1,b1);
       color(grey2).get_rgb(r2,g2,b2);
       int fcol = color((r1+r2)/2,(g1+g2)/2,(b1+b2)/2).get_value();
       if (fcol == -1) fcol = press_color;
       x_set_color(draw_win,fcol);
       x_box(draw_win,x1+1,y1+1,x2-1,y2-1);
      }
   }

   x_set_color(draw_win, (enab) ? it->label_clr : disable_color);

   x_set_button_font(draw_win);

   int scut = it->shortcut;

   if (it->argc == 0) // no bitmap/pixmap button
   { if (it->ref != this) // insert text
     { int ch = x_text_height(draw_win,"H");
       //int ch = x_text_height(draw_win,s);
       if (center_button_label)
         { if (pressed) { xt += dt; yt += dt; }
           x_ctext(draw_win,xt,yt,s);
           if (scut >= 0) x_ctext_underline(draw_win,xt,yt,s,scut,scut);
          }
       else
         { xt = x1 + ch/2;
           yt = yt - ch/2;
           if (pressed) { xt += dt; yt += dt; }
           x_text(draw_win,xt,yt,s);
           if (scut >= 0) x_text_underline(draw_win,xt,yt,s,scut,scut);
          }
      }

     if (it->ref && it->enabled) // sub window
     { int d = (y2-y1)/2;
       int x = (pressed) ? dt : 0;
       if (dt == 0) pressed = !pressed;
       if (it->ref == this) 
       { 
          if (strcmp(it->label_str,"DOWN") == 0)
             draw_down_menu_button(x1+d+x+1,y1+d+x-1,pressed,enab);
          else
          if (strcmp(it->label_str,"UP") == 0)
             draw_up_menu_button(x1+d+x+1,y1+d+x+1,pressed,enab);
        }
       else
       if (buts_per_line == 1 && item_count == but_count) // sub menu
          draw_right_menu_button(x2-d+x,y1+d+x,-1,enab);
       else // string_menu_item ...
          //draw_down_menu_button(x1+d+1,y1+d-1,pressed,enab);
          draw_down_menu_button(x1+d,y1+d,pressed,enab);
     }

   }
  else 
  { 
    if (it->argc == 1)
     { // bitmap
       if (pressed)
         x_insert_bitmap(draw_win,x1+3,y2,it->argv[0]);
       else
         x_insert_bitmap(draw_win,x1+2,y2-1,it->argv[0]);
      }

      if (it->argc == -1)
      { // pixmap
        char* p = it->argv[pressed ? 1 : 0];
        int w,h;
        x_get_pixrect_size(p,w,h);
        int x = x1 + 2 + (it->width - w) /2;
        int y = y2 - (it->height - h)/2;
        if (pressed) { x++; y++; }
        x_insert_pixrect(draw_win,x,y,p);
      }
   }

  flush_item(it);

  x_set_text_font(draw_win);
  x_set_color(draw_win,black);
  x_flush_display();
}


void BASE_WINDOW::draw_menu_button(panel_item it, int pressed)
{ 
  if (it == active_button) {
/*
    if (pressed == 0 && it->help_str) cout << it->help_str << endl;
*/
    pressed = 1;
  }

  char* s = it->label_str;

  int x1 = it->xcoord;
  int y1 = it->ycoord;

  int x2 = x1 + it->width;

  //int y2 = y1 + it->height+1;
  int y2 = y1 + it->height + panel_line_width;


  int xt = x1 + it->width/2;
//int yt = y1 + it->height/2 + it->height%2;
  int yt = y1 + it->height/2 + 1;


  if (!pressed) 
    { x_set_color(draw_win,panel_bg_color);
      x_box(draw_win,x1-1,y1-1,x2+2,y2+2);
     }
  else
    draw_d3_box(x1,y1,x2,y2,0,it->enabled);


  x_set_color(draw_win,it->enabled ? it->label_clr : disable_color);

  if (it->argc == 1) // bitmap
  {
    if (it->enabled)
       x_set_color(draw_win,pressed ? bitmap_color1 : bitmap_color0);

    if (pressed)
       x_insert_bitmap(draw_win,x1+2,y2-1,it->argv[0]);
    else
       x_insert_bitmap(draw_win,x1+3,y2,it->argv[0]);
   }
  else
  if (it->argc == -1) // pixmap
  { y2 -= 1;
    char* p = it->argv[pressed ? 1 : 0];
    int w,h;
    x_get_pixrect_size(p,w,h);
    int x = x1 + 2 + (it->width - w) /2;
    int y = y2 - (it->height - h)/2;
    if (pressed) { x--; y--; }
    x_insert_pixrect(draw_win,x,y,p);
/*
    if (pressed)
       x_insert_pixrect(draw_win,x1+2,y2-1,it->argv[0]);
    else
       x_insert_pixrect(draw_win,x1+3,y2,it->argv[1]);
*/
   }
  else
  { if (!pressed) { xt++; yt++; }
    if (s[0] == '<' && s[1] == '<') 
      draw_left_menu_button(xt,yt+1,0,it->enabled);
    else
    if (s[0] == '>' && s[1] == '>')
      draw_right_menu_button(xt,yt+1,0,it->enabled);
    else
    { x_set_button_font(draw_win);
      int scut = it->shortcut;
      x_ctext(draw_win,xt,yt,s);

      if (scut >= 0) x_ctext_underline(draw_win,xt,yt,s,scut,scut);
      x_set_text_font(draw_win);
     }
   }

  x_flush_display();
}



void BASE_WINDOW::draw_menu_item(panel_item it, int pressed)
{ 
   char* s = it->label_str;

   int x1 = it->xcoord;
   int y1 = it->ycoord;

   int x2 = x1 + it->width + 1;
   //int x2 = x1 + it->width + panel_line_width;

   //int y2 = y1 + it->height + 1;
   int y2 = y1 + it->height + panel_line_width + 1;

   int xt = x1 + 4;

   //if (but_count <= menu_size) xt += (button_h+1);

   if (it == active_item) pressed = true;

   //int col  =  (pressed) ? blue : white; 
   int col  =  (pressed) ? blue2 : white; 

   x_set_color(draw_win,col);
   x_box(draw_win,x1,y1,x2,y2);

   int text_clr = color(col).text_color();
   if (text_clr == black) text_clr = 0x333333;

   if (s[0] == '@') 
   { text_clr = pressed ? white : blue;
     s = s+1;
    }

   if (it->enabled)
    { x_set_color(draw_win,black);
      int save_lw = x_set_line_width(draw_win,1);
      x_rect(draw_win,x1,y1,x2,y2);
      x_set_line_width(draw_win,save_lw);
      x_set_color(draw_win,text_clr);
     }
   else 
     x_set_color(draw_win,disable_color);

   x_set_fixed_font(draw_win);
   int dy = it->height - x_text_height(draw_win,"H");
   int yt = y1 + dy/2 + dy%2;
   x_text(draw_win,xt,yt,s);
   x_set_color(draw_win,black);
   x_set_text_font(draw_win);

   x_flush_display();
}



void BASE_WINDOW::draw_up_menu_button(int x, int y, int pressed, int enabled)
{ 
/*
  int w = color_h/2;
  if (pressed > 0)
    draw_d3_box(x-w,y-w,x+w,y+w,pressed,enabled);
*/

  //int d = (2*tw)/3;
  int d = int(0.6*tw + 0.5);

  int X[4];
  int Y[4];
  X[0] = x;
  Y[0] = y - d;
  X[1] = x + d;
  Y[1] = y + d;
  X[2] = x - d;
  Y[2] = y + d;
  X[3] = X[0];
  Y[3] = Y[0];

//x_set_color(draw_win, (pressed>0) ? press_color : panel_bg_color);
//x_fill_polygon(draw_win,4,X,Y);

  x_set_color(draw_win, panel_bg_color);
  x_box(draw_win,x-d,y-d,x+d,y+d);

  color c1 = shadow_color;
  color c2 = white;

  if (pressed)
  { c1 = white;
    c2 = shadow_color;
   }

  if (!enabled) c1 = c2 = disable_color;

  x_set_color(draw_win,c1);
  //x_polyline(draw_win,3,X,Y);
  x_line0(draw_win,X[0],Y[0],X[1],Y[1]);
  x_line0(draw_win,X[1],Y[1],X[2],Y[2]);

  x_set_color(draw_win,c2);
  x_line0(draw_win,X[2],Y[2],X[3],Y[3]);


  x_set_color(draw_win,black);
  x_flush_display();
}


void BASE_WINDOW::draw_down_menu_button(int x, int y, int pressed, int enabled)
{ 
/*
  int w = color_h/2;
  if (pressed > 0)
    draw_d3_box(x-w,y-w,x+w,y+w,pressed,enabled);
*/

  //int d = (2*tw)/3;
  int d = int(0.6*tw + 0.5);

  int X[4];
  int Y[4];
  X[0] = x;
  Y[0] = y + d;
  X[1] = x - d;
  Y[1] = y - d;
  X[2] = x + d;
  Y[2] = y - d;
  X[3] = X[0];
  Y[3] = Y[0];

//x_set_color(draw_win, (pressed>0) ? press_color : panel_bg_color);
//x_fill_polygon(draw_win,4,X,Y);

  x_set_color(draw_win, panel_bg_color);
  x_box(draw_win,x-d,y-d,x+d,y+d);
  

  color c1 = white;
  color c2 = shadow_color;

  if (pressed)
  { c1 = shadow_color;
    c2 = white;
   }

  if (!enabled) c1 = c2 = disable_color;

  x_set_color(draw_win,c1);
  x_line0(draw_win,X[0],Y[0],X[1],Y[1]);
  x_line0(draw_win,X[1],Y[1],X[2],Y[2]);
  x_pixel(draw_win,X[1],Y[1]);

  x_set_color(draw_win,c2);
  x_line0(draw_win,X[2],Y[2],X[3],Y[3]);

  x_set_color(draw_win,black);
  x_flush_display();
}



void BASE_WINDOW::draw_right_menu_button(int x, int y, int pressed, int enabled)
{ 
/*
  int w = color_h/2;
  if (pressed > 0)
    draw_d3_box(x-w,y-w,x+w,y+w,pressed,enabled);
*/

  int X[4];
  int Y[4];

  //int d = (2*tw)/3;
  int d = int(0.6*tw + 0.5);

  x += int(0.5*d);

  X[0] = x - d;
  Y[0] = y + d;
  X[1] = x + d;
  Y[1] = y;
  X[2] = x - d;
  Y[2] = y - d;
  X[3] = X[0];
  Y[3] = Y[0];


  color c1 = white;
  color c2 = shadow_color;

  if (!enabled) c1 = c2 = disable_color;

//x_set_color((pressed>0) ? press_color : panel_bg_color);
//x_fill_polygon(draw_win,4,X,Y);

  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,x-d,y-d,x+d,y+d);

  if (enabled)
  { x_set_color(draw_win,c1);
    x_polyline(draw_win,3,X+1,Y+1);
    x_set_color(draw_win,c2);
    x_line(draw_win,X[0]+1,Y[0]-1,X[1],Y[1]);
   }

  x_set_color(draw_win,black);
  x_flush_display();
}


void BASE_WINDOW::draw_left_menu_button(int x, int y, int pressed, int enabled)
{ 
/*
  int w = color_h/2;
  if (pressed > 0)
    draw_d3_box(x-w,y-w,x+w,y+w,pressed,enabled);
*/


  int X[4];
  int Y[4];

  //int d = (2*tw)/3;
  int d = int(0.6*tw + 0.5);

  x -= int(0.5*d);

  X[0] = x + d;
  Y[0] = y + d;
  X[1] = x - d;
  Y[1] = y;
  X[2] = x + d;
  Y[2] = y - d;
  X[3] = X[0];
  Y[3] = Y[0];


  color c1 = white;
  color c2 = shadow_color;

  if (!enabled) c1 = c2 = disable_color;

//x_set_color((pressed>0) ? press_color : panel_bg_color);
//x_fill_polygon(draw_win,4,X,Y);

  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,x-d,y-d,x+d,y+d);

  if (enabled)
  { x_set_color(draw_win,c1);
    x_polyline(draw_win,3,X,Y);
    x_set_color(draw_win,c2);
    x_line(draw_win,X[2],Y[2]+1,X[3],Y[3]);
   }

  x_set_color(draw_win,black);
  x_flush_display();
}




void BASE_WINDOW::make_menu_bar(int kind)
{ // collects all buttons inserted so far into a menu bar

  if (menu_bar_mode) return;

  if (kind == -1) // only set menu button style
  { for(int i=0; i<item_count; i++)
    { panel_item it = Item[i];
      if (it->kind == Button_Item && it->menu_but != it)
      { it->dat2 |= 0x1; 
        it->height = button_h;
       }
    }
    return;
  }

  for(int i=0; i<item_count; i++)
  { panel_item it = Item[i];
    if (it->menu_but != it)
    { it->menu_bar = 1; // goes into menu bar

      if (it->kind == Button_Item) 
      { // menu style buttons 
        //if (it->ref) /* only menu buttons */
          it->dat2 |= 0x1;

        //it->height = button_h;
        it->height = button_h - panel_line_width;
       }

      if (it->kind != Space_Item)
      { 
        if (it->argc == 0) 
        { x_set_button_font(draw_win);
          it->width = x_text_width(draw_win,it->label_str) + tw;
          x_set_text_font(draw_win);
         }
        else
         it->width += tw/2;
       }

      if (menu_bar_height < it->height) menu_bar_height = it->height;
     }

   }

   menu_bar_height += int(0.7*yoff);

//int min_bar_height = 32;
  int min_bar_height = int(0.175 * screen_dpi());
  if (menu_bar_height < min_bar_height) menu_bar_height = min_bar_height;


  if (kind > 0)
  { menu_bar_mode = kind;
    button_w = 20; 
    button_d = th/2;
    button_h = th + 3;
   }
}


void BASE_WINDOW::place_panel_items()
{
/*
  cout << "PLACE PANEL ITEMS" << endl;
  cout << "but_count  = " << but_count << endl;
  cout << "item_count = " << item_count << endl;
*/

  x_save_font(draw_win);

  x_set_fixed_font(draw_win);
  twf = x_text_width(draw_win,"HHHHHHHHHHHHHHHHHHHH")/20.0;

  bool is_menu = (p_win && item_count == but_count);

  // adjust panel width

  if (is_menu)  // menu
  { xoff  = 0;
    yoff  = 0;

    //yskip = button_h+2;
    yskip = button_h + 2*panel_line_width;

    //panel_width = buts_per_line*(button_w+1) + 3;
    panel_width = buts_per_line*(button_w + panel_line_width) + 
                  3*panel_line_width;

    center_button_label = 0;

    if (menu_style == 1) 
    { //button_h = string_h + 4;
      yskip = int(1.2*button_h);
     }

    if (menu_style == 2) 
    { if (but_count > menu_size) panel_width += scrollbar_w;
      panel_width--;
      yskip = int(1.1*button_h);
      yoff = -2;
     }
   }
  else
  {  // not a menu
    int w = x_text_width(draw_win,default_frame_label);

    if (panel_width == -1 && item_count == but_count) // only buttons
    { if (w > 300) w = 300;
      if (w > panel_width) panel_width = w;
     }
    //else if (panel_width < 0) panel_width = 50;

    if (menu_bar_mode) xoff = 3;
   }

  if (buts_per_line > but_count) buts_per_line = but_count;



  // set width for all items and adjust panel width

  int max_w = 0;
  int max_choice_width = 0;
  panel_item max_choice_it = 0;

  int menu_bar_items = 0;
  int non_button_items = 0;

  int i;

  for(i = 0; i<item_count; i++)
  { panel_item it = Item[i];

    if (it->menu_bar) 
      menu_bar_items++;
    else
      if (it->kind != Button_Item && it->kind != Text_Item) non_button_items++;

    int lw = get_item_label_width(it);

    switch (it->kind)
    {
      case  Button_Item: 
             if (it->menu_but != it && it->menu_bar == 0) 
                it->width  = button_w;
             break;

      case  Text_Item:
             { it->width = 0;
               if (it->argc > 0)
               { if ((it->argv[0][0]&0xf0) == 0x10) x_set_bold_font(draw_win);
                 if ((it->argv[0][0]&0xf0) == 0x20) x_set_fixed_font(draw_win);
                }
               int tw = 0;
               for(int j=0; j<it->argc; j++) 
               { const char* s = it->argv[j];
                 if (j == 0) s++;
                 int w = x_text_width(draw_win,s);
                 if (w == 0)
                 { tw = 0;;
                   continue;
                  }
                 tw += w;
                 if (tw > it->width) it->width = tw;
                }
               x_set_text_font(draw_win);

               it->width += xoff;

               int tmax_w = 800;

               if (win_parent) {
                   int p_width = win_parent->window_width - 100;
                   if (p_width < tmax_w) tmax_w = p_width;
               }

               if (it->width > tmax_w) it->width =  tmax_w;
               break;
              }

      case Bool_Item:
             //it->width = label_w + color_h;
             it->width = lw + color_h;
             break;

      case Choice_Item:
      case Choice_Mult_Item: 
           { 
             int bw = get_item_button_width(it);

             int w = it->argc * bw;

             if (w > max_choice_width) 
             { max_choice_it = it;
               max_choice_width = w; 
              }
             it->width = lw + w;

             break;
            }

      case Color_Item:
             //it->width = lw + 18*(color_h+2)-2;
             it->width = 18*(color_h + 2*panel_line_width) - 2*panel_line_width;
             break;

      case Bitmap_Choice_Item:
      case Bitmap_Choice_Mult_Item:
             //it->width = lw + it->argc * (it->dat1+3) - 3;
             it->width = lw + it->argc * (it->dat1+3);
             break;

      case Slider_Item:
           //it->width = lw + slider_w;
             it->width = lw + get_default_width(it);
             break;

      case String_Item:
      case String_Menu_Item:
           //it->width = lw + string_w;
             it->width = lw + get_default_width(it);
             break;

      case Float_Item:
      case Int_Item:
           //it->width = lw + number_w;
             it->width = lw + get_default_width(it);
             break;
  
     }

    if (it->width > max_w) max_w = it->width;
  }


  // left_margin = xoff   right_margin = 1.5*xoff

  int w = max_w + int(2.5*xoff); 
  if (w > panel_width) panel_width = w;

  if (!is_menu && !menu_bar_mode && panel_width < min_panel_width) {
     panel_width = min_panel_width;
  }


  // adjust width of string and slider items

  if (slider_w < max_choice_width)
  { slider_w = max_choice_width;
    string_w = max_choice_width;
    number_w = max_choice_width;
   }

  for(i = 0; i<item_count; i++)
  { panel_item it = Item[i];
    int lw = get_item_label_width(it);

    switch (it->kind) {
      case String_Item:
      case String_Menu_Item: {
              x_set_fixed_font(draw_win);
              it->width = lw + get_default_width(it);
              it->dat2 = it->dat1 + int(0.5 + get_default_width(it)/twf) - 1;
              break;
      }

      case Slider_Item:
      case Float_Item:
      case Int_Item:
            //it->width = lw + (it->item_width > 0) ? it->item_width : string_w;
              it->width = lw + get_default_width(it);
              break;
      }
   }


  if (max_choice_it && slider_w > max_choice_width
                    && slider_w - choice_w < max_choice_width)
  { 
    int dw = (slider_w - max_choice_width)/max_choice_it->argc;
    choice_w += dw;
    for(i = 0; i<item_count; i++)
    { panel_item it = Item[i];
      int lw = get_item_label_width(it);

      switch (it->kind) {
        case Choice_Item:
        case Choice_Mult_Item:
                it->width = lw + get_item_button_width(it)*it->argc - 3;
                //it->width = lw + choice_w*it->argc - 3;
                break;
      
       }
     }
   }


  //  button layout

  int bxoff = xoff;      // left and right button boundary space
  int bskip = button_d;  // horizontal space between buttons

  if (is_menu)
    bskip = 1;
  else
    if (menu_bar_mode)
      bxoff = xoff;
    else
    { int bw = button_w + bskip;

      //int w = but_count*bw+xoff-bskip;
      int w = but_count*bw+2*xoff-bskip;

    //if (buts_per_line == 0 && w < panel_width) buts_per_line = but_count;
      if (w < panel_width) buts_per_line = but_count;

      if (buts_per_line > 0) // increase panel width if necessary 
      { //w = buts_per_line*bw+xoff-bskip;
        w = buts_per_line*bw+2*xoff-bskip;
        if (w > panel_width) panel_width = w;
        // center buttons
        bxoff = (panel_width - (buts_per_line * bw - bskip))/2;
       }
     }


  // place items

  // menu bar

  if (button_h < string_h+1) button_h = string_h+1;


  if (menu_bar_mode == 1 || menu_bar_mode > 2) 
  { // use user defined ordering of buttons

    int left = menu_bar_mode;
    int right = menu_bar_items - left;

/*
    if (menu_bar_mode == 1) 
    { left = menu_bar_items;
      right = 0;
     }
*/

    int x = xoff + 4;

    for(int i=0; i<item_count; i++)
    { panel_item it = Item[i];
      int lw = get_item_label_width(it);

      if (!it->menu_bar) continue;
      
      if (left-- == 0) break;

      it->xcoord = x;
      it->ycoord = int(1.0 + menu_bar_height - it->height)/2;
      it->ycoord += 3;

      if (it->menu_but)
      { // string menu button
        panel_item p = it->menu_but;
        p->xcoord = it->xcoord + lw - p->width - 6;
        p->ycoord = it->ycoord + 2;
       }

      x += (it->width + bskip);
    }

    int x_left = x;


    x = panel_width - bskip;

    for(int i=item_count-1; i>=0; i--)
    { panel_item it = Item[i];
      int lw = get_item_label_width(it);

      if (!it->menu_bar) continue;

      if (right-- == 0) break;

      x -= (it->width + bskip);

      it->xcoord = x;

      if (x < x_left) // hide overlapping items
        it->ycoord =  -50;
      else
      { it->ycoord = int(1.0 + menu_bar_height - it->height)/2;
        it->ycoord += 3;

        if (it->menu_but)
        { // string menu button
          panel_item p = it->menu_but;
          p->xcoord = it->xcoord + lw - p->width - 6;
          p->ycoord = it->ycoord + 2;
         }
       }

      }
   }


  if (menu_bar_mode == 2) 
  { // place menu buttons and non-button itmes left of normal buttons

    // non-menu buttons start placing from right border

    int x_right = panel_width;
    for(int i=item_count-1; i>=0; i--)
    { panel_item it = Item[i];
      if (it->menu_bar && it->ref == 0)
      { x_right -= (it->width + bskip);
        it->xcoord = x_right;
        it->ycoord = int(1.0 + menu_bar_height - it->height)/2;
        it->ycoord += 3;
       }
     }

    // menu buttons or other items (start placing from left border)

    int x_left = xoff + 5;

    for(int i=0; i<item_count; i++)
    { panel_item it = Item[i];
      if (it->menu_bar && it->ref)
      { it->xcoord = x_left;
        it->ycoord = int(1.0 + menu_bar_height - it->height)/2;
        it->ycoord += 3;
        x_left += it->width;
        if (x_left > x_right) it->xcoord = panel_width+1; // overlap (hide)
        x_left += bskip;
      }
     }

  }


  int cur_xcoord = panel_width+1;

  int cur_ycoord = (2*yoff)/3;

/*
  if (item_count > 0 && Item[0]->kind == Text_Item) cur_ycoord = yoff;
  if (cur_ycoord < 0) cur_ycoord = -1;
*/

  int cur_skip   = 0;

  int non_bar_items = 0;
  int non_bar_buttons = 0;

  for(int i = 0; i<item_count; i++)
  { panel_item it = Item[i];
    if ((!menu_bar_mode || it->menu_bar == 0) && it->menu_but != it)
    {  non_bar_items++;
       if (it->kind == Button_Item) non_bar_buttons++;
     }
   }

  if (menu_bar_mode) 
  { //cur_ycoord = menu_bar_height + yoff/2;
    cur_ycoord = menu_bar_height;

//  if (non_bar_items) cur_ycoord += 2*yoff;
    if (non_bar_items) cur_ycoord += yoff;

/*
    if (non_bar_buttons) cur_ycoord += 3;
    if (item_count > but_count) cur_ycoord += 2*yoff;
    if (non_bar_items || but_count == 0) cur_ycoord += yoff;
*/
   }


  for(int i = 0; i<item_count; i++)
  { panel_item it = Item[i];

    if (it->menu_bar) continue;

    if (it->kind == Button_Item)  continue;
    if (it->kind == Space_Item)   continue;
    if (it->kind == Fill_Item)    continue;
    if (it->kind == Newline_Item) continue;

    int lw = get_item_label_width(it);

    if ( it ->kind == Text_Item ||
        (cur_xcoord > xoff && cur_xcoord + it->width > panel_width) )
    { 
      // newline
      cur_xcoord = xoff; 
      cur_ycoord += cur_skip;

      it->xcoord = cur_xcoord;

      switch (it->kind) { 
        case Bitmap_Choice_Item:
        case Bitmap_Choice_Mult_Item:
              cur_skip = it->dat2 + th/2;
              if (cur_skip < yskip) cur_skip = yskip;
              cur_ycoord += th/4;
              break;
        case Text_Item: 
              if (cur_skip == yskip) cur_ycoord += th/2;
              cur_skip = draw_text_item(it,0);
              break;
        default: 
              cur_skip = yskip;
              break;
        }
     }

    it->xcoord = cur_xcoord;
    it->ycoord = cur_ycoord + (yskip - it->height)/2;

    if (it->kind == Space_Item)
       cur_xcoord += it->width;
    else
    if (it->kind == Bool_Item)
      //cur_xcoord += (it->width + 2*xoff);
      cur_xcoord += (it->width + xoff);
    else
      if (it->kind == Text_Item)
        cur_xcoord += panel_width;
      else
        //cur_xcoord += (it->width + 2*xoff);
        cur_xcoord += (it->width + xoff);

    if (it->menu_but)
    { panel_item p = it->menu_but;
      p->xcoord = it->xcoord + lw - p->width - 6;
      p->ycoord = it->ycoord + 2;
     }
   }

  // place buttons

  int x = cur_xcoord;
  int y = cur_ycoord - yskip;
  if (non_bar_items) y += cur_skip;


  if (auto_button_layout)
  { x = panel_width + 1;
    if (non_button_items > 0) y += (yoff-1);
   }


  if (is_menu)
  { x = 0;
    y = 0;
    if (menu_style == 1) {
      y = 3; // 3px top margin
    }
   }


  for(int i=0; i<item_count; i++)
  { panel_item it = Item[i];

    if (it->menu_bar) continue;

    switch (it->kind) {

     case Newline_Item: x = bxoff;
                        y += (yskip+2);
                        it->ycoord = y-1;
                        break;

     case Space_Item:   if (it->width > 0) x += it->width;
                        break;


     case Fill_Item:  { if (x > bxoff && x+it->width > panel_width-bxoff) 
                        { x = bxoff;
                          y += yskip;
                         }
                        int x1 = panel_width - button_w - bxoff;
                        if (x1 > x) x = x1;
                        break;
                       }

     case Button_Item: { if (it->menu_but == it || 
                             (it->menu_bar && menu_bar_mode)) continue;
                         if (x > bxoff && x+it->width > panel_width-bxoff) 
                         { x = bxoff;
                           y += yskip;
                          }
                         it->height = button_h;
                         it->xcoord = x;
                         it->ycoord = y + (yskip - it->height)/2;

                         x += (it->width + bskip);
                         break;
                       }
     }
  }


  y += button_h;

  if (menu_bar_mode)  {
    y += int(0.75*button_h);
    if (non_bar_items > 0) y += int(0.5*button_h);
  }
  else
  { if (is_menu)  
      y += button_h/4;
    else
    { 
     if (but_count == 0)  
         y += button_h/2;
      else
         y += button_h;

      //if (win_parent == 0) y += button_h/2;
     }
   }

/*
  if (y > panel_height) panel_height = y;
*/

  // set panel height - except for scrollbar (menu_style == -1)
  if (menu_style != -1) panel_height = y; 


  if (menu_style == 2)  
  { if (but_count <= menu_size) 
      panel_height = but_count*yskip + 2;
    else
    { // use scrollbar (if but_count > menu_size)
      panel_height = menu_size*yskip + 2;
      if (active_item)
      { // center active item
        int j = (menu_size/2 - active_item->index);
        if (menu_size%2 == 0) j--;
        if (j > 0) j = 0;
        if (j < menu_size-item_count) j = menu_size-item_count;
        for(int i=0; i< item_count; i++) Item[i]->ycoord += j*yskip;
       }
     }

  }

  x_restore_font(draw_win);

}


void BASE_WINDOW::draw_item(panel_item it)
{
  // redraw item it 

  int          save_lw = x_set_line_width(draw_win,panel_line_width);
  line_style   save_ls = x_set_line_style(draw_win,solid);
  text_mode    save_tm = x_set_text_mode(draw_win,transparent);
  drawing_mode save_mo = x_set_mode(draw_win,src_mode);

  clipping(0);

  x_save_font(draw_win);
  x_set_color(draw_win,black);

    set_tooltip(it);

    switch (it->kind) {


    case Newline_Item:
        { if (menu_style == 1) draw_separator(it);
          break;
         }

    case Button_Item:
        { draw_button(it,it == last_sel_button);
          break;
         }

    case Text_Item:
        { draw_text_item(it,draw_win);
          break;
         }

    case Choice_Item:
    case Choice_Mult_Item:
        { draw_choice_item(it);
          break;
         }

    case Bool_Item:
        { draw_bool_item(it);
          break;
         }

    case Color_Item:
        { draw_color_item(it);
          break;
         }

    case Bitmap_Choice_Item:
    case Bitmap_Choice_Mult_Item:
        { draw_bitmap_choice_item(it);
          break;
         }

    case Slider_Item:
        { draw_slider_item(it,0);
          break;
         }

    case Int_Item:
        { //if (active_item == 0) active_item = it;
          delete[] it->data_str;
          it->data_str = make_string(*(int*)it->ref);
          draw_string_item(it);
          break;
         }
 
    case Float_Item:
        { //if (active_item == 0) active_item = it;
          delete[] it->data_str;
          it->data_str = make_string(*(double*)it->ref,panel_float_format);
          draw_string_item(it);
          break;
         }
  
    case String_Item:
    case String_Menu_Item:
        { it->data_str = access_str(it->ref);
          //if (active_item == 0) active_item = it;
          if (it->offset < 0)
             it->offset = str_length(it->data_str);
          x_set_fixed_font(draw_win);
          it->dat1 = 0;
          it->dat2 = int(0.5 + get_default_width(it)/twf) - 1;
          draw_string_item(it);
          break;
         }
   }

   if (it->menu_but) draw_button(it->menu_but,0);

 clipping(2);

 x_set_line_width(draw_win,save_lw);
 x_set_line_style(draw_win,save_ls);
 x_set_mode(draw_win,save_mo);
 x_set_text_mode(draw_win,save_tm);

 x_restore_font(draw_win);

 x_flush_display();
}




void BASE_WINDOW::draw_panel() {
   draw_panel(0,0,panel_width,panel_height);
}

void BASE_WINDOW::draw_panel(int x0, int y0, int x1, int y1)
{
  if (!is_open() || is_minimized()) return;

  int          save_lw    = x_set_line_width(draw_win,panel_line_width);
  line_style   save_style = x_set_line_style(draw_win,solid);
  drawing_mode save_mode  = x_set_mode(draw_win,src_mode);

  bool is_menu = (panel_height >= window_height && item_count == but_count);

  if (is_menu && menu_style == 1) x_set_bg_color(draw_win,shadow_color);

  clipping(0);
  start_buffering();
  clipping(0);

  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,0,0,window_width,panel_height-1);

  if (is_menu)
  { 
    x_set_color(draw_win,white);

    x_line(draw_win,0,0,window_width,0);
    x_line(draw_win,0,0,0,window_height);

    x_set_color(draw_win,shadow_color);

    x_line(draw_win,0,window_height-1,window_width,window_height-1);
    x_line(draw_win,window_width-1,0,window_width-1,window_height);
   }


  if (window_height > panel_height)
  { 
    x_set_color(draw_win,shadow_color);
    x_line(draw_win,0,panel_height-1,window_width-1,panel_height-1);

/*
  //int y = yoff-2;
  //int y = yoff-5;
    int y = yoff-6;
    x_line(draw_win,window_width-2,y,window_width-2,panel_height);

    x_set_color(draw_win,white);
    x_line(draw_win,0,y,0,panel_height);
    x_line(draw_win,0,y,window_width-1,y);
*/

  }

    if (menu_bar_mode)
    { int non_bar_items = 0;
      for(int i = 0; i<item_count; i++)
      { panel_item it = Item[i];
        if (it->menu_bar == 0) non_bar_items++;
       }



      if (non_bar_items)
      { int y = menu_bar_height + 2;
        x_set_color(draw_win,shadow_color);
        x_line(draw_win,0,y,window_width,y);
        x_set_color(draw_win,white);
        x_line(draw_win,0,y+1,window_width-1,y+1);
       }
/*
      else
      { int x0 = xoff;
        int y0 = yoff-1;
        int y1 = y0 + menu_bar_height+1;
        int x1 = window_width-xoff;

        x_set_color(draw_win,white);
        x_line(draw_win,x0,y0,x1,y0);
        x_line(draw_win,x0,y0,x0,y1);
        x_set_color(draw_win,shadow_color);
        x_line(draw_win,x0+1,y1,x1+1,y1);
        x_line(draw_win,x1,y1,x1,y0);
       }
*/
    }

  // draw all items
  for(int i=0; i<item_count; i++) draw_item(Item[i]);

  flush_pixels(x0,y0,x1,y1);

  stop_buffering();
  clipping(2); 

  x_set_mode(draw_win,save_mode);
  x_set_line_style(draw_win,save_style);
  x_set_line_width(draw_win,save_lw);
}


void BASE_WINDOW::redraw_panel(int x0, int y0, int x1, int y1)
{ if (item_count == 0) return;
  if (!clear_on_resize && y1 > panel_height) y1 = panel_height;
  draw_panel(x0,y0,x1,y1);
//if (buf_level > 0) cerr << "REDRAW_PANEL: LEVEL = " << buf_level << endl;
}

void BASE_WINDOW::redraw_panel()
{ redraw_panel(0,0,panel_width,panel_height); }

void BASE_WINDOW::clear_panel()
{ for(int i=0; i<item_count; i++) delete Item[i];
  panel_init();
  if (is_open()) resize(width(),height());
}

void BASE_WINDOW::clear_item(panel_item it)
{ int d = 2; 
  int w = get_default_width(it) + get_item_label_width(it);
  if (it->kind == Button_Item) w = it->width;

  int x0 = it->xcoord - d;
  int x1 = it->xcoord + w + d;
  int y0 = it->ycoord - d;
  int y1 = it->ycoord + it->height + d;

  start_buffering();
  clipping(1);
  x_set_color(draw_win,panel_bg_color);
  x_box(draw_win,x0,y0,x1,y1);
}


void BASE_WINDOW::flush_item(panel_item it)
{ 
  assert(is_buffering() && buf_level > 0);

  int d = 2; 
  int w = get_default_width(it) + get_item_label_width(it);

  if (it->kind == Button_Item) w = it->width;

  int x0 = it->xcoord - d;
  int x1 = it->xcoord + w + d;
  int y0 = it->ycoord - d;
  int y1 = it->ycoord + it->height + d;

  flush_pixels(x0,y0,x1,y1);

  stop_buffering();
  clipping(2);
}



    
void BASE_WINDOW::open_sub_window(BASE_WINDOW* wp, int x, int y)
{ 
  wp->last_sel_button = 0;

  if (wp->is_open()) return;

  wp->p_win  = this;
  wp->p_root = p_root;

  wp->menu_style = 1;
  wp->place_panel_items();
  active_window = wp;

#if defined(__APPLE__)
  wp->display(x,y,this);
  x_set_border_width(wp->draw_win,0);
#else
  x_set_border_width(wp->draw_win,0);
  wp->display(x,y,this);
#endif
  
  x_grab_pointer(wp->draw_win);
  x_set_focus(wp->draw_win);

  wp->last_sel_button = 0;
  wp->menu_mode = 1;
}


void BASE_WINDOW::open_sub_panel(panel_item it)
{

  BASE_WINDOW* wp = (BASE_WINDOW*)it->ref;

  wp->last_sel_button = 0;

  if (wp->is_open()) return;

  bool is_menu = (buts_per_line == 1 && item_count == but_count);

  // find "owner" p of (menu)button
  int j = 0;
  while (j<item_count && Item[j]->menu_but != it) j++;
  panel_item owner = (j < item_count) ? Item[j] : 0;


  //BASE_WINDOW* wpp = p_root;

  BASE_WINDOW* wpp = this;
  while (wpp->win_parent && wpp->win_parent != wpp) wpp = wpp->win_parent;
  wp->p_win  = this;

  //wp->p_root = p_root; // ?????
  wp->p_root = wpp;


  //if (wp->menu_style == 0) 
  { if (wp->owner_item == 0 || wp->owner_item->menu_but != wp->owner_item)
      wp->menu_style = p_root->menu_style;
   }

  int border_w = 0;
  //int border_w = 1;

  int x = 0;
  int y = 0;

  if (is_menu)
    { x = window_width + 1;
      y = it->ycoord + 1;
      if (wp->menu_style == 1) { x--; y++; }
      wp->hotx1 = -window_width;
      wp->hotx2 = 0;
      wp->hoty1 = 0;
      wp->hoty2 = yskip;
    }
  else
   { x = it->xcoord;
     y = it->ycoord + it->height + 2;
   //y = it->ycoord + (yskip+it->height)/2 + 2;

     wp->hotx1 = 0;
     wp->hotx2 = it->width;
     wp->hoty1 = it->ycoord - y;
     wp->hoty2 = 0;

     if (wp->menu_style == 2) // scroll box menu
     { wp->yskip = 0;
       wp->button_w = get_default_width(owner) + 2;
/*
       if (wp->but_count > wp->menu_size) {
         // scrollbar
         wp->button_w -= wp->button_h+1;
       }
*/

       // find active item
       wp->active_item = 0;
       for(j = 0; j < wp->item_count; j++) {
         if (strcmp(wp->Item[j]->label_str,owner->data_str) == 0)
         { wp->active_item = wp->Item[j];
           break;
          }
       }

       int lw = get_item_label_width(owner);
       x = owner->xcoord + lw;
       //y += 2;

       //border_w = 1;
      }
    }


    if (owner && owner->kind==String_Menu_Item)
      activate_string_item(owner,panel_width);

    x_window_to_screen(draw_win,x,y);

    int x1 = x;
    int y1 = y;
    x_screen_to_window(wpp->draw_win,x1,y1);

    wp->place_panel_items();

    if (wpp->window_height < y1 + wp->panel_height + 2 ||
        wpp->window_width  < x1 + wp->panel_width + 2)
    {
      wpp = wp;

      int disp_w, disp_h, dpi;
      x_display_info(disp_w,disp_h,dpi);

      int dx = disp_w - (x + wp->panel_width);
      int dy = disp_h - (y + wp->panel_height);

      if (dx < 0)
      { x += dx;
        wp->hotx1 -= dx;
        wp->hotx2 -= dx;
       }

      if (dy < 0)
      { y += dy;
        wp->hoty1 -= dy;
        wp->hoty2 -= dy;
       }
    }
    else
    { x = x1;
      y = y1;
     }

  active_window = wp;


#if defined(__APPLE__)
//set border width after display (because of a problem with MacOS/XQuartz)
  wp->display(x,y,wpp);
  x_set_border_width(wp->draw_win,border_w);
#else
  x_set_border_width(wp->draw_win,border_w);
  wp->display(x,y,wpp);
#endif


  if (wp->menu_style == 2)
    { double sz = double(wp->menu_size)/wp->item_count;
      if (sz < 1)
      { int  h = wp->item_count * wp->yskip;
        double pos = double(wp->Item[0]->ycoord)/(wp->panel_height - h);
        wp->open_scrollbar(scroll_up_action,scroll_down_action,
                           scroll_drag_action, sz,pos);
       }
     }


  x_grab_pointer(wp->draw_win);
  x_set_focus(wp->draw_win);



/*
  { // skip pending events from wpp
    int e,w,v1,v2,,x,y;
    unsigned long t;
    do { e = x_get_next_event(w,x,y,v1,v2,t,-1);
    } while (e != no_event && wp->draw_win != w);
    if (e != no_event) x_put_back_event();
  }
*/

  wp->last_sel_button = 0;
  wp->menu_mode = 1;
}


    
    
void BASE_WINDOW::close_sub_window(BASE_WINDOW* wp)
{
/*
cout << "close_sub_window: this =  " << draw_win 
     << " menu = " << wp->draw_win << endl;
*/

  wp->last_sel_button = 0;
  wp->menu_mode = 0;

  if (wp->is_open())
  { 
    wp->close();
    int e,w,v1,v2,x,y;
    unsigned long t;

    // read all remaining events 
    while ((e = x_get_next_event(w,x,y,v1,v2,t,-1)) != no_event)
    { if (e == exposure_event && w == wp->p_win->draw_win)
      { x_put_back_event();
        event_handler(wp->p_win,0);
       }
    }

    wp->close();
    wp = wp->p_win;

    if (wp == 0) return;

/*
// NEW 
if (wp->p_win) 
{ // grab pointer if not root window (otherwise x11 hangs ?)
  x_grab_pointer(wp->draw_win);
}
*/

    x_set_focus(wp->draw_win);

    wp->redraw_panel();

    if (!x_window_bits_saved(wp->draw_win) && win_parent != this)
       { while ((e = x_get_next_event(w,x,y,v1,v2,t)) != exposure_event) {
         //cout << string("event1: w = %d, e = %s",w,event_name[e]) << endl;
         }
         if (w == wp->draw_win)
         { x_put_back_event();
           event_handler(wp,0);
          }
        }
    else
       while ((e=x_get_next_event(w,x,y,v1,v2,t,-1)) != no_event)
       { //cout << string("event2: w = %d e = %s",w,event_name[e],x,y) << endl;
         if (e == exposure_event && w == wp->draw_win)
         { x_put_back_event();
           event_handler(wp,0);
          }
        }

  }
}


void BASE_WINDOW::scroll(int steps)
{ 
  panel_item p1 = Item[0];
  panel_item p2 = Item[item_count-1];

  int dy = steps * yskip;

  if (p1->ycoord + dy > button_h/2) return;
  if (p2->ycoord + dy < panel_height-3*button_h/2) return;

  for(int i=0; i<item_count; i++) Item[i]->ycoord += dy;

  start_buffering();
  clipping(0);
  draw_panel();
  x_flush_buffer(draw_win,0,0,panel_width,panel_height);
  stop_buffering();

  double f = double(p1->ycoord)/(panel_height - item_count*yskip);
  set_scrollbar_pos(f,-1);
}





int BASE_WINDOW::panel_event_handler(int w, int k, int b, int x, int y, 
                                                         unsigned long t)
{
/*
   printf("panel event: w = %d  k = %d  b = %d  x = %d  y = %d   %s\n",
        w,k,b,x,y,event_name[k]);
   fflush(stdout);
*/

  if (k == button_press_event && b == 4)
  { // scroll wheel up
    k = key_press_event;
    b = KEY_UP;
   }

  if (k == button_press_event && b == 5)
  { // scroll wheel down
    k = key_press_event;
    b = KEY_DOWN;
   }

  if (k == button_press_event && y > panel_height)
  { // click outside of panel area
    // cout << "click outside" << endl;

    active_item = 0;

    // close sub windows 
    if (last_sel_button)
    { //cout << "close sub-windows" << endl;
      panel_item lsb = last_sel_button;
      last_sel_button = 0;
      draw_button(lsb,0);
      if (lsb->ref)
      { close_sub_window((BASE_WINDOW*)lsb->ref);
        active_window = this;
       }
     }

    return -1;
  }

  int          save_lw = x_set_line_width(draw_win,panel_line_width);
  line_style   save_ls = x_set_line_style(draw_win,solid);
  text_mode    save_tm = x_set_text_mode(draw_win,transparent);
  drawing_mode save_mo = x_set_mode(draw_win,src_mode);
  clipping(1);

  if (w != draw_win) 
  { x = -1;
    y = -1;
   }

  int but = -1;

  panel_item it = 0;

  if (menu_style == 2)
  { int i = 0;
    int max_i = item_count-1;

    if (active_item) 
       i = active_item->index;
    else
      { if (b == KEY_DOWN) i = -1; 
        if (b == KEY_UP)   i = max_i + 1;  
       }

    if (k == key_press_event)
    {
      start_buffering();

      switch (b) {

      case KEY_UP   : if (i == 0) break;
                      active_item = Item[i-1];
                      if (active_item->ycoord < 0) 
                         scroll(+1);
                      else
                         scroll(0);
                      break;

      case KEY_DOWN : if (i == max_i) break;
                      active_item = Item[i+1];
                      if (active_item->ycoord+button_h > panel_height) 
                         scroll(-1);
                      else
                         scroll(0);
                      break;

      case KEY_HOME : scroll(-item_count);
                      break;

      case KEY_END  : scroll(+item_count);
                      break;
      }

      stop_buffering();

      if ( b == KEY_RETURN && active_item)
      { // simulate button release on last selected button
        x = active_item->xcoord + button_w/2;
        y = active_item->ycoord + button_h/2;
        last_sel_button = active_item;
        k = button_release_event;
       }
      else return -1;
     }

   }


  if (k == key_press_event && b != KEY_RETURN && active_item != 0) 
     { // input for active string item
        if (b == KEY_ESCAPE)
        { // inactivate
          active_item = 0; 
          redraw_panel();
         }
        else
        if (b == KEY_TAB || b == KEY_DOWN || b == KEY_UP) {
        if (b == KEY_TAB /* || KEY_BACKTAB*/)
           { int i = active_item->index;
             int ki;
             do { if (b == KEY_TAB /*|| b == KEY_DOWN*/)
                    { if (++i >= item_count) i = 0; }
                  else
                    { if (--i < 0) i = item_count-1; }
                  ki =  Item[i]->kind; 
                 }
             while (!Item[i]->enabled ||
                    (ki != String_Item && ki != String_Menu_Item && 
                     ki != Int_Item    && ki != Float_Item));
             activate_string_item(Item[i],panel_width);
            }
         }
         else // character
            { it = active_item;
              int lw = get_item_label_width(it);
              //x = it->xcoord + string_w + lw;
              x = it->xcoord + get_default_width(it) + lw;
              y = it->ycoord;
              x_put_back_event(); // event will be handled by p/anel_text_edit
             }
       }
    else
      { 
        if (focus_button && k == key_press_event && b == KEY_RETURN) 
        { panel_item it = focus_button;
          draw_button(it,1);
          { // wait for key release event
            int w,v1,v2,x,y;
            unsigned long t;
            while (x_get_next_event(w,x,y,v1,v2,t) != key_release_event);
           }
          // simulate button release on focus button
          x = it->xcoord + button_w/2;
          y = it->ycoord + button_h/2;
          last_sel_button = it;
          k = button_release_event;
        }

        // search for selected item

        int i;
        for(i=0; i<item_count; i++) 
        { it = Item[i];
          if (!it->enabled) continue;
          int x1 = it->xcoord;
          int x2 = x1 + it->width; 

          int y1 = it->ycoord; 
          int y2 = y1 + it->height;

          int lw = get_item_label_width(it);

          if (it->kind != Button_Item) x1 += lw;

          if (it->kind == Color_Item) x2 += it->width; 

          if (it->kind == Slider_Item) 
          { x1 -= button_h;
            x2 += button_h;
           }

          if (it->kind == String_Item || it->kind == String_Menu_Item) 
            y2 += 5;

          if (menu_mode) 
          { y1 -= 2;
            y2 += 2;
           }
          if (x < x1 || x > x2 || y < y1 || y > y2) continue;
          break;
         }


        if (i == item_count)  it = 0;  // no item selected

        if (it && k == motion_event && it->kind == Button_Item
               && ((it->dat2 & 0x1) || it->help_str || 
                   (it->menu_but==it && str_length(it->label_str) == 0)))
          { active_button_win = this;
            active_button = it;
           }
        else
          active_button = 0;


        //????????????????????
        //if (k != button_press_event && !menu_mode) it = 0;
        //????????????????????

        if (k != button_press_event) it = 0;

        if (k == button_press_event && last_sel_button
                                    && (it==0 || it->kind!=Button_Item)) 
        { // buttons are handled later
          // close open sub window (if not selected again)
          panel_item lsb = last_sel_button;
          last_sel_button = 0;
          draw_button(lsb,0);
          if (lsb->ref)
          { close_sub_window((BASE_WINDOW*)lsb->ref);
            active_window = this;
           }
         }


       // repeaters, e.g. scrolling (it->offset > 0)

       if (it && it->kind == Button_Item && it->offset > 0 && 
           (it->action || it->action_ptr) && k == button_press_event)
       { int v1,v2;
         unsigned long t;
         draw_button(it,1);
         last_sel_button = it;
         call_window = this;
         call_item = it;
         clipping(2);
	 
	 if (it->action_ptr) {
	   it->action_ptr->set_params(this,NULL,it->dat1);
	   it->action_ptr->operator()();
	 }
         else it->action(it->dat1);
	 
         clipping(1);
         int t1 = 500;
         int t2 = it->offset;
         // return if button released after t1 msec
         if (x_get_next_event(w,x,y,v1,v2,t,t1) != button_release_event)
         { // otherwise execute action every t2 msec until button released
           draw_button(it,1);
           while (x_get_next_event(w,x,y,v1,v2,t,t2)!=button_release_event)
           { if (it->ref == 0) draw_button(it,0);
             clipping(2);
	     
	     if (it->action_ptr) {
	       it->action_ptr->set_params(this,NULL,it->dat1); 
	       it->action_ptr->operator()();
	     }
             else it->action(it->dat1);
	     
             clipping(1);
             if (it->ref == 0) draw_button(it,1);
            }
          }
         last_sel_button = 0;
         clipping(1);
         draw_button(it,0);
         clipping(2);
         return -1;
       }


       // check special buttons (scrollbar slider: it->ref == this)

       if (it && it->kind == Button_Item && it->ref == this
                                         && k == button_press_event)
       { int e,v1,v2;
         unsigned long t;
         draw_button(it,1);
         last_sel_button = it;

         if (it->action || it->action_ptr) 
         { call_window = this;
           call_item = it;
           //int but_dy = it->ycoord + (yskip - it->height)/2 - y;
           int but_dy = it->ycoord - y;

           int yoff1 = button_h/2 + 4*panel_line_width;
           int yoff2 = it->height + yoff1 + panel_line_width;

           int y0 = 0;

           BASE_WINDOW* owner = (BASE_WINDOW*)get_inf();

           if (owner->p_win == 0)
               x_grab_pointer(draw_win);
	       
           if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,-1);
	     it->action_ptr->operator()();
	   }
           else it->action(-1); // start

           while ((e=x_get_next_event(w,x,y,v1,v2,t)) != button_release_event)
           { 
             if (e != motion_event || y == y0) continue;

             y0 = y;
             y += but_dy;
             if (y < yoff1) y = yoff1;
             if (y > panel_height - yoff2) y = panel_height - yoff2;

             //it->ycoord  = y + (it->height - yskip)/2;
             it->ycoord  = y;

             int D = (1000*(y - yoff1))/(panel_height-yoff1-yoff2);
	     
	     if (it->action_ptr) {
	       it->action_ptr->set_params(this,NULL,D);
	       it->action_ptr->operator()();
	     }
             else it->action(D);

             start_buffering();
             draw_panel();
             flush_pixels(0,0,panel_width,panel_height);
             stop_buffering();

            }

           if (owner->p_win == 0) x_grab_pointer(0);

           last_sel_button = 0;
           draw_button(it,0);

           if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,-2);
	     it->action_ptr->operator()();
	   }
           else it->action(-2); // finish

           return -1;
          }
       }


       if (k == button_release_event || 
          (k == button_press_event && menu_mode))
       { 

       if (last_sel_item)
       { panel_item it = last_sel_item;
         last_sel_item = 0;
         if (it->release_action) 
         { clipping(2);
           call_window = this;
           call_item = it;
           it->release_action(*(int*)it->ref);
           clipping(1);
          }
        }

        if (active_item)
        { panel_item it = active_item;
          if (it->release_action) 
          { clipping(2);
            call_window = this;
            call_item = it;
            it->release_action(0);
            clipping(1);
           }
         }


         if (last_sel_button == 0)
           { active_window = this;
             menu_mode = 0; }
         else
           { it = last_sel_button;
             if (it->ref == 0)
               { draw_button(it,0);
                 but = it->dat1;
                }
             else
               { BASE_WINDOW* wp =  ((BASE_WINDOW*)it->ref);
                 active_window = wp;
                 wp->menu_mode = 0;
                 it = 0;
                }
            }
         }

      }


    if (but == -1 && it) // item selected
    {
      if (k != button_press_event) b = 0;

      int lw = get_item_label_width(it);

      clipping(0);

      switch (it->kind) {
  
      case Text_Item: break;
  
      case Slider_Item:
      { int xoff1 = it->xcoord + lw;

      //if (x < xoff1-button_h || x > xoff1+slider_w+button_h) break;
        if (x < xoff1-button_h || x > xoff1 + it->width + button_h) break;

        int w = draw_win;
        int last_val = *(int*)it->ref;

        for(;;)
        { 
          draw_slider_item(it,x);

          int val = *(int*)it->ref;

          if (val != last_val && (it->action || it->action_ptr))
          { 
            clipping(2);
            call_window = this;
            call_item = it;
   	    if (it->action_ptr) {
               it->action_ptr->set_params(this,NULL,val);
               it->action_ptr->operator()();
      	    }
            else it->action(val);

            clipping(1);
           }

           last_val = val;

           int xx = 0;
           int yy = 0;
           if (x_get_next_event(w,xx,yy,b,b,t) == button_release_event) break;
           // ignore events from other windows
           if (w == draw_win) x = xx;
         }

        break;
      }
  
      case Int_Item:
      { activate_string_item(it,x,b);
        if (k == key_press_event)
        { panel_text_edit(it);
          *(int*)it->ref = atoi(it->data_str);
         }
        break;
       }
  
      case Float_Item:
      { activate_string_item(it,x,b);
        if (k == key_press_event)
        { panel_text_edit(it);
          *(double*)it->ref = atof(it->data_str);
         }
        break;
       }
  
      case String_Item:
      case String_Menu_Item:
      { activate_string_item(it,x,b);
        if (k == key_press_event)
        { panel_text_edit(it);
          assign_str(it->ref,it->data_str);
          it->data_str = access_str(it->ref);
         }
        break;
       }
  
     case Choice_Item:
     { 
       int d = (x-it->xcoord-lw)/get_item_button_width(it);
       //int d = (x-it->xcoord-lw)/choice_w;

       if (d < it->argc)
       { int x = it->offset + d * it->dat2;

         *(int*)it->ref = x;
         draw_choice_item(it);

         if (it->action || it->action_ptr)
         { clipping(2);
           call_window = this;
           call_item = it;
	   
	   if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,x);
	     it->action_ptr->operator()();
	   }
           else it->action(x);

           clipping(1);
          }
        }
       break;
      }

     case Choice_Mult_Item:
     { 
       int d = (x-it->xcoord-lw)/get_item_button_width(it);
       //int d = (x-it->xcoord-lw)/choice_w;

       if (d < it->argc)
       { int x = *(int*)it->ref ^ (1 << d);

         *(int*)it->ref = x;
         draw_choice_item(it);

         if (it->action || it->action_ptr)
         { clipping(2);
           call_window = this;
           call_item = it;
	   
	   if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,x);
	     it->action_ptr->operator()();
	   }
           else it->action(x);
	   
           clipping(1);
          }
        }
       break;
      }
  
  
     case Bool_Item:
     { int b = (*(bool*)it->ref) ? 0 : 1;
       *(bool*)it->ref = (b != 0);

       draw_bool_item(it);

       last_sel_item = it;
       if (it->action || it->action_ptr) 
       { clipping(2);
         call_window = this;
         call_item = it;
	 
	 if (it->action_ptr) {
	   it->action_ptr->set_params(this,NULL,b);
	   it->action_ptr->operator()();
	 }
         else it->action(b);

         clipping(1);
       }
       break;
      }
  
     case Color_Item:
     { //int dx = color_h + 2;
       int dx = color_h + 2*panel_line_width;

       int x0 = it->xcoord + lw;
       int d = (x - x0)/dx - 1;

       if (d < -1 || d > 17) break; 

       color clr = color::get(d);

       *(int*)it->ref = clr;
       draw_color_item(it);

       if (b == 3 && d >= 0)
       { x_choose_color(0,clr);
         draw_color_item(it);
         break;
        }

       if (it->action || it->action_ptr) 
       { clipping(2);
         call_window = this;
         call_item = it;
         if (it->action_ptr) {
           it->action_ptr->set_params(this,NULL,clr);
           it->action_ptr->operator()();
         }
         else it->action(clr);

         clipping(1);
       }


       break;
      }
  
     case Bitmap_Choice_Mult_Item:
     case Bitmap_Choice_Item:
     { int d = (x - it->xcoord - lw)/(it->dat1 + 3);
       if (d < it->argc) 
       { int c = d;
         if (it->kind == Bitmap_Choice_Mult_Item) 
            c = (*(int*)it->ref) ^ (1 << d);

         *(int*)it->ref = c;
         draw_bitmap_choice_item(it);

         if (it->action || it->action_ptr) 
         { clipping(2);
           call_window = this;
           call_item = it;
	   
	   if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,c);
	     it->action_ptr->operator()();
	   }
           else it->action(c);

           clipping(1);
         }

        }
       break;
     }

     case Button_Item:
     { 
       panel_item lsb = last_sel_button;

       if (menu_style == 2 && active_item)
       { panel_item tmp = active_item;
         active_item = 0;
         draw_button(tmp,0);
        }

       if (it != lsb) 
       { draw_button(it,1);
         last_sel_button = it;
        }
       else  // second click on active button
         if (k == button_press_event) last_sel_button = 0;
    
       if (lsb && (it != lsb || k==button_press_event))
       { draw_button(lsb,0);
         if (lsb->ref)
         { close_sub_window((BASE_WINDOW*)lsb->ref);
           active_window = this;
          }
        }

       if (it->menu_but == it)
       { if (it->action || it->action_ptr)
         { call_window = this;
           call_item = it;
	   
	   if (it->action_ptr) {
	     it->action_ptr->set_params(this,NULL,0);
	     it->action_ptr->operator()();
	   }
           else it->action(0);
           flush_pixels(0,0,window_width,window_height);
          }
         if (it->ref == 0)
         { draw_button(it,0);
           last_sel_button = 0;
          }
       }

       if (it != lsb && it->ref && it->ref != this) open_sub_panel(it);

       break;
     }

    }

    if (it->kind == Button_Item) it = 0;

  }


  int result = but;

  if (it  && it->kind == Button_Item  && it->ref == 0 ) 
  { 
    BASE_WINDOW* wp = this;
    while(wp->p_win) 
    { BASE_WINDOW* wpp = wp->p_win;
      close_sub_window(wp);
      wp->p_win = 0;
      wp = wpp;
     }
    active_window = wp;

    panel_item lsb = wp->last_sel_button;

    if (lsb)
    { drawing_mode save_mo = x_set_mode(wp->draw_win,src_mode);
      wp->clipping(1);
      wp->draw_button(lsb,0);
      x_set_mode(wp->draw_win,save_mo);
      wp->clipping(2);
      wp->last_sel_button = 0;
      if ((lsb->dat2 & 0x2) == 0)  // second bit = 0: return number of lsb 
           result = lsb->dat1;
     }

     if (lsb == 0 || lsb->menu_but == lsb) result = -1;

   }

  x_set_line_width(draw_win,save_lw);
  x_set_line_style(draw_win,save_ls);
  x_set_mode(draw_win,save_mo);
  x_set_text_mode(draw_win,save_tm);
  clipping(2);

  if (it && it->kind == Button_Item && (it->action || it->action_ptr) && it->menu_but == 0)
  { call_window = this;
    call_item = it;
    
    if (it->action_ptr) {
      it->action_ptr->set_params(this,NULL,but);
      it->action_ptr->operator()();
    }
    else it->action(but);
   }

  if (it == 0 && but == -1 && k == button_press_event && result == -1) {
    // click on panel background (inactivate current active string item)
    active_item = 0;
    redraw_panel();
    keyboard(0);
  }


 return result;

}




panel_item BASE_WINDOW::get_item(const char* s)
{ for(int i = 0; i<item_count; i++)
  { panel_item p = Item[i];
    if (strcmp(p->label_str,s) == 0) return p;
   }
  return NULL;
}

int BASE_WINDOW::get_button(const char* s)
{ panel_item p = get_item(s);
  if (p && p->kind == Button_Item)
     return p->dat1;
  else
     return -1;
}


panel_item BASE_WINDOW::get_button_item(int but)  
{ for(int i = 0; i<item_count; i++)
  { panel_item p = Item[i];
    if (p->kind == Button_Item && p->dat1 == but)  return p;
   }
  return 0;
 }




BASE_WINDOW* BASE_WINDOW::get_window(panel_item it)
{ return (it->kind == Button_Item) ? (BASE_WINDOW*)it->ref : 0; }

BASE_WINDOW* BASE_WINDOW::get_window(int but)
{ panel_item p = get_button_item(but);
  return p ? (BASE_WINDOW*)p->ref : 0;
}

panel_action_func BASE_WINDOW::get_action(panel_item it)
{ return (it->kind == Button_Item) ? it->action : 0; }


int BASE_WINDOW::get_value(panel_item it) { return int(it->dat1); }


BASE_WINDOW* BASE_WINDOW::set_window(panel_item p, BASE_WINDOW* wp)
{ BASE_WINDOW* old = (BASE_WINDOW*)p->ref;
  p->ref = wp;
  if (wp) wp->owner_item = p;
  if (old && wp != old) old->owner_item = 0;
  return old;
}

BASE_WINDOW* BASE_WINDOW::set_window(int but, BASE_WINDOW* wp)
{ panel_item p = get_button_item(but);
  return (p==0) ? 0 : set_window(p,wp);
}

panel_action_func BASE_WINDOW::set_action(int but, 
                                                 panel_action_func func)
{ panel_action_func ret = 0;
  panel_item p = get_button_item(but);
  if (p)
  { ret =  p->action;
    p->action = func;
   }
  return ret;
}

const window_handler* BASE_WINDOW::set_action(int but, const window_handler& obj)
{ const window_handler* ret = 0;
  panel_item p = get_button_item(but);
  if (p)
  { ret =  p->action_ptr;
    p->action_ptr = & (window_handler&) obj;
   }
  return ret;
}


int BASE_WINDOW::set_value(int but, int val)
{ int ret = -1;
  panel_item p = get_button_item(but);
  if (p)
  { ret =  p->dat1;
    p->dat1 = val;
   }
  return ret;
}


void BASE_WINDOW::set_secret(panel_item it, bool b) 
{ if (it)
  { it->secret = b; 
    if (is_open()) draw_item(it);
   }
}


void BASE_WINDOW::enable_item(panel_item it) 
{ if (it && !it->enabled)
  { it->enabled = 1; 
    if (it->menu_but) it->menu_but->enabled = 1;
    panel_enabled = 1;
    if (is_open()) draw_item(it);
   }
}

void BASE_WINDOW::disable_item(panel_item it) 
{ if (it && it->enabled)
  { it->enabled = 0; 
    if (it->menu_but) it->menu_but->enabled = 0;
    if (is_open()) draw_item(it);
   }
}

void BASE_WINDOW::enable_button(int but) 
{ panel_item it = get_button_item(but);
  if (it) enable_item (it);
}


void BASE_WINDOW::disable_button(int but) 
{ panel_item it = get_button_item(but);
  if (it) disable_item (it);
}


void BASE_WINDOW::enable_buttons()  
{ for(int i = 0; i<item_count; i++)
  { panel_item p = Item[i];
    if (p->kind == Button_Item) p->enabled = 1;
    draw_item(p);
   }
 }


void BASE_WINDOW::disable_buttons()  
{ for(int i = 0; i<item_count; i++)
  { panel_item p = Item[i];
    if (p->kind == Button_Item) p->enabled = 0;
    draw_item(p);
   }
 }



void BASE_WINDOW::disable_panel(bool disable_items)  
{ panel_enabled = 0; 
  if (disable_items){
    for (panel_item it = first_item(); it; it = next_item(it)) it->enabled = 0;
    //if (is_open()) draw_panel();
    if (is_open()) redraw_panel();
  }
}


void BASE_WINDOW::enable_panel()  
{ panel_enabled = 1; 
  for (panel_item it = first_item(); it; it = next_item(it)) it->enabled = 1;
  //if (is_open()) draw_panel();
  if (is_open()) redraw_panel();
}


const char* BASE_WINDOW::get_item_label(panel_item p) { return p->label_str; }

void BASE_WINDOW::set_item_button_width(panel_item p, int w) 
{ p->button_width = w; }

void BASE_WINDOW::set_item_label(panel_item p, const char* s)
{ delete[] p->label_str;
  p->label_str = string_dup(s);
  if (is_open() && p->menu_win == 0) draw_item(p);
/*
  if (p->menu_win && p->menu_win->is_open()) p->menu_win->draw_item(p);
*/
 }

void BASE_WINDOW::set_item_help_str(panel_item p, const char* hlp)
{ delete[] p->help_str;
  p->help_str = string_dup(hlp);
}

void BASE_WINDOW::set_item_selected(panel_item p, bool b)
{ p->str_selected = b;
  if (is_open()) draw_item(p);
}

void BASE_WINDOW::set_item_text_color(panel_item p, color clr)
{ p->text_clr = clr;
  if (is_open()) draw_item(p);
}

void BASE_WINDOW::set_item_label_color(panel_item p, color clr)
{ p->label_clr = clr;
  if (is_open()) draw_item(p);
}

void BASE_WINDOW::set_item_label_width(panel_item p, int w)
{ p->label_width = w; }

void BASE_WINDOW::set_item_width(panel_item p, int w)
{ p->item_width = w; }

void BASE_WINDOW::set_item_height(panel_item p, int h)
{ p->height = h; }


int BASE_WINDOW::get_item_label_width(panel_item p)
{ int lw = p->label_width;
  return (lw < 0) ? label_w : lw;
}

int BASE_WINDOW::get_item_button_width(panel_item p)
{ int w = p->button_width;
  return (w < 0) ? choice_w : w;
}

int BASE_WINDOW::get_item_width(panel_item p) { return p->width; }

int BASE_WINDOW::get_default_width(panel_item p)
{ int lw = p->item_width;

  if (lw > 0) return lw;

  switch (p->kind) {
  
     case String_Item:
     case String_Menu_Item: lw = string_w;
                            break;
    
     case Slider_Item: lw = slider_w;
                       break;
    
     case Int_Item: 
     case Float_Item: lw = number_w;
                      break;

     case Choice_Mult_Item:
     case Choice_Item: lw = p->argc * get_item_button_width(p); 
                       break;

     case Bitmap_Choice_Mult_Item: 
     case Bitmap_Choice_Item: 
                       lw = p->argc * (p->dat1 + 3 ); // n * w
                       break;

     case Color_Item: 
                   //lw = 18 * (color_h + 2*panel_line_width );
                   lw = 18*(color_h + 2*panel_line_width) - 2*panel_line_width;
                   break;

     case Bool_Item: lw = color_h;
                     break;

   }

  return lw;
}


int BASE_WINDOW::get_item_height(panel_item p) 
{ 
  int h = yskip;

  switch (p->kind) {

   case Int_Item:
   case Float_Item:
   case String_Item: h = string_h;
                     break;

   case Color_Item:  h = color_h;
                     break;

   case Slider_Item: h = slider_h;
                     break;

   case Bitmap_Choice_Item: 
                    h = p->dat2;
                    break;

   case Choice_Mult_Item: 
   case Choice_Item: h = choice_h;
                     break;
   }

   return h;
}



const char* BASE_WINDOW::get_button_label(int but)
{ panel_item p = get_button_item(but);
  return  (p) ? p->label_str : 0;
}

void BASE_WINDOW::set_button_label(int but, const char* s)
{ panel_item p = get_button_item(but);
  if (p) 
  { delete[] p->label_str;
    p->label_str = string_dup(s);
    if (is_open()) draw_item(p);
   }
}


void BASE_WINDOW::set_button_help_str(int but, const char* hlp)
{ panel_item p = get_button_item(but);
  if (p)
  { delete[] p->help_str;
    p->help_str = string_dup(hlp);
   }
}


void BASE_WINDOW::set_button_repeat(int but, int msec)
{ panel_item p = get_button_item(but);
  if (p) p->offset = msec;
}


void BASE_WINDOW::set_button_pixrects(int but,char*pmap0,char*pmap1)
{ panel_item p=get_button_item(but);
  if(p) 
  { delete[] p->argv; 
    p->argv = 0;
    p->argc = 0;
    if (pmap0 && pmap1)
    { p->argv = new char*[2];
      p->argv[0] = pmap0;
      p->argv[1] = pmap1;
      p->argc = -1; // pmap
     }
    if (is_open()) draw_item(p);
  }
}


// Iteration

panel_item BASE_WINDOW::first_item() const { return Item[0]; } 

panel_item BASE_WINDOW::next_item(panel_item it)  const
{ if (it == 0) return 0;
  int i = it->index + 1;
  return (i < item_count) ? Item[i] : 0;
 }



void BASE_WINDOW::open_scrollbar(void (*scroll_up)(int),
                                 void (*scroll_down)(int),
                                 void (*scroll_drag)(int), 
                                 double sz, double pos)
{ if (sz > 1) sz = 1;
  if (sz < 0) sz = 0;

//cout << string("open_scrollbar:  sz = %.2f  pos = %.2f", sz, pos) << endl;

  int w = scrollbar_w;

  if (scroll_bar) // adjust slider size and position of existing scroll bar
  { set_scrollbar_pos(pos,sz);
    return;
   }

  BASE_WINDOW* wp = this;

  
  BASE_WINDOW* sp = new BASE_WINDOW(-1,-1);

  //sp->menu_style = 0;
  sp->menu_style = -1;  // this makes sp a scrollbar window

  sp->set_inf(wp);

  sp->set_border_color(panel_bg_color);

  sp->panel_width = w+3; 

  int spx = window_width - w - 3;
  int spy = 0;

  if (window_height == panel_height)
  { // menu
    sp->panel_height = window_height+2;
    spy = -2;

  }
  else 
  { // panel & draw window
    sp->panel_height = window_height - panel_height + 1;
    spy = panel_height-1;
   }

  sp->window_width  = sp->panel_width;
  sp->window_height = sp->panel_height;
  sp->yskip         = w;

  sp->button("UP",-1,scroll_up);
  sp->button("DOWN",-1,scroll_down);
  sp->button("SLIDER",-1,scroll_drag);

  panel_item p;

  p = sp->Item[0];
  p->ref    = sp;
  p->width  = w;
  p->height = w;
  p->xcoord = 0;
  p->ycoord = 0;
  p->menu_but = p;
  p->offset = 50;  // repeat every 50 msec
    
  p = sp->Item[1];
  p->ref    = sp;
  p->width  = w;
  p->height = w;
  p->xcoord = 0;
  p->ycoord = sp->panel_height - w - 1;
  p->menu_but = p;
  p->offset = 50;  // repeat every 50 msec

  p = sp->Item[2];
  p->ref    = sp;

//p->width  = w-1;
  p->width  = w;

  p->height = int(sz*(sp->panel_height - 2*w));

//p->xcoord = 2;
  p->xcoord = 1;
  p->ycoord = int(w + pos *(sp->panel_height - 2*w - p->height - 5)) + 2;

  p->menu_but = p;

  x_set_border_width(sp->draw_win,1);

  x_open_window(sp->draw_win,spx,spy,sp->window_width,sp->window_height,
                                                                 draw_win);
  sp->configure();
  sp->redraw_panel();
  sp->scroll(0);

  if (scroll_bar) delete scroll_bar;
  scroll_bar = sp;
  
  redraw_panel();  
//draw_copy_right();

}


void BASE_WINDOW::close_scrollbar()
{ if (scroll_bar)  delete scroll_bar;
  scroll_bar = 0;
 }



void BASE_WINDOW::set_scrollbar_pos(double p, double sz)
{
  BASE_WINDOW* sp = scroll_bar;

  if (sp == 0) return;

  if (p > 1) p = 1;
  if (p < 0) p = 0;

  panel_item it1 = sp->Item[0];
  panel_item it2 = sp->Item[1];
  panel_item it3 = sp->Item[2]; // slider

  int scroll_h = sp->panel_height-it1->height - it2->height-it3->height - 5;

  int slider_pos = int(it1->height + p*scroll_h) + 2;

  int slider_h = it3->height;

  if (sz > 0) 
  { int w = button_h - 2;
    slider_h  = int(sz*(sp->panel_height - 2*w));
   }

  if (it3->ycoord != slider_pos || it3->height != slider_h)
  { // slider pos has changed
    it3->ycoord = slider_pos;
    it3->height = slider_h;

 // if (sp->is_open()) sp->draw_panel();

    if (sp->is_open())
    { //sp->start_buffering();
      sp->draw_panel();
      x_flush_buffer(sp->draw_win,0,0,sp->panel_width,sp->panel_height);
      //sp->stop_buffering();
    }

   }
}
  

LEDA_END_NAMESPACE

