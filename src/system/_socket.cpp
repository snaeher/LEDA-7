/*******************************************************************************
+
+  LEDA 7.0  
+
+
+  _socket.cpp
+
+
+  Copyright (c) 1995-2023
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/socket.h>
#include <LEDA/system/file.h>
#include <LEDA/system/assert.h>
#include <LEDA/core/string.h>

#include <string.h>
#include <stdlib.h>


#if defined(queue)
#undef queue
#endif


#define MAX_DATA_SIZE   (1<<20)

#if defined(__unix__)
#undef __win32__
#undef __win64__
#endif


#if defined(__win32__) || defined(__win64__)
#undef MAXINT
#include <winsock.h>
#include <fcntl.h>
#include <io.h>
#else

#if defined(_AIX)
#include <strings.h>
#endif

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>


#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#if !defined(__aCC__)
#include <sys/select.h>
#endif

#endif



LEDA_BEGIN_NAMESPACE

#if defined(_AIX)
typedef unsigned int SockLength;
#elif defined(__linux__) || (__GNUC__ > 2 && !defined(mips) && !defined(__alpha))
typedef socklen_t SockLength;
#else
typedef int SockLength;
#endif


int leda_socket::leda_socket_init = 0;

void leda_socket::send_size(size_t sz)
{ // send num_sz_bytes bytes (4 or 8)

  char sz_bytes[8];

  for(int i=0; i<8; i++) sz_bytes[i] = 0;

  int num_bytes = 4;

  int i = 0;
  while (sz != 0)
  { sz_bytes[i] = char(sz % 256);
    sz /= 256;
    i++;
   }

  if (i > 4 || (i == 4 && sz_bytes[3] == 0xff))
  { for(int j=7; j>=4; j--) sz_bytes[j] = sz_bytes[j-1];
    sz_bytes[3] = char(0xff);
    num_bytes = 8;
   }

/*
for(int i=0; i<num_bytes; i++) {
  cout << string("%d: 0x%2x",i,(unsigned char)sz_bytes[i]) << endl;
}
*/

  send(sz_bytes,num_bytes);
 }

/*
bool leda_socket::set_nodelay(int enable) {
 int x = setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,(char*)&enable,sizeof(int));
 return x == 0;
}
*/


int leda_socket::receive_raw(char* buf, size_t sz)
{
/*
  return receive(buf,sz,timeout);
*/
  timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(timeval));

  int fd = sockfd;

  timeval polltime;
  polltime.tv_sec  = timeout;
  polltime.tv_usec = 0;

  fd_set rdset,wrset,xset;
  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_ZERO(&xset);
  FD_SET(fd,&rdset);
  FD_SET(fd,&xset);

  int n = select(fd+1,&rdset,&wrset,&xset,&polltime);

  if (n <= 0)
  { if (n == -1) error("receive_raw failed");
    return 0;
   }

//return recv(sockfd,buf,sz,0);

  int total = 0;
  while (sz > 0)
  { int bytes = recv(sockfd,buf,(int)sz,0);
    if (bytes <= 0) break;
    total += bytes;
    buf += bytes; 
    sz -= bytes;
  }

  return total;
}


void leda_socket::send(char* buf, size_t sz) { 
//cout << "leda_socket::send: sz = " << sz << endl; 
 ::send(sockfd,buf,(int)sz,0); 
}

void leda_socket::send(string msg) { 
  send(msg.cstring(),msg.length()); 
}


size_t leda_socket::receive(char* buf, size_t sz) { 
  return recv(sockfd,buf,(int)sz,0); 
}

size_t leda_socket::receive(char* buf, size_t sz, int sec)
{
  int fd = sockfd;

  timeval polltime;
  polltime.tv_sec  = sec;
  polltime.tv_usec = 0;

  fd_set rdset,wrset,xset;
  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_ZERO(&xset);
  FD_SET(fd,&rdset);
  FD_SET(fd,&xset);

  if (select(fd+1,&rdset,&wrset,&xset,&polltime))
    return recv(sockfd,buf,(int)sz,0);
  else
    return 0;
}




string leda_socket::read_string()
{ char buf[1024];
  string result;
  for(;;)
  { int len = recv(sockfd,buf,1023,0);
    if (len == 0) break;
    buf[len] = 0;
    result += buf;
  }
  return result;
}

