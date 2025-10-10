/*******************************************************************************
+
+  LEDA 7.0  
+
+
+  socket.h
+
+
+  Copyright (c) 1995-2023
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_SOCKET_H
#define LEDA_SOCKET_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 700170
#include <LEDA/internal/PREAMBLE.h>
#endif

/*{\Manpage {leda_socket} {} {Sockets} {S}}*/


#include <LEDA/system/basic.h>
#include <LEDA/system/stream.h>

LEDA_BEGIN_NAMESPACE


class socket {

// basic socket interface

public:

virtual ~socket() {}

virtual void set_host(string) = 0;
virtual void set_port(int) = 0;

virtual string get_host() const = 0;
virtual int get_port() const = 0;

virtual bool listen() = 0;
virtual bool accept() = 0;

virtual bool connect() = 0;
virtual bool connect(int sec) = 0;
virtual void disconnect() = 0;
virtual void detach() = 0;
virtual bool connected() = 0;

virtual size_t receive(char* buf, size_t sz) = 0;
virtual size_t receive(char* buf, size_t sz, int ) = 0;

virtual void send(char* buf, size_t sz) = 0;
virtual void send(string s) = 0;

virtual bool poll(int msec) = 0;

virtual string get_error() const = 0;

virtual int sock_fd() const = 0;
virtual void set_fd(int fd) = 0;

virtual string client_ip() const = 0;

};




