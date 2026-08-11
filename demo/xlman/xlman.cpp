/*******************************************************************************
+
+  LEDA 6.6.1  
+
+
+  xlman.c
+
+
+  Copyright (c) 1995-2020
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>
#include <LEDA/core/d_array.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>

/*
#include <LEDA/internal/std/stdio.h>
#include <LEDA/internal/std/stdlib.h>
*/

#include <LEDA/graphics/pixmaps/button32.h>
#include <LEDA/graphics/pixmaps/earth.xpm>


using namespace leda;

using std::cerr;
using std::endl;
using std::ifstream;


int help_but = 0;

enum { XLMAN_SEC = 1, XLMAN_DATE = 2 };
enum { MENU_TABLE = 0, MENU_SCROLLBOX = 1 };

string text_edit_cmd;

const char* small_clock_font = "T48";
const char* large_clock_font = "T64";

#if defined(__win32__) || defined(__CYGWIN32__)
const char* exe_suffix = ".exe";
#else
const char* exe_suffix = "";
#endif

static const string delim = get_directory_delimiter();

#if defined(__win32__)

static string grep_cmd = "grep -d -l %S %p";
static string lpr_cmd  = "dvips -q -o !'lpr -h' %p";
static string xdvi_cmd = "yap %p";
static string html_browser_cmd = "open %p";

inline string make_dir(string path) { return path.replace_all("/", delim); }
inline string quote_wildcards(string filter) { return filter; }

#else

static string grep_cmd = "grep -r -l %S %p";
static string lpr_cmd  = "dvips -q -o !'lpr -h' %p";
static string xdvi_cmd = "xdvi -s 7 %p";
static string html_browser_cmd = "xdg-open %p";

inline string make_dir(string path) { return path; }
inline string quote_wildcards(string filter) { return "'" + filter + "'"; }

#endif



static panel main_panel;

static panel_item leda_menu_item;
static panel_item add_menu_item;

//static window clock_win(150,50);
static window clock_win(180,50);

/*
#if defined(__win32__)
static window clock_win(62,20);
#else
static window clock_win(72,20);
#endif
*/

//static int menu_style = MENU_TABLE;

static int menu_style = MENU_SCROLLBOX;

static int clock_display = XLMAN_DATE | XLMAN_SEC;

//static color clock_fg_color = blue;
static color clock_fg_color = 0x0000a0;
static color clock_bg_color = grey1;

static string leda_root;


static string leda_dtype;
static string add_dtype;

static string leda_dvi_dir;
static string leda_html_dir;
static string add_dvi_dirs;

static list<string> M;     // man pages menu
static list<string> Mplus; // man pages menu LEPS
static bool dviadd = false;
static d_array<string,string> DviPath;


static string win_label = "LEDA Manual";

static string demo_cmd[256];

static string src_file[256];

static bool show_frame = true;

string alarm_tm  = "00:00";
string alarm_cmd = "";



void null_del_handler(window*)  {}

void win_del_handler(window*) 
{ panel P("acknowlege");
  P.text_item("");
  P.text_item("\\bf\\blue You really want to quit xlman~?");
  P.fbutton("no",0);
  P.button("yes",1);
  if (P.open(main_panel) == 1) exit(0);
}


static int open_panel(panel& p)
{ int x0,y0,x1,y1;
  main_panel.frame_box(x0,y0,x1,y1);
  return p.open(p,x0,y1+1);
 }


static bool file_ok( string fname)
{ ifstream fstr(fname);
  if (fstr.fail()) 
  { error_handler(0,"xlman: Cannot open " + fname);
    return false;
   }
  return true;
}

static void set_label(string s)
{ //s = s.iso8859_to_utf();
  main_panel.set_frame_label(s); 
  main_panel.set_icon_label("LEDA Manual");
  main_panel.flush();
}

