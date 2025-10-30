/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _file_view.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h>

LEDA_BEGIN_NAMESPACE

enum { file_load, file_save, file_all, file_exit };

#define MAX_LINES1 600
#define MAX_LINES2 8000

static window* win_ptr;

static string  dname = ".";
static string  fname = "none";
static string  filter = "";

static double ypos  = 0;
static double ylow  = 0;
static double yhigh = 0;

static list<string> txt;
static double txt_height;
static double tot_height;


void update_status_line()
{ int line = int((ypos + tot_height - win_ptr->ymax())/txt_height) + 1;
  string str("  %4d/%d",line,txt.length());
  string path = " " + dname + "/" +fname + " " + str;
  win_ptr->set_status_string(path.replace_all("\\","/"));
 }

static void draw_nothing(window* wp) {}

static void draw_text(window* wp) 
{
  wp->set_fixed_font();

  txt_height = wp->text_height("H");
  //tot_height = txt_height * txt.length();

  double x = wp->pix_to_real(3);
  double y = ypos + tot_height - wp->pix_to_real(1);

  double ymin = wp->ymin() + wp->pix_to_real(18);
  double ymax = wp->ymax();


  wp->start_buffering();
  wp->clear();
  string s;
  forall(s,txt)
  { if (y <= ymax) wp->draw_text(x,y,s);
    y -= txt_height;
    if (y < ymin) break;
   }
  wp->flush_buffer();
  wp->stop_buffering();

  update_status_line();
}



static void scroll_down(int)   
{ double d = txt_height;
  if (ypos < yhigh) ypos += d;
  draw_text(win_ptr);
  double dy = (ypos - ylow)/(yhigh- ylow);
  win_ptr->set_scrollbar_pos(dy);
 }


static void scroll_up(int)
{ double d = txt_height;
  if (ypos > ylow) ypos -= d;
  draw_text(win_ptr);
  double dy = (ypos - ylow)/(yhigh - ylow);
  win_ptr->set_scrollbar_pos(dy);
 }


static void scroll_drag(int i) 
{ if (i >= 0)
    { double dy = i/1000.0; 
      ypos = ylow + dy * (yhigh - ylow); 
     }
  else
    if (i == -2) // finish
    { // round ypos to next line
      double lh = txt_height;
      int li = int(0.5 + (ypos - ylow)/lh);
      ypos = ylow + li*lh;
     }
  draw_text(win_ptr);
}


static void display_text() 
{ 
  double y0 = win_ptr->ymax();
  double y1 = win_ptr->ymin() + win_ptr->pix_to_real(18);

  win_ptr->set_fixed_font();

  txt_height = win_ptr->text_height("H");
  tot_height = txt_height * txt.length();

  int w = win_ptr->width();
  int h = win_ptr->real_to_pix(tot_height);
  if (h < win_ptr->height()) h = win_ptr->height();

  double x = win_ptr->pix_to_real(3);
  double y = y0;


  if (txt.length() < MAX_LINES1)
   { win_ptr->start_buffering(w,h);
     string s;
     forall(s,txt)
     { win_ptr->draw_text(x,y,s);
       y -= txt_height;
      }
     win_ptr->stop_buffering();
     ylow  = y; 
   }
  else
    ylow = y0 - txt.length() * txt_height;

  yhigh = y0 - int((y0 - y1)/txt_height) * txt_height;

  ypos  = ylow;

  win_ptr->clear();
  draw_text(win_ptr);
  //win_ptr->set_frame_label(fn);


  int w_width = win_ptr->real_to_pix(80 * win_ptr->text_width("H")) + 10;
  int w_height = win_ptr->real_to_pix(50 * win_ptr->text_height("H")) + 10;

  double f = (y0 - y1)/ tot_height;

  if (f < 1)
  { win_ptr->open_scrollbar(scroll_up,scroll_down,scroll_drag,f);
    w_width += win_ptr->scrollbar_width();
   }
  else
    win_ptr->close_scrollbar();

/*
  win_ptr->resize(win_ptr->xpos(),win_ptr->ypos(),w_width,w_height);
*/

}

static bool confirm_overwrite(const char* fname)
{
#if defined (__win32__)
  return true;
#else
  panel P;
  P.buttons_per_line(2);
  P.text_item("");
  P.text_item(string("\\bf\\blue File\\black %s\\blue exists.",fname));
  //P.text_item("");
  P.button("overwrite",0);
  P.button("cancel",1);
  return (P.open(*win_ptr) == 0);
#endif
}



static void read_file(string fn) 
{ 
  win_ptr->set_status_string(" Reading " + fname);
  ifstream in(fn);
  txt.clear();
  while (in && txt.length() < MAX_LINES2) 
  { string s = read_line(in);
    if (in || s != "") txt.append(s);
   }
  display_text();
}


static void write_file(string fname)
{ 
  if (is_file(fname) && !confirm_overwrite(fname)) return;

  win_ptr->set_status_string(" Writing " + fname);
  ofstream out(fname);
  string line;
  forall(line,txt) out << line << endl;
  display_text();
 }


