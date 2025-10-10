#include <LEDA/core/string.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <resolv.h>
#include <netdb.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

using std::ostream;
using std::cerr;
using std::endl;

LEDA_BEGIN_NAMESPACE

class tls_socket_impl 
{
  SSL_CTX* ctx;
  SSL* ssl;

  string hostname;
  string ipaddr;

  int portnum;
  int fd0;
  int sockfd;
  int qlength;

  string cert_file;
  string key_file;

  string error_msg;


bool init_ssl_context(const SSL_METHOD* method)
{
  SSL_library_init();
  OpenSSL_add_all_algorithms();  
  SSL_load_error_strings();   

  ctx = SSL_CTX_new(method);   


/*
  SSL_CTX_set_verify(ctx,mode,callback);
*/

  SSL_CTX_set_options(ctx,SSL_OP_ALL);
  SSL_CTX_set_options(ctx,SSL_OP_NO_TLSv1);
  SSL_CTX_set_options(ctx,SSL_OP_NO_SSLv2);

  if (ctx == NULL)
  { error_msg = string("SSL Context: ") + ERR_error_string(ERR_get_error(),0); 
    return false;
   }

  return true;
}


static int client_cert_cb(SSL* ssl, X509** x509, EVP_PKEY** pkey) {
cout << "CLIENT_CERT_CB" << endl;
return 0;
}


bool init_ssl_server_context()
{
#if OPENSSL_VERSION_NUMBER <= 0x009400000L
//#if OPENSSL_VERSION_NUMBER <= 0x101000000L
  const SSL_METHOD* method = TLSv1_1_server_method();
#else
  const SSL_METHOD* method = TLS_server_method();
#endif

  if (!init_ssl_context(method)) return false;

  // set private key and certificate

  if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
      error_msg = ERR_error_string(ERR_get_error(),0); 
      return false;
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0 ) {
      error_msg = ERR_error_string(ERR_get_error(),0); 
      return false;
  }

  SSL_CTX_set_client_cert_cb(ctx,client_cert_cb);

  return true;
}

bool init_ssl_client_context() {
#if OPENSSL_VERSION_NUMBER <= 0x009400000L
//#if OPENSSL_VERSION_NUMBER <= 0x101000000L
  const SSL_METHOD* method = TLSv1_1_client_method();
#else
  const SSL_METHOD* method = TLS_client_method();
#endif
  return init_ssl_context(method);
}


void release_ssl_context() {
   SSL_CTX_free(ctx); 
}


public:

void show_certificates(ostream& out)
{
  X509* cert = SSL_get_peer_certificate(ssl); 

  if ( cert == NULL )
  { out << "No certificates configured." << endl;
    return;
   }

  out << "Server certificates" << endl;

  char* line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
  out << "Subject: " << line << endl;;
  delete[] line;

  line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
  out << "Issuer: " << line << endl;;
  delete[] line;

  X509_free(cert);

  out << "Server Encryption: " <<  SSL_get_cipher(ssl) << endl;
}


tls_socket_impl(string host="", int port=0) {
  ssl = 0;
  fd0 = -1;
  sockfd = -1;
  hostname = host;
  portnum = port;
  qlength = 256;
}


~tls_socket_impl() {
   release_ssl_context();
   disconnect();
 }

void set_host(string host) { hostname = host; }
void set_port(int port) { portnum = port; }

string get_host() const { return hostname; }
int get_port() const { return portnum; }

void set_cert_file(string path) { cert_file = path; }
void set_key_file(string path) { key_file = path; }