static string basename(string path, string& fname)
{ int len = path.length();
  int n = len - 1;
  while (path[n] != '/' && path[n] != '\\') n--;
  fname = path(n+1,len-1);
  return path(0,n-1);
}

static string header_basename(string path)
{
	string base;
	basename(path, base);
	int pos = base.index(".h");
	if (pos < 0) return string();
	return base(0,pos+1); // remove everything after ".h"
}

static string remove_extension(string file_name, string& ext)
{
	int pos_dot = file_name.length()-1;
	while (pos_dot >= 0 && file_name[pos_dot] != '.') --pos_dot;
	if (pos_dot < 0) { ext = ""; return file_name; }
	ext = file_name(pos_dot+1, file_name.length()-1);
	return file_name(0, pos_dot-1);
}

static string remove_extension(string file_name)
{
	string dummy;
	return remove_extension(file_name, dummy);
}

static string eval_cmd(string cmd, string path, string search_expr = string())
{
  string fname, ext;
  string dname = basename(path,fname); 
  remove_extension(fname, ext);

  cmd = cmd.replace_all("%p", path);
  cmd = cmd.replace_all("%f", fname);
  cmd = cmd.replace_all("%d", dname);
  cmd = cmd.replace_all("%e", ext);
  cmd = cmd.replace_all("%S", search_expr);
  cmd = cmd.replace_all("%F", path);
  return cmd;
}

static void show_dvi_file(string path)
{
  if (file_ok(path))
  { set_label("Starting DVI Previewer"); 
    create_process(eval_cmd(xdvi_cmd, path));
    leda_wait(3);
    set_label(win_label); 
  }
}

static void show_html_file(string path)
{
  if (file_ok(path))
  { set_label("Starting HTML Browser"); 
    string cmd = eval_cmd(html_browser_cmd, path);
    create_process(cmd);
    leda_wait(3);
    set_label(win_label); 
  }
}

static void show_text_file(string path)
{
  if (file_ok(path)) {
    set_label("Starting File Viewer"); 

    string cmd = eval_cmd(text_edit_cmd, path);
//    cerr << cmd << endl;
    create_process(cmd);
    leda_wait(3);
    set_label(win_label); 
  }
}

static void view(int)
{ 
  string fname;

  //if (leda_menu_item == main_panel.active_string_item())
  if (leda_menu_item == main_panel.get_active_item())
    { if (leda_dtype == version_string)
         fname = leda_dvi_dir + delim + "Introduction.dvi";
      else
         fname = leda_dvi_dir + delim + leda_dtype + ".dvi";
     }
  else
    fname = DviPath[add_dtype] + delim + add_dtype + ".dvi";

  show_dvi_file(fname);
}


static void print(int)
{ string fname;

  //if (leda_menu_item == main_panel.active_string_item())
  if (leda_menu_item == main_panel.get_active_item())
    { if (leda_dtype == version_string)
         fname = leda_dvi_dir + delim + "Introduction.dvi";
      else
         fname = leda_dvi_dir + delim + leda_dtype + ".dvi";
     }
  else
    fname = DviPath[add_dtype] + delim + add_dtype + ".dvi";

  if (file_ok(fname))
  { panel pr_panel("LEDA Manual Printer",main_panel.width(),-1);
    pr_panel.string_item("file",fname);
    pr_panel.string_item("print cmd",lpr_cmd);
    pr_panel.button("print",0);
    pr_panel.button("cancel",1);
  
    if (open_panel(pr_panel) == 0)
    { set_label(fname);
      create_process(eval_cmd(lpr_cmd, fname));
      set_label(win_label); 
    }
  }
}


static void show_text(int i)
{ 
  show_text_file(src_file[i]);
}


static void about_xlman(int) 
{ window* wp = window::get_call_window();
  wp->display_help_text("xlman");
}