static void file_handler(int what)
{ 
  file_panel FP(*win_ptr,fname,dname);

  switch (what) {
  case file_load: FP.set_load_handler(read_file);
                  break;
  case file_save: FP.set_save_handler(write_file);
                  break;
  case file_all:  FP.set_load_handler(read_file);
                  FP.set_save_handler(write_file);
                  break;
  }
  if (filter != "") FP.set_pattern(filter,filter);
  FP.open();
}

static bool win_close_flag = false;

static void close_handler(window*) { win_close_flag = true; }



void LedaFileViewer(window& W, const list<string>& L)
{ 
  string fn,dn,pat;

  if (L.length() == 4 && L.head()[0] == char(1))
  { fn  = L[L.get_item(1)];
    dn  = L[L.get_item(2)];
    pat = L[L.get_item(3)];
   }

  if (pat != "" && pat != "$") filter = pat;
  if (dn != "") set_directory(dn);

  win_ptr = &W;
  W.set_redraw(draw_text);
  W.set_window_close_handler(close_handler);

  if (fn != "") 
  { fname = fn;
    read_file(fname);
/*
    if (scroll_down)
    { ypos = yhigh;
      draw_text(&W);
      win_ptr->set_scrollbar_pos(1);
     }
*/
   }
  else
  { txt = L;
    display_text();
   }

  win_close_flag = false;

  if (pat == "$") 
  { // scroll to end
    double text_height = txt.length() * txt_height;
    double win_height = win_ptr->ymax() - win_ptr->ymin();
    if (text_height > win_height) {
      ypos = yhigh;
      draw_text(&W);
      win_ptr->set_scrollbar_pos(1);
    }
  }

  for(;;)
  { 
    int    val;
    double x,y;
    int    e = W.read_event(val,x,y);

    if (e == button_press_event && val == file_exit) break;

    if (win_close_flag) break;

    if (e == button_press_event)
    {
       switch (val) {
         case MOUSE_BUTTON(4): scroll_up(0);
                               break;
         case MOUSE_BUTTON(5): scroll_down(0);
                               break;
       }
     }

    if (e == key_press_event)
    {  
       if (val == 'q') break;

       switch (val) {
         case 'k':
         case KEY_UP:   scroll_up(0);
                        break;
         case 'j':
         case KEY_DOWN: scroll_down(0);
                        break;
         case 'g':
         case KEY_HOME: ypos = ylow; 
                        draw_text(&W);
                        win_ptr->set_scrollbar_pos(0);
                        break;
         case 'G':
         case KEY_END:  ypos = yhigh;
                        draw_text(&W);
                        win_ptr->set_scrollbar_pos(1);
                        break;
       }

       while (W.get_event(val,x,y) == key_press_event);
     }
   }

}


void LedaFileViewer(const list<string>& L, int lines, int cols, string label)
{
  if (cols == 0) cols = 80;

  int w = int(cols*window::screen_dpi()/10.35);
  int h = int(1.2*w);

  menu file_menu;
  file_menu.button("&Load File",file_load,file_handler);
  file_menu.button("&Save File",file_save,file_handler);
  //file_menu.button("Load/Save",file_all, file_handler);
  file_menu.button("E&xit",file_exit);

  menu dummy_menu;
  dummy_menu.button("button1");
  dummy_menu.button("button2");
  dummy_menu.button("button3");
  dummy_menu.button("button4");

  window W(w,h,label);
  win_ptr = &W;

  W.button("&File", file_menu);
  int edit_but = W.button("&Edit", dummy_menu);
  int help_but = W.button("&Help", dummy_menu);
//int exit_but = W.button("E&xit ",file_exit);

  W.make_menu_bar(2);
  W.set_redraw(draw_text);
  W.set_window_close_handler(close_handler);

  W.display(window::center,window::center);

  W.set_fixed_font();

  w = W.real_to_pix(80*W.text_width("H")) + 10;
  h = int(1.15*w);
  W.resize(W.xpos(),W.ypos(),w,h);

  dname = get_directory();

  W.disable_button(edit_but);
  W.disable_button(help_but);

  BASE_WINDOW* swin = W.open_status_window();
  swin->set_fixed_font();

  LedaFileViewer(W,L);
}



void LedaFileViewer(const list<string>& L)
{ LedaFileViewer(L,0,0,"LEDA File Viewer"); }


void LedaFileViewer(window& W, string fn, string dn, string pat)
{ list<string> L;
  L.append(string(char(1)));
  L.append(fn);
  L.append(dn);
  L.append(pat);
  LedaFileViewer(W,L);
 }

void LedaFileViewer(string fn, string dn, string pat)
{ list<string> L;
  L.append(string(char(1)));
  L.append(fn);
  L.append(dn);
  L.append(pat);
  LedaFileViewer(L);
 }

void LedaFileViewer(window& W, string fn, string dn) 
{ LedaFileViewer(W,fn,dn,""); }

void LedaFileViewer(string fn, string dn) 
{ LedaFileViewer(fn,dn,""); }


void LedaFileViewer(window& W, string fn) 
{ LedaFileViewer(W,fn,"",""); }

void LedaFileViewer(string fn) 
{ LedaFileViewer(fn,"",""); }

LEDA_END_NAMESPACE
 
 
