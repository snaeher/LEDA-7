/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _x_basic.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// basic graphics x_... functions (declared in <LEDA/graphics/x_basic.h>)
// implemented by X11 library functions
//
// s.n. (1994-????)
//
// s.n. 2019
// xft fonts
// new color management
//
//------------------------------------------------------------------------------

#define V
#define XXX
#include "_x_basic.h"

#if defined(__unix__)

#include<stdlib.h>
#include<fcntl.h>
#include<sys/socket.h>

#if defined(__linux__)
#include<linux/netlink.h>
#endif

/*
#include<sys/inotify.h>
*/
#endif


LEDA_BEGIN_NAMESPACE

inline void SWAP(int& x1, int& x2)
{ int t = x1; x1 = x2; x2 = t; }


#define NUM_TMP_FONTS 128

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TRANSPARENT 0x00fffffe


// static variables


static Display* display = NULL;
static Visual* visual = NULL;
static int screen = 0;
static int depth = 0;
static int display_dpi = 96;

static Window root_window;
static Window top_window;
static Colormap color_map;

static x11_win* wlist[MAXWIN];


static bool do_not_open_display = false;

static Atom wm_protocols;
static Atom wm_delete_window;

static char dot_mask[2] = { 2,6 };
static char dash_mask[2] = { 6,4 };
static char dash_dot_mask[4] = { 6,2,2,2 };

static int fd_mnt = -1;
static int fd_dev = -1;

#if defined(__linux__)
static sockaddr_nl nls_dev;
#endif


static int vendor_rel;

static XEvent event;
static int trace_events = 0;
static int wcount = 0;

static XftFont* xft_text_font;
static XftFont* xft_italic_font;
static XftFont* xft_bold_font;
static XftFont* xft_button_font;
static XftFont* xft_fixed_font;

static int xft_text_sz;
static int xft_italic_sz;
static int xft_bold_sz;
static int xft_button_sz;
static int xft_fixed_sz;

static XftFont* tmp_xft_font[NUM_TMP_FONTS];
static char     tmp_font_name[NUM_TMP_FONTS][128];
static int      tmp_font_count = 0;


static int wm_frame_width =  7;
static int wm_title_width = 24;

static const char* x11_event_name[64];


struct x_pixrect {
 int w;
 int h;
 Pixmap P;
 Pixmap mask;

 float phi;

 int alpha;
 int anchor_x;
 int anchor_y;

 x_pixrect(int width, int height) : w(width), h(height), P(None), mask(None),
                                    phi(0), alpha(-1), anchor_x(0),anchor_y(0) 
 {}

 x_pixrect(int width, int height, Pixmap pm) : 
                                    w(width), h(height), P(pm), mask(None),
                                    phi(0), alpha(-1), anchor_x(0),anchor_y(0) 
 {}

};

//------------------------------------------------------------------------------
// display functions
//------------------------------------------------------------------------------

static int x_LEDA_EXCEPTION(Display* disp, XErrorEvent* err)
{ char msg[80];
  XGetErrorText(disp,err->error_code,msg,80);
  fprintf(stderr, "\nXError: %s\n",msg);
  abort();
  return 0;
}

static void x_init_xft_color(XftColor& color, int rgb)
{ 
  XRenderColor rcol;
  rcol.blue  = 256*(rgb & 0xff);
  rcol.green = 256*((rgb >>  8) & 0xff);
  rcol.red   = 256*((rgb >> 16) & 0xff);
//rcol.alpha = 256*((rgb >> 24) & 0xff);
  rcol.alpha = 0xffff;

  color.color = rcol;
  color.pixel = rgb | 0xff000000;

/*
   if (!XftColorAllocValue(display, visual, color_map, &rcol, &color))
     fprintf(stderr,"Cannot define color %08lx\n",rgb);
*/

/*
printf("xft: %08lx",color.pixel);
printf(" argb = %04x %04x %04x %04x\n",color.color.alpha,color.color.red,color.color.green,color.color.blue);
printf("\n");
*/

}


static XftFont* x_load_xft_font(const char* fname, int* sz_ptr = 0)
{
  int  sz = 0;
  if (isdigit(fname[1]))
  { for(int i=1; fname[i]!=0; i++) 
       sz = 10*sz + fname[i] - '0';
   }

  sz = int(0.9*sz);

  if (sz_ptr) *sz_ptr = sz;

  int i = tmp_font_count-1;
  while (i >= 0 && strcmp(fname,tmp_font_name[i]) != 0) i--;
  if (i >= 0) return tmp_xft_font[i];


  const char* fn = "";
  const char* weight = "";
  const char* slant = "";
  const char* antialias = "true";


/*
  const char* style = "";
*/

  switch (fname[0]) {

     case 'T': fn = "Droid Sans";
               //fn = "Arial";
               //style = "Regular";
               weight = "medium";
               slant = "roman";
               break;

     case 'B': fn = "Droid Sans";
               //fn = "Arial";
               //style = "Bold";
               weight = "bold";
               slant = "roman";
               break;

     case 'F': //fn = "Consolas";
               fn = "DejaVu Sans Mono";
               //style = "Regular";
               weight = "medium";
               slant = "roman";
               break;

     case 'I': //fn = "Droid Sans";
               fn = "Arial";
               //style = "Italic";
               weight = "medium";
               slant = "italic";
               break;
   }


  char buf[256];
  sprintf(buf,"%s:pixelsize=%d:weight=%s:slant=%s:antialias=%s",
                                           fn,sz,weight,slant,antialias);

  //fprintf(stderr,"Load Xft font %s\n",buf);

  XftFont* fp = XftFontOpenName(display,screen,buf);
  if (!fp) { 
     fprintf(stderr,"Cannot load font %s\n",buf);
  }

  if (tmp_font_count < NUM_TMP_FONTS)
  { assert(strlen(fname) < 128);
    strcpy(tmp_font_name[tmp_font_count],fname);
    tmp_xft_font[tmp_font_count] = fp;
    tmp_font_count++;
   }

  return fp;
}


static const char* getXresource(const char* name)
{
  char *type = NULL;
  XrmValue value;
  XrmInitialize();

  char *resourceString = XResourceManagerString(display);

  if (resourceString ==  NULL) {
      //printf("Xrm: DB = NULL\n");
      return NULL;
  }

/*
  printf("Entire DB\n%s\n", resourceString);
*/

  XrmDatabase db = XrmGetStringDatabase(resourceString);

  if (XrmGetResource(db,name,"String",&type,&value) == True)
    return value.addr;
  else
    return NULL;

}


//-----------------------------------------------------------------------


void x_do_not_open_display(bool b) { do_not_open_display = b; }

int x_open_display()
{ 
  if (do_not_open_display) return -1;

  if (display != NULL) {
    // display has been  opened before
    return ConnectionNumber(display);
  }


  x11_event_name[0]  = "None";
  x11_event_name[1]  = "None";
  x11_event_name[2]  = "KeyPress";
  x11_event_name[3]  = "KeyRelease";
  x11_event_name[4]  = "ButtonPress";
  x11_event_name[5]  = "ButtonRelease";
  x11_event_name[6]  = "MotionNotify";
  x11_event_name[7]  = "EnterNotify";
  x11_event_name[8]  = "LeaveNotify";
  x11_event_name[9]  = "FocusIn";
  x11_event_name[10] = "FocusOut";
  x11_event_name[11] = "KeymapNotify";
  x11_event_name[12] = "Expose";
  x11_event_name[13] = "GraphicsExpose";
  x11_event_name[14] = "NoExpose";
  x11_event_name[15] = "VisibilityNotify";
  x11_event_name[16] = "CreateNotify";
  x11_event_name[17] = "DestroyNotify";
  x11_event_name[18] = "UnmapNotify";
  x11_event_name[19] = "MapNotify";
  x11_event_name[20] = "MapRequest";
  x11_event_name[21] = "ReparentNotify";
  x11_event_name[22] = "ConfigureNotify";
  x11_event_name[23] = "ConfigureRequest";
  x11_event_name[24] = "GravityNotify";
  x11_event_name[25] = "ResizeRequest";
  x11_event_name[26] = "CirculateNotify";
  x11_event_name[27] = "CirculateRequest";
  x11_event_name[28] = "PropertyNotify";
  x11_event_name[29] = "SelectionClear";
  x11_event_name[30] = "SelectionRequest";
  x11_event_name[31] = "SelectionNotify";
  x11_event_name[32] = "ColormapNotify";
  x11_event_name[33] = "ClientMessage";
  x11_event_name[34] = "MappingNotify";
  x11_event_name[35] = "GenericEvent";


  display = XOpenDisplay(0);

  if (display == NULL) {
    fprintf(stderr, "X11: Cannot open display.\n");
    exit(1);
    return -1;
  }

  vendor_rel  = XVendorRelease(display);

  screen = DefaultScreen(display);

  root_window = RootWindow(display,screen);
  top_window = None;

/*
{ Window root;
  int x,y;
  unsigned int width, height, bwidth, depth;
  XGetGeometry(display,root_window,&root,&x,&y,&width,&height,&bwidth,&depth);
  printf("root: width = %d  height = %d depth = %d\n",width,height,depth);
}
*/
  

/*
  depth = DefaultDepth(display,screen);
  visual = DefaultVisual(display,screen);
  color_map = DefaultColormap(display,screen);
*/


/*
  int nxvisuals = 0;
  XVisualInfo visual_template;

  visual_template.screen = screen;

  XVisualInfo* visual_list = 
      XGetVisualInfo(display, VisualScreenMask, &visual_template, &nxvisuals);

  for (int i = 0; i < nxvisuals; ++i)
  { printf("  %3d: visual 0x%lx class %d (%s) depth %d\n",
             i,
             visual_list[i].visualid,
             visual_list[i].c_class,
             visual_list[i].c_class == TrueColor ? "TrueColor" : "unknown",
             visual_list[i].depth);
    }
   printf("\n");
   XFree(visual_list);
*/


  XVisualInfo vinfo;

//if (!XMatchVisualInfo(display,screen,32,TrueColor,&vinfo)) //bad match errors

  if (!XMatchVisualInfo(display,screen,24,TrueColor, &vinfo))
  { fprintf(stderr,"XMatchVisualInfo failed.\n");
    return -1;
   }

/*
   printf("Matched visual 0x%lx class %d (%s) depth %d\n",
         vinfo.visualid,
         vinfo.c_class,
         vinfo.c_class == TrueColor ? "TrueColor" : "unknown",
         vinfo.depth);
   printf("\n");
*/

  depth = vinfo.depth;
  visual = vinfo.visual;
  color_map = XCreateColormap(display,root_window,visual,AllocNone);

  // dpi
  int disp_w = DisplayWidth(display,screen);  
  int millis = DisplayWidthMM(display,screen);  

  display_dpi = int(0.5 + (25.4*disp_w)/millis);
  char* p = getenv("LEDA_DPI");
  if (p != 0) display_dpi = atoi(p);
 
  int sz = int(display_dpi/5.5);

//printf("xft font: sz = %d  dpi = %d\n",sz,display_dpi);

  xft_text_font   = x_load_xft_font(string("T%2d",sz),  &xft_text_sz);
  xft_italic_font = x_load_xft_font(string("I%2d",sz),  &xft_italic_sz);
  xft_bold_font   = x_load_xft_font(string("B%2d",sz-1),&xft_bold_sz);
  xft_fixed_font  = x_load_xft_font(string("F%2d",sz-2),&xft_fixed_sz);
  xft_button_font = x_load_xft_font(string("T%2d",sz),  &xft_button_sz);


/*
  // 17 predefined colors from /usr/lib/X11/rgb.txt
  color_count = 0;
  x_new_color("white");           // 0: white
  x_new_color("black");           // 1: black
  x_new_color("red");             // 2: red
  x_new_color("green2");          // 3: green
  x_new_color("blue3");           // 4: blue
  x_new_color("yellow");          // 5: yellow
  x_new_color("purple");          // 6: violet
  x_new_color("darkorange");      // 7: orange
  x_new_color("cyan");            // 8: cyan
  x_new_color("sienna");          // 9: brown
  x_new_color("magenta");         //10: pink 
  x_new_color("#0cb3a0");         //11: green2
  x_new_color("#6495ed");         //12: blue2 (cornflowerblue)
  x_new_color("grey85");          //14: grey1
  x_new_color("grey70");          //14: grey2
  x_new_color("grey45");          //14: grey3
  x_new_color("#ffffe4");         //16: ivory
*/

  XSetErrorHandler(x_LEDA_EXCEPTION);

  wm_protocols     = XInternAtom(display,"WM_PROTOCOLS",False);
  wm_delete_window = XInternAtom(display,"WM_DELETE_WINDOW",False);

  // NULL window

  x11_win* wp = new x11_win;

  wp->win = root_window;

  wp->LINEWIDTH = 1;
  wp->LINESTYLE = solid;
  wp->JOINSTYLE = 1;
  wp->MODE      = src_mode;
  wp->TEXTMODE  = transparent;
  wp->COLOR     = 0;
  wp->redraw    = 0;
  wp->inf       = 0;
  wp->mapped    = 0;
  wp->cursor_id = -1;
  wp->resizing = 0;
  wp->BORDER_COLOR_SAVE = 0;
  wp->BORDER_COLOR = 0;

  wp->tt_top = -1;
  wp->ttp_open = 0;
  wp->ttp_current = 0;

  wlist[0] = wp;
  wcount = 0;

  return ConnectionNumber(display);
}



char* x_root_pixrect(int x0, int y0, int x1, int y1)
{ 
  if (x_open_display() == -1) return 0;

  Window win = root_window;

  XGCValues  gc_val;
  gc_val.background = 0xffffff;
  gc_val.foreground = 0x000000;

  gc_val.function  = GXcopy; 
  GC gc = XCreateGC(display,win, GCBackground | GCForeground | GCFunction,
                    &gc_val);


  if (x0 > x1) SWAP(x0,x1);
  if (y0 > y1) SWAP(y0,y1);

  int w = x1-x0+1;
  int h = y1-y0+1;

  XImage* I = XGetImage(display,win,x0,y0,w,h,AllPlanes,ZPixmap);

  x_pixrect* im = new x_pixrect(w,h);

  im->P = XCreatePixmap(display,win,w,h,depth);

  XPutImage(display,im->P,gc,I,0,0,0,0,w,h);

  XDestroyImage(I);
  XFreeGC(display,gc);

  return (char*)im;

}



void x_close_display()
{ 
  if (display == 0)
  { cout << "x_close_display: already closed." << endl;
    return;
   }

  for(int i=1; i <= wcount; i++) 
  { if (wlist[i] == 0) continue; 
    delete wlist[i];
    wlist[i] = 0;
  }
  wcount = 0;

  XCloseDisplay(display); 
  display = 0; 
}



const char* x_display_info(int& width, int& height, int& dpi) 
{
  if (x_open_display() == -1)
  { width = 1024;
    height = 1024;
  }
  else
  { width = DisplayWidth(display,screen); 
    height = DisplayHeight(display,screen); 
    dpi = display_dpi;
  }

  return "x11"; 
}


/*
int x_display_bits_saved() { 
  return XDoesBackingStore(XScreenOfDisplay(display,screen)) != NotUseful; 
}
*/

int x_window_bits_saved(int w) { 
  x11_win* wp = wlist[w];
  XWindowAttributes attrib;
  if(wp->win_save) 
     XGetWindowAttributes(display,wp->win_save,&attrib);
  else
     XGetWindowAttributes(display,wp->win,&attrib);
  return attrib.backing_store != NotUseful;
}
 

