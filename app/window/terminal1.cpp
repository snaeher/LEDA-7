#include <LEDA/graphics/window.h>

#include <LEDA/system/file.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

using namespace leda;


#if defined(__win32__)

int main() { 
  cerr << endl;
  cerr << "This program cannot be used on MS-Windows." << endl;
  cerr << endl;
  return 0; 
}

#else

#include <unistd.h>
#include <signal.h>
#include <string.h>

#define BS char(8)
#define LF char(10)
#define CR char(13)

using namespace leda;

static string title = "LEDA Terminal";

static window* wp = 0;
static string text;
static string cursor;

static int xmargin = 5;
static int ymargin = 10;

static double line_height = 0;
static double text_height = 0;
static double scroll_top = 0;

static double win_ymin = 0;
static double win_ymax = 0;

static int cols = 80;
static int lines = 32;
static int font_sz = 40;
static float height_f = 0.9;

static bool closed = false;

static int child_pid = 0;

static string user;
static string pwd;

static bool tty_raw = false;

void exit_handler() 
{
  cout << "TERMINAL EXIT: terminate process and sub-processes" << endl;
  cout << "child_pid = " << child_pid << endl;

  if (child_pid == 0) return; 

/*
  // kill group
  int pgid = getpgid(child_pid);
  cout << "child_pid = " << child_pid << endl;
  cout << "group_pid = " << pgid << endl;
  killpg(0,SIGTERM); // kill child process group 
  killpg(pgid),SIGTERM);
*/

  // kill sub-processes of process with pid = child_pid
  string cmd = string("/bin/pkill -P %d",child_pid);
  cout << cmd << endl;
  system(cmd);

  // kill process with pid = child_pid 
  cmd = string("/bin/kill %d",child_pid);
  cout << cmd << endl;
  system(cmd);
}


/*
void start_child_process(string prog, int fd_out, int fd_in)
{
  // start program in backround and redirect its std output to fd_out 
  // and its std input from fd_in (using bash command line syntax)

  string f_pid("%d.child",getpid());

  char cmd[256];
  sprintf(cmd,"%s >&%d 2>&1 <&%d & echo $! > %s", ~prog, fd_out,fd_in,~f_pid);

  cout << cmd << endl;

  system(cmd);

  child_pid = 0;

  if (is_file(f_pid))
  { ifstream ifstr(f_pid);
    ifstr >> child_pid;
    ifstr.close();
    delete_file(f_pid);
  }

  cout << "child_pid = " << child_pid << endl;;
}
*/


void sig_handler(int s) {
  cout << string("TERMINAL: signal(%d): %s ---> EXIT",s,strsignal(s)) << endl;
  if (s == SIGCHLD)
    child_pid = 0;
  else
   exit(0);
}


