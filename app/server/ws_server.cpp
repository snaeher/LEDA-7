#include <LEDA/system/websocket.h>
#include <LEDA/system/file.h>
#include <LEDA/core/list.h>



#if defined(__win32__)

#include <process.h>

#define SIGCHLD 0
#define SIGUSR1 0
int fork() { return 0; }

#else

#include <unistd.h>
#include <sys/wait.h>

#endif

#include <signal.h>
#include <string.h>
#include <stdlib.h>

using namespace leda;

using std::cerr;
using std::endl;
using std::ofstream;

static ofstream ws_log("ws_server.log",ofstream::app);

static ofstream demo_log("demos.log",ofstream::app);

void log(string msg) { 
  if (msg == "") 
     ws_log << endl;
  else
     ws_log << string("PARENT(%d): ",getpid()) << msg << endl; 
}

void log_child(string msg) { 
  if (msg == "") 
     ws_log << endl;
  else
     ws_log << string("CHILD (%d): ",getpid()) << msg << endl; 
}




int exec_program(string prog, string arg, string env)
{
  string path = "./" + prog;

  string cwd = get_directory();

/*
  string cmd = "env " + env + " " + path;
  if (arg != "") cmd += " " + arg;
  int stat = system(cmd);
  if (stat == -1) {
      log_child(string("system: errno=%d %s", errno, strerror(errno)));
  }
  return stat;
*/


/*
  // do not fork again

  if (fork() != 0) // parent
  { int stat = 0;
    wait(&stat); // wait for the child to terminate
    if (WIFEXITED(stat)) stat = WEXITSTATUS(stat);
    return stat;
   }
*/

  // child

  log_child("exec " + path);
  log_child("arg: " + arg);
  log_child("cwd: " + get_directory());

  string A[16];
  int n = env.split(A,16);

  char** envp = new char*[n+1];
  for(int i=0; i<n; i++) {
    log_child("env: " + A[i]);
    envp[i] = A[i].cstring();
  }
  envp[n] = 0;

//execle(~path,~prog,NULL,(char* const*)envp);

  const char* argv[] = {0,0,0};
  argv[0] = prog;
  if (arg != "") argv[1] = arg;

  execve(~path,(char* const*)argv,(char* const*)envp);

  // execve only returns in case of error
  log_child(string("EXECVE ERROR(%d): %s", errno, strerror(errno)));

  delete[] envp;

  return 0;
}



void start_program(string prog, string arg, string ip, int fd, bool full)
{
  demo_log << time_string()      << " ";
  demo_log << ip.format("%-16s") << " ";
  demo_log << prog               << " ";

  if (arg != "") demo_log << arg << " ";
  if (full) demo_log << "F";

  demo_log << endl;

  string home = getenv("HOME");

  string env = "WS_SOCK_FD=" + string("%d",fd);
  env += " WS_CLIENT=" + ip;
  env += " HOME=" + home;
  env += " LD_LIBRARY_PATH=.";
  env += " TERM=xterm";
  env += " LEDAROOT=/home/naeher/leda";

  if (full) env += " LEDA_OPEN_MAXIMIZED=1";

  int status = exec_program(prog,arg,env);

  // never reached (exec_program does not return)
  log_child(string("EXIT STATUS = %d",status));

}
  


int main(int argc, char** argv)
{
  int port = 9681;
  if (argc > 1) port = atoi(argv[1]);

  ws_log << "---------------------------------------------------" << endl;
  ws_log << string("WS SERVER START (%d)  ",port) + time_string() << endl;
  ws_log << "---------------------------------------------------" << endl;

  signal(SIGUSR1,SIG_IGN); // SIGUSR1 only kills apps not server
  signal(SIGCHLD,SIG_IGN); // do not wait for child processes 


  leda_socket sock;
  sock.set_port(port);

  if (!sock.listen())
  { log(string("Listen Error: ") + sock.get_error());
    return 1;
   }


  for (;;)  
  {
    //log("Waiting for connection ...");

    if (!sock.accept())
    { log("Accept Error: " + sock.get_error());
      exit(1);
     }

    log("");
    log("CONNECTED " + time_string() + "  client: " + sock.client_ip());

    if (fork() == 0) 
    { // child process

      int sock_fd = sock.sock_fd();
      string client_ip = sock.client_ip();

      log_child(string("forked (sock_fd = %d)",sock_fd));

      websocket ws(&sock);

      log_child("ws.server_handshake");
      ws.server_handshake();

      while (ws.connected())
      { 
        string msg = ws.receive_text();
        log_child("msg = " + msg);
  /*
        if (msg == "") continue;
        if (msg == "ok") continue;
        if (msg == "keep_alive") continue;
        if (msg.starts_with("event")) continue;
  */
        if (msg.starts_with("ip:")) { 
           client_ip = msg.substring(3);
           continue;
        }

        if (msg.starts_with("start")) 
        { 
          bool full = msg.starts_with("start_full");

          int p = msg.index(":");
          string prog = msg.substring(p+1).trim();

          string arg = "";
          if (msg.starts_with("start_terminal"))
          { arg = prog;
            prog = "window/terminal";
           }

          start_program(prog,arg,client_ip,sock_fd,full);

          break;
         }
      }

      log_child("DISCONNECTED");
      log_child("");

      return 0;

    } // fork

    sock.disconnect();
  }

  log("WS SERVER EXIT");
}