string leda_socket::read_string(int sz, int timeout)
{ char* buf = new char[sz];
  size_t len = receive(buf, 1024, timeout);
  buf[len] = 0;
  string s = buf;
  delete[] buf;
  return s;
}




char* leda_socket::receive_bytes(size_t& num)
{
  num = receive_size();
  
  if (num <= 0) return NULL;
  
  char* buf = new char[num];
  
  receive_raw(buf, num);
  
  return buf;
}
   

int leda_socket::receive_bytes(char* buf, size_t buf_sz)
{
  size_t sz = receive_size();
  if (sz > buf_sz) sz = buf_sz;
  if (sz > 0) 
  { size_t received = receive_raw(buf,sz);
    if (received != sz) 
    { error_str = string("receive expected %d bytes, but got %d bytes", 
                                                     (int)sz, received);
      return -1;
     }
  }
  return (int)sz;
}


size_t leda_socket::receive_size()
{ 
  // assign corresponding error message to error_str
  // and return -1 if something goes wrong

  char sz_bytes[8];
  int num_bytes = 4;
 
  //int b = receive_raw(sz_bytes,num_sz_bytes);

  int b = receive_raw(sz_bytes,4);

  if (b  && (unsigned char)sz_bytes[3] == 0xff) {
    for(int i=1; i<4; i++) sz_bytes[i-1] = sz_bytes[i];
    b = receive_raw(sz_bytes+3,4);
    sz_bytes[7] = 0;
    num_bytes = 7;
  }

/*
for(int i=0; i<num_bytes; i++) {
  cout << string("%d: 0x%2x",i,(unsigned char)sz_bytes[i]) << endl;
}
*/

  if (b == 0)
  { error_str = "timeout in receive_size";
    return -1;
   }

  size_t sz = 0;
  for(int i=num_bytes-1; i>=0; i--) {
    sz = 256*sz + (unsigned char)sz_bytes[i];
  }

  if (sz == 0)
  { error_str = "zero length in receive_size";
    return 0;
   }
   
  if (receive_limit > 0  && sz > receive_limit)
  { error_str = "message size larger than limit in receive_size";
    return 0;
   }

/*
  if (sz > MAX_DATA_SIZE)
  { error_str = "message size larger than MAX_DATA_SIZE";
    return 0;
   }
*/

  return sz;
}



void leda_socket::error(string msg)
{
#if defined(__win32__) || defined(__win64__)
  error_str = msg + string(": %d", WSAGetLastError());
#else
  error_str = msg + ": " + strerror(errno); 
#endif
}


void leda_socket::init()
{
  num_sz_bytes = 4;

/*
  num_sz_bytes = 8;
  num_sz_bytes = sizeof(size_t);
*/

  timeout = 10;

  qlength = 256;
  
  receive_limit = (size_t)-1;

  wait_error_handler = 0;
  receive_handler = 0;
  send_handler = 0;

  sockfd0 = -1;
  sockfd = -1;

  // create two temporary files

  infile_name = tmp_file_name(); 
  outfile_name = tmp_file_name(); 

  in_stream = new ifstream(infile_name);
  out_stream = new ofstream(outfile_name);


#if defined(__win32__) || defined(__win64__)
  if (leda_socket_init == 0)
  {
    WORD    wsa_vers = 0x0101;
    WSADATA wsa_data;
   
    if (WSAStartup(wsa_vers, &wsa_data) != 0)
      LEDA_EXCEPTION(1,"No WINSOCK.DLL found");
    else
      { assert(LOBYTE(wsa_data.wVersion) == 1);
        assert(HIBYTE(wsa_data.wVersion) == 1);
       }
   }
  leda_socket_init++;
#endif
}

leda_socket::leda_socket(string host, int port) : hostname(host), 
                                                  portnum(port) 
{ init(); }


leda_socket::leda_socket(string host) : hostname(host), portnum(0)
{ init(); }


leda_socket::leda_socket() : hostname(""), portnum(0)
{ init(); }

leda_socket::~leda_socket()
{ 
#if defined(__win32__) || defined(__win64__)
  if (sockfd != -1) {
    closesocket(sockfd);
    shutdown(sockfd,2);
    if (--leda_socket_init == 0)  WSACleanup();
  }
#else
  if (sockfd != -1) {
    close(sockfd);
    shutdown(sockfd,SHUT_RDWR);
  }
#endif


  // delete temporary files

  delete in_stream;
  delete_file(infile_name);

  delete out_stream;
  delete_file(outfile_name);
}