void x_flush_display() { 
  XFlush(display); 
}


/*
int x_parse_color(const char* name)
{ 
  if (x_open_display() == -1) return 0;

  XColor xcolor;
  XParseColor(display, color_map, name, &xcolor);
  int r = xcolor.red/256;
  int g = xcolor.green/256;
  int b = xcolor.blue/256;
  return x_new_color(r,g,b);
}
*/


 
  

//------------------------------------------------------------------------------
// windows
//------------------------------------------------------------------------------

int x_create_window(void* inf, int width, int height, int bg_col,
                    const char* header, const char* label, int parent,
                    void (*func)(void*,int,int,int,int,int))
{
  if (display == NULL) return 0;

  x11_win* wp = new x11_win;

  wp->BG_COLOR = bg_col;

  XSetWindowAttributes attrib;
  attrib.colormap = color_map;
/*
  attrib.border_pixel = 0;
  attrib.background_pixel = 0;
*/
  attrib.border_pixel = 0xff000000;
  attrib.background_pixel = 0xffffffff;
  attrib.override_redirect = False;

  attrib.backing_store = Always;
  attrib.save_under = True;
/*
  attrib.backing_store = NotUseful;
  attrib.save_under = False;
*/


  if (width <= 0) width = 1;
  if (height <= 0) height = 1;


  // tooltip window
  wp->win_tt = XCreateWindow(display, root_window, 0, 0,
                             10,10,1,depth, InputOutput,visual,0,0);
 
/*
  wp->win_tt = XCreateWindow(display, root_window, 0, 0,
                             10,10,1,depth, InputOutput,visual,
                             CWBackPixel    | CWBorderPixel |
                             CWBackingStore | CWColormap,
                             &attrib);
*/
 
  XSelectInput(display,wp->win_tt, ExposureMask | StructureNotifyMask);

  XChangeWindowAttributes(display,wp->win_tt, CWOverrideRedirect,&attrib);

  //XSetWindowBackground(display,wp->win_tt,0xffffe4); // ivory
  XSetWindowBackground(display,wp->win_tt,0xffffee); // ivory
  XSetWindowBorder(display,wp->win_tt,0x000000); // black


  XGCValues  gc_val;
  gc_val.background = 0xffffe4; // ivory
  gc_val.foreground = 0x000000; // black

  wp->gc_tt = XCreateGC(display,root_window,
                                GCBackground | GCForeground, &gc_val);

  // main window

/*
  printf("Default Depth = %d\n",depth);
  int count;
  int* depths = XListDepths(display,screen,&count);
  for(int j=0; j<count; j++) printf("%d\n",depths[j]);
  printf("\n");
*/

  wp->win = XCreateWindow(display, wlist[parent]->win,
                          0, 0, width,height,1,depth,
                          InputOutput,visual,
                          CWBackPixel    | CWBorderPixel |
                          CWBackingStore | CWColormap,
                          &attrib);

  XSelectInput(display,wp->win, EnterWindowMask   | LeaveWindowMask    |
                                KeyPressMask      | KeyReleaseMask     |
                                ButtonPressMask   | ButtonReleaseMask  |
                                PointerMotionMask | FocusChangeMask    | 
                                ExposureMask      | StructureNotifyMask);

  XSetWindowBackground(display,wp->win,wp->BG_COLOR);
 
  XStoreName(display,wp->win,header);
  XSetIconName(display,wp->win,label);

  XWMHints wm_hints;
  wm_hints.flags = StateHint | InputHint;
  wm_hints.initial_state = NormalState;
  wm_hints.input = True;
  XSetWMProperties(display,wp->win,0,0,0,0,0,&wm_hints,0);

  XSetWindowBorder(display,wp->win,0x000000);

  //XSetWindowBorderWidth(display,wp->win,1);

  XSetWMProtocols(display,wp->win,&wm_delete_window,1);


  wp->gc_val.background = 0xffffff;
  wp->gc_val.foreground = 0x000000;

  wp->gc_val.function  = GXcopy; 
  wp->gc_val.line_style = LineSolid;
  wp->gc_val.cap_style = CapButt;
  //wp->gc_val.join_style = JoinMiter;
  wp->gc_val.join_style = JoinRound;
  wp->gc_val.line_width = 1;

  unsigned long gc_mask = GCBackground | GCForeground | GCFunction  | 
                          GCJoinStyle  | GCLineStyle  | GCLineWidth | 
                          GCFillRule   | GCGraphicsExposures;

  wp->gc_val.fill_rule =  WindingRule;
  wp->gc_val.graphics_exposures = False;

  wp->gc = XCreateGC(display,root_window,gc_mask, &(wp->gc_val));
 

  wp->xft_font = 0;
  wp->xft_size = 0;

  wp->xft_draw = XftDrawCreate(display,wp->win,visual,color_map);

  wp->xft_last_font = xft_text_font;
  wp->xft_last_size = xft_text_sz;

  wp->LINEWIDTH = 1;
  wp->JOINSTYLE = 1;
  wp->LINESTYLE = solid;
  wp->MODE      = src_mode;
  wp->COLOR     = 0x000000;
  wp->BG_COLOR  = 0xffffff;
  wp->BG_PIXMAP = 0;
  wp->TEXTMODE  = transparent;
  wp->redraw    = func;
  wp->special_event_handler = 0;
  wp->special_event_data = 0;
  wp->mapped    = 0;
  wp->inf       = inf;
  wp->buf       = 0;
  wp->buf_w     = 0;
  wp->buf_h     = 0;
  wp->win_save  = 0;
  wp->win_save2 = 0;
  wp->clip_mask = None;
  wp->icon_win  = 0;
  wp->frame_win  = 0;
  wp->cursor_id = -1;

  wp->tt_top = -1;
  wp->ttp_open = 0;
  wp->ttp_current = 0;

  int i = 1;
  while (i <= wcount)
  { if (wlist[i] == 0) break;
    i++;
   }
  if (i > wcount) wcount = i;

  assert(wcount < MAXWIN);

  wlist[i] = wp;
  return i;
}


/*
void x_set_topmost(int w) 
{ x11_win* wp = wlist[w];

  Atom stateAbove = XInternAtom(display,"_NET_WM_STATE_ABOVE",False);
  XChangeProperty(display, 
                  wp->win,
                  XInternAtom(display,"_NET_WM_STATE",False),
                  XA_ATOM, 
                  32,
                  PropModeReplace, 
                  (unsigned char*)&stateAbove,1);
}
*/



void x_set_topmost(int w) 
{ x11_win* wp = wlist[w];
  top_window = wp->win;

  XEvent event;
  event.xclient.type = ClientMessage;
  event.xclient.window = wp->win;
  event.xclient.message_type = XInternAtom(display,"_NET_WM_STATE",1);
  event.xclient.format = 32;

  event.xclient.data.l[0] = 1; // add
  event.xclient.data.l[1] = XInternAtom(display,"_NET_WM_STATE_ABOVE",1);
  event.xclient.data.l[2] = 0; 
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

  XSendEvent(display,root_window,
                     False,
                     SubstructureRedirectMask | SubstructureNotifyMask, 
                     &event);
}


void x_resize_window(int w,int xpos,int ypos,int width,int height,int parent)
{ 
  // xpos,ypos: position of upper left FRAME corner
  // width,height: width and height of CLIENT area

  int buffering = x_test_buffer(w);

  if (buffering)
  { x_stop_buffering(w);
    x_delete_buffer(w);
   }

  x11_win* wp = wlist[w];

  if (parent == 0)
  { xpos += wm_frame_width;
    ypos += wm_title_width;
   }

/*
  if (wp->win_save != 0)
     XMoveResizeWindow(display,wp->win_save,xpos,ypos,width,height);
  else
*/
     XMoveResizeWindow(display,wp->win,xpos,ypos,width,height);

  if (buffering) x_start_buffering(w);

  XFlush(display);
}


void* x_window_handle(int w) 
{ x11_win* wp = wlist[w];
  return (void*)wp->win;
}


int x_window_opened(int w) 
{ return wlist[w] && wlist[w]->mapped; }


/*
void x_set_size_hints(int w, int wmin, int wmax, int hmin, int hmax)
{
  x11_win* wp = wlist[w];
  
  XSizeHints ht;
 
  ht.min_width = wmin;
  ht.max_width = wmax;
 
  ht.min_height = hmin;
  ht.max_height = hmax;
 
  ht.flags =  PMinSize | PMaxSize;
	    
  XSetNormalHints(display, wp->win, &ht);  
}
*/


void x_open_window(int w, int xpos, int ypos, int width, int height, int pw, bool hidden)
{

 x11_win* wp = wlist[w];

/*
 printf("x_open_window: %d  win = %d  w = %d  h = %d  pw = %d  hidden = %d\n",
                        w,int(wp->win),width,height,pw,hidden); 
 fflush(stdout);
*/

  if (wp->mapped || hidden) return;

  if (width < 0)  
  { width = -width;
    xpos -= (width-1);
    if (pw == 0) xpos -= (2 * wm_frame_width);
   }

  if (height < 0)  
  { height = -height;
    ypos -= (height-1);
    if (pw == 0) ypos -= (wm_frame_width + wm_title_width);
   }


  if (pw > 0)
  { // set parent
    x11_win* pwp = wlist[pw];
    if (pwp->win_save != 0)
       XReparentWindow(display,wp->win,pwp->win_save,xpos,ypos);
    else
       XReparentWindow(display,wp->win,pwp->win,xpos,ypos);
  }

  if (pw < 0)
  { // frameless window (no parent)
    XSetWindowAttributes attrib;
    attrib.override_redirect = True;
    XChangeWindowAttributes(display,wp->win, CWOverrideRedirect,&attrib);
    XRaiseWindow(display,wp->win);
    x_set_cursor(w,XC_arrow);
   }

  if (pw == 0)
  { // normal main window (no parent)
    XSetWindowAttributes attrib;
    attrib.override_redirect = False;
    XChangeWindowAttributes(display,wp->win, CWOverrideRedirect,&attrib);
    XRaiseWindow(display,wp->win);
    XUndefineCursor(display,wp->win);
   }


/* raise window ?

  #define _NET_WM_STATE_REMOVE        0    // remove/unset property
  #define _NET_WM_STATE_ADD           1    // add/set property
  #define _NET_WM_STATE_TOGGLE        2    // toggle property

  Atom wmStateAbove = XInternAtom( display, "_NET_WM_STATE_ABOVE", 1 );
  Atom wmNetWmState = XInternAtom( display, "_NET_WM_STATE", 1 );

  if (wmStateAbove != None && wmNetWmState != None)
  { XClientMessageEvent xclient;
    memset( &xclient, 0, sizeof (xclient) );
    xclient.type = ClientMessage;
    xclient.window = wp->win;
    xclient.message_type = wmNetWmState;
    xclient.format = 32;
    xclient.data.l[0] = _NET_WM_STATE_ADD;
    xclient.data.l[1] = wmStateAbove;
    xclient.data.l[2] = 0;
    xclient.data.l[3] = 0;
    xclient.data.l[4] = 0;
    XSendEvent(display,root_window,False,
                       SubstructureRedirectMask | SubstructureNotifyMask,
                       (XEvent *)&xclient );

    XFlush(display);
  }
*/

  XMoveResizeWindow(display,wp->win,xpos,ypos,width,height);

  XSizeHints size_hints;
  size_hints.flags = PPosition;
  size_hints.x = xpos;
  size_hints.y = ypos;
  XSetWMProperties(display,wp->win,0,0,0,0,&size_hints,0,0);

  if (wp->BG_PIXMAP)
  { Pixmap pm = ((x_pixrect*)wp->BG_PIXMAP)->P;
    XSetWindowBackgroundPixmap(display,wp->win,pm);
   }
  else
    XSetWindowBackground(display,wp->win,wp->BG_COLOR);

  XMapWindow(display,wp->win);

  wp->mapped = 1;


/*
  // set border width to 1 pixel
  //XSetWindowBorderWidth(display,wp->win,1);

  XWindowChanges changes;
  changes.border_width = 1;
  XConfigureWindow(display,wp->win,CWBorderWidth,&changes);
*/


  // wait until window is mapped
  XEvent e;
  do XMaskEvent(display,StructureNotifyMask,&e); while (e.type != MapNotify);

  // remove all exposure events  (why ?)
  while (XCheckWindowEvent(display, wp->win, ExposureMask, &e));


/*
  if (pw < 0)
    XSetInputFocus(display,wp->win,RevertToParent,CurrentTime);
*/

  Window wm_frame = wp->win;

  if (pw >= 0)
  { for(;;) 
    { Window root,parent;
      Window *childlist = 0;
      unsigned int u;
      Status status = XQueryTree(display,wm_frame,&root,&parent,&childlist,&u);
      if (childlist) XFree((char*)childlist);
      if (parent == root || !parent || !status) break;
      wm_frame = parent;
     }
   }
  
  wlist[w]->frame_win = wm_frame;

  if (pw == 0)
  { int wx0,wy0,wx1,wy1;
    x_window_frame(w,wx0,wy0,wx1,wy1);
    wx1 = wy1 = 0;
    x_window_to_screen(w,wx1,wy1);
    wm_frame_width = wx1 - wx0;
    wm_title_width = wy1 - wy0;
   }

}
  


void x_close_window(int w)
{ x11_win* wp = wlist[w];

  if (!wp->mapped) return;

  if (wp->ttp_open) x_close_window(wp->ttp_open->win);
  wp->ttp_open = 0;
  wp->ttp_current = 0;

  XUnmapWindow(display,wp->win);
  wp->mapped = 0;
}


void x_minimize_window(int w)
{ x11_win* wp = wlist[w];
  if (wp->mapped) 
    XIconifyWindow(display,wp->win,screen);
}

void x_maximize_window(int w)
{ x11_win* wp = wlist[w];
/*
  if (wp->mapped) 
    XMaximizeWindow(display,wp->win,screen);
*/
}


void x_destroy_window(int w)
{ x11_win* wp = wlist[w];

//cout << "destroy_window: w = " << w << endl;

  if (display)
  { x_stop_buffering(w);
    x_close_window(w);

    if (wp->icon_win) XDestroyWindow(display,wp->icon_win);

    XFreeGC(display,wp->gc_tt);
    XDestroyWindow(display,wp->win_tt); 
  
/*
    // disconnect children
    Window r_win,p_win;
    Window* child=0;
    unsigned int n;
    XQueryTree(display,wp->win,&r_win,&p_win,&child,&n);
    for(unsigned int i=0; i < n; i++)
       XReparentWindow(display,child[i],root_window,0,0);
    if (child) XFree((char*)child);
*/
  
    XFreeGC(display,wp->gc);
    XDestroyWindow(display,wp->win); 
  }

  for(int i = wp->tt_top; i >= 0; i--) delete wp->tt_stack[i];

  delete wp;

  wlist[w] = 0;
  if (w == wcount) {
    while (wlist[w] == 0) w--;
    wcount = w;
  }
 }


static void get_window_geometry(Window win, int& xpos, 
                                            int& ypos, 
                                            unsigned& width, 
                                            unsigned& height, 
                                            unsigned& border_w)
{ Window root;
  unsigned depth;
  XGetGeometry(display,win,&root,&xpos,&ypos,&width,&height,&border_w,&depth);
}


