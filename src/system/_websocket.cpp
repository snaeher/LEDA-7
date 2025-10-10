#include <LEDA/system/websocket.h>
#include <LEDA/coding/base64.h>
#include <LEDA/coding/sha.h>

#include <LEDA/core/string.h>
#include <LEDA/core/random.h>
#include <LEDA/system/file.h>

#include <string.h>
#include <stdint.h>

#include <assert.h>

LEDA_BEGIN_NAMESPACE;

using namespace leda;
using std::cout;
using std::endl;
using std::flush;


enum { OPCODE_CONTINUATION = 0x00,
       OPCODE_TEXT   = 0x01,
       OPCODE_BINARY = 0x02,
       OPCODE_CLOSE  = 0x08,
       OPCODE_PING   = 0x09,
       OPCODE_PONG   = 0x0a,
       OPCODE_ERROR  = 0xff 
     };

string opcode_string(int x) {
  string s = "OPCODE_UNUSED";
  switch (x) {
    case  OPCODE_CONTINUATION: 
               s = "OPCODE_CONTINUATION";
               break;
    case  OPCODE_TEXT: 
               s = "OPCODE_TEXT";
               break;
    case  OPCODE_BINARY: 
               s = "OPCODE_BINARY";
               break;
    case  OPCODE_CLOSE: 
               s = "OPCODE_CLOSE";
               break;
    case  OPCODE_PING: 
               s = "OPCODE_PING";
               break;
    case  OPCODE_PONG: 
               s = "OPCODE_PONG";
               break;
    case  OPCODE_ERROR: 
               s = "OPCODE_ERROR";
               break;
  }


  return s + string(" 0x%02x",x);
}



static string extract_value(string txt, string key) 
{ int p = txt.index(key);
  if (p == -1) return "";
  p += key.length();
  int q = txt.index("\r\n",p);
  if (q == -1) return "";
  return txt.substring(p,q).trim();
}




//----------------------------------------------------------------------------
// client: connect, disconnect, handshake
//----------------------------------------------------------------------------

websocket::websocket(socket* sock_ptr)
{ 
  sock = sock_ptr;
  sock_local = 0;
  if (sock == 0) {
    sock_local = new leda_socket;
    sock = sock_local;
  }
  host = sock_ptr->get_host();
  port = sock_ptr->get_port();
  trace = false;
  msg_deflate = false;
  buf_bytes = 0;
  buf_p = 0;
}

websocket::~websocket() 
{ if (connected()) disconnect(); 
  if (sock_local) delete sock_local;
}


bool websocket::connect() 
{ sock->set_host(host);
  sock->set_port(port);
  // reset buffer
  buf_bytes = 0;
  buf_p = 0;
  return sock->connect();
}

void websocket::disconnect() 
{ // send close frame and disconnect
  unsigned char cframe[] = { 0x88, 0x00 };
  sock->send((char*)cframe,2);
  sock->disconnect();
}

void websocket::detach() {
  sock->detach();
}



void websocket::handshake(string path) 
{
  //string origin = "http://" + host;
  string origin = host;

  // generate key
  unsigned char bytes[16];
  for(int i=0; i<16; i++) bytes[i] = (unsigned char)rand_int(0,255);
  string key = base64(bytes,16);

/*
  string request = "";
  request += string("GET /%s HTTP/1.1\r\n",~path);
  request += string("Host: %s:%d\r\n",~host,port);
  request += string("Upgrade: websocket\r\n");
  request += string("Connection: Upgrade\r\n");
  request += string("Sec-WebSocket-Key: %s\r\n",~key);
  request += string("Origin: %s\r\n", ~host);

//request += string("Sec-WebSocket-Protocol: chat, superchat\r\n");

  request += string("Sec-WebSocket-Version: 13\r\n");
  request += string("\r\n");

  if (trace) cout << request << endl;

  sock->send(~request);

  char txt[1024];
  int sz = sock->receive(txt,1024);
  txt[sz] = '\0';
*/

  // send header
  sock->send(string("GET /%s HTTP/1.1\r\n",~path));
  sock->send(string("Host: %s:%d\r\n",~host,port));
  sock->send("Upgrade: websocket\r\n");
  sock->send("Connection: Upgrade\r\n");
  sock->send(string("Sec-WebSocket-Key: %s\r\n",~key));
  sock->send(string("Origin: %s\r\n", ~host));
//sock->send("Sec-WebSocket-Protocol: chat, superchat\r\n");
  sock->send("Sec-WebSocket-Version: 13\r\n");

  sock->send("\r\n");


  // receive header
  string txt = "";
  while (txt.tail(4) != "\r\n\r\n")
  { unsigned char byte = 0;
    if (!next_byte(byte)) break;
    txt += string(byte);
  }

 if (trace) cout << txt << endl;


}

