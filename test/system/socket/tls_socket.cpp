#include <LEDA/system/tls_socket.h>

using namespace leda;
using std::cout;
using std::endl;

int main()
{
    int port = 443;

  //string host = "www.google.com";
  //string host = "www.spiegel.de";

    string host = "2.base.maps.ls.hereapi.com";

  //host = "192.168.178.8";
  //port = 8002;

  tls_socket s(host,port);


    if (!s.connect())
    { cout << "Connect Error: " << s.get_error() << endl;
      return 1;
    }

    cout << "Connected to " << host << endl;

    // send request

/*
    string html = string("GET / HTTP/1.1\r\nHost: %s\r\n",~host)
                + "Connection: close\r\n\r\n";
    s.send(html);
*/

  string user_agent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36";

/*
  string user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";
*/


string path = "/";


  string header;
  header += "GET " + path + " HTTP/1.1";
  header += "\r\n";
  header += "User-Agent: " + user_agent;
  header += "\r\n";
  header += "Host: " + host;
  header += "\r\n";
/*
  header += "Connection: Keep-Alive";
  header += "\r\n";
*/
  header += "Accept: */*";
  header += "\r\n";

  header += "Connection: close";
  header += "\r\n";

  header += "\r\n";

cout << endl;
cout << header<< endl;

s.send(header);




    ofstream out("response.txt", ios::app | ios::binary);

    int received = 0;

    for (;;)
    { char buf[1024];
      int bytes = s.receive(buf, sizeof(buf));

      if (bytes < 0) {
         cout << "Error receiving data" << endl;
         break;
      }

      if (bytes == 0) {
         cout << "Socket disconnected" << endl;
         break;
       }

      out.write(buf,bytes);
      received += bytes;
    }

    cout << string("Received %d bytes",received) << endl;

    s.disconnect();
}