static void ClearPixmap(int w, Pixmap pmap, int x0, int y0, int x1, 
                                                            int y1, 
                                                            int xorig,
                                                            int yorig)
{ x11_win* wp = wlist[w];

 if (wp->BG_PIXMAP)
 { x_pixrect* im = (x_pixrect*)wp->BG_PIXMAP;
   Pixmap pm = im->P;
   int wi = im->w;
   int he = im->h;

   if (xorig > 0)
      while (xorig > 0) xorig -= wi;
   else
      while (xorig+wi < 0) xorig += wi;

   if (yorig > 0)
      while (yorig > 0) yorig -= he;
   else
      while (yorig+he < 0) yorig += he;

   Window root;
   int xpos,ypos;
   unsigned width,height,bw,dep;
   XGetGeometry(display,pmap,&root,&xpos,&ypos,&width,&height,&bw,&dep);

   XSetFunction(display,wp->gc,GXcopy);

   int xmax = width;
   int ymax = height;

   for(int y = yorig;  y < ymax; y += he)
     for(int x = xorig; x < xmax; x += wi)
      if (x < x1 && x+wi > x0 && y < y1 && y+he >y0)
        XCopyArea(display,pm,pmap,wp->gc,0,0,wi,he,x,y);
  }
 else
  { wp->gc_val.foreground = wp->BG_COLOR;
    XChangeGC(display,wp->gc,GCForeground,&(wp->gc_val));
    XSetClipMask(display,wp->gc,None);
    XFillRectangle(display,pmap,wp->gc,x0,y0,x1-x0+1,y1-y0+1);
   }
}

 

void x_clear_window(int w, int x0, int y0, int x1, int y1, int xorig, int yorig)
{ x11_win* wp = wlist[w];
  if (x0 > x1)  SWAP(x0,x1);
  if (y0 > y1)  SWAP(y0,y1);

  wp->gc_val.ts_x_origin = xorig;
  wp->gc_val.ts_y_origin = yorig;
  XChangeGC(display,wp->gc,GCTileStipXOrigin|GCTileStipYOrigin,&(wp->gc_val));

  if (wp->win_save == 0 && ((xorig == 0 && yorig == 0) || wp->BG_PIXMAP == 0 ))
    XClearArea(display,wp->win,x0,y0,x1-x0+1,y1-y0+1,False);
  else
    ClearPixmap(w,wp->win,x0,y0,x1,y1,xorig,yorig);
}



void* x_window_inf(int w) { return wlist[w]->inf; }

int x_window_minimized(int win) 
{
  return 0 ;
}

int x_window_height(int w)
{ x11_win* wp = wlist[w];
  int x,y;
  unsigned width,height,bw;
  get_window_geometry(wp->win,x,y,width,height,bw);
  return height;
 }

int x_window_width(int w)
{ x11_win* wp = wlist[w];
  int x,y;
  unsigned width,height,bw;
  get_window_geometry(wp->win,x,y,width,height,bw);
  return width;
 }



void x_window_frame(int w, int& x0, int& y0, int& x1, int& y1)
{ x11_win* wp = wlist[w];
  unsigned width,height,bw;
  get_window_geometry(wp->frame_win,x0,y0,width,height,bw);
  x1 = x0 + width - 1;
  y1 = y0 + height - 1;
 }




//------------------------------------------------------------------------------
// drawing functions
//------------------------------------------------------------------------------

static void adjust_line(int s, int& x1, int& y1, int& x2, int& y2)
{ int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0) return;

  int xoff = s;
  int yoff = s;

  if (dx < 0) { dx = -dx; xoff = -s; }
  if (dy < 0) { dy = -dy; yoff = -s; }

  if ( dx >= dy) x2 += xoff;
  if ( dx <= dy) y2 += yoff;
 }



void x_pixel(int w, int x, int y)
{ x11_win* wp = wlist[w];
  XDrawPoint(display,wp->win,wp->gc,x,y); 
 }

int x_get_pixel(int, int, int) { return 0; }

void x_pixels(int w, int n, int *x, int *y)
{ x11_win* wp = wlist[w];
  XPoint* points = new XPoint[n];
  int i;
  for(i=0; i<n; i++)
  { points[i].x = (short)x[i];
    points[i].y = (short)y[i];
   }
  XDrawPoints(display,wp->win,wp->gc,points,n,CoordModeOrigin);
  delete[] points;
 }


inline void restrict_coord(int& x, int max_c)
{ if (x > max_c) x =  max_c;
  else if (x < -max_c) x = -max_c;
}

void x_line0(int w, int x1, int y1, int x2, int y2)
{ // draws a closed line (including endpoints)
  x_line(w,x1,y1,x2,y2);
  int d = (wlist[w]->LINEWIDTH - 1)/2;
  x_box(w,x1-d,y1-d,x1+d,y1+d);
  x_box(w,x2-d,y2-d,x2+d,y2+d);
/*
  int js = x_set_join_style(w,3);
  x_line(w,x1,y1,x2,y2);
  x_set_join_style(w,js);
*/
}


void x_line(int w, int x1, int y1, int x2, int y2)
{ x11_win* wp = wlist[w];

  int jstyle = wp->JOINSTYLE;

//int lwidth = (wp->LINEWIDTH+1)/2;
  int lwidth = 1;

  int max_c = (1 << 15) - 1;

  restrict_coord(x1,max_c);
  restrict_coord(x2,max_c);
  restrict_coord(y1,max_c);
  restrict_coord(y2,max_c);


  if (x1 > x2 || (x1 == x2 && y1 > y2))
  { SWAP(x1,x2);
    SWAP(y1,y2);
    if (jstyle == 1) 
       jstyle = 2; 
    else 
       if (jstyle == 2) jstyle = 1; 
   }

  if ((jstyle & 1) == 0) adjust_line(-lwidth,x2,y2,x1,y1);
  if ((jstyle & 2) == 2) adjust_line(+lwidth,x1,y1,x2,y2);

  XDrawLine(display,wp->win,wp->gc,x1,y1,x2,y2); 
 }


void x_lines(int w, int n, int *x1, int *y1, int* x2, int* y2)
{ 
  x11_win* wp = wlist[w];
  XSegment* segs = new XSegment[n];

  int max_c = (1 << 15) - 1;

  for(int i=0; i<n; i++)
  { restrict_coord(x1[i],max_c);
    restrict_coord(x2[i],max_c);
    restrict_coord(y1[i],max_c);
    restrict_coord(y2[i],max_c);

    segs[i].x1 = x1[i];
    segs[i].y1 = y1[i];
    segs[i].x2 = x2[i];
    segs[i].y2 = y2[i];
  }

  XDrawSegments(display,wp->win,wp->gc,segs,n);
  delete[] segs;
}
 


void x_rect(int w, int x1, int y1, int x2, int y2)
{ x11_win* wp = wlist[w];
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);

  int js = wp->gc_val.join_style;
  wp->gc_val.join_style = JoinMiter;
  XChangeGC(display,wp->gc,GCJoinStyle,&(wp->gc_val));

  XDrawRectangle(display,wp->win,wp->gc,x1,y1,x2-x1,y2-y1);

  wp->gc_val.join_style = js;
  XChangeGC(display,wp->gc,GCJoinStyle,&(wp->gc_val));
}
 
 
void x_box(int w, int x1, int y1, int x2, int y2)
{ x11_win* wp = wlist[w];
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  XFillRectangle(display,wp->win,wp->gc,x1,y1,x2-x1+1,y2-y1+1);
}



#define setarcpix(win,x,y)\
{ if (orient >= 0)\
   { if (ax*y >= ay*x && x*by >= y*bx)\
        XDrawPoint(display,wp->win,wp->gc,x0+x,y0+y); }\
  else  if (ax*y >= ay*x || x*by >= y*bx)\
          XDrawPoint(display,wp->win,wp->gc,x0+x,y0+y);\
 }
 

void x_arc0(int w, int x0, int y0, int r0, int, double start,double angle)
{ x11_win* wp = wlist[w];

  if (angle > 0)
  { start += angle;
    angle = -angle;
   }

  double ax =  cos(start);
  double ay = -sin(start);
  double bx =  cos(start+angle);
  double by = -sin(start+angle);
  double orient = ax*by - ay*bx;

  for (int r = r0-wp->LINEWIDTH/2; r <= r0+wp->LINEWIDTH/2; r++)
  { int y = r;
    int x = 1;
    int e = 3 - 2*y;

    setarcpix(win, 0, r);
    setarcpix(win, 0,-r);
    setarcpix(win, r, 0);
    setarcpix(win,-r, 0);

    while (x < y)
    { setarcpix(win, x, y);
      setarcpix(win, x,-y);
      setarcpix(win,-x, y);
      setarcpix(win,-x,-y);
      setarcpix(win, y, x);
      setarcpix(win, y,-x);
      setarcpix(win,-y, x);
      setarcpix(win,-y,-x);
      x++;
      if (e>=0) { y--; e = e - 4*y; }
      e = e + 4*x + 2;
     }

    if (x == y)
    { setarcpix(win, x, y);
      setarcpix(win, x,-y);
      setarcpix(win,-x, y);
      setarcpix(win,-x,-y);
     }
  }
}


void x_arc(int w, int x0, int y0, int r1, int r2, double start, double angle)
{ double len = angle*r1;
  if (len > 2000 || len < -2000) 
  { x_arc0(w,x0,y0,r1,r2,start,angle);
    return;
   }
  x11_win* wp = wlist[w];
  int s = (int)(360*32*start/M_PI);
  int a = (int)(360*32*angle/M_PI);
  XDrawArc(display,wp->win,wp->gc,x0-r1,y0-r2,2*r1+1,2*r2+1,s,a);
}



void x_ellipse(int w, int x0, int y0, int r1, int r2)
{ x11_win* wp = wlist[w];
  XDrawArc(display,wp->win,wp->gc,x0-r1,y0-r2,2*r1+1,2*r2+1,0,360*64); 
}


void x_fill_arc(int w, int x0, int y0, int r1, int r2, double start, double angle)
{ x11_win* wp = wlist[w];
  int s = (int)(360*32*start/M_PI);
  int a = (int)(360*32*angle/M_PI);
  XFillArc(display,wp->win,wp->gc,x0-r1,y0-r2,2*r1+1,2*r2+1,s,a);
}


/*
#define SETPIX(x,y)\
 XDrawPoint(display,wp->win,wp->gc,x,y); 

void x_circle(int w, int x0,int y0,int r0)
{ x11_win* wp = wlist[w];

  for (int r = r0-wp->LINEWIDTH/2; r <= r0+wp->LINEWIDTH/2; r++)
  { int y = r;
    int x = 0;
    int e = 3-2*y;

    XDrawPoint(display,wp->win,wp->gc,x,y); 

    SETPIX(x0,y0+r);
    SETPIX(x0,y0-r);
    SETPIX(x0+r,y0);
    SETPIX(x0-r,y0);

    for (x=1;x<y;)
      { SETPIX(x0+x,y0+y);
        SETPIX(x0+x,y0-y);
        SETPIX(x0-x,y0+y);
        SETPIX(x0-x,y0-y);
        SETPIX(x0+y,y0+x);
        SETPIX(x0+y,y0-x);
        SETPIX(x0-y,y0+x);
        SETPIX(x0-y,y0-x);
        x++;
        if (e>=0) { y--; e = e - 4*y; }
        e = e + 4*x + 2;
       }

    if (x == y)
    { SETPIX(x0+x,y0+y);
      SETPIX(x0+x,y0-y);
      SETPIX(x0-x,y0+y);
      SETPIX(x0-x,y0-y);
     }
  }
}



#define HLINE(x,y)\
XDrawLine(display,wp->win,wp->gc,x0-(x),y0+(y),x0+(x)+1,y0+(y))

void x_fill_circle(int w, int x0, int y0, int r)
{ x11_win* wp = wlist[w];

  int y = 1;
  int x = r;
  int e = 3-2*r;

  HLINE(x,0);

  while (y <= x)
  { HLINE(x,+y);
    HLINE(x,-y);
    if (y < x && e >= 0)
    { HLINE(y,+x);
      HLINE(y,-x);
      x--;
      e = e - 4*x;
     }
    y++;
    e = e + 4*y + 2;
   }
}
*/



void x_fill_ellipse(int w, int x0, int y0, int r1, int r2)
{ x11_win* wp = wlist[w];
  XFillArc(display,wp->win,wp->gc,x0-r1,y0-r2,2*r1+1,2*r2+1,0,360*64); 
 }


void x_fill_polygon(int w, int n, int *xcoord, int *ycoord)
{ x11_win* wp = wlist[w];
  XPoint* edges = new XPoint[n];
  for(int i=0;i<n;i++) 
  { edges[i].x = (short) xcoord[i];
    edges[i].y = (short) ycoord[i];
   }

  XFillPolygon(display,wp->win,wp->gc,edges,n,Nonconvex,CoordModeOrigin);

  delete[] edges;
}


void x_polyline(int w, int n, int *xcoord, int *ycoord, int adjust)
{ x11_win* wp = wlist[w];
  int jstyle = wp->JOINSTYLE;

  if (n < 2) return;

  if (xcoord[0] == xcoord[n-1] && ycoord[0] == ycoord[n-1]) jstyle = -1;

  if (adjust && (jstyle == 0 || jstyle == 3))
  { int d1=0,d2=0;
    if (jstyle == 0) 
    { d1 = -1;
      if (xcoord[0] > xcoord[1] || 
         (xcoord[0] == xcoord[1] && ycoord[0] > ycoord[1])) d1++;
      d2 = -1;
      if (xcoord[n-1] > xcoord[n-2] || 
         (xcoord[n-1] == xcoord[n-2] && ycoord[n-1] > ycoord[n-2])) d2++;
     }
  
    if (jstyle == 3) 
    { d1 = 0;
      if (xcoord[0] > xcoord[1] || 
         (xcoord[0] == xcoord[1] && ycoord[0] > ycoord[1])) d1++;
      d2 = 0;
      if (xcoord[n-1] > xcoord[n-2] || 
         (xcoord[n-1] == xcoord[n-2] && ycoord[n-1] > ycoord[n-2])) d2++;
    }
    adjust_line(d1,xcoord[1],ycoord[1],xcoord[0],ycoord[0]);
    adjust_line(d2,xcoord[n-2],ycoord[n-2],xcoord[n-1],ycoord[n-1]);
  }
  
  if (adjust == 1) return;

  XPoint* P = new XPoint[n];

  for(int i=0;i<n;i++) 
  { P[i].x = (short) xcoord[i];
    P[i].y = (short) ycoord[i];
   }

  XDrawLines(display,wp->win,wp->gc,P,n,CoordModeOrigin);
  delete[] P;
}


