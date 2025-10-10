/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  http.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_HTTP_H
#define LEDA_HTTP_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 721173
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/system/socket.h>

LEDA_BEGIN_NAMESPACE


class __exportC http  {

  static const int buf_len = 2048;

  string host;
  string path;
  int port;
  int time_out;


  string error_msg;

  char buf[buf_len+1]; // buf[buf_len] sentinel
  char* first;
  char* last;

  int delimiter;

  int    content_length;
  string content_type;
  string transfer_encoding;

  int current_length;

  list<string> header_lines;
  list<string> user_headers;

  socket* sock;

  bool get(string& str);
  bool is_delimiter(char c) const;

  void send_string(string s);


protected:
  socket* lsock; // non-tls socket
  socket* tsock; // tls-socket

public:

 http();
~http();


bool connect(string url);
void disconnect() { sock->disconnect(); }

void set_timeout(int t) { time_out = t; }

int    get_content_length() const { return content_length; }
string get_content_type() const { return content_type; }

void set_header(string s) { user_headers.append(s); }

list<string> get_headers()  const { return header_lines; }

int          get_timeout() const { return time_out; }
string       get_error()   const { return error_msg; }


int get(char* p, int sz);

int get(char* buffer);

int get(ostream& out, void (*progress_f)(int,int)=0);

int get_file(string fname, void (*progress_f)(int,int)=0);
int append_to_file(string fname, void (*progress_f)(int,int)=0);

bool get_line(string& line); 
bool get_string(string& s);
bool get_int(int& x);
bool get_float(double& x);

};


/*
extern __exportF int wget(string url, list<string>& header, 
                                      ostream& out,
                                      int timeout=10);

extern __exportF int wget(string url, list<string>& header, 
                                      string fname,
                                      int timeout=10);

extern __exportF int wget(string url, ostream& out, int timeout=10);
extern __exportF int wget(string url, string fname, int timeout=10);
*/



LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 721173
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


#endif
