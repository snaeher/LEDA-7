/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _file_panel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h>
#include <LEDA/system/socket.h>
#include <LEDA/core/string.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>

const int file_load    = 100; 
const int file_save    = 101; 
const int file_open    = 102; 
const int file_forward = 103; 
const int file_back    = 104; 
const int file_cancel  = 105;

static double scroll_off = 0;
static window* win_ptr = 0;


list<string>  file_panel::xget_files(string dir, int what)
{ 
  // file:  name:time:size

  list<string> lst;

  if (xhost == "") 
  { // local host
    list<string> L;
    if (what == 0) L = get_entries(dir);
    if (what == 1) L = get_files(dir);
    if (what == 2) L = get_directories(dir);

    string s;
    forall(s,L)
    { string path = get_dir_name() + sep + s;
      time_t tf = time_of_file(path);
      size_t sz = size_of_file(path);
      lst.append(string("%s:%d:%d",~s,tf,sz));
     }
     return lst;
   }


  // remote host (xhost)
  
  leda_socket sock;
  //sock.set_num_sz_bytes(8);
  sock.set_host(xhost);
  sock.set_port(9668);
  sock.set_limit(-1);

  if (!sock.connect())
  { cerr << endl;
    cerr << sock.get_error() <<endl;
    return lst;
   }

  int r0;
  sock.receive_int(r0);
  
  random_source ran;
  ran.set_seed(r0);
  int r1 = ran();

  sock.send_int(r1);
  string s;
  sock.receive_string(s);

  if (s != "ok") return lst;

//if (what == 0) sock.send_string("list_entries");
  if (what == 1) sock.send_string("list_files");
  if (what == 2) sock.send_string("list_directories");

  sock.send_string(dir);

  string txt;
  sock.receive_string(txt);

  sock.disconnect();

  string fn;
  forall_words(fn,txt) lst.append(fn);

  return lst;
}



static bool file_panel_closed = false;

static void win_close_handler(window* wp) { 
  file_panel_closed = true;
/*
  int but = wp->get_button("Cancel");
  if (but != -1) wp->set_focus_button(but);
*/
}


static int cmp_files(const string& fname1, const string& fname2) 
{
  string s1 = fname1;
  string s2 = fname2;
/*
  if (s2.starts_with(".")) s2 = s2.tail(-1);
  if (s1.starts_with(".")) s1 = s1.tail(-1);
*/

  return compare(s1,s2);
}


static void special_event_handler(void* ptr, const char* event, 
                                           const char* arg, int x, int y) { 
  window*  wp = (window*)ptr;
  file_panel* fp = (file_panel*)wp->get_inf();
  string dir = fp->get_dir_name();
  if (string(event) == "drop" && fp->drop_handler) fp->drop_handler(arg,dir);
  fp->update(dir,"");
}