void leda_socket::detach() { 
  // stop listening (close socket endpoint)
  if (sockfd0 != -1) close(sockfd0);
  sockfd0 = -1;
}


void leda_socket::disconnect()
{
#if defined(__win32__) || defined(__win64__)
  //shutdown(sockfd,2);
  closesocket(sockfd);
#else
  //shutdown(sockfd,SHUT_RDWR);
  close(sockfd);
#endif

  sockfd  = -1;
}


bool leda_socket::listen()
{
  // create endpoint
  sockfd0 = (int)::socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd0 < 0) {
    error("leda_socket::listen: Could not create socket endpoint.");
    return false;
  }

 int enable = 1;

 if (setsockopt(sockfd0,SOL_SOCKET,SO_REUSEADDR,(char*)&enable,sizeof(int)) < 0)
 { error(string("SO_REUSEADDR ") + strerror(errno));
   return false;
  }

#if defined(__linux__)
 if (setsockopt(sockfd0,SOL_SOCKET,SO_REUSEPORT,(char*)&enable,sizeof(int)) < 0)
 { error(string("SO_REUSEPORT ") + strerror(errno));
   return false;
  }
#endif

  // bind address
  sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons((u_short)portnum);

  if (bind(sockfd0, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    error("leda_socket::listen:  bind failed.");
    return false;
  }

  ::listen(sockfd0,qlength);

  return true;
}
 

bool leda_socket::accept()
{
  //cout << "leda_socket::accept" << endl;

  sockaddr_in client_addr;
  SockLength len = sizeof(client_addr);

  if (sockfd0 == -1) {
    error("accept error: endpoint sock_fd0 == -1");
    return false;
  }

  sockfd = (int)::accept(sockfd0, (sockaddr*)&client_addr, &len);

  if (sockfd == -1) {
    error("accept error: sockfd == -1");
    return false;
  }

  ipaddr = inet_ntoa(client_addr.sin_addr);

  return true;
}




bool leda_socket::connect(int sec)
{
 sockfd = (int)::socket(AF_INET, SOCK_STREAM, 0);

// set non-blocking 

#if defined(__win32__) || defined(__win64__)
  unsigned long arg = 1;
  ioctlsocket(sockfd,FIONBIO,&arg);
#else
  long arg = fcntl(sockfd, F_GETFL, NULL); 
  arg |= O_NONBLOCK; 
  fcntl(sockfd, F_SETFL, arg); 
#endif
 
// connect to server

  hostent* hp = gethostbyname(hostname);

  if (hp == NULL)
  { error("gethost failed");
    return false;
   }
 
  sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr,hp->h_addr,hp->h_length);
  serv_addr.sin_port = htons((u_short)portnum);
 
  if (::connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
  { timeval tv; 
    tv.tv_sec = sec; 
    tv.tv_usec = 0; 
    fd_set wrset; 
    FD_ZERO(&wrset); 
    FD_SET(sockfd, &wrset); 

    if (select(sockfd+1, NULL, &wrset, NULL, &tv)) 
     { SockLength lon = sizeof(int); 
       int valopt=0; 
       getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)(&valopt), &lon); 
       if (valopt) 
       { error_str = string("socket::connect failed (err = %d)",valopt); 
         return false;
        } 
     } 
    else 
     { error_str = "socket::connect timeout"; 
       return false;
     } 
  } 

// set to blocking mode again 

#if defined(__win32__) || defined(__win64__)
  arg = 0;
  ioctlsocket(sockfd,FIONBIO,&arg);
#else
  arg = fcntl(sockfd, F_GETFL, NULL); 
  arg &= (~O_NONBLOCK); 
  fcntl(sockfd, F_SETFL, arg); 
#endif

  ipaddr = inet_ntoa(serv_addr.sin_addr);
  return true;
}




