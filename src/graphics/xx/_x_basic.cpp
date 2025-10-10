/*******************************************************************************
+
+  LEDA 7.2  
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
//  basic graphics functions 
//------------------------------------------------------------------------------

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/b_queue.h>
#include <LEDA/graphics/x_window.h>

#include <LEDA/system/assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <LEDA/graphics/pixmaps/leda_icon.xpm>

// fonts
#include "ttf/roman.64"
#include "ttf/bold.64"
#include "ttf/italic.64"
#include "ttf/fixed.48"


LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>

#include "x_image.h"

typedef int Window;

typedef unsigned int pixel;

//--------------------------------------------------------------------------
// device dependent functions
//--------------------------------------------------------------------------

extern int  OPEN_DISPLAY(int& width, int& height, int& depth, int& dpi);

extern void CLOSE_DISPLAY();

extern void UPDATE_DISPLAY(unsigned int* pixels, int w, int h, 
                           int x0, int y0, int x1, int y1, 
                           int x, int y); 
/*
   copy pixel rectangle [x0,y0,x1,y1] from (w x h) pixel buffer (pixels)
   to display at position (x,y)
*/

extern int NEXT_EVENT(string&, int& val, int& x, int& y, unsigned long& t,
                                                         int msec);
// msec > 0: blocking / timeout
// msec = 0: blocking / no timeout
// msec < 0: non-blocking (check)

extern void SET_CURSOR(int id);

extern void SEND_TEXT(string text);


//--------------------------------------------------------------------------

// some basic algebra - applied to integer (pixel) coordinates
// we use doubles to represent these integers with 53 bit precision

inline double TRIANGLE_AREA2(double px, double py, double qx, double qy, 
                                                   double rx, double ry)
{ // 2 * signed area of triangle (p,q,r)
  return (px-qx) * (py-ry) - (py-qy) * (px-rx); 
}


inline bool INSIDE_ELLIPSE(double x,double y, double xc, double yc,
                                              double a,  double b)
{ 
  // test if (x,y) inside ellipse with center (xc,yc) and radii a,b

  double a2 = a*a;
  double b2 = b*b;

  double dx = x-xc;
  double dy = y-yc;

  return dx*dx*b2 + dy*dy*a2 <= a2*b2;
}



//------------------------------------------------------------------------------
// window
//------------------------------------------------------------------------------

#define TOOLTIP_MAX 64 

struct xx_win;

struct xx_tooltip
{ 
  int win;
  int pw;

  int id;
  int xmin,ymin,xmax,ymax;

  int    num_lines;
  char** lines;

  xx_tooltip(int w,int ID, int x0, int y0, int x1, int y1, const char* s)
  : win(0), pw(w), id(ID),xmin(x0),ymin(y0),xmax(x1),ymax(y1)
  { 
    num_lines = 0;
    lines = 0;

    if (strlen(s) == 0) return;

    for(unsigned i=0; i<=strlen(s); i++) {
       if (i == strlen(s) || s[i] == '\n') num_lines++; 
    }

    lines = new char*[num_lines];
    int count = 0;

    unsigned i = 0;
    for(unsigned j=0; j<=strlen(s); j++)
    { if (j < strlen(s) && s[j] != '\n') continue;
      int n = j-i;
      char* p = new char[n+1];
      strncpy(p,s+i,n);
      p[n] = '\0';
      lines[count++] = p;
      i = j+1;
    }
    assert(count == num_lines);

   }

 const char* get_line(int i) { return lines[i]; }

 ~xx_tooltip() 
 { if (lines) {
     while (num_lines > 0) delete[] lines[--num_lines];
     delete[] lines;
   }
  }

};




struct xx_win
{
  int   width;
  int   height;
  int   xpos;
  int   ypos;
  int   x0;
  int   y0;
  int   x1;
  int   y1;

  int   save_width;
  int   save_height;
  int   save_xpos;
  int   save_ypos;
  int   save_x0;
  int   save_y0;
  int   save_x1;
  int   save_y1;

  int   icon_x;
  int   icon_y;


  int          COLOR;
  int          LINEWIDTH;
  line_style   LINESTYLE;
  drawing_mode MODE;
  text_mode    TEXTMODE;

  char         FONT_NAME[64];
  char         FONT_NAME_TMP[64];

  int            FONT_HEIGHT = 0;
  int*           FONT_WIDTH = 0;
  int*           FONT_OFFSET = 0;
  unsigned char* FONT_ALPHA = 0;
  float          FONT_SCALE = 0.8f;

  drawing_mode save_mode;
  line_style   save_ls;
  int          save_lw;
  int          save_clr;

  int   border_clr;
  int   border_w;
  int   cursor;

  int   bg_clr;
  int   save_bg_clr;
  int   label_clr;
  char  header[512];
  char  label[512];

  char* bg_pixmap;
  char* icon_pixmap;

  Window  id;

  // state & 0x01 :  minimized bit
  // state & 0x02 :  maximized bit
  // 0: normal 1: iconized 2: maximized(normal) 3: iconized(maximized)

  int   state; 

  void* inf;
  int   mapped;
  bool  flush;

  xx_win* pwin; // parent

  x_image* canvas;
  x_image* canvas_save;
  x_image* buffer;

  void  (*redraw)(void*,int,int,int,int,int);

  void (*special_event_handler)(void*,const char*,const char*,int,int);

  unsigned long special_event_data;

  int tt_top;
  xx_tooltip* tt_stack[TOOLTIP_MAX];

  xx_tooltip* ttp_current;
  xx_tooltip* ttp_open;
  int ttp_x;
  int ttp_y;

/*
  ~xx_win() { cout << "xx_win destructor" << endl; }
*/

  void save()
  { save_width  = width;
    save_height = height;
    save_x0 = x0;
    save_y0 = y0;
    save_x1 = x1;
    save_y1 = y1;
    save_xpos = xpos;
    save_ypos = ypos;
  }

  void restore()
  { width  = save_width;
    height = save_height;
    x0 = save_x0;
    y0 = save_y0;
    x1 = save_x1;
    y1 = save_y1;
    xpos = save_xpos;
    ypos = save_ypos;
    state = 0;
  }

  void minimize()
  { 
    if (state == 0) save();

    int icon_w, icon_h;
    x_get_pixrect_size(icon_pixmap,icon_w,icon_h);

    width = icon_w;
    height = icon_h;
    x0 = icon_x;
    y0 = icon_y;
    x1 = x0 + icon_w - 1;
    y1 = y0 + icon_h - 1;
    xpos = x0;
    ypos = y0;
    state |= 1;
   }

  void maximize(int w, int h)
  { 
    if (state == 0) save();

    int xoff = save_xpos - save_x0;
    int yoff = save_ypos - save_y0;

    x0 = 0; 
    y0 = 0; 
    x1 = w;
    y1 = h;
    xpos = x0 + xoff;
    ypos = y0 + yoff;
    width  = w - 2*xoff; 
    height = h - xoff - yoff; 
    state = 2;
  }


};


//------------------------------------------------------------------------------
// events
//------------------------------------------------------------------------------

struct xx_event {
  int ev;
  int val;
  int x;
  int y;
  unsigned long t;
  xx_event() { ev = -1; val = x = y = 0; t = 0; }
};

static xx_event event_buf;
static xx_event last_event;

static int focus_window = -1;


//------------------------------------------------------------------------------
// display
//------------------------------------------------------------------------------

static int display_width = 0;
static int display_height = 0;
static int display_depth = 0;
static int display_dpi = 0;
static int display_fd = 0;

static bool do_not_open_display = false;

//------------------------------------------------------------------------------

// window manager

// REDEFINED when display dpi available !

static int HEADER_W = 47;

static int BORDER_W = int(HEADER_W/4.25);
static int RESIZE_W = HEADER_W/3;
static int BUTTON_W = HEADER_W/3;

static int BUTTON_Y = int(0.36 * HEADER_W);
  
static int BUTTON_X[] = { int(3.7*HEADER_W),
                          int(2.5*HEADER_W), 
                          int(1.3*HEADER_W) };



void redraw_root(int);

//----------------------------------------------------------------------------


const int win_max = 256;

static xx_win* wlist[win_max];
static xx_win* wstack[win_max];

static xx_win* root_win = 0;
static xx_win* grab_win = 0;

static int app_count = 0;
static int win_count = 0;
static int win_top = 0;

static int active_win = 0;

static int active_win_button = -1;



void CopyArea(x_image* src_img, int src_x, int src_y, 
              x_image* dst_img, int dst_x, int dst_y, 
              int width, int height, bool use_alpha=false) 
{ 
  // copy/blend pixel rectangle from src_img  to dst_img

  x_image* root_canv = root_win->canvas;

   assert(src_img != root_canv);

/*
  // root canvas possible (if we are drawing window decorations)
  assert(dst_img != root_canv);
*/

  pixel* src = src_img->buf; 
  int src_w = src_img->w;
  int src_h = src_img->h;

  pixel* dst = dst_img->buf; 
  int dst_w = dst_img->w;
  int dst_h = dst_img->h;

  assert(src != dst);

  int dx = dst_x - src_x;
  int dy = dst_y - src_y;

  // src --> dst with offset (dx,dy)

  int x0 = src_x;
  int y0 = src_y;
  int x1 = src_x + width;
  int y1 = src_y + height;

  // clip to src

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= src_w) x1 = src_w-1;
  if (y1 >= src_h) y1 = src_h-1;

  int clip_x0 = dst_img->clip_x0;
  int clip_y0 = dst_img->clip_y0;
  int clip_x1 = dst_img->clip_x1;
  int clip_y1 = dst_img->clip_y1;

  for(int src_y = y0; src_y <= y1; src_y++)
  { for(int src_x = x0; src_x <= x1; src_x++) 
    { int dst_x = src_x + dx;
      int dst_y = src_y + dy;

      if (dst_x < clip_x0 || dst_x > clip_x1 || 
          dst_y < clip_y0 || dst_y > clip_y1) continue;

      pixel* p = src + src_x + src_y*src_w;
      pixel* q = dst + dst_x + dst_y*dst_w;

      *q  = x_image::blend_pixels(*q,*p,use_alpha);
    }
  }

}


void CopyWindowAreaToRoot(xx_win* win, int x0, int y0, int x1, int y1,
                          int* vis_x0=0, int* vis_y0=0, 
                          int* vis_x1=0, int* vis_y1=0)
{ 
  // copy pixel rectangle from src_img rectangle to root canvas
  // pixels obscured by windows above win in wstack are not copied
  // if (vis_x0 != null) the bounding box of all visible pixels 
  // assigned to [*vis_x0,*vis_y0,*vis_x1,*vis_y0]
  // (src coordinates)

  // src --> dst: add dx = win->xpos and  dy = win->ypos to coords

  int stack_p = win->mapped; // stack position

  x_image* win_canvas = win->canvas;
  pixel* src = win_canvas->buf; 
  int src_w = win_canvas->w;
  int src_h = win_canvas->h;

  x_image* root_canvas = root_win->canvas;
  pixel* dst = root_canvas->buf; 
  int dst_w = root_canvas->w;
  int dst_h = root_canvas->h;

  assert(src != dst);

  int dx = win->xpos;
  int dy = win->ypos;

  // clip to src

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= src_w) x1 = src_w-1;
  if (y1 >= src_h) y1 = src_h-1;

  if (vis_x0)
  { *vis_x0 = x1; 
    *vis_y0 = y1;
    *vis_x1 = x0;
    *vis_y1 = y0;
  }


  for(int src_y = y0; src_y <= y1; src_y++)
  { for(int src_x = x0; src_x <= x1; src_x++) 
    { int dst_x = src_x + dx;
      int dst_y = src_y + dy;

      if (dst_x < 0 || dst_x >= dst_w || dst_y < 0 || dst_y >= dst_h) continue;

      bool visible = true ;

      if (stack_p > 0)
      { for (int k = stack_p+1; k <= win_top; k++)
        { xx_win* wp = wstack[k];
          if (dst_x >= wp->x0 && dst_x <= wp->x1 && 
              dst_y >= wp->y0 && dst_y <= wp->y1)
          { visible = false;
            break;
           }
         }
       }

      if (visible) 
      { dst[dst_x + dst_y*dst_w] = src[src_x + src_y*src_w];
        if (vis_x0)
        { if (src_x < *vis_x0) *vis_x0 = src_x;
          if (src_x > *vis_x1) *vis_x1 = src_x;
          if (src_y < *vis_y0) *vis_y0 = src_y;
          if (src_y > *vis_y1) *vis_y1 = src_y;
         }
       }

    }
  }

}



void SetAlpha(x_image* img, int x0, int y0, int x1, int y1, int alpha, int op) 
{ 
  // op = src_mode
  // op = or_mode
  // op = and_mode

  int w = img->w;
  int h = img->h;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= w) x1 = w-1;
  if (y1 >= h) y1 = h-1;

  int aa = (alpha << 24);

  for(int y = y0; y <= y1; y++)
    for(int x = x0; x <= x1; x++) 
    {  pixel* q = img->buf + y*w + x; 
       int rgb = *q & 0xffffff;
       switch (op) {
         case src_mode: *q = (aa | rgb);
                        break;
         case or_mode:  *q |= aa;
                        break;
         case and_mode: *q &= (aa | 0xffffff);
                        break;
       }
     }
 }


void FlushDisplay(xx_win* win, int x0, int y0,int x1, int y1)
{ 
  if (win->flush == false) return;

  // do not flush if window or its parent is minimized
  if ((win->state & 0x01)) return; 

  if (win->pwin && (win->pwin->state & 0x01)) return; 

  x_image* root_canv = root_win->canvas;

/*
// clipping
  x_image* canv = win->canvas;
  if (x0 < canv->clip_x0) x0 = canv->clip_x0;
  if (y0 < canv->clip_y0) y0 = canv->clip_y0;
  if (x1 > canv->clip_x1) x1 = canv->clip_x1;
  if (y1 > canv->clip_y1) y1 = canv->clip_y1;
*/

  if (win != root_win) {
     CopyWindowAreaToRoot(win, x0,y0,x1,y1, &x0,&y0,&x1,&y1);
  }

  x0 += win->xpos;
  y0 += win->ypos;
  x1 += win->xpos;
  y1 += win->ypos;

  //cout << string("%4d %4d %4d %4d",x0,y0,x1,y1) << endl;

  UPDATE_DISPLAY(root_canv->buf,root_canv->w,root_canv->h,x0,y0,x1,y1,x0,y0);
}


const char* x_display_info(int& width, int& height, int& dpi)  
{ 
  x_open_display();

  width = display_width;
  height = display_height;
  dpi = display_dpi;
  return "xx"; 
} 



int x_window_bits_saved(int) { return 1; }


// fonts

int x_set_font(Window w, const char* font_name) 
{ xx_win* win = wlist[w];

  strcpy(win->FONT_NAME,font_name);

  float sz = (float)atoi(font_name+1);

  // adjust sz to dpi resolution
  sz *= (display_dpi/192.0f);


  switch(font_name[0]) {

     case 'T': win->FONT_WIDTH  = ROMAN_FONT_WIDTH;
               win->FONT_HEIGHT = ROMAN_FONT_HEIGHT;
               win->FONT_OFFSET = ROMAN_FONT_OFFSET;
               win->FONT_ALPHA  = ROMAN_FONT_ALPHA;
               win->FONT_SCALE  = sz/ROMAN_FONT_HEIGHT;
               break;
    
     case 'F': sz *= 0.8f;
               win->FONT_WIDTH  = FIXED_FONT_WIDTH;
               win->FONT_HEIGHT = FIXED_FONT_HEIGHT;
               win->FONT_OFFSET = FIXED_FONT_OFFSET;
               win->FONT_ALPHA  = FIXED_FONT_ALPHA;
               win->FONT_SCALE  = sz/FIXED_FONT_HEIGHT;
               break;
    
     case 'B': win->FONT_WIDTH  = BOLD_FONT_WIDTH;
               win->FONT_HEIGHT = BOLD_FONT_HEIGHT;
               win->FONT_OFFSET = BOLD_FONT_OFFSET;
               win->FONT_ALPHA  = BOLD_FONT_ALPHA;
               win->FONT_SCALE  = sz/BOLD_FONT_HEIGHT;
               break;
    
     case 'I': win->FONT_WIDTH  = ITALIC_FONT_WIDTH;
               win->FONT_HEIGHT = ITALIC_FONT_HEIGHT;
               win->FONT_OFFSET = ITALIC_FONT_OFFSET;
               win->FONT_ALPHA  = ITALIC_FONT_ALPHA;
               win->FONT_SCALE  = sz/ITALIC_FONT_HEIGHT;
               break;
    }

  return 1;
}


