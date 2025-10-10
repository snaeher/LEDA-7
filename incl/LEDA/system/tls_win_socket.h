#include <LEDA/core/string.h>

#undef MAXINT

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <winsock2.h>
#include <windows.h>

#define SECURITY_WIN32
#include <security.h>
#include <schannel.h>
#include <shlwapi.h>
#include <assert.h>
#include <stdio.h>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "secur32.lib")
#pragma comment (lib, "shlwapi.lib")

LEDA_BEGIN_NAMESPACE

class tls_socket_impl
{
   // payload + extra over head for header/mac/padding (
   static const int TLS_MAX_PACKET_SIZE  = 16384 + 512;
   char incoming[TLS_MAX_PACKET_SIZE];

   SOCKET sock;
   CredHandle handle;
   CtxtHandle context;

   SecPkgContext_StreamSizes sizes;

   size_t received; // bytes in incoming buffer (ciphertext)
   size_t used;     // bytes used from incoming buffer to decrypt current packet
   size_t available;// bytes available for decrypted bytes

   char* decrypted; // incoming buffer where data is decrypted inplace

   string host_name;
   int port_num;

   const char* error_str;

public:

tls_socket_impl(string host = "", int port = 0) 
{ received = 0;
  used = 0;
  available = 0;
  decrypted = 0;
  host_name = host,
  port_num = port,
  error_str = "";
}

~tls_socket_impl() {}

void set_host(string host) { host_name = host; }
void set_port(int port) { port_num = port; }

string get_host() const { return host_name; }
int get_port() const { return port_num; }

void set_cert_file(string path) { }
void set_key_file(string path) { }


string client_ip() const { return ""; }
int    sock_fd()   const { return sock; }
bool   connected() const { return sock != 0;  }

void   set_fd(int fd) { sock = fd; }

const char* get_error() const { return error_str; }


public:

// no server mode for windows

bool listen() { return false;}
bool accept() { return false;}

void show_certificates(ostream& out) {}


// client mode

int connect(int sec)
{
  sock = 0;

  // initialize windows sockets
  WSADATA wsadata;
  if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
      error_str = "WSAStartup failed";
      return false;
  }

  // create socket
  sock = ::socket(AF_INET, SOCK_STREAM, 0);
  if (sock == INVALID_SOCKET) {
      WSACleanup();
      error_str = "INVALID_SOCKET";
      sock = 0;
      return false;
  }

  char port_str[64];
  sprintf(port_str,"%u",port_num);

  struct timeval tv;
  tv.tv_sec = sec; // timeout seconds
  tv.tv_usec = 0;

  // connect to server
  if (!WSAConnectByNameA(sock,host_name,port_str,NULL,NULL,NULL,NULL,&tv,NULL))
  { closesocket(sock);
    WSACleanup();
    error_str = "WSAConnectByName failed";
    sock = 0;
    return false;
  }


  // setup schannel credentials

  // use only strong crypto alogorithms
  // automatically validate server certificate
  // no client certificate authentication
  // allow only TLS v1.2

  SCHANNEL_CRED cred;
  ZeroMemory(&cred,sizeof(cred));

  cred.dwVersion = SCHANNEL_CRED_VERSION;
  cred.grbitEnabledProtocols = SP_PROT_TLS1_2;  

/*
  cred.dwFlags = SCH_USE_STRONG_CRYPTO  | SCH_CRED_AUTO_CRED_VALIDATION  
                                        | SCH_CRED_NO_DEFAULT_CREDS;     

*/


   cred.dwFlags |= SCH_USE_STRONG_CRYPTO;
   cred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;

   // ERROR: SEC_E_UNTRUSTED_ROOT (samsung)
   // cred.dwFlags |= SCH_CRED_AUTO_CRED_VALIDATION; 

   cred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;


   // Get Client Credentials handle
   int cred_res = AcquireCredentialsHandleA(NULL,
                                            UNISP_NAME_A,
                                            SECPKG_CRED_OUTBOUND,
                                            NULL,
                                            &cred,
                                            NULL,
                                            NULL,
                                            &handle,
                                            NULL);


/*
  printf("cred_res = 0x%08x\n",cred_res);
  printf("cred_handle = 0x%08x\n",(int)&handle);
*/

  if (cred_res == SEC_E_INSUFFICIENT_MEMORY) {
    cout << "SEC_E_INSUFICIENT_MEMORY" << endl;
  }

  if (cred_res == SEC_E_NO_CREDENTIALS) {
    cout << "SEC_E_NO_CREDENTIALS" << endl;
  }

  if (cred_res != SEC_E_OK)
  { closesocket(sock);
    WSACleanup();
    sock = 0;
    return false;
   }