void leda_socket::send_file(string fname, int buf_sz)
{ 
  char* buf = new char[buf_sz];

  size_t sz = size_of_file(fname);

#if defined(__win32__) || defined(__win64__)
  int fd = open(fname,O_RDONLY|O_BINARY);
#else
  int fd = open(fname,O_RDONLY);
#endif

  send_size(sz);

  size_t bytes = 0;

  int n;
  while ((n = read(fd,buf,buf_sz)) > 0) 
  { send(buf,n); 
    bytes += n;
    if (send_handler) send_handler(*this,bytes,sz);
   }

  close(fd);

  delete[] buf;
}
  


void leda_socket::send()
{ delete out_stream;
  send_file(outfile_name);
  out_stream = new ofstream(outfile_name);
}

void leda_socket::send_string(string msg)
{ size_t len = msg.length();
  send_size(len);
  send(msg.cstring(),len);
}

void leda_socket::send_int(int x) { send_string(string("%d",x)); }


void leda_socket::send_bytes(char* buf, size_t num)
{ send_size(num);
  send(buf,num);
}

bool leda_socket::receive(ostream& out) 
{ const int buf_sz = 8*1024;

  char buf[buf_sz];

  size_t sz = receive_size();

  if (sz == 0) return false;  // error in receive_size

  size_t sz0 = sz;

  while (sz > 0)
  { size_t bytes = buf_sz;
    if (bytes > sz) bytes = sz;

    int n = receive_raw(buf,bytes);

    if (n == 0) 
    { error_str = string("timeout sz = %d (%d bytes missing)",(int)sz0,(int)sz);
      break;
     }
    out.write(buf,n);
    sz -= n;
    if (receive_handler) receive_handler(*this,sz0-sz,sz0);
   }
  out.flush();

  return sz == 0;
}


bool leda_socket::receive_file(string fname) 
{ ofstream out(fname,ios::binary);
  bool b = receive(out);
  out.close();
  return b && is_file(fname);
}


bool leda_socket::receive() 
{ delete in_stream;

  ofstream out(infile_name);
  bool b = receive(out);
  out.close();

  in_stream = new ifstream(infile_name);
  if (in_stream->fail())
    LEDA_EXCEPTION(1,
          string("leda::socket: could not open in_stream ") + infile_name);

  return b;
}


bool leda_socket::receive_string(string& s)
{ 
  size_t sz = receive_size();

  if (sz < 1) {
    s = "";
    return false;
  }
  
  char* buf = new char[sz+1];
  char* p = buf;

  while (sz > 0)
  { int n = receive_raw(p,sz);
    if (n == 0) 
    { error_str = string("receive_string: timeout (%d bytes missing)",(int)sz);
      break;
     }
    p  += n;
    sz -= n;
   }

  *p = '\0';
  
  s = string(buf);

  delete[] buf;
  return sz == 0;
}

string leda_socket::receive_string()
{ 
  size_t sz = receive_size();

  if (sz < 1) return "";
  
  char* buf = new char[sz+1];
  char* p = buf;

  while (sz > 0)
  { int n = receive_raw(p,sz);
    if (n == 0) 
    { error_str = string("receive_string: timeout (%d bytes missing)",(int)sz);
      break;
     }
    p  += n;
    sz -= n;
   }

  *p = '\0';
  
  string s(buf);

  delete[] buf;
  return s;
}




bool leda_socket::receive_int(int& x)
{ string s;
  bool b = receive_string(s);
  if (b) x = atoi(s);
  return b;
}


bool leda_socket::wait(string s) 
{ 
  size_t sz = receive_size();

  char* buf = new char[sz+1];
  int n = receive_raw(buf,sz);
  buf[n] = '\0';
  
  string x = buf;

  if (x == s) return true;

  error_str = "received \"" + x + "\" expecting \""+ s + "\"";

  if (wait_error_handler) wait_error_handler(*this,x);

  return false;
}


bool leda_socket::poll(int msec)
{
  timeval polltime;
  polltime.tv_sec  = msec / 1000;
  polltime.tv_usec = 1000 * (msec % 1000);

  fd_set rdset,wrset,xset;
  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_ZERO(&xset);
  FD_SET(sockfd,&rdset);
  FD_SET(sockfd,&xset);

  return select(sockfd+1,&rdset,&wrset,&xset,&polltime) > 0;
}

void leda_socket::set_fd(int fd) {
  if (sockfd != -1) close(sockfd);
  sockfd = fd;
}


LEDA_END_NAMESPACE