static void redraw(window* wp)
{ window& W = *wp;

  if (!W.is_open()) return;

  W.set_text_font();

  // center buttons

  //int d = W.real_to_pix(W.text_width("H"));
  int d = W.real_to_pix(W.text_width("x"));

  panel_item but1 = W.get_button_item(file_load);

  if (but1 == 0) but1 = W.get_button_item(file_save);

  if (but1)
  { int but_x1 = W.width()/2 - W.get_item_width(but1) - d; 
    W.set_item_xpos(but1,but_x1);
   }

  panel_item but2 = W.get_button_item(file_cancel);

  if (but2 == 0) but2 = W.get_button_item(file_open);

  if (but2)
  { int but_x2 = W.width()/2 + d; 
    W.set_item_xpos(but2,but_x2);
   }

  W.redraw_panel();


  file_panel* fp = (file_panel*)W.get_inf();

  W.start_buffering();

  W.clear();

  const list<string>& dir_list = fp->get_dir_list();
  const list<string>& file_list = fp->get_file_list();

  double pix = W.pix_to_real(1);

  double xleft = W.xmin();
  double xright = W.xmax() - W.pix_to_real(W.scrollbar_width() + 12);
  double ytop = W.ymax();
//double ybot = W.ymin();

  double tw = W.text_width("H");

  double y = ytop + pix;

  double x0 = xleft + tw;
  double x1 = xright - 25*tw;

  int yskip = fp->yskip;

  y += scroll_off;

  double y1 = 0;
  double y2 = 0;
  double ycur = 0;

  string s;
  forall(s,dir_list)
  { 
    color clr  = "#eeeeee";
    color lclr = "#d0d0d0";
    color tclr = blue;

    //if (s.starts_with(".")) tclr = "#888888";
    if (s.starts_with(".")) tclr = "#55aaee";

    string selected_dn = fp->get_selected_dir();
    if (s == selected_dn) clr = "#ffffe0";

    string A[3];
    s.split(A,3,':');
    s = A[0];
    int tf = atoi(A[1]);

    string time_str = time_string(tf);

    string s1 = time_str.replace(" ","   ").replace_all("/","-");

    string s2 = "Folder";

    s += fp->sep;

    if (s.length() > 34) s = s.head(34) + " ...";

    y1 = y - W.pix_to_real(yskip);
    W.draw_box(xleft,y,W.xmax(),y1,clr);
    W.draw_rectangle(xleft,y,W.xmax(),y1,lclr);

    double yt = y - 0.15*W.pix_to_real(yskip);

    double x2 = xright - W.text_width(s2) - W.pix_to_real(5);

    W.draw_text(x0,yt,s,tclr);
    W.draw_text(x1,yt,s1,tclr);
    W.draw_text(x2,yt,s2,tclr);
    y = y1; 
   }

  y2 = y;

  string current_fn = fp->get_file_name();

  forall(s,file_list)
  { 
    string A[3];
    s.split(A,3,':');
    s = A[0];
    int tf = atoi(A[1]);
    int sz = atoi(A[2]);

    color clr = "#fcfcfc";
    color lclr = grey1;
    color tclr = "#222222";

    if (s.starts_with(".")) tclr = "#888888";

    if (s == current_fn) 
    { clr = "#ffffe0";
      ycur = y;
     }


    if (s.length() > 34) s = s.head(34) + " ...";

    string time_str = time_string(tf);

    string s1 = time_str.replace(" ","   ").replace_all("/","-");

    string s2 = string("%d   kb",(int)sz/1024);
    
    double y1 = y - W.pix_to_real(yskip);

    W.draw_box(xleft,y,W.xmax(),y1,clr);
    W.draw_rectangle(xleft,y,W.xmax(),y1,lclr);

    double yt = y - (W.pix_to_real(yskip+2)-W.text_height("H"))/2;

    if (s == "No matching files.") 
    { tclr = grey2;
      s1 = "";
      s2 = "";
     }

    double x2 = xright - W.text_width(s2) - W.pix_to_real(5);

    W.draw_text(x0,yt,s,tclr);
    W.draw_text(x1,yt,s1,tclr);
    W.draw_text(x2,yt,s2,tclr);

    y = y1; 
  }

//W.draw_hline(y1,grey2);
  W.draw_hline(y2,grey2);
  W.draw_hline(y,grey2);

  if (ycur != 0)
  { double y1 = ycur;
    double y2 = ycur - W.pix_to_real(yskip);
    W.draw_rectangle(xleft-1,y1,W.xmax(),y2,"#333333");
   }
  
  W.flush_buffer();
  W.stop_buffering();

}



static void update_scroll_pos(int steps)
{ 
  file_panel* fp = (file_panel*)win_ptr->get_inf();

  int yskip = fp->yskip;

  double d = win_ptr->pix_to_real(yskip);


  const list<string>& dir_list = fp->get_dir_list();
  const list<string>& file_list = fp->get_file_list();
  int files = dir_list.length() + file_list.length();
  //int h = win_ptr->height();

  int h = win_ptr->real_to_pix(win_ptr->ymax() - win_ptr->ymin());
  int lines = int(h/yskip);

  if (files <= lines) return;

  double max_off = d*(files-lines);
  scroll_off += steps*d;

  if (scroll_off < 0) scroll_off = 0;
  if (scroll_off > max_off) scroll_off = max_off;

  //if (win_ptr->has_scrollbar()) 
  win_ptr->set_scrollbar_pos(scroll_off/max_off);
  redraw(win_ptr);
}


static void scroll_down(int i)
{ //cout << "scroll_down: " << i << endl;
  update_scroll_pos(+1);
}

static void scroll_up(int i)
{ //cout << "scroll_up: " << i << endl;
  update_scroll_pos(-1);
}