bool listen()
{
  init_ssl_server_context();

  // create endpoint
  fd0 = ::socket(AF_INET, SOCK_STREAM, 0);

  if (fd0 < 0) {
    error_msg = "listen: Could not create socket endpoint.";
    return false;
  }

 int enable = 1;

 if (setsockopt(fd0,SOL_SOCKET,SO_REUSEADDR,(char*)&enable,sizeof(int)) < 0)
 { error_msg = string("SO_REUSEADDR ") + strerror(errno);
   return false;
  }

#if defined(__linux__)
 if (setsockopt(fd0,SOL_SOCKET,SO_REUSEPORT,(char*)&enable,sizeof(int)) < 0)
 { error_msg = string("SO_REUSEPORT ") + strerror(errno);
   return false;
  }
#endif

  // bind address
  sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons((u_short)portnum);

  if (bind(fd0, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    error_msg = "socket::listen:  bind failed.";
    return false;
  }

  if (::listen(fd0,qlength) < 0)
  { error_msg = "listen: listen failed.";
    return false;
   }

  show_certificates(cout);

  return true;
}
 

bool accept()
{
  sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);

  sockfd = ::accept(fd0, (sockaddr*)&client_addr, &len);

  if (sockfd == -1) {
    error_msg = "accept: accept failed";
    return false;
  }

  ipaddr = inet_ntoa(client_addr.sin_addr);

  ssl = SSL_new(ctx);  

  SSL_set_fd(ssl,sockfd); 

  int ssl_a = SSL_accept(ssl);

  if (ssl_a == -1) 
  { error_msg = string("SSL accept: port = %d\n", portnum);
    error_msg += ERR_error_string(ERR_get_error(),0); 
    return false;
    }

  return true;
}


bool connect(int timeout)
{
  init_ssl_client_context();

  hostent* host = gethostbyname(hostname);

  if (host == NULL )
  { error_msg = string("connect: ") + strerror(errno);
    return false;
  }

  //cerr << "CONNECT: " << hostname << " " << portnum << endl;

  sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd == -1)
  { error_msg = string("connect: ") + strerror(errno);
    return false;
  }

  int enable = 1;
  setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int));
//setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&enable,sizeof(int));

  if (timeout != 0) 
  { // set fd to non-blocking 
    long arg = fcntl(sockfd, F_GETFL, NULL); 
    arg |= O_NONBLOCK; 
    fcntl(sockfd, F_SETFL, arg); 
   }
 
// connect to server

  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portnum);
  addr.sin_addr.s_addr = *(long*)(host->h_addr);

  if (::connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) 
  { 
    if (timeout == 0)
    { error_msg = string("connect: ") + strerror(errno);
      return false;
     }

    // connect with timeout (seconds)

    timeval poll_t; 
    poll_t.tv_sec = timeout; 
    poll_t.tv_usec = 0; 
    fd_set wrset; 
    FD_ZERO(&wrset); 
    FD_SET(sockfd, &wrset); 

    if (select(sockfd+1, NULL, &wrset, NULL, &poll_t)) 
     { socklen_t len = sizeof(int); 
       int valopt = 0; 
       getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &valopt, &len); 
       if (valopt) 
       { error_msg = string("connect: getsockopt (err = %d)",valopt); 
         return false;
        } 
     } 
    else 
     { error_msg = "connect: timeout"; 
       return false;
     } 
  } 

  if (timeout != 0)
  { // set back to blocking mode 
    long arg = fcntl(sockfd, F_GETFL, NULL); 
    arg &= (~O_NONBLOCK); 
    fcntl(sockfd, F_SETFL, arg); 
   }


  // create new ssl structure and connect

  ssl = SSL_new(ctx);  

  SSL_set_fd(ssl,sockfd); 

  if (SSL_connect(ssl) == -1) 
  { error_msg = string("SSL connect: %s port = %d\n", ~hostname, portnum);
    error_msg += ERR_error_string(ERR_get_error(),0); 
    return false;
    }

  return true;
}

bool   connected() const { return ssl != 0; }
int    sock_fd()   const { return sockfd; }
void   set_fd(int fd) { sockfd = fd; }
string client_ip() const { return ipaddr; }
string get_error() const { return error_msg; }


void disconnect()
{ // release ssl connection  and close socket
  if (ssl) { SSL_free(ssl); ssl = 0; }
  if (sockfd != -1) close(sockfd);
  sockfd = -1;
}

void detach()
{ // close socket endpoint
  if (fd0 != -1) close(fd0); 
  fd0 = -1; 
}


size_t receive(char* buf, size_t buf_sz) {
  return SSL_read(ssl,buf,buf_sz); 
}


void send(char* buf, size_t sz) {
  SSL_write(ssl,buf,sz); 
}

};

LEDA_END_NAMESPACE
