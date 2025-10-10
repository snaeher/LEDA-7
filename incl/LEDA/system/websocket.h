/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  web_socket.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_WEBSOCKET_H
#define LEDA_WEBSOCKET_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 669999
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/string.h>
#include <LEDA/system/socket.h>

LEDA_BEGIN_NAMESPACE

class __exportC websocket  {

  // pointer to socket (interface)
  socket* sock; 
  socket* sock_local; // allocated localy by websocket

  static const int buf_sz = 1024;

  char buffer[buf_sz];
  int buf_bytes;
  int buf_p;

  string host;
  int    port;
  bool   msg_deflate;
  bool   trace;

  string error_msg;

  bool next_byte(unsigned char& byte);

public:

  size_t read_frame(int& opcode, bool& fin, unsigned char* buf, size_t sz);
  void   send_frame(unsigned char type, unsigned char* buf, size_t sz);

  websocket(socket* sock_ptr);
 ~websocket();

  void set_socket(socket* sock_ptr) { sock = sock_ptr; }

  void set_host(string _host) { host = _host; }
  void set_port(int _port) { port = _port; }

  string get_host() { return host; }
  int    get_port() { return port; }


// client
  bool connect();
  void handshake(string path = "");

// server
  bool listen();
  bool accept();
  bool server_handshake();


  void set_trace(bool b) { trace = b; }

  bool connected(){ return sock->connected(); }

  string get_error() { return sock->get_error(); }

  socket& get_socket() { return *sock; }

  int sock_fd() { return sock->sock_fd(); }

  void set_fd(int fd) { sock->set_fd(fd); }

  bool poll(int msec) { return sock->poll(msec); }

  // sending and receiveing frames

  void send_text(string text);
  void send_data(unsigned char* data, size_t len);
  void send_file(string fname);

  void ping(string text);
  void pong(string text);

  string receive_text();
  string receive_text(int timeout); 

  void disconnect();
  void detach();

/*
  void show_certificates(ostream& out) { sock->show_certificates(out); }
*/

};

LEDA_END_NAMESPACE

#endif


