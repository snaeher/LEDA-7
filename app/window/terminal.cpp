#include <LEDA/graphics/window.h>
#include <LEDA/system/file.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

using namespace leda;


#if !defined(__unix__)

int main() { 
  cerr << endl;
  cerr << "This program works only for Unix." << endl;
  cerr << endl;
  return 0; 
}

#else

#include <unistd.h>
#include <signal.h>
#include <string.h>


using namespace leda;

static string title = "LEDA Terminal";

static window* wp = 0;
static string text;
static string cursor;

static int win_width = 0;
static int win_height = 0;
static int xmargin = 5;
static int ymargin = 10;

static double line_height = 0;
static double text_height = 0;
static double scroll_top = 0;

static bool closed = false;

static int child_pid = 0;


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


void sig_handler(int s) {
  cout << string("TERMINAL: signal(%d): %s ---> EXIT",s,strsignal(s)) << endl;
  exit(0);
}


void handle_controls(string& s)
{
  // cursor up         up = \eA
  // turn off attribs  me = \e[0m
  // turn on bold      md = \e[1m
  // underline start   us = \e[4m
  // turn on blinking  mb = \e[5m
  // standout start    so = \e[7m
  //                   ue = \e[23m
  // underline end     ue = \e[24m
  // standout end      se = \e[27m
  //                   ue = \e[29m

  // clear to end of display cd = \e[(3)J
  // clear screen and home   cl = \e[H\e]2J

  // audible bell        bl = ^G
  // begin termcap usage ti = \E[?1049h\E[22;0;0t
  // end termcap usage   te = \E[?1049l\E[23;0;0t


  // \e[?2004h
  // \e=
  // cs = \e[%d;%dr
  // \e[%d;%dt
  // \e[6n
  // ce = \e[K

  // cursor move     cm =\e[%i%d;%dH
  // home & clear    cl =\e[H\e[2J
  // clear to end    cd =\e[J

  // set scroll region cs = \e[%i%d;%dr

  // begin cursor motion ti = \e[?1049h\E[22;0;0t
  // end   cursor motion te = \e[?1049l\E[23;0;0t

  string clear_ctrl1 = "\e[3J\e[H\e[2J";
  string clear_ctrl2 = "\e[H\e[2J";
  string label_ctrl  = "\e]2;";

  int p = 0;

/*
  p = s.index("\e[");

  if (p == -1) return;

  if (s(p,p+5) == "\e[?25l") { 
   // cursor invisible
   s = s.del(p,p+5);
   return;
  }

  if (s(p,p+5) == "\e[?25h") { 
   // cursor visible
   s = s.del(p,p+5);
   return;
  }
*/


  if ((p = s.index(clear_ctrl1)) != -1) 
  { //s = "\n" + s.substring(p+clear_ctrl1.length()+1);
    s = s.del(p,p+clear_ctrl1.length());
    text = "";
    text_height = 0;
    scroll_top = 0;
   }

  if ((p = s.index(clear_ctrl2)) != -1) 
  { //s = "\n" + s.substring(p+clear_ctrl2.length()+1);
    s = s.del(p,p+clear_ctrl2.length());
    text = "";
    text_height = 0;
    scroll_top = 0;
   }

  if ((p = s.index(label_ctrl)) != -1)
  { int q = s.index("\a",p); // ^G
    if (q != -1) {
      string label = s(p+label_ctrl.length(),q-1);
      s = s.del(p,q);
      wp->set_frame_label(label);
    }
  }

/*
  // move cursor  \e[x;yH

  if ((p = s.index("\e[")) != -1)
  { int q = s.index(";",p);
    if (q != -1)
    { int r = s.index("H",q);
      if (r != -1)
      { int x = s.substring(p+2,q).atoi();
        int y = s.substring(q+1,r).atoi();
        // move cursor to (x,y)
        cout << "move to: " << x << " " << y << endl;
        s = s.substring(r+1);
      }
    }
  }
*/

  p = 0;
  //while ((p = s.index("\e[",p)) != -1) 
  while ((p = s.index("\e",p)) != -1) 
  {  
     p+=1;
     //int q = s.index("\e[",p);
     int q = s.index("\e",p);
     cout << p << ": \\E" << s(p,q-1) << endl;
  }

}



static void print_text(bool update_sb)
{ window& W = *wp;

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
  print_text(false);
}

void scroll_down(int i)
{ if (scroll_top > (text_height - line_height - 0.001)) return;
  scroll_top += line_height;
  wp->set_scrollbar_pos(scroll_top/text_height);
  print_text(false);
}

void scroll_drag(int i)
{ if (i < 0) return;
  scroll_top = (i/1000.0)*(text_height - line_height);
  if (scroll_top < 0) scroll_top = 0;
  print_text(false);
}


