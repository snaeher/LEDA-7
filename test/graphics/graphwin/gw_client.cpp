/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_client.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/socket.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;


static leda_socket sock;

static int format = 0;
static const char* format_str[] = { "gw", "gml"};

static void run_agd(GraphWin& gw)
{ 
  window*    cw = window::get_call_window();
  panel_item ci = window::get_call_item();
  string cmd = cw->get_label(ci);

  window& W = gw.get_window();
  W.set_cursor(XC_watch);


  if (!sock.connect())
  { error_handler(1,sock.get_error());
    return;
   }

  sock.send_string("gw_client");

  sock.wait("ok");
  sock.send_string("layout");

  sock.wait("ok");
  sock.send_string(cmd);

  sock.wait("ok");
  sock.send_string("options");
  sock.wait("ok");
  sock.ostr() << "option1  value1" << endl;
  sock.ostr() << "option2  value2" << endl;
  sock.ostr() << "option3  value3" << endl;
  sock.send();

  sock.wait("ok");

  switch (format) {
   case 0: sock.send_string("gw");
           sock.wait("ok");
           gw.save_gw(sock.ostr());
           break;
   case 1: sock.send_string("gml");
           sock.wait("ok");
           gw.save_gml(sock.ostr());
           break;
  }
  sock.send();


 string msg;
 sock.receive_string(msg);

 if (msg == "ok") 
 { sock.send_string("result");
   sock.receive();
   switch (format) {
    case 0: gw.load_layout(sock.istr());
            break;

    case 1: gw.read_gml(sock.istr());
            gw.zoom_graph();
            break;
   }
 }

 W.set_cursor();

 if (msg == "error") 
 { sock.send_string("result");
   sock.receive_string(msg);
   gw.acknowledge("AGD Server", msg);
  }
}


static void agd_options(GraphWin& gw)
{ panel P;
  P.text_item("\\bf\\blue AGD File Formats");
  P.text_item("");
  P.choice_item("input",format,"gw","gml");
  P.button("continue");
  P.open(gw.get_window());
 }


int main(int argc, char *argv[])
{
  string host;

  if (argc >= 2) 
      host = argv[1];
  else
   if (!get_environment("AGDServer",host)) host = "localhost";


  sock.set_host(host);
  sock.set_port(9660);

  if (!sock.connect())
  { error_handler(1,sock.get_error());
    return 1;
   }

  sock.send_string("gw_client");
  sock.wait("ok");

  sock.send_string("list");
  sock.wait("ok");

  sock.send_string("result");
  sock.receive();

  list<string> alg_list;
  string s;
  while (sock.istr())
  { s.read_line(sock.istr());
    if (s != "") alg_list.append(s);
   }

  GraphWin gw("AGD Server (" + sock.get_ip() + ")");

  int agd_menu = gw.add_menu("agd");
  forall(s,alg_list)
    gw.add_simple_call(run_agd,s,agd_menu);

  int opt_menu = gw.add_simple_call(agd_options,"options");

  if (argc >=3) gw.read(argv[2]);

  gw.display(window::center,window::center);
  gw.edit();

  sock.disconnect();

  return 0;
}