void x_save_font(Window w) 
{ xx_win* win = wlist[w];
  strcpy(win->FONT_NAME_TMP,win->FONT_NAME);
}

void x_restore_font(Window w) 
{ xx_win* win = wlist[w];
  char tmp[64];
  strcpy(tmp,win->FONT_NAME_TMP);
  x_set_font(w,tmp);
 }


// predefined fonts

void x_set_text_font(Window w)   { x_set_font(w,"T36"); }
void x_set_bold_font(Window w)   { x_set_font(w,"B35"); }
void x_set_fixed_font(Window w)  { x_set_font(w,"F35"); }
void x_set_italic_font(Window w) { x_set_font(w,"I36"); }
void x_set_button_font(Window w) { x_set_font(w,"T36"); }



int x_set_cursor(Window w, int c) 
{ int c_old = wlist[w]->cursor;
  wlist[w]->cursor  = c;
  //if (w == active_win) SET_CURSOR(c);
  SET_CURSOR(c);
  return c_old;
}

int x_get_cursor(Window w) { return wlist[w]->cursor; }


drawing_mode x_get_mode(Window w)       { return wlist[w]->MODE; }
text_mode    x_get_text_mode(Window w)  { return wlist[w]->TEXTMODE; }
line_style   x_get_line_style(Window w) { return wlist[w]->LINESTYLE; }

int x_get_line_width(Window w)   { return wlist[w]->LINEWIDTH; }
int x_get_color(Window w)        { return wlist[w]->COLOR; }
int x_get_border_color(Window w) { return wlist[w]->border_clr; }
int x_get_border_width(Window w) { return wlist[w]->border_w; }


int x_set_line_width(Window w, int width)
{ xx_win* win = wlist[w];
  int save = win->LINEWIDTH;
  win->LINEWIDTH = width;
  return save;
}

text_mode x_set_text_mode(Window w, text_mode mode)
{ xx_win* win = wlist[w];
  text_mode save = win->TEXTMODE;
  win->TEXTMODE = mode;
  return save;
}

line_style x_set_line_style(Window w, line_style style)
{ xx_win* win = wlist[w];
  line_style save = win->LINESTYLE;
  win->LINESTYLE = style;
  return save;
}

int x_set_color(Window w, int clr)
{ xx_win* win = wlist[w];
  int save = win->COLOR;
  //win->COLOR = clr;
  win->COLOR = clr & 0xffffff;
  return save;
 }

drawing_mode x_set_mode(Window w, drawing_mode mode)
{ xx_win* win = wlist[w];
  drawing_mode save = win->MODE;
  win->MODE = mode;
  return save;
}

int x_text_height(Window w, const char*)
{ xx_win* win = wlist[w];
  return int(0.5 + win->FONT_HEIGHT * win->FONT_SCALE);
  //return int(1.0 + win->FONT_HEIGHT * win->FONT_SCALE);
}

int x_text_width(Window w, const char* s) { 
  return x_text_width(w,s,(int)strlen(s));
}

int  x_text_width(Window w, const char* s, int len)
{ xx_win* win = wlist[w];
  if (len > int(strlen(s))) len = int(strlen(s));
  float f = win->FONT_SCALE;
  int width = 0;
  for(int i = 0; i < len; i++) { 
    int c = s[i] & 127;
    width += int(0.5 + f*win->FONT_WIDTH[c]);
  }
  return width;
}


inline void bigpix(xx_win* win, int x, int y)
{ x_image* canv = win->canvas;
  canv->bigpix(x,y,win->LINEWIDTH,win->COLOR,win->MODE);
}

inline void bigpix1(xx_win* win, int x, int y)
{ x_image* canv = win->canvas;
  canv->bigpix1(x,y,win->LINEWIDTH,win->COLOR,win->MODE);
}

inline void hline(xx_win* win, int x0, int x1, int y)
{ x_image* canv = win->canvas;
  canv->hline(x0,x1,y,win->COLOR,win->MODE);
}

inline void vline(xx_win* win, int x, int y0, int y1)
{ x_image* canv = win->canvas;
  canv->vline(x,y0,y1,win->COLOR,win->MODE);
}


inline void bitmap_line(xx_win* win, int x,int y,unsigned char* bits,int len)
{ x_image* canv = win->canvas;
  canv->bitmap_line(x,y,bits,len,win->COLOR,win->MODE);
}


//-----------------------------------------------------------------------------
// Wu's Line Algorithm (anti-aliasing)
//-----------------------------------------------------------------------------

inline void plot(xx_win* win, double x, double y, pixel clr, double bright) {
  int a = int(bright*255);
  clr |= (a << 24);
  win->canvas->blend_pixel(int(x),int(y),clr,src_mode);
}


inline double ipart(double x) {
  // integer part of x
  return floor(x);
}

inline double fpart(double x) {
  // fractional part of x
  return x - floor(x);
}

inline double rfpart(double x) {
  // 1 - fractional part
  return 1 - fpart(x);
}

void WuDrawLine(xx_win* win, int xx0, int yy0, int xx1, int yy1) 
{
  x_image* canv = win->canvas;

  pixel clr = win->COLOR;

  double x0 = xx0;
  double y0 = yy0;
  double x1 = xx1;
  double y1 = yy1;

  bool steep = abs(y1-y0) > abs(x1-x0);
  
  if (steep)  {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }

  if (x0 > x1){
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  
  double dx = x1 - x0;
  double dy = y1 - y0;

  double gradient = (dx == 0) ? 1.0 : dy/dx;

  // handle first endpoint

  double xend = round(x0);
  double yend = y0 + gradient * (xend - x0);
  double xgap = rfpart(x0 + 0.5);
  double xpxl1 = xend; // this will be used in the main loop
  double ypxl1 = ipart(yend);

  if (steep) {
     plot(win,ypxl1,   xpxl1, clr, rfpart(yend) * xgap);
     plot(win,ypxl1+1, xpxl1, clr,  fpart(yend) * xgap);
  }
  else {
      plot(win,xpxl1, ypxl1  ,clr,  rfpart(yend) * xgap);
      plot(win,xpxl1, ypxl1+1,clr,   fpart(yend) * xgap);
  }

  double intery = yend + gradient; // first y-intersection for the main loop
  
  // handle second endpoint

  xend = round(x1);
  yend = y1 + gradient * (xend - x1);
  xgap = fpart(x1 + 0.5);

  double xpxl2 = xend; //this will be used in the main loop
  double ypxl2 = ipart(yend);

  if (steep) {
      plot(win,ypxl2  , xpxl2,clr,  rfpart(yend) * xgap);
      plot(win,ypxl2+1, xpxl2,clr,   fpart(yend) * xgap);
  }
  else {
      plot(win,xpxl2, ypxl2,  clr, rfpart(yend) * xgap);
      plot(win,xpxl2, ypxl2+1,clr,  fpart(yend) * xgap);
  }
  
  // main loop

  if (steep) 
    for (double x = xpxl1 + 1; x < xpxl2 - 1; x++)
    { plot(win,ipart(intery)  , x, clr, rfpart(intery));
      plot(win,ipart(intery)+1, x, clr,  fpart(intery));
      intery = intery + gradient;
     }
  else
    for (double x = xpxl1 + 1; x < xpxl2 - 1; x++)
    { plot(win,x, ipart(intery),  clr, rfpart(intery));
      plot(win,x, ipart(intery)+1,clr,  fpart(intery));
      intery = intery + gradient;
    }

  int lw = win->LINEWIDTH;
  if (xx0 > xx1) std::swap(xx0,xx1);
  if (yy0 > yy1) std::swap(yy0,yy1);
  FlushDisplay(win,xx0-lw,yy0-lw,xx1+lw,yy1+lw);
}




static void extend_line(int s, int& x1, int& y1, int& x2, int& y2)
{ // extend line by s pixels

  int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0) return;

  int xoff = s;
  int yoff = s;

  if (dx < 0) { dx = -dx; xoff = -s; }
  if (dy < 0) { dy = -dy; yoff = -s; }

  if ( dx >= dy) x2 += xoff;
  if ( dx <= dy) y2 += yoff;
 }



inline bool FILTER_PIX(int pix_count, int dash_w, int dot_w, int dash_dot_w)
{ if (dash_w == 0) return true;
  int x = (pix_count/dash_w) % dot_w;
  return (x == 0 || (dash_dot_w > 0 && (pix_count % dash_dot_w) == 0));
}


void x_line0(Window w, int x1, int y1, int x2, int y2) { 
  x_line(w,x1,y1,x2,y2);
}

void x_line(Window w, int x1, int y1, int x2, int y2)
{ 
  xx_win* win = wlist[w];

  if (x1 > x2) { std::swap(x1,x2); std::swap(y1,y2); }

  int lw = win->LINEWIDTH;

  if (lw <= 0) return;

/*
  int lw2 = win->LINEWIDTH/2;
  extend_line(lw2,x1,y1,x2,y2);
  extend_line(lw2,x2,y2,x1,y1);
*/

  int sx = 1;
  int sy = 1;

  int dx = x2 - x1;
  int dy = y2 - y1;

  if (dy < 0)
  { dy = -dy;
    sy = -1;
   }

  int dash_w = 0;
  int dot_w = 0;
  int dash_dot_w = 0;

  if (win->LINESTYLE == dashed) { dash_w = 16; dot_w = 2; }
  if (win->LINESTYLE == dotted) { dash_w = 1; dot_w = 12; }

  if (win->LINESTYLE == dashed_dotted) { 
      dash_w = 20; dot_w = 2; dash_dot_w = 10; 
  }

  int pix_count = 0;
 
  int x = x1;
  int y = y1;

  if (dx > dy)
  { int c = dx / 2;
    if (FILTER_PIX(++pix_count,dash_w,dot_w,dash_dot_w)) bigpix(win,x,y);
    while(x != x2)
    { x += sx;
      if ((c+=dy) >= dx)
      { c -= dx;
        y += sy;
       }
      if (FILTER_PIX(++pix_count,dash_w,dot_w,dash_dot_w)) bigpix(win,x,y);
    }
  }
  else  
  { // dy >= dx
    int c = dy / 2;
    if (FILTER_PIX(++pix_count,dash_w,dot_w,dash_dot_w)) bigpix(win,x,y);
    while(y != y2)
    { y += sy;
      if ((c+=dx) >= dy)
      { c -= dy;
        x += sx;
       }
      if (FILTER_PIX(++pix_count,dash_w,dot_w,dash_dot_w)) bigpix(win,x,y);
    }
  }

  if (y1 > y2) std::swap(y1,y2);

  FlushDisplay(win,x1-lw,y1-lw,x2+lw,y2+lw);
}




void x_lines(Window w, int n, int* x1, int* y1, int* x2, int* y2) 
{ xx_win* win = wlist[w];

  if (n <= 0) return;

  bool win_flush  = win->flush;

  win->flush = false;

  int x_min = x1[0];
  int x_max = x1[0];
  int y_min = y1[0];
  int y_max = y1[0];

  for (int i=0; i<n; i++)
  { x_line(w,x1[i],y1[i],x2[i],y2[i]); 

   if (x1[i] < x_min) x_min = x1[i];
   if (x1[i] > x_max) x_max = x1[i];
   if (x2[i] < x_min) x_min = x2[i];
   if (x2[i] > x_max) x_max = x2[i];

   if (y1[i] < y_min) y_min = y1[i];
   if (y1[i] > y_max) y_max = y1[i];
   if (y2[i] < y_min) y_min = y2[i];
   if (y2[i] > y_max) y_max = y2[i];
  }

  win->flush = win_flush;

  int lw = win->LINEWIDTH;
  FlushDisplay(win,x_min-lw,y_min-lw,x_max+lw,y_max+lw);

}


/*
// unused
static void clip_bitmap(x_image* canv, int x, int y, unsigned char* data,
                                                     int width, int height)
{ 
  int y1 = y + height - 1;
  int x1 = x + 8*width - 1;

  int left_margin = -1;
  int top_margin  = -1;
  int right_margin  = width;
  int bottom_margin = height;

  unsigned char mask1 = 0xFF;
  unsigned char mask2 = 0xFF;

  if (y  < canv->clip_y0) top_margin    += canv->clip_y0 - y;
  if (y1 > canv->clip_y1) bottom_margin -= y1 - canv->clip_y1;

  if (x  < canv->clip_x0)  
  { left_margin  = canv->clip_x0 - x;
    mask1 = (0xFF >> (left_margin%8));
    left_margin /= 8;
   }

  if (x1 > canv->clip_x1) 
  { right_margin = (x1 - canv->clip_x1);
    mask2 = ~(0xFF >> (right_margin%8));
    right_margin = width - right_margin/8 - 1;
   }

  for (int i = 0; i < height; i++)
  { unsigned char* p = data + i*width;
    if (i < top_margin || i > bottom_margin)
       { unsigned char* stop = p + width;
         while (p < stop) *p++ = 0;
        }
    else
      for(int j=0; j < width; j++)
      { if (j < left_margin || j > right_margin) *p=0;
        if (j == left_margin) *p &= mask1;
        if (j == right_margin) *p &= mask2;
        p++;
       }
   }

}
*/


void x_polyline(Window w, int n, int* xcoord, int* ycoord, int adjust)
{ 
  xx_win* win = wlist[w];
  bool win_flush = win->flush;
  win->flush = false;

  int xmin = xcoord[0];
  int xmax = xcoord[0];
  int ymin = ycoord[0];
  int ymax = ycoord[0];

  for(int i=0; i<n; i++)
  { if (xcoord[i] < xmin) xmin = xcoord[i];
    if (xcoord[i] > xmax) xmax = xcoord[i];
    if (ycoord[i] < ymin) ymin = ycoord[i];
    if (ycoord[i] > ymax) ymax = ycoord[i];
    if (i < n-1) x_line(w,xcoord[i],ycoord[i],xcoord[i+1],ycoord[i+1]);
  }

  win->flush = win_flush;
  FlushDisplay(win,xmin-1,ymin-1,xmax+1,ymax+1);
}


void x_polygon(Window w, int n, int* xcoord, int* ycoord)
{ xx_win* win = wlist[w];
  bool win_flush = win->flush;
  win->flush = false;
  x_line(w,xcoord[n-1],ycoord[n-1],xcoord[0],ycoord[0]);
  win->flush = win_flush;
  x_polyline(w,n,xcoord,ycoord);
}




static void fill_triangle0(x_image* canvas, int clr, int mode, bool closed,
                           int x0, int y0, int x1, int y1, int x2, int y2)
{
  double a = TRIANGLE_AREA2(x0,y0,x1,y1,x2,y2); // this is an integer

  if (fabs(a) < 1) return;

  if (a < 0)
  { // make triangle positive oriented
    std::swap(x1,x2);
    std::swap(y1,y2);
  }

  // bounding box

  int xmin = x0;
  int ymin = y0;
  int xmax = x0;
  int ymax = y0;

  if (x1 < xmin) xmin = x1;
  if (x1 > xmax) xmax = x1;
  if (y1 < ymin) ymin = y1;
  if (y1 > ymax) ymax = y1;

  if (x2 < xmin) xmin = x2;
  if (x2 > xmax) xmax = x2;
  if (y2 < ymin) ymin = y2;
  if (y2 > ymax) ymax = y2;

  // draw all pixels in bounding box lying inside triangle 

  int D = closed ? 0 : 1;

  for(int y = ymin; y <= ymax; y++)
  { for(int x = xmin; x <= xmax; x++)
    { if (TRIANGLE_AREA2(x0,y0,x1,y1,x,y) >= 0 &&
          TRIANGLE_AREA2(x1,y1,x2,y2,x,y) >= 0 &&
          TRIANGLE_AREA2(x2,y2,x0,y0,x,y) >= 0) 
        canvas->setpix(x,y,clr,mode);
     }
   }
}