static void start_demo(int i) 
{ string cmd;
  string dname = basename(demo_cmd[i],cmd);
  set_label("Running  " + cmd); 
  cerr << demo_cmd[i] << endl;
  create_process(demo_cmd[i]);
  leda_wait(3);
  set_label(win_label); 
}

static void start_ascii(int i) 
{ string rcmd = leda_root + delim + "Manual" + delim + "cmd" + delim + "runascii";
  string dcmd = demo_cmd[i];
#if defined(__unix__)
  string cmd = "xterm -sb -sl 500 -T " + dcmd + " -e " + rcmd + " " + dcmd;
#else
  string cmd = rcmd + ".bat " + dcmd;
#endif
  cerr << dcmd << endl;
  set_label("Running  " + dcmd);
  create_process(cmd);
  leda_wait(3);
  set_label(win_label); 
}





const char* month[] =
{ "Januar",
  "Februar",
  "M\344rz",
  "April",
  "Mai",
  "Juni",
  "Juli",
  "August",
  "September",
  "Oktober",
  "November",
  "Dezember"
};


const char* week_day[] =
{ "Sonntag",
  "Montag",
  "Dienstag",
  "Mittwoch",
  "Donnerstag",
  "Freitag",
  "Samstag"
};


static void display_time(window* wp)
{ 
  time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 


  if (T->tm_hour + T->tm_min == 0)
  { win_label = string("%s  %d. %s %d", week_day[T->tm_wday], 
                                        T->tm_mday,
                                        month[T->tm_mon],
                                        T->tm_year+1900);
    set_label(win_label); 
   }


  wp->clear();

  int minutes = 0;
  int percent = 0;
  int status = 0;

/*
  if (apm_get_status(status,percent,minutes))
  { string msg;
    color c1,c2;
    if (status == 1) 
     { // AC on-line
       msg = string("charging  (%d%%)",percent);
       c1 = blue;
       c2 = ivory;
      }
    else
     { // AC off-line
       msg = string("%d:%02d  (%d%%)",minutes/60,minutes%60,percent);
       c1 = green;
       c2 = red;
      }
 
    // main_panel.set_button_help_str(help_but,msg); 
 
    double x0 = wp->xmin();
    double y0 = wp->ymin();
    double x1 = wp->xmax();
    double y1 = y0 + wp->pix_to_real(2);
    double x = x0 + (x1-x0)*float(percent)/100;
    wp->draw_box(x0,y0,x,y1,c1);
    wp->draw_box(x, y0,x1,y1,c2);
   }
*/

  string tm_str;

  if (clock_display & XLMAN_SEC)
  { tm_str = string("%2d:%02d:%02d",T->tm_hour,T->tm_min,T->tm_sec);

    wp->set_font(small_clock_font);
    //wp->draw_ctext(tm_str);

    int dx = 0;
    if (T->tm_hour < 10)
      wp->draw_text(17,27, string("%d",T->tm_hour));
    else
      wp->draw_text(5,27, string("%d",T->tm_hour));
    wp->draw_text(39, 27, string("%02d",T->tm_min));
    wp->draw_text(73, 27, string("%02d",T->tm_sec));

    wp->set_font("T34");
    wp->draw_text(33.5,24.0,":");
    wp->draw_text(67.0,24.0,":");
   }
  else
  { tm_str = string("%2d:%02d",T->tm_hour,T->tm_min);
    wp->set_font(large_clock_font);
    wp->draw_ctext(tm_str);
   }

  wp->flush_buffer();

  main_panel.set_icon_label("xlman " + tm_str);

  if (alarm_cmd != "" && (T->tm_sec == 0 || (clock_display & XLMAN_SEC) == 0))
  { if (alarm_tm == string("%02d:%02d",T->tm_hour,T->tm_min))
    { set_label("Alarm at " + alarm_tm);
      string cmd = alarm_cmd;
      if (cmd[0] == '#')
        cmd = leda_root + delim + "demo" + delim + "xlman" + delim + "xlman_msg " + alarm_tm 
            + cmd.replace("#"," ");
      create_process(cmd);
     }
    else set_label(win_label); 
   }
}


