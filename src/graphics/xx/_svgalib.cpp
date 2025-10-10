/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _svgalib.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

void INIT_GRAPHICS(int mode, int& width, int& height, int& depth, int& dpi);

void DRAW_AREA(unsigned int* pixels, int w, int h, 
               int dx, int dy, int x0, int y0, int x1, int y1); 

int  CHECK_NEXT_EVENT(int& val, int& x, int& y, unsigned long& t, int msec);



#include <LEDA/system/basic.h>
#include <LEDA/graphics/x_window.h>
#include <stdio.h>
#include <vga.h>
#include <vgamouse.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/stat.h>

LEDA_BEGIN_NAMESPACE

static int mouse_but;
static int mouse_x;
static int mouse_y;

static int mouse_fd;
static int keyboard_fd;

enum { NO_EVENT, KEYBOARD_EVENT, MOUSE_EVENT }; 

void INIT_GRAPHICS(int mode, int& width, int& height, int& depth, int& dpi)
{
  if (mode == 1) // graphics mode
  { 
    if (vga_init() !=0)
     { fprintf (stderr, "vga_init failed.\n");
       exit(1);
      }

    //if (vga_setmode(G1024x768x256) != 0) 
    //if (vga_setmode(G800x600x256) != 0)
    if (vga_setmode(G640x480x16) != 0)
     { fprintf (stderr, "Cannot switch to graphics mode.\n");
       exit(1);
      }

     int rate = 300;

     int mouse_type = vga_getmousetype();

     mouse_fd = mouse_init_return_fd("/dev/psaux",MOUSE_PS2,rate);

     keyboard_fd = 0;

     initscr();
     cbreak();
     noecho();

     width = vga_getxdim();
     height= vga_getydim();
     depth = vga_getcolors();

     dpi = 96;

     mouse_setxrange(0,width-1);
     mouse_setyrange(0,height-1);
     mouse_setscale(32);
     mouse_setwrap(0);
    }
  else
  { mouse_close();
    vga_setmode(TEXT);
    nocbreak();
    echo();
   }
}

/*
void  define_color(int i, int r, int g, int b) 
{  vga_setpalette(i,r,g,b); }


void  draw_pixel(int x, int y, unsigned char c)
{ vga_setcolor(c);
  vga_drawpixel(x,y); 
 }
*/


void DRAW_AREA(unsigned char* pixels, int w, int h, int dx, int dy, 
                                      int x0, int y0, int x1, int y1,
                                      unsigned char* mask = 0)
{ unsigned char* p = pixels + y0*w + x0;
  for (int y = y0; y <= y1; y++)
  { vga_drawscansegment(p,x0+dx,y+dy,x1-x0+1);
    p += w;
   }
}



static int read_next_event(int *val, int *x, int *y, int timeout)
{ 
  int f = ((mouse_fd > keyboard_fd) ? mouse_fd : keyboard_fd) + 1;

  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(mouse_fd,&rfds);
  FD_SET(keyboard_fd,&rfds);

  struct timeval tv;
  tv.tv_usec = timeout;
  tv.tv_sec = 0;

  if (timeout == 0)
     select(f,&rfds,NULL,NULL,NULL);
  else
    if (select(f,&rfds,NULL,NULL,&tv) <= 0) return NO_EVENT;

  if (FD_ISSET(keyboard_fd,&rfds)) { 
    char c;
    read(keyboard_fd,&c,1);
    *val = c;
    return KEYBOARD_EVENT;
   }

  if (FD_ISSET(mouse_fd,&rfds)) { 
    //mouse_update();
    mouse_waitforupdate();
    *x = 2*mouse_getx();
    *y = 2*mouse_gety();
    *val = mouse_getbutton();
    return MOUSE_EVENT;
   }

}




int CHECK_NEXT_EVENT(int& val, int& x, int& y, unsigned long& t, int block)
{ 
  t = 0;

  int timeout = (block) ? 0 : 1000;

  int k = read_next_event(&val, &x, &y, timeout);

  if (k == KEYBOARD_EVENT)
  { x = mouse_x;
    y = mouse_y;
    switch (val) {
    case   1: val = KEY_ESCAPE;
              break;
    case 127: val = KEY_BACKSPACE;
              break;
    case  13: val = KEY_RETURN;
              break;
    }
    return key_press_event;
   }

  if (k == MOUSE_EVENT)
  { 
    if (val != mouse_but)
    { // read_next_event(&val, &x, &y, 150000);
      int e;
      if (val == 0) 
         { if (mouse_but == MOUSE_LEFTBUTTON)  val = 1;
           else
           if (mouse_but == MOUSE_RIGHTBUTTON) val = 3;
           else
           if (mouse_but == MOUSE_RIGHTBUTTON+MOUSE_LEFTBUTTON) val = 2;
           e = button_release_event;
          }
      else
         { if (val == MOUSE_LEFTBUTTON)  val = 1;
           else
           if (val == MOUSE_RIGHTBUTTON) val = 3;
           else
           if (val == MOUSE_RIGHTBUTTON+MOUSE_LEFTBUTTON) val = 2;
           e = button_press_event;
          }
       mouse_x = x;
       mouse_y = y;
       mouse_but = val;
       return e;
     }

    if (mouse_x != x || mouse_y != y)
    { mouse_x = x;
      mouse_y = y;
      return motion_event;
     }
   }

  return no_event;
}

LEDA_END_NAMESPACE
