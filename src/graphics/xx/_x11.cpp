/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _x11.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/graphics/x_window.h>
#include <LEDA/core/string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


LEDA_BEGIN_NAMESPACE

//----------------------------------------------------------------------------

extern int  OPEN_DISPLAY(int& width, int& height, int& depth, int& dpi);

extern void CLOSE_DISPLAY();

extern void UPDATE_DISPLAY(unsigned int* pixels, int w, int h, 
                           int src_x0, int src_y0, int src_x1, int src_y1, 
                           int x, int y);

extern int  NEXT_EVENT(string& ev, int& val,int& x,int& y, unsigned long& t,
                                                           int msec);
extern void SET_CURSOR(int id);

extern void SEND_TEXT(string txt);

//----------------------------------------------------------------------------


static Display*      display = NULL;
static Visual*       visual = NULL;

static int x_width= 2000;
static int x_height = 1500;
static int x_depth = 24;
static int x_dpi = 96;

static int screen = 0;

static Window        root_window;
static Window        win;
static GC            gc;
static Cursor        cursor = 0;

static XImage*       image = 0;


void CLOSE_DISPLAY()
{ if (display == 0) return;
  XDestroyWindow(display,win);

  image->data = 0;
  XDestroyImage(image);
  image = 0;

  XCloseDisplay(display); 
  display = NULL;
}


int get_depth(Drawable d)
{ Window root;
  int x,y;
  unsigned int width, height, bwidth, depth;
  XGetGeometry(display,d,&root,&x,&y,&width,&height,&bwidth,&depth);
  return depth;
}
  


int OPEN_DISPLAY(int& width, int& height, int& depth, int& dpi)
{
  if (display != NULL) return ConnectionNumber(display);

  if ((display = XOpenDisplay(0)) == NULL)	
  { fprintf(stderr, "Can\'t open display: \n");
    abort();
   }

  screen = DefaultScreen(display);
  root_window = RootWindow(display,screen);

  x_width = int(0.6 * DisplayWidth(display,screen));
  x_height= int(0.8 * DisplayHeight(display,screen));

  int pixels = DisplayWidth(display,screen);
  int millis = DisplayWidthMM(display,screen);

  x_dpi = int(0.5 + (25.4 * pixels)/millis);

  XVisualInfo vinfo;

  XMatchVisualInfo(display,screen,24,TrueColor, &vinfo);
  assert(vinfo.depth == 24);

/*
  assert(XMatchVisualInfo(display,screen,32,TrueColor, &vinfo));
  assert(vinfo.depth == 32);
*/

  visual = vinfo.visual;
  x_depth = vinfo.depth;

  depth = x_depth;
  width = x_width;
  height = x_height;
  dpi = x_dpi;


  XGCValues     gc_val;
  gc_val.background = WhitePixel(display,screen);
  gc_val.foreground = BlackPixel(display,screen);
  gc_val.function = GXcopy;

  gc = XCreateGC(display,RootWindow(display,screen),
                 GCBackground | GCForeground | GCFunction, &gc_val);
  
  XSetLineAttributes(display,gc,1,LineSolid,CapButt,JoinMiter);

  image = XCreateImage(display,       // X display
                       visual,        // X visual
                       x_depth,       // X depth
                       ZPixmap,       // format
                       0,             // offset
                       0,             // pixel data (rgb)
                       x_width,       // pixel data width
                       x_height,      // pixel data height
                       32,            // bitmap_pad
                       //0              // bytes per line
                       4*x_width
                      );

  // open window
  
  XSetWindowAttributes attrib;
  attrib.colormap = XCreateColormap(display,root_window,visual,AllocNone);
  attrib.backing_store = Always;
  attrib.background_pixel = 0xffffffff;
  attrib.border_pixel = 0xff000000;

  win = XCreateWindow(display, root_window, 0, 0, width,height, 2, 
                     x_depth, InputOutput, visual,
                     CWBackingStore | CWColormap  | CWBorderPixel | CWBackPixel,
                     &attrib);

  XSelectInput(display,win, EnterWindowMask | LeaveWindowMask    |
                            KeyPressMask    | KeyReleaseMask     |
                            ButtonPressMask | ButtonReleaseMask  |
                            PointerMotionMask  | 
                            ExposureMask    | StructureNotifyMask); 
    
  XSetWindowBackground(display,win,WhitePixel(display, screen)); 
   
  XStoreName(display,win,"LEDA");
  XSetIconName(display,win,"LEDA");

  int xpos = 50;
  int ypos = 75;
  XMoveResizeWindow(display,win,xpos,ypos,width,height);

  XSizeHints size_hints;
  size_hints.flags = PPosition;
  size_hints.x = xpos;
  size_hints.y = ypos;
  XSetWMProperties(display,win,0,0,0,0,&size_hints,0,0);

  XMapWindow(display,win);
  
  XEvent e;
  do XMaskEvent(display,StructureNotifyMask,&e);
  while (e.type != MapNotify);

  while (XCheckWindowEvent(display, win, ExposureMask, &e));

  return ConnectionNumber(display);
}