class __exportC leda_socket : public socket {

/*{\Mdefinition
A data {\bf packet} consists of a sequence of bytes (in {\tt C} of type
{\tt unsigned char}) $c_0,c_1,c_2,c_3,x_1,\ldots,x_n$. The first four 
bytes encode the number $n$ of the following bytes such that
$n = c_0 + c_1\cdot 2^8 + c_2\cdot 2^{16} + c_3\cdot 2^{24}$.
The LEDA data type {\tt leda\_socket} offers, in addition to the
operations for establishing a socket connection, functions for
sending and receiving packets across such a connection. 
It is also possible to set a receive limit; if such a receive limit is set,
messages longer than the limit will be refused. If the limit is negative (default),
no messages will be refused. \\
In particular, the following operations are available:
}*/

static int leda_socket_init;

string hostname;
string ipaddr;
int    portnum;
int    sockfd0;
int    sockfd;
int    qlength;
int    timeout;

int    num_sz_bytes;

size_t   receive_limit;

string error_str;
string infile_name;
string outfile_name;

ofstream* out_stream;
ifstream* in_stream;

void (*wait_error_handler)(leda_socket&,string);

void (*receive_handler)(leda_socket&,size_t,size_t);
void (*send_handler)(leda_socket&,size_t,size_t);

size_t receive_size();

void send_size(size_t sz);
void error(string msg);


public:

int  receive_raw(char*,size_t);


/*{\Mcreation}*/

leda_socket(string host, int port);
/*{\Mcreate creates an instance |\Mvar| of type |\Mname| associated with host name |host| and port number |port|.}*/ 
 
leda_socket(string host); 
/*{\Mcreate creates an instance |\Mvar| of type |\Mname| associated with host name |host|.}*/
 
leda_socket(); 
/*{\Mcreate creates an instance |\Mvar| of type |\Mname|.}*/ 
 
 
~leda_socket();

/*{\Moperations}*/

void init();

bool connected() { return sockfd != -1; }

int get_fd() const { return sockfd; }

ofstream& ostr() { return *out_stream; }
ifstream& istr() { return *in_stream;  }

void set_host(string host) { hostname = host; }
/*{\Mop  sets the host name to |host|. }*/

void set_port(int port)         { portnum = port;  }
/*{\Mop  sets the port number to |port|. }*/

size_t get_limit() const { return receive_limit; }
/*{\Mop  returns the receive limit parameter.}*/

void set_limit(size_t limit)       { receive_limit = limit; }
/*{\Mop  sets the receive limit parameter to |limit|. If a negative limit is set,
the limit parameter will be ignored. }*/


void set_num_sz_bytes(int num) { num_sz_bytes = num; }
int  get_num_sz_bytes() { return num_sz_bytes; }

void set_qlength(int len) { qlength = len;   }
/*{\Mop  sets the queue length to |len|. }*/

void set_timeout(int sec) { timeout = sec;   }
/*{\Mop  sets the timeout interval to |sec| seconds. }*/

void set_error_handler(void (*f)(leda_socket&,string)) 
{ wait_error_handler = f; }
/*{\Mop  sets the error handler to function |f|. }*/

void set_receive_handler(void (*f)(leda_socket&,size_t,size_t)) 
{ receive_handler = f; }
/*{\Mop  sets the receive handler to function |f|. }*/

void set_send_handler(void (*f)(leda_socket&,size_t,size_t)) 
{ send_handler = f; }
/*{\Mop  sets the send handler to function |f|. }*/


string get_host()  const  { return hostname; }
/*{\Mop  returns the host name. }*/

int    get_port()  const  { return portnum;  }
/*{\Mop  returns the port number. }*/

int    get_timeout() const { return timeout;  }
/*{\Mop  returns the timeout interval length in seconds. }*/

int    get_qlength() const { return qlength;  }
/*{\Mop  returns the queue length. }*/


string get_error() const { return error_str; }


// client side

bool connect(int sec);
/*{\Mop  tries to establish a connection from a client to a server. If the 
         connection can be established within $sec$ seconds, the operation 
         returns |true| and  |false| otherwise.}*/

bool connect() { return connect(10); }
/*{\Mop  same as |\Mvar.connect(10)| }*/


// server side
bool listen();
/*{\Mop  creates a socket endpoint on the server, performs address binding and signals readiness of a
server to receive data.}*/

bool accept();
/*{\Mop  the server takes a request from the queue.}*/

void detach();
/*{\Mop  detach from endpoint port.}*/

void disconnect();
/*{\Mop  ends a connection.}*/

string client_ip()  const   { return ipaddr;    }
/*{\Mop  returns the client ip address. }*/

void send();

/*{\Mtext {\bf Sending and receiving packets}}*/


void send_file(string fname) { send_file(fname, 8192); }
/*{\Mop  sends the contents of file |fname|.}*/

void send_file(string fname, int buf_sz);
/*{\Mop  sends |fname| using a buffer of size |buf_sz|.}*/


void send_bytes(char* buf, size_t num);
/*{\Mop  sends |num| bytes starting at address |buf|.}*/

void send_string(string msg);
/*{\Mop  sends string |msg|.}*/

void send_int(int x);
/*{\Mop  sends (a text representation of) integer $x$.}*/


bool receive();

bool receive_file(string fname);
/*{\Mopl receives data and writes it to file |fname|.}*/

bool receive(ostream& out);

char* receive_bytes(size_t& num);
/*{\Mopl receives |num| bytes. The function allocates memory and returns the first address
of the allocated memory. |num| is used as the return parameter for the number of received bytes.}*/

int receive_bytes(char* buf, size_t buf_sz);
/*{\Mopl receives at most |buf_sz| bytes and writes them into the buffer |buf|.
         It returns the number of bytes supplied by the sender (maybe more 
		 than |buf_sz|), or -1 in case of an error.}*/

string receive_string();

bool receive_string(string& s);
/*{\Mopl receives string |s|.}*/

bool receive_int(int& x);
/*{\Mop  receives (a text representation of) an integer and stores its
         value in $x$.}*/


bool wait(string s);
/*{\Mopl returns |true|, if |s| is received, |false| otherwise.}*/


size_t receive(char* buf, size_t sz);
size_t receive(char* buf, size_t sz, int timeout);

void   send(char* buf, size_t sz);
void   send(string s);

string read_string();
string read_string(int sz, int timeout);

bool poll(int msec);

int sock_fd() const { return sockfd; }
void set_fd(int fd);

};


/*{\Mtext
\bigskip
The following template functions can be used to send/receive objects supporting 
input and output operators for iostreams.
}*/

// templates for sending/ receiving objects ...
template<class T>
void socket_send_object(const T& obj, leda_socket& sock)
/*{\Mfuncl
sends |obj| to the connection partner of |sock|. 
}*/ 
{
 string_ostream OS;
 OS << obj;
 OS << ends;
 sock.send_string(OS.str());
}


template<class T>
void socket_receive_object(T& obj, leda_socket& sock)
/*{\Mfuncl
receives |obj| from the connection partner of |sock|.
}*/ 
{
 string St;
 sock.receive_string(St);
 string_istream IS(St.cstring());
 IS >> obj;
}


#if LEDA_ROOT_INCL_ID == 700170
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


LEDA_END_NAMESPACE

#endif
