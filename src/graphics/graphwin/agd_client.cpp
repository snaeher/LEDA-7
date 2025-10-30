/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  agd_client.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>   
#include <LEDA/system/socket.h>
#include <LEDA/system/file.h>   

#include <stdlib.h>

#define MAXOPT 32

LEDA_BEGIN_NAMESPACE

static string agd_dir = "";


static void agd_error(GraphWin& gw)
{ 
  panel P;
  P.text_item("");
  P.text_item("");
  P.text_item("\\bf Could not connect to AGD server on host");
  P.text_item("");
  P.text_item("\\tt\\blue " + gw.get_agd_host() + " \\rm\black\\c""");
  P.text_item("");
  P.text_item("Please check the value of the AGDHOST environment variable\
               or give a new host name in the Options --> AGD Server dialog.\
               More information on AGD is available on the AGD home page at\
               \\blue http://www.mpi-sb.mpg.de/AGD");
  P.text_item("");
  P.button("continue",0);
  P.button("AGD homepage",1);

  if (gw.open_panel(P) == 1) 
     open_url("http://www.mpi-sb.mpg.de/AGD");
}



bool GraphWin::socket_connect()
{ window& W = get_window();

  W.set_tmp_label("Trying to connect to " + sock.get_host());
  if (!sock.connect(5))
  { LEDA_EXCEPTION(0,"Could not connect to AGD-Server");
    return false;
   }
  sock.send_string(GraphWin::version_str());
  sock.wait("ok");
  W.set_tmp_label("Connected to " + sock.client_ip());
  return true;
}


void GraphWin::socket_disconnect()
{ sock.disconnect();
  get_window().reset_frame_label();
 }


void gw_about_agd_alg(GraphWin& gw, string alg)
{ 
//window& W = gw.get_window();

  if (!gw.socket_connect()) return;

  leda_socket& sock = gw.sock;

  istream& in = sock.istr();
  

  sock.send_string("info");
  sock.wait("ok");
  sock.send_string(alg);
  sock.wait("ok");
  sock.send_string("result");
  sock.receive();

  gw.socket_disconnect();

  panel P;
  P.text_item("");
  P.text_item("\\bf\\blue " + alg);
  P.text_item("");
  while (in)
  { string str;
    str.read_line(in);
    P.text_item(str);
    P.text_item("");
   }

  P.fbutton("ok");
  gw.open_panel(P);
}



static bool option_diag(GraphWin& gw, string alg, istream& sock_in, 
                                                  string opt_fname)
{
  window& W = gw.get_window();

  string name[MAXOPT];
  string type[MAXOPT];
  string value[MAXOPT];
  string value_def[MAXOPT];

  bool   b_opt[MAXOPT];
  int    i_opt[MAXOPT];
  double f_opt[MAXOPT];
  string s_opt[MAXOPT];

  list<string>  enum_list[MAXOPT];

  int n = 0;
  int i = 0;

  while (sock_in)
  { sock_in >> name[n] >> type[n];
    if (name[n] == "") break;
    if (type[n] == "enum")
    { int sz; 
      sock_in >> sz;
      while (sz--)
      { string s;
        sock_in >> s;
        enum_list[n].append(s);
       }
     }

    sock_in >> value[n];
    value_def[n] = value[n];
    n++;
   }


  ifstream opt_in(opt_fname);
  string nstr,vstr;
  while (opt_in >> nstr >> vstr)
  { for(int j=0; j<n; j++)
       if (nstr == name[j]) value[j] = vstr;
   }
  opt_in.close();


  panel P;
  P.text_item("");
  P.text_item("\\bf\\blue " + alg);
  P.text_item("");


  ofstream opt_out(opt_fname);

  for(i=0; i<n; i++)
  {
    if (type[i] == "bool")
    { b_opt[i] = (value[i] == "true" || value[i] == "1");
      P.bool_item(name[i],b_opt[i]);
     }

    if (type[i] == "int")
    { i_opt[i] = atoi(value[i]);
      P.int_item(name[i],i_opt[i]);
     }

    if (type[i] == "float")
    { f_opt[i] = atof(value[i]);
      P.double_item(name[i],f_opt[i]);
     }

    if (type[i] == "enum")
    { s_opt[i] = value[i];
      P.string_item(name[i],s_opt[i],enum_list[i],5);
     }
   }

  P.buttons_per_line(4);

  P.fbutton("run",3);
  P.button("reset",2);
  P.button("about",1);
  P.button("cancel",0);

  W.disable_panel();

  P.display(W,window::center,window::center);

  int but = P.read_mouse();

  while (but == 1 || but == 2) 
  { if (but == 1)
      gw_about_agd_alg(gw,alg);
    else
     { for(i=0; i<n; i++)
       { if (type[i] == "bool")
            b_opt[i] = (value_def[i] == "true" || value_def[i] == "1");
           
         if (type[i] == "int")
            i_opt[i] = atoi(value_def[i]);
           
         if (type[i] == "float")
            f_opt[i] = atof(value_def[i]);
           
         if (type[i] == "enum")
            s_opt[i] = value_def[i];
           
        }
       P.redraw_panel();
     }
    but = P.read_mouse();
   }

  P.close();

  W.enable_panel();

  gw.redraw();

  if (but == 0) return false;

  for(i=0; i<n; i++)
  { opt_out << name[i] <<  " ";
    if (type[i] == "bool")  opt_out << b_opt[i] << endl;
    if (type[i] == "int")   opt_out << i_opt[i] << endl;
    if (type[i] == "float") opt_out << f_opt[i] << endl;
    if (type[i] == "enum")  opt_out << s_opt[i] << endl;
   }
  opt_out.close();
  return true;
}