static void x_clip_polyline(int w, int n, int *xcoord, int *ycoord)
{ x11_win* wp = wlist[w];
  XPoint* edges = new XPoint[n];
  int x0 = xcoord[0];
  int x1 = xcoord[0];
  int y0 = ycoord[0];
  int y1 = ycoord[0];

  int i;
  for(i=0;i<n;i++) 
  { int x = xcoord[i];
    int y = ycoord[i];
    if (x < x0) x0 = x;
    if (y < y0) y0 = y;
    if (x > x1) x1 = x;
    if (y > y1) y1 = y;
    edges[i].x = x;
    edges[i].y = y;
   }

  int width  = x1-x0 + 1;
  int height = y1-y0 + 1;

  int win_w = x_window_width(w);
  int win_h = x_window_height(w);
  if (width > win_w) { width = win_w; x0 = 0; }
  if (height > win_h) { height = win_h; y0 = 0; }

  for(i=0;i<n;i++) 
  { edges[i].x -= x0;
    edges[i].y -= y0;
   }

  Pixmap pm = XCreatePixmap(display,wp->win,width,height,1);

  XGCValues  gc_val;
  gc_val.background = 0;//BlackPixel(display,screen);
  gc_val.foreground = 0;//BlackPixel(display,screen);
  gc_val.line_width = wp->LINEWIDTH;
  gc_val.function  = GXcopy; 
  GC gc = XCreateGC(display,pm,
                    GCBackground | GCForeground | GCLineWidth | GCFunction, 
                    &gc_val);

  XFillRectangle(display,pm,gc,0,0,width,height);

  gc_val.foreground = 1;//WhitePixel(display,screen);
  XChangeGC(display,gc,GCForeground,&gc_val);
  XDrawLines(display,pm,gc,edges,n,CoordModeOrigin);


  XSetClipOrigin(display,wp->gc,x0,y0);
  XSetClipMask(display,wp->gc,pm);
  XFreePixmap(display,pm);
  XFreeGC(display,gc);
  delete[] edges;
}

void x_polyline(int w, int n, int *xcoord, int *ycoord, int* clr)
{
  if (n < 2) return;

  int max_c = (1 << 15) - 1;
  for(int i=0; i<n-1; i++)
  { restrict_coord(xcoord[i],max_c);
    restrict_coord(ycoord[i],max_c);
   }


  x11_win* wp = wlist[w];

  x_clip_polyline(w,n,xcoord,ycoord);

  wp->gc_val.line_width = 3*wp->LINEWIDTH+1;
  XChangeGC(display,wp->gc,GCLineWidth,&(wp->gc_val));

  for(int i=0; i<n-1; i++)
  { wp->gc_val.foreground = clr[i];
    XChangeGC(display,wp->gc,GCForeground,&(wp->gc_val));
    double dx = xcoord[i+1] - xcoord[i];
    double dy = ycoord[i+1] - ycoord[i];
    double f = 3*wp->LINEWIDTH;
    if (fabs(dx) > fabs(dy))
      if (dx > 0) 
        { dy = f*dy/dx; dx = f; }
      else 
        { dy = -f*dy/dx; dx = -f; }
    else
      if (dy > 0) 
        { dx = f*dx/dy; dy = f; }
      else 
        { dx = -f*dx/dy; dy = -f; }

    XDrawLine(display,wp->win,wp->gc,xcoord[i]-int(dx),ycoord[i]-int(dy),
                                     xcoord[i+1]+int(dx),ycoord[i+1]+int(dy));
   }

  wp->gc_val.line_width = wp->LINEWIDTH;
  XChangeGC(display,wp->gc,GCLineWidth,&(wp->gc_val));

  XSetClipMask(display,wp->gc,None);
}




//------------------------------------------------------------------------------
// text
//------------------------------------------------------------------------------

int x_text_width(int w, const char* s, int len)
{ x11_win* wp = wlist[w];
  XftFont* fp = wp->xft_font;

  if ((unsigned)len > strlen(s)) len = strlen(s);

  XGlyphInfo info;
  XftTextExtents8(display,fp,(XftChar8*)s,len,&info);

//return info.width;
  return info.xOff;
}


int x_text_width(int w,const char* s)
{ return x_text_width(w,s,strlen(s)); }


int x_text_height(int w, const char* s)
{ XftFont* fp = wlist[w]->xft_font;
  return fp->ascent + fp->descent;
}


void x_text(int w, int x, int y, const char* s, int len)
{ x11_win* wp = wlist[w];

  XftFont* fp = wp->xft_font;

  if ((unsigned)len > strlen(s)) len = strlen(s);

  if (wp->TEXTMODE == opaque)
  { // draw background rectangle
    int wi = x_text_width(w,s,len);
    int he = x_text_height(w,s);
    int save = x_set_color(w,wp->BG_COLOR);
    x_box(w,x,y,x+wi,y+he);
    x_set_color(w,save);
  }

  y += (fp->ascent + 1); // 1 pixel lower

  XftColor cp;
  x_init_xft_color(cp,wp->COLOR);
  XftDrawChange(wp->xft_draw,wp->win);

//XftDrawStringUtf8(wp->xft_draw, &cp, fp, x, y,(XftCharUtf8*)s,len);
  XftDrawString8(wp->xft_draw, &cp, fp, x, y,(XftChar8*)s,len);
}


void x_text(int w, int x, int y, const char* s)
{ x_text(w,x,y,s,strlen(s)); }

void x_ctext(int w, int x, int y, const char* s)
{ int tw = x_text_width(w,s);
  int th = x_text_height(w,s);
  int xc = x-tw/2;
  int yc = int(y-th/2 + 0.5f);
  x_text(w,xc,yc,s);
}


void x_text_underline(int w, int x, int y, const char* s, int l,int r)
{ x11_win* wp = wlist[w];

  int x1 = x + x_text_width(w,s,l);
  int x2 = x + x_text_width(w,s,r+1);

  y += x_text_height(w,s);

  XDrawLine(display,wp->win,wp->gc,x1,y,x2,y);
}


void x_ctext_underline(int w, int x, int y, const char* s, int l, int r)
{ int tw = x_text_width(w,s);
  int th = x_text_height(w,s);
  x_text_underline(w, x-tw/2, y-th/2, s, l, r);
}


//------------------------------------------------------------------------------
// pixrects
//------------------------------------------------------------------------------


static Pixmap bgra_bitmask(Window win, unsigned char* bgra, int width, 
                                                            int height)
{ int bytes_per_line = width/8;
  if ((width % 8) > 0) bytes_per_line++;

  int  bytes = height*bytes_per_line;

  char* bits = new char[bytes];

  for(int i=0; i<bytes; i++) bits[i] = 0;

  unsigned int* p = (unsigned int*) bgra;
  for(int y=0; y<height; y++)
  { char* line = bits + y*bytes_per_line;
    for(int x=0; x<width; x++)
    { unsigned int clr = p[y*width + x];
    //if (clr & 0xff000000)   line[x/8] |= (1 << (x%8));
      if (clr != TRANSPARENT) line[x/8] |= (1 << (x%8));
     }
   }

  Pixmap pm = XCreateBitmapFromData(display,win,bits,width,height);
  delete[] bits;
  return pm;
}



  
static unsigned int interpolate_colors(float f, unsigned int clr1, 
                                                unsigned int clr2)
{ unsigned int result = 0;

/*
  if (clr1 == TRANSPARENT || clr2 == TRANSPARENT) return TRANSPARENT
*/

  for(int i=0; i<4; i++)
  { unsigned int x = ((clr1 >> 8*i)& 0xff);
    unsigned int y = ((clr2 >> 8*i)& 0xff);
    unsigned int z = (unsigned int)((1-f)*x + f*y);
    result += (z << 8*i);
   }

  return result;
}


static unsigned int interpolate_bgra(unsigned char* bgra, int width, int height,
                                     double x, double y)
{
  if (x < 0 || x >= width || y < 0 || y >= height) return 0x00000000;

  unsigned int* p = (unsigned int*)bgra;

  int x1 = int(x);
  int y1 = int(y);

  double xf = x - x1;
  double yf = y - y1;

  int i = y1*width+x1;

  unsigned int C11 = p[i];
  unsigned int C12 = (x1<width-1) ? p[i+1] : C11;
  unsigned int C21 = (y1<height-1) ? p[i+width] : C11;
  unsigned int C22 = (y1<height-1 && x1<width-1) ? p[i+width+1] : C11;

  unsigned int R1 = interpolate_colors(xf,C11,C12);
  unsigned int R2 = interpolate_colors(xf,C21,C22);

  return interpolate_colors(yf,R1,R2);
}
    


static void resize_bgra(unsigned char* bgra1, int width1, int height1,
                        unsigned char* bgra2, int width2, int height2)
{
  unsigned int* q = (unsigned int*)bgra2;

  double fx = double(width2)/width1;
  double fy = double(height2)/height1;

  for(int y=0; y<height2; y++)
  { for(int x=0; x<width2; x++)
    { unsigned int c = interpolate_bgra(bgra1,width1,height1,x/fx,y/fy);
      q[y*width2+x] = c;
     }
   }

}



static void rotate_bgra(unsigned char* bgra1, int width, int height,
                        unsigned char* bgra2, float phi)
{ 
  unsigned int* q = (unsigned int*)bgra2;

  int x0 = width/2;
  int y0 = height/2;

  for(int x = 0; x<width; x++)
  { for(int y = 0; y<height; y++)
    { double r = hypot(x-x0,y-y0);
      double alpha = atan2(x-x0,y-y0) + phi;
      double xx = x0 + r*sin(alpha);
      double yy = y0 + r*cos(alpha);
      q[y*width+x] = interpolate_bgra(bgra1,width,height,xx,yy);
     }
   }

}


static void resize_rotate_bgra(unsigned char* bgra1, int width1, int height1,
                               unsigned char* bgra2, int width2, int height2, 
                                                                 float phi)
{
  unsigned int* q = (unsigned int*)bgra2;

  double fx = double(width2)/width1;
  double fy = double(height2)/height1;

  int x0 = width1/2;
  int y0 = height1/2;

  for(int x = 0; x<width2; x++)
  { for(int y = 0; y<height2; y++)
    { double xf = x/fx;
      double yf = y/fy;
      unsigned int clr = 0;
      if (phi == 0)
        clr = interpolate_bgra(bgra1,width1,height1,xf,yf);
      else
      { double r = hypot(xf-x0,yf-y0);
        double alpha = atan2(xf-x0,yf-y0) + phi;
        double xx = x0 + r*sin(alpha);
        double yy = y0 + r*cos(alpha);
        clr = interpolate_bgra(bgra1,width1,height1,xx,yy);
       }
      q[y*width2+x] = clr;
     }
   }

}



static void blend_bgra(unsigned char* bgra1, unsigned char* bgra2, int width, 
                                                                   int height,
                                                                   float alpha)
{ int len = width * height;

  unsigned int* p = (unsigned int*)bgra1;
  unsigned int* q = (unsigned int*)bgra2;

  for(int i=0; i<len; i++)
  { unsigned int clr1 = p[i];
    unsigned int clr2 = q[i];

    int b1 = clr1 & 0xff;
    int g1 = (clr1 >>  8) & 0xff;
    int r1 = (clr1 >> 16) & 0xff;
    int a1 = (clr1 >> 24) & 0xff;

    int b2 = clr2 & 0xff;
    int g2 = (clr2 >>  8) & 0xff;
    int r2 = (clr2 >> 16) & 0xff;
    int a2 = (clr2 >> 24) & 0xff;

    float f = (a2 == 0) ? 0 : alpha;

    int r = int((1-f)*r1 + f*r2);
    int g = int((1-f)*g1 + f*g2);
    int b = int((1-f)*b1 + f*b2);
    int a = 255;

    q[i] = (a<<24) + (r<<16) + (g<<8) + b;
  }
}



static void x_print_pixrect(int w, char* prect)
{ x_pixrect* im = (x_pixrect*)prect;
  unsigned char* bgra = x_pixrect_to_bgra(w,prect);
  unsigned int* p = (unsigned int*)bgra;
  cout << im->w << " x " << im->h << endl;
  int len = im->w * im->h;
  for(int i=0; i<len; i++) cout << string("%08x",p[i]) << endl;
  cout <<endl;
}


static char* x_resize_pixrect(int w, char* prect, int width, int height)
{ x_pixrect* im = (x_pixrect*)prect;
  unsigned char* bgra = x_pixrect_to_bgra(w,prect);
  unsigned char* bgra2 = new unsigned char[4*width*height];
  resize_bgra(bgra,im->w,im->h,bgra2,width,height);
  delete[] bgra;
  return x_create_pixrect_from_bgra(w,width,height,bgra2);
}


static char* x_rotate_pixrect(int w, char* prect, float phi)
{ x_pixrect* im = (x_pixrect*)prect;
  int width = im->w;
  int height = im->h;
  unsigned char* bgra = x_pixrect_to_bgra(w,prect);
  unsigned char* bgra2 = new unsigned char[4*width*height];
  rotate_bgra(bgra,width,height,bgra2,phi);
  delete[] bgra;
  return x_create_pixrect_from_bgra(w,width,height,bgra2);
}


static char* x_resize_rotate_pixrect(int w, char* prect, int width, int height,
                                                                     float phi)
{ x_pixrect* im = (x_pixrect*)prect;

  double scale_x = double(width)/im->w;
  double scale_y = double(height)/im->h;

  unsigned char* bgra = x_pixrect_to_bgra(w,prect);

  unsigned char* bgra2 = new unsigned char[4*width*height];
  resize_rotate_bgra(bgra,im->w,im->h,bgra2,width,height,phi);

  char* prect1 = x_create_pixrect_from_bgra(w,width,height,bgra2);

  x_pixrect* im1 = (x_pixrect*)prect1;
  im1->anchor_x = int(0.5 + scale_x*im->anchor_x);
  im1->anchor_y = int(0.5 + scale_y*im->anchor_y);

  delete[] bgra;
  delete[] bgra2;

  return prect1;
}


static char* x_blend_pixrect(int w, char* prect1, char* prect2, float alpha)
{ x_pixrect* im1 = (x_pixrect*)prect1;
  int width = im1->w;
  int height = im1->h;

  x_pixrect* im2 = (x_pixrect*)prect2;
  if (im2->w < width) width = im2->w;
  if (im2->h < height) height = im2->h;

  unsigned char* bgra1 = x_pixrect_to_bgra(w,prect1);
  unsigned char* bgra2 = x_pixrect_to_bgra(w,prect2);
  blend_bgra(bgra1,bgra2,width,height,alpha);
  return x_create_pixrect_from_bgra(w,width,height,bgra2);
}



void  x_set_pixrect_alpha(char* prect, int alpha)
{ x_pixrect* im = (x_pixrect*)prect;
  im->alpha = alpha;
}

void  x_set_pixrect_phi(char* prect,float phi)
{ x_pixrect* im = (x_pixrect*)prect;
  im->phi = phi;
}

void  x_set_pixrect_anchor(char* prect,int x, int y)
{ x_pixrect* im = (x_pixrect*)prect;
  im->anchor_x = x;
  im->anchor_y = y;
}


char* x_create_pixrect_from_bgra(int w, int width, int height, 
                                                   unsigned char* data, 
                                                   float scale_f)
{ 
  // bgra:  a<<24 + r<<16 + g<<8 + b

  x11_win* wp = wlist[w];
  XGCValues  gc_val;
  gc_val.background = 0xffffff;
  gc_val.foreground = 0x000000;
  gc_val.function  = GXcopy; 

  GC gc = XCreateGC(display,root_window,
                    GCBackground | GCForeground | GCFunction,
                    &gc_val);

  unsigned char* buf = 0;

  if (scale_f == 1)
  { int sz = 4*width*height;
    buf = new unsigned char[sz];
    memcpy(buf,data,sz);
  }
  else
  { int w = int(0.5 + scale_f*width);
    int h = int(0.5 + scale_f*height);
    buf = new unsigned char[4*w*h];
    resize_bgra(data,width,height,buf,w,h);
    width = w;
    height = h;
  }

  unsigned int* p = (unsigned int*)buf;
  int sz = width*height;
  for(int i=0; i<sz; i++) {
    //if ((p[i] & 0xff000000) == 0) p[i] = TRANSPARENT;
    if (((p[i] >> 24) & 0xff) < 64 ) p[i] = TRANSPARENT;
  }


  Pixmap pm = XCreatePixmap(display,wp->win, width, height, depth);

  XImage* im = XCreateImage(display,
                            visual,
                            depth,
                            ZPixmap,      //format
                            0,            //offset
                            (char*)buf,
                            width,
                            height,
                            32,           // bitmap_pad
                            0  /* 4*width // bytes_per_line */
                            );

  XPutImage(display,pm,gc,im,0,0,0,0,width,height);

  x_pixrect* prect =  new x_pixrect(width,height,pm);
  prect->mask = bgra_bitmask(wp->win,buf,width,height);

  im->data = 0; // prevents deleting image data
  XDestroyImage(im);

  XFreeGC(display,gc);

  return (char*)prect; 
}