//----------------------------------------------------------------------------
// server: listen & accept
//----------------------------------------------------------------------------

bool websocket::listen() {
   sock->set_port(port);
   return sock->listen();
}

bool websocket::accept()
{
  if  (trace) cout << "websocket::accept()" << endl;

  if (!sock->accept())
  { if  (trace) cout << "ws: socket::accept failed" << endl;
    return false;
  }

  return true;
}


bool websocket::server_handshake()
{
  // reset buffer
  buf_bytes = 0;
  buf_p = 0;

  string txt = "";
  while (txt.tail(4) != "\r\n\r\n")
  { unsigned char byte = 0;
    if (!next_byte(byte)) {
      if  (trace) cout << "ws: next_byte failed" << endl;
      return false;
    }
    txt += string(byte);
  }

  string extensions = extract_value(txt,"Sec-WebSocket-Extensions:");

  msg_deflate = extensions.index("permessage-deflate") != -1;

  if (trace) 
  { cout <<  txt << endl;
    cout << endl;
    cout << "EXTENSIONS: " << extensions << endl;
    cout << endl;
    if (msg_deflate) cout << "PER MESSAGE DEFLATE" << endl;
  }



  string key = extract_value(txt,"Sec-WebSocket-Key:");

  string magic = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  unsigned char sha1_buf[20];
  sha1(sha1_buf,(uint8_t*)~magic,magic.length());

  string b64 = base64(sha1_buf,20);

  string answer;
  answer += "HTTP/1.1 101 Switching Protocols\r\n";
  answer += "Upgrade: websocket\r\n";
  answer += "Connection: Upgrade\r\n";
  answer += "Sec-WebSocket-Accept: " + b64 + "\r\n";

//answer += "Sec-WebSocket-Protocol: chat, superchat\r\n";

/*
  if (msg_deflate)  {
   answer += "Sec-WebSocket-Extensions: permessage-deflate";
   //answer += "client_max_window_bits=10;";
   //answer += "server_max_window_bits=10;";
   answer += "\r\n";
  }
*/

  answer += "\r\n";

  if (trace) {
    cout << "ANSWER:" << endl;
    cout << answer << endl;
  }

  sock->send((char*)answer.cstring(),answer.length());

  return true;
}

//----------------------------------------------------------------------------
// read bytes from socket (buffer)
//----------------------------------------------------------------------------

bool websocket::next_byte(unsigned char& byte)
{
  if (buf_p >= buf_bytes) 
  { buf_p = 0;
    buf_bytes = (int)sock->receive(buffer,buf_sz);
    if (buf_bytes <= 0) return false;
   }

  byte = (unsigned char)buffer[buf_p++];
  return true;
}

  


//----------------------------------------------------------------------------
// client/server: receiving and sending frames
//----------------------------------------------------------------------------

#define NEXT_BYTE(x)       \
  if (!next_byte(x))    \
  { opcode = OPCODE_ERROR; \
    buf[0] = '\0';         \
    return 0; }            
    