void set_clock_fg_color(int col)
{ clock_win.set_color(col);
  display_time(&clock_win);
}

void set_clock_bg_color(int col)
{ clock_win.set_bg_color(color(col));
  if (col == grey1)
    clock_win.set_border_color(grey1);
  else
    clock_win.set_border_color(black);
  display_time(&clock_win);
}


void start_clock(int disp) 
{ 
  clock_win.stop_timer();

  if (disp & XLMAN_DATE) 
  { //get date and assign it to win_label
    time_t clock; 
    time(&clock);
    tm* T = localtime(&clock); 
    win_label = string("%s  %d. %s %d", week_day[T->tm_wday], 
                                        T->tm_mday,
                                        month[T->tm_mon],
                                        T->tm_year+1900);
   }
  else
    win_label = "LEDA Manual";

/*
  if (disp & XLMAN_SEC)
    clock_win.start_timer(1000,display_time);
  else
    clock_win.start_timer(60000,display_time);
*/

  set_label(win_label);

  clock_display = disp;

  display_time(&clock_win);
}


static int cmp_files(const string& f1, const string& f2)
{ return compare(f1.to_lower(),f2.to_lower()); }

static list<string> get_leda_dvi_list(string leda_dir)
{
  list<string> Ldir;
  if (!is_directory(leda_dir)) {
     cerr << "xlman: Directory " + leda_dir + " does not exist.\n";
  }
  else {
    Ldir = get_files(leda_dir,"*.dvi");
    list_item fit;
    forall_items(fit,Ldir) Ldir[fit]=Ldir[fit].del(".dvi");
  }
  Ldir.sort(cmp_files);
  Ldir.push(version_string);
  return Ldir;
}

static list<string> get_dvi_list(string dvi_dir_str)
{
   string dvi_dirs = dvi_dir_str + ":";

   DviPath.clear();

   list<string> Lall;

   int start = 0;

   for (int i=0; i<dvi_dirs.length(); i++)
   {
     if (dvi_dirs[i] != ':') continue;

     string cur_dir = dvi_dirs(start,i-1);
     start = i+1;

     if (!is_directory(cur_dir))
         cerr << "xlman: Directory " + cur_dir + " does not exist.\n";
     else
       { list<string> Ldir = get_files(cur_dir,"*.dvi");
         list_item fit;
         forall_items(fit,Ldir)
         { string s = Ldir[fit].del(".dvi");
#if !defined(_MSC_VER)
           DviPath[s]=cur_dir;
#endif
           Lall.append(s);
         }
       }
     }

  Lall.sort();
  return Lall;
}


void set_main_panel_frame(int frame)
{ main_panel.close();
  if (frame)
     main_panel.display(0,0);
  else
     main_panel.display(main_panel,0,0);

 }

  

