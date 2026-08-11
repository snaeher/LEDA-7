#include <LEDA/graphics/terminal.h>
#include <LEDA/system/file.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

using namespace leda;


#if defined(__win32__)

int main()
{ cout << endl;
  cout << "This program cannot be used on MS-Windows." << endl;
  cout << endl;
  return 0; 
}

#else

#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <sys/termios.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>


static terminal* tp = 0;
static termios orig_termios;
static termios raw_termios;
static string user_prog;


int start_shell(const char** prog_argv)
{
  // return master fd

  int rows = 30;
  int cols = 80;
  
  // save current termios setting
  tcgetattr(0,&orig_termios);

  // set up raw termios setting
  raw_termios = orig_termios; 
//raw_termios.c_lflag &= ~(ECHO | ICANON);
  raw_termios.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
  raw_termios.c_oflag &= ~OPOST;
  raw_termios.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  raw_termios.c_cflag &= ~(CSIZE|PARENB);
  raw_termios.c_cflag |= CS8;

/*
  // requires -lutil library
  int fdm,fds;
  if (openpty(&fdm,&fds,NULL,NULL,NULL) < 0)
  { cerr << "openpty failed: " << strerror(errno) << endl;
    return 1;
  }
*/

  int fdm = posix_openpt(O_RDWR); // pty master fd

  if (fdm < 0)
  { cerr << "posix_opnpt failed: " << strerror(errno) << endl;
    return 1;
   }

  if (grantpt(fdm) != 0)
  { cerr << "grantpt failed: " << strerror(errno) << endl;
    return 1;
  }

  if (unlockpt(fdm) != 0)
  { cerr << "unlockpt failed: " << strerror(errno) << endl;
    return 1;
  }

  int fds = open(ptsname(fdm),O_RDWR); // pty slave fd

  if (fork() == 0)
  {
    // child (pty slave): exec shell and put it on the slave end

    close(fdm); // close master fd
    close(0);
    close(1);
    close(2);

    dup2(fds,0);
    dup2(fds,1);
    dup2(fds,2);

    // create a new process group and make fds the controlling terminal 
    setsid(); 
    ioctl(fds,TIOCSCTTY,NULL);

    // exec shell
    execv(prog_argv[0],(char* const*)prog_argv);

    cerr << "execv " << prog_argv[0] << "failed "<< endl;
    cerr << "error: " << strerror(errno)<< endl;
    return 1;
   }

   // parent (pty master)

   close(fds); // close slave 

   //tcsetattr(0, TCSANOW, &raw_termios); //only if std input is a terminal ?

   return fdm;

   return 0;
}



void sig_handler(int s)
{ 
  cout << endl;
  cout << string("SIGNAL (%d) : %s",s,strsignal(s)) << endl;

  //tcsetattr (0, TCSANOW, &orig_termios); 

  if (s == SIGCHLD && user_prog) 
  { tp->set_finished(true);
    return;
   }

  exit(0);
}


void resize_handler(terminal* tp, int old_rows, int old_cols)
{ terminal& T = *tp;
  window& W = T.get_window();

  int rows = T.get_rows();
  int cols = T.get_cols();

  if (rows == old_rows && cols == old_cols)
    cout << "resize: size not changed." << endl;
  else
  { int fw = T.get_font_width();
    int fh = T.get_font_height();
    string txt("size = %d x %d  font = %d x %d", rows,cols,fw,fh);
    //W.acknowledge(txt);
    cout << txt << endl;
    cout << endl;
  }
}


void login_dialog(terminal& T, string& host, string& user, string& pwd)
{
  window& W = T.get_window();

  T.hide_cursor();

  host = "";
  user = "";
  pwd  = "";

  panel P("Login");
  P.text_item("\\bf Login");
  P.text_item("");
  panel_item it1 = P.string_item("Host ", host);
  panel_item it2 = P.string_item("User ", user);
/*
  panel_item it3 = P.string_item("Password ", pwd);
  P.set_secret(it3,true);
*/

  P.fbutton("continue",1);
  P.button("cancel",0);
  
  P.display(W);
  P.activate_item(it1);
  P.set_focus();

  //while (host == "" || user == "")
  while (host == "" || user == "")
  { int but = P.read();
    if (but == 0) {
       host = "";
       user = "";
       pwd = "";
       break;
    }

/*
    if (host == "localhost") { 
      host = ""; 
      P.redraw(); 
      P.activate_item(it1);
      continue; 
    }
*/
    P.activate_item(it2);
  }

  if (host == "localhost") host = "";

  T.show_cursor();

}


void usage() {
   cerr << endl;
   cerr << "usage: Lterm [-s size] [ cmd ]" << endl;
   exit(1);
}


int main(int argc, const char** argv)
{ 
  const char* shell_tcsh_argv[] = { "/bin/tcsh", NULL };
  const char* shell_bash_argv[] = { "/bin/bash", NULL };
  const char* shell_su_argv[]   = { "/usr/bin/su", "naeher", NULL };

  const char* shell_ssh_argv[]   = { "/usr/bin/ssh",
                                     "-o","PreferredAuthentications=password",
                                     "-o","PubkeyAuthentication=no",
                                     "-o","StrictHostKeyChecking=no",
                                     "-o","UserKnownHostsFile=none",
                                     "-o","LogLevel=ERROR",
                                     "user@host",
                                      NULL } ;

  signal(SIGINT, sig_handler);
  signal(SIGTERM,sig_handler);
  signal(SIGQUIT,sig_handler);
  signal(SIGCHLD,sig_handler);

  setenv("LEDA_TERM","1",true);

  int rows = 30;
  int cols = 80;
  //int fnt_sz = 36;
  int fnt_sz = 38;

  if (window::screen_height() > window::screen_width()) {
    // portrait (mobile)
    fnt_sz = 43;
    rows = 20;
  }

  const char** prog_argv = 0;
  string host = "";
  string user = "";
  string pwd = "";

  // parse program arguments
  // Lterm [-s size] [prog]

  if (argc >= 3 && string(argv[1]) == "-s")
  { string size = argv[2];
    int p = size.index("x");
    if (p == -1) usage();
    rows = atoi(size.substring(0,p));
    cols = atoi(size.substring(p+1));

    // shift
    argv += 2;
    argc -= 2;
  }

  if (argc > 1)
  { user_prog = argv[1];
    prog_argv = (const char**)(argv+1);
   }
    
  
  cout << "rows = " << rows << endl;
  cout << "cols = " << cols << endl;
  cout << "prog = " << user_prog << endl;
  cout << endl;


  terminal T("LEDA TERMINAL",rows,cols,fnt_sz);
  tp = &T;
  T.set_resize(resize_handler);

  T.display();

  if (user_prog == "")
  { 
    //char* home = getenv("HOME");
    //if (home) set_directory(home);

    login_dialog(T,host,user,pwd);

    if (user  == "" && host == "") exit(0); // canceled

    prog_argv = shell_ssh_argv;
    prog_argv[11] =  (user + "@" + host).cstring();

    if (pwd != "") T.set_password(pwd);
  }

  for(int i=0; i<16; i++) {
    if (prog_argv[i] == NULL) break;
    cout << string("execv: arg[%d] = ",i) << prog_argv[i] << endl;
   }
  cout << endl;


  int fdm = start_shell(prog_argv);

  T.connect(fdm);

  return 0;
}

#endif