static void scroll_drag(int i)
{ //cout << "scroll_drag: " << i << endl;
  if (i == -1 ) {
    // drag start
    return;
  }

  if (i == -2 ) {
    // drag end
    return;
  }

  file_panel* fp = (file_panel*)win_ptr->get_inf();

  //double pix = win_ptr->pix_to_real(1);

  int yskip = fp->yskip;

  const list<string>& dir_list = fp->get_dir_list();
  const list<string>& file_list = fp->get_file_list();

  int files = dir_list.length() + file_list.length();
  //int h = win_ptr->height();

  int h = win_ptr->real_to_pix(win_ptr->ymax() - win_ptr->ymin());
  int lines = int(h/yskip);

  // status win
  lines--;


  if (lines >= files)
     scroll_off = 0;
  else
  { double f = double(i)/1000;
    //scroll_off = f*(files-lines)*20*pix; 
    scroll_off = f*(files-lines)*win_ptr->pix_to_real(yskip); 
   }

  redraw(win_ptr);
}
  

 
file_panel::~file_panel()
{ //W.close();
  if (wpp) wpp->redraw();
 }


void file_panel::change_filter(char* descr)
{ window* call_win = window::get_call_window();
  file_panel* fp = (file_panel*)call_win->get_inf();
  int pos = 0;
  string s;
  forall(s,fp->descr_list)
  { if (s == string(descr)) break;
    pos++;
   }

  if (pos < fp->pat_list.length()) 
     fp->pat_string = fp->pat_list[fp->pat_list.get_item(pos)];

  fp->update(*fp->dir_name,"");
}

void file_panel::change_dir(char* dir)
{ window* call_win = window::get_call_window();
  file_panel* fp = (file_panel*)call_win->get_inf();
  fp->update(dir,"");
  fp->dir_stack.clear();
  call_win->disable_button(file_forward);
 }

void file_panel::change_hidden(int b)
{ window* call_win = window::get_call_window();
  file_panel* fp = (file_panel*)call_win->get_inf();
  fp->show_hidden = (b != 0);
  fp->update(*fp->dir_name,"");
 }



void file_panel::init(window* wptr, string& fname, string& dname)
{
#if defined(__win32__)
  mswin = true;
  sep = "\\";
#else
  mswin = false;
  sep = "/";
#endif

  wpp = wptr;

  load_handler = 0;
  open_handler = 0;
  save_handler = 0;
  drop_handler = 0;
  cancel_handler= 0;

  show_hidden = false;

  win_ptr = &W;

  W.set_window_close_handler(win_close_handler);
  W.set_redraw(redraw);

  W.set_special_event_handler(special_event_handler);

  W.set_fixed_font();

  W.set_inf(this);

  load_ptr = 0;
  open_ptr = 0;
  save_ptr = 0;
  cancel_ptr=0;
  
  if (dname == "" || dname == ".") dname = get_directory();

/*
  // append time and size
  dname += ":0:0";
*/

  start_dir = dname;

  // if dname is prefix of fname remove it
  if (fname.index(fname) == 0) 
      fname = fname.replace(dname,"");
  if (fname.starts_with(sep)) fname = fname.tail(-1);

  if (fname.empty()) fname = "untitled";

  file_name  = &fname;
  dir_name  = &dname;

  load_string = "Load File";
  save_string = "Save File";

  panel_init = 0;

  char* s;
  if ((s = getenv("HOME")) != 0)
  { home_dir = s;
    global_dir_list.append(s);
   }

  global_dir_list.append(dname);
   
  list<string> drives = get_disk_drives();
  string drv;
  forall(drv,drives) {
    global_dir_list.append(drv + sep);
  }

  global_dir_list.sort();
}

  

file_panel::file_panel(string& fname, string& dname) : 
W(int(window::screen_width()/2.5),int(window::screen_width()/2.3),"File Panel")
{ 
  init(0,fname,dname); 
}

file_panel::file_panel(string& fname, string& dname, int width, int height) : 
           W(width,height,"File Panel")
{ 
  init(0,fname,dname); 
}

file_panel::file_panel(window& win, string& fname, string& dname) : 
W(int(window::screen_width()/2.5),int(window::screen_width()/2.3),"File Panel")
{ 
  init(&win,fname,dname); 
}


void file_panel::set_load_object(const file_panel_handle_base& f)
{ load_ptr = &f; }

