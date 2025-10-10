/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  time_server.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/socket.h>
#include <LEDA/system/file.h>

#include <time.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::cin;
using std::endl;



string get_current_time()
{ time_t clock; 
  time(&clock);
  tm* T = localtime(&clock); 
  return string("%2d:%02d:%02d", T->tm_hour,T->tm_min,T->tm_sec);
}


int main()
{
  leda_socket sock;

  sock.set_port(9666);

  if (!sock.listen())
  { cerr << sock.get_error() << endl; 
    return 1;
   }

  string fname = tmp_file_name();

  for (;;) {

    if (!sock.accept())
    { cerr << sock.get_error() << endl; 
      continue;
     }

/*
    string cmd;
    sock.receive_string(cmd);
    system(cmd + " > " + fname + " 2>&1");
    sock.send_file(fname);
    delete_file(fname);
*/
    system("xterm&");

    string time_str = get_current_time();
    sock.send_string(time_str);

    sock.disconnect();
  }

}