size_t websocket::read_frame(int& opcode, bool& fin, unsigned char* buf, 
                                                     size_t buf_sz)
{ unsigned char byte = 0;

  NEXT_BYTE(byte);

  fin = (byte & 0x80) != 0;
  opcode = byte & 0x0f;

  NEXT_BYTE(byte);
  
  bool masked = (byte & 0x80) != 0;

  size_t len = byte & 0x7f;

  if (len > 125)
  { int k = 0;
    if (len == 126) k = 2;
    if (len == 127) k = 8;
    len = 0;
    for(int i=0; i<k; i++) {
      NEXT_BYTE(byte);
      len = 256*len + byte;
    }
  }

  assert(len < buf_sz);

  // unmasking

  unsigned char masking_key[4] = { 0, 0, 0, 0 };

  if (masked) {
    for(int i=0; i<4; i++) { NEXT_BYTE(byte); masking_key[i] = byte; }
  }

  for(size_t i=0; i<len; i++) {
     NEXT_BYTE(byte);
     buf[i] = byte ^ masking_key[i%4];
  }

  if (trace) {
    cout << "read_frame: opcode = " << opcode << " len = " << len << endl;
  }

  buf[len] = '\0';

/*
  if (msg_deflate)
  { cout << "len  = " << len << endl;
    cout << "buf  = " << buf << endl;

    unsigned char* cbuf = new unsigned char[1024];

    len = zlib_inflate(cbuf,1024,buf,len);
    cout << "clen = " << len << endl;
    cout << "cbuf = " << cbuf << endl;
  
    for(size_t i=0; i<len; i++) buf[i] = cbuf[i];
    buf[len] = '\0';
  }
*/
  
  return len;
}


string websocket::receive_text()
{ 
  string result = "";

  bool fin = false;
  int opcode = -1;

/*
  assert(opcode != OPCODE_CONTINUATION);
*/

  while (!fin || (opcode != OPCODE_TEXT && opcode != OPCODE_CONTINUATION)) 
  { //const int buf_sz = 1024;
    const int buf_sz = 2048;
    unsigned char buf[buf_sz];

    size_t len = read_frame(opcode,fin,buf,buf_sz);

    if (opcode == OPCODE_ERROR)
    { if (trace) cout << "WEBSOCKET: ERROR" << endl;
      sock->disconnect();
      result = "";
      break;
     }

    if (opcode == OPCODE_CLOSE)
    { /*
      for(int i=0; i<len+2; i++) cout << string("0x%02x ",buf[i]);
      cout << endl;
      */

      if (trace) 
      { int code = -1;
        string reason = "none";
        if (len > 1) code = 256*buf[0] + buf[1];
        if (len > 2) reason = string((char*)buf+2);
        cout << "WEBSOCKET: CLOSE  code = " << code << " reason = " << reason;
        cout << endl;
       }
      
/*
      // send back close frame and disconnect
      send_frame(OPCODE_CLOSE,0,0);
*/
      sock->disconnect();
      result = "";
      break;
     }

    if (opcode == OPCODE_PING)
    { if (trace) {
        cout << string("WEBSOCKET: PING (sz = %d) ---> PONG",(int)len) << endl;
      }
      send_frame(OPCODE_PONG,buf,len);
      continue;
     }
  
    if (opcode == OPCODE_PONG) {
      if (trace) cout << "WEBSOCKET PONG: " << buf << endl;
      // do nothing
      continue;
    }

    if (opcode == OPCODE_TEXT || opcode == OPCODE_CONTINUATION)
    { if (result != "" && opcode != OPCODE_CONTINUATION) {
        cout << "OPCODE:   " << opcode_string(opcode) << endl;
        cout << "EXPECTED: " << "OPCODE_CONTINUATION" << endl;
      }
     result += string((char*)buf);
     continue;
    }

    if (trace) {
      cout << "WEBSOCKET: " << opcode_string(opcode) << " len = " << len << endl;
    }
  }

  return result;
}

string websocket::receive_text(int msec) {
  unsigned char buf[1024];
  buf[0] = '\0';
  if (sock->poll(msec))
  { bool fin = false;
    int opcode = -1;
    size_t len = read_frame(opcode,fin,buf,sizeof(buf));
    if (opcode == OPCODE_PING)
    { if (trace ) {
        cout << string("WEBSOCKET: PING (sz = %d) ---> PONG",(int)len) << endl;
      }
      send_frame(OPCODE_PONG,buf,len);
     }
   }
  return (char*)buf;
}


