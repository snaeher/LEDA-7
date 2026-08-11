/*******************************************************************************
+
+  LEDA 7.2.3  
+
+
+  _ws.cpp
+
+
+  Copyright (c) 1995-2026
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/h_array.h>
#include <LEDA/core/d_array.h>


#if defined(TLS_SOCKET)
#include <LEDA/system/tls_socket.h>
#else
#include <LEDA/system/websocket.h>
#endif

#include <LEDA/system/file.h>

#include <LEDA/coding/zlib.h>
#include <LEDA/coding/png.h>

#include <LEDA/graphics/x_window.h>

#include <signal.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include <iostream>

using std::cout;
using std::endl;

LEDA_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// functions defined in this file
//-----------------------------------------------------------------------------

extern int  OPEN_DISPLAY(int& width, int& height, int& depth, int& dpi);

extern void CLOSE_DISPLAY();

extern void UPDATE_DISPLAY(unsigned int* pixels, int w, int h, 
                           int x0, int y0, int x1, int y1, int x, int y);

extern int  NEXT_EVENT(string& s, int& v1, int& v2, int& x, int& y, 
                                                            unsigned long& t,
                                                            int timeout);
extern void SET_CURSOR(int id);

extern void SEND_TEXT(string txt);

//-----------------------------------------------------------------------------

enum format { RLE=0, RGB=1, RGBA=2, PNG=3 };

static const char* format_name[] = { "RLE", "RGB", "RGBA", "PNG" };

const int COMPRESSION_LIMIT = 1024;

const int HEADER_SZ = 21; // 1 byte for format + 5 int's (w,h,x,y,t_sz)

const int ws_port1 = 9681;
const int ws_port2 = 9682;

#if defined(TLS_SOCKET)
static tls_socket* sockp = 0;
#else
static socket* sockp = 0;
#endif

static websocket* wsp = 0;

//static int format = RLE;
static int format = RGB;
static int compression = 1;  // off(0) on(1) auto(2)

static unsigned long bytes_total = 0;
static unsigned long bytes_sent = 0;


static int cursor_id = -1;

static int display_width = 0;
static int display_height = 0;

static unsigned int* buffer_pixels = 0; 
static int buffer_sz = 0;


static void init_buffer(int width, int height) 
{ 
  if (width == display_width && height == display_height) return;

  buffer_sz = width*height;
  if (buffer_pixels) delete[] buffer_pixels;
  buffer_pixels = new unsigned int[buffer_sz];
  for(int i=0; i<buffer_sz; i++) buffer_pixels[i] = 0xff000000;  // undefined
  display_width = width;
  display_height = height;
}


inline unsigned int result_pixel(unsigned int* pixels, int i, bool write)
{ 
  // compare pixels[i] with buffer_pixels[i] and 
  // return pixels[i] if they are different and 0x00000001 if equal
  // set buffer_pixels[i] = pixels[i] if write = true

  assert(i < buffer_sz);

  unsigned int pix1 = pixels[i] & 0xffffff;
  unsigned int pix2 = buffer_pixels[i];

  if (write) buffer_pixels[i] = pix1;

  // alpha = 0 in result !
  return (pix1 == pix2) ? 0x00000001 : pix1;
}


static int send_buffer(unsigned char* buf, int buf_sz)
{ // do not compress first byte (format) and set compression bit
  int bytes = buf_sz;
  if (compression == 1 || (compression == 2 && buf_sz > COMPRESSION_LIMIT)) 
  { buf[0] |= 0x10;
    bytes = zlib().deflate(buf+1,buf_sz-1) + 1;
   }
  wsp->send_data(buf,bytes);
/*
  bytes_total += 3*num_pixels; // rgb
  bytes_sent += n;
  wsp->send_text(string("statistics: %lu %d",bytes_total,bytes_sent));
*/
  return bytes;
 }


static void signal_handler(int sig) {
  //cout << string("WS: SIGNAL(%d) %s",sig,sys_siglist[sig]) << endl;
  cout << string("WS: SIGNAL(%d)",sig) << endl;
  cout << "WS: SERVER DISCONNECT  --> CLOSE & EXIT" << endl;
  CLOSE_DISPLAY();
  exit(0);
}


void CLOSE_DISPLAY()
{ 
  cout << endl;
  cout << "CLOSE DISPLAY" << endl;
  cout << endl;

  if (wsp) 
  { wsp->disconnect();
    delete wsp;
    wsp = 0;
   }

  if (sockp) delete sockp;
  sockp = 0;

  if (buffer_pixels) delete[] buffer_pixels;
  buffer_pixels = 0;
}