char* x_create_pixrect_from_bits(int w, int width, int height, 
                                                   unsigned char* data,
                                                   int fg_col, 
                                                   int bg_col) 
{ 
  x11_win* wp = wlist[w];
  Pixmap pm = XCreatePixmapFromBitmapData(display,wp->win,(char*)data, 
                                          width,height, 
                                          fg_col, bg_col,
                                          depth);
  return (char*) new x_pixrect(width,height,pm);
 }



char* x_create_pixrect(int w, int x1, int y1, int x2, int y2)
{ x11_win* wp = wlist[w];
  drawing_mode save = x_set_mode(w,src_mode);
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);

  int wi = x2-x1+1;
  int he = y2-y1+1;

  x_pixrect* im = new x_pixrect(wi,he);
  im->P = XCreatePixmap(display,wp->win,wi,he,depth);

  XSetFunction(display,wp->gc,GXcopy);
  XCopyArea(display,wp->win,im->P,wp->gc,x1,y1,wi,he,0,0);

  x_set_mode(w,save);
  return (char*)im;
 }


void x_get_pixrect_size(char* pr, int& w, int& h)
{ if (pr == 0)
    w = h = 0;
  else
  { x_pixrect* im = (x_pixrect*)pr;
    w = im->w;
    h = im->h;
   }
 }


void x_get_pixrect_anchor(char* pr, int& x, int& y)
{ if (pr)
  { x_pixrect* im = (x_pixrect*)pr;
    x = im->anchor_x;
    y = im->anchor_y;
   }
 }


void x_insert_pixrect(int w, char* prect)
{ // place into upper left corner of w
  x_pixrect* im = (x_pixrect*)prect;
  x_insert_pixrect(w,0,im->h-1,prect);
}


void x_insert_pixrect(int w, int x, int y, char* prect)
{ // (x,y) lower left corner !

  if (prect == 0) return;

  x11_win* wp = wlist[w];
  drawing_mode save = x_set_mode(w,src_mode);
  x_pixrect* im = (x_pixrect*)prect;

  //cout << "pixrect: w = " << im->w << "  h = " << im->h << endl;

  if (im->w == 0 || im->h == 0) {
     cout << "EMPTY PIXRECT" << endl;
     return;
  }


  y -= (im->h - 1);

  x -= im->anchor_x;
  y += im->anchor_y;

  if (im->mask != None)
  { XSetClipOrigin(display,wp->gc,x,y);
    XSetClipMask(display,wp->gc,im->mask);
   }

  if (wp->clip_mask != None) {
    XSetClipOrigin(display,wp->gc,0,0);
    XSetClipMask(display,wp->gc,wp->clip_mask);
  }


  XSetFunction(display,wp->gc,GXcopy);
  XCopyArea(display,im->P,wp->win,wp->gc,0,0,im->w,im->h,x,y);

  XFlush(display);

  x_set_mode(w,save);

  if (im->mask != None) XSetClipMask(display,wp->gc,None);
 }


void x_insert_pixrect(int w, int x, int y, char* prect, int x0, int y0, int wi, int he)
{ x11_win* wp = wlist[w];
  drawing_mode save = x_set_mode(w,src_mode);
  x_pixrect* im = (x_pixrect*)prect;

  y -= (he - 1);

  x -= im->anchor_x;
  y += im->anchor_y;

  XSetFunction(display,wp->gc,GXcopy);
  XCopyArea(display,im->P,wp->win,wp->gc,x0,y0,wi,he,x,y);

  XFlush(display);
  x_set_mode(w,save);
 }


void x_insert_pixrect(int w, int x, int y, int wi, int he, char* prect)
{ 
  // scale pixrect into box (x,y,x+wi,y-he) (upper left corner)

  // dont know how to do this with x11
  // we center prect into box instead (if phi == 0)

  x_pixrect* im = (x_pixrect*)prect;

  if (wi == im->w && he == im->h && im->phi == 0 && im->alpha == -1)
  { x_insert_pixrect(w,x,y,prect);
    return;
   }

/*
  if (im->phi != 0) prect = x_rotate_pixrect(w,prect,im->phi);
  if (wi != im->w || he != im->h) prect = x_resize_pixrect(w,prect,wi,he);
*/

  if (wi != im->w || he != im->h || im->phi != 0) {
     prect = x_resize_rotate_pixrect(w,prect,wi,he,im->phi);
  }

  float a = 1;
  if (im->alpha > 0) a = im->alpha/255.0f;

  if (a != 1)
  { char* wprect = x_create_pixrect(w,x,y-he,x+wi-1,y+1); // upper left corner !
    prect = x_blend_pixrect(w,wprect,prect,a);
  }


  // lower left corner !
  x_insert_pixrect(w,x,y,prect);


/*
  int dx = (wi - im->w)/2;
  int dy = (he - im->h)/2;
  x_insert_pixrect(w,x+dx,y-dy,prect);
*/

 }




unsigned char* x_pixrect_to_bgra(int w, char* prect)
{ x_pixrect* im = (x_pixrect*)prect;
  int width = im->w;
  int height = im->h;

  XImage* I = XGetImage(display,im->P,0,0,width,height,AllPlanes,ZPixmap);

  int sz = width * height;

  unsigned char* buf = new unsigned char[4*sz];
  unsigned int* p = (unsigned int*)I->data;
  unsigned int* q = (unsigned int*)buf;

  for(int i=0; i<sz; i++) {
/*   // alpha always null ?
     assert((p[i] & 0xff000000) == 0);
*/
     unsigned int pix = p[i] & 0xffffff;

     q[i] = (pix == TRANSPARENT) ? 0x00ffffff : (pix | 0xff000000);
  }


  XDestroyImage(I);
  return buf;
}



void x_delete_pixrect(char* prect)
{ x_pixrect* im = (x_pixrect*)prect;
  if (im)
  { XFreePixmap(display,im->P);
    if (im->mask != None) XFreePixmap(display,im->mask);
    delete im;
   }
 }



char* x_create_bitmap(int w, int width, int height, unsigned char* bits, int f)
{ x11_win* wp = wlist[w];

  if (f == 1)
  { x_pixrect* im = new x_pixrect(width,height);
    im->P = XCreateBitmapFromData(display,wp->win,(char*)bits,width,height);
    return (char*)im;
   }

  // scale  factor f
  int bw1 = (width+7)/8;

  int w2 = f*width;
  int h2 = f*height;
  int bw2 = (w2+7)/8;

  unsigned char* bits2 = new unsigned char[bw2*h2];

  for(int i=0; i<h2; i++)
    for(int j=0; j<bw2; j++)
    { char x = bits[(i/f)*bw1 + j/f]; // source byte
      int y = 0;
      for(int k = 0; k<8; k++) {
        if (x & (1<<k)) y |=  ((1<<f)-1) << f*k;
      }
      unsigned char* z = (unsigned char*)&y;
      bits2[i*bw2 + j] = z[j%f];
     }

   x_pixrect* im = new x_pixrect(w2,h2);
   im->P = XCreateBitmapFromData(display,wp->win,(char*)bits2,w2,h2);

   delete[] bits2;

   return (char*)im;

}


/*
char* x_pixrect_to_bitmap(int w, char* pr)
{ x11_win* wp = wlist[w];
  drawing_mode save = x_set_mode(w,src_mode);

  x_pixrect* im = (x_pixrect*)pr;
  int wi = im->w;
  int he = im->h;

  Pixmap bm = XCreatePixmap(display,wp->win,wi,he,1);

  XGCValues  gc_val;
  gc_val.background = 0;
  gc_val.foreground = 1;
  gc_val.function  = GXcopy; 

  GC gc = XCreateGC(display,bm,
                     GCBackground | GCForeground | GCFunction, &gc_val);

  XCopyPlane(display,im->P,bm,gc,0,0,wi,he,0,0,1);

  gc_val.function = GXor;
  XChangeGC(display,gc,GCFunction,&gc_val);
  XCopyPlane(display,im->P,bm,gc,0,0,wi,he,0,0,2);
  XCopyPlane(display,im->P,bm,gc,0,0,wi,he,0,0,4);
  XCopyPlane(display,im->P,bm,gc,0,0,wi,he,0,0,8);

  XFreeGC(display,gc);

  x_set_mode(w,save);

  x_pixrect* im1 = new x_pixrect(wi,he);
  im1->P = bm;


  return (char*)im1;
}
*/


/*
char* x_create_bitmap(int w, int x1, int y1, int x2, int y2)
{ x11_win* wp = wlist[w];
  drawing_mode save = x_set_mode(w,src_mode);

  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);

  x_pixrect* im = new x_pixrect(x2-x1+1,y2-y1+1);
  im->P = XCreatePixmap(display,wp->win,im->w,im->h,1);

  XGCValues  gc_val;
  gc_val.background = WhitePixel(display,screen);
  gc_val.foreground = BlackPixel(display,screen);
  gc_val.function  = GXcopy; 
  GC gc = XCreateGC(display,im->P,
                     GCBackground | GCForeground | GCFunction, &gc_val);

  XCopyPlane(display,wp->win,im->P,gc,x1,y1,im->w,im->h,0,0,1);
  gc_val.function = GXor;
  XChangeGC(display,gc,GCFunction,&gc_val);
  XCopyPlane(display,wp->win,im->P,gc,x1,y1,im->w,im->h,0,0,2);
  XCopyPlane(display,wp->win,im->P,gc,x1,y1,im->w,im->h,0,0,4);
  XCopyPlane(display,wp->win,im->P,gc,x1,y1,im->w,im->h,0,0,8);

  XFreeGC(display,gc);

  x_set_mode(w,save);

  return (char*)im;
}
*/




void x_insert_bitmap(int w, int x, int y, char* bm)
{ x11_win* wp = wlist[w];
  x_pixrect* im = (x_pixrect*)bm;
  y -= (im->h - 1);
  XSetClipOrigin(display,wp->gc,x,y);
  XSetClipMask(display,wp->gc,im->P);
  x_box(w,x,y,x+im->w-1,y+im->h-1);
  XSetClipMask(display,wp->gc,None);
}


void x_delete_bitmap(char* bmap) 
{ /*
  x_pixrect* im = (x_pixrect*)bmap;
  if (im)
  { XFreePixmap(display,im->P);
    delete im;
   }
   */
   x_delete_pixrect(bmap);
 }



//------------------------------------------------------------------------------
// fonts
//------------------------------------------------------------------------------


static void x_set_xft_font(int w, XftFont* xft_font, int xft_sz)
{ x11_win* wp = wlist[w];
  wp->xft_font = xft_font; 
  wp->xft_size = xft_sz; 
}


void x_set_text_font(int w) { 
  x_set_xft_font(w,xft_text_font,xft_text_sz); 
}

void x_set_italic_font(int w) { 
  x_set_xft_font(w,xft_italic_font,xft_italic_sz); 
}

void x_set_bold_font(int w) { 
  x_set_xft_font(w,xft_bold_font,xft_bold_sz); 
}

void x_set_fixed_font(int w) { 
  x_set_xft_font(w,xft_fixed_font,xft_fixed_sz); 
}

void x_set_button_font(int w) { 
  x_set_xft_font(w,xft_button_font,xft_button_sz); 
}


int x_set_font(int w, const char *fname)
{ if (display==NULL) return 0;

  x11_win* wp = wlist[w];
  XftFont* fp = x_load_xft_font(fname,&wp->xft_size);
  wp->xft_font = fp;
  return fp != NULL;

}

void x_save_font(int w)
{ x11_win* wp = wlist[w];
  wp->xft_last_font = wp->xft_font;
  wp->xft_last_size = wp->xft_size;
}

void x_restore_font(int w)
{ x11_win* wp = wlist[w];
  wp->xft_font = wp->xft_last_font;
  wp->xft_size = wp->xft_last_size;
}


//------------------------------------------------------------------------------
// setting parameters
//------------------------------------------------------------------------------

int x_set_cursor(int w, int id)
{ x11_win* wp = wlist[w];
  Window win = (wp->win_save) ? wp->win_save : wp->win;
  if (wp->cursor_id == id) return id;
  if (wp->cursor_id >= 0) XFreeCursor(display,wp->cursor);
  if (id >= 0)
  { wp->cursor = XCreateFontCursor(display,id);
    XDefineCursor(display,win,wp->cursor);
   }
  else
    XUndefineCursor(display,win);

  XFlush(display);

  int old_id = wp->cursor_id;
  wp->cursor_id = id;
  return old_id;
}


void x_set_border_width(int w, int width)
{ x11_win* wp = wlist[w];
  XWindowChanges changes;
  changes.border_width = width;
  if (wp->win_save)
    XConfigureWindow(display,wp->win_save,CWBorderWidth,&changes);
  else
    XConfigureWindow(display,wp->win,CWBorderWidth,&changes);
}

void x_set_border_color(int w, int col) 
{ x11_win* wp = wlist[w];
  if (wp->win_save)
    { XSetWindowBorder(display,wp->win_save,col);
      wp->BORDER_COLOR_SAVE = col;
     }
  else
    { XSetWindowBorder(display,wp->win,col);
      wp->BORDER_COLOR = col;
     }
}
 
 
void x_set_label(int w, const char* label)
{ x11_win* wp = wlist[w];
  if (wp->win_save)
    XStoreName(display,wp->win_save,label); 
  else
    XStoreName(display,wp->win,label); 
  XFlush(display);
}

 
void x_set_icon_label(int w, const char* label)
{ x11_win* wp = wlist[w];
  if (wp->win_save)
    XSetIconName(display,wp->win_save,label);
  else
    XSetIconName(display,wp->win,label);
}




int x_set_bg_color(int w, int col)
{ x11_win* wp = wlist[w];
  int save = wp->BG_COLOR;
  wp->BG_COLOR = col;
  Window win = (wp->win_save) ? wp->win_save : wp->win;
  XSetWindowBackground(display,win,wp->BG_COLOR);
  return save;
 }


char* x_set_bg_pixmap(int w, char* prect)
{ x11_win* wp = wlist[w];
  char* save = wp->BG_PIXMAP;
  wp->BG_PIXMAP = prect;
  Window win = (wp->win_save) ? wp->win_save : wp->win;
  if (prect)
  { Pixmap pm = ((x_pixrect*)prect)->P;
    XSetWindowBackgroundPixmap(display,win,pm);
   }
  else
    XSetWindowBackground(display,win,wp->BG_COLOR);
  return save;
 }