void UPDATE_DISPLAY(unsigned int* pixels, int w, int h, 
                    int src_x0, int src_y0, int src_x1, int src_y1,
                    int x, int y)
{ 
  // copy pixel rectangle [src_x0,src_y0,src_x1,src_y1] from (w x h) pixel 
  // buffer to display at position (x,y)

  if (src_x0 < 0) { x -= src_x0; src_x0 = 0; }
  if (src_y0 < 0) { y -= src_y0; src_y0 = 0; }

  if (src_x1 > w-1) src_x1 = w-1;
  if (src_y1 > h-1) src_y1 = h-1;

  if (src_x0 > src_x1 || src_y0 > src_y1) return;

  int width  = src_x1-src_x0+1;
  int height = src_y1-src_y0+1;

  assert(get_depth(win) == x_depth);
  assert(image->depth == x_depth);

  image->data = (char*)pixels;
  XPutImage(display,win,gc,image,src_x0,src_y0,x,y,width,height);

/*
  image = XCreateImage(display,       // X display
                       visual,        // X visual
                       x_depth,       // X depth
                       ZPixmap,       // format
                       0,             // offset
                       (char*)pixels, // pixel data (rgb)
                       w,             // pixel data width
                       h,             // pixel data height
                       32,            // bitmap_pad
                       0              // bytes per line
                       );

  XPutImage(display,win,gc,image,src_x0,src_y0,x,y,width,height);
  image->data = 0; // do not free pixel puffer !
  XDestroyImage(image);
  image = 0;
*/


}



//------------------------------------------------------------------------------
// event handling
//------------------------------------------------------------------------------

static XEvent event;

