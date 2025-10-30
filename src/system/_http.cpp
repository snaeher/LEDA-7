/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _http.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/http.h>

#include <assert.h>
#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

static bool extract(string s, string label, string& x)
{ int p = s.index(label);
  if (p == -1) return false;
  p += label.length();
  int q = s.index("\r\n",p);
  if (q == -1) return false;
  x = s(p,q-1).trim();
  return true;
}

static bool extract(string s, string label, int& x)
{ string txt;
  if (!extract(s,label,txt)) return false;
  x = atoi(txt);
  return true;
}


http::http() 
{ time_out = 10;
  first = buf;
  last = buf-1;
  delimiter = '\n';

  lsock = new leda_socket;
  sock = lsock;
  port = 80;

  tsock = 0;
}

http::~http() { 
  delete lsock;
}

int http::get(ostream& out, void (*progress_f)(int,int))
{
  int count = int(last-first)+1;
  if (count > 0)  out.write(buf,count);

  if (count != current_length) {
   cerr << "count = " << count << endl;
   cerr << "current_length = " << current_length << endl;
  }
  assert(count == current_length);

  if (progress_f) progress_f(count,content_length);

  while (count < content_length)
  { //size_t n = sock->receive(buf,buf_len-1,time_out);
    size_t n = sock->receive(buf,buf_len-1);
    if (n == 0) break;
    out.write(buf,n);
    count += (int)n;
    if (progress_f) progress_f(count,content_length);
   }
  return count;
}

int http::get_file(string fname, void (*progress_f)(int,int))
{ ofstream out(fname,ios::binary);
  if (!out.good())
  { error_msg = "Cannot write to file: " + fname;
    return -1;
   }
  return get(out,progress_f);
}

int http::append_to_file(string fname, void (*progress_f)(int,int))
{ ofstream out(fname,ios::binary | ios::app);
  if (!out.good())
  { error_msg = "Cannot write to file: " + fname;
    return -1;
   }
  return get(out,progress_f);
}

int http::get(char* buffer)
{
  char* q = buffer;
  int count = int(last-first)+1;

  for(int i=0; i<count; i++) *q++ = buf[i];

  while (count < content_length)
  { //int n = sock->receive(buf,buf_len-1,time_out);
    size_t n = sock->receive(buf,buf_len-1);
    if (n == 0) break;
    for(size_t i=0; i<n; i++) *q++ = buf[i];
    count += (int)n;
   }
  return count;
}


/*
int http::get(ostream& out, int length, void (*progress_f)(int,int))
{  
  int count = last-first+1;
  if (count > 0) out.write(buf,count);

  for(;;)
  { //int n = sock->receive(buf,buf_len-1,time_out);
    int n = sock->receive(buf,buf_len-1);
    if (n == 0) break;
    out.write(buf,n);
    count += n;
    progress_f(count,length);
   }
  return count;
}
*/



bool http::get(string& str)
{ str = "";
  // try to fill buffer if empty
  if (first > last && current_length < content_length) 
  { first = buf;
    //int len = sock->receive(buf,buf_len-1,time_out);
    size_t len = sock->receive(buf,buf_len-1);
    last = buf + len - 1;
    current_length += (int)len;
   }

  if (first > last) return false; 

  bool end_of_string = false;

  while (!end_of_string) 
  { char* p = first;
    while (p <= last && !is_delimiter(*p)) p++; 
    if (p <= last) end_of_string = true;
    *p = '\0';
    str += string(first);
    first = p+1;
    if (!end_of_string && first > last) 
    { if (current_length == content_length) 
        end_of_string = true;
      else
      { first = buf;
        //int len = sock->receive(buf,buf_len-1,time_out);
        size_t len = sock->receive(buf,buf_len-1);
        last = buf + len - 1;
        current_length += (int)len;
        if (len == 0) end_of_string = true;
      }
     }
  }
  return true;
}


void http::send_string(string s)
{ //cout << "http send: " << s << endl;
  sock->send(s);
 }