void x_set_bg_origin(int w, int xorig, int yorig)
{ x11_win* wp = wlist[w];
  wp->gc_val.ts_x_origin = xorig;
  wp->gc_val.ts_y_origin = yorig;
  XChangeGC(display,wp->gc,GCTileStipXOrigin|GCTileStipYOrigin,&(wp->gc_val));
}


int x_set_color(int w, int col)
{ x11_win* wp = wlist[w];
  int save = wp->COLOR;
  wp->COLOR = col;
  if (wp->MODE == xor_mode) 
    wp->gc_val.foreground = wp->COLOR ^ 0xffffff;
  else
    wp->gc_val.foreground = wp->COLOR;
  XChangeGC(display,wp->gc,GCForeground,&(wp->gc_val));
  return save;
}


void x_set_stipple(int w, char* bits, int col)
{ x11_win* wp = wlist[w];
  if (bits)
   { Pixmap stip_pm = XCreateBitmapFromData(display,wp->win,bits,16,16);
     wp->gc_val.stipple = stip_pm;
     wp->gc_val.background = col;
   //wp->gc_val.fill_style = FillOpaqueStippled;
     wp->gc_val.fill_style = FillStippled;
     XChangeGC(display,wp->gc,GCBackground|GCStipple|GCFillStyle,&(wp->gc_val));
    }
  else
   { wp->gc_val.background = wp->BG_COLOR;
     wp->gc_val.fill_style = FillSolid;
     XChangeGC(display,wp->gc,GCBackground|GCFillStyle,&(wp->gc_val));
    }
}



drawing_mode x_set_mode(int w, drawing_mode m)
{ x11_win* wp = wlist[w];

  if (wp ==0)
  { cout << "x_set_mode(null)" << endl;
    return src_mode;
  }

  if (wp->MODE == m) return m;

  drawing_mode save = wp->MODE;

  wp->MODE = m;

  wp->gc_val.foreground = wp->COLOR;

  switch (m)  {
   case src_mode: wp->gc_val.function = GXcopy;
                   break;
   case or_mode:  wp->gc_val.function = GXxor;
                  break;
   case and_mode: wp->gc_val.function = GXand;
                  break;
   case xor_mode: wp->gc_val.function = GXor;
                  wp->gc_val.foreground ^= 0xffffff;
                  break;
   default: break;
  }

  XChangeGC(display,wp->gc,GCFunction,&(wp->gc_val));

  return save;
}


text_mode x_set_text_mode(int w, text_mode tm) 
{ x11_win* wp = wlist[w];
  text_mode save = wp->TEXTMODE;
  wp->TEXTMODE = tm;  
  return save;
 }

int x_set_join_style(int w, int js) 
{ x11_win* wp = wlist[w];
  int save = wp->JOINSTYLE;
  wp->JOINSTYLE = js;  
  return save;
 }



int x_set_line_width(int w, int lw)
{ x11_win* wp = wlist[w];
  if (wp->LINEWIDTH == lw) return lw;
  int save = wp->LINEWIDTH;
  wp->LINEWIDTH = lw;
  wp->gc_val.line_width = lw;
  XChangeGC(display,wp->gc,GCLineWidth,&(wp->gc_val));
  return save;
}


line_style x_set_line_style(int w, line_style s)
{ x11_win* wp = wlist[w];
  if (wp->LINESTYLE == s) return s;
  line_style save = wp->LINESTYLE;

  wp->LINESTYLE = s;

  switch (s)  {

   case solid  : wp->gc_val.line_style = LineSolid;
                 break;
   case dashed : wp->gc_val.line_style = LineOnOffDash;
                 XSetDashes(display,wp->gc,0,dash_mask,2);
                 break;
   case dotted : wp->gc_val.line_style = LineOnOffDash;
                 XSetDashes(display,wp->gc,0,dot_mask,2);
                 break;
   case dashed_dotted : 
                 wp->gc_val.line_style = LineOnOffDash;
                 XSetDashes(display,wp->gc,0,dash_dot_mask,4);
                 break;
   }

  XChangeGC(display,wp->gc,GCLineStyle,&(wp->gc_val));
  return save;
}


int           x_get_color(int w)      { return wlist[w]->COLOR;     }
drawing_mode  x_get_mode(int w)       { return wlist[w]->MODE;      }
int           x_get_line_width(int w) { return wlist[w]->LINEWIDTH; }
line_style    x_get_line_style(int w) { return wlist[w]->LINESTYLE; }
text_mode     x_get_text_mode(int w)  { return wlist[w]->TEXTMODE;  }
int           x_get_cursor(int w)     { return wlist[w]->cursor_id; }
int           x_get_resizing(int w)   { return wlist[w]->resizing;  }



int x_get_border_width(int w)
{ x11_win* wp = wlist[w];
  XWindowAttributes attributes;
  if(wp->win_save) 
     XGetWindowAttributes(display,wp->win_save,&attributes);
  else
     XGetWindowAttributes(display,wp->win,&attributes);
  return attributes.border_width;
}

int x_get_border_color(int w)
{ x11_win* wp = wlist[w];
  if(wp->win_save)
     return wp->BORDER_COLOR_SAVE;
  else
     return wp->BORDER_COLOR;
}


//------------------------------------------------------------------------------
// tooltips
//------------------------------------------------------------------------------

static void close_tt_window(int w) {
  x11_win* wp = wlist[w];
  x11_tooltip* ttp = wp->ttp_open;
  if (ttp == 0) return;

//cout << "close: " << ttp << " s = " << ttp->get_line(0)  << endl;

  x_close_window(ttp->win);
  wp->ttp_open = 0;
}

static void open_tt_window(int w, int x, int y, x11_tooltip* ttp)
{ x11_win* wp = wlist[w];
  if (ttp == wp->ttp_open) return;

  const char* s = ttp->get_line(0);

//cout << "open: " << x << "  " << y << " s = " << s  << endl;

  close_tt_window(w);

  x_set_text_font(w);

/*
  int width = x_text_width(w,s) + 20;
  int height = x_text_height(w,s) + 15;
*/
  int width = x_text_width(w,s) + 15;
  int height = x_text_height(w,s) + 5;

  x_restore_font(w); 

  int bg_clr = 0xffffe7;
  const char* header = "tooltip";
  const char* label = "tooltip";
  bool hidden = false;

  ttp->win = x_create_window(0,width,height,bg_clr,header,label,w,0);
  x_open_window(ttp->win,x+15,y+25,width,height,w,hidden);

  x_set_color(ttp->win,bg_clr);
  x_box(ttp->win,0,0,width,height);

  x_set_color(ttp->win,0x555555);
  x_set_text_font(ttp->win);
  x_ctext(ttp->win,width/2,height/2-1,s);

  wp->ttp_open = ttp;
}


static void check_tooltips(int w, int x, int y)
{ x11_win* wp = wlist[w];

  int i = wp->tt_top;
  while (i >= 0)
  { x11_tooltip* ttp = wp->tt_stack[i];
    if (x <= ttp->xmax && y <= ttp->ymax &&
        x >= ttp->xmin && y >= ttp->ymin) break;
    i--;
   }

  x11_tooltip* ttp = (i >= 0) ? wp->tt_stack[i] : 0;
  wp->ttp_current = ttp;
  wp->ttp_x = x;
  wp->ttp_y = y;

  if (ttp == 0) close_tt_window(w);
}


void x_set_tooltip(int win, int id, int x0, int y0, int x1, int y1, 
                                                             const char* msg) 
{ x11_win* wp = wlist[win];

//cout << "tooltip: " << msg << endl;

  int i = wp->tt_top;
  while (i >= 0 && wp->tt_stack[i]->id != id) i--; 

  if (i >= 0) {
    // existing tooltip with this id (do not overwrite)
    return; 
  }

  // push new tooltip

  int t = wp->tt_top + 1;
  if (t < TOOLTIP_MAX)
  { wp->tt_stack[t] = new x11_tooltip(win,id,x0,y0,x1,y1,msg);
    wp->tt_top = t;
   }
}


void  x_del_tooltip(int win, int id) 
{ x11_win* wp = wlist[win];

  int i = wp->tt_top;
  while (i >= 0 && wp->tt_stack[i]->id != id) i--; 

  if (i >= 0) 
  { x11_tooltip* ttp = wp->tt_stack[i];
    if (ttp == wp->ttp_open) close_tt_window(win);
    delete ttp;
    wp->tt_stack[i] = wp->tt_stack[wp->tt_top];
    wp->tt_top--;
   }
}
  

//------------------------------------------------------------------------------
// event handling
//------------------------------------------------------------------------------


void start_tracing_events(int w)
{ 
  int rel = vendor_rel;

  printf("START TRACING EVENTS\n");
  printf("\n");
  printf("VENDOR:  %s\n", XServerVendor(display));
  printf("RELEASE: %d\n",rel);

  printf("X11 VERSION: %d.%d.%d\n",rel/10000000,
                               (rel/100000)%100,
                               (rel/1000)%100);
  printf("\n");

  printf("XFT VERSION: %d\n",XFT_VERSION);
  printf("\n");

  printf("DISPLAY WIDTH = %d\n", 
                   DisplayWidth(display,screen)); 
  printf("DISPLAY HEIGHT = %d\n", 
                   DisplayHeight(display,screen)); 
  printf("DISPLAY DEPTH = %d\n", 
                   DefaultDepth(display,screen)); 
  printf("\n");

  XWindowAttributes attrib;
  x11_win* wp = wlist[w];
  if(wp->win_save) 
     XGetWindowAttributes(display,wp->win_save,&attrib);
  else
     XGetWindowAttributes(display,wp->win,&attrib);

  //switch (XDoesBackingStore(XScreenOfDisplay(display,screen)))
  switch (attrib.backing_store) {
    case WhenMapped: printf("BackingStore = WhenMapped\n"); break;
    case NotUseful:  printf("BackingStore = NotUseful\n"); break;
    case Always:     printf("BackingStore = Always\n"); break;
    default:         printf("BackingStore = ???\n");
   } 

  if (XDoesSaveUnders(XScreenOfDisplay(display,screen)) == True)
    printf("XDoesSaveUnders = True\n");
  else
    printf("XDoesSaveUnders = False\n");

  printf("window_bits_saved = %d\n", x_window_bits_saved(w));
  printf("\n"); 
  trace_events = 1;
}


static int handle_event(int& w, int& x, int& y, int& val1, int&val2, 
                                                           unsigned long& t)
{
  KeySym keysym;
  XComposeStatus status;

  int  kind = no_event;

  // find w (window of event)

  Window win = event.xany.window;

  wlist[0]->win = win; //stopper

  //int i = wcount-1;
  int i = wcount;
  while (wlist[i] == 0 || 
         (wlist[i]->win != win && wlist[i]->win_save != win)) i--;

  wlist[0]->win = root_window;

  w = i;

  x = 0;
  y = 0;
  val1 = 0;
  val2 = 0;
  t = 0;



  switch (event.type) {


  case ClientMessage:
                { // window [x] button clicked
                  Atom type_at = event.xclient.message_type;
                  Atom data_at = event.xclient.data.l[0];
                  if (type_at == wm_protocols && data_at == wm_delete_window) 
                  { //printf("delete window %d\n",w);
                    kind = destroy_event;
                   }
                  break;
                 }

  case Expose:  //printf("expose: count = %d\n",event.xexpose.count);
                kind = exposure_event;
                x = event.xexpose.x;
                y = event.xexpose.y;
                val1 = event.xexpose.width;
                val2 = event.xexpose.height;
                break;

  case FocusIn: 
  case FocusOut: 
               { /*
                  printf("%s  mode = %d\n",
                    x11_event_name[event.type],event.xfocus.mode);
                  fflush(stdout);
                 */
              
                 if (event.type == FocusOut && event.xfocus.mode != 1) break;

                 int r = (event.type == FocusIn) ? 0 : 1;

                 if (r != wlist[i]->resizing)
                 { wlist[i]->resizing = r;
                   kind = exposure_event;
                   x = 0;
                   y = 0;
                   val1 = x_window_width(w);
                   val2 = x_window_height(w);
                  }

                 break;
               }


  case ResizeRequest: //printf("ResizeReques Event\n");fflush(stdout);
                      break;


  case ConfigureNotify: kind = configure_event;
                      x = event.xconfigure.x;
                      y = event.xconfigure.y;
                      if (x == 0 && y == 0) x_window_to_screen(w,x,y);
                      break;


  case DestroyNotify: kind = destroy_event;
                      //printf("destroy notify: w = %d\n",w);
                      break;


  case ButtonPress: val1 = event.xbutton.button;
                    x = event.xbutton.x;
                    y = event.xbutton.y;
                    t = event.xbutton.time;
                    kind = button_press_event;
                    if (event.xbutton.state & ShiftMask)   val1 |= 256; 
                    if (event.xbutton.state & ControlMask) val1 |= 512;
                    if (event.xkey.state & Mod1Mask)       val1 |= 1024; // alt
                    if (event.xkey.state & Mod4Mask)       val1 |= 1024; // sun
                    //XUngrabPointer(display,CurrentTime);
                    //x_ungrab_pointer();
                    break;

  case ButtonRelease: 
                    val1 = event.xbutton.button;
                    x = event.xbutton.x;
                    y = event.xbutton.y;
                    t = event.xbutton.time;
                    if (event.xbutton.state & ShiftMask)   val1 |= 256; 
                    if (event.xbutton.state & ControlMask) val1 |= 512;
                    if (event.xkey.state & Mod1Mask)       val1 |= 1024; // alt
                    if (event.xkey.state & Mod4Mask)       val1 |= 1024; // sun
                    kind = button_release_event;
                    break;

  case LeaveNotify:
  case EnterNotify:
  case MotionNotify: x = event.xmotion.x;
                     y = event.xmotion.y;
                     t = event.xbutton.time;
                     kind = motion_event;
                     check_tooltips(w,x,y);
                     break;

  case KeyRelease: 
  case KeyPress: { x = event.xkey.x;
                   y = event.xkey.y;
                   t = event.xkey.time;

                   //char c = 0;
                   //XLookupString(&event.xkey,&c,1, &keysym, &status);

                   char buf[4];
                   XLookupString(&event.xkey,buf,4, &keysym, &status);
                   char c = buf[0];

                   if (c == 0) c = char(-1);
  
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
  
                     case XK_F10: if (event.type != KeyPress) break;
                                  if (trace_events)
                                  { printf("STOP TRACING EVENTS\n");
                                    trace_events = 0;
                                   }
                                  else
                                    start_tracing_events(w);
                                  break;
  
 /* 
                     default:  if (c == 3) {
                                // CTRL-C pressed
                                kind = destroy_event;
                               }
*/
                               break;
                          
                    }

                   if (c == char(-1) || kind == destroy_event) break;

                   val1 = c;
  
                   if (event.type == KeyPress) 
                     kind = key_press_event;
                   else
                     kind = key_release_event;
  
                   if (event.xkey.state & ShiftMask)   val1 |= 256; 
                   if (event.xkey.state & ControlMask) val1 |= 512;
                   if (event.xkey.state & Mod1Mask)    val1 |= 1024; // alt
                   if (event.xkey.state & Mod4Mask)    val1 |= 1024; // sun
                   break;
                 }

  
  case MappingNotify:
                 { XMappingEvent* mapping_event = (XMappingEvent*)&event; 
                   if (mapping_event->request == MappingKeyboard || 
                       mapping_event->request == MappingModifier) 
                          XRefreshKeyboardMapping(mapping_event);
                    break;
                 }

  case ReparentNotify:
                 { 
/*
                   Window ww = ((XReparentEvent*)&event)->window;
                   Window pp = ((XReparentEvent*)&event)->parent;
                   int wi = 0, pi = 0;
                   for(int i=1; i<=wcount; i++) {
                      if (wlist[i]->win == ww) wi = i;
                      if (wlist[i]->win == pp) pi = i;
                   }
                   printf("ReparentNotify: win = %d   parent = %d\n",wi,pi);
                   fflush(stdout);
*/
                   break;
                  }

  }

  if (trace_events  && kind != motion_event) {
   printf("%s (%s): win = %d  w = %d  x = %3d  y = %3d  val1 = %d  val2 = %d  t = %lu\n", 
           x11_event_name[event.type],event_name[kind],int(win),w,x,y,val1,val2,t);

/*
   if (kind == configure_event)
   {
     Window w_above = event.xconfigure.above;
     printf("win = %d above = %d  event = %d  top = %d  root = %d\n",
            (int)event.xconfigure.window, (int)w_above, 
            (int)event.xconfigure.event, (int)top_window, (int)root_window);
     
     if (w_above != None && w_above == top_window) {
       printf("raise: %d\n",(int)w_above);
       XRaiseWindow(display,w_above);
      }
   }
*/

  }


  if (kind == no_event && w == 0) w = 1;




  return kind;
}