  received = used = available = 0;

  decrypted = NULL;

/*
  perform tls handshake
  1) call InitializeSecurityContext to create/update schannel context
  2) when it returns SEC_E_OK - tls handshake completed
  3) when it returns SEC_I_INCOMPLETE_CREDENTIALS 
     server requests client certificate (not supported here)
  4) when it returns SEC_I_CONTINUE_NEEDED  
     send token to server and read data
  5) when it returns SEC_E_INCOMPLETE_MESSAGE  
     need to read more data from server
  6) otherwise read data from server and go to step 1
*/

  CtxtHandle* context_ptr = NULL;

  bool result = true;

  for (;;)
  {
    SecBuffer inbuffers[2] = { 0 };
    inbuffers[0].BufferType = SECBUFFER_TOKEN;
    inbuffers[0].pvBuffer = incoming;
    inbuffers[0].cbBuffer = received;
    inbuffers[1].BufferType = SECBUFFER_EMPTY;

    SecBuffer outbuffers[1] = { 0 };
    outbuffers[0].BufferType = SECBUFFER_TOKEN;

    SecBufferDesc indesc = 
                  { SECBUFFER_VERSION, ARRAYSIZE(inbuffers), inbuffers };

    SecBufferDesc outdesc = 
                  { SECBUFFER_VERSION, ARRAYSIZE(outbuffers), outbuffers };

    DWORD flags = ISC_REQ_USE_SUPPLIED_CREDS | ISC_REQ_ALLOCATE_MEMORY | 
                  ISC_REQ_CONFIDENTIALITY    | ISC_REQ_REPLAY_DETECT   | 
                  ISC_REQ_SEQUENCE_DETECT    | ISC_REQ_STREAM;

    SECURITY_STATUS sec = InitializeSecurityContextA(
                           &handle,
                           context_ptr,
                           context_ptr ? NULL : (SEC_CHAR*)host_name.cstring(),
                           flags,
                           0,
                           0,
                           context_ptr ? &indesc : NULL,
                           0,
                           context_ptr ? NULL : &context,
                           &outdesc,
                           &flags,
                           NULL);

    // after first call to InitializeSecurityContext 
    // context is available and should be reused for next calls

    context_ptr = &context;

    if (inbuffers[1].BufferType == SECBUFFER_EXTRA)
    { 
      MoveMemory(incoming, incoming + (received - inbuffers[1].cbBuffer), 
                                                      inbuffers[1].cbBuffer);
      received = inbuffers[1].cbBuffer;
    }
    else {
      received = 0;
    }

    if (sec == SEC_E_OK) { 
      // tls handshake completed
      break;
    }

    if (sec == SEC_I_INCOMPLETE_CREDENTIALS) 
    {  // server asked for client certificate, not supported here
      result = false;
      break;
    }

    if (sec == SEC_I_CONTINUE_NEEDED) 
    { // need to send data to server

      char* buffer = (char*)outbuffers[0].pvBuffer;
      int size = outbuffers[0].cbBuffer;

      while (size != 0)
      { int d = ::send(sock, buffer, size, 0);
        if (d <= 0) {
          break;
        }
        size -= d;
        buffer += d;
      }

      FreeContextBuffer(outbuffers[0].pvBuffer);

      if (size != 0)
      { // failed to fully send data to server
        result = false;
        printf("size != 0\n");
        break;
      }
    }
    else 
      //if (sec != SEC_E_INCOMPLETE_MESSAGE)
      if (sec != SEC_E_INCOMPLETE_MESSAGE && sec != SEC_E_INVALID_TOKEN)
    {
      // SEC_E_CERT_EXPIRED - certificate expired or revoked
      // SEC_E_WRONG_PRINCIPAL - bad hostname
      // SEC_E_UNTRUSTED_ROOT - cannot vertify CA chain
      // SEC_E_ILLEGAL_MESSAGE / SEC_E_ALGORITHM_MISMATCH  
      // (cannot negotiate crypto algorithms)


      printf("sec = 0x%08x\n",sec);
      switch(sec) {
       case SEC_E_INSUFFICIENT_MEMORY: 
                                  printf("SEC_E_INSUFFICIENT_MEMORY\n");
                                  break;
       case SEC_E_INTERNAL_ERROR: printf("SEC_E_INTERNAL_ERROR\n");
                                  break;
       case SEC_E_INVALID_HANDLE: printf("SEC_E_INVALID_HANDLE\n");
                                  break;
       case SEC_E_INVALID_TOKEN:  printf("SEC_E_INVALID_TOKEN\n");
                                  break;
       case SEC_E_LOGON_DENIED:   printf("SEC_E_LOGON_DENIED\n");
                                  break;
       case SEC_E_NO_AUTHENTICATING_AUTHORITY:    
                                  printf("SEC_E_NO_AUTHENTICATING_AUTHORITY\n");
                                  break;
       case SEC_E_NO_CREDENTIALS: printf("SEC_E_NO_CREDENTIALS\n");
                                  break;
       case SEC_E_TARGET_UNKNOWN: printf("SEC_E_TARGE_UNKNOWN\n");
                                  break;
       case SEC_E_UNSUPPORTED_FUNCTION: 
                                  printf("SEC_E_UNSUPPORTED_FUNCTION\n");
                                  break;
       case SEC_E_WRONG_PRINCIPAL:printf("SEC_E_WRONG_PRINCIPAL\n");
                                  break;


       case SEC_E_CERT_EXPIRED:   printf("SEC_E_CERT_EXPIRED\n");
                                  break;
       case SEC_E_UNTRUSTED_ROOT:  printf("SEC_E_UNTRUSTED_ROOT\n");
                                   break;
       case SEC_E_ILLEGAL_MESSAGE: printf("SEC_E_ILLEGAL_MESSAGE\n");
                                   break;
       case SEC_E_ALGORITHM_MISMATCH: printf("SEC_E_ALGORITHM_MISMATCH\n");
                                   break;
       }

      result = false;
      break;
    }

    // read more data from server when possible
    if (received == sizeof(incoming))
    { // server is sending too much data instead of proper handshake?
      result = false;
      printf("too much data\n");
      break;
    }

    int r = recv(sock, incoming + received, 
                          sizeof(incoming) - received, 0);
    if (r == 0) { 
      // server disconnected socket
      return true;
    }

    if (r < 0)
    { // socket error
      result = false;
      printf("r < 0\n");
      break;
    }

    received += r;
  }