int NEXT_EVENT(string& ev, int& val, int& x, int& y, unsigned long& t, int msec)
{
  // msec > 0: blocking / timeout
  // msec = 0: blocking / no timeout
  // msec < 0: non-blocking (check)

  ev = "";

  int e = no_event;

  bool blocking = (msec >= 0);

  if (msec > 0 && XPending(display) == 0)
  { int fd_disp = ConnectionNumber(display);

    timeval polltime;
    polltime.tv_sec  = msec / 1000;
    polltime.tv_usec = 1000 * (msec % 1000);

    fd_set rdset, wrset, xset;
    FD_ZERO(&rdset);
    FD_SET(fd_disp,&rdset);
    FD_ZERO(&wrset);
    FD_ZERO(&xset);
    FD_SET(fd_disp,&xset);

    int sel = select(fd_disp+1,&rdset,&wrset,&xset,&polltime);

    if (sel <= 0)
    { //w = 0;
      return no_event;
     }
  }


  if (blocking)
      XNextEvent(display, &event);
  else
  { if (XCheckMaskEvent(display,
                      EnterWindowMask | LeaveWindowMask    |
                      KeyPressMask    | PointerMotionMask  |
                      ButtonPressMask | ButtonReleaseMask  |
                      ExposureMask    | StructureNotifyMask, &event) == 0)
    return no_event;
   }


  switch (event.type) {

 case Expose:  e = exposure_event;
               x = event.xexpose.x;
               y = event.xexpose.y;
               val = event.xexpose.width;
               t = event.xexpose.height;
               break;


  case ConfigureNotify: e = configure_event;
                        x = event.xconfigure.x;
                        y = event.xconfigure.y;
                        //if (x == 0 && y == 0) x_window_to_screen(*w,x,y);
                        break;


  case DestroyNotify: //e = destroy_event;
                      break;



  case ButtonPress: val = event.xbutton.button;
                    x = event.xbutton.x;
                    y = event.xbutton.y;
                    t = event.xbutton.time;
                    e = button_press_event;
                    if (event.xbutton.state & Mod1Mask)    val = 2; 
                    if (event.xbutton.state & ShiftMask)   val |= 256; 
                    if (event.xbutton.state & ControlMask) val |= 512;
                    XUngrabPointer(display,CurrentTime);
                    break;

  case ButtonRelease: val = event.xbutton.button;
                      x = event.xbutton.x;
                      y = event.xbutton.y;
                      t = event.xbutton.time;
                      if (event.xbutton.state & Mod1Mask)    val = 2; 
                      if (event.xbutton.state & ShiftMask)   val |= 256; 
                      if (event.xbutton.state & ControlMask) val |= 512;
                      e = button_release_event;
                      break;

  case LeaveNotify:
  case EnterNotify:
  case MotionNotify: x = event.xmotion.x;
                     y = event.xmotion.y;
                     t = event.xbutton.time;
                     e = motion_event;
                     break;

  case KeyRelease:
  case KeyPress: { char c = 0;
                   KeySym keysym;
                   XComposeStatus status;
                   XLookupString((XKeyEvent*)&event,&c,1, &keysym, &status);

                   switch (keysym) {
  
                     case XK_Print:     c = KEY_PRINT;
                                        break;
                     case XK_BackSpace: c = KEY_BACKSPACE;
                                        break;
                     case XK_Return:    c = KEY_RETURN;
                                        break;
                     case XK_Escape:    c = KEY_ESCAPE;
                                        break;
                     case XK_Left:      c = KEY_LEFT;
                                        break;
                     case XK_Right:     c = KEY_RIGHT;
                                        break;
                     case XK_Up:        c = KEY_UP;
                                        break;
                     case XK_Down:      c = KEY_DOWN;
                                        break;
                     case XK_Home:      c = KEY_HOME;
                                        break;
                     case XK_End:       c = KEY_END;
                                        break;
                     case XK_Tab:       c = KEY_TAB;
                                        break;
                     case XK_Insert:    c = KEY_INSERT;
                                        break;
                     case XK_Delete:    c = KEY_DELETE;
                                        break;
                     case XK_Page_Up:   c = KEY_PAGE_UP;
                                        break;
                     case XK_Page_Down: c = KEY_PAGE_DOWN;
                                        break;

                     case XK_F1:        c = KEY_F1;
                                        break;
                     case XK_F2:        c = KEY_F2;
                                        break;
                     case XK_F3:        c = KEY_F3;
                                        break;
                     case XK_F4:        c = KEY_F4;
                                        break;
                     case XK_F5:        c = KEY_F5;
                                        break;
                     case XK_F6:        c = KEY_F6;
                                        break;
                     case XK_F7:        c = KEY_F7;
                                        break;
                     case XK_F8:        c = KEY_F8;
                                        break;
                     case XK_F9:        c = KEY_F9;
                                        break;
                     case XK_F10:       c = KEY_F10;
                                        break;
  
                    }

                   val = c;

                   if (event.xkey.state & ShiftMask)   val |= 256;
                   if (event.xkey.state & ControlMask) val |= 512;
                   if (event.xkey.state & Mod1Mask)    val |= 1024; // alt

                   x = event.xmotion.x;
                   y = event.xmotion.y;
                   t = event.xkey.time;

                   if (event.type == KeyPress)
                     e = key_press_event;
                   else
                     e = key_release_event;

                   break;
                  }
   }


  //printf("x11:  e = %d  val = %d  x = %d   y = %d\n",e,val,x,y);

  return e;
}

//------------------------------------------------------------------------------
// mouse cursor
//------------------------------------------------------------------------------


void SET_CURSOR(int id)
{
  if (cursor) {
    XFreeCursor(display,cursor);
    cursor = 0;
  }

  if (id >= 0)
  { cursor = XCreateFontCursor(display,id);
    XDefineCursor(display,win,cursor);
   }
  else
    XUndefineCursor(display,win);

  XFlush(display);
}


void SEND_TEXT(string txt) {}


LEDA_END_NAMESPACE