#if defined(__unix__)

#if 0
static int check_device_change(int fd, char* device_name)
{ 
  if (fd < 0) return -1;

  char buf[512];
  int len = read (fd,buf,512);

  int action = -1;  // 1: connect,  0: disconnect
  char* name = 0;

   int i = 0;
   while (i < len && action == -1)
   { inotify_event* pevent = (inotify_event*)(buf+i);
 
     //printf("name = %s  mask = %04x\n", pevent->name, pevent->mask); 
     //fflush(stdout);

     name = pevent->name;

     if (pevent->mask & IN_DELETE) 
     { //printf("IN_DELETE: %s\n",name); fflush(stdout);
       if (action == -1) action = 0;
      }

     if (pevent->mask & IN_CREATE) 
     { //printf("IN_CREATE: %s\n",name); fflush(stdout);
       if (action == -1) action = 1;
      }

/*
     if (pevent->mask & IN_DELETE_SELF)
     { printf("IN_DELETE_SELF: update watch\n"); fflush(stdout);
       //inotify_add_watch(fd,"/etc/mtab", filter);
      }

     if (pevent->mask & IN_ATTRIB)
     { //printf("IN_ATTRIB: update watch\n"); fflush(stdout);
       inotify_add_watch(fd,"/etc/mtab", filter);
      }

     if (pevent->mask & IN_CLOSE_WRITE) 
     { //printf("IN_CLOSE_WRITE\n"); fflush(stdout);
       if (action == -1) action = 2;
      }
*/

     i += sizeof(struct inotify_event) + pevent->len;
    }

   device_name[0] = '\0';

   if (action == 0 || action == 1) 
     sprintf(device_name,"/dev/bus/usb/001/%s",name);
   
/*
   if (action == 2) 
     sprintf(device_name,"/etc/mtab");
*/
  
   return action;
}

#endif

void get_device_event(int fd, int& action, int& vid, int& pid)
{
  int busnum = -1;
  int devnum = -1;

  action = -1;
  vid = -1;
  pid = -1;

#if defined(__linux__)
  char buf[512];
  int len = recv(fd, buf, 512, MSG_WAITALL);

  char* stop = buf + len;
  for(char* p = buf; p < stop; p++)
  {  if (strncmp(p,"ACTION=",7) == 0) 
     { if (strcmp(p+7,"add") == 0) action = 1;
       else
       if (strcmp(p+7,"remove") == 0) action = 0;
      }
     else
     if (strncmp(p,"BUSNUM=",7) == 0) busnum = atoi(p+7);
     else
     if (strncmp(p,"DEVNUM=",7) == 0) devnum = atoi(p+7);
     else
     if (strncmp(p,"PRODUCT=",8) == 0) 
     { int x = 0;
       sscanf(p+8,"%x/%x/%x",&vid,&pid,&x);
      }

     while (*p != '\0') p++;
   }
#endif

   if (devnum == -1 || busnum == -1) action = -1;
}

#endif



int x_get_next_event(int& w, int& x, int& y, int& val1, int& val2,
                                                        unsigned long& t, 
                                                        int msec)
{ 
  // blocking: msec = 0
  // timeout:  msec > 0
  // non-blocking(check):  msec < 0

  if (msec < 0)
  { if (XCheckMaskEvent(display, 
                        EnterWindowMask | LeaveWindowMask    |
                        KeyPressMask    | KeyReleaseMask     |
                        ButtonPressMask | ButtonReleaseMask  |
                        PointerMotionMask |  FocusChangeMask | 
                        ExposureMask    | StructureNotifyMask, &event) == 0) 
    { w = 0;
      return no_event; 
     }
    else
      return handle_event(w,x,y,val1,val2,t);
  }


  // handle tooltips (if blocking)

  if (msec == 0)
  { 
    int event = x_get_next_event(w,x,y,val1,val2,t,100);
    if (event != no_event) return event;

    x11_win* wp = wlist[w];

    for(;;) {
      unsigned long delay = wp->ttp_open ? 1500 : 400;
      event = x_get_next_event(w,x,y,val1,val2,t,delay);
      if (event != no_event) return event;

      if (wp->ttp_current && wp->ttp_current != wp->ttp_open)
         open_tt_window(w,wp->ttp_x,wp->ttp_y,wp->ttp_current);
      else
      { close_tt_window(w);
        wp->ttp_current = 0;
       }
    }

  }



  if (XPending(display) > 0) 
  { // handle display events if available
    XNextEvent(display, &event);
    return handle_event(w,x,y,val1,val2,t);
   }


  int fd_disp = ConnectionNumber(display);

#if defined(__linux__)
  if (fd_dev == -1)  
  { // usb device file descriptor    
    fd_dev = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    memset(&nls_dev,0,sizeof(sockaddr_nl));
    nls_dev.nl_family = AF_NETLINK;
    nls_dev.nl_pid = getpid();
    nls_dev.nl_groups = -1;
    bind(fd_dev, (sockaddr*)&nls_dev, sizeof(sockaddr_nl));
  }
#endif


  if (fd_mnt ==-1)  
  { // mount file descriptor    
    fd_mnt = open("/proc/mounts",O_RDONLY);
/*
    if (fd_mnt == -1)
      cout << "could not open mounts-file" << endl;
*/
   }


  int e = no_event;

  fd_set rdset, wrset, xset;

  FD_ZERO(&rdset);
  FD_ZERO(&wrset);
  FD_ZERO(&xset);

  if (fd_dev != -1) FD_SET(fd_dev, &rdset);

  FD_SET(fd_disp,&rdset);

  if (fd_mnt != -1) FD_SET(fd_mnt, &xset);

  int fd_max = fd_disp;
  if (fd_dev > fd_max) fd_max = fd_dev;
  if (fd_mnt > fd_max) fd_max = fd_mnt;

  if (msec > 0)
  { timeval polltime;
    polltime.tv_sec  = msec / 1000; 
    polltime.tv_usec = 1000 * (msec % 1000);
    // with timeout
    select(fd_max+1,&rdset,&wrset,&xset,&polltime);
   }
   else
   { // blocking
     select(fd_max+1,&rdset,&wrset,&xset,NULL);
    }


  if (FD_ISSET(fd_disp,&rdset))
  { // display event
    XNextEvent(display, &event);
    return handle_event(w,x,y,val1,val2,t);
   }

  if (fd_mnt != -1 && FD_ISSET(fd_mnt,&xset))
  { // mount event
    //printf("fd_mnt: volume_connect_event\n"); fflush(stdout);

    x11_win* wp = wlist[0];
    if (wp->special_event_handler)
    { wp->special_event_handler(wp->inf,"volume","unknown",0,0);
      return no_event; 
     }

    e = volume_connect_event;
    w = 1;
    x = 0;
    y = 0;
    val1 = 0;
    val2 = 0;
    t = 0;
    return e;
   }


  if (fd_dev != -1 && FD_ISSET(fd_dev,&rdset))
  { // device event
/*
    char dev_name[256];
    sprintf(dev_name,"%s","");
    int action = check_device_change(fd_dev,dev_name);
*/
    int a,v,p;
    get_device_event(fd_dev,a,v,p);

    e = no_event;

    if (a != -1 && v != -1 && p != -1)
    { int dev_action = a;
      int dev_vid = v;
      int dev_pid = p;

      x11_win* wp = wlist[0];
      if (wp->special_event_handler)
      { string name = string("VID_%04X&PID_%04X",dev_vid,dev_pid);
        wp->special_event_handler(wp->inf,"interface",name,a,0);
        return no_event;
       }
        
        
/*
      if (a == 0)
        printf("fd_dev: device_disconnect\n"); 
      else
        printf("fd_dev: device_connect\n");
*/
      fflush(stdout);
      e = device_connect_event;
      w = 1;
      x = dev_vid;
      y = dev_pid;
      val1 = dev_action;
      t = 0;
     }
   }

  if (trace_events && e != no_event)
  { printf("%s (%d) x = %d y = %d val1 = %d val2 = %d\n", 
                    event_name[e],e,x,y,val1,val2);
    fflush(stdout);
   }

  return e;
}


void x_put_back_event()
{ if (trace_events) printf("XPutBackEvent(%s)\n",x11_event_name[event.type]);
  XPutBackEvent(display,&event); 
}


int x_create_buffer(int w, int wi, int he)
{ x11_win* wp = wlist[w];

  assert(wi > 0 && he > 0);
  assert(wp->win_save == 0);

  if (wp->buf != 0 && wi == wp->buf_w && he == wp->buf_h) {
    // reuse existing buffer
    return 0;
  }

  // allocate new buffer

  if (wp->buf) XFreePixmap(display,wp->buf);
  wp->buf_w = wi;
  wp->buf_h = he;
  wp->buf = XCreatePixmap(display,wp->win,wi,he,depth);
  ClearPixmap(w,wp->buf,0,0,wi,he,0,0);
  if (wp->win_save) wp->win = wp->buf;

  return 1;
}


int x_create_buffer(int w)
{ int wi = x_window_width(w);
  int he = x_window_height(w);
  return x_create_buffer(w,wi,he);
}

int x_start_buffering(int w, int wi, int he)
{ x11_win* wp = wlist[w];
  x_stop_buffering(w);
  x_delete_buffer(w);
  x_create_buffer(w,wi,he);
  wp->win_save = wp->win;
  wp->win = wp->buf;
  return 1;
}



int x_start_buffering(int w) 
{ x11_win* wp = wlist[w];
  if (wp->win_save) return 0;
/*
  int new_buf = wp->buf == 0;
  if (new_buf) x_create_buffer(w);
*/
  int new_buf = x_create_buffer(w);
  wp->win_save = wp->win;
  wp->win = wp->buf;
  return new_buf;
} 


void x_set_buffer(int w, char* pr) 
{ x11_win* wp = wlist[w];

  if (pr == 0) 
  { if (wp->win_save2)
    { wp->win = wp->win_save2;
      wp->win_save2 = 0;
     }
   }
  else
  { if (wp->win_save2)
    { fprintf(stderr,"x_set_buffer: nested call.");
      abort();
     }
    x_pixrect* im = (x_pixrect*)pr;
    wp->win_save2 = wp->win;
    wp->win =  im->P;
   }
} 


int x_test_buffer(int w)
{ x11_win* wp = wlist[w];
  return wp->win_save != 0;
 } 

void x_flush_buffer(int w, int x1, int y1, int x2, int y2, int xoff, int yoff) 
{ x11_win* wp = wlist[w];
  if (wp->buf == 0) return;

  Window win = wp->win_save;
  if (win == 0) win = wp->win;

  drawing_mode save = x_set_mode(w,src_mode);
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  int wi = x2-x1+1;
  int he = y2-y1+1;

  XSetFunction(display,wp->gc,GXcopy);
  XCopyArea(display,wp->buf,win,wp->gc,x1+xoff,y1+yoff,wi,he,x1,y1);

  XFlush(display);
  x_set_mode(w,save);
}

void x_flush_buffer(int w, int x1, int y1, int x2, int y2) 
{ x_flush_buffer(w,x1,y1,x2,y2,0,0); }


void x_stop_buffering(int w)
{ x11_win* wp = wlist[w];
  if (wp->win_save) wp->win  = wp->win_save;
  wp->win_save = 0;
  x_set_bg_pixmap(w,wp->BG_PIXMAP);
} 

void x_stop_buffering(int w, char** pr)
{ x11_win* wp = wlist[w];
  if (!wp->win_save) 
  { // not in buffering mode
    *pr = 0;
    return;
   }
  int wi = x_window_width(w);
  int he = x_window_height(w);
  x_pixrect* im = new x_pixrect(wi,he);
  im->P = wp->buf;  
  x_stop_buffering(w);
  wp->buf = 0;
  *pr = (char*)im;
} 

void x_delete_buffer(int w)  
{ x11_win* wp = wlist[w];
  x_stop_buffering(w);
  if (wp->buf) XFreePixmap(display,wp->buf);
  wp->buf = 0;
}



void x_start_timer(int, int ) {}
void x_stop_timer(int) {}



//------------------------------------------------------------------------------
// other functions
//------------------------------------------------------------------------------


void x_set_read_gc(int w)
{ XGCValues gc_val;
  gc_val.function = GXxor; 
  gc_val.foreground = BlackPixel(display,screen); 
  gc_val.line_style = LineSolid;
  gc_val.line_width = 1;
  XChangeGC(display,wlist[w]->gc,
            GCForeground|GCFunction|GCLineStyle|GCLineWidth,&gc_val);
  x_flush_display();
}

void x_reset_gc(int w)
{ x11_win* wp = wlist[w];
  XChangeGC(display,wp->gc,
            GCForeground|GCFunction|GCLineStyle|GCLineWidth,&(wp->gc_val));
  x_flush_display();
}



void x_grab_pointer(int w)
{ 
  if (w == 0)
  { XUngrabPointer(display,CurrentTime);
    //XUngrabKeyboard(display,CurrentTime); 
    return;
  }

  x11_win* wp = wlist[w];

  XGrabPointer(display,wp->win,False,ButtonPressMask|ButtonReleaseMask|
                                                     PointerMotionMask, 
               GrabModeAsync,GrabModeAsync,None,None,CurrentTime); 

//XGrabKeyboard(display,wp->win,False,GrabModeAsync,GrabModeAsync,CurrentTime); 
//XSetInputFocus(display,wp->win,RevertToParent,CurrentTime);
 }


void x_set_focus(int w)
{ x11_win* wp = wlist[w];
  if (wp->win_save == 0)
    XSetInputFocus(display,wp->win,RevertToParent,CurrentTime);
  else
    XSetInputFocus(display,wp->win_save,RevertToParent,CurrentTime);
 }


void x_set_icon_pixmap(int w, char* prect)
{ x11_win* wp = wlist[w];
  Window win = wp->icon_win;
  if (win == 0)
  { win = XCreateSimpleWindow(display,root_window, 0, 0, 1, 1, 0,
                                           BlackPixel(display,screen),
                                           BlackPixel(display,screen));
    wp->icon_win = win;

    XWMHints wm_hints;
    wm_hints.icon_window = win;
    wm_hints.flags = IconWindowHint;
    XSetWMProperties(display,wp->win,0,0,0,0,0,&wm_hints,0);

   }
  x_pixrect* im = (x_pixrect*)prect;
  XSetWindowBackgroundPixmap(display,win,im->P);
  XResizeWindow(display,win,im->w,im->h);
}