  if (!result)
  { DeleteSecurityContext(context_ptr);
    FreeCredentialsHandle(&handle);
    closesocket(sock);
    sock = 0;
    WSACleanup();
    return result;
  }

  QueryContextAttributes(context_ptr, SECPKG_ATTR_STREAM_SIZES, &sizes);

  return true;
}

void disconnect()
{
  // disconnects socket & releases resources 
  // call this even if tls_write/tls_read function return error

  DWORD type = SCHANNEL_SHUTDOWN;

  SecBuffer inbuffers[1];
  inbuffers[0].BufferType = SECBUFFER_TOKEN;
  inbuffers[0].pvBuffer = &type;
  inbuffers[0].cbBuffer = sizeof(type);

  SecBufferDesc indesc = { SECBUFFER_VERSION, ARRAYSIZE(inbuffers), inbuffers };
  ApplyControlToken(&context, &indesc);

  SecBuffer outbuffers[1];
  outbuffers[0].BufferType = SECBUFFER_TOKEN;

  SecBufferDesc outdesc={ SECBUFFER_VERSION,ARRAYSIZE(outbuffers),outbuffers };

  DWORD flags = ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_CONFIDENTIALITY | 
                ISC_REQ_REPLAY_DETECT   | ISC_REQ_SEQUENCE_DETECT | 
                ISC_REQ_STREAM;

  if (InitializeSecurityContextA(&handle, &context, NULL, flags, 0, 0, 
                        &outdesc, 0, NULL, &outdesc, &flags, NULL) == SEC_E_OK)
  {
    char* buffer = (char*)outbuffers[0].pvBuffer;
    int size = outbuffers[0].cbBuffer;
    while (size != 0)
    {
      int d = ::send(sock, buffer, size, 0);
      if (d <= 0)
      { // ignore any failures socket will be closed anyway
        break;
       }
      buffer += d;
      size -= d;
    }
    FreeContextBuffer(outbuffers[0].pvBuffer);
  }
  shutdown(sock, SD_BOTH);

  DeleteSecurityContext(&context);
  FreeCredentialsHandle(&handle);
  closesocket(sock);
  sock = 0;
  WSACleanup();
}

void detach() {
  if (sock) closesocket(sock);
  sock = 0;
}


