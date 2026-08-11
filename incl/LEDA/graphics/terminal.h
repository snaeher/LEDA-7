/*******************************************************************************
+
+  LEDA 7.2.3  
+
+
+  terminal.h
+
+
+  Copyright (c) 1995-2026
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#ifndef LEDA_TERMINAL_H
#define LEDA_TERMINAL_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 723203
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graphics/window.h>

LEDA_BEGIN_NAMESPACE


class terminal 
{
  static const char CC  =  3; // ^C
  static const char BELL=  7; // ^G
  static const char BS  =  8; // ^H
  static const char TAB =  9; // ^I
  static const char LF  = 10; // ^J
  static const char CR  = 13; // ^M
  static const char ESC = 27; // ^[
  static const char DEL = 127;

  static const int clr_cursor1 = 0x222222;
  static const int clr_cursor2 = 0x800000;

  static void win_redraw(window*);
  static void sb_drag(int);
  static void sb_down(int);
  static void sb_up(int);


  window* wp;

  int fd_shell;

  bool trace;

  list<string> lines;
  string line_buffer;
  int line_scroll_pos;

  string uncomplete; // uncomplete control sequence

  string label;

  bool alternate_buffer; // e.g. vim

  int  cursor_col;
  int  cursor_row;

  int  cursor_col_save;
  int  cursor_row_save;
  
  int scrolling_start;
  int scrolling_end;

  bool cursor_visible;

  int fg_color;
  int bg_color;
  int cursor_color;
  
  char last_char;
  
  double win_xleft;
  double win_xright;
  double win_ybot;
  double win_ytop;
  double row_height;
  double col_width;

  int font_h;
  int font_w;

  int sb_width;

  int num_cols;
  int num_rows;

  void (*resize_handler)(terminal* tp, int rows, int cols);

  double xcoord(int col);
  double ycoord(int row);

  void draw_cursor();

  void move_to(int row, int col);
  void move(int dy, int dx);

  void newline(string txt);

  void clear_line();
  void shift_line(int x_start, int dx);
  void clear_display();
  void scroll(int dy);
  void text_attribute(int x);
  void text_color(int c, int fg);
  void draw_text(string s);
  void trace_cmd(string s, string cmd, string text);
  bool tcap(string& word, string pat, string esc,string cmd);
  
  void parse_text(string s);

  void update_scrollbar();

  bool resize(); 
  int  send(string s);

  void line_scroll_start();
  void line_scroll_stop(bool goto_end);

  void line_scroll_draw(int pos);
  void line_scroll_to(int pos);
  void line_scroll_f(double f);

  void line_scroll_home();
  void line_scroll_end();
  void line_scroll_up();
  void line_scroll_down();

public:
  
  terminal(string label, int rows, int cols, int font_sz);
 ~terminal();

  void display(int x = window::center, int y = window::center);
  void connect(int fd);
  void prompt(string msg);
  bool show_cursor();
  bool hide_cursor();

  window& get_window() { return *wp; }

  int get_cols() { return num_cols; }
  int get_rows() { return num_rows; }

  int get_font_width()  { return font_w ; }
  int get_font_height() { return font_h ; }

  void set_resize(void (*f)(terminal*,int,int)) { resize_handler = f; }
  void set_trace(bool b) { trace = b; }

};
  

LEDA_END_NAMESPACE

#endif