static GraphWin* gwp;

static void wait_error_handler(leda_socket& sock, string msg)
{ gwp->get_window().set_cursor();
  if (msg == "error") 
  { sock.send_string("result");
    sock.receive_string(msg);
    gwp->acknowledge("AGD Server", msg);
   }
}

void gw_run_agd_alg(GraphWin& gw)
{ 
  window*    cw = window::get_call_window();
  panel_item ci = window::get_call_item();
  string alg = cw->get_label(ci);

  window& W = gw.get_window();

  leda_socket& sock = gw.sock;

  string opt_fname = agd_dir + "/" + alg;

  //gw.agd_option_diag = false;

cout << "gw.agd_option_diag = " << gw.agd_option_diag << endl;

  if (gw.agd_option_diag)
  { if (!gw.socket_connect()) return;
  //ostream& out = sock.ostr();
    istream& in  = sock.istr();
    sock.send_string("options");
    sock.wait("ok");
    sock.send_string(alg);
    sock.wait("ok");
    sock.send_string("result");
    sock.receive();
    gw.socket_disconnect();
    if (!option_diag(gw,alg,in,opt_fname)) return;
  }

 
  W.set_cursor(XC_watch);


  if (!gw.socket_connect()) return;

  ostream& out = sock.ostr();
//istream& in  = sock.istr();

  sock.send_string("layout");

  sock.wait("ok");
  sock.send_string(alg);

  if (gw.agd_option_diag)
  { sock.wait("ok");
    sock.send_string("options");
    sock.wait("ok");
    sock.send_file(opt_fname);
   }

  sock.wait("ok");

  switch (gw.agd_input_format) {
   case 0: sock.send_string("gw");
           sock.wait("ok");
           gw.save_gw(out);
           break;
   case 1: sock.send_string("gml");
           sock.wait("ok");
           gw.save_gml(out);
           break;
  }


  sock.send();

  if (sock.wait("ok"))
  { sock.send_string("result");
    sock.receive();
  
    switch (gw.agd_input_format) {
     case 0: gw.load_layout(sock.istr());
             break;
     case 1: //gw.read_gml(sock.istr());
             //gw.zoom_graph();
             gw.load_gml_layout(sock.istr());
             break;
      }
   }
 
  W.set_cursor();

  gw.socket_disconnect();
}



bool GraphWin::run_agd(string alg)
{ 

  window& W = get_window();
 
  W.set_cursor(XC_watch);

  gwp = this;

  sock.set_error_handler(wait_error_handler);

  if (!socket_connect()) return false;

  bool result = true;

  ostream& out = sock.ostr();

  sock.send_string("layout");
  sock.wait("ok");
  sock.send_string(alg);
  sock.wait("ok");

  sock.send_string("gw");
  sock.wait("ok");
  save_gw(out);
  sock.send();

  if (sock.wait("ok"))
  { sock.send_string("result");
    sock.receive();
    load_layout(sock.istr());
   }
  else
   result = false;
 
  W.set_cursor();

  socket_disconnect();

  return result;
}






void GraphWin::init_agd_menu(int menu_id, bool silent)
{
  if (menu_id < 0) return;

  if (agd_host == "") return;

/*
#if defined(__win64__)
  // problems with 64 bit windows ?
  add_simple_call(agd_error,"AGD Server",menu_id);
  return;
#endif
*/

  win_p->set_status_string(" Trying to connect to AGD server " + agd_host);

  string home_dir = get_home_directory();

  agd_dir = home_dir + "/.agdserver";

  if (!is_directory(agd_dir)) create_directory(agd_dir);

  sock.set_host(agd_host);
  sock.set_port(agd_port);

  gwp = this;

  bool connected = sock.connect(4);

  if (connected)
  { //leda_wait(1.0);
    leda_wait(0.5);
    sock.send_string(GraphWin::version_str());
    if (!sock.wait("ok")) connected = false;
   }

  update_status_line();

  if (!connected)
  { add_simple_call(agd_error,"AGD Server",menu_id);
    if (!silent) agd_error(*this);
    return;
   }

  sock.set_error_handler(wait_error_handler);

  sock.send_string("version");
  string version;
  sock.receive_string(version);

  if (!silent)
  { 
/*
    panel P;
    P.text_item("");
    P.text_item("\\bf Connected to AGD Server " + version);
    P.text_item("");
    P.text_item("on host\\blue\\tt ~" + agd_host + "\\black .");
    P.text_item("");
    P.display(get_window(),window::center,window::center);
    leda_wait(3.0);
    P.close();
*/
    message("Connected to AGD Server\\tt\\blue ~" + version 
             + " \\rm\\black on ~\\tt\\blue " + agd_host + "\\black .");
    leda_wait(3.0);
    message("");
   }

  if (!socket_connect()) return;

  sock.send_string("list");
  sock.wait("ok");

  sock.send_string("result");
  sock.receive();


  list<string> alg_list;
  while (sock.istr())
  { string s;
    s.read_line(sock.istr());
    if (s != "") alg_list.append(s);
   }

  add_separator(menu_id);
  int agd_menu = add_menu("AGD Server",menu_id);

  string s;
  forall(s,alg_list) add_simple_call(gw_run_agd_alg,s,agd_menu);

  socket_disconnect();
}


LEDA_END_NAMESPACE