void websocket::send_frame(unsigned char opcode, unsigned char* buf, size_t len)
{
  unsigned char* cbuf = 0;

  if (trace) {
    cout << "send_frame: opcode = " << (int)opcode << " len = " << len << endl;
  }

/*
  if (msg_deflate && opcode == OPCODE_BINARY)
  { int cbuf_sz = len;
    if (cbuf_sz < 1024) cbuf_sz = 1024;
    cbuf = new unsigned char[cbuf_sz];
    int clen = zlib_deflate(cbuf,cbuf_sz,buf,len);
    cout << "SEND DEFLATED FRAME: clen = " << clen << endl;
    buf = cbuf;
    len = (size_t)clen;
   }
*/

  size_t frame_sz = len + 2;

  if (len >= 0xffff) frame_sz += 8;
  else if (len >= 126) frame_sz += 2;

  if (host != "") {
    // client: 4 masking bytes
    frame_sz += 4;
  }

  unsigned char* frame = new unsigned char[frame_sz];
  unsigned char* p = frame;

  *p++ = 0x80 | opcode;  // 0x80 (fin) + opcode

  if (len < 126)
    *p++ = (unsigned char)len;
  else
    if (len < 0xffff)
    { *p++ = 126; 
      *p++ = (len >> 8) & 0xff;
      *p++ = (len >> 0) & 0xff;
     }
   else
   { *p++ = 127;
#if defined(__win32__)
     *p++ = 0;
     *p++ = 0;
     *p++ = 0;
     *p++ = 0;
#else
     *p++ = (len >> 56) & 0xff;
     *p++ = (len >> 48) & 0xff;
     *p++ = (len >> 40) & 0xff;
     *p++ = (len >> 32) & 0xff;
#endif
     *p++ = (len >> 24) & 0xff;
     *p++ = (len >> 16) & 0xff;
     *p++ = (len >>  8) & 0xff;
     *p++ = (len >>  0) & 0xff;
   }

   if (host != "") 
   { // ws client: set mask bit
     frame[1] |= 0x80;
     //generate random mask bytes
     unsigned char mask_key[4]; 
     for(int i=0; i<4; i++) {
      unsigned char r = (unsigned char)rand_int(0,255);
      mask_key[i] = r;
      *p++ = r;
     }
     for(size_t i=0; i<len; i++) *p++ = buf[i] ^ mask_key[i%4];
   }
   else
   { // ws server
     for(size_t i=0; i<len; i++) *p++ = buf[i];
    }

   assert(p == frame+frame_sz);

   sock->send((char*)frame,frame_sz);
 
   delete[] frame;

   if (cbuf) delete[] cbuf;
}


void websocket::send_text(string text)
{ //cout << "SEND TEXT FRAME: " << text << endl;
  send_frame(OPCODE_TEXT,(unsigned char*)text.cstring(),text.length());
}


void websocket::send_data(unsigned char* buf, size_t sz)
{ //cout << "SEND BINARY FRAME: sz = " << sz << endl;
  send_frame(OPCODE_BINARY,buf,sz);
}


void websocket::send_file(string fname)
{ size_t sz = size_of_file(fname);
  unsigned char* buf = new unsigned char[sz];
  FILE* fp = fopen(fname,"rb");
  size_t bytes = fread(buf,1,sz,fp);
  fclose(fp);
  if (bytes > 0) send_data(buf,bytes);
  delete[] buf;
}


void websocket::ping(string text) {
   //cout << "WEBSOCKET PING: " << text << endl;
   send_frame(OPCODE_PING,(unsigned char*)~text,text.length());
}

void websocket::pong(string text) {
   //cout << "WEBSOCKET PONG: " << text << endl;
   send_frame(OPCODE_PONG,(unsigned char*)~text,text.length());
}

LEDA_END_NAMESPACE;