void file_panel::set_open_object(const file_panel_handle_base& f)
{ open_ptr = &f; }

void file_panel::set_save_object(const file_panel_handle_base& f)
{ save_ptr = &f; }

void file_panel::set_cancel_object(const file_panel_handle_base& f)
{ cancel_ptr = &f; }



void file_panel::init_panel()
{
  if (panel_init++) return;

  W.set_fixed_font();
  int tw = W.real_to_pix(W.text_width("H"));
  int th = W.real_to_pix(W.text_height("H"));

  yskip = int(1.7*th);


  if (descr_string.tail(1) != "|") descr_string += "|";
  if (pat_string.tail(1) != "|") pat_string += "|";

  descr_list.clear();
  pat_list.clear();

  int p1 = 0;
  int p2 = 0;
  while (p1 < descr_string.length() && p2 < pat_string.length())
  { int q1 = descr_string.index("|",p1);
    int q2 = pat_string.index("|",p2);
    string descr = descr_string(p1,q1-1);
    string pat =  pat_string(p2,q2-1).replace_all(";"," ");
    descr_list.append(descr);
    pat_list.append(pat);
    p1 = q1+1;
    p2 = q2+1;
  }

/*
  pat_list.append("*.*");
  descr_list.append("All Files (*.*)");
*/

  pat_string = pat_list.front();
  descr_string = descr_list.front();

  string title;

  if ((load_handler || load_ptr) && (save_handler || save_ptr))
     title = "File Panel";
  else
  { if (load_handler || load_ptr) title = load_string;
    if (save_handler || save_ptr) title = save_string;
   }

  //if (xhost != "") title = xhost;

  W.set_frame_label(title);

  //W.set_auto_button_layout(false);

  dir_item = W.string_item(" Folder",*dir_name,global_dir_list,10,change_dir);

  W.set_item_label_width(dir_item,int(10*tw));
  W.set_item_width(dir_item,50*tw);

  W.set_button_height(40);

  W.button(" < ",file_back);
  W.button(" > ",file_forward);

  if (window::display_type() == "xx") 
  { W.disable_item(dir_item);
    W.disable_button(file_back);
    W.disable_button(file_forward);
  }


  W.make_menu_bar();

  file_item = W.string_item(" Selection",*file_name);
  W.set_item_label_width(file_item,int(10.35*tw));
  W.set_item_width(file_item,50*tw);

  //W.vspace(yskip/2);
  W.text_item("");

  pat_item = W.string_item(" Filter",descr_string,descr_list,10,change_filter);
  W.set_item_label_width(pat_item,int(10.35*tw));
  W.set_item_width(pat_item,25*tw);

  panel_item it = W.bool_item("                show hidden files",show_hidden,
                                                                 change_hidden);
  W.set_item_label_width(it,int(4*tw));

  // buttons
  W.button(".x");
  W.button(".x");
  W.button(".x");

  if (load_handler || load_ptr) W.fbutton("Load",file_load);
  if (save_handler || save_ptr) W.button("Save",file_save);
  if (open_handler || open_ptr) W.button("Open",file_open);

  W.button("Cancel",file_cancel);

}