static void setup(int)
{ 
  list<string> choices;
  choices.append("seconds");
  choices.append("date");

  int xxx = 0;
/*
  if (apm_get_status(xxx,xxx,xxx)) choices.append("battery");
*/


  panel setup_panel("xlman options",main_panel.width(),-1);


  setup_panel.text_item("");
  setup_panel.text_item("\\bf\\blue Manual");
  setup_panel.string_item("LEDA dvi files",leda_dvi_dir);
  if ( dviadd )
    setup_panel.string_item("Others",add_dvi_dirs);
  setup_panel.string_item("LEDA html files",leda_html_dir);

  setup_panel.text_item("");
  setup_panel.text_item("\\bf\\blue Tools & Viewers");
  setup_panel.text_item("");
  setup_panel.text_item("(%p=path / %f=file / %d=dir / %e=ext / %S=search pattern)");
  setup_panel.text_item("");
  setup_panel.string_item("grep cmd",grep_cmd);
  setup_panel.string_item("view dvi",xdvi_cmd);
  setup_panel.string_item("print dvi",lpr_cmd);
  setup_panel.string_item("browse html",html_browser_cmd);
  setup_panel.string_item("edit text",text_edit_cmd);

  setup_panel.text_item("");
  setup_panel.text_item("\\bf\\blue Window");
  setup_panel.bool_item("frame",     show_frame, set_main_panel_frame);
  setup_panel.choice_item("menu",menu_style,"table","scrollbox");


  setup_panel.text_item("");
  setup_panel.text_item("\\bf\\blue Clock");
  setup_panel.choice_mult_item("display",clock_display,choices,start_clock);
  setup_panel.color_item("color",clock_fg_color,set_clock_fg_color);
//setup_panel.color_item("bgcol",clock_bg_color,set_clock_bg_color);

  setup_panel.text_item("");
  setup_panel.text_item("\\bf\\blue Alarm");
  setup_panel.string_item("hh:mm",  alarm_tm);
  setup_panel.string_item("command",alarm_cmd);

  setup_panel.fbutton("continue",1);
//setup_panel.button("iconify",2);
  setup_panel.button("exit xlman",0);

  setup_panel.set_window_close_handler(null_del_handler);

//setup_panel.set_border_color(grey1);

  switch (open_panel(setup_panel)) {
    case 0 : //win_del_handler(&main_panel);
             exit(0);
             break;
    case 2 : main_panel.iconify();
             break;
  }

  DviPath.clear(); // reset parsed paths

  M = get_leda_dvi_list(leda_dvi_dir);
  if (M.empty()) 
     leda_dtype = "none";
  else
     leda_dtype = M.head();
  switch (menu_style) {
     case MENU_TABLE:     main_panel.add_menu(leda_menu_item,M);
                          break;
     case MENU_SCROLLBOX: main_panel.add_menu(leda_menu_item,M,12);
                          break;
  }

  if ( dviadd ) {
     Mplus = get_dvi_list(add_dvi_dirs);
     if (Mplus.empty()) 
       add_dtype = "none";
     else 
       add_dtype = Mplus.head();
     switch (menu_style) {
     case MENU_TABLE:     main_panel.add_menu(add_menu_item,Mplus);
                          break;
     case MENU_SCROLLBOX: main_panel.add_menu(add_menu_item,Mplus,12);
                          break;
     }
  }

  main_panel.redraw_panel();
}

static void show_grep_hits(const list<string>& hits);
static void extract_grep_path(string& hit);

static void grep(int)
{
	string search_expr;

	panel grep_panel("find class/function",main_panel.width(),-1);

	grep_panel.text_item("");
	grep_panel.text_item("\\bf\\blue Search Expression");
	grep_panel.string_item("", search_expr);
	
	grep_panel.fbutton("start",1);
	grep_panel.button("cancel",0);
	grep_panel.set_window_close_handler(null_del_handler);

	if (open_panel(grep_panel) == 0) return;

	string leda_headers = leda_root + delim + "incl" + delim + "LEDA";
	string grep_output_file_name = tmp_file_name();

	string cmd = eval_cmd(grep_cmd + " > " + grep_output_file_name, leda_headers, search_expr);
//	cerr << cmd << endl;

	list<string> hits;

	int result = system(cmd);

        if ((result & 0xFF) == 0)
	{
		ifstream grep_output(grep_output_file_name);
		string line;
		while (grep_output) {
			line.read_line(grep_output);
			if (!grep_output || line == "") break;
			hits.append(line);
		}
	}
	delete_file(grep_output_file_name);

	list_item it;
	forall_items(it, hits) {
		extract_grep_path(hits[it]);
		if (hits[it].contains("PREFIX.h") || hits[it] == "") hits.del_item(it);
	}

	if (! hits.empty()) show_grep_hits(hits);
	else {
		panel no_hits_panel("acknowledge");

		no_hits_panel.text_item("");
		no_hits_panel.text_item("\\bf\\blue No hits found!");
		no_hits_panel.fbutton("ok", 0);
		no_hits_panel.open(main_panel);
	}
}

