#include <LEDA/system/websocket.h>
#include <LEDA/system/file.h>
#include <LEDA/core/list.h>

#include <stdlib.h>
#include <signal.h>

#include <sys/wait.h>
#include <unistd.h>

#include <cstring>

using namespace leda;

using std::cerr;
using std::endl;
using std::ofstream;
using std::ostream;
using std::ios;

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


void sigchld_handler(int) 
{ int status;
  while (waitpid(-1,&status, WNOHANG) > 0);
}


int main(int argc, char** argv)
{
/*
  if (argc < 2)
  { cerr << endl;
    cerr << "usage: xserver port" << endl;
    return 1;
   }
*/
  
  int port = 9681;
  if (argc > 1) port = atoi(argv[1]);

  ws_log << "---------------------------------------------------" << endl;
  ws_log << string("WS SERVER START (%d)  ",port) + time_string() << endl;
  ws_log << "---------------------------------------------------" << endl;

  leda_socket sock;
  sock.set_port(port);

  if (!sock.listen())
  { log(string("Listen Error: ") + sock.get_error());
    return 1;
   }

  signal(SIGCHLD,sigchld_handler);

  for (;;)  
  {
    //log("Waiting for connection ...");

    if (!sock.accept())
    { log("Accept Failed: " + sock.get_error());
      exit(1);
     }

    string client_ip = sock.client_ip();

    log("");
    log("CONNECTED " + time_string() + "  client: " + client_ip);


    if (fork() == 0) 
    {
      // child process
  
      websocket ws(&sock);
  
      ws.server_handshake();
  
      int fd = ws.sock_fd();
      log_child("sock_fd = " + string("%d",fd));
  
      ws.detach();
  
      while(ws.connected())
      { 
        string msg = ws.receive_text();
  
        log_child("msg = " + msg);
  
        if (msg == "")
        { log_child("EMPTY MESSAGE");
          continue;
  /*
          int ppid = getppid();
          log_child(string("KILL ppid = %d",ppid));
          kill(ppid,9);
  */
         }
  
  /*
        if (msg.starts_with("event:")) continue;
        if (msg == "ok") continue;
        if (msg == "keep_alive") continue;
  */
  
        if (msg.starts_with("start")) 
        { string prog;
          int full = 0;
  
          if (msg.starts_with("start_full"))
          { full = 1;
            prog = msg.del("start_full: ");
           }
          else
          { full = 0;
            prog = msg.del("start: ");
          }
  
          demo_log << string("%20s %16s   %s", ~time_string(),
                                               ~client_ip,~prog) << endl;
          string cmd;
          if (prog.starts_with("terminal"))
            cmd = string("env WS_SOCK_FD=%d ./window/terminal %s",
                                                            fd,~prog);
          else
            if (full)
              cmd = string("env WS_SOCK_FD=%d LEDA_OPEN_MAXIMIZED=1 ./%s",
                                                               fd,~prog);
            else
              cmd = string("env WS_SOCK_FD=%d ./%s",fd,~prog);
  
          log_child("cmd = " + cmd);
          log_child("directory: " + get_directory());
  
          if (!create_process_wait(cmd)) {
            log_child("CREATE PROCESS FAILED");
          }
  
          break;
         }
      }

      log_child("DISCONNECTED");
      return 0;

    } // fork

    sock.disconnect();
  }

  log("SERVER EXIT");
}