static int ws_connect()
{
  bytes_total = 0;
  bytes_sent = 0;

#if !defined(TLS_SOCKET)
  sockp = new leda_socket; 
#else
  sockp = new tls_socket;
  sockp->set_cert_file("/etc/ssl/certbot/fullchain.pem");
  sockp->set_key_file("/etc/ssl/certbot/privkey.pem");

/*
  char* cert_file = getenv("SSL_CERT_FILE");
  if (cert_file) 
  { cout << "SSL_CERT_FILE: " << cert_file << endl;
    sockp->set_cert_file(cert_file);
   }
  else
    cout << "SSL_CERT_FILE:  NULL" << endl;

  char* key_file = getenv("SSL_KEY_FILE");
  if (key_file) 
  { cout << "SSL_KEY_FILE:  " << key_file << endl;
    sockp->set_key_file(key_file);
   }
  else
    cout << "SSL_KEY_FILE:  NULL" << endl;
*/
#endif

  string client_ip;
  int sock_fd = -1;

  char* p = getenv("REMOTE_ADDR");
  if (p) client_ip = p;

  wsp = new websocket(sockp);
//wsp->set_trace(true);

  char* ws_fd = getenv("WS_SOCK_FD");
  string msg = "";

  int ws_port = 0;

  if (ws_fd != 0)
  { ws_port = ws_port1;
    wsp->set_port(ws_port1);
    sock_fd = atoi(ws_fd);
    wsp->set_fd(sock_fd);
    char* p = getenv("WS_CLIENT");
    if (p) client_ip = p;
   }
  else
  { ws_port = ws_port2;
    wsp->set_port(ws_port2);
    cout << "ws: listening on port " << wsp->get_port() << endl;
    if (!wsp->listen()) {
      cout << "WEBSOCKET: listen failed" << endl; 
      cout << wsp->get_error() << endl;
      return 0;
    }
  
    cout << "ws: waiting for connection" << endl;
    if (!wsp->accept()) 
    { cout << "WEBSOCKET: accept failed" << endl; 
      cout << sockp->get_error() << endl;
      return 0;
    }
  
    sock_fd = wsp->sock_fd();

    cout << "ws: server handshake" << endl;
    if (!wsp->server_handshake()) 
    { cout << "WEBSOCKET: server_handshake failed" << endl; 
      cout << wsp->get_error() << endl;
      return 0;
    }

    cout << "ws: detach from socket endpoint" << endl;
    wsp->detach();

    cout << "wait for first msg" << endl;
    msg = wsp->receive_text();
  }

  string ws_client = wsp->get_socket().client_ip();

  cout << string("connected to %s  port = %d  fd = %d  msg = %s",
                                   ~client_ip, ws_port, sock_fd, ~msg) << endl;

  return wsp->sock_fd();
}


int OPEN_DISPLAY(int& width, int& height, int& depth, int& dpi)
{
  // return socket fd (-1 on error)

  if (wsp) {
    // display opened already
    return  wsp->sock_fd();
  }

  signal(SIGUSR1,signal_handler);
  signal(SIGTERM,signal_handler);
  
  string prog_name = program_invocation_name;

  cout << endl;
  cout << "OPEN DISPLAY: prog = " << prog_name << endl;
  cout << endl;

  int sock_fd = ws_connect();

  wsp->send_text("open: " + prog_name);

  cout << "waiting for display event" << endl;

  string event;
  int e,val1,val2,x,y;
  unsigned long t;
  while ((e = NEXT_EVENT(event,val1,val2,x,y,t,0)) != display_event) {
    cout << "skip: " << e << "  " << event_name[e] << endl;
  }

  // set display parameters

  width  = x - 2;
  height = y - 2;
  depth  = 24;
  dpi    = val1;

  format = t & 0x0f;
  compression = (t>>4) & 0x0f;

  cout << string("DISPLAY EVENT: %d x %d  dpi: %d  format: %s-%d", 
                 width,height,dpi,format_name[format],compression) << endl;
  cout << endl;

  init_buffer(width,height);

  return sock_fd;
}


static unsigned char* write_header(unsigned char* buf, unsigned char format,
                         int xpos, int ypos, int w, int h, int t_sz)
{ buf[0] = format;

  unsigned int* p = (unsigned int*)(buf+1);
  *p++ = xpos;
  *p++ = ypos;
  *p++ = w;
  *p++ = h;
  *p++ = t_sz;

  unsigned char* q = (unsigned char*)p;
  assert(q-buf == HEADER_SZ);

  return q;
}


