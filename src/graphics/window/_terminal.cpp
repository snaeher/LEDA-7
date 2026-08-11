#include <LEDA/graphics/terminal.h>

#if defined(__linux__) || defined(__APPLE__)

#include <unistd.h>
#include <string.h>
#include <sys/termios.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

using namespace leda;


static terminal* tp = 0;

// static functions

void terminal::win_redraw(window* wp)
{ terminal* tp = (terminal*)wp->get_client_data();

  if (tp->trace) cout << "terminal::redraw" << endl;

/*
  window& W = tp->get_window();
  W.clear(green2);
  W.read_mouse();
*/
  if (tp->resize()) { 
    if (tp->trace) cout << "size changed" << endl;
  }
 }


void terminal::sb_up(int i) {
  tp->line_scroll_up();
}

void terminal::sb_down(int i) {
  tp->line_scroll_down();
}

void terminal::sb_drag(int i)
{
  if (i >= 0) tp->line_scroll_f(0.001*i);
/*
  if (i == -1) 
  { // scrollbar button pressed
    sb_dragging = true;
   }
  if (i == -2)
  { // scrollbar button released
    sb_dragging = false;
   }
*/
}


static string escape(string s)
{ int len = s.length();
  char* buf = new char[2*len];
  int n = 0;
  for(int i=0; i<len; i++)
  { char c = s[i];
    if (c >= 32 && isprint(c)) 
      buf[n++] = c;
    else
    { buf[n++] = '^';
      buf[n++] = '@' + c;
     }
   }
   buf[n] = '\0';

  string result(buf);
  delete[] buf;
  return result;
}


static void ioctl_winsize(int fd, int rows, int cols)
{ winsize ws;
  ws.ws_row = rows;
  ws.ws_col = cols;
  ioctl(fd,TIOCSWINSZ,&ws);
 }


// member functions

double terminal::xcoord(int col) { return win_xleft + (col-1)*col_width; }

double terminal::ycoord(int row) { return win_ytop - row*row_height;}

void terminal::draw_cursor()
{ window& W = *wp;

  //cout << string("draw_cursor(%d,%d)",cursor_row,cursor_col) << endl;

  if (cursor_col == 0 || cursor_row == 0) return;

  double px = W.pix_to_real(1);

  double x1 = xcoord(cursor_col);
  double y1 = ycoord(cursor_row) + px;
  double x2 = x1 + col_width;
  double y2 = y1 + row_height - px;

  W.set_mode(xor_mode);
  W.draw_box(x1,y1,x2,y2,cursor_color);
  W.set_mode(src_mode);

}

void terminal::newline(string txt)
{ if (cursor_row < scrolling_end)
    move_to(cursor_row+1,1);
  else
  { scroll(+1);
    move_to(cursor_row,1);
   }
  draw_text(txt);
}

void terminal::move(int dy, int dx) { 
  move_to(cursor_row+dy, cursor_col+dx);
}

void terminal::move_to(int row, int col)
{ window& W = *wp;

  if (cursor_col == col && cursor_row == row) return;

  if (trace || row > num_rows)
  {
    cout << string("move_to(%d,%d) --> (%d,%d)",
                                  cursor_row,cursor_col,row,col) << endl;
  }

/*
  assert(row >= 1);
  assert(row <= num_rows);
*/
  if (cursor_visible)  draw_cursor();
  cursor_col = col;
  cursor_row = row;
  if (cursor_visible)  draw_cursor();
}


void terminal::clear_line()
{ window& W = *wp;

  if (trace) {
    cout << string("clear_line(%d,%d)",cursor_row,cursor_col) << endl;
  }

  double x1 = xcoord(cursor_col);
  double y1 = ycoord(cursor_row);
  double x2 = win_xright;
  double y2 = y1 + row_height;

  W.draw_box(x1,y1,x2,y2,white);
  if (cursor_visible)  draw_cursor();
}


void terminal::shift_line(int x_start, int dx)
{ window& W = *wp;

  if (trace) {
    cout << string("shift_line(%d,%d) dx = %d",
                          cursor_row,cursor_col,dx) << endl;
  }

  double x1 = xcoord(x_start);
  double y1 = ycoord(cursor_row);
  double x2 = win_xright;
  double y2 = y1 + row_height;

  bool c_visible = hide_cursor();

  char* pm = W.get_pixrect(x1,y1,x2,y2);

  W.start_buffering();
  W.put_pixrect(x1+dx*col_width,y1,pm);
  W.flush_buffer(x1,y1,x2,y2);
  W.stop_buffering();

  W.del_pixrect(pm);

  if (c_visible) show_cursor();
}