bool http::connect(string url) 
{
  header_lines.clear();

  first = buf;
  last = buf-1;

  sock = lsock;
  port = 80;


  string prefix = "";
  
  if (url.starts_with("https://")) {
    if (tsock == 0) LEDA_EXCEPTION(1,"http: cannot handle https address");
    prefix = "https";
    url = url.replace("https://","");
    sock = tsock;
    port = 443;
  }
  else
  if (url.starts_with("http://")) {
    prefix = "http";
    url = url.replace("http://","");
    sock = lsock;
    port = 80;
   }

  int p = url.index("/");

  if (p == -1) 
  { p = url.length();
    url += "/";
   }

  host = url(0,p-1);

  path = url(p,url.length()-1); 

  p = host.index(":");

  if (p >= 0)
  { port = atoi(host(p+1,host.length()-1));
    host = host(0,p-1);
    if (port == 443) 
    { if (tsock == 0) LEDA_EXCEPTION(1,"http: cannot handle https address");
      sock = tsock;
     }
   }


/*
cout << "host = " << host << endl;
cout << "path = " << pathf << endl;
*/

  sock->set_host(host);
  sock->set_port(port);

  if (!sock->connect(time_out))
  { error_msg = sock->get_error();
    return false;
   }

  string user_agent = "curl/7.79.1";

/*
  string user_agent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36";
*/

/*
 string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
*/

/*
 string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36";
*/

  string header;
/*
  header += "GET " + path + " HTTP/1.1";
  header += "\r\n";
*/
  header += "GET " + path + " HTTP/1.0";
  header += "\r\n";
  header += "User-Agent: " + user_agent;
  header += "\r\n";
  header += "Host: " + host;
  header += "\r\n";
  header += "Connection: Keep-Alive";
  header += "\r\n";
  header += "Accept: */*";
  header += "\r\n";

  string s;
  forall(s,user_headers) header += s + "\r\n";

  header += "\r\n";

/*
  cout << endl;
  cout << header<< endl;
*/

  send_string(header);



  // receive  header

  content_length = MAXINT;
  content_type = "unknown";

  int header_length = 0;
  int content_begin = 0;
  int buffer_len = 0;

  while (header_length == 0)
  { 
    buffer_len = (int)sock->receive(buf,buf_len-1,time_out);

    if (buffer_len == 0) break;

    buf[buffer_len] = 0;
    s += buf;
    int p = s.index("\r\n\r\n");
    if (p > 0) 
    { header_length = p+4;
      content_begin = header_length;
     }
    else
     content_begin -= buffer_len;
   }

  if (header_length == 0) 
  { error_msg = "no http header (timeout)";
    return false;
   }

  assert(content_begin >= 0);

  extract(s,"Content-Length:",content_length);
  extract(s,"Content-Type:",content_type);
  extract(s,"Transfer-Encoding:",transfer_encoding);


  // split header into lines

  p = 0;
  while (p <s.length())
  { int q = s.index("\r\n",p);
    if (q == -1) break;
    header_lines.append(s(p,q-1).trim());
    p = q+2;
  } 

  string response = header_lines.head();

  if (response.index("302 Found") != -1 || 
      response.index("301 Moved") != -1)
  { // redirection;
    string location;
    string s;
    forall(s,header_lines) {
      if (s.index("Location:") == 0)
      { location = s.replace("Location:", "").trim();
        break;
       }
    }
    return connect(location);
   }


  if (response.tail(6) != "200 OK")
  { error_msg = "NOT OK";
    string s;
    forall(s,header_lines) error_msg += s + "\n";
    return false;
   }

  first = buf;
  last = buf-1;
  for(int i=content_begin; i < buffer_len; i++) *++last = buf[i];
  current_length = int(last-first)+1;

  return true;
}


bool http::is_delimiter(char c) const
{ if (delimiter == 1000) 
     return isspace(c) != 0;
  else
     return int(c) == delimiter; 
}


int http::get(char* p, int sz)
{ int len = 0;
  while (first <= last && len++ < sz) *p++ = *first++;
  *p = 0;
  if (len == 0 && current_length < content_length)
  { //len = sock->receive(p,sz,time_out);
    len = (int)sock->receive(p,sz);
    p[len] = 0;
    current_length += len;
   }
  return len;
}


bool http::get_line(string& line) 
{ delimiter = '\n';
  return get(line); 
}


bool http::get_string(string& s) 
{ delimiter = 1000;
  while (get(s) && s == "");
  return s != "";
}


bool http::get_int(int& x)
{ string s;
  if (get_string(s))
  { x = atoi(s);
    return true;
   }
  else 
   { x = 0;
     return false;
    }
}


bool http::get_float(double& x)
{ string s;
  if (get_string(s))
  { x = atof(s);
    return true;
   }
  else 
   { x = 0;
     return false;
    }
}



/*
int wget(string url, list<string>& header, ostream& out, int sec)
{ http ht;
  ht.set_timeout(sec);

  if (!ht.connect(url)) 
  { string err_msg = ht.get_error();
    header.append(err_msg);
    return 0;
   }

  header = ht.get_headers();
  return ht.get(out);
}


int wget(string url, list<string>& header, string fname,int timeout)
{ ofstream out(fname,ios::binary);
  return wget(url,header,out,timeout);
}

int wget(string url, ostream& out, int timeout)
{ list<string> header;
  return wget(url,header,out,timeout); 
}

int wget(string url, string fname,int timeout)
{ list<string> header;
  return wget(url,header,fname,timeout);
}
*/



LEDA_END_NAMESPACE
