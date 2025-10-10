/*******************************************************************************
+
+  LEDA 7.2  
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


#include <LEDA/core/string.h>
#include <LEDA/system/websocket.h>
#include <LEDA/system/http.h>

#include <string.h>
#include <assert.h>

#include <LEDA/system/tls_socket.h>

using namespace leda;

using std::cout;
using std::cerr;
using std::cin;
using std::endl;


int main()
{ 
  tls_socket sock;
  websocket ws(&sock);

#if defined(__win32__)
  ws.set_host("192.168.178.8");
#else
  ws.set_host("localhost");
#endif

  ws.set_port(8002);
  ws.set_trace(true);
  
  string path = "api/v2/channels/samsung.remote.control?name=W0hvbWUgQXNzaXN0YW50XSBMaXZpbmcgUm9vbSBUVg==";

/*
  string name = ws.base64("Remote-1");
  string path = "api/v2/channels/samsung.remote.control?name=" + name;
  //path += "&token=";
*/


  if (!ws.connect())
  { cerr << ws.get_error() << endl;
    return 1;
   }


  ws.handshake(path);

  //ws.ping("ok");

string json = "{\
  \"method\":\"ms.remote.control\",\
  \"params\":{\"Cmd\":\"Click\",\
              \"DataOfCmd\":\"KEY_VOLUP\",\
              \"TypeOfRemote\":\"SendRemoteKey\"\
             }\
}"; 

json = json.replace_all(" ","");
json = json.replace_all(",",", ");

cout << json << endl;
cout << endl;

while (ws.connected())
{ 
/*
  unsigned char buf[1024];
  int opcode = -1;
  bool fin =false;
  int sz = ws.read_frame(opcode,fin,buf,1024);
  cout << string("opcode = 0x%02x  sz = %d",opcode,sz) << endl;
  cout << buf << endl;
  if (opcode == 0xff) break;
*/

  string txt = ws.receive_text();
  cout << txt << endl;

  if (ws.connected()) {
    leda_wait(2);
    ws.send_text(json);
  }
}


  return 0;
}