void terminal::clear_display()
{ window& W = *wp;

  if (trace) cout << "clear_display" << endl;

  scrolling_start = 1;
  scrolling_end = num_rows;
  cursor_row = 1;
  cursor_col = 1;

/*
  W.clear(0xeeeeee);
  W.draw_box(win_xleft,win_ybot,win_xright,win_ytop,white);
*/
  W.clear();
//W.draw_rectangle(win_xleft,win_ybot,win_xright,win_ytop,0xcccccc);

  if (cursor_visible) draw_cursor();
}


void terminal::scroll(int dy)
{ window& W = *wp;

  if (trace) {
    cout << string("scrolling(%d,%d,%+d)",
                          scrolling_start,scrolling_end,dy) << endl;
  }

  double scroll_ymin = ycoord(scrolling_end);
  double scroll_ymax = ycoord(scrolling_start-1);

  double x0 = win_xleft;
  double x1 = win_xright;
  double y0 = scroll_ymin;
  double y1 = scroll_ymax - dy*row_height;

  if (dy < 0) {
    y0 = scroll_ymin - dy*row_height;
    y1 = scroll_ymax;
   }

  if (y0 > y1) return;

  bool c_visible = hide_cursor();

  char* pm = W.get_pixrect(x0,y0,x1,y1);

  W.start_buffering();
  W.draw_box(x0,scroll_ymin,x1,scroll_ymax,white);
  W.put_pixrect(x0,y0+dy*row_height,pm);
  W.flush_buffer(win_xleft,scroll_ymin,win_xright,scroll_ymax);
  W.stop_buffering();

  W.del_pixrect(pm);

  if (c_visible) show_cursor();
}


void terminal::text_attribute(int x)
{
  if (trace)  {
    cout << string("text_attribute(%d)",x) << endl;
  }

    switch (x) {

     case  0: // reset attributes
              fg_color = black;
              bg_color = white;
              break;

     case  1: // bold
              break;

     case  4: // underline start
              break;

     case 24: // underline end
              break;

     case  5: // blinking on
              break;

     case 25: // blinking off
              break;

     case  7: // standout start
              break;

     case 27: // standout end
              break;

     case 23: // normal
              break;

     case 29: // rmxx ?
              break;

     default: { string cmd = string("\e[%dm",x);
                cout << "text attribute: " << escape(cmd) << endl;
                break;
               }
   }
}


void terminal::text_color(int c, int fg)
{
  if (trace)  {
    cout << string("text_color: clr = %d fg = %d",c,fg) << endl;
  }

  int clr = fg ? black : white;

  switch (c) 
  {
     // dark
     case 0: clr = black;  break;
     case 1: clr = red;    break;
     case 2: clr = green;  break;

   //case 3: clr = yellow; break;
     case 3: clr = blue; break;

     case 4: clr = blue;   break;
     case 5: clr = pink;   break;
     case 6: clr = cyan;   break;
     case 7: clr = white;  break;

     // light
     case  8: clr = black;  break;
     case  9: clr = red;    break;
     case 10: clr = green;  break;

   //case 11: clr = yellow; break;
     case 11: clr = blue2; break;

     case 12: clr = blue;   break;
     case 13: clr = pink;   break;
     case 14: clr = cyan;   break;
     case 15: clr = white;  break;
  }

  if (fg == 0) 
    bg_color = clr;
  else
    fg_color = clr;
}



void terminal::draw_text(string s)
{ window& W = *wp;

  if (s == "") return;

  if (trace) 
    cout << string("text(%d,%d) bg = %06x  fg = %06x : |%s|",
             cursor_row,cursor_col,bg_color,fg_color,~s) << endl;

  if (cursor_col > num_cols)
  { int row = cursor_row+1;
    if (row > scrolling_end) 
    { scroll(+1);
      row = scrolling_end;
     }
    move_to(row,1);
   }

  string remaining;

  int slen = s.length();
  int maxlen = num_cols - cursor_col + 1;

  if (slen > maxlen) 
  { remaining = s.substring(maxlen);
    s = s.substring(0,maxlen);
    slen = maxlen;
  }

  last_char = s[slen-1];

  double x1 = xcoord(cursor_col);
  double y1 = ycoord(cursor_row) + row_height;

  double x2 = x1+slen*col_width;
  double y2 = y1-row_height;

  bool c_visible = hide_cursor();
  W.start_buffering();
  W.draw_box(x1,y1,x2,y2,bg_color);
  W.draw_text(x1,y1,s,fg_color);
  W.flush_buffer(x1,y1,x2,y2);
  W.stop_buffering();
  if (c_visible) show_cursor();

  move(0,slen);

  draw_text(remaining);
}