void file_panel::update(string dname, string fname)
{ 
  if (dname != "") 
  { 
    if (dname == "..")
    { int p = dir_name->last_index(sep);
      if (p == 0) p = 1; // unix /abc
      if (p == 2 && dir_name->char_at(1) == ':') p = 3; // windows D:\abc
      *dir_name = dir_name->operator()(0,p-1);
    }
    else
    { int p = dname.last_index(":");
      if (p != -1) p = dname.last_index(":",p-1);
      if (p == -1) p = dname.length();
      dname = dname(0,p-1);

      if (dname.starts_with(sep) || dname[1] == ':') // complete path
        *dir_name = dname;
      else
      { if (!dir_name->ends_with(sep)) *dir_name += sep;
        *dir_name += dname;
       }
     }

    if (load_handler || load_ptr) *file_name = "untitled";

    W.redraw_panel();
  }

  if (fname != "")
  { int p = fname.index(":");
    if (p == -1) p = fname.length(); 
    *file_name = fname(0,p-1);
   }

  if (xhost == "" && !is_directory(*dir_name))
  { 
    dir_list.clear();
    string s = dir_name->replace_all("\\","/");

    panel P("File Panel");
    P.text_item("");
    P.text_item(string("\\bf\\blue %s:~~no such directory.",~s));
    P.text_item("");
    P.fbutton("continue");
    P.display(W,window::center,window::center);
    P.read();

    file_list.clear();
    return;
  }


  string dir = *dir_name;

  if (window::display_type() == "xx" && dir == start_dir) 
    W.disable_button(file_back);
  else
    W.enable_button(file_back);

  if (dir.length() == 2 && dir.ends_with(":")) dir += sep;

  if (global_dir_list.search(dir) == nil) 
  { global_dir_list.append(dir);
    global_dir_list.sort();
   }

/*
  // crash if update is called from current dir_item menu
  W.set_menu(dir_item,global_dir_list,8);
*/

  if (dname != "") 
  { // new directory: update  file and directory list

    list<string> L = xget_files(*dir_name,1);
    file_list.clear();

    if (pat_string == "" || pat_string == "*" || pat_string  == "*.*")
       file_list = L;
    else
    { string s;
      forall(s,L)
      { string pat;
        forall_words(pat,pat_string)
        { pat = pat.del("*"); // *.suf --> .suf
          if (s.index(pat) != -1) file_list.append(s);
         }
      }
    }


    if (!show_hidden) 
    { // delete hidden files 
      list_item it;
      forall_items(it,file_list)
      { string s = file_list[it];
        if (s[0] == '.') file_list.del_item(it);
       }
     }

/*
    // mark special files
    forall_items(it,file_list)
    { string s = file_list[it];
      string path = *dir_name + sep + s;
      // symbolic links
      if (is_link(path)) file_list[it] = s + "@";
     }
*/

    if (file_list.empty()) file_list.append("No matching files.");

    file_list.sort(cmp_files);

    dir_list.clear();

    list<string> tmp_dir_list = xget_files(*dir_name,2);
    tmp_dir_list.sort(cmp_files);

    //dir_list.push("..");

    string d;
    forall(d,tmp_dir_list) {
       if (show_hidden || d[0] != '.') dir_list.append(d);
    }

    scroll_off = 0; // reset scrollbar if directory changed
  }

  int dnum = dir_list.length();
  int fnum = file_list.length();
  if (fnum == 1 && file_list.front().starts_with("No matching")) fnum--;

  string status_line = " ";

  if (xhost == "")
    status_line += get_host_name();
  else
    status_line +=  xhost;

  status_line += "    ";

  if (dnum == 1)
    status_line += string("%d directory", dnum);
  else
    status_line += string("%d directories", dnum);

  status_line += "    ";

  if (fnum == 1)
    status_line += string("%d file", fnum);
  else
    status_line += string("%d files", fnum);

  W.set_status_string(status_line);

  int len = dir_list.length() + file_list.length();

  double h = W.real_to_pix(W.ymax() - W.ymin());
  double sb_sz = h/(yskip*len);




  if (sb_sz < 1)
    W.open_scrollbar(scroll_up,scroll_down,scroll_drag,sb_sz,0);
  else
    W.close_scrollbar();

/*
  if (load_handler || load_ptr) { 
    // load (use first matching file as selection)
    if (!file_list.empty()) *file_name = file_list.front();
   }
  else
  { //save
    string s = *file_name;
    int p = s.length()-1;
    while (p >= 0 && s[p] != '.') p--;
    if (p >= 0) s = s(0,p-1);
    if (s != "") s += pat_string.del("*");
    *file_name = s;
  }
*/

  if (W.is_open()) 
  { W.redraw_panel();
    redraw(&W);
  }
}