void UPDATE_DISPLAY(unsigned int* pixels, int w, int h, 
                    int x0, int y0, int x1, int y1,
                    int xpos, int ypos)
{
  // copy pixel rectangle [x0,y0,x1,y1] from (w x h) pixel buffer
  // to display at position (xpos,ypos)
  
  if (x1 > w-1) x1 = w-1;
  if (y1 > h-1) y1 = h-1;
  if (x0 < 0) { xpos -= x0; x0 = 0; }
  if (y0 < 0) { ypos -= y0; y0 = 0; }

  if (x0 > x1 || y0 > y1) return;

  int width = x1-x0+1;
  int height = y1-y0+1;
  int num_pixels = width*height;

/*
  // handshake: send image parameters & receive "continue" (to slow)
  // problem: events can get lost
  int f = format;
  if (deflate) f |= 0x10;
  wsp->send_text(string("image: %d %d %d %d %d",f,width,height,xpos,ypos));
  string msg = wsp->receive_text();
  cout << "msg = " << msg << endl;
*/


  if (format == RGBA)
  { // extract rgba pixels from (x0,y0,x1,y1) into char buffer

    int buf_sz = 4*num_pixels + HEADER_SZ;
    unsigned char* buf = new unsigned char[buf_sz];

    unsigned char* q = write_header(buf,RGBA,xpos,ypos,width,height,0); 

    for (int j = y0; j <= y1; j++)
    { for(int i = x0; i <= x1; i++)
      { unsigned int pix = pixels[j*w + i];
         *q++ = (pix >> 16) & 0xff; // red
         *q++ = (pix >>  8) & 0xff; // green
         *q++ = (pix >>  0) & 0xff; // blue
         *q++ = 255;                // alpha
      }
    }

    send_buffer(buf,buf_sz);
    delete[] buf;

    return;
  }


  if (format == RGB)
  { // extract rgb pixels from (x0,y0,x1,y1) into char buffer
    
    int buf_sz = 3*num_pixels + HEADER_SZ;
    unsigned char* buf = new unsigned char[buf_sz];
    unsigned char* q = write_header(buf,RGB,xpos,ypos,width,height,0); 
  
    for (int j = y0; j <= y1; j++)
    { for(int i = x0; i <= x1; i++)
      { unsigned int pix = pixels[j*w + i];
         *q++ = (pix >> 16) & 0xff; // red
         *q++ = (pix >>  8) & 0xff; // green
         *q++ = (pix >>  0) & 0xff; // blue
      }
    }

    send_buffer(buf,buf_sz);
    delete[] buf;

    return;
  }



  if (format == PNG)
  { 
    // extract rgba pixels from (x0,y0,x1,y1) into char buffer

    unsigned char* rgba = new unsigned char[4*num_pixels];
    unsigned char* q = rgba;
  
    for (int j = y0; j <= y1; j++)
    { for(int i = x0; i <= x1; i++)
      { unsigned int pix = result_pixel(pixels,j*w+i,true);
        int alpha = (pix == 0x00000001) ? 0x00 : 0xff;
        *q++ = (pix >> 16) & 0xff; //red
        *q++ = (pix >>  8) & 0xff; //green
        *q++ = (pix >>  0) & 0xff; //blue
        *q++ = alpha;
       }
    }

    // encode rgba buffer into png format

    size_t png_sz = 0;
    unsigned char* png = png_encode_rgba(png_sz,rgba,width,height);
    delete[] rgba;

    if (png == 0)
    { error_handler(1,"ERROR: PNG DECODING");
      return;
    }

    // copy png bytes into send buffer and send it

    int buf_sz = png_sz + HEADER_SZ;
    unsigned char* buf = new unsigned char[buf_sz];
    unsigned char* p = write_header(buf,PNG,xpos,ypos,width,height,0); 

    for(size_t i=0; i<png_sz; i++) p[i] = png[i];
    delete[] png;

    wsp->send_data(buf,buf_sz);
    delete[] buf;

/*
    float percent = (100.0*png_sz)/(4*num_pixels);
    cout << endl;
    cout << string("%4d x%4d  %8d --->%6d bytes %5.2f %%  time: %.2f sec", 
                        width,height,4*num_pixels,png_sz,percent, cpu_time(t));
*/

    return;
  }


  assert(format == RLE);


  // RLE (Run Lengh Encoding)
  // use alpha-byte for encoding number of consecutive pixels of the same color

  double t = cpu_time();

  unsigned char* buf = 0;

  int table_sz = 0; // numColors
  int buf_sz = 0;
  int rle_sz = 0;


/*

  // too slow ?

  unsigned int color_table[256];

//d_array<int,int> color_map(0);
  h_array<int,int> color_map(0,1024);

  for (int j = y0; j <= y1; j++)
  { for(int i = x0; i <= x1; i++)
    { if (table_sz >= 256) continue;
      unsigned int clr = result_pixel(pixels,j*w+i,false);
      if (!color_map.defined(clr))
      { color_map[clr] = table_sz;
        color_table[table_sz] = clr | 0xff000000;
        table_sz++;
       }
     }
   }

  if (table_sz < 256)
  { 
    // less than 256 colors
    // use a color table and represent colors by a 1-byte index in the table

    // RLE entry (layout) 2 bytes:  [clr_bits|num_bits]
    // clr_bits + num_bits = 16  
    // clr_bits <= 8 and num_bits >= 8

    int clr_bits = 0;
    while ((1 << clr_bits) < table_sz) clr_bits++;

    int num_bits = 16 - clr_bits;
    int max_count =  (1 << num_bits) - 1;

    //cout << endl;
    //cout << "table_sz = " << table_sz << " ";
    //cout << "clr_bits = " << clr_bits << " ";
    //cout << "num_bits = " << num_bits << " ";
    //cout << "max_count = " << max_count << endl;

    // 2 bytes (unsigned short) per pixel + 
    // 4 bytes for each table entry + header

    buf_sz = 2*num_pixels + 4*table_sz + HEADER_SZ;
    buf = new unsigned char[buf_sz];

    unsigned char* q = write_header(buf,RLE,xpos,ypos,width,height,table_sz); 

    // write color table

    for(int i=0; i<table_sz; i++) 
    { *(unsigned int*)q = color_table[i];
       q += 4;
     }

    unsigned char* p = q;

    unsigned current_clr = result_pixel(pixels,y0*w+x0,false);
    int count = 0;

    for (int j = y0; j <= y1; j++)
    { for(int i = x0; i <= x1; i++)
      { unsigned int clr = result_pixel(pixels,j*w+i,true);
        if (clr == current_clr && count < max_count)
          count++;
        else
        { *(unsigned short*)q = (color_map[current_clr] << num_bits) | count;
          q += 2;
          current_clr = clr;
          count = 1;
         }
      }
    }

    *(unsigned short*)q = (color_map[current_clr] << num_bits) | count;
    q += 2;

    buf_sz = q-buf;
    rle_sz = (q-p)/2;

    assert(buf_sz == 4*table_sz + 2*rle_sz + HEADER_SZ);
  }
  else
*/

  { // 256 colors or more
    // pixel layout:  rgba  (a = #repeating rgb pixels)

    buf_sz = 4*num_pixels + HEADER_SZ; // 4 bytes per pixel + header 
    buf = new unsigned char[buf_sz];

    unsigned current_clr = result_pixel(pixels,y0*w+x0,false);

    int count = 0;

    unsigned char* p = write_header(buf,RLE,xpos,ypos,width,height,0); 

#if 1

    unsigned int* q = (unsigned int*)p;

    for (int j = y0; j <= y1; j++)
    { int i_start = j*w + x0;
      int i_stop = i_start + width;
      for(int i = i_start; i < i_stop; i++)
      { unsigned int clr = result_pixel(pixels,i,true);
        if (clr == current_clr && count < 255)
          count++;
        else
        { *q++ = (count << 24) | current_clr;
          current_clr = clr;
          count = 1;
         }
      }
    }

    *q++ = (count << 24) | current_clr;

    buf_sz = ((unsigned char*)q)-buf;

#else

    for (int j = y0; j <= y1; j++)
    { for(int i = x0; i <= x1; i++)
      { unsigned int clr = result_pixel(pixels,j*w+i,true);
        if (clr == current_clr && count < 255)
          count++;
        else
        { *(unsigned int*)p = (count << 24) | current_clr;
          p += 4;
          current_clr = clr;
          count = 1;
         }
      }
    }

    *(unsigned int*)p = (count << 24) | current_clr;
    p += 4;

    buf_sz = p-buf;

#endif

    rle_sz = buf_sz - HEADER_SZ;

/*
    int sum_alpha = 0;
    for(unsigned char* p = buf; p < q; p+=4) sum_alpha += p[3];
    assert(sum_alpha = num_pixels);
*/
  }

  int sbytes = send_buffer(buf,buf_sz);
  delete[] buf;

/*
  float p1 = (100.0*rle_sz)/num_pixels;
  float p2 = (100.0*buf_sz)/(3*num_pixels);
  float p3 = (100.0*sbytes)/(3*num_pixels);

  cout << string("%4dx%4d %3d %7d", width,height,table_sz,num_pixels);
  cout << string(" --->%6d %5.2f %%", rle_sz,p1);
  cout << string(" --->%6d %5.2f %%", buf_sz,p2);
  cout << string(" --->%6d %5.2f %%", sbytes,p3);
  cout << string("  %.6f s",cpu_time(t)) << endl;
  cout << endl;
*/

}