void terminal::trace_cmd(string ctrl, string cmd, string text)
{ string c = ctrl + " (" + cmd + ")"; 
  cout <<  string("%-25s : %s", ~escape(c).head(25), ~escape(text)) << endl;
 }

bool terminal::tcap(string& word, string ctrl, string tc, string cmd)
{ if (word.starts_with(ctrl))
  { word = word.substring(ctrl.length());
    if (trace) trace_cmd(ctrl,cmd,word);
    if (cmd[0] == '@') {
      // not handled
      cout <<  string("%-10s: %s", ~escape(ctrl), ~cmd) << endl;
    }
    return true;
  }
  return false;
}

void terminal::parse_text(string s)
{ window& W = *wp;

  // handle control sequences and print text

  if (s.length() == 0) return;

  // prepend not completed part of the previous text
  s = uncomplete + s;
  uncomplete = "";

  //cout << "parse_text: s =  " << escape(s) << endl;

  int p = 0;

  string word;

  while (p < s.length())
  { 
    int q = p+1;
    while(q < s.length() && isprint(s[q])) q++;
    word = s.substring(p,q);
    p = q;

    if (word == "") continue;

    if (trace) {
      cout << "word = |" << escape(word) << "|" << endl;
    }

    if (isprint(word[0]))
    { // just normal text: print it
      draw_text(word);
      line_buffer += word;
      continue;
     }

    // check control characters

    string cmd = "";

    if (tcap(word,string(LF),"LF","LF")) {

       if (!alternate_buffer && line_scroll_pos == -1)
       { lines.append(line_buffer);
         update_scrollbar();
        }
 
      if (cursor_row < scrolling_end)
        move(+1,0);
      else
        scroll(+1);
 
      draw_text(word);
      line_buffer = word;
      continue;
    }
  
    if (tcap(word,string(CR),"CR","CR")) {
      move_to(cursor_row,1);
      draw_text(word);
      continue;
    }

    if (tcap(word,string(TAB),"TAB","TAB")) {

      int col = 8*(1 + (cursor_col-1)/8)+1;

      string space = string("        ").head(col-cursor_col);
/*
      move_to(cursor_row,col);
      draw_text(word);
*/     
      draw_text(space + word);
      
      line_buffer += (space + word);
      continue;
    }

    if (tcap(word,string(BS),"BS","BS")) {
      if (cursor_col > 1) {
        move(0,-1);
        line_buffer = line_buffer.head(-1);
      }
      draw_text(word);
      continue;
    }

    if (tcap(word,string(BELL),"BELL","BELL")) {
      // ^G = \a used as string delimiter (label commands)
      continue;
    }

    // check escape sequences

    // vim special codes

    if (tcap(word,"\e[?2004h","t_BE","vim_special_BE")) continue;
    if (tcap(word,"\e[?2004l","t_BD","vim_special_BD")) continue;
    if (tcap(word,"\e[>c",    "t_RV","vim_special_RV")) continue;
    if (tcap(word,"\e]10;?",  "t_RF","vim_special_RF")) continue;
    if (tcap(word,"\e]11;?",  "t_RB","vim_special_RB")) continue;
    if (tcap(word,"\e[>4;2m", "t_TI","vim_special_TI")) continue;
    if (tcap(word,"\e[>4;m",  "t_TE","vim_special_TE")) continue;
    if (tcap(word,"\e[22;1t", "t_SI","vim_special_SI")) continue;
    if (tcap(word,"\e[22;2t", "t_ST","vim_special_ST")) continue;
    if (tcap(word,"\e[23;1t", "t_RI","vim_special_RI")) continue;
    if (tcap(word,"\e[23;2t", "t_RT","vim_special_RT")) continue;


    // unhandled codes

    if (tcap(word,"\e(0",      "as", "@alt_char_start")) continue;
    if (tcap(word,"\e(B",      "ae", "@alt_char_end")) continue;

    if (tcap(word,"\e[6n",     "dsr","@report_cursor_pos")) continue;


    if (tcap(word,"\e=",       "xx", "@alt_mode_start")) continue;
    if (tcap(word,"\e>",       "xx", "@alt_mode_end")) continue;
    if (tcap(word,"\e[22;0;0t","xx", "@cursor_motion_start")) continue;
    if (tcap(word,"\e[23;0;0t","xx", "@cursor_motion_end")) continue;
    if (tcap(word,"\e[?7h",    "xx", "@auto_wrap_start")) continue;
    if (tcap(word,"\e[?7l",    "xx", "@auto_wrap_end")) continue;
    if (tcap(word,"\e[?1h",    "xx", "@app_cursor_key_start")) continue;
    if (tcap(word,"\e[?1l",    "xx", "@app_cursor_key_end")) continue;
    if (tcap(word,"\e[?1004h", "xx", "@focus_report_start")) continue;
    if (tcap(word,"\e[?1004l", "xx", "@focus_report_end")) continue;
    if (tcap(word,"\e[22;0;0t","ti", "@termcap_start")) continue;
    if (tcap(word,"\e[23;0;0t","te", "@termcap_end")) continue;

    if (tcap(word,"\ePzz",     "xx", "@device_control")) continue;



    // handled codes

    // visible bell: 
    // \e[?5h reverse video
    // \e[?5l normal video
    
    if (tcap(word,"\e[?5l","vb1","reverse_video_end")) {
      // visible bell 1
      continue;
    }

    if (tcap(word,"\e[?5h","vb2","reverse_video_start")) {
      // visible bell 2
      bool c_visible = cursor_visible;
      hide_cursor();
      W.set_mode(xor_mode);
      W.draw_box(win_xleft,win_ybot,win_xright,win_ytop,grey2);
      leda_sleep(0.1);
      W.draw_box(win_xleft,win_ybot,win_xright,win_ytop,grey2);
      W.set_mode(src_mode);
      if (c_visible) show_cursor();
      continue;
    }

    if (tcap(word,"\e[4h","im", "insert_start")) 
    { shift_line(cursor_col,word.length());
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[4l","ei", "insert_end")) 
    {
      continue;
    }

    if (tcap(word,"\e7","sc","save_cursor_pos"))
    { cursor_row_save = cursor_row;
      cursor_col_save = cursor_col;
      continue;
     }

    if (tcap(word,"\e8","fs","restore_cursor_pos"))
    { move_to(cursor_row_save,cursor_col_save);
      continue;
     }

    if (tcap(word,"\eM","xx","scroll_text_down")) // scroll down
    { scroll(-1);
      continue;
     }

    if (tcap(word,"\e]2;","xx","set_window_label")) { 
      //\e]2label\a 
      //W.set_frame_label(label + "  " + word);
      W.set_frame_label(word);
      continue;
    }

    if (tcap(word,"\e]1;","xx","set_icon_label")) {
      //\e]1label\a 
      W.set_icon_label(word);
      continue;
    }

    if (tcap(word,"\e[?1049h","smcup","alternate_buffer_start")) {
      alternate_buffer = true;
      int xmargin = (W.width()- num_cols * font_w)/2;
      win_xleft  = W.xmin() + W.pix_to_real(xmargin);
      win_xright = win_xleft + num_cols * col_width;
      clear_display();
      lines.clear();
      line_buffer = "";
      W.close_scrollbar();
      continue;
    }

    if (tcap(word,"\e[?1049l","rmcup","alternate_buffer_end")) {
      alternate_buffer = false;
      int xmargin = (W.width() - sb_width - num_cols * font_w)/2;
      win_xleft  = W.xmin() + W.pix_to_real(xmargin);
      win_xright = win_xleft + num_cols * col_width;
      clear_display();
      lines.clear();
      line_buffer = "";
      W.open_scrollbar(sb_width,sb_up,sb_down,sb_drag,0,1);
      continue;
    }

    if (tcap(word,"\e[?25h","ve","cursor_normal")) { // ve
      show_cursor();
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[?25h","vs","cursor_visible")) { // vs
      show_cursor();
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[?25l","vi","cursor_invisible")) { //vi
      hide_cursor();
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[?12h","vs","very_visible1")) { // vs
      // cursor very visible ?
      bool c_visible = cursor_visible;
      hide_cursor();
      cursor_color = clr_cursor2;
      if (c_visible) show_cursor();
      continue;
    }

    if (tcap(word,"\e[?12l","xx","very_visible0")) { // ??
      bool c_visible = cursor_visible;
      hide_cursor();
      cursor_color = blue;
      cursor_color = clr_cursor1;
      if (c_visible) show_cursor();
      continue;
    }

    if (tcap(word,"\e[J","cd","clear_display1")) { //cd
      clear_display();
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[2J","cd","clear_display2")) {
      clear_display();
      draw_text(word);
      lines.clear();
      line_buffer = "";
      continue;
    }

    if (tcap(word,"\e[3J","cd","clear_display3")) {
/*
      clear_display();
      draw_text(word);
*/
      continue;
    }

    if (tcap(word,"\e[K","ce","clear_line")) {
      clear_line();
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[H","ho","move_home")) {
      move_to(1,1);
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[r","cs","scroll_reset")) {
      scrolling_start = 1;
      scrolling_end = num_rows;
      draw_text(word);
      continue;
    }

    if (tcap(word,"\e[0%m","xx","reset_attrib")) {
      text_attribute(0);
      draw_text(word);
      continue;
    }


    // \e[38;5;%dm : foreground color
    // \e[48;5;%dm : background color

    if (word.starts_with("\e[38;5;") || word.starts_with("\e[48;5;"))
    { 
      string cmd = "";

      int i = -1;
      int jmax = std::min(10,word.length()-1); 

      for(int j = 8; j<=jmax; j++)
      { char c = word[j];
        if (c == 'm') 
        { i = j; 
          if (word[2] == '3') cmd = "fg_color";
          if (word[2] == '4') cmd = "bg_color";
          break;
        }
        if (!isdigit(c)) break;
      }

      if (i != -1)
      { int val = word(7,i-1).atoi();
        string ctrl = word(0,i);
        word = word.substring(i+1);
        if (trace) trace_cmd(ctrl,cmd,word);
        if (cmd == "bg_color") text_color(val,0);
        if (cmd == "fg_color") text_color(val,1);
        draw_text(word);
        continue;
       }
    }


    if (word.starts_with("\e[")) // \e[%d{@,P,A,B,C,D,G,L,M,d,b,m} 1 Parameter
    { 
      string cmd = "";

      int i = -1;
      int jmax = std::min(4,word.length()-1); 

      for(int j = 2; j<=jmax; j++)
      { char c = word[j];
        if (c == '@') { i = j; cmd = "insert_chars";  break; } // IC
        if (c == 'P') { i = j; cmd = "delete_chars";  break; } // DC
        if (c == 'A') { i = j; cmd = "move_up";       break; } // UP
        if (c == 'B') { i = j; cmd = "move_down";     break; } // DO 
        if (c == 'C') { i = j; cmd = "move_right";    break; } // RI
        if (c == 'D') { i = j; cmd = "move_left";     break; } // LE 
        if (c == 'G') { i = j; cmd = "cursor_column"; break; } // ch 
        if (c == 'L') { i = j; cmd = "add_lines";     break; } // AL
        if (c == 'M') { i = j; cmd = "del_lines";     break; } // DL
        if (c == 'd') { i = j; cmd = "cursor_row";    break; } // cv 
        if (c == 'b') { i = j; cmd = "repeat_char";   break; } // REP 
        if (c == 'm') { i = j; cmd = "text_attrib";   break; }
        if (!isdigit(c)) break;
      }

      if (i != -1)
      { 
        int val = word(2,i-1).atoi();

        if (i == 2) 
        { // empty value field
          val = 1;
          if (cmd == "text_attrib") val = 0;
         }

        string ctrl = word(0,i);
        word = word.substring(i+1);


        if (cmd == "insert_chars") {
          shift_line(cursor_col,val);
          draw_text(word);
        }

        if (cmd == "delete_chars") {
          shift_line(cursor_col+val,-val);
          draw_text(word);
        }

        if (cmd == "repeat_char") {
          char buf[256];
          memset(buf,last_char,val);
          buf[val] = '\0';
          draw_text(string(buf)+word);
        }

        if (cmd == "cursor_row") {
          move_to(val,cursor_col);
          draw_text(word);
        }

        if (cmd == "cursor_column") {
          move_to(cursor_row,val);
          draw_text(word);
        }

        if (cmd == "move_up") {
          move(-val,0);
          draw_text(word);
         }

        if (cmd == "move_down") {
          move(+val,0);
          draw_text(word);
         }

        if (cmd == "move_left") {
          move(0,-val);
          draw_text(word);
         }

        if (cmd == "move_right") {
          move(0,+val);
          draw_text(word);
         }

        if (cmd == "add_lines") scroll(-val);

        if (cmd == "del_lines") scroll(+val);

        if (cmd == "text_attrib") { 
          text_attribute(val);
          draw_text(word);
         }

       if (trace) trace_cmd(ctrl,cmd,word);

       continue;
      }

    }


    if (word.starts_with("\e[")) // \e[yy;xxx{H,r}  (2 Parameters)
    { 
      string cmd = "";

      int i = -1;
      int j = -1;

      if (word[3] == ';') i = 3;
      if (word[4] == ';') i = 4;

      if (i != -1)
      { int kmax = std::min(i+4,word.length()-1); 
        for(int k = i+2; k<=kmax; k++)
        { char c = word[k];
          if (c == 'H') { j = k; cmd = "move_to";   break; } // cm
          if (c == 'r') { j = k; cmd = "scroll_region"; break; } // cs
          if (!isdigit(c)) break;
        }
      }

      if (j != -1)
      { 
        int val1 = word(2,i-1).atoi();
        int val2 = word(i+1,j-1).atoi();

        string ctrl = word(0,j);
        word = word.substring(j+1);

        if (cmd == "move_to")
        { move_to(val1,val2);
          draw_text(word);
         }
        else
        if (cmd == "scroll_region")
        { assert(val2 >= val1);
          scrolling_start = val1;
          scrolling_end = val2;
        }

        if (trace) trace_cmd(ctrl,cmd,word);

        continue;
       }
     }


    if (q < s.length()-1)
    { // report unrecognized esc sequence (if not at the end of s)
      //draw_text(escape(word));
      string w = word.replace("%","%%");
      cout <<  string("%-10s: not recognized", ~escape(w)) << endl;
     }

  }

  if (word != "" && iscntrl(word[0])) {
    uncomplete = word; // remaining (prefix of a) ctrl-sequence
  }
}


// public

terminal::~terminal() { delete wp; }

terminal::terminal(string s, int rows, int cols, int font_sz)
{ 
  tp = this;

  label = s;

  line_buffer = "";
  line_scroll_pos = -1;

  fd_shell = -1;

  trace = false;

  cursor_col = 0;
  cursor_row = 0;
  cursor_col_save = 0;
  cursor_row_save = 0;
  cursor_visible = true;
  scrolling_start = 1;
  scrolling_end = 30;
  fg_color = black;
  bg_color = white;
  cursor_color = clr_cursor1;

  alternate_buffer = false;

  last_char = 0;

  win_xleft = 0;
  win_xright = 0;
  win_ybot = 0;
  win_ytop = 0;

  num_cols = 0;
  num_rows = 0;

  font_h = 0;
  font_w = 0;
  sb_width  = 0;
 
  resize_handler = 0;


  // init window

  string fname("F%d",font_sz);

  int fw,fh;
  window::font_size(fname,"H",fw,fh);

  sb_width = int(1.25*fw);

  int width  = int(cols*fw + sb_width + 0.4*fh);
  int height = int(rows*fh + 0.4*fh);


  wp = new window(width,height,label);

  window& W = *wp;

  //W.set_clear_on_resize(false);

  W.set_client_data(this);
  W.set_font(fname);

  fh = W.real_to_pix(W.text_height("H"));
  fw = W.real_to_pix(W.text_width("H"));

  //cout << "font " << fname << ": " << fh << " x " << fw << endl;

  wp->set_redraw(win_redraw);
  wp->set_show_notice(false);
}


bool terminal::resize() 
{ window& W = *wp;

  double th = W.text_height("H");
  font_h = W.real_to_pix(th);
  row_height = W.pix_to_real(font_h);

  double tw = W.text_width("H");
  font_w = W.real_to_pix(tw);
  col_width = W.pix_to_real(font_w);

  int cols = (W.width()-sb_width)/font_w;
  int xmargin = (W.width() - cols*font_w)/3;

  int rows = W.height()/font_h;
  int ymargin = (W.height() - rows*font_h)/2;
/*
  if (rows == num_rows && cols == num_cols) return false;
*/

  if (rows != num_rows || cols != num_cols) 
  { if (trace) {
      cout << string("resize: size = %d x %d  font = %d x %d", 
                                  rows, cols, font_w,font_h) << endl;
    }
    ioctl_winsize(fd_shell,rows,cols);
    if (resize_handler) resize_handler(this,rows,cols);
  }

  num_rows = rows;
  num_cols = cols;

  win_xleft  = W.xmin() + W.pix_to_real(xmargin);
  win_ytop   = W.ymax() - W.pix_to_real(ymargin);

  win_xright = win_xleft + cols*col_width;
  win_ybot   = win_ytop  - rows*row_height;


  if (lines.empty())
    clear_display();
  else
  { if (cursor_row > num_rows) move_to(num_rows,cursor_col);
    line_scroll_start();
    line_scroll_to(lines.length() - cursor_row);
    line_scroll_stop(false);
  //W.toast(string("RESIZE: %d x %d",num_rows,num_cols),1.5);
    W.set_frame_label(label + string("  %d x %d",num_rows,num_cols));
  }

  return true;
}


void terminal::display(int x, int y)
{ window& W = *wp;
  W.display(x,y);
  W.open_scrollbar(sb_width,sb_up,sb_down,sb_drag,0,1);
  resize();
}

int terminal::send(string s) {
  if (fd_shell != -1) return write(fd_shell,~s,s.length());
  return 0;
}


bool terminal::show_cursor()
{ if (trace) {
    cout << string("show cursor(%d,%d)",cursor_row,cursor_col) << endl;
  }
  bool visible = cursor_visible;
  if (!visible) draw_cursor();
  cursor_visible = true;
  return visible;
}

bool terminal::hide_cursor()
{ if (trace) {
    cout << string("hide cursor(%d,%d)",cursor_row,cursor_col) << endl;
  }
  bool visible = cursor_visible;
  if (visible) draw_cursor();
  cursor_visible = false;
  return visible;
}


void terminal::prompt(string msg)
{ window& W = *wp;

  newline(msg);
  lines.append("");
  lines.append(msg);
  line_buffer = "";

  int val;
  double x,y;
  for(;;)
  {
    int e = W.read_event(val,x,y);

    if (e == button_press_event)
    { if (val == MOUSE_BUTTON(1)) line_scroll_stop(true);
      if (val == MOUSE_BUTTON(4)) line_scroll_up();
      if (val == MOUSE_BUTTON(5)) line_scroll_down();
      continue;
     }

    if (e != key_press_event) continue;

    if (val == KEY_HOME) { line_scroll_home(); continue; }
    if (val == KEY_END)  { line_scroll_end(); continue; }
    if (val == KEY_UP)   { line_scroll_up();  continue; }
    if (val == KEY_DOWN) { line_scroll_down();continue; }

    // any other key

    if (line_scroll_pos != -1) 
      line_scroll_stop(true);
    else
      break;
  }
 }



void terminal::connect(int fd)
{ window& W = *wp;

//const int buf_sz = 1024;
  const int buf_sz = 512;

  fd_shell = fd;

  ioctl_winsize(fd_shell,num_rows,num_cols);

  char buf[buf_sz];
  int fd_disp = W.display_fd();

  fd_set rdset;
  FD_ZERO(&rdset);
  int fd_max = std::max(fd_disp,fd_shell);

  for(;;) 
  { 
//cout << "SELECT" << endl;

    FD_SET(fd_disp,&rdset);
    FD_SET(fd_shell,&rdset);
    select(fd_max+1,&rdset,0,0,0);

    if (FD_ISSET(fd_disp,&rdset))
    { 
//cout << "READ(fd_disp)" << endl;

      window* wp = 0;
      int val = 0;
      double x,y;
      unsigned long t;

/*
      int e = W.read_event(val,x,y,t);
*/
      int e = read_event(wp,val,x,y);
      if (wp != &W || e == no_event) continue;


      if (e == button_press_event)
      {
        if (val== MOUSE_BUTTON(1)) W.keyboard(-1); // toggle
        if (val == CLOSE_BUTTON) break;

        if (val == MOUSE_BUTTON(1)) line_scroll_stop(true);
        if (val == MOUSE_BUTTON(4)) line_scroll_up();
        if (val == MOUSE_BUTTON(5)) line_scroll_down();

        continue;
       }


      if (e == key_press_event)
      { 
        if (!alternate_buffer)
        { // line buffer scrolling

          if (val == KEY_HOME) { line_scroll_home(); continue; }
          if (val == KEY_END)  { line_scroll_end(); continue; }

          if (line_scroll_pos != -1) 
          { // line scrolling mode
            if (val == KEY_UP)  { line_scroll_up();  continue; }
            if (val == KEY_DOWN){ line_scroll_down();continue; }

            // any other key terminates scrolling mode
            line_scroll_stop(true);
            //continue;
          }
        }
   
        if (val == KEY_UP)    { send("\e[A"); continue; }
        if (val == KEY_DOWN)  { send("\e[B"); continue; }
        if (val == KEY_RIGHT) { send("\e[C"); continue; }
        if (val == KEY_LEFT)  { send("\e[D"); continue; }

        char c = char(val);

        if (val == KEY_RETURN)    c = CR;
        if (val == KEY_ESCAPE)    c = ESC;
        if (val == KEY_BACKSPACE) c = alternate_buffer ? BS : DEL; // ???
        if (val == KEY_TAB)       c = TAB;

        int x = 0;
        if (c > 0) x = write(fd_shell,&c,1);
      }

      continue;
    }


    if (FD_ISSET(fd_shell,&rdset)) 
    { 
//cout << "READ(fd_shell)" << endl;

      int n = read(fd_shell,buf,buf_sz-1); 

      if (n <= 0)
      { cout << endl;
        cout << "terminal: child terminated or killed." << endl;
        break;
       }

      // filter out negative chars
      int j = 0;
      for(int i=0; i<n; i++) {
        char c = buf[i];
        if (c >= 0) buf[j++] = c; 

      }
      n = j;
      buf[n] = '\0';

      string s = string(buf);

      parse_text(s);

      if (!alternate_buffer && lines.length() < 10)
      { 
        if (s.index("ssh: Could not resolve") != -1 ||
            s.index("Permission denied, please try again") != -1) 
        { hide_cursor();
          newline("Login failed.");
          leda_sleep(3.5);
          break;
        }

      }

      continue;
    }
  }

}


// line buffer (scrolling)

void terminal::line_scroll_start()
{ if (line_scroll_pos != -1) return;

  //fg_color = 0x0000aa;
  fg_color = 0x000077;

  cursor_row_save = cursor_row;
  cursor_col_save = cursor_col;
  hide_cursor();

  if (line_buffer != "") {
    lines.append(line_buffer);
    line_buffer = "";
  }
}


void terminal::line_scroll_stop(bool goto_end)
{  window& W = get_window();

  if (line_scroll_pos == -1) return;

  if (goto_end) line_scroll_end();

  fg_color = black;
  line_scroll_draw(line_scroll_pos);

  line_scroll_pos = -1;
  cursor_row = cursor_row_save;
  cursor_col = cursor_col_save;
  show_cursor();

  update_scrollbar();
}


void terminal::line_scroll_f(double f)
{ int num_lines = lines.length();
  int p = int(f*(num_lines-num_rows));
  line_scroll_to(p);
}


void terminal::line_scroll_draw(int pos)
{ window& W = *wp;

  W.start_buffering();
  clear_display();
  move_to(0,1);

  list_item it = lines.get_item(pos);
  for(int i=0; i<num_rows && it != 0; i++)
  { newline(lines[it]);
    it = lines.succ(it);
   }

  W.stop_buffering();
  W.flush_buffer();
}


void terminal::line_scroll_to(int start_pos)
{ 
  line_scroll_start(); // can change lines.length()

  int num_lines = lines.length();

  if (start_pos < 0) start_pos = 0;
  if (start_pos > num_lines-1) start_pos = num_lines-1;

  if (trace) {
   cout << string("line_scroll_to: %d --> %d", line_scroll_pos,start_pos);
   cout << endl;
  }

  // redraw (only if position changed)

  if (start_pos != line_scroll_pos)
  { line_scroll_pos = start_pos;
    line_scroll_draw(start_pos);
   }
}


void terminal::line_scroll_home()
{ line_scroll_start();
  line_scroll_to(0); 
  update_scrollbar();
}

void terminal::line_scroll_end()
{ line_scroll_start();
  line_scroll_to(lines.length()-num_rows); 
  update_scrollbar();
}

void terminal::line_scroll_up()
{ window& W = *wp;
  // start scrolling at bottom (if not active)
  if (line_scroll_pos == -1) line_scroll_end();
  line_scroll_to(line_scroll_pos-1); 
  update_scrollbar();
}


void terminal::line_scroll_down()
{ window& W = *wp;
  // do not start scrolling
  if (line_scroll_pos == -1) return;
  line_scroll_to(line_scroll_pos+1);
  update_scrollbar();

}


void terminal::update_scrollbar()
{ window& W = get_window();
  if (!W.has_scrollbar()) return;
  int num_lines = lines.length();
  double sb_size = double(num_rows)/num_lines;
  if (sb_size > 1) sb_size = 1;
  double sb_pos = 1.0;
  if (line_scroll_pos != -1)
    sb_pos = double(line_scroll_pos)/(lines.length()-num_rows);
  W.set_scrollbar_pos(sb_pos,sb_size);
}


#endif