static void fill_triangle(x_image* canvas, int clr, int mode, bool closed,
                          int x0, int y0, int x1, int y1, int x2, int y2)
{
  // sort points by y-coordinate such that p0 is lowest and p2 highest point

  if (y1 < y0) { std::swap(x0,x1); std::swap(y0,y1); }
  if (y2 < y0) { std::swap(x0,x2); std::swap(y0,y2); }
  if (y2 < y1) { std::swap(x1,x2); std::swap(y1,y2); }

  double a = TRIANGLE_AREA2(x0,y0,x1,y1,x2,y2); 

  if (fabs(a) < 1) return;

  int orient = (a > 0) ? +1 : -1;

  int xmin = x0;
  if (x1 < xmin) xmin = x1;
  if (x2 < xmin) xmin = x2;

  int xmax = x0;
  if (x1 > xmax) xmax = x1;
  if (x2 > xmax) xmax = x2;

  // draw all pixels in bounding box lying inside triangle 
  // start along all points on edge (p0,p2) 
  // running to the right or left (depending on orientation(p0,p1,p2))

  double f = double(x2-x0)/(y2-y0); // f = 1/slope(p0,p2)

  int xstep  = orient;  // +1 or -1


  int D = closed ? 0 : 1;

  for(int y = y0; y <= y2; y++)
  { // interpolate x 
    int x = int(x0 + f*(y-y0)) - xstep;

    // be sure to start on the correct side of p0,p2 (inside triangle)
    while (orient*TRIANGLE_AREA2(x2,y2,x0,y0,x,y) < D) x += xstep;

    int steps  = (orient > 0) ? (xmax - x) : (x - xmin); 

    for(int i = 0; i<steps; i++)
    { if (orient*TRIANGLE_AREA2(x0,y0,x1,y1,x,y) < D || 
          orient*TRIANGLE_AREA2(x1,y1,x2,y2,x,y) < D) break;
      canvas->setpix(x,y,clr,mode);
      x += xstep;
    }
  }

}


void x_fill_triangle(Window w, int x0, int y0, int x1, int y1, int x2, int y2)
{ xx_win* win = wlist[w];

  fill_triangle(win->canvas,win->COLOR,win->MODE,true,x0,y0,x1,y1,x2,y2);

  int xmin = x0;
  int ymin = y0;
  int xmax = x0;
  int ymax = y0;

  if (x1 < xmin) xmin = x1;
  if (x1 > xmax) xmax = x1;
  if (y1 < ymin) ymin = y1;
  if (y1 > ymax) ymax = y1;

  if (x2 < xmin) xmin = x2;
  if (x2 > xmax) xmax = x2;
  if (y2 < ymin) ymin = y2;
  if (y2 > ymax) ymax = y2;

  FlushDisplay(win,xmin,ymin,xmax,ymax);
}


  

void x_fill_polygon(Window w, int n, int* xcoord, int* ycoord)
{ xx_win* win = wlist[w];

  if (n < 3) return;

  if (n == 3)
  { x_fill_triangle(w,xcoord[0],ycoord[0],xcoord[1],ycoord[1],
                                          xcoord[2],ycoord[2]);
    return;
   }

  double A = 0; // polygon area (sign gives orientation)
  for(int i=0; i<n; i++)
  { int j = (i+1) % n;
    A += TRIANGLE_AREA2(0,0,xcoord[i],ycoord[i],xcoord[j],ycoord[j]);
  }

  if (fabs(A) < 1) return; // area = 0

  int orient = (A > 0) ? +1 : -1;

  // bounding box 

  int xmin = xcoord[0];
  int xmax = xcoord[0];
  int ymin = ycoord[0];
  int ymax = ycoord[0];

  for(int i=1;i<n;i++)
  { int x = xcoord[i];
    int y = ycoord[i];
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
   }

  int width  = xmax - xmin + 1;
  int height = ymax - ymin + 1;

  x_image img(width,height);

  int ADD = -1; // add value to pixel
  bool closed = true;

  for(int i=0;i<n;i++)
  { int j = (i+1) % n;
    int x1 = xcoord[i] - xmin;
    int y1 = ycoord[i] - ymin;
    int x2 = xcoord[j] - xmin;
    int y2 = ycoord[j] - ymin;

    double a = TRIANGLE_AREA2(0,0,x1,y1,x2,y2);

    int c = 0;
    if (a*orient > 0) c = +1;
    if (a*orient < 0) c = -1;

    if (c != 0) fill_triangle(&img,c,ADD,closed,0,0,x1,y1,x2,y2);
  }

  x_image* canv = win->canvas;
  int clr = win->COLOR;
  int mode = win->MODE;

  for(int x = 0; x < width; x++)
  { for(int y = 0; y < height; y++)
    { int c = img.getpix(x,y);
/*
      if (c > 1 || c < 0)  {
        cout << "x = " << x << " y = " << y << " c = " << c << endl;
      }
      assert(c == 0 || c == 1);
*/
      if (c > 0) canv->setpix(x+xmin,y+ymin,clr,mode);
     }
   }

  FlushDisplay(win,xmin,ymin,xmax,ymax);
}



void x_fill_polygon_ear_clipping(Window w, int n, int* xcoord, int* ycoord)
{
  // triangulate polygon by ear clipping and fill triangles

  xx_win* win = wlist[w];

  if (n < 3) return;

  if (n == 3)
  { x_fill_triangle(w,xcoord[0],ycoord[0],xcoord[1],ycoord[1],
                                          xcoord[2],ycoord[2]);
    return;
   }


  double A = 0; // polygon area (sign gives orientation)

  for(int i=0; i<n; i++) {
    int j = (i+1) % n;
    A += TRIANGLE_AREA2(0,0,xcoord[i],ycoord[i],xcoord[j],ycoord[j]);
  }

  if (fabs(A) < 1) return; // area to small


  // compute bounding box and build positive oriented (singly linked) list L
  // of the points (indices)

  list<int> L;

  int xmin = xcoord[0];
  int xmax = xcoord[0];
  int ymin = ycoord[0];
  int ymax = ycoord[0];

  for(int i=0;i<n;i++)
  { int x = xcoord[i];
    int y = ycoord[i];
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;

    if (A > 0) 
      L.append(i);
    else
      L.push(i);
   }


/*
  list<list_item> L_reflex; // list of all reflex vertices in current polygon L

  list_item* reflex_it = new list_item[n];
  for(int i=0; i<n; i++) reflex_it[i] = 0;

  // reflex_it[i] = position (list_item) of i in L_reflex, if i is reflex)
  // 0 if i is not reflex or has been clipped


   list_item a;
   forall_items(a,L) {
     list_item b = L.cyclic_succ(a);
     list_item c = L.cyclic_succ(b);
     if (TRIANGLE_AREA2(xcoord[L[a]],ycoord[L[a]],
                        xcoord[L[b]],ycoord[L[b]],
                        xcoord[L[c]],ycoord[L[c]]) < 0)
     reflex_it[L[b]] = L_reflex.append(b);
   }
*/



  // Ear Clipping:
  // Let list item i rotate through vertex cycle and repeatedly find 
  // and clip empty positive oriented triangles starting at i.
  // In each round at least one triangle is found (if the polygon is simple); 

  list_item it_i = L.first(); 

  int last_n = 0;

  while (L.length() >= 3)
  { // we have at least 3 vertices

    if (it_i == L.first())
    { // round completed
      // at least one vertex must have been clipped in last round
      // otherwise something is wrong with the input polygon (non-simple)

      if (L.length() == last_n) 
      { cout << "x_fill_polygon: cannot clip  n = " << L.length() << endl;
        break;
      }

      last_n = L.length();
     }

    // walk through all vertices i and check if triangle (i,i+1,i+2) 
    // can be drawn and clipped by deleting vertex j = i+1

    list_item it_j = L.cyclic_succ(it_i);
    list_item it_k = L.cyclic_succ(it_j);

    int i = L[it_i];
    int j = L[it_j];
    int k = L[it_k];

    double a = TRIANGLE_AREA2(xcoord[i],ycoord[i],xcoord[j],ycoord[j],
                                                  xcoord[k],ycoord[k]);
    if (a == 0) {
     // colinear vertices: clip j and keep i
/*
     list_item it = reflex_it[j];
     if (it) L_reflex.del_item(it);
     reflex_it[j] = 0;
*/
     L.del_item(it_j);
     continue;
    }

    if (a <= 0)
    { // non convex (reflex) corner: move i forward
      it_i = L.cyclic_succ(it_i);
      continue;
     }

    // a > 0:  convex corner at vertex j
/*
    // remove j from L_reflex (if present)
    list_item x = reflex_it[j];
    if (x) L_reflex.del_item(x);
    reflex_it[j] = 0;
*/

    // check if some other (reflex) vertex p lies inside triangle (i,j,k)
    // check vertices k+1, ..., i-1 (cyclic)

    list_item it = L.first();

    while (it)
    { 
      if (it != it_i && it != it_j && it != it_k)
      { int q = L[it];
        int p = L[L.cyclic_pred(it)];
        int r = L[L.cyclic_succ(it)];
        bool reflex = (TRIANGLE_AREA2(xcoord[p],ycoord[p],
                                      xcoord[q],ycoord[q],
                                      xcoord[r],ycoord[r]) < 0);
        if (reflex &&
            TRIANGLE_AREA2(xcoord[i],ycoord[i],xcoord[j],ycoord[j],
                                               xcoord[q],ycoord[q]) > 0 &&
            TRIANGLE_AREA2(xcoord[j],ycoord[j],xcoord[k],ycoord[k],
                                               xcoord[q],ycoord[q]) > 0 &&
            TRIANGLE_AREA2(xcoord[k],ycoord[k],xcoord[i],ycoord[i],
                                               xcoord[q],ycoord[q]) >= 0) break;
       }

      it = L.succ(it);
    }


/*
    list_item it = L_reflex.first();

    while (it)
    { list_item it_r = L_reflex[it];

      // check if vertex q is still reflex (delete it otherwise from list)
      int p = L[L.cyclic_pred(it_r)];
      int q = L[it_r];
      int r = L[L.cyclic_succ(it_r)];

      if (TRIANGLE_AREA2(xcoord[p],ycoord[p],xcoord[q],ycoord[q],
                                             xcoord[r],ycoord[r]) >= 0)
      { reflex_it[q] = 0;
        list_item x = it;
        it = L_reflex.succ(x);
        L_reflex.del_item(x);
        continue;
      }


      if (q != i && q != j && q != k)
      { if (TRIANGLE_AREA2(xcoord[i],ycoord[i], xcoord[j],ycoord[j],
                                                xcoord[q],ycoord[q]) > 0 &&
            TRIANGLE_AREA2(xcoord[j],ycoord[j], xcoord[k],ycoord[k],
                                                xcoord[q],ycoord[q]) > 0 &&
            TRIANGLE_AREA2(xcoord[k],ycoord[k], xcoord[i],ycoord[i],
                                                xcoord[q],ycoord[q]) >= 0) break;
      }

      it = L_reflex.succ(it);
    }
*/

    if (it == 0)
    { // triangle (i,j,k) is empty: draw it and clip vertex j 
      bool closed = true;
      fill_triangle(win->canvas,win->COLOR,win->MODE, closed,
                                                      xcoord[i],ycoord[i],
                                                      xcoord[j],ycoord[j],
                                                      xcoord[k],ycoord[k]);
      L.del_item(it_j);
     }
    else
    { // move i forward
      it_i = L.cyclic_succ(it_i);
     }
  }

/*
  delete[] reflex_it;
*/

  FlushDisplay(win,xmin,ymin,xmax,ymax);
}




static bool compute_interior_point(int n, int* xcoord, int* ycoord, int& x, 
                                                                    int& y)
{ // preconditions: 
  // polygon is simple 
  // has positive orientation
  // and size >= 3

  assert(n >= 3);

  // if convex return center of gravity 
  // otherwise find largest positive oriented empty triangle (ear)
  // and return center of this triangle

  int i_max = -1;
  double a_max = -1;

  bool convex = true;

  for(int i = 0; i < n; i++)
  { int j = (i+1) % n; 
    int k = (j+1) % n; 

    double a = TRIANGLE_AREA2(xcoord[i],ycoord[i],xcoord[j],ycoord[j],
                                                  xcoord[k],ycoord[k]);
    if (a <= 0) { 
      // non-convex or colinear
      if (a < 0) convex = false;
      continue; 
    }

    // a > 0 : convex corner at vertex j 
    // check if some other vertex q lies inside triangle (i,j,k)

    int p = 0;
    while (p < n)
    { if (p != i && p != j && p != k)
      { if ((TRIANGLE_AREA2(xcoord[i],ycoord[i],xcoord[j],ycoord[j],
                                                xcoord[p],ycoord[p]) >= 0)
        &&  (TRIANGLE_AREA2(xcoord[j],ycoord[j],xcoord[k],ycoord[k],
                                                xcoord[p],ycoord[p]) >= 0)
        &&  (TRIANGLE_AREA2(xcoord[k],ycoord[k],xcoord[i],ycoord[i],
                                                xcoord[p],ycoord[p]) >= 0)) break;
      }
      p++;
     }

     if (p < n) continue; // p lies inside triangle (i,j,k)

     // triangle (i,j,k) is empty

     if (a > a_max) {
       a_max = a;
       i_max = i;
     }
  }

  if (convex) 
  { double sum_x = 0;
    double sum_y = 0;
    for(int i=0; i<n; i++)
    { sum_x += xcoord[i];
      sum_y += ycoord[i];
     }
    x = int(0.5 + sum_x/n);
    y = int(0.5 + sum_y/n);
    return true;
  }

//assert(i_max > -1);

  if (i_max == -1) return false; // could not find an interior point

  int i = i_max;
  int j = (i+1) % n;
  int k = (j+1) % n;

  x = int(0.5 + (xcoord[i] + xcoord[j] + xcoord[k])/3.0);
  y = int(0.5 + (ycoord[i] + ycoord[j] + ycoord[k])/3.0);

  return true;
}



static void flood_fill(x_image* canv, int x, int y, 
                       int x_min, int y_min, int x_max, int y_max, pixel clr)
{ 
  int width = canv->w;

  if (x_min < canv->clip_x0+1) x_min = canv->clip_x0+1;
  if (x_max > canv->clip_x1-1) x_max = canv->clip_x1-1;
  if (y_min < canv->clip_y0+1) y_min = canv->clip_y0+1;
  if (y_max > canv->clip_y1-1) y_max = canv->clip_y1-1;

  if (x < x_min || x > x_max || y < y_min || y > y_max) return;


  pixel* buf = canv->buf;
  pixel* pos = buf + y*width + x;

  b_queue<pixel*> queue(32768);   // 1 << 15

  if ((*pos & 0x80000000) == 0)
  { *pos = clr;
     queue.append(pos);
   }

  int offset[4] = { -1, +1, -width, +width };

  int max_qsize = 0;

  while (!queue.empty())
  { int sz = queue.size();
    if (sz > max_qsize) max_qsize = sz;
    pixel* p = queue.pop();

    if (sz > queue.max_size() - 4) continue;

    for (int i=0; i<4; i++)
    { pixel* q = p + offset[i];
      int xx = (q-buf) % width;
      int yy = (q-buf) / width;
      if (xx < x_min || xx > x_max || yy < y_min || yy > y_max) continue;

      if ((*q & 0x80000000) == 0) queue.append(q); 

      if (clr & 0x01000000) // write clip mask bit
        *q = (*q & 0x00ffffff) | (clr & 0xff000000);
      else
        *q = clr; 
    }
  }

  if (max_qsize > queue.max_size()-10) { 
    cout << "max_qsize = " << max_qsize << endl;
  }

}



