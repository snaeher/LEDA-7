#ifndef _X_BASIC_H
#define _X_BASIC_H

#include <LEDA/system/basic.h>
#include <LEDA/system/assert.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#if defined(mips)
#include <bstring.h>
#endif

#if defined(_AIX)
#include <strings.h>
#include <sys/select.h>
#else
#include <sys/time.h>
#include <sys/times.h>
#if !defined(__aCC__)
#include <sys/select.h>
#endif
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xresource.h>
#include <X11/Xft/Xft.h>

#include <LEDA/graphics/x_window.h>


LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>



#define TOOLTIP_MAX 64 

struct x11_win;

struct x11_tooltip
{ 
  int win;
  int pw;

  int id;
  int xmin,ymin,xmax,ymax;

  int    num_lines;
  char** lines;

  x11_tooltip(int w,int ID, int x0, int y0, int x1, int y1, const char* s)
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

 ~x11_tooltip() 
 { if (lines) {
     while (num_lines > 0) delete[] lines[--num_lines];
     delete[] lines;
   }
  }

};



struct  x11_win
{
  Drawable     win;
  Drawable     win_save;
  Drawable     win_save2;
  Drawable     buf;
  Drawable     win_tt;

  // required for OpenGL

  Drawable     win_ret;
  void*        glc_ret;
  void*        glc;

  Pixmap       clip_mask;

  int          buf_w;
  int          buf_h;

  XGCValues    gc_val;
  GC           gc;
  GC           gc_tt;


  XftDraw*     xft_draw;

  XftFont*     xft_font;
  int          xft_size;

  XftFont*     xft_last_font;
  int          xft_last_size;


  int          LINEWIDTH;
  line_style   LINESTYLE;
  int          JOINSTYLE;
  drawing_mode MODE;
  text_mode    TEXTMODE;
  int          COLOR;
  int          BG_COLOR;
  char*        BG_PIXMAP;

  int          mapped;

  void* inf;

  int resizing;

  void (*redraw)(void*,int,int,int,int,int);
  void (*special_event_handler)(void*, const char*, const char*, int, int);
  unsigned long special_event_data;

  Window  icon_win;
  Window  frame_win;
  Cursor  cursor;
  int     cursor_id;
  int     BORDER_COLOR;
  int     BORDER_COLOR_SAVE;


  int tt_top;
  x11_tooltip* tt_stack[TOOLTIP_MAX];
  x11_tooltip* ttp_current;
  x11_tooltip* ttp_open;
  int ttp_x;
  int ttp_y;

  x11_win() { memset(this,0,sizeof(x11_win)); }
};

const int MAXWIN = 256;

LEDA_END_NAMESPACE

#endif