void redraw(window* wp) {
   print_text(true);
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

  //signal(SIGCHLD,sig_handler);
  signal(SIGCHLD,SIG_IGN);

  signal(SIGUSR1,sig_handler);

  atexit(exit_handler);


  int  dpi = window::screen_dpi();
  bool portrait = window::screen_height() > 1.5*window::screen_width();

  string host = get_host_name();

  setenv("HOSTNAME",host,1);

/*
  const char* shell_argv[] = { "/bin/tcsh", "-i", 0 };
*/

  const char* shell_argv[] = { "/usr/bin/su","-f","-c","/bin/tcsh -i",
                                                       "naeher", 0 };


  const char** prog_argv = 0;

  if (argc < 2)
  { prog_argv = shell_argv;
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


  int cols = 80;
  int lines = portrait ? 28 : 35;

  //int font_sz = 38;
  //float height_f = 1.12;

  int font_sz = 40;
  float height_f = 1.0;

  if (window::display_type() == "x11") height_f = 0.95;

  cout << "font_sz = " << font_sz << endl;

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

  win_width = W.real_to_pix((cols+1)*tw) + W.scrollbar_width();

  int max_width = 0.97*window::screen_width();
  if (win_width > max_width) win_width = max_width;

  line_height = height_f*th;

  win_height = W.real_to_pix(lines * line_height);

  cout << "width  = " << win_width << endl;
  cout << "height = " << win_height << endl;

  int xpos = (window::screen_width() - (win_width  + (30*dpi)/192))/2;
  int ypos = (window::screen_height()- (win_height + (50*dpi)/192))/2;

  if (portrait) ypos = 15;

  W.resize(xpos,ypos,win_width,win_height);

  tw = W.text_width("H");

  cols = (W.width() - W.scrollbar_width())/W.real_to_pix(tw) - 1; 
  cout << "terminal: width = " << W.width() << " cols = " << cols << endl;

  lines = W.height()/W.real_to_pix(line_height); 
  cout << "terminal: height = " << W.height() << " lines = " << lines << endl;

  setenv("COLUMNS",~string("%d",cols),1);
  setenv("LINES",~string("%d",lines),1);


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
 
      if (s.index("Authentication failure") != -1)
      { text += "AUTHENTICATION FAILURE ---> EXIT";
        cursor = "";
        print_text(true);
        leda_wait(1.5);
        break;
      }
 
      handle_controls(s);
 
      text += s;
      if (text.length() > 50000) text = text.tail(25000);
 
      cursor = "_";
      print_text(true);
 
      continue;
     }
 

    // fd = fd_disp 
    // handle window events
 
    int val = 0;
    double x,y;
    unsigned long t;
    window* wp;
 
  //int e = W.read_event(val,x,y,t);
  //int e = W.read_event(val,x,y,t,500);
    int e = W.read_event(val,x,y,t,250);
 
/*
    if (e == display_event && val == 0) {
       cout << "TERMINAL: DISPLAY_EVENT 0" << endl;
       break; 
    }
*/
 
    if (e == button_press_event) 
    { //cout << "button pressed: " << val << endl;
 
      if (val == MOUSE_BUTTON(1)) W.keyboard(-1); // toggle keyboard
 
      if (val == CLOSE_BUTTON) break;
 
      if (val == 999)  break;
 
      if (val == MOUSE_BUTTON(4)) scroll_up(0);
      if (val == MOUSE_BUTTON(5)) scroll_down(0);
 
      continue;
    }
 
 
    if (e == key_press_event) 
    { 
      //cout << "key pressed: " << val << endl;
 
      if (val == KEY_ESCAPE) break;
 
      if (val == 3) 
      { // ctrl-c
        break;
 /*
        string s = string(char(3));
        write(fd_write,s,s.length());
        continue;
 */
      }
 
      if (val == KEY_UP) {
         scroll_up(0);
         continue;
      }
 
      if (val == KEY_DOWN) {
        scroll_down(0);
        continue;
      }
 
      if (isprint(val))
      { line += char(val);
        if (text_height == line_height && text.starts_with("Password: "))
         text += ".";
        else
         text += char(val);
       }
      else
      { if (val == KEY_BACKSPACE && line != "")
        { line = line.head(-1);
          if (text != "Password: ") text = text.head(-1);
         }
       }
       
   
      print_text(true);
   
      if (val == KEY_RETURN) 
      { 
        if (text_height > line_height || !text.starts_with("Password: ")) {
          cursor = "";
        }
        line += "\n";
        text += "\n";
 
        print_text(true);
     
        // write line to input pipe of prog
        write(fd_write,line,line.length());
     
        if (line == "exit\n") {
           leda_wait(0.5);
           break;
        }
     
        line = "";
      }
   
      continue;
    }
 
  }
 
  cout << "TERMINAL: FINISHED" << endl;
 
  W.keyboard(0);
 
  return 0;
}
 
#endif