void x_fill_polygon_flood_fill(Window w, int n, int* xcoord, int* ycoord)
{
  // find an interior point p and flood fill starting at p

  if (n < 3) return;

  if (n == 3)
  { x_fill_triangle(w,xcoord[0],ycoord[0],xcoord[1],ycoord[1],
                                          xcoord[2],ycoord[2]);
    return;
   }

  // bounding box (x_min,y_min,x_max,y_max)

  int x_min = xcoord[0];
  int x_max = xcoord[0];
  int y_min = ycoord[0];
  int y_max = ycoord[0];

  for(int i=1;i<n;i++)
  { if (xcoord[i] < x_min) x_min = xcoord[i];
    if (xcoord[i] > x_max) x_max = xcoord[i];
    if (ycoord[i] < y_min) y_min = ycoord[i];
    if (ycoord[i] > y_max) y_max = ycoord[i];
   }

  if (x_min == x_max || y_min == y_max)  return; // degenerate


  // compute area and check orientation

  double A = 0; // total area (sign gives orientation)
  for(int i=0; i<n; i++) {
    int j = (i+1) % n;
    A += TRIANGLE_AREA2(0,0,xcoord[i],ycoord[i],xcoord[j],ycoord[j]);
  }

  if (A < 0) 
  { // we want positive (counter-clockwise) orientation 
    // reverse point list if A < 0
    for(int i=0; i<n/2; i++) {
      int j = n-i-1;
      std::swap(xcoord[i],xcoord[j]);
      std::swap(ycoord[i],ycoord[j]);
     }
     A = -A;
   }

  if (fabs(A) < 1) return;

  // find an interior point (x,y)
  int x,y;
  if (!compute_interior_point(n,xcoord,ycoord,x,y)) return;

  // draw outline of polygon with 0x80 bit set and flood fill from (x,y)

  xx_win* win = wlist[w];

  pixel      win_color  = win->COLOR;
  int        win_lwidth = win->LINEWIDTH;
  line_style win_lstyle = win->LINESTYLE;
  drawing_mode win_mode = win->MODE;

  // draw outline

  win->LINEWIDTH = 1;
  win->COLOR = 0x80000000; // fill bit
  win->MODE = or_mode;

  for(int i=0; i<n; i++)
    x_line(w,xcoord[i],ycoord[i],xcoord[(i+1)%n],ycoord[(i+1)%n]);

  // flood fill from (x,y)

  pixel fill_clr = win_color | 0x80000000;
  flood_fill(win->canvas,x,y,x_min,y_min,x_max,y_max,fill_clr);

  // reset fill poly bits
  SetAlpha(win->canvas, x_min,y_min,x_max,y_max,~0x80,and_mode);

  // reset drawing attributes
  win->LINEWIDTH = win_lwidth;
  win->LINESTYLE = win_lstyle;
  win->COLOR = win_color;
  win->MODE = win_mode;

  FlushDisplay(win,x_min,y_min,x_max,y_max);
}




void x_box(Window w, int x0, int y0, int x1, int y1)
{ xx_win* win = wlist[w];

  if (x0 > x1) std::swap(x0,x1);
  if (y0 > y1) std::swap(y0,y1);

  x_image* canv = win->canvas;

  // clipping
  if (x0 < canv->clip_x0) x0 = canv->clip_x0;
  if (x1 > canv->clip_x1) x1 = canv->clip_x1;
  if (y0 < canv->clip_y0) y0 = canv->clip_y0;
  if (y1 > canv->clip_y1) y1 = canv->clip_y1;

  if (x0 <= x1 && y0 <= y1) 
    for(int y=y0; y<=y1; y++) hline(win,x0,x1,y);

  FlushDisplay(win,x0,y0,x1,y1);
}


void x_clear_window(Window w, int x0, int y0, int x1, int y1, int xorig, 
                                                               int yorig)
{ xx_win* win = wlist[w];

  //if (win->mapped == 0) return;
  if (win->canvas == 0) return; // not displayed
  

  char* pm = win->bg_pixmap;

  if (pm == 0)
  { int c = x_set_color(w,win->bg_clr);
    x_box(w,x0,y0,x1,y1);
    x_set_color(w,c);
    return;
   }

  x_image* im = (x_image*)pm;
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

  int xmax = win->canvas->w;
  int ymax = win->canvas->h;

  for(int y = yorig;  y < ymax; y += he)
    for(int x = xorig; x < xmax; x += wi)
      if (x < x1 && x+wi > x0 && y < y1 && y+he >y0)
        CopyArea(im,0,0,win->canvas,x,y,wi,he);

  FlushDisplay(win,0,0,xmax-1,ymax-1);
}




void x_rect(Window w, int x0, int y0, int x1, int y1)
{ xx_win* win = wlist[w];

  if (x0 > x1) std::swap(x0,x1);
  if (y0 > y1) std::swap(y0,y1);

  int lw = win->LINEWIDTH;

  for(int i=0; i<lw; i++)
  { int d = (i%2) ? -(i+1)/2 : +(i+1)/2;
    hline(win,x0+d,x1-d,y0+d);
    hline(win,x0+d,x1-d,y1-d);
    vline(win,x0+d,y0+d,y1-d);
    vline(win,x1-d,y0+d,y1-d);
  }

/*
  FlushDisplay(win,x0-lw,y0-lw,x1+lw,y1+lw);
*/

  FlushDisplay(win,x0-lw,y0-lw,x0+lw,y1+lw);
  FlushDisplay(win,x1-lw,y0-lw,x1+lw,y1+lw);
  FlushDisplay(win,x0-lw,y0-lw,x1+lw,y0+lw);
  FlushDisplay(win,x0-lw,y1-lw,x1+lw,y1+lw);

}


/*
void x_circle0(Window w, int x0,int y0,int r0)
{ 
  xx_win* win = wlist[w];
  x_image* canvas = win->canvas;
  pixel clr = win->COLOR;
  int mode = win->MODE;

  int r;
  for (r = r0-win->LINEWIDTH/2; r <= r0+win->LINEWIDTH/2; r++)
  { int y = r;
    int x = 0;
    int e = 3-2*y;

    canvas->setpix(x0,y0+r,clr,mode);
    canvas->setpix(x0,y0-r,clr,mode);
    canvas->setpix(x0+r,y0,clr,mode);
    canvas->setpix(x0-r,y0,clr,mode);

    for (x=1;x<y;)
    { canvas->setpix(x0+x,y0+y,clr,mode);
      canvas->setpix(x0+x,y0-y,clr,mode);
      canvas->setpix(x0-x,y0+y,clr,mode);
      canvas->setpix(x0-x,y0-y,clr,mode);
      canvas->setpix(x0+y,y0+x,clr,mode);
      canvas->setpix(x0+y,y0-x,clr,mode);
      canvas->setpix(x0-y,y0+x,clr,mode);
      canvas->setpix(x0-y,y0-x,clr,mode);
      x++;
      if (e>=0) { y--; e = e - 4*y; }
      e = e + 4*x + 2;
     }

    if (x == y)
    { canvas->setpix(x0+x,y0+y,clr,mode);
      canvas->setpix(x0+x,y0-y,clr,mode);
      canvas->setpix(x0-x,y0+y,clr,mode);
      canvas->setpix(x0-x,y0-y,clr,mode);
     }
  }

  FlushDisplay(win,x0-r0,y0-r0,x0+r0,y0+r0);
}
*/



#define SETARCPIX(x,y)\
{ if (orient >= 0)\
  { if (ax*y >= ay*x && x*by >= y*bx) canvas->setpix(x0+x,y0+y,clr,mode); }\
  else \
  { if (ax*y >= ay*x || x*by >= y*bx) canvas->setpix(x0+x,y0+y,clr,mode); }\
}
 

void x_arc(Window w, int x0, int y0, int r0, int r1, double start, double angle)
{ xx_win* win = wlist[w];
  x_image* canvas = win->canvas;
  pixel clr = win->COLOR;
  int mode = win->MODE;

  double ax =  cos(start);
  double ay = -sin(start);
  double bx =  cos(start+angle);
  double by = -sin(start+angle);

  double orient = ax*by - ay*bx;

  for (int r = r0-win->LINEWIDTH/2; r <= r0+win->LINEWIDTH/2; r++)
  { int y = r;
    int x = 0;
    int e = 3-2*y;

    SETARCPIX( 0, r);
    SETARCPIX( 0,-r);
    SETARCPIX( r, 0);
    SETARCPIX(-r, 0);

    for (x=1;x<y;)
      { SETARCPIX( x, y);
        SETARCPIX( x,-y);
        SETARCPIX(-x, y);
        SETARCPIX(-x,-y);
        SETARCPIX( y, x);
        SETARCPIX( y,-x);
        SETARCPIX(-y, x);
        SETARCPIX(-y,-x);
        x++;
        if (e>=0) { y--; e = e - 4*y; }
        e = e + 4*x + 2;
       }

    if (x == y)
    { SETARCPIX( x, y);
      SETARCPIX( x,-y);
      SETARCPIX(-x, y);
      SETARCPIX(-x,-y);
     }
  }

  if (r0 < r1) r0 = r1;
  FlushDisplay(win,x0-r0,y0-r0,x0+r1,y0+r1);
}




/*
void x_fill_circle0(Window w, int x0, int y0, int r)
{ 
  xx_win* win = wlist[w];
  int y = 1;
  int x = r;
  int e = 3-2*r;

  hline(win,x0-x,x0+x,y0);

  while (y<=x)
  { hline(win,x0-x,x0+x,y0+y);
    hline(win,x0-x,x0+x,y0-y);

    if (y<x && e>=0)
    { hline(win,x0-y,x0+y,y0+x);
      hline(win,x0-y,x0+y,y0-x);
      x--;
      e = e - 4*x;
     }
    y++;
    e = e + 4*y + 2;
   }
  FlushDisplay(win,x0-r,y0-r,x0+r,y0+r);
}
*/


void x_pixel(Window w, int x, int y) 
{ xx_win* win =wlist[w];
  win->canvas->setpix(x,y,win->COLOR,win->MODE);
  FlushDisplay(win,x,y,x,y);
}


int x_get_pixel(Window w, int x, int y) 
{ xx_win* win =wlist[w];
  return (int)win->canvas->getpix(x,y);
}

void x_pixels(Window w, int n, int* x, int* y)
{ if (n <=0 ) return;

  xx_win* win = wlist[w];
  x_image* canvas = win->canvas;
  int clr = win->COLOR;
  int mode = win->MODE;

  int xmin = *x;
  int xmax = *x;
  int ymin = *y;
  int ymax = *y;
  while(n--) 
  { if (xmin > *x) xmin = *x;
    if (xmax < *x) xmax = *x;
    if (ymin > *y) ymin = *y;
    if (ymax < *y) ymax = *y;
    canvas->setpix(*x++,*y++,clr,mode); 
   }
  FlushDisplay(win,xmin,ymin,xmax,ymax);
}


static void draw_ellipse(xx_win* win, int x0, int y0, int a, int b, bool fill)
{
  /* Foley, van Dam, Feiner, Huges: Computer Graphics, page 90 */

  double xc = x0;
  double yc = y0;
  double rx = a;
  double ry = b;

  double rx_2 = rx*rx;
  double ry_2 = ry*ry;

/*
  bigpix(win, xc, yc-ry);
  bigpix(win, xc, yc+ry);
  bigpix(win, xc-rx, yc);
  bigpix(win, xc+rx, yc);
*/

  double x = 0;
  double y = ry;

  double d1 = ry_2 + rx_2*(0.25 - b); 
   
  while (rx_2*(y - 0.5) > ry_2*(x+1))
  { if (d1 < 0)
      d1 += ry_2*(2*x + 3);
    else
    { d1 += ry_2*(2*x + 3) + rx_2*(2 - 2*y);
      y--;
     }

    x++;

    if (fill)
    { hline(win,int(xc-x),int(xc+x),int(yc+y));
      hline(win,int(xc-x),int(xc+x),int(yc-y));
     }
    else
    { bigpix(win,int(xc+x),int(yc+y));
      bigpix(win,int(xc-x),int(yc+y));
      bigpix(win,int(xc+x),int(yc-y));
      bigpix(win,int(xc-x),int(yc-y));
     }
  }

  double d2 = ry_2*(x+0.5)*(x+0.5) + rx_2*(y - 1)*(y - 1) - rx_2*ry_2;

  while (y > 0)
  { if (d2 < 0)
    { d2 += ry_2*(2*x+2)+rx_2*(3-2*y);
      x++;
     }
    else
      d2 += rx_2*(3-2*y);

    y--;

    if (fill)
    { hline(win,int(xc-x),int(xc+x),int(yc+y));
      hline(win,int(xc-x),int(xc+x),int(yc-y));
     }
    else
    { bigpix(win,int(xc+x),int(yc+y));
      bigpix(win,int(xc-x),int(yc+y));
      bigpix(win,int(xc+x),int(yc-y));
      bigpix(win,int(xc-x),int(yc-y));
     }
  }

}


static void fill_ellipse(xx_win* win, int xc, int yc, int a, int b)
{
  // set all pixels in bounding box lying inside ellipse

  x_image* canvas = win->canvas;

  int xmin = xc - a;
  int xmax = xc + a;
  int ymin = yc - b;
  int ymax = yc + b;

  for(int x = xmin; x <= xmax; x++)
    for(int y = ymin; y <= ymax; y++)
      if (INSIDE_ELLIPSE(xc,yc,x,y,a,b))
          canvas->setpix(x,y,win->COLOR,win->MODE);
}



void x_ellipse(Window w, int x, int y, int a, int b)
{ xx_win* win = wlist[w];
  draw_ellipse(win,x,y,a,b,false);
  a += win->LINEWIDTH;
  b += win->LINEWIDTH;
  FlushDisplay(win,x-a,y-b,x+a,y+b);
}


void x_fill_ellipse(Window w, int x, int y, int a, int b)
{ xx_win* win = wlist[w];
  fill_ellipse(win,x,y,a,b);
  FlushDisplay(win,x-a,y-b,x+a,y+b);
}




//------------------------------------------------------------------------------
// bitmaps & pixrects
//------------------------------------------------------------------------------

static unsigned char rev_byte(unsigned char c)
{ unsigned char c1 = 0x00;
   for(int i=0; i<8; i++)
   { c1 <<= 1;
     if (c&1) c1 |= 1;
     c >>= 1;
    }
  return c1;
 }

void x_set_pixrect_alpha(char* p, int) {}

void x_get_pixrect_anchor(char* p, int& x, int& y) {
    x = ((x_image*)p)->anchor_x;
    y = ((x_image*)p)->anchor_y;
}

void x_set_pixrect_anchor(char* p, int x, int y) {
    ((x_image*)p)->anchor_x = x;
    ((x_image*)p)->anchor_y = y;
}

void x_set_pixrect_phi(char* p, float phi) {
  ((x_image*)p)->phi = phi;
}



char* x_create_pixrect_from_bits(Window w, int wi, int he, unsigned char* data,
                                                                        int fg, 
                                                                        int bg)
{ //xx_win* win = wlist[w];
  x_image* img = new x_image((pixel*)NULL,wi,he);
  return (char*)img;
 }


char* x_create_pixrect(Window w, int x0, int y0, int x1, int y1)
{ xx_win* win = wlist[w];

  if (x0 > x1) std::swap(x0,x1);
  if (y0 > y1) std::swap(y0,y1);

  int wi = x1-x0+1;
  int he = y1-y0+1;


  x_image* img = new x_image(wi,he);

  pixel* p = img->buf;

  //for(int y = y0; y < y1; y++)
  for(int y = y0; y <= y1; y++)
  { pixel* q = win->canvas->buf + x0 + y*win->canvas->w;
    for(int x = x0; x <= x1; x++) *p++ = *q++;
   }

  return (char*)img;
 }

