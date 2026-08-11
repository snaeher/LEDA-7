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

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>


static int child_pid = 0;

void signal_handler(int sig, siginfo_t* si, void* ctxt)
{ int pid  = si->si_pid;
  int stat = si->si_status;
  int code = si->si_code;

  cout << string("SIGNAL %d: \"%s\" pid = %d code = %d status = %d",
                               sig,strsignal(sig),pid,code,stat) << endl;
  if (sig == SIGINT) exit(1);

  if (sig == SIGCHLD)
  { 
    if (code == CLD_EXITED)
      cout << string("Child %d exited normally with status %d.", 
                                                       pid,stat) << endl;
    else
      cout << string("Child %d was killed by signal %d (%s).",
                                     pid,stat,strsignal(stat)) << endl;

    if (pid == child_pid && code == CLD_KILLED) {
      // exit if child was killed
      exit(1); 
    }
  }
}



int start_shell(int& fdm, const char** prog_argv)
{
  // return child pid and assign pty master fd to fdm

  int rows = 30;
  int cols = 80;
  
/*
  // requires -lutil library
  int fds;
  if (openpty(&fdm,&fds,NULL,NULL,NULL) < 0)
  { cerr << "openpty failed: " << strerror(errno) << endl;
    return 0;
  }
*/

  fdm = posix_openpt(O_RDWR); // pty master fd

  if (fdm < 0)
  { cerr << "posix_opnpt failed: " << strerror(errno) << endl;
    return 0;
   }

  if (grantpt(fdm) != 0)
  { cerr << "grantpt failed: " << strerror(errno) << endl;
    return 0;
  }

  if (unlockpt(fdm) != 0)
  { cerr << "unlockpt failed: " << strerror(errno) << endl;
    return 0;
  }

  int fds = open(ptsname(fdm),O_RDWR); // pty slave fd

  int pid = fork();

  if (pid == 0)
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
    return 0;
   }

   // parent (pty master)

   close(fds); // close slave 

   return pid;
}


string login_dialog(terminal& T)
{
  window& W = T.get_window();

  T.hide_cursor();

  string address = "";

  panel P("LEDA Login");
  P.text_item("\\bf Login");
  P.text_item("");
  P.string_item("user@host ", address);
  P.fbutton("continue");

  P.display(W);
  P.set_focus();
  P.read();
  T.show_cursor();

  W.set_focus();

  return  address;
}


void usage() {
   cerr << endl;
   cerr << "usage: Lterm [-t] [-s size] [cmd]" << endl;
   exit(1);
}


int main(int argc, const char** argv)
{ 
  const char* shell_tcsh_argv[] = { "/bin/tcsh", NULL };
  const char* shell_bash_argv[] = { "/bin/bash", NULL };
  const char* shell_ssh_argv[]  = { "/usr/bin/ssh",
                                    "-o","PreferredAuthentications=password",
                                    "-o","PubkeyAuthentication=no",
                                    "-o","StrictHostKeyChecking=no",
                                    "-o","UserKnownHostsFile=none",
                                    "-o","LogLevel=ERROR",
                                    "user@host",
                                     NULL };

  struct sigaction sa;
  sa.sa_sigaction = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;

  sigaction(SIGCHLD,&sa,0);
  sigaction(SIGINT, &sa,0); // otherwise ctrl-c is not correctly handled 

  setenv("LEDA_TERM","1",true);

  int rows = 27;
  int cols = 80;
  int fnt_sz = 41;

  bool trace = false;

  if (window::display_type() == "x11") fnt_sz = 35;

  if (window::screen_height() > 1.5*window::screen_width()) {
    // portrait (mobile)
    fnt_sz = 43;
    rows = 23;
  }

  const char** prog_argv = 0;

  // parse program arguments
  // terminal [-t] [-s size] [prog]

  for(int i=1; i<argc; i++)
  {
    if (string(argv[i]) == "-t")
    { trace = true;
      continue;
     }

    if (string(argv[i]) == "-s")
    { int p = -1;
      i++;
      if (i < argc)
      { string size = argv[i];
        p = size.index("x");
        if (p != -1)
        { rows = atoi(size.substring(0,p));
          cols = atoi(size.substring(p+1));
         }
       }
       if (p == -1) usage();
       continue;
     }

     prog_argv = (const char**)(argv+i);
   }

  cout << "rows = " << rows << endl;
  cout << "cols = " << cols << endl;
  cout << endl;


//string title("LEDA TERMINAL  %d x %d",rows,cols);

  string title = "LEDA TERMINAL";

  if (prog_argv) title = prog_argv[0];

  if (!title.starts_with("/"))
  { int p = title.last_index("/");
    if (p != -1) title = title.substring(p+1);
  }


  terminal T(title,rows,cols,fnt_sz);
  T.set_trace(trace);
  T.display();

  T.get_window().keyboard(1);

  if (prog_argv == 0)
  { string addr = login_dialog(T);
    if (addr.ends_with("localhost") || addr.index("@") <= 0) exit(0); 
    prog_argv = shell_ssh_argv;
    prog_argv[11] = addr.cstring();
    prog_argv[12] = NULL;
  }

  for(int i=0; i<16; i++) {
    if (prog_argv[i] == NULL) break;
    cout << string("execv: arg[%d] = ",i) << prog_argv[i] << endl;
   }
  cout << endl;

  string path = prog_argv[0];

  int fdm = 0;
  child_pid = start_shell(fdm,prog_argv);

  T.connect(fdm);

  if (!path.starts_with("/"))  T.prompt("Press any key to exit");

  return 0;
}

#endif