static void extract_grep_path(string& hit)
{
	if (hit.index("grep:") == 0) { hit = ""; return; }
	if (hit.index("File ") == 0) {
		int start = 5, end = hit.length()-1;
		if (hit[end] == ':') --end;
		hit = hit(start, end);
	}
}

static void update_hits_panel(string hit, window& hits_panel)
{
//	cerr << hit << endl;
	string html_file = make_dir(leda_html_dir + delim + remove_extension(hit) + ".html");
	if (is_file(html_file)) hits_panel.enable_button(1);
	else				    hits_panel.disable_button(1);
	string dvi_file = make_dir(leda_dvi_dir + delim + remove_extension(hit) + ".dvi");
	if (is_file(dvi_file)) hits_panel.enable_button(2);
	else				   hits_panel.disable_button(2);	
}

static void hits_panel_select_action(char* hit)
{
	update_hits_panel(hit, *window::get_call_window());
}


static void show_grep_hits(const list<string>& hits_full_path)
{
// 	hits.print();
	panel hits_panel("hits",main_panel.width(),-1);

	hits_panel.text_item("");
	hits_panel.text_item("\\bf\\blue View Hit");

	list<string> hits;
	string hit;
	forall(hit, hits_full_path) hits.append( header_basename(hit) );
	hits.remove("PREFIX.h");
	hits.remove("");
	hits.sort();

	string selected_hit = hits.head();
	if (menu_style == MENU_SCROLLBOX) 
		hits_panel.string_item("", selected_hit, hits, 12, hits_panel_select_action);
	else
		hits_panel.string_item("", selected_hit, hits, hits_panel_select_action);

	hits_panel.button("html", 1);
	hits_panel.button("dvi", 2);
	hits_panel.fbutton("text", 3);
	hits_panel.button("done", 0);

	update_hits_panel(selected_hit, hits_panel);

	int x0,y0,x1,y1;
	main_panel.frame_box(x0,y0,x1,y1);
	hits_panel.display(hits_panel, x0, y1+1);

	int button;
	while ( (button = hits_panel.read()) ) {
		string path_selected_hit_h;
		forall(hit, hits_full_path) {
			if (header_basename(hit) == selected_hit) {
				path_selected_hit_h = hit;
				break;
			}
		}

		string path_selected_hit_html = make_dir(leda_html_dir + delim + remove_extension(selected_hit) + ".html");
		string path_selected_hit_dvi = make_dir(leda_dvi_dir + delim + remove_extension(selected_hit) + ".dvi");

		switch (button) {
			case 1: show_html_file(path_selected_hit_html); break;
			case 2: show_dvi_file(path_selected_hit_dvi); break;
			case 3: show_text_file(path_selected_hit_h); break;
		}
	}
}

void build_menu(string dir, menu& demo_menu, menu& src_menu, 
                                                   int& i, int& j,
                                                   void (*start)(int))
{
  string demo_dir = leda_root + delim + "demo";
  menu* pm = new menu;
  menu* sm = new menu;

  list<string> src_files = get_files(demo_dir + delim + dir, "*.cpp");
  src_files.sort();
  int count = 0;
  string s;
  forall(s,src_files) 
  { string cmd = s.replace(".cpp","");
    if (!is_file(demo_dir + delim +dir + delim + cmd + exe_suffix)) continue;
    sm->button(s,i,show_text);
    src_file[i++] = demo_dir + delim + dir + delim + s;
    pm->button(cmd,j,start);
    demo_cmd[j++] = demo_dir + delim + dir + delim + cmd;
    count++;
   }

  if (count > 0)
    { demo_menu.button(dir,*pm);
      src_menu.button(dir,*sm);
     }
  else
    { delete pm;
      delete sm;
     }
}