char* x_root_pixrect(int x0, int y0, int x1, int y1) 
{ return x_create_pixrect(0,x0,y0,x1,y1); }


void x_get_pixrect_size(char* p, int& w, int& h)
{ x_image* img = (x_image*)p;
  if (img)
  { w = img->w;
    h = img->h;
   }
  else w = h = 0;
}


void x_insert_pixrect(Window w, int x, int y, char* p)
{ xx_win* win = wlist[w];
  if (p == NULL) return; 
  x_image* img = (x_image*)p;

  y -= (img->h -1);

  x -= img->anchor_x;
  y += img->anchor_y;

  CopyArea(img,0,0,win->canvas,x,y,img->w,img->h,img->use_alpha);
  FlushDisplay(win,x,y,x+img->w,y+img->h);
}


void x_insert_pixrect(Window w, char* p)
{ xx_win* win = wlist[w];
  if (p == NULL) return; 
  x_image* img = (x_image*)p;
  CopyArea(img,0,0,win->canvas,0,0,img->w,img->h,img->use_alpha);
  FlushDisplay(win,0,0,img->w,img->h);
}


void x_insert_pixrect(Window w, int x, int y, char* p, int x0, int y0, 
                                                       int width, int height)
{ xx_win* win = wlist[w];
  assert(p != NULL);
  x_image* img = (x_image*)p;
  y -= (height-1);
  CopyArea(img,x0,y0,win->canvas,x,y,width,height,img->use_alpha);
  FlushDisplay(win,x,y,width,height);
}


void  x_insert_pixrect(Window w, int x, int y, int wi, int he, char* p)
{
  // scale pixrect into box (x,y,x+wi,y-he) (upper left corner)

  xx_win* win = wlist[w];
  x_image* im = (x_image*)p;

  if (wi == im->w && he == im->h && im->phi == 0) 
  { x_insert_pixrect(w,x,y,p);
    return;
   }

  int x0 = (x < 0) ? -x : 0;
  int x1 = x0 + x_window_width(w);

  int y0 = he - y;
  if (y0 < 0) y0 = 0; 
  int y1 = y0 + x_window_height(w);

  x_image* im1 = im->resize_rotate(wi,he,im->phi,x0,y0,x1,y1);
  x_insert_pixrect(w,x,y,(char*)im1);
  delete im1;
}


void x_delete_pixrect(char* p) 
{ x_image* img_ptr = (x_image*)p; 
  delete img_ptr;
}


void x_pixrect_to_matrix(Window w, char* prect, int* matrix) {}

void x_matrix_to_pixrect(Window w, int* matrix, int width, int height,
                                   char* prect) {}


unsigned char* scale_bitmap(int f, unsigned char* bits, int width, int height)
{
 int bw1 = (width+7)/8;

 int w = f*width;
 int h = f*height;
 int bw = (w+7)/8;

 unsigned char* result_bits = new unsigned char[bw*h];

 for(int i=0; i<h; i++)
   for(int j=0; j<bw; j++)
   { char x = bits[(i/f)*bw1 + j/f]; // source byte
     int y = 0;
     for(int k = 0; k<8; k++) {
       if (x & (1<<k)) y |=  ((1<<f)-1) << f*k;
     }
     unsigned char* z = (unsigned char*)&y;
     result_bits[i*bw + j] = z[j%f];
    }

  return result_bits;
}


char* x_create_bitmap(Window, int width, int height, unsigned char* p, int f)
{ 
  if (f > 1)
  { p = scale_bitmap(f,p,width,height);
    width *= f;
    height *= f;
  }

  int sz = height * ((width+7)/8); // bytes

  unsigned char* bmp = new unsigned char[sz];
  for(int i=0; i<sz; i++) bmp[i] = rev_byte(*p++);

  return (char*) new x_image(bmp,width,height);
}

char* x_create_pixrect_from_bgra(Window w, int wi, int he, unsigned char* bgra, 
                                                           float f ) 
{ int sz = wi*he;
  pixel* p = new pixel[sz];
  memcpy(p,bgra,4*sz);
  x_image* im = new x_image(p,wi,he); 
  im->use_alpha = true;
  x_image* im1 = im->resize_rotate(int(0.5+f*im->w),int(0.5+f*im->h),0);
  delete im;
  return (char*)im1; 
}


unsigned char* x_pixrect_to_bgra(Window w, char* prect) { 
  x_image* im = (x_image*)prect;
  int sz = im->w * im->h;
  unsigned int* bgra = new unsigned int[sz];
  memcpy(bgra,im->buf,4*sz);
  return (unsigned char*)bgra; 
}


void x_insert_bitmap(Window w, int x, int y, char* bm)
{ xx_win* win = wlist[w];

  x_image* img = (x_image*)bm;
  unsigned char* bmp = img->bmp;

  int width  = img->w;
  int height = img->h;
  int lbytes = img->size/height;

  y -= (height-1);

/*
  // NOT NECESSARY:  canvas does clipping anyway (setpix)
  // clipping on a copy
  unsigned char* bmp1 = new unsigned char[height*lbytes];
  for(int i=0; i < height*lbytes; i++) bmp1[i] = bmp[i];

  clip_bitmap(win->canvas,x,y,bmp1,lbytes,height);

  for(int i=0; i<height; i++)
     bitmap_line(win,x,y+i,bmp1+i*lbytes,lbytes);
  delete[] bmp1;
*/

  for(int i=0; i<height; i++)
     bitmap_line(win,x,y+i,bmp+i*lbytes,lbytes);

  FlushDisplay(win,x,y,x+width,y+height);
}


void x_delete_bitmap(char* bm) { x_delete_pixrect(bm); }


/*
// unused
void x_insert_bitmap(Window w, int x, int y, int width, int height, 
                                                        unsigned char* data)
{ xx_win* win = wlist[w];

  int len =  width/8;
  if (width % 8) len++;

  unsigned char* buf = new unsigned char[len*height];
  unsigned char* q = buf;
  unsigned char* stop = q + len*height;

  while (q < stop) *q++ = rev_byte(*data++);

  clip_bitmap(win->canvas,x,y,buf,len,height);

  for(int i=0; i<height; i++)
    bitmap_line(win,x,y+i,buf+i*len,len);

  FlushDisplay(win,x,y,x+len,y+height);

  delete[] buf;
}
*/

void x_text(Window w, int x, int y, const char *txt)
{ xx_win* win = wlist[w];

  x_image* canv = win->canvas;

  int x1 = x + x_text_width(w,txt);
  int y1 = y + x_text_height(w,txt);

  if (win->TEXTMODE == opaque)  // clear background
  { int save_color = x_set_color(w,win->bg_clr);
    drawing_mode save_mode = x_set_mode(w,src_mode);
    x_box(w,x,y,x1,y1);
    x_set_mode(w,save_mode);
    x_set_color(w,save_color);
   }

  int xmin = canv->clip_x0;
  int xmax = canv->clip_x1 - 5;

/*
  int offset = 0;
  for(int i=0; i<128; i++) {
    assert(offset == FONT_OFFSET[i]);
    offset += win->FONT_WIDTH[i] * FONT_HEIGHT;
  }
*/

  int xx = x;
  for (unsigned int i = 0; i < strlen(txt); i++) 
  { if (xx < xmin || xx > xmax) continue;
    int c = txt[i] & 127;
    int tw  = win->FONT_WIDTH[c];
    int th  = win->FONT_HEIGHT;
    int off = win->FONT_OFFSET[c];

    int pix_num = tw*th;
    pixel* pixels = new pixel[pix_num];

    for(int j=0;j<pix_num;j++)
    { int alpha = win->FONT_ALPHA[off+j];
      pixels[j] = win->COLOR | (alpha << 24);
     }

    x_image* img = new x_image(pixels,tw,th);

    float f = win->FONT_SCALE;
    if (f != 1)
    { x_image* p = img->resize_rotate(int(0.5+f*img->w),int(0.5+f*img->h),0);
      delete img;
      img = p;
     }

    CopyArea(img,0,0,win->canvas,xx,y,img->w,img->h,true);

    xx += int(0.5 + f*tw);

    delete img;
  }

  FlushDisplay(win,x,y,x1,y1);
}


void x_text(Window w, int x, int y, const char *text, int l)
{ char* str = new char[strlen(text)+1];
  strcpy(str,text);
  if (unsigned(l) < strlen(text)) str[l] = '\0';
  x_text(w,x,y,str);
  delete[] str;
}

void x_ctext(Window w, int x, int y, const char* str)
{ x_text(w,x-(x_text_width(w,str)-1)/2, y-(x_text_height(w,str)-1)/2, str); }

void x_text_underline(Window w, int x, int y, const char* text, int, int)
{ x_text(w,x,y,text); }

void x_ctext_underline(Window w, int x, int y, const char* text, int, int)
{ x_ctext(w,x,y,text); }




void x_set_clip_rectangle(Window w, int x ,int y, int width, int height) 
{ 
/*
  printf("clip_rect(w = %d  x = %d y = %d  w = %d  h = %d)\n",
                    w,x,y,width,height);
*/

  xx_win* win = wlist[w];
  x_image* canv = win->canvas;

  assert(canv != 0);

  if (y < 0) y = 0;
  if (y >= canv->h) y = canv->h - 1;
  if (x < 0) x = 0;
  if (x >= canv->w) y = canv->w - 1;

  canv->clip_x0 = x;
  canv->clip_y0 = y;
  int dx = canv->w - (x+width);
  int dy = canv->h - (y+height);
  if (dx < 0) width  += dx;
  if (dy < 0) height += dy;
  canv->clip_x1 = x + width - 1;
  canv->clip_y1 = y + height - 1;
}


static void set_clipping_params(xx_win* win, int clip_mode)
{ 
  if (clip_mode == 0) {
    // set clip bit: removes point from clipping area 
    win->COLOR = 0x01000000;
    win->MODE = or_mode;
  }
  else
  { // unset clip bit: adds point to clipping area
    win->COLOR = ~0x01000000;
    win->MODE = and_mode;
   }
}

void x_clip_mask_rectangle(Window w, int x0,int y0,int x1,int y1,int clip_mode)
{ xx_win* win = wlist[w];
  pixel clr = win->COLOR;
  drawing_mode mode = win->MODE;
  set_clipping_params(win,clip_mode);
  x_box(w,x0,y0,x1,y1);
  win->COLOR = clr;
  win->MODE = mode;
 }


void x_clip_mask_ellipse(Window w, int x, int y, int a, int b, int clip_mode) 
{ xx_win* win = wlist[w];
  pixel clr = win->COLOR;
  drawing_mode mode = win->MODE;
  set_clipping_params(win,clip_mode);
  fill_ellipse(win,x,y,a,b);
  win->COLOR = clr;
  win->MODE = mode;
}

void x_clip_mask_polygon(Window w, int n, int* xc, int* yc, int clip_mode) 
{ xx_win* win = wlist[w];

  if (n == 0) 
  { // empty polygon: entire canvas
    x_image* canv = win->canvas;
    x_clip_mask_rectangle(w, 0, 0, canv->w, canv->h,clip_mode);
    return;
  }

  pixel clr = win->COLOR;
  drawing_mode mode = win->MODE;
  set_clipping_params(win,clip_mode);
  x_fill_polygon(w,n,xc,yc);
  win->COLOR = clr;
  win->MODE = mode;

}


void x_set_read_gc(Window w)
{ xx_win* win = wlist[w];
  win->save_clr  = x_set_color(w,black);
  win->save_mode = x_set_mode(w,xor_mode);
  win->save_ls   = x_set_line_style(w,solid);
  win->save_lw   = x_set_line_width(w,1);
 }


void x_reset_gc(Window w)
{ xx_win* win = wlist[w];
  x_set_color(w,win->save_clr);
  x_set_mode(w,win->save_mode);
  x_set_line_style(w,win->save_ls);
  x_set_line_width(w,win->save_lw);
 }


void x_screen_to_window(Window w, int& x, int& y) 
{ xx_win* win = wlist[w];
  x -= win->xpos;
  y -= win->ypos;
 }

void x_window_to_screen(Window w, int& x, int& y)
{ xx_win* win = wlist[w];
  x += win->xpos;
  y += win->ypos;
 }

int   x_set_bg_color(Window w, int c) 
{ xx_win* win = wlist[w];
  int old_c = win->bg_clr;
  win->bg_clr = c;
  return old_c;
}

char* x_set_bg_pixmap(Window w, char* pm) 
{ xx_win* win = wlist[w];
  char* old_pm = win->bg_pixmap;
  win->bg_pixmap = pm;
  return old_pm;
}


void x_set_border_color(Window w, int c) 
{ xx_win* win = wlist[w];
  win->border_clr = c;
}

void x_set_border_width(Window w, int width) 
{ xx_win* win = wlist[w];
  win->border_w = width;
}



// buffering

int x_create_buffer(Window w, int wi, int he) 
{ xx_win* win = wlist[w];

  if (win->buffer != 0 && wi == win->buffer->w && he == win->buffer->h) {
    // reuse existing buffer
    return 0;
  }

  // allocate new buffer

  if (win->buffer) x_delete_buffer(w);
  win->buffer = new x_image(wi,he);
  if (win->canvas_save) win->canvas = win->buffer;
  return 1;
 }

int x_create_buffer(Window w) 
{ int wi = x_window_width(w);
  int he = x_window_height(w);
  return x_create_buffer(w,wi,he);
 }


void x_delete_buffer(Window w)
{ xx_win* win = wlist[w];
  x_stop_buffering(w);
  if (win->buffer) 
  { delete win->buffer;
    win->buffer = 0;
   }
}


int   x_start_buffering(Window w, int wi ,int he)
{ xx_win* win = wlist[w];
  assert(win != root_win);
  x_stop_buffering(w);
  x_delete_buffer(w);
  x_create_buffer(w,wi,he);
  win->canvas_save = win->canvas;
  win->canvas = win->buffer;
  win->flush = false;
  return 1;
}

int   x_start_buffering(Window w) 
{ xx_win* win = wlist[w];
  assert(win != root_win);
  if (win->canvas_save) return 0;
/*
  int new_buf = (win->buffer == 0);
  if (new_buf) x_create_buffer(w);
*/
  int new_buf = x_create_buffer(w);
  win->canvas_save = win->canvas;
  win->canvas = win->buffer;
  win->flush = false;
  return new_buf;
}


void x_flush_buffer(Window w,int x0,int y0,int x1,int y1, int xoff, int yoff)
{ xx_win* win = wlist[w];

  assert(win != root_win);

  if (win->buffer == 0) return; 

  if (y0 > y1) std::swap(y0,y1);
  if (x0 > x1) std::swap(x0,x1);

  int wi = x1-x0+1;
  int he = y1-y0+1;

  if (win->canvas_save )
  { // buffering on
    CopyArea(win->buffer,x0+xoff,y0+yoff,win->canvas_save,x0,y0,wi,he);
   }
  else
  { // buffering has been finished before
    CopyArea(win->buffer,x0+xoff,y0+yoff,win->canvas,x0,y0,wi,he);
   }

  x0 += xoff;
  x1 += xoff;
  y0 += yoff;
  y1 += yoff;

  bool flush = win->flush;
  win->flush = true;
  FlushDisplay(win,x0,y0,x1,y1);
  win->flush = flush;

}


void x_flush_buffer(Window w ,int x0, int y0, int x1, int y1) 
{ x_flush_buffer(w,x0,y0,x1,y1,0,0); }


void  x_stop_buffering(Window w)
{ xx_win* win = wlist[w];
  if (win->canvas_save)
  { win->canvas = win->canvas_save;
    win->canvas_save = 0;
   }
  win->flush = true;
}

void  x_stop_buffering(Window w, char** ppr)  
{ xx_win* win = wlist[w];
  if (win->canvas_save)
  { win->canvas = win->canvas_save;
    win->canvas_save = 0;
    *ppr = (char*)win->buffer;
    win->buffer = 0;
  }
  else *ppr = 0;
  win->flush = true;
}

int   x_test_buffer(Window w) 
{ xx_win* win = wlist[w];
  return win->canvas_save != 0; 
 }