void x_window_to_screen(int w, int& x, int& y)
{ x11_win* wp = wlist[w];
  Window src_win  = wp->win_save ? wp->win_save : wp->win;
  Window dest_win = root_window;
  Window child_win;
  int x1,y1;
  XTranslateCoordinates(display,src_win,dest_win,x,y,&x1,&y1,&child_win);
  x = x1;
  y = y1;
}


void x_screen_to_window(int w, int& x, int& y)
{ x11_win* wp = wlist[w];
  Window src_win  = root_window;
  Window dest_win = wp->win_save ? wp->win_save : wp->win;
  Window child_win;
  int x1,y1;
  XTranslateCoordinates(display,src_win,dest_win,x,y,&x1,&y1,&child_win);
  x = x1;
  y = y1;
}


void x_set_clip_rectangle(int w, int x, int y, int width, int height)
{ 
  x11_win* wp = wlist[w];
  XRectangle rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = width+1;
  rect.height = height+1;
  XSetClipMask(display,wp->gc,None);
  XSetClipRectangles(display,wp->gc,x,y,&rect,1,0);
}


/*
void x_set_clip_ellipse(int win, int x, int y, int r1, int r2)
{ x11_win* wp = wlist[win];

  int width  = 2*r1+1;
  int height = 2*r2+1;

  Pixmap pm = XCreatePixmap(display,wp->win,width,height,1);

  XGCValues  gc_val;
  gc_val.background = 0;
  gc_val.foreground = 0;
  gc_val.function  = GXcopy; 
  GC gc = XCreateGC(display,pm,
                     GCBackground | GCForeground | GCFunction, &gc_val);

  XFillRectangle(display,pm,gc,0,0,width,height);

  gc_val.foreground = 1;
  XChangeGC(display,gc,GCForeground,&gc_val);

  XFillArc(display,pm,gc,0,0,width,height,0,360*64);

  XSetClipOrigin(display,wp->gc,x-r1,y-r2);
  XSetClipMask(display,wp->gc,pm);
  //XFreePixmap(display,pm);
  XFreeGC(display,gc);
}



void x_set_clip_polygon(int w, int n, int *xcoord, int *ycoord)
{ x11_win* wp = wlist[w];
  XPoint* edges = new XPoint[n];
  int x0 = xcoord[0];
  int x1 = xcoord[0];
  int y0 = ycoord[0];
  int y1 = ycoord[0];

  int i;
  for(i=0;i<n;i++) 
  { int x = xcoord[i];
    int y = ycoord[i];
    if (x < x0) x0 = x;
    if (y < y0) y0 = y;
    if (x > x1) x1 = x;
    if (y > y1) y1 = y;
    edges[i].x = x;
    edges[i].y = y;
   }

  for(i=0;i<n;i++) 
  { edges[i].x -= x0;
    edges[i].y -= y0;
   }

  int width  = x1-x0 + 1;
  int height = y1-y0 + 1;

  Pixmap pm = XCreatePixmap(display,wp->win,width,height,1);


  XGCValues  gc_val;
  gc_val.background = 0;//BlackPixel(display,screen);
  gc_val.foreground = 0;//BlackPixel(display,screen);
  gc_val.function  = GXcopy; 
  GC gc = XCreateGC(display,pm,
                     GCBackground | GCForeground | GCFunction, &gc_val);


  XFillRectangle(display,pm,gc,0,0,width,height);

  gc_val.foreground = 1;//WhitePixel(display,screen);
  XChangeGC(display,gc,GCForeground,&gc_val);

  XFillPolygon(display,pm,gc,edges,n,Nonconvex,CoordModeOrigin);

  XSetClipOrigin(display,wp->gc,x0,y0);
  XSetClipMask(display,wp->gc,pm);
  XFreePixmap(display,pm);
  XFreeGC(display,gc);
  delete[] edges;
}
*/


void x_clip_mask_rectangle(int win,int x0,int y0,int x1,int y1,int mode)
{ int xc[4];
  int yc[4];
  xc[0] = x0; yc[0] = y0;
  xc[1] = x0; yc[1] = y1;
  xc[2] = x1; yc[2] = y1;
  xc[3] = x1; yc[3] = y0;
  x_clip_mask_polygon(win,4,xc,yc,mode);
}


void x_clip_mask_polygon(int w, int n, int* xcoord, int* ycoord, int mode)
{ x11_win* wp = wlist[w];

  // mode = 0:  add to clip mask
  // mode = 1:  subtract
  // mode = 3:  xor1 (rather too little)
  // mode = 4:  xor2 (rather too much)

/*
  if (n == 0)
  { if (wp->clip_mask)
    { XFreePixmap(display,wp->clip_mask);
      wp->clip_mask = None;
     }
    XSetClipMask(display,wp->gc,None);
    return;
   }
*/
      

  XPoint* edges = new XPoint[n];

  if (wp->clip_mask == None)
  { int wi = x_window_width(w);
    int he = x_window_height(w);
    Pixmap pm = XCreatePixmap(display,wp->win,wi,he,1);
    wp->clip_mask = pm;
    XGCValues  gc_val;
    gc_val.foreground = 1;
    gc_val.function  = GXcopy; 
    GC gc = XCreateGC(display,pm, GCForeground | GCFunction, &gc_val);
    XFillRectangle(display,pm,gc,0,0,wi,he);
    XFreeGC(display,gc);
  }

  if (n == 0)
  { n = 5;
    edges = new XPoint[n];
    int wi = x_window_width(w);
    int he = x_window_height(w);
    edges[0].x = 0;  edges[0].y = 0;
    edges[1].x = wi; edges[1].y = 0;
    edges[2].x = wi; edges[2].y = he;
    edges[3].x = 0;  edges[3].y = he;
    edges[4].x = 0;  edges[4].y = 0;
   }
  else
  { edges = new XPoint[n];
    for(int i=0;i<n;i++) 
    { edges[i].x = xcoord[i];
      edges[i].y = ycoord[i];
     }
   }

  Pixmap pm = wp->clip_mask;

  if (mode < 3) {
    XGCValues  gc_val;
    gc_val.foreground = mode;
    gc_val.function  = GXcopy;
    GC gc = XCreateGC(display,pm,GCForeground | GCFunction, &gc_val);
    XFillPolygon(display,pm,gc,edges,n,Nonconvex,CoordModeOrigin);
    XFreeGC(display,gc);
  }
  else { // xor
    int wi = x_window_width(w), he = x_window_height(w);
    Pixmap am = XCreatePixmap(display,wp->win,wi,he,1);
    XGCValues gc_val;

    // clear pixmap
    gc_val.foreground = 0;
    gc_val.function   = GXcopy;
    GC gc_clear = XCreateGC(display,am,GCForeground | GCFunction, &gc_val);
    XFillRectangle(display,am,gc_clear,0,0,wi,he);

    // draw polygon
    gc_val.foreground = 1;
    gc_val.function   = GXcopy;
    GC gc_put = XCreateGC(display,am,GCForeground | GCFunction, &gc_val);
    XFillPolygon(display,am,gc_put,edges,n,Nonconvex,CoordModeOrigin);

    // remove/add edges in pixmap am
    GC& gc_edge = mode == 3 ? gc_clear : gc_put;
    for (int i=0; i < n; ++i) {
      int j = (i+1) % n;
      XDrawLine(display,am,gc_edge,xcoord[i],ycoord[i],xcoord[j],ycoord[j]);
    }
    XFreeGC(display,gc_put);
    XFreeGC(display,gc_clear);

    // xor pixmap am into clipmask
    gc_val.foreground = 1;
    gc_val.function   = GXxor;
    GC gc_xor = XCreateGC(display,pm,GCForeground | GCFunction, &gc_val);
    XCopyArea(display, am, pm, gc_xor, 0, 0, wi, he, 0, 0);
    XFreeGC(display,gc_xor);

    XFreePixmap(display,am);
  }

  XSetClipOrigin(display,wp->gc,0,0);
  XSetClipMask(display,wp->gc,pm);
  delete[] edges;
}


void x_clip_mask_ellipse(int w, int x, int y, int r1, int r2, int mode)
{ x11_win* wp = wlist[w];

  // mode = 0:  add to clip mask
  // mode = 1:  subtract
  // mode = 3:  xor

  if (wp->clip_mask == None)
  { int wi = x_window_width(w);
    int he = x_window_height(w);
    Pixmap pm = XCreatePixmap(display,wp->win,wi,he,1);
    wp->clip_mask = pm;
    XGCValues  gc_val;
    gc_val.foreground = 1;
    gc_val.function  = GXcopy; 
    GC gc = XCreateGC(display,pm, GCForeground | GCFunction, &gc_val);
    XFillRectangle(display,pm,gc,0,0,wi,he);
    XFreeGC(display,gc);
  }

  Pixmap pm = wp->clip_mask;

  XGCValues  gc_val;
  gc_val.foreground = mode < 3 ? mode : 1;
  gc_val.function  = mode < 3 ? GXcopy : GXxor;
  GC gc = XCreateGC(display,pm,GCForeground | GCFunction, &gc_val);
  XFillArc(display,pm,gc,x-r1,y-r2,2*r1+1,2*r2+1,0,360*64); 
  XFreeGC(display,gc);

  XSetClipOrigin(display,wp->gc,0,0);
  XSetClipMask(display,wp->gc,pm);
}

// copied from _point.c
static double compute_angle(int px, int py, int qx, int qy, int rx, int ry)
{
  double dx1 = double(qx - px);
  double dy1 = double(qy - py);
  double dx2 = double(rx - px);
  double dy2 = double(ry - py);

  double norm  = (dx1*dx1+dy1*dy1)*(dx2*dx2+dy2*dy2);

  double cosfi = (dx1*dx2+dy1*dy2) / sqrt(norm);

  if (cosfi >=  1.0 ) return 0;
  if (cosfi <= -1.0 ) return LEDA_PI;
  
  double fi = acos(cosfi);

  if (dx1*dy2 < dy1*dx2) fi = -fi;

  if (fi < 0) fi += 2*LEDA_PI;

  return fi;
}

void x_clip_mask_chord(int w, int x0, int y0, int x1, int y1, int cx, int cy, int r, int mode) 
{ x11_win* wp = wlist[w];

  // mode = 0:  add to clip mask
  // mode = 1:  subtract
  // mode = 3:  xor1 (rather too little)
  // mode = 4:  xor2 (rather too much)

  if (wp->clip_mask == None)
  { int wi = x_window_width(w);
    int he = x_window_height(w);
    Pixmap pm = XCreatePixmap(display,wp->win,wi,he,1);
    wp->clip_mask = pm;
    XGCValues  gc_val;
    gc_val.foreground = 1;
    gc_val.function   = GXcopy; 
    GC gc = XCreateGC(display,pm, GCForeground | GCFunction, &gc_val);
    XFillRectangle(display,pm,gc,0,0,wi,he);
    XFreeGC(display,gc);
  }

  Pixmap pm = wp->clip_mask;

  int angle_start = int( compute_angle(cx, -cy, cx+1, -cy, x0, -y0) / LEDA_PI * 180 * 64 + 0.5);
  int angle_span  = int( compute_angle(cx, -cy, x0, -y0, x1, -y1) / LEDA_PI * 180 * 64 + 0.5);

  if (mode < 3) {
    XGCValues gc_val;
    gc_val.foreground = mode;
    gc_val.function   = GXcopy;
    gc_val.arc_mode   = ArcChord;
    GC gc = XCreateGC(display,pm,GCForeground|GCFunction|GCArcMode,&gc_val);
    XFillArc(display,pm,gc,cx-r,cy-r,2*r,2*r,angle_start,angle_span);
    XFreeGC(display,gc);
  }
  else { // xor
//    int wi = x_window_width(w), he = x_window_height(w), xorg = 0, yorg = 0;
    int wi = 2*r, he = 2*r, xorg = cx-r, yorg = cy-r;
    Pixmap am = XCreatePixmap(display,wp->win,wi,he,1);
    XGCValues gc_val; GC gc;

    // clear pixmap
    gc_val.foreground = 0;
    gc_val.function   = GXcopy;
    gc = XCreateGC(display,am,GCForeground|GCFunction,&gc_val);
    XFillRectangle(display,am,gc,0,0,wi,he);
    XFreeGC(display,gc);

    // draw chord
    gc_val.foreground = 1;
    gc_val.function   = GXcopy;
    gc_val.arc_mode   = ArcChord;
    gc = XCreateGC(display,am,GCForeground|GCFunction|GCArcMode,&gc_val);
    XFillArc(display,am,gc,cx-r-xorg,cy-r-yorg,2*r,2*r,angle_start,angle_span);
    XFreeGC(display,gc);

    gc_val.foreground = mode-3; // remove/add line to am
    gc_val.function   = GXcopy;
    gc = XCreateGC(display,am,GCForeground|GCFunction,&gc_val);
    XDrawLine(display,am,gc, x0-xorg,y0-yorg, x1-xorg,y1-yorg);
    XFreeGC(display,gc);

    // xor pixmap am into clipmask
    gc_val.foreground = 1;
    gc_val.function   = GXxor;
    gc = XCreateGC(display,pm,GCForeground|GCFunction,&gc_val);
    XCopyArea(display, am, pm, gc, 0, 0, wi, he, xorg, yorg);
    XFreeGC(display,gc);

    XFreePixmap(display,am);
  }

  XSetClipOrigin(display,wp->gc,0,0);
  XSetClipMask(display,wp->gc,pm);
}



// moving the pointer

void x_move_pointer(int win, int x, int y)
{ x11_win* wp = wlist[win];
  Drawable w = wp->win;
  if (wp->win_save != 0) w = wp->win_save;
  XEvent e;
  while (XCheckWindowEvent(display, wp->win, PointerMotionMask, &e));
  XWarpPointer(display,None,w,0,0,0,0,x,y);
 }


void x_set_special_event_handler(int, 
                   void (*func)(void*,const char*,const char*,int,int), 
                   unsigned long data) 
{ x11_win* wp = wlist[0]; // root window
  wp->special_event_handler = func;
  wp->special_event_data = data;
}


// clipboard

void  x_text_to_clipboard(int, const char* txt) {
 FILE* fp = popen("/usr/bin/xclip -i","w");
 if (fp) {
   fputs(txt,fp);
   pclose(fp);
 }
}


char* x_text_from_clipboard(int) { 
 char* buf = new char[1024];
 buf[0] = '\0';
 FILE* fp = popen("/usr/bin/xclip -o","r");
 if (fp) {
   if (fgets(buf,1024,fp) == NULL) buf[0] = '\0';
   pclose(fp);
 }
 return buf;
}


// not implemented

void x_send_text(const char*) {}

void  x_set_alpha(int,int)  { }

void  x_pixrect_to_clipboard(int, char*) {}
char* x_pixrect_from_clipboard(int) { return 0; }

void x_open_metafile(int,const char*) {}
void x_close_metafile(int)            {}
void x_load_metafile(int,int,int,int,int,const char*) {}
void x_metafile_to_clipboard(int)     {}

int x_choose_file(int,int,const char*,const char*,const char*, char*,char*)  
{ return 0; }

int x_choose_color(int,int) { return 0; }


LEDA_END_NAMESPACE