#if defined(WINMAIN)

int main() {
  int    argc;
  char** argv;
  get_command_line(argc,argv);

#else

int main(int argc, char** argv) {

#endif

 if (argc > 1 && string(argv[1]) == "-f") show_frame = false;

 if (!get_environment("LEDAROOT",leda_root)) {
   error_handler(1,"xlman: LEDAROOT (environment variable) not defined.");
   exit(1);
 }

 if (!is_directory(leda_root)) {
   string msg = "xlman: directory " + leda_root + " (LEDAROOT) does not exist.";
   error_handler(1,msg);
   exit(1);
 }

 text_edit_cmd = make_dir(leda_root + delim + "demo" + delim + "xlman" + delim + 
                          "fileview '%f' '%d' '*.%e'");
 leda_dvi_dir = make_dir(leda_root + delim + "Manual" + delim + "DVI");
 leda_html_dir = make_dir(leda_root + delim + "Manual" + delim + "HTML");


 // man path

 char* lmanpath = getenv("LEDAMANPATH");

 if (lmanpath != 0) {
   add_dvi_dirs = string(lmanpath);
   dviadd = true;
 }


 // xdvi cmd

 char* lxdvi    = getenv("LEDAXDVI");
 if (lxdvi) xdvi_cmd = string(lxdvi) + string(" %p");


 user_log();
 write_log("xlman: LEDA Manual Reader");

 string demo_dir = leda_root + delim + "demo";
 string book_dir = leda_root + delim + "book";
 string demo_cout_dir = leda_root + delim + "demo_cout";
 string book_cout_dir = leda_root + delim + "book_cout";

 main_panel.text_item("");

 string label = "                          ";

 M = get_leda_dvi_list(leda_dvi_dir);

 if ( M.empty() ) {
   leda_dtype = "none";
   leda_menu_item = main_panel.string_item(label,leda_dtype);
 }
 else {
   leda_dtype = M.head();
   if (menu_style == MENU_SCROLLBOX) 
     leda_menu_item = main_panel.string_item(label,leda_dtype,M,12);
   else
     leda_menu_item = main_panel.string_item(label,leda_dtype,M);
 }

 //main_panel.set_item_width(leda_menu_item,300);
 main_panel.set_item_width(leda_menu_item,370);

 if (dviadd) {
   Mplus = get_dvi_list(add_dvi_dirs);
   if ( !Mplus.empty() ) {
     add_dtype = Mplus.head();
     if (menu_style == MENU_SCROLLBOX)
       add_menu_item = main_panel.string_item("additional",
	add_dtype,Mplus,12);
     else
       add_menu_item = main_panel.string_item("additional",
	add_dtype,Mplus);
   }
 }


  int i=0;
  int j=0;

  menu demo_menu;
  menu help_menu;
  menu src_menu;

  menu book_demo_menu;
  menu book_src_menu;

  src_file[i] = "";
  help_menu.button("about xlman", i++, about_xlman);
  help_menu.separator();

  src_file[i] = leda_root + delim + "CHANGES";
  help_menu.button("LEDA Changes", i++,show_text);

  src_file[i] = leda_root + delim + "FIXES";
  help_menu.button("LEDA Fixes",i++, show_text);

  
  list<string> prog_list;
  prog_list.append("graphwin");
  prog_list.append("geowin");

  string s;
  forall(s,prog_list)
  { string cmd = demo_dir + delim + s + delim + s;
    if (!is_file(cmd + exe_suffix)) continue;
    src_menu.button(s + ".cpp",i,show_text);
    demo_menu.button(s,j,start_demo);
    src_file[i++] = cmd + ".cpp";
    demo_cmd[j++] = cmd;
   }

  demo_menu.separator();
  src_menu.separator();

  build_menu("graphwin",demo_menu,src_menu,i,j,start_demo);
  build_menu("graph_alg",demo_menu,src_menu,i,j,start_demo);
  build_menu("graph_draw",demo_menu,src_menu,i,j,start_demo);
  build_menu("graph_iso",demo_menu,src_menu,i,j,start_demo);
  build_menu("geowin",demo_menu,src_menu,i,j,start_demo);
  build_menu("d2_geo",demo_menu,src_menu,i,j,start_demo);
  build_menu("d3_geo",demo_menu,src_menu,i,j,start_demo);
  build_menu("anim",demo_menu,src_menu,i,j,start_demo);

  demo_menu.separator();
  src_menu.separator();

  //build_menu("intro",demo_menu,src_menu,i,j,start_ascii);
  build_menu("data_types",demo_menu,src_menu,i,j,start_ascii);
  build_menu("stl",demo_menu,src_menu,i,j,start_ascii);
  build_menu("numbers",demo_menu,src_menu,i,j,start_ascii);
  build_menu("graph",demo_menu,src_menu,i,j,start_ascii);
  build_menu("geo",demo_menu,src_menu,i,j,start_ascii);


//char* news_pr  = main_panel.create_pixrect(news_xpm);

  char* watch_pr = main_panel.create_pixrect(watch_xpm);
  char* print_pr = main_panel.create_pixrect(printer_xpm);
  char* run_pr   = main_panel.create_pixrect(run_xpm);
  char* src_pr   = main_panel.create_pixrect(c_file_xpm);
  char* help_pr  = main_panel.create_pixrect(books_xpm);
  char* setup_pr = main_panel.create_pixrect(pencils_xpm);
  char* grep_pr  = main_panel.create_pixrect(help_xpm);

//char* exit_pr  = main_panel.create_pixrect(door_exit_xpm);

  //main_panel.text_item("");

  main_panel.button(watch_pr, watch_pr, "view page",   2, view);
/*
  main_panel.button(print_pr, print_pr, "print page",  3, print);
*/

  main_panel.button(run_pr,   run_pr,   "run demos",   4, demo_menu);
  main_panel.button(src_pr,   src_pr,   "demo sources",5, src_menu);
  help_but = 
  main_panel.button(help_pr,  help_pr,  "information", 1, help_menu);
  main_panel.button(grep_pr,  grep_pr,  "find class/function", 7, grep);
  main_panel.button(setup_pr, setup_pr, "options",     6, setup);

//main_panel.button(exit_pr,  exit_pr,  "exit",        0);

  main_panel.buttons_per_line(8);

  char* icon = main_panel.create_pixrect(earth_xpm);

  main_panel.set_icon_pixrect(icon);
  set_label(win_label);

  //main_panel.set_item_width(200);
  //main_panel.set_item_space(-1);
  //main_panel.make_menu_bar(1);
  //main_panel.make_menu_bar(-1);

  main_panel.set_window_close_handler(win_del_handler);


  if (show_frame)
     main_panel.display(0,0);
  else
     main_panel.display(main_panel,0,0);

  //main_panel.resize(0,0,330,84);
  //main_panel.resize(0,0,800,190);

  main_panel.resize(0,0,635,160);

#if defined(__win32__)
  clock_win.set_border_width(0);
#endif

  clock_win.set_color(clock_fg_color);
  clock_win.set_bg_color(clock_bg_color);
  clock_win.set_border_color(clock_bg_color);
  clock_win.set_redraw(display_time);
  //clock_win.display(main_panel,20,20);
  clock_win.display(main_panel,5,20);
  clock_win.start_buffering();

  start_clock(clock_display);


  //while (main_panel.read() != 0); 

  for(;;) {
    int but;
    double x,y;
    unsigned long t = 0;
    //int k = main_panel.read_event(but,x,y,t,250);
    window* wp = 0;
    read_event(wp,but,x,y,t,250);
    display_time(&clock_win);
  }




  return 0;
}