void  x_set_buffer(int,char*) {}


void x_window_frame(int w, int& x0, int& y0, int& x1, int& y1) 
{ xx_win* win = wlist[w];
  x0 = win->x0;
  y0 = win->y0;
  x1 = win->x1;
  y1 = win->y1;
}



//------------------------------------------------------------------------------
// window manager
//------------------------------------------------------------------------------

const int ROOT_COLOR = blue2;   
const int LABEL_COLOR = grey2;

static int mouse_x = 0;
static int mouse_y = 0;


static int handle_next_event(int* win, int* x, int* y, int* val1, int* val2, 
                                                    unsigned long* t, int msec)
{ // msec > 0: blocking / msec timeout
  // msec = 0: blocking without timeout
  // msec < 0: non blocking

  *val2 = 0;

  // find next event

  string e_name = "";
  int e = no_event;

  int cur_x = last_event.x;
  int cur_y = last_event.y;

  if (event_buf.ev != -1)
  { e = event_buf.ev;
    *val1 = event_buf.val;
    cur_x = event_buf.x;
    cur_y = event_buf.y;
    *t = event_buf.t;
    event_buf.ev = -1;
   }
  else {
    e = NEXT_EVENT(e_name,*val1,cur_x,cur_y,*t,msec);
  }


  if (e_name.starts_with("upload:") && wlist[0]->special_event_handler) 
  { // cout << "SPECIAL_EVENT: " << e_name << endl;
    // e_name = "upload:file"

    string A[2];
    e_name.split(A,2,':');
    string fname = A[1].trim();

    unsigned long i = wlist[0]->special_event_data;
    xx_win* wp = wlist[i]; 
    wlist[0]->special_event_handler(wp->inf,"upload",fname,0,0);
    return no_event;
  }
    

  if (e == display_event)
  { //cout << string ("DISPLAY EVENT: %d %d %d",*val1,cur_x,cur_y) << endl;

    if (*val1 > 0) {
    // display size changed
    //cout << string ("DISPLAY CHANGED: %d x %d (%d)",cur_x,cur_y,*val1);
      redraw_root(1);
    }

  //cout << "DISPLAY CLOSED" << endl;

    *win = 0;
    return e;
  }


  if (e != no_event)
  { last_event.ev  = e;
    last_event.val = *val1;
    last_event.x = cur_x;
    last_event.y = cur_y;
    last_event.t = *t;
    if (e != key_press_event && e != key_release_event)
    { mouse_x = cur_x;
      mouse_y = cur_y;
     }
  }


  // find window of this event

  xx_win* w = grab_win;

  if ((e == key_press_event || e == key_release_event) && focus_window != -1) {
    w = wlist[focus_window];
  }

  if (w == 0)
  { for(int i = win_top; i >= 0; i--)
    { w = wstack[i];
      if (mouse_x >= w->x0  && mouse_x <= w->x1 &&
          mouse_y >= w->y0  && mouse_y <= w->y1 )  break;
     }

   }

  *win = w->id;
  *x = mouse_x - w->xpos;
  *y = mouse_y - w->ypos;

/*
  cout << event_name[e] << endl;
  cout << "win = " << *win << " val = " << *val1;
  cout << " x = " << *x << " y = " << *y << "  t = " << *t << endl;
*/

  return e;
}



static void draw_window_button(xx_win* win, int i)
{
  if (win->state & 0x01) return;

  int x1 = win->x1 - BUTTON_X[i]; 
  int x2 = x1 + BUTTON_W;
  int y1 = win->y0 + BUTTON_Y;
  int y2 = y1 + BUTTON_W;

//x_set_line_width(0,3);

//int lw = int(0.5 + display_dpi/50.0);
  int lw = int(1 + display_dpi/60.0);
  x_set_line_width(0,lw);

  int clr = white;
  int bg_clr = win->label_clr;

  if (active_win_button == i) clr = 0x333333;

  switch (i) {
 
   case 0: x1-=2; x2++;
           x_set_color(0,clr);
           x_line(root_win->id,x1,(y1+y2)/2,x2,(y1+y2)/2);
           break;

   case 1: { int dx = 0;
             int dy = 0;
             x1--; x2++;
             if (win->state & 0x02) { dx = 4; dy = 3; y1++; y2++;}
             x_set_color(0,clr);
             x_rect(root_win->id,x1+dx,y1-dy,x2+dx,y2-dy);
             x_set_color(0,bg_clr);
             x_box(root_win->id,x1-dx,y1+dy,x2-dx,y2+dy);
             x_set_color(0,clr);
             x_rect(root_win->id,x1-dx,y1+dy,x2-dx,y2+dy);
             break;
            }

   case 2: x_set_color(0,clr);
           x_line(root_win->id,x1,y1,x2,y2);
           x_line(root_win->id,x1,y2,x2,y1);
           break;
  }

}


static int check_window_button(xx_win* win)
{
  if (win->y0+BUTTON_Y-10 > mouse_y || win->y0+BUTTON_Y+BUTTON_W+10 < mouse_y)
    return 0;

  int but = 0;
  for(int i=0; i<3; i++) {
    if (win->x1-BUTTON_X[i]-10 <= mouse_x && 
        win->x1-BUTTON_X[i]+BUTTON_W+10 >= mouse_x) but = i+1;
  }

  return but;
}


static void set_window_label(Window w, const char* s)
{
  xx_win* win = wlist[w];

  if (win->pwin || (win->state & 0x01)) return;

  int          clr = x_set_color(0,win->label_clr);
  text_mode    tm = x_set_text_mode(0,transparent);
  drawing_mode mode = x_set_mode(0,src_mode);

//x_set_font(0,"T32"); 
  x_set_font(0,"B32"); 

  int label_w = win->x1 - win->x0 - int(1.5*display_dpi);
  int label_h = x_text_height(0,"H");

  int label_x = win->x0 + int(0.19*display_dpi);
  int label_y = win->y0 + int(0.5*(HEADER_W - label_h)) + 2;

  x_set_color(0,win->label_clr);
  x_box(root_win->id,label_x,label_y,label_x+label_w,label_y+label_h);

  int len = strlen(s);
  while (x_text_width(0,s,len) > label_w) len--;

  if (s != win->header) {
    strncpy(win->header,s,256);
    win->header[255] = '\0';
  }

  x_set_color(0,0x333333);
  x_text(root_win->id,label_x,label_y,win->header,len);

  x_set_text_mode(0,tm);
  x_set_color(0,clr);
  x_set_mode(0,mode);

}

void x_set_label(Window w, const char* s)
{ xx_win* win = wlist[w];
  root_win->flush = false;
  set_window_label(w,s);
  root_win->flush = true;
  FlushDisplay(root_win,win->x0,win->y0,win->x1,win->y0+HEADER_W);
}


static void draw_window(xx_win* win, int clear_win=1)
{
  assert(win != root_win);

  if (win->state & 0x01)
  { // iconified

    int icon_w, icon_h;
    x_get_pixrect_size(win->icon_pixmap,icon_w,icon_h);
    x_insert_pixrect(0,win->xpos,win->ypos+icon_h-1,win->icon_pixmap);

    int save_lw = x_set_line_width(0,1);
    x_set_color(0,black);
    x_rect(0,win->x0,win->y0,win->x1,win->y1);
    x_set_line_width(0,save_lw);

    // icon label

    x_set_font(0,"T30"); 

    char str[64];
    strncpy(str,win->label,64);
    str[63] = '\0';

    int sz = strlen(str);
    while (x_text_width(0,str) > icon_w) str[--sz] = 0;

    int label_x = (win->x0 + win->x1)/2;
    int label_y = win->y1 + int(0.7*x_text_height(0,"H"));
    x_set_color(0,white);
    x_ctext(0,label_x,label_y,str);
    x_set_color(0,black);
    return;
   }

  int border_w  = win->xpos - win->x0 - 2;
  int label_h  = win->ypos - win->y0 - 2;

  int x0 = win->x0;
  int y0 = win->y0;

  int x1 = win->x1;
  int y1 = win->y1;

  drawing_mode save_mode = x_set_mode(0,src_mode);
  int save_lw = x_set_line_width(0,1);

  if (clear_win)
  { x_set_color(0,win->bg_clr);
  //x_box(x0,y0,x1,y1);
    x_box(0,win->xpos, win->ypos, win->xpos+win->width-1, 
                                  win->ypos+win->height-1);
   }

  if (border_w > 1)
  { 
    int bw = border_w + 1;
    int bw1 = int(2.5*border_w);

    x_set_color(0,grey1);
    x_box(root_win->id,x0,y0,x1,y1);

    x_set_color(0,win->label_clr);
    x_box(root_win->id,x0+bw,y0+bw,x1-bw,y0+label_h);
    x_box(root_win->id,x0,y0+bw1,x0+bw,y1-bw1);
    x_box(root_win->id,x1-bw,y0+bw1,x1,y1-bw1);
    x_box(root_win->id,x0+bw1,y1-bw,x1-bw1,y1);
    x_box(root_win->id,x0+bw1,y0,x1-bw1,y0+bw);

    //x_set_color(0,black);
    x_set_color(0,grey3);

    x_rect(root_win->id,x0,y0,x1,y1);

    x_line(root_win->id,x0+bw,y0+bw,x0+bw1,y0+bw);
    x_line(root_win->id,x0+bw1,y0+bw,x0+bw1,y0+1);
    x_line(root_win->id,x0+bw1,y0+1,x1-bw1,y0+1);
    x_line(root_win->id,x1-bw1,y0+1,x1-bw1,y0+bw);
    x_line(root_win->id,x1-bw1,y0+bw,x1-bw,y0+bw);
    x_line(root_win->id,x1-bw,y0+bw,x1-bw,y0+bw1);
    x_line(root_win->id,x1-bw,y0+bw1,x1-1,y0+bw1);
    x_line(root_win->id,x1-1,y0+bw1,x1-1,y1-bw1);
    x_line(root_win->id,x1-1,y1-bw1,x1-bw,y1-bw1);
    x_line(root_win->id,x1-bw,y1-bw1,x1-bw,y1-bw);
    x_line(root_win->id,x1-bw,y1-bw,x1-bw1,y1-bw);
    x_line(root_win->id,x1-bw1,y1-bw,x1-bw1,y1-1);
    x_line(root_win->id,x1-bw1,y1-1,x0+bw1,y1-1);
    x_line(root_win->id,x0+bw1,y1-1,x0+bw1,y1-bw);
    x_line(root_win->id,x0+bw1,y1-bw,x0+bw,y1-bw);
    x_line(root_win->id,x0+bw,y1-bw,x0+bw,y1-bw1);
    x_line(root_win->id,x0+bw,y1-bw1,x0+1,y1-bw1);
    x_line(root_win->id,x0+1,y1-bw1,x0+1,y0+bw1);
    x_line(root_win->id,x0+1,y0+bw1,x0+bw,y0+bw1);
    x_line(root_win->id,x0+bw,y0+bw1,x0+bw,y0+bw);
  }

  if (win->border_w > 0)
  { x_set_color(0,win->border_clr);
    x_rect(root_win->id, win->xpos-1, win->ypos-1,
                         win->xpos+win->width, win->ypos+win->height);
   }

  if (label_h > 15) {
    for(int i=0; i<3; i++) draw_window_button(win,i);
    set_window_label(win->id,win->header);
   }
  
  x_set_mode(0,save_mode);
  x_set_line_width(0,save_lw);

/*
  if (win->redraw) win->redraw(win->inf,0,0,win->width,win->height,1);
*/

  x_image* canv = win->canvas;
  if (win->canvas_save) canv = win->canvas_save;

  CopyWindowAreaToRoot(win, 0,0,canv->w-1,canv->h-1);
}



void redraw_root(int x0, int y0, int x1, int y1)
{
  //cout << string("redraw_root(%d,%d,%d,%d): ",x0,y0,x1,y1) << endl; 

  root_win->flush = false;

  // clear root window
  root_win->canvas->fill(root_win->bg_clr);

  root_win->COLOR = black;

  // draw windows
  for(int i=1; i<=win_top; i++) 
  { xx_win* wp = wstack[i];
    if (wp->pwin && (wp->pwin->state & 0x01)) continue;
    draw_window(wp);
   }

  // flush to display

  x_image* root_canv = root_win->canvas;

  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 >= root_canv->w) x1 = root_canv->w - 1;
  if (y1 >= root_canv->h) y1 = root_canv->h - 1;

  root_win->flush = true;
  UPDATE_DISPLAY(root_canv->buf,root_canv->w,root_canv->h,x0,y0,x1,y1,x0,y0);

}


void redraw_root(int x) { 
  //cout << x << ": " << std::flush;
  redraw_root(root_win->x0,root_win->y0,root_win->x1,root_win->y1); 
}



static bool move_win(xx_win* win)
{ 
  int xp0 = win->x0;
  int yp0 = win->y0;
  int xp1 = win->x1;
  int yp1 = win->y1;
  int wi  = xp1-xp0+1;
  int he  = yp1-yp0+1;

  int xc = mouse_x; /* absolute cursor coordinates */
  int yc = mouse_y; 
  int dx = xc-xp0;  /* relative to upper left corner */
  int dy = yc-yp0;

  root_win->MODE = xor_mode;
  root_win->COLOR = root_win->bg_clr;

  int lw = 2;

  root_win->LINEWIDTH = lw;
  x_rect(0,xp0-1,yp0-1,xp1+1,yp1+1);

  x_image* root_canv = root_win->canvas;
  root_win->flush = false;

  Window w;
  int e,x,y,val1,val2;
  unsigned long t;

  do { e = handle_next_event(&w,&x,&y,&val1,&val2,&t,0);
      if (mouse_x != xc || mouse_y != yc)
       { int ax0 = mouse_x-dx-1;
         int ay0 = mouse_y-dy-1;
         int ax1 = mouse_x-dx+wi;
         int ay1 = mouse_y-dy+he;

         int bx0 = xc-dx-1;
         int by0 = yc-dy-1;
         int bx1 = xc-dx+wi;
         int by1 = yc-dy+he;

         x_rect(0,ax0,ay0,ax1,ay1);
         x_rect(0,bx0,by0,bx1,by1);

         int fx0 = min(ax0,bx0) - lw;
         int fy0 = min(ay0,by0) - lw;
         int fx1 = max(ax1,bx1) + lw;
         int fy1 = max(ay1,by1) + lw;
      
         UPDATE_DISPLAY(root_canv->buf,root_canv->w,root_canv->h,
                        fx0,fy0,fx1,fy1,fx0,fy0);

         xc = mouse_x;
         yc = mouse_y;
       }
     } while (e != button_release_event);

  root_win->flush = true;

  xc -= dx;
  yc -= dy;

  x_rect(0,xc-1,yc-1,xc+wi,yc+he);

  root_win->MODE = src_mode;
  root_win->COLOR = black;
  root_win->LINEWIDTH = 1;

  dx = xc - win->x0; 
  dy = yc - win->y0; 

  if (dx*dx + dy*dy <= 25) {
    // movement below threshold
    return false;
  }

  win->x0 += dx;
  win->y0 += dy;
  win->x1 += dx;
  win->y1 += dy;
  win->xpos += dx;
  win->ypos += dy;

  if (!(win->state & 0x01))
  { // move child windows
    for(int i=1; i<=win_count; i++)
    { xx_win* wp = wlist[i];
      if (wp == 0 || wp->pwin != win) continue;
      wp->x0 += dx;
      wp->y0 += dy;
      wp->x1 += dx;
      wp->y1 += dy;
      wp->xpos += dx;
      wp->ypos += dy;
    }
   }
  
  redraw_root(2);

  return true;
}