int send(void* buffer, size_t sz)
{ 
 // returns 0 on success or negative value on error

  unsigned long size = sz;

  while (size != 0)
  {
    unsigned long use = min(size,sizes.cbMaximumMessage);

    char wbuffer[TLS_MAX_PACKET_SIZE];

    assert(sizes.cbHeader + sizes.cbMaximumMessage + sizes.cbTrailer 
                                                       <= sizeof(wbuffer));

    SecBuffer buffers[3];

    buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
    buffers[0].pvBuffer = wbuffer;
    buffers[0].cbBuffer = sizes.cbHeader;

    buffers[1].BufferType = SECBUFFER_DATA;
    buffers[1].pvBuffer = wbuffer + sizes.cbHeader;
    buffers[1].cbBuffer = use;

    buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
    buffers[2].pvBuffer = wbuffer + sizes.cbHeader + use;
    buffers[2].cbBuffer = sizes.cbTrailer;

    CopyMemory(buffers[1].pvBuffer, buffer, use);

    SecBufferDesc desc = { SECBUFFER_VERSION, ARRAYSIZE(buffers), buffers };
    SECURITY_STATUS sec = EncryptMessage(&context, 0, &desc, 0);

    if (sec != SEC_E_OK)
    { // this should not happen, but just in case check it
      return -1;
     }

    int total = buffers[0].cbBuffer + buffers[1].cbBuffer + buffers[2].cbBuffer;
    int sent = 0;
    while (sent != total)
    { int d = ::send(sock, wbuffer + sent, total - sent, 0);
      if (d <= 0)
      { // error sending data to socket, or server disconnected
        return -1;
       }
      sent += d;
    }

    buffer = (char*)buffer + use;
    size -= use;
  }

  return 0;
}


size_t receive(void* buffer, size_t size)
{
  // blocking read, waits & reads up to size bytes 
  // returns amount of bytes received on success (<= size)
  // returns 0 on disconnect or negative value on error
 
  size_t result = 0;

  while (size != 0)
  {
    if (decrypted)
    {
      // if there is decrypted data available, then use it as much as possible

      int use = min(size,available);

/*
      int use = size;
      if (use > available) use = available;
*/

      CopyMemory(buffer, decrypted, use);
      buffer = (char*)buffer + use;
      size -= use;
      result += use;

      if (use == available)
      {
        // all decrypted data is used, remove ciphertext from incoming buffer 
        // so next time it starts from beginning

        MoveMemory(incoming, incoming + used, received - used);
        received -= used;
        used = 0;
        available = 0;
        decrypted = NULL;
      }
      else
      {
        available -= use;
        decrypted += use;
      }
    }
    else
    {
      // if any ciphertext data available then try to decrypt it
      if (received != 0)
      {
        SecBuffer buffers[4];
        assert(sizes.cBuffers == ARRAYSIZE(buffers));

        buffers[0].BufferType = SECBUFFER_DATA;
        buffers[0].pvBuffer = incoming;
        buffers[0].cbBuffer = received;
        buffers[1].BufferType = SECBUFFER_EMPTY;
        buffers[2].BufferType = SECBUFFER_EMPTY;
        buffers[3].BufferType = SECBUFFER_EMPTY;

        SecBufferDesc desc = { SECBUFFER_VERSION, ARRAYSIZE(buffers), buffers };

        SECURITY_STATUS sec = DecryptMessage(&context, &desc, 0, NULL);

        if (sec == SEC_E_OK)
        {
          assert(buffers[0].BufferType == SECBUFFER_STREAM_HEADER);
          assert(buffers[1].BufferType == SECBUFFER_DATA);
          assert(buffers[2].BufferType == SECBUFFER_STREAM_TRAILER);

          decrypted = (char*)buffers[1].pvBuffer;
          available = buffers[1].cbBuffer;
          used = received - 
          (buffers[3].BufferType == SECBUFFER_EXTRA ? buffers[3].cbBuffer : 0);

          // data is now decrypted, go back to beginning of loop to 
          // copy memory to output buffer
          continue;
        }

        if (sec == SEC_I_CONTEXT_EXPIRED)
        { // server closed TLS connection (but socket is still open)
          received = 0;
          return result;
         }

        if (sec == SEC_I_RENEGOTIATE)
        { // server wants to renegotiate TLS connection, not implemented here
          return -1;
         }

        if (sec != SEC_E_INCOMPLETE_MESSAGE)
        { // some other schannel or TLS protocol error
          return -1;
         }

        // otherwise sec == SEC_E_INCOMPLETE_MESSAGE which means need to 
        // read more data
      }

      // otherwise  not enough data received to decrypt

      if (result != 0)
      { // some data is already copied to output buffer, so return that 
        // before blocking with recv
        return result;
      }

      if (received == sizeof(incoming))
      { // server is sending too much garbage data instead of proper TLS packet
        return -1;
      }

      // wait for more ciphertext data from server
      int r = recv(sock, incoming+received, sizeof(incoming)-received,0);

      if (r == 0)
      { // server disconnected socket
        return 0;
      }

      if (r < 0)
      { // error receiving data from socket
        result = -1;
        return result;
      }

      received += r;
    }
  }

  return result;
}

};

LEDA_END_NAMESPACE