void file_panel::x_open(int xpos, int ypos)
{ 
  init_panel();

  W.display(xpos,ypos);


/*
  redraw(&W);
*/

  W.open_status_window();
  W.set_status_string("Status Line");
  W.set_topmost();

  W.set_fixed_font();

  int tw = W.real_to_pix(W.text_width("H"));

  int wi = 73*tw;
  int he = 80*tw;
  int dx = (wi - W.width())/2; 
  int dy = (he - W.height())/2; 
  W.resize(W.xpos()-dx,W.ypos()-dy,wi,he);

  redraw(&W);

  file_panel_closed = false;


  update(*dir_name,"");


  for(;;) 
  { 
    int but = -1;
    double x,y;
    unsigned long t;

    int event = W.read_event(but,x,y,t);

    if (file_panel_closed) {
       but = file_cancel;
       break;
    }

    if (event != button_press_event) continue;

    if (but == MOUSE_BUTTON(4)) { 
      scroll_up(0);
      continue;
    }

    if (but == MOUSE_BUTTON(5)) { 
         scroll_down(0);
         continue;
    }

    if (but == MOUSE_BUTTON(1))
    { int d = int((W.ymax() + scroll_off - y)/W.pix_to_real(yskip));

      assert(d >= 0);

      string fn;
   
      if (d < dir_list.length())
      { list_item it = dir_list.get_item(d);
        fn = dir_list[it];
       }
      else
      { int dd = d - dir_list.length();
        if (dd < file_list.length())
        { list_item it = file_list.get_item(dd);
          string s = file_list[it];
          if (s != "No matching files.") fn = dir_list[it];
         }
       }

      selected_dir = fn;
      redraw(&W);
      while (W.read_event(but,x,y,t) != button_release_event);
      selected_dir = "";

      if (d < dir_list.length())
      { update(fn,"");
        dir_stack.clear();
        W.disable_button(file_forward);
       }
      else
        update("",fn);

      continue;
    }

    if (but == file_back) 
    { if (*dir_name != sep && !dir_name->ends_with(":" + sep))
      { dir_stack.push(*dir_name);
        W.enable_button(file_forward);
        update("..","");
       }
      continue;
    }

    if (but == file_forward) 
    { if (!dir_stack.empty())
      { string d = dir_stack.pop();
        if (dir_stack.empty()) W.disable_button(file_forward);
        update(d,"");
       }
      continue;
    }

    string fn = *dir_name + sep + *file_name;

    if (but == file_save) {
      // add suffix from pattern string
      fn += pat_string.tail(-1); // remove leading '*' as in '*.gpx'
    }
  
    if (but == file_load)
    { if (xhost == "") W.close();
      if (load_ptr) 
         load_ptr->operator()(fn);
      else 
         load_handler(fn);
      if (xhost == "") break;
     }

    if (but == file_open)
    { //if (xhost == "") W.close();
      if (open_ptr) 
         open_ptr->operator()(fn);
      else 
         open_handler(fn);
      //if (xhost == "") break;
      continue;
     }
  
    if (but == file_save)
    { if (xhost == "") W.close();
      if (save_ptr) 
        save_ptr->operator()(fn);
      else 
        save_handler(fn);
      if (xhost == "") break;
     }

    if (but == file_cancel) break;
  }

  W.close();
  if (wpp) wpp->redraw();

}


void file_panel::x_open() { x_open(window::center,window::center); }



void file_panel::ms_open()
{ 
  int dw = wpp ? wpp->draw_win : 0;

  char dname[128];
  strcpy(dname,*dir_name);

  char fname[128];
  strcpy(fname,*file_name);

  string filter;

  if (descr_string.tail(1) != "|") descr_string += "|";
  if (pat_string.tail(1) != "|") pat_string += "|";

  int p1 = 0;
  int p2 = 0;
  while (p1 < descr_string.length() && p2 < pat_string.length())
  { int q1 = descr_string.index("|",p1);
    int q2 = pat_string.index("|",p2);
    filter += descr_string(p1,q1);
    filter += pat_string(p2,q2);
    p1 = q1+1;
    p2 = q2+1;
  }

  for(int i=0; i < filter.length(); i++)
    if (filter[i] == '|') filter[i] = '\0';


  string def_ext = pat_string.del("*.");

  if (load_handler || load_ptr)
  { 
    if (x_choose_file(dw,0,load_string,filter,def_ext,dname,fname)) 
    { *file_name = fname;
      *dir_name = dname;
      string fn = *dir_name + sep + *file_name;
      if (load_ptr) 
         load_ptr->operator()(fn);  
      else 
         load_handler(fn);
     }
   }

  if (save_handler || save_ptr)
  { 
    if (x_choose_file(dw,1,save_string,filter,def_ext,dname,fname))
    { *file_name = fname;
      *dir_name = dname;
      string fn = *dir_name + sep + *file_name;
      if (save_ptr) 
         save_ptr->operator()(fn);    
      else 
         save_handler(fn);
     }
   }

}



void file_panel::ms_open(int, int) { ms_open(); }


LEDA_END_NAMESPACE