static void resize_win(xx_win* win,int pos)
{ 
  // pos = 0,1,2,3 (corner)

  int xp0 = win->x0;
  int yp0 = win->y0;
  int xp1 = win->x1;
  int yp1 = win->y1;
  int xb = win->xpos - xp0;
  int yb = win->ypos - yp0;
  
  int xc  = mouse_x; /* absolute cursor coordinates */
  int yc  = mouse_y;


  root_win->MODE = xor_mode;
  root_win->COLOR = root_win->bg_clr;

  int lw = 2;

  root_win->LINEWIDTH = lw;
  x_rect(0,xp0-1,yp0-1,xp1+1,yp1+1);

  int dx=0;
  int dy=0;

  switch(pos) {
   case 0: dx = xp0-xc; dy = yp0-yc;
           break;
   case 1: dx = xp1-xc; dy = yp0-yc;
           break;
   case 2: dx = xp1-xc; dy = yp1-yc;
           break;
   case 3: dx = xp0-xc; dy = yp1-yc;
           break;
   }

  x_image* root_canv = root_win->canvas;
  root_win->flush = false;


  Window w;
  int e,x,y,val1,val2;
  unsigned long t;

  do { e = handle_next_event(&w,&x,&y,&val1,&val2,&t,0);
       if (mouse_x != xc || mouse_y != yc)
        { 
          int fx0,fy0,fx1,fy1;

          switch(pos) {
          case 0: x_rect(0,mouse_x+dx-1,mouse_y+dy-1,xp1+1,yp1+1);
                  x_rect(0,xc+dx-1,yc+dy-1,xp1+1,yp1+1);
                  fx0 = min(mouse_x+dx-1,xc+dx-1) - lw;
                  fy0 = min(mouse_y+dy-1,yc+dy-1) - lw;
                  fx1 = xp1+1 + lw;
                  fy1 = yp1+1 + lw;
                  break;

          case 1: x_rect(0,xp0-1,mouse_y+dy-1,mouse_x+dx+1,yp1+1);
                  x_rect(0,xp0-1,yc+dy-1,xc+dx+1,yp1+1);
                  fx0 = xp0-1 - lw;
                  fy0 = min(mouse_y+dy-1,yc+dy-1) - lw;
                  fx1 = max(mouse_x+dx+1,xc+dx+1) + lw;
                  fy1 = yp1+1 + lw;
                  break;
          case 2: x_rect(0,xp0-1,yp0-1,mouse_x+dx+1,mouse_y+dy+1);
                  x_rect(0,xp0-1,yp0-1,xc+dx+1,yc+dy+1);
                  fx0 = xp0-1 - lw;
                  fy0 = yp0-1 - lw;
                  fx1 = max(mouse_x+dx+1,xc+dx+1) + lw;
                  fy1 = max(mouse_y+dy+1,yc+dy+1) + lw;

                  break;
          case 3: x_rect(0,mouse_x+dx-1,yp0-1,xp1+1,mouse_y+dy+1);
                  x_rect(0,xc+dx-1,yp0-1,xp1+1,yc+dy+1);
                  fx0 = min(mouse_x+dx-1,xc+dx-1) - lw;
                  fy0 = yp0-1 - lw;
                  fx1 = xp1+1 + lw;
                  fy1 = max(mouse_y+dy+1,yc+dy+1) + lw;
                  break;
           }

         UPDATE_DISPLAY(root_canv->buf,root_canv->w,root_canv->h,
                        fx0,fy0,fx1,fy1,fx0,fy0);

          xc = mouse_x;
          yc = mouse_y;
        }
     } while (e != button_release_event);

     root_win->flush = true;

     switch(pos) {
     case 0: x_rect(0,xc+dx-1,yc+dy-1,xp1+1,yp1+1);
             break;
     case 1: x_rect(0,xp0-1,yc+dy-1,xc+dx+1,yp1+1);
             break;
     case 2: x_rect(0,xp0-1,yp0-1,xc+dx+1,yc+dy+1);
             break;
     case 3: x_rect(0,xc+dx-1,yp0-1,xp1+1,yc+dy+1);
             break;
     }

   root_win->MODE = src_mode;
   root_win->COLOR = black;
   root_win->LINEWIDTH = 1;

   xc += dx;
   yc += dy;

   switch(pos) {
   case 0: win->x0 = xc; win->y0 = yc; 
           break;
   case 1: win->x1 = xc; win->y0 = yc; 
           break;
   case 2: win->x1 = xc; win->y1 = yc; 
           break;
   case 3: win->x0 = xc; win->y1 = yc; 
           break;
   }

  win->xpos = win->x0 + xb;
  win->ypos = win->y0 + yb;
  win->width  = win->x1 - win->x0 - 2*xb + 1; 
  win->height = win->y1 - win->y0 - xb - yb + 1; 


  if (win->canvas != 0) 
  { x_image* new_canv = new x_image(win->width,win->height,win->bg_clr);
    delete win->canvas;
    win->canvas = new_canv;
   }

  // NOT NECESSARY if win has children --> x_resize
  redraw_root(3);
}


//------------------------------------------------------------------------------
//tooltips
//------------------------------------------------------------------------------

static void close_tt_window(int w) {
  xx_win* wp = wlist[w];
  xx_tooltip* ttp = wp->ttp_open;
  if (ttp == 0) return;

//cout << "close: " << ttp << " s = " << ttp->get_line(0)  << endl;

  x_close_window(ttp->win);
  wp->ttp_open = 0;
}

static void open_tt_window(int w, int x, int y, xx_tooltip* ttp)
{ xx_win* wp = wlist[w];
  if (ttp == wp->ttp_open) return;

  const char* s = ttp->get_line(0);

//cout << "open:  " << ttp << " s = " << s  << endl;

  close_tt_window(w);

  x_set_text_font(w);

  int width = x_text_width(w,s) + 20;
  int height = x_text_height(w,s) + 12;

  x_restore_font(w); 

  int bg_clr = 0xffffee;
  const char* header = "tooltip";
  const char* label = "tooltip";
  bool hidden = false;

  ttp->win = x_create_window(0,width,height,bg_clr,header,label,w,0);
  x_open_window(ttp->win,x+25,y+30,width,height,w,hidden);

  x_set_color(ttp->win,bg_clr);
  x_box(ttp->win,0,0,width,height);

  x_set_color(ttp->win,0x555555);
  x_set_text_font(ttp->win);
  x_ctext(ttp->win,width/2,height/2,s);

  wp->ttp_open = ttp;
}


static void check_tooltips(int w, int x, int y)
{ xx_win* wp = wlist[w];

  int i = wp->tt_top;
  while (i >= 0)
  { xx_tooltip* ttp = wp->tt_stack[i];
    if (x <= ttp->xmax && y <= ttp->ymax &&
        x >= ttp->xmin && y >= ttp->ymin) break;
    i--;
   }

  xx_tooltip* ttp = (i >= 0) ? wp->tt_stack[i] : 0;
  wp->ttp_current = ttp;
  wp->ttp_x = x;
  wp->ttp_y = y;

  if (ttp == 0) close_tt_window(w);
}


void x_set_tooltip(int win, int id, int x0, int y0, int x1, int y1, 
                                                             const char* msg) 
{ xx_win* wp = wlist[win];

  int i = wp->tt_top;
  while (i >= 0 && wp->tt_stack[i]->id != id) i--; 

  if (i >= 0) {
    // existing tooltip with this id (do not overwrite)
    return; 
  }

  // push new tooltip

  int t = wp->tt_top + 1;
  if (t < TOOLTIP_MAX)
  { wp->tt_stack[t] = new xx_tooltip(win,id,x0,y0,x1,y1,msg);
    wp->tt_top = t;
   }
}


void  x_del_tooltip(int win, int id) 
{ xx_win* wp = wlist[win];

  int i = wp->tt_top;
  while (i >= 0 && wp->tt_stack[i]->id != id) i--; 

  if (i >= 0) 
  { xx_tooltip* ttp = wp->tt_stack[i];
    if (ttp == wp->ttp_open) close_tt_window(win);
    delete ttp;
    wp->tt_stack[i] = wp->tt_stack[wp->tt_top];
    wp->tt_top--;
   }
}
  


//------------------------------------------------------------------------------
// manage events and windows  
//------------------------------------------------------------------------------


static int manage_next_event(Window* w, int* x, int* y, int* val1, int* val2,
                                                  unsigned long* t, int msec)

{
  // get next event and run a simple window manager (motion & button events)

  // msec > 0: blocking / timeout
  // msec = 0: blocking 
  // msec < 0: non-blocking 

  int e = handle_next_event(w,x,y,val1,val2,t,msec);

  if (e != motion_event && e != button_press_event && e != button_release_event)
  {
    return e;
  }

  // motion or button event


  if (*w == 0 || wlist[*w]->pwin)
  { // root or child window

    SET_CURSOR(-1);

    active_win = 0;

    if (active_win_button != -1) 
    { int but = active_win_button;
      active_win_button = -1;
      for(int i=1; i<=win_count; i++) {
        if (wlist[i]->pwin != 0) continue;
        draw_window_button(wlist[i],but);
      }
    }

    return e;
   }

  xx_win* win = wlist[*w];

  int x0 = win->x0;
  int y0 = win->y0;
  int x1 = win->x1;
  int y1 = win->y1;

  int border_w = win->xpos - win->x0; 
  int label_h = win->ypos - win->y0;  


  if ((win->state & 0x01)  && e == button_press_event) 
  { // click on icon: restore window (normal or maximized)
    active_win = 0;
    e = handle_next_event(w,x,y,val1,val2,t,msec);

    if (e != button_release_event && move_win(win)) return no_event;

    // button_release or movement below threshold
    // un-iconify window

    win->icon_x = win->x0;
    win->icon_y = win->y0;

    if (win->state == 3)  // iconized maximized window
       win->maximize(display_width, display_height); 
    else
       win->restore();

    redraw_root(4);

    *x = 0;
    *y = 0;
    *val1 = win->width;
    *val2 = win->height;
    return exposure_event;
  }

  
  // no frame border events for minimized windows
  if (win->state & 0x01) return no_event; 


  if ((x0<=mouse_x && mouse_x<=x0+border_w) || 
      (x1>=mouse_x && mouse_x>=x1-border_w) ||  
      (y0<=mouse_y && mouse_y<=y0+label_h)  || 
      (y1>=mouse_y && mouse_y>=y1-border_w) )
  {
    // pointer on window frame: raise/move or resize window

    if (active_win != 0) SET_CURSOR(-1);

    active_win = 0;

    // check resize corners

    int cx[4];
    int cy[4];
    cx[0] = x0;          cy[0] = y0;
    cx[1] = x1-RESIZE_W; cy[1] = y0;
    cx[2] = x1-RESIZE_W; cy[2] = y1-RESIZE_W;
    cx[3] = x0;          cy[3] = y1-RESIZE_W;

    int CORNER[] = { XC_top_left_corner,
                     XC_top_right_corner,
                     XC_bottom_right_corner,
                     XC_bottom_left_corner };
  
    if (win->state == 0)
    { // check resize corners only for normal windows
      for(int i=0; i<4; i++) {
        if (cy[i] <= mouse_y && mouse_y <= cy[i]+RESIZE_W &&
            cx[i] <= mouse_x && mouse_x <= cx[i]+RESIZE_W )
         { 
           SET_CURSOR(CORNER[i]);
  
           if (e == button_press_event)
           { resize_win(win,i);

             x_set_color(win->id,win->bg_clr);
             x_box(win->id,0,0,*x,*y);
             *x = 0;
             *y = 0;
             *val1 = win->width;
             *val2 = win->height;

             // resizing happened
             return exposure_event;
            }

            // no resizing
            return no_event;
          }
       }
    }

   
    // check window buttons (- o x)

    // motion_event: only highlight button under mouse pointer
    // button_press_event: execute associated action

    int but = -1;

    if (mouse_y >= win->y0+BUTTON_Y-10 && 
        mouse_y <= win->y0+BUTTON_Y+BUTTON_W+10)
    { for(int i=0; i<3; i++) {
        if (mouse_x >= win->x1-BUTTON_X[i]-10 && 
            mouse_x <= win->x1-BUTTON_X[i]+BUTTON_W+10) but = i;
      }
    }

    if (but != active_win_button) {
       active_win_button = but;
       for(int i=0; i<3; i++) draw_window_button(win,i);
     }

    if (e != button_press_event) {
      SET_CURSOR(-1);
      return no_event;
    }

    // button_press_event: button action

    switch (but) {

      case 0: { // iconize button
                active_win_button = -1;
                win->minimize();
                redraw_root(5);
                *x = 0;
                *y = 0;
                *val1 = win->width;
                *val2 = win->height;
                //return (win->state & 0x01) ? no_event : configure_event;
                return no_event;
               }

      case 1: { // maximize button
                active_win_button = -1;

                if (win->state == 0)
                  win->maximize(display_width,display_height);
                else
                  win->restore();


                *x = 0;
                *y = 0;
                *val1 = win->width;
                *val2 = win->height;

                if (win->canvas != 0) 
                { x_image* new_canv = new x_image(win->width,win->height,
                                                             win->bg_clr);
                  delete win->canvas;
                  win->canvas = new_canv;
                 }

                // NOT NECESSARY if win has children --> x_resize
                redraw_root(6);

                return exposure_event;
               }

      case 2: // close button
              return destroy_event;
    }


    // mouse button pressed on frame (not on a window button)

    if (*val1 == 1)
    { // left button: raise and move (normal) window 
      x_set_topmost(*w);
      if (win->state == 0) move_win(win); 
      return configure_event; 
     }

    if (*val1 == 3)
    { // right button: write pixels (for testing purpose)
      char tmp[512];
      strcpy(tmp,win->header);
      x_set_label(win->id,"Saving Image to File (pixels.bgra)");
      int w = x1-x0+1;
      int h = y1-y0+1;
      string fname = string("pixels-%d-%d.bgra",w,h);
      root_win->canvas->write_bin(fname,x0,y0,x1,y1);
      x_set_label(win->id,tmp);
      return no_event;
     }


  }  // pointer on frame
  else
  { 
    // pointer in client area

    xx_win* wp = wlist[*w];
    assert(wp);

    if (active_win_button != -1) 
    { int but = active_win_button;
      active_win_button = -1;
      draw_window_button(wp,but);
    }

    if (active_win != *w)
    { active_win = *w;
      if (wp) SET_CURSOR(wp->cursor);
     }

    if (e == motion_event) check_tooltips(*w,*x,*y);

  }

  return e;
}


int x_get_next_event(Window& w, int& x, int& y, int& val1, int& val2,
                                                           unsigned long &t,
                                                           int msec)
{ // get next event 

#define HANDLE_TOOLTIPS

#if defined(HANDLE_TOOLTIPS)
  if (msec == 0)
  { // blocking 

    // if we receive an event after 100 msec return it immediately

    int e = manage_next_event(&w,&x,&y,&val1,&val2,&t,100);
    if (e != no_event) return e;

    // otherwise handle tooltip windows

    xx_win* wp = wlist[w];

    for(;;) {

      unsigned long timeout = wp->ttp_open ? 1500 : 400;

      int e = manage_next_event(&w,&x,&y,&val1,&val2,&t,timeout);
      if (e != no_event) return e;

      if (wp->ttp_current && wp->ttp_current != wp->ttp_open)
         open_tt_window(w,wp->ttp_x,wp->ttp_y,wp->ttp_current);
      else
      { close_tt_window(w);
        wp->ttp_current = 0;
       }
    }
  }

#endif

  int e = manage_next_event(&w,&x,&y,&val1,&val2,&t,msec);

  if (e == no_event) {
   x = 0;
   y = 0;
   val1 = 0;
   val2 = 0;
  }

  return e;
}



void x_put_back_event()
{ event_buf.ev = last_event.ev;  
  event_buf.val = last_event.val; 
  event_buf.x = last_event.x; 
  event_buf.y = last_event.y; 
  event_buf.t = last_event.t; 
}


void x_do_not_open_display(bool b) { do_not_open_display = b; }