//------------------------------------------------------------------------------
// event handling
//------------------------------------------------------------------------------

int NEXT_EVENT(string& arg,int& val1,int& val2,int& x,int& y,unsigned long& t,
                                                               int msec)
{
  // msec > 0: msec timeout
  // msec = 0: blocking / no timeout
  // msec < 0: non-blocking (check)

  arg = "";
  val1 = 0;
  val2 = 0;
  x = 0;
  y = 0;
  t = 0;

  if (msec < 0) {
    // non blocking (set timeout to 1 msec)
    msec = 1;
  }

  if (msec > 0 && !wsp->poll(msec)) 
  { // TIMEOUT
    return no_event;
   }

/*
  if (msec == 0) {
    // blocking
    // manage_next_event uses always msec > 0 (non-blocking)
  }
*/

  string msg = wsp->receive_text();

  if (!wsp->connected() || msg == "") 
  { cout << "WS: CLIENT DISCONNECT  --> CLOSE & EXIT" << endl;
    CLOSE_DISPLAY();
    exit(0);
  }

  if (msg == "keep_alive") { 
    cout << "WS: KEEP ALIVE" << endl;
    return no_event;
  }

  if (!msg.starts_with("event:"))  {
    // cout << "WS: msg = " << msg << endl;
    return no_event;
  }

  msg = msg.replace("event:","").trim();

  string A[6];
  int num = msg.break_into_words(A,6);

  assert(num == 6);

  string event = A[0];

  arg = A[1];
  val1 = atoi(A[2]);
  val2 = 0;
  x = atoi(A[3]);
  y = atoi(A[4]);
  t = atol(A[5]);

  int e = no_event;

  if (event == "exit") {
    cout << "WS: EXIT EVENT --> DESTROY" << endl;
    e = destroy_event;
/*
    CLOSE_DISPLAY();
    exit(0);
*/
  }

  if (event == "display") 
  { e = display_event;
    // resize buffer if (size has changed)
    init_buffer(x,y);
   }

  if (event == "exposure")  e = exposure_event;
  if (event == "upload")    e = upload_file_event;
  if (event == "mousedown") e = button_press_event;
  if (event == "mouseup")   e = button_release_event;
  if (event == "mousemove") e = motion_event;
  if (event == "keydown")   e = key_press_event;
  if (event == "keyup")     e = key_release_event;

  if ((event == "keydown" || event == "keyup") && val1 > 255)
  { //special key
    switch (val1 & 0xff) {
        case  8: val1 = KEY_BACKSPACE;
                 break;
        case  9: val1 = KEY_TAB;
                 break;
        case 13: val1 = KEY_RETURN;
                 break;
        case 27: val1 = KEY_ESCAPE;
                 break;
        case 33: val1 = KEY_PAGE_UP;
                 break;
        case 34: val1 = KEY_PAGE_DOWN;
                 break;
        case 35: val1 = KEY_END;
                 break;
        case 36: val1 = KEY_HOME;
                 break;
        case 37: val1 = KEY_LEFT;
                 break;
        case 38: val1 = KEY_UP;
                 break;
        case 39: val1 = KEY_RIGHT;
                 break;
        case 40: val1 = KEY_DOWN;
                 break;
        case 44: val1 = KEY_PRINT;
                 break;
        case 45: val1 = KEY_INSERT;
                 break;
        case 46: val1 = KEY_DELETE;
                 break;
    }
  }

/*
  cout << event_name[e] << " "; 
  cout << "val1: " << val1 << " x: " << x << " y: " << y << " t: " << t << endl;
*/

  return e;
}


void SET_CURSOR(int id) 
{ if (id != cursor_id)
  { cursor_id = id;
    wsp->send_text(string("cursor: %d",id));
   }
}


void SEND_TEXT(string text) { 
  if (wsp) wsp->send_text(text); 
}


LEDA_END_NAMESPACE
