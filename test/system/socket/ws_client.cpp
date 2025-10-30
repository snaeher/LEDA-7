/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  ws_client.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/websocket.h>

#include <string.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::cin;
using std::endl;


int main()
{ 
  leda_socket sock;
  websocket ws(&sock);
  ws.set_host("chomsky.uni-trier.de");
  ws.set_port(8081);

  //ws.set_trace(true);

  if (!ws.connect())
  { cerr << ws.get_error() << endl;
    return 1;
   }

//ws.handshake();
  ws.handshake("chat/blabla?msg=hello");

  cout << "send: ok" << endl;
  ws.send_text("ok");

  string text = ws.receive_text();
  cout << "received: " << text << endl;

  text = "event: display 192 2000 1500 0";
  cout << "send: " << text << endl;
  ws.send_text(text);

  ws.ping("hello");

  // size_t buf_sz = 1024;

  size_t buf_sz = 1 << 24;
  unsigned char* buf = new unsigned char[buf_sz];


  cout.flush();

  int count = 0;

  for(;;) 
  { if ((++count % 3) == 0) ws.ping("hello");
    int opcode = -1;
    bool fin =false;
    int sz = ws.read_frame(opcode,fin,buf,buf_sz);
    cout << string("opcode = 0x%02x  sz = %d",opcode,sz) << endl;
    if (opcode == 0x0a) cout << "PONG" << endl;
    if (opcode == 0x01) cout << buf << endl;
    if (opcode == 0xff) break;
  }


  return 0;
}