int x_open_display()
{ 
  if (do_not_open_display) return -1;

  if (root_win) {
    // display has been opend already
    return display_fd; 
  }

  display_fd = 
   OPEN_DISPLAY(display_width,display_height,display_depth,display_dpi);

  if (display_fd == 0) {
    exit(0);
    return -1;
  }

  if (display_fd < 0)
  { cout << "x_open_display failed" << endl;
    exit(0);
    return -1;
   }

  char* p = getenv("LEDA_DPI");
  if (p != 0) display_dpi = atoi(p);



  HEADER_W = int(display_dpi/3.5);

  BORDER_W = int(HEADER_W/4.25);
  RESIZE_W = HEADER_W/3;
  BUTTON_W = HEADER_W/3;

  BUTTON_Y    = int(0.36 * HEADER_W);
  BUTTON_X[0] = int(3.7*HEADER_W);
  BUTTON_X[1] = int(2.5*HEADER_W);
  BUTTON_X[2] = int(1.3*HEADER_W);


  mouse_x = 0;
  mouse_y = 0;

  grab_win = 0;

  // create root window (wlist[0])

  root_win = new xx_win;
  root_win->id = 0;
  root_win->flush = true;
  wlist[0] = root_win;
  wstack[0] = root_win;
  win_count = 0;
  app_count = 0;
  win_top = 0;

  root_win->inf = 0;
  root_win->xpos = 0;
  root_win->ypos = 0;
  root_win->width  = display_width;
  root_win->height = display_height;

  root_win->x0 = 0;
  root_win->y0 = 0;
  root_win->x1 = root_win->width-1;
  root_win->y1 = root_win->height-1;

  root_win->bg_clr = ROOT_COLOR;
  if (getenv("LEDA_OPEN_MAXIMIZED"))  root_win->bg_clr = 0xdddddd;

  root_win->border_clr = black;
  root_win->border_w = 1;
  root_win->cursor = -1;

  strcpy(root_win->header,"Root Window");

  root_win->COLOR = black;
  root_win->LINEWIDTH = 1;
  root_win->LINESTYLE = solid;
  root_win->MODE = src_mode;
  root_win->TEXTMODE = transparent;

  strcpy(root_win->FONT_NAME,"T38");
  strcpy(root_win->FONT_NAME_TMP,"T38");

  root_win->bg_pixmap = 0;
  root_win->icon_pixmap = 0;

  root_win->canvas = new x_image(display_width,display_height);
  root_win->canvas_save = 0;

  root_win->pwin = 0;
  root_win->state = 0;

  root_win->special_event_handler = 0;
  root_win->special_event_data = 0;

  root_win->tt_top = -1;
  root_win->ttp_open = 0;
  root_win->ttp_current = 0;

  if (win_top==0) // no windows (other than root)
     x_clear_window(0,0,0,root_win->x1, root_win->y1,0,0);

  return display_fd;
}



void x_close_display()
{ 
  // delete root_win and close display

  if (root_win == 0) {
   cout << "x_close_display: already closed." << endl;
   return;
  }

  if (root_win->icon_pixmap) delete (x_image*)root_win->icon_pixmap;
  if (root_win->bg_pixmap) delete (x_image*)root_win->bg_pixmap;
  if (root_win->canvas) delete root_win->canvas;
  if (root_win->canvas_save) delete root_win->canvas_save;
  delete root_win;
  root_win = 0;

  CLOSE_DISPLAY();
 }


void  x_send_text(const char* text) { 
  SEND_TEXT(text); 
}



int   x_window_width(Window win)  { return wlist[win]->width; }
int   x_window_height(Window win) { return wlist[win]->height; }
void* x_window_inf(Window win)    { return wlist[win]->inf; }
void* x_window_handle(Window w) { return wlist[w]; }


void x_window_position(Window win,int* x,int* y) 
{ *x = wlist[win]->x0; 
  *y = wlist[win]->y0; 
 }


Window x_create_window(void* inf, int width,int height,int bg,
                     const char* header, const char* label, int wp,
                     void (*func)(void*,int,int,int,int,int))
{ 
/*
  if (width > display_width) width = display_width;
  if (height > display_height) height = display_height;
*/

  xx_win* win = new xx_win;

/*
  wlist[++win_count] = win;
  win->id = win_count;
*/

  // find first free index >= 1 in wlist

  int index = 1;
  while (index <= win_count && wlist[index] != 0) index++;
  if (index == win_count+1) win_count = index;

  wlist[index] = win;
  win->id = index;

  win->pwin = 0;

  strncpy(win->header,header,256);
  win->header[255] = '\0';

  strncpy(win->label,label,256);
  win->label[255] = '\0';


  strcpy(win->FONT_NAME,"T35");
  strcpy(win->FONT_NAME_TMP,"T35");

  win->bg_clr = bg;
  win->border_clr = black;
  win->border_w = 1;
  win->cursor = -1;

  win->label_clr = LABEL_COLOR;

  win->width = width;
  win->height = height;

  win->canvas = 0;
  win->canvas_save = 0;
  win->buffer = 0;

  if (width > 0) width  += 2*BORDER_W;
  if (height > 0) height += HEADER_W+BORDER_W;

  win->x0 = 0;
  win->y0 = 0;
  win->x1 = width-1;
  win->y1 = height-1;

  win->xpos = BORDER_W;
  win->ypos = HEADER_W;

  win->state = 0;

  int icon_w, icon_h;
  x_get_pixrect_size(root_win->icon_pixmap,icon_w,icon_h);

  win->redraw = func;
  win->inf = inf;

  win->COLOR = black;
  win->LINEWIDTH = 1;
  win->LINESTYLE = solid;
  win->MODE = src_mode;
  win->TEXTMODE = transparent;

  win->save_ls = solid;
  win->save_lw = 1;
  win->save_clr = black;
  win->save_mode = src_mode;

  win->icon_pixmap = root_win->icon_pixmap;
  win->bg_pixmap = 0;

  win->mapped = 0;

  win->flush = true;

  win->special_event_handler = 0;
  win->special_event_data = 0;

  win->tt_top = -1;
  win->ttp_open = 0;
  win->ttp_current = 0;

  return win->id;
}


void x_open_window(int w, int x, int y, int width, int height, int pw, bool hidden)
{
  xx_win* win = wlist[w];
/*
  cout << string("X_OPEN_WINDOW: %s  x = %d  y = %d  width = %d  height = %d",
                                     win->header, x,y,width,height) << endl;
*/

  if (win->mapped) return;

  if (width < 0)  
  { width = -width;
    x -= (width-1);
    //if (pw == 0) x -= (2 * wm_frame_width);
   }

  if (height < 0)  
  { height = -height;
    y -= (height-1);
    //if (pw == 0) y -= (wm_frame_width + wm_title_width);
   }


  int max_width = display_width;
  int max_height = display_height;

  if (getenv("LEDA_OPEN_MAXIMIZED"))
  { // to avoid frame problems in fullscreen mode
    x-=1;
    y-=1;
    width += 2;
    height += 2;
   }


  if (pw == 0)
  { max_width -= 2*BORDER_W;
    max_height -= (HEADER_W + BORDER_W);
   }
  else
  { max_width -= 2;
    max_height -= 2;
   }

  if (pw > 0)
  { xx_win* p_win = wlist[pw];
    max_width = p_win->width - x;
    max_height = p_win->height - y;
   }

  if (width > max_width) width = max_width;
  if (height > max_height) height = max_height;


  if (width < 0)
  { width = -width;
    x -= (width-1);
    if (pw == 0) x -= 2*BORDER_W;
   }

  if (height < 0)
  { height = -height;
    y -= (height-1);
    if (pw == 0) y -= (BORDER_W+HEADER_W);
   }

  int bw = BORDER_W;
  int hw = (pw==0) ? HEADER_W : BORDER_W;


  if (pw)
  { bw = win->border_w;
    hw = win->border_w;
   }

  // push window on wstack

  win_top++;

  if (pw <= 0)
  { // app window (not child): push on top
    wstack[win_top] = win;
    win->mapped = win_top;
    int icon_w, icon_h;
    x_get_pixrect_size(win->icon_pixmap,icon_w,icon_h);
    win->icon_x = 2*BORDER_W;
    win->icon_y = 2*BORDER_W + (icon_h + 7*BORDER_W)*app_count;
    app_count++;
   }
  else
  { // child window: insert above pw and its children
    // find first position k above pw and its children
    xx_win* p_win = wlist[pw];
    int k = p_win->mapped + 1;
    while (k < win_top && wstack[k]->pwin == p_win) k++;

    for(int i = win_top; i > k ; i--) 
    { wstack[i] = wstack[i-1];
      wstack[i]->mapped = i;
     }

    wstack[k] = win;
    win->mapped = k;
   }


  width  += 2*bw;
  height += hw+bw;

  if (pw > 0)
  { xx_win* p_win = wlist[pw];
    x += p_win->xpos;
    y += p_win->ypos;
    win->pwin = p_win;
   }

  win->x0 = x;
  win->y0 = y;
  win->x1 = x+width-1;
  win->y1 = y+height-1;
  win->xpos = x+bw;
  win->ypos = y+hw;
  win->width = width - 2*bw; 
  win->height = height - hw - bw;

  if (win->canvas != 0) delete win->canvas;

  win->canvas = new x_image(win->width,win->height,win->bg_clr);

  //draw_window(win);

  redraw_root(6);

/*
  // consume all pending events
  string event;
  int e,val,px,py;
  unsigned long t;
  while ((e = NEXT_EVENT(event,val,px,py,t,100)) != no_event) {
    cout << "SKIP EVENT: " << event_name[e] << endl;
  }
*/
  
}


int x_window_opened(Window w) 
{  return wlist[w] && wlist[w]->mapped; }


void x_close_window(Window w)
{ xx_win* wp = wlist[w];

  if (grab_win == wp) grab_win = 0;

  if (wp->mapped == 0) return;

  //wp->canvas->write_bin("pixels.bgra");

  if (wp->ttp_open) x_close_window(wp->ttp_open->win);
  wp->ttp_open = 0;
  wp->ttp_current = 0;

  int x0 = wp->x0;
  int y0 = wp->y0;
  int x1 = wp->x1;
  int y1 = wp->y1;

  for(int i=wp->mapped; i < win_top; i++)
  { xx_win* wx = wstack[i+1];
    wstack[i] = wx;
    wx->mapped = i;
   }

  win_top--;
  wp->mapped = 0;

  redraw_root(x0,y0,x1,y1);
}


void x_destroy_window(Window w) 
{ xx_win* win = wlist[w];

  if (win->mapped) x_close_window(w);

  if (win->canvas) delete win->canvas;
  if (win->canvas_save) delete win->canvas_save;
  if (win->buffer) delete win->buffer;

  delete win; 

  wlist[w] = 0;
  if (w == win_count) 
  { while (wlist[w] == 0) w--;
    win_count = w;
  }
}

void x_set_icon_label(Window w, const char* s) 
{ xx_win* win = wlist[w];
  strncpy(win->label,s,265);
  win->label[255] = '\0';
  if (win->state & 0x01) draw_window(win);
}

void  x_set_icon_pixmap(Window w, char* pm)
{ xx_win* win = wlist[w];
  win->icon_pixmap = pm;
  int icon_w, icon_h;
  x_get_pixrect_size(pm,icon_w,icon_h);
  win->icon_x = 2*BORDER_W;
  win->icon_y = 2*BORDER_W + (icon_h + 7*BORDER_W)*(app_count-1);

/*
  if (!(win->state & 0x01))
  { win->save_x1 = win->save_x0 + icon_w-1;
    win->save_y1 = win->save_y0 + icon_h-1;
    win->save_width = icon_w;
    win->save_height = icon_h;
   }
  else
*/
  if (win->state & 0x01)
  { win->x1 = win->x0 + icon_w-1;
    win->y1 = win->y0 + icon_h-1;
    win->width = icon_w;
    win->height = icon_h;
    draw_window(win);
   }
}

void x_minimize_window(Window w)
{ xx_win* win = wlist[w];
  win->minimize();
  redraw_root(7);
}

void x_maximize_window(Window w)
{ xx_win* win = wlist[w];
  win->maximize(display_width, display_height);
  draw_window(win);
  redraw_root(7);
}


void x_grab_pointer(Window w) { grab_win = (w > 0) ? wlist[w] : 0; }


void x_resize_window(Window wi, int x, int y, int w, int h, int p) 
{
  // x,y: position of upper left FRAME corner
  // w,h: width and height of CLIENT area

/*
  cout << string("resize:  wi = %d x = %d y = %d w = %d h = %d p = %d",
                                                        wi,x,y,w,h,p) << endl; 
*/

  int buffering = x_test_buffer(wi);
  if (buffering) x_delete_buffer(wi);

  xx_win* win = wlist[wi];

  xx_win* pw = win->pwin;

  if (pw) {
   if (w > pw->width - x) w = pw->width - x; 
   if (h > pw->height - y) h = pw->height - y; 
   x += pw->xpos;
   y += pw->ypos;

  }

  win->x0 = x;
  win->y0 = y;
  win->x1 = x + w;
  win->y1 = y + h;

  win->xpos = x;
  win->ypos = y;
  win->width = w;
  win->height = h;
  
  if (pw == 0)
  { win->xpos += BORDER_W;
    win->ypos += HEADER_W;
    win->x1 += 2*BORDER_W;
    win->y1 += HEADER_W + BORDER_W;
  }

  if (win->canvas != 0) 
  { x_image* new_canv = new x_image(win->width,win->height,win->bg_clr);
    delete win->canvas;
    win->canvas = new_canv;
   }

  redraw_root(8);

  if (buffering) x_start_buffering(wi); 

  if (win->redraw) win->redraw(win->inf,0,0,w,h,1);
}


void x_set_topmost(Window w)
{ xx_win* win = wlist[w];

  // copy window group (window and its children)
  xx_win* grp[win_max];
  int k = 0;
  int i = win->mapped;
  do { grp[k++] = wstack[i++];
  } while (i <= win_top && wstack[i]->pwin == win);

  // remove group
  win_top -= k;
  for(int i=win->mapped; i <= win_top; i++) {
     wstack[i] = wstack[i+k];
     wstack[i]->mapped = i;
   }

  // copy back to top of stack
  for(int i=0; i<k; i++)
  { wstack[++win_top] = grp[i];
    grp[i]->mapped = win_top;
  }

  redraw_root(9);
}


void x_set_special_event_handler(Window w, 
                   void (*func)(void*,const char*,const char*,int,int), 
                   unsigned long data)
{ wlist[0]->special_event_handler = func;
  wlist[0]->special_event_data = w; 
}


int x_window_minimized(Window w) { return wlist[w]->state & 0x01 ; }
int x_window_maximized(Window w) { return wlist[w]->state & 0x02 ; }


void x_set_focus(Window w) { focus_window = w; }

//------------------------------------------------------------------------------
// not implemented
//------------------------------------------------------------------------------

int  x_get_resizing(int) { return 0; }

void x_flush_display() { }

void x_set_alpha(int, int) {}

void x_move_pointer(Window,int,int) {}

void x_set_stipple(Window,char*,int) {}

int  x_set_join_style(Window,int) { return 0; }

void x_fill_arc(Window,int,int,int,int,double,double) {}


//clipping
void x_clip_mask_chord(int, int, int, int, int, int, int, int, int) {}


// ms-windows

void x_polyline(int w, int n, int *xcoord, int *ycoord, int*) {}

int x_choose_file(int,int,const char*,const char*,const char*, char*,char*)  
{ return 0; }

void  x_text_to_clipboard(int, const char*) {}
char* x_text_from_clipboard(int) { return 0; }

void x_open_metafile(int, char const *) {}
void x_load_metafile(int, int, int, int, int, char const *) {}
void x_close_metafile(int) {}

void  x_metafile_to_clipboard(int) {}
void  x_pixrect_to_clipboard(int, char *) {}
char* x_pixrect_from_clipboard(int) { return 0; }

void x_start_timer(Window,int) {}
void x_stop_timer(Window) {}

int  x_choose_file(int,int,char const *,char const *,char *,char *) { return 0;}
int  x_choose_color(int, int) { return 0; }
void x_set_drop_handler(int, void (*func)(void*,const char*,int,int)) {}


LEDA_END_NAMESPACE
