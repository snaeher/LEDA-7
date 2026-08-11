#include <LEDA/system/websocket.h>
#include <LEDA/system/file.h>
#include <LEDA/core/list.h>

#include <assert.h>



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


int exec_program(string prog, const list<string>& L_arg, 
                              const list<string>& L_env)
{
  string s;
  string path;

  if (prog.starts_with("/")) 
    path = prog;
  else
    path = "./" + prog;

  string cwd = get_directory();

  // child

  log_child("exec " + path);
  log_child("cwd: " + cwd);

  const char* argv[] = {0,0,0,0,0,0,0,0,0,0};
  assert(L_arg.length() < 10);

  argv[0] = prog;

  int i = 1;
  forall(s,L_arg) {
    log_child("arg: " + s);
    argv[i++] = s.cstring();
  }


  const char* envp[] = {0,0,0,0,0,0,0,0,0,0};
  assert(L_env.length() < 10);

  int j = 0;
  forall(s,L_env) {
    log_child("env: " + s);
    envp[j++] = s.cstring();
  }

  execve(path,(char* const*)argv,(char* const*)envp);

  // execve only returns in case of error
  log_child(string("EXECVE ERROR(%d): %s", errno, strerror(errno)));

  return 0;
}



void start_program(string prog, string arg, string ip, int fd, bool maximized)
{
  demo_log << time_string()      << " " 
           << ip.format("%-16s") << " " 
           << prog << " " << arg << " "
           << "maximized = " << maximized  << endl;

  string home = getenv("HOME");

  list<string> L_arg;
  if (arg != "") L_arg.append(arg);

  list<string> L_env;
  L_env.append("WS_SOCK_FD=" + string("%d",fd));
  L_env.append("WS_CLIENT=" + ip);
  L_env.append("HOME=" + home);
  L_env.append("LD_LIBRARY_PATH=.");
  L_env.append("TERM=xterm");
  L_env.append("LEDAROOT=/home/naeher/leda");

  if (maximized) {
    L_env.append("LEDA_OPEN_MAXIMIZED=1");
  }

  int status = exec_program(prog,L_arg,L_env);

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

  log(string("LISTEN: port = %d",port));

  if (!sock.listen())
  { log(string("Listen Error: ") + sock.get_error());
    return 1;
   }

  for (;;)  
  {
    //log("ACCEPT: waiting for connection ...");

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
          bool maximized = msg.starts_with("start_max");

          int p = msg.index(":");
          string prog = msg.substring(p+1).trim();

          if (msg.starts_with("start_terminal") || prog.starts_with("/"))
            start_program("window/terminal",prog,client_ip,sock_fd,maximized);
          else
            start_program(prog,"",client_ip,sock_fd,maximized);

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

