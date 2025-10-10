/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  file_panel.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_FILE_PANEL_H
#define LEDA_FILE_PANEL_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721277
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graphics/window.h>
#include <LEDA/core/stack.h>

LEDA_BEGIN_NAMESPACE


typedef void (*file_panel_handler)(string);
typedef void (*file_panel_handler2)(string,string);

// handler object ...
class __exportC file_panel_handle_base {
protected:
  file_panel_handler  FPH;
public:
  file_panel_handle_base() { FPH = NULL; }
  file_panel_handle_base(file_panel_handler f) { FPH = f; }
  virtual ~file_panel_handle_base() {}
  
  virtual void operator()(string s) const
  { if (FPH != NULL) FPH(s); }
};


class __exportC file_panel {

window W;

window* wpp; // parent

string* dir_name;
string* file_name;

list<string> global_dir_list;
list<string> dir_list;
list<string> file_list;


panel_item dir_item;
panel_item file_item;
panel_item pat_item;


string start_dir;
string home_dir;

string selected_dir;

string load_string;
string save_string;
string pat_string;
string descr_string;

list<string> descr_list;
list<string> pat_list;

int  panel_init;

bool mswin;

bool show_hidden;


file_panel_handler load_handler;
file_panel_handler open_handler;
file_panel_handler save_handler;
file_panel_handler cancel_handler;

//load/save/cancel objects ...
const file_panel_handle_base* load_ptr; 
const file_panel_handle_base* open_ptr; 
const file_panel_handle_base* save_ptr; 
const file_panel_handle_base* cancel_ptr; 

stack<string> dir_stack;

public:

file_panel_handler2 drop_handler;

string xhost;
int    yskip;
string sep;

void update(string dname, string fname);

private:

void init(window*,string& ,string&);

static void change_filter(char*);
static void change_dir(char*);
static void change_hidden(int);

/*
static void update_dir_menu(int);
static void update_file_menu(int);
*/


list<string> xget_files(string dir, int what);

public:

 //panel& get_panel() { return (panel&)P; }
 window& get_window() { return W; }

 void init_panel();

 file_panel(string& fname, string& dname);
 file_panel(string& fname, string& dname,int width, int height);
 file_panel(window& W, string& fname, string& dname);
~file_panel();

 void set_xhost(string s) { 
   if (s == "") return;
   xhost = s; 
   sep = "/";
   global_dir_list.clear();
   global_dir_list.append("/home/naeher");
   global_dir_list.append("/disk1");
   global_dir_list.append("/local");
   global_dir_list.append("/LEDA");
 }


 void set_load_handler(file_panel_handler f)  { load_handler  = f;  }
 void set_open_handler(file_panel_handler f)  { open_handler  = f;  }
 void set_save_handler(file_panel_handler f)  { save_handler  = f;  }
 void set_drop_handler(file_panel_handler2 f)  { drop_handler  = f;  }
 void set_cancel_handler(file_panel_handler f){ cancel_handler = f; }
 
 void set_load_object(const file_panel_handle_base& f);
 void set_open_object(const file_panel_handle_base& f);
 void set_save_object(const file_panel_handle_base& f);
 void set_cancel_object(const file_panel_handle_base& f);

 void set_load_string(string s) { load_string = s; }
 void set_save_string(string s) { save_string = s; }
 void set_pattern(string s)     { descr_string = pat_string = s; }

 void set_pattern(string s1, string s2) { 
    descr_string = s1 + "  (" + s2 + ")";
    pat_string = s2; 
 }

 void add_pattern(string s1, string s2) { 
    descr_string += s1 + "  (" + s2 + ")|"; 
    pat_string += s2 + "|"; 
 }


 void set_mswin(bool b) { mswin = b; }

 void set_frame_label(string s) { W.set_frame_label(s); }
 
 string get_pattern() const { return pat_string; }

 const list<string>& get_file_list() const { return file_list; }
 const list<string>& get_dir_list() const { return dir_list; }

 string get_file_name() const { return *file_name; }
 string get_dir_name() const { return *dir_name; }

 string get_selected_dir() const { return selected_dir; }

 void x_open();
 void x_open(int x, int y);

 void ms_open();
 void ms_open(int x, int y);

 void open()             { if (mswin) ms_open();    else x_open();    }
 void open(int x, int y) { if (mswin) ms_open(x,y); else x_open(x,y); }

};

#if LEDA_ROOT_INCL_ID == 721277
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif



LEDA_END_NAMESPACE

#endif