string escape(string s)
{ int len = s.length();
  char* buf = new char[2*len];
  int n = 0;
  for(int i=0; i<len; i++)
  { char c = s[i];
    if (isprint(c)) 
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


static int  tcap_col = 0;
static int  tcap_row = 0;

static string tcap_last_char;

static int tcap_scroll_start = 1;
static int tcap_scroll_end = 32;
static int tcap_text_color = black;

static bool tcap_cursor_visible = false;


void tcap_draw_cursor()
{ window& W = *wp;

  //cout << string("draw_cursor(%d,%d)",tcap_row,tcap_col) << endl;

  if (tcap_col == 0 || tcap_row == 0) return;

  double px = W.pix_to_real(1);

  double tw = W.text_width("H");
  double th = line_height;

  double x = (tcap_col-1)*tw;
  double y = win_ymax - tcap_row*line_height + 0.15*th;
  W.set_mode(xor_mode);
  W.draw_box(x,y,x+tw,y+th,blue);
  W.set_mode(src_mode);
}

void tcap_show_cursor()
{ //cout << string("show cursor(%d,%d)",tcap_row,tcap_col) << endl;
  if (!tcap_cursor_visible) tcap_draw_cursor();
  tcap_cursor_visible = true;
}

void tcap_hide_cursor()
{ //cout << string("hide cursor(%d,%d)",tcap_row,tcap_col) << endl;
  if (tcap_cursor_visible) tcap_draw_cursor();
  tcap_cursor_visible = false;
}

void tcap_move_cursor(int row, int col)
{ window& W = *wp;

  if (tcap_col == col && tcap_row == row) return;
/*
  cout << string("tcap_move_cursor(%d,%d) --> (%d,%d)",
                                   tcap_row,tcap_col,row,col) << endl;
*/
  if (tcap_cursor_visible)  tcap_draw_cursor();
  tcap_col = col;
  tcap_row = row;
  if (tcap_cursor_visible)  tcap_draw_cursor();
}



void tcap_clear_line()
{ window& W = *wp;
  //cout << string("clear_line(%d,%d)",tcap_row,tcap_col) << endl;
  double tw = W.text_width("H");
  double x1 = (tcap_col-1)*tw;
  double y1 = win_ymax - tcap_row*line_height;
  double x2 = W.xmax();
  double y2 = y1 + line_height;
  W.draw_box(x1,y1,x2,y2,white);
}

void tcap_clear_display(string txt)
{ //cout << "clear_display" << endl;
  tcap_hide_cursor();
  text = txt;
  text_height = 0;
  scroll_top = 0;
  wp->clear();
}


void tcap_scroll(int dy)
{ window& W = *wp;
/*
  cout << string("tcap_scroll(%d,%d,%+d)",
                      tcap_scroll_start, tcap_scroll_end,dy) << endl;
*/
  double scroll_ymin = win_ymax - tcap_scroll_end*line_height;
  double scroll_ymax = win_ymax - (tcap_scroll_start-1)*line_height;

  double x0 = W.xmin();
  double x1 = W.xmax();
  double y0 = scroll_ymin;
  double y1 = scroll_ymax - dy*line_height;

  if (dy < 0) {
    y0 = scroll_ymin - dy*line_height;
    y1 = scroll_ymax;
   }

  assert(y0 <= y1);
  assert(y0 >= scroll_ymin);
  assert(y0 <= scroll_ymax);
  assert(y1 >= scroll_ymin);
  assert(y1 <= scroll_ymax);

  char* win_pr = W.get_window_pixrect();

  char* pm = W.get_pixrect(x0,y0,x1,y1);

  W.start_buffering();
  W.set_pixrect(win_pr);
  W.draw_box(x0,scroll_ymin,x1,scroll_ymax,white);
  W.put_pixrect(x0,y0+dy*line_height,pm);
  W.flush_buffer();
  W.stop_buffering();

  W.del_pixrect(pm);
}


void tcap_text_attribute(int x, string cmd)
{
  switch (x) {

     case  0: // reset attributes
              //cout << "reset attributes" << endl;
              tcap_text_color = black;
              break;

     case  1: // bold
              //cout << "bold text" << endl;
              break;

     case  4: // underline start
              //cout << "underline start" << endl;
              break;

     case  5: // blinking on
              //cout << "blinking on" << endl;

     case  7: // standout start
              //cout << "standout start" << endl;
              break;

     case 23: // normal
              //cout << "normal text" << endl;
              break;

     case 24: // underline end
              //cout << "underline end" << endl;
              break;

     case 27: // standout end
              //cout << "standout end" << endl;
              break;

     case 29: // rmxx ?
              //cout << "rmxx" << endl;
              break;

     case 33: 
              //cout << "textcolor: yellow" << endl;
              break;

     case 37: 
              //cout << "textcolor: white" << endl;
              break;

     case 41: 
              //cout << "background: red" << endl;
              tcap_text_color = red;
              break;

     default: 
              //cout << "not recognized" << endl;
              cout << "attrib: " << escape(cmd) << endl;
              break;
   }
}



void tcap_text(string s)
{ window& W = *wp;

  if (s == "") return;

  int len = s.length();

  //cout << string("tcap_text(%d,%d): |%s|",tcap_row,tcap_col,~s) << endl;

  double char_w = W.text_width("H");

  double x = (tcap_col-1)*char_w;
  double y = win_ymax - (tcap_row-1)*line_height;

  int maxlen = cols - tcap_col + 1;

  string rest;
  if (len > maxlen) 
  { rest = s.substring(maxlen);
    s = s.substring(0,maxlen);
  }

  bool cursor_visible = tcap_cursor_visible;
  tcap_hide_cursor();

  W.set_text_mode(opaque);
  W.draw_text(x,y,s,tcap_text_color);
  W.set_text_mode(transparent);

  if (cursor_visible) tcap_show_cursor();

  //tcap_col += s.length(); // move cursor
  tcap_move_cursor(tcap_row,tcap_col+s.length());

  tcap_last_char = s.tail(1); // keep last char (for repeat cmd)

  if (rest != "") tcap_text(rest);

}



string tcap(string& word, string s, string cmd)
{ if (word.starts_with(s))
  { //cout << "tcap: word = " << word.replace("\e","\\E") << endl;
    // remove prefix s from word
    word = word.substring(s.length());
    //cout << "cmd = " << cmd << endl;
    return cmd;
  }
  return "";
}


void parse_text(string& text, string s)
{ window& W = *wp;

  // handle termcap commands

  if (s.length() == 0) return;

  if (tty_raw && s.length() == 1 && isprint(s[0]))
  { // single character at ":" prompt
    tcap_hide_cursor();
    tcap_text(s);
    tcap_show_cursor();
    return;
  }

  //cout << "s = " << escape(s) << endl;
  
  int p = 0;

  while (p < s.length())
  { 
    //int q = s.index("\e",p+1);
    //if (q == -1) q = s.length();

    int q = p+1;
    while(q < s.length())
    { 
      if (!isprint(s[q])) break;
/*
      if (s[q] == '\e') break; // ctrl-[   escape
      if (s[q] == BS) break;   // ctr-H    back space
      if (s[q] == LF) break;   // ctrl-J   line feed
      if (s[q] == CR) break;   // ctrl-M   carriage return
*/
      q++;
     }

    string word = s.substring(p,q);
    p = q;

    if (word == "") continue;

    //cout << escape(word) << endl;


    if (tty_raw)
    {
      // check single control characters

      if (word[0] == LF) {
        if (tcap_row < lines-1) 
           tcap_move_cursor(tcap_row+1,tcap_col);
        else
           tcap_scroll(+1);
        tcap_text(word.substring(1));
        continue;
      }
  
      if (word[0] == CR) {
        tcap_move_cursor(tcap_row,1);
        tcap_text(word.substring(1));
        continue;
      }
  
      if (word[0] == BS) {
        if (tcap_col > 1) tcap_move_cursor(tcap_row,tcap_col-1);
        tcap_text(word.substring(1));
        continue;
      }
    }

    // check escape sequences

    string cmd = "";

    if ((cmd = tcap(word,"\eM","scroll_text_down")) != "") // scroll down
    { tcap_scroll(-1);
      continue;
     }

    if ((cmd = tcap(word,"\e]2;","frame_label")) != "") { //\e]2word\a 
      wp->set_frame_label(word);
      continue;
    }


    if ((cmd = tcap(word,"\e(0","alternate_char_start")) != "") { // as
      // clear alternate screenbuffer
      continue;
    }

    if ((cmd = tcap(word,"\e(B","alternate_char_end")) != "") { // ae
      // clear alternate screenbuffer
      continue;
    }


    // \e[...

    if ((cmd = tcap(word,"\e[4h","insert_mode")) != "") { // im
      continue;
    }

    if ((cmd = tcap(word,"\e[4l","insert_mode_end")) != "") { // ei
      continue;
    }

    if ((cmd = tcap(word,"\e[?1049h","smcup")) != "") {
      // clear alternate screenbuffer
      cout << "TERMCAP START" << endl;
      tty_raw = true;
      text = "";
      text_height = 0;
      scroll_top = 0;
      wp->clear();
      continue;
    }

    if ((cmd = tcap(word,"\e[?1049l","rmcup")) != "") {
      // restore
      cout << "TERMCAP END" << endl;
      tty_raw = false;
      continue;
    }

    if ((cmd = tcap(word,"\e[?7h","keypad_transmit_start1")) != "") {
      continue;
    }
    if ((cmd = tcap(word,"\e=","keypad_transmit_start2")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?1l","keypad_transmit_end1")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e>","keypad_transmit_end2")) != "") {
      continue;
    }


    if ((cmd = tcap(word,"\e[22;0;0t","begin_cursor_motion")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[23;0;0t","end_cursor_motion")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?1h","keypad_transmit")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?5h","flash")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?1004l","focus_event")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?1004h","focus event")) != "") {
      continue;
    }

    if ((cmd = tcap(word,"\e[?5l","visible_bell")) != "") { // vb
      continue;
     }

    if ((cmd = tcap(word,"\e[?25h","cursor_normal1")) != "") { // ve
      tcap_show_cursor();
      tcap_text(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[?12l","cursor_normal2")) != "") { // ve
/*
      tcap_show_cursor();
      tcap_text(word);
*/
      continue;
    }

    if ((cmd = tcap(word,"\e[?25l","cursor_invisible")) != "") { //vi
      tcap_hide_cursor();
      tcap_text(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[?25h","cursor_visible")) != "") { // vs
      tcap_show_cursor();
      tcap_text(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[22;0;0t","termcap_start")) != "") { // ti
      continue;
    }

    if ((cmd = tcap(word,"\e[23;0;0t","termcap_end")) != "") { // te
      continue;
    }

    if ((cmd = tcap(word,"\e[J","clear_display1")) != "") { //cd
      if (tty_raw) tcap_clear_display(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[2J","clear_display2")) != "") { //cd
      tcap_clear_display(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[3J","clear_display3")) != "") { //cd
      if (tty_raw) tcap_clear_display(word);
      continue;
    }

    if ((cmd = tcap(word,"\e[K","clear_to_end_of_line")) != "") { //ce
      tcap_clear_line();
      tcap_text(word);
      continue;
    }


    if ((cmd = tcap(word,"\e[6n","u7")) != "") { // u7
      continue;
    }
    if (word.starts_with("\e[H")) { // home
      cmd = "move_home";
      word = word.substring(3);
      //cout << "cmd  = " << ecape(word) << endl;
      tcap_move_cursor(1,1);
      tcap_text(word);
      continue;
    }


    if (word.starts_with("\e[")) // \e[%d{A,B,C,D,G,L,M,m}  (1 Parameter)
    { int i = -1;

      if (word[2] == 'A') { i = 2; cmd = "move_up";    } // UP
      if (word[3] == 'A') { i = 3; cmd = "move_up";    }
      if (word[4] == 'A') { i = 4; cmd = "move_up";    }

      if (word[2] == 'B') { i = 2; cmd = "move_down";  } // DO 
      if (word[3] == 'B') { i = 3; cmd = "move_down";  }
      if (word[4] == 'B') { i = 4; cmd = "move_down";  }

      if (word[2] == 'C') { i = 2; cmd = "move_right"; } // RI
      if (word[3] == 'C') { i = 3; cmd = "move_right"; } // RI
      if (word[4] == 'C') { i = 4; cmd = "move_right"; }

      if (word[2] == 'D') { i = 2; cmd = "move_left";  } //LE 
      if (word[3] == 'D') { i = 3; cmd = "move_left";  }
      if (word[4] == 'D') { i = 4; cmd = "move_left";  }

      if (word[2] == 'd') { i = 2; cmd = "cursor_row";  } //cv 
      if (word[3] == 'd') { i = 3; cmd = "cursor_row";  } 
      if (word[4] == 'd') { i = 4; cmd = "cursor_row";  }

      if (word[2] == 'b') { i = 2; cmd = "repeat_char";  } //REP 
      if (word[3] == 'b') { i = 3; cmd = "repeat_char";  } 
      if (word[4] == 'b') { i = 4; cmd = "repeat_char";  }

      if (word[2] == 'G') { i = 2; cmd = "cursor_column";  } //ch 
      if (word[3] == 'G') { i = 3; cmd = "cursor_column";  }
      if (word[4] == 'G') { i = 4; cmd = "cursor_column";  }

      if (word[2] == 'L') { i = 2; cmd = "add_lines";  } // AL
      if (word[3] == 'L') { i = 3; cmd = "add_lines";  }
      if (word[4] == 'L') { i = 4; cmd = "add_lines";  }
 
      if (word[2] == 'M') { i = 2; cmd = "del_lines";  } // DL
      if (word[3] == 'M') { i = 3; cmd = "del_lines";  }
      if (word[4] == 'M') { i = 4; cmd = "del_lines";  }

      if (word[2] == 'm') { i = 2; cmd = "text_attrib";  }
      if (word[3] == 'm') { i = 3; cmd = "text_attrib";  }
      if (word[4] == 'm') { i = 4; cmd = "text_attrib";  }

      if (i != -1)
      { int val = (i == 2) ? 1 : word(2,i-1).atoi();

        if (i == 2) 
        { // empty value field
          val = 1;
          if (cmd == "text_attrib") val = 0;
         }

        
        string tcap_cmd = word.substring(0,i+1);

        word = word.substring(i+1);

        //cout << "cmd = " << ecape(cmd) << " val = " << val << endl;

        if (cmd == "repeat_char") {
          //cout << string("REPEAT: %d  c = %s",val,~tcap_last_char) << endl;
          for(int i=0; i<val; i++) tcap_text(tcap_last_char);
          tcap_text(word);
        }


        if (cmd == "cursor_row") {
          tcap_move_cursor(val,tcap_col);
          tcap_text(word);
        }

        if (cmd == "cursor_column") {
          tcap_move_cursor(tcap_row,val);
          tcap_text(word);
        }


        if (cmd == "move_up") {
          tcap_move_cursor(tcap_row-val,tcap_col);
          tcap_text(word);
         }

        if (cmd == "move_down") {
          tcap_move_cursor(tcap_row+val,tcap_col);
          tcap_text(word);
         }

        if (cmd == "move_left") {
          tcap_move_cursor(tcap_row,tcap_col-val);
          tcap_text(word);
         }

        if (cmd == "move_right") {
          tcap_move_cursor(tcap_row,tcap_col+val);
          tcap_text(word);
         }

        if (cmd == "add_lines") tcap_scroll(-val);

        if (cmd == "del_lines") tcap_scroll(+val);

        if (cmd == "text_attrib")
        { tcap_text_attribute(val,tcap_cmd);
          tcap_text(word);
         }

       continue;
    }


    if (word.starts_with("\e[")) // \e[%d;%d{H,r,t}  (2 Parameters)
    { int i = -1;
      int j = -1;
      if (word[3] == ';') i = 3;
      if (word[4] == ';') i = 4;

      if (i != -1)
      { if (word[i+2] == 'H') { j = i+2; cmd = "move"; } // cm
        if (word[i+3] == 'H') { j = i+3; cmd = "move"; }

        if (word[i+2] == 'r') { j = i+2; cmd = "scroll_region"; } //cs
        if (word[i+3] == 'r') { j = i+3; cmd = "scroll_region"; }

        if (word[i+2] == 't') { j = i+2; cmd = "scroll2"; } // cs
        if (word[i+3] == 't') { j = i+3; cmd = "scroll2"; }
       }

      if (j != -1)
      { int val1 = word(2,i-1).atoi();
        int val2 = word(i+1,j-1).atoi();
        word = word.substring(j+1);

        //cout << "cmd = " << ecape(cmd) << string(" (%d,%d)", val1,val2);
        //cout << endl;

        if (cmd == "move")
        { tcap_move_cursor(val1,val2);
          tcap_text(word);
          continue;
         }

        if (cmd == "scroll_region")
        { tcap_scroll_start = val1;
          tcap_scroll_end = val2;
          continue;
         }

        continue;
       }
     }

    }

    if (tty_raw)
    { if (isprint(word[0]))
      { //cout << "word = " << escape(word);
        tcap_text(word);
       }
      else
      { // unrecognized control sequence
        //cout << "TERMCAP: " << escape(word) << endl;
      }
     }
    else
      text += word.replace("\e","\\E");
  }

}



static void print_text(string text, bool update_sb)
{ window& W = *wp;

  if (tty_raw) return;

  if (update_sb) 
  { 
    text_height = 0;

    string s;
    forall_lines(s,text) text_height += line_height;

    double display_height = W.ymax() - W.ymin() - line_height;

    if (text_height >= display_height) {
      scroll_top = text_height - display_height;
    }

    double sb_pos = scroll_top/text_height;
    double sb_size = display_height/text_height;
    if (sb_size > 1.0) sb_size = 1.0;
    W.set_scrollbar_pos(sb_pos,sb_size);
  }


  double x = W.xmin() + W.pix_to_real(xmargin);
  double y = W.ymax() + scroll_top - W.pix_to_real(ymargin);

  string txt = text + cursor;

  W.start_buffering();
  W.clear();

  string s;
  forall_lines(s,txt) 
  { //if (s[s.length()-1] == 13) s = s.head(-1); // remove trailing \n
    if (s.length() == 1 && s[0] == 13) s[0] = '\0';
    if (y <= W.ymax() && y >= W.ymin()) W.draw_text(x,y,s.expand_tabs(8));
    y -= line_height;
  }

  W.flush_buffer();
  W.stop_buffering();
}
     


void scroll_up(int i)
{ if (scroll_top < 0.1) return;
  scroll_top -= line_height;
  wp->set_scrollbar_pos(scroll_top/text_height);
  print_text(text,false);
}

void scroll_down(int i)
{ if (scroll_top > (text_height - line_height - 0.001)) return;
  scroll_top += line_height;
  wp->set_scrollbar_pos(scroll_top/text_height);
  print_text(text,false);
}

void scroll_drag(int i)
{ if (i < 0) return;
  scroll_top = (i/1000.0)*(text_height - line_height);
  if (scroll_top < 0) scroll_top = 0;
  print_text(text,false);
}


void redraw(window* wp) {
   print_text(text,true);
}

void close_handler(window* wp) {
  cout << "TERMINAL: CLOSE HANDLER" << endl;
  closed = true;
}


bool fd_poll(int fd, int msec)
{ timeval polltime;
  polltime.tv_sec  = msec / 1000;
  polltime.tv_usec = 1000 * (msec % 1000);

  fd_set rdset;
  FD_ZERO(&rdset);
  FD_SET(fd,&rdset);
  return select(fd+1,&rdset,0,0,&polltime) > 0;
}


int fd_select(int fd1, int fd2)
{ 
  int msec = 100;
  timeval polltime;
  polltime.tv_sec  = 0;
  polltime.tv_usec = 1000 * msec;

  fd_set rdset;
  FD_ZERO(&rdset);
  FD_SET(fd1,&rdset);
  FD_SET(fd2,&rdset);
  int fd = fd1;
  if (fd2 > fd) fd = fd2;
  //select(fd+1,&rdset,0,0,0);
  select(fd+1,&rdset,0,0,&polltime);

  if (FD_ISSET(fd1,&rdset)) return fd1;
  if (FD_ISSET(fd2,&rdset)) return fd2;
  return -1;
}




int main(int argc, char** argv) 
{
  signal(SIGINT,sig_handler);
  signal(SIGTERM,sig_handler);
  signal(SIGQUIT,sig_handler);

  signal(SIGCHLD,sig_handler);
  //signal(SIGCHLD,SIG_IGN);

  signal(SIGUSR1,sig_handler);

  atexit(exit_handler);


  int  dpi = window::screen_dpi();
  bool portrait = window::screen_height() > 1.5*window::screen_width();

  string host = get_host_name();

  setenv("HOSTNAME",host,1);

  const char* shell_tcsh_argv[] = { "/bin/tcsh", "-i", 0 };
  const char* shell_bash_argv[] = { "/bin/bash", "-i", 0 };
  const char* shell_su_argv[] = { "/usr/bin/su","-f","-c","/bin/tcsh -i",
                                                        "naeher", 0 };


  const char** prog_argv = 0;

  if (argc < 2)
  { prog_argv = shell_su_argv;
    char* home = getenv("HOME");
    if (home) set_directory(home);
    text = "";
  }
  else
  { prog_argv = (const char**)(argv+1);
    text = "\n" + string(argv[1]) + "\n";
   }

  cout << title << " dpi = " << dpi <<  "  prog = " << prog_argv[0] << endl;


  window W(title);
  wp = &W;

  W.enable_close_button(true);

/*
  menu M;
  M.button("Button 1");
  M.button("Button 2");
  M.button("Settings");
  M.separator();
  M.button("Exit",999);

  W.button("Menu", M);
  W.button("done",999);
  W.make_menu_bar(1);
*/


  if (portrait) lines = 24;


  W.set_font(string("F%d",font_sz));
  W.set_window_close_handler(close_handler);
  W.set_redraw(redraw);

  W.display();

  W.open_scrollbar(scroll_up, scroll_down, scroll_drag,0,0);
  W.set_focus();

  int fd_disp = W.display_fd();

  cout << "display: fd = " << fd_disp << endl;

  double tw = W.text_width("H");
  double th = W.text_height("H");

  line_height = height_f*th;

  if (!getenv("LEDA_OPEN_MAXIMIZED"))
  { 
    // resize

    int win_width = W.real_to_pix((cols+1.5)*tw) + W.scrollbar_width();

    //int max_width = 0.97*window::screen_width();
    int max_width = window::screen_width();
    if (win_width > max_width) win_width = max_width;

    line_height = height_f*th;

    int win_height = lines * W.real_to_pix(line_height);

    cout << "lines  = " << lines << endl;
    cout << "cols   = " << cols << endl;
    cout << "width  = " << win_width << endl;
    cout << "height = " << win_height << endl;
    cout << "line_height = " << W.real_to_pix(line_height) << endl;

    W.resize(win_width,win_height);

    int xpos = (window::screen_width() - (win_width  + (30*dpi)/192))/2;
    int ypos = (window::screen_height()- (win_height + (50*dpi)/192))/2;
    if (portrait) ypos = 15;
    W.resize(xpos,ypos,win_width,win_height);


    tw = W.text_width("H");

    cols = (W.width() - W.scrollbar_width())/W.real_to_pix(tw) - 1; 
    cout << "terminal: width = " << W.width() << " cols = " << cols << endl;

    lines = W.height()/W.real_to_pix(line_height); 
    cout << "terminal: height = " << W.height() << " lines = " << lines << endl;

    cout << "line_height = " << W.real_to_pix(line_height) << endl;
  }


  if (argc == 1) 
  { // start shell 
    char* p = getenv("USER");
    if (p) user = string(p);

    if (user == "naeher") 
      pwd = "bun-run";
    else
    {
      panel P("Login");
      P.text_item("\\bf Login");
      P.text_item("");
      panel_item it1 = P.string_item("Username ", user);
      panel_item it2 = P.string_item("Password ", pwd);
      P.set_secret(it2,true);
      P.fbutton("continue");
  
      P.display(W);

      P.activate_item(it1);
      P.set_focus();
      while (user == "" || pwd == "") {
        P.read();
        P.activate_item(it2);
      }
    }

/*
    cout << "user = " << user << endl;
    cout << "pwd = " << pwd << endl;
*/
 
  //prog_argv[4] = ~user;
    prog_argv[4] = strdup(user);
    pwd += "\n";
  }

  setenv("COLUMNS",~string("%d",cols),1);
  setenv("LINES",~string("%d",lines),1);


  win_ymax = W.ymax() - th/4;
  win_ymin = win_ymax - lines * line_height;

  int output_pipe[2];
  output_pipe[0] = -1;
  output_pipe[1] = -1;

  int input_pipe[2];
  input_pipe[0] = -1;
  input_pipe[1] = -1;

  if (pipe(output_pipe) < 0) {
    cerr << "Could not create output pipe." << endl;
    return 1;
  }

  if (pipe(input_pipe) < 0) {
    cerr << "Could not create input pipe." << endl;
    return 1;
  }

  // start program in backround and redirect its output from stdout 
  // to one end (1) of the output pipe and its input from stdin to 
  // one end (0) of the input pipe  (using bash command line syntax)

/*
  start_child_process(prog,output_pipe[1],input_pipe[0]);
*/
  int pid = fork();

  if (pid == 0) //child
  {
    dup2(input_pipe[0], 0);
    close(input_pipe[1]);

    dup2(output_pipe[1],1);
    dup2(output_pipe[1],2);
    close(output_pipe[0]);

    execv(prog_argv[0],(char* const*)prog_argv);

    return 0;
  }

  // parent

  child_pid = pid;

  string line = "";

  while (!closed)
  {
    char buf[1024];
 
    int fd_read = output_pipe[0]; 
    int fd_write = input_pipe[1]; 
 
    //int fd = fd_select(fd_read,fd_disp);
    int fd = fd_select(fd_disp,fd_read);
 
    if (fd == -1) continue;
 
    if (fd == fd_read) 
    { // read output pipe of program
      int sz = read(fd_read, buf, 1023);
      buf[sz] = '\0';

      string s = buf;

/*
for(int i=0; i<sz; i++) {
if (!isprint(buf[i])) 
   cout << "NOT PRINT: i = " << i << "  c = " << int(buf[i]) <<endl;
//if (buf[i] == 13) buf[i] = ' ';
}
*/
 
      if (s.index("Authentication failure") != -1)
      { cursor = "";
        print_text("Authentication failure.",true);
        leda_wait(1.5);
        break;
       }

      if (s.starts_with("Password:"))
      { s = "";
        write(fd_write,pwd,pwd.length());
       }

      if (s.starts_with("Warning:")) s = "";

      parse_text(text,s);

      if (text.length() > 50000) text = text.tail(25000);
 
      cursor = "_";

/*
      if (text.starts_with("Password:"))
      { text = "";
        write(fd_write,pwd,pwd.length());
       }

      if (text.starts_with("Warning:")) text = "";
*/

      print_text(text,true);
 
      continue;
     }
 

    // fd = fd_disp 
    // handle window events
 
    int val = 0;
    double x,y;
    unsigned long t;
    window* wp;
 
/*
    int e = W.read_event(val,x,y,t);
    int e = W.read_event(val,x,y,t,250);
*/

    int e = W.read_event(val,x,y,t,100);
 

    if (e == display_event && val == 0) {
       cout << "TERMINAL: DISPLAY_EVENT 0" << endl;
       break; 
    }

 
    if (e == button_press_event) 
    { //cout << "button pressed: " << val << endl;
 
      if (val == MOUSE_BUTTON(1)) 
      { if (child_pid == 0) break;
        W.keyboard(-1); // toggle keyboard
       }
 
      if (val == CLOSE_BUTTON) break;
 
      if (val == 999)  break;
 
      if (val == MOUSE_BUTTON(4)) scroll_up(0);
      if (val == MOUSE_BUTTON(5)) scroll_down(0);
 
      continue;
    }
 
    if (e == key_press_event) 
    { 
      if (child_pid == 0 && (val == KEY_RETURN || val == 3)) break;

      if (tty_raw)
      { if (val == KEY_RETURN) val = 10;
        if (val == KEY_ESCAPE) val = 27;
        if (val == KEY_BACKSPACE) val = 8;
        write(fd_write,&val,1);
        fsync(fd);
        continue;
      }
 
      if (val == KEY_UP) {
         scroll_up(0);
         continue;
      }
 
      if (val == KEY_DOWN) {
        scroll_down(0);
        continue;
      }

      if (val == KEY_ESCAPE) val = 27;
 

      if (val == 3) break;

      if (val == KEY_BACKSPACE)
      { if (line != "")
        { line = line.head(-1);
          if (text != "Password: ") text = text.head(-1);
          print_text(text,true);
         }
        continue;
       }

      if (val == KEY_RETURN) 
      { 
        if (text_height > line_height || !text.starts_with("Password: ")) {
          cursor = "";
        }
        line += "\n";
        text += "\n";
 
        print_text(text,true);
     
        // write line to input pipe of prog
        write(fd_write,line,line.length());
        fsync(fd);
     
        if (line == "exit\n") {
          leda_wait(0.5);
          break;
        }
     
        line = "";
        continue;
      }

 
      //if (isprint(val))
      { // local echo
        line += char(val);
        if (text_height == line_height && text.starts_with("Password: "))
         text += ".";
        else
        { if (isprint(val))
            text += char(val);
          else
            text += string("^%c",'@'+val);
         }
        print_text(text,true);
        continue;
       }
    }
 
  }
 
  cout << "TERMINAL: FINISHED" << endl;
 
  W.keyboard(0);
 
  return 0;
}
 
#endif
