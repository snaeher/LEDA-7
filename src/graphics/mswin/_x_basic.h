#ifndef _X_BASIC_H
#define _X_BASIC_H

/*
#include <LEDA/graphics/x_basic.h>
*/

#include <LEDA/graphics/x_window.h>

#include <LEDA/internal/system.h>
#include <LEDA/system/assert.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include <LEDA/graphics/pixmaps/win_icon.xpm>
#include <LEDA/graphics/pixmaps/win_small_icon.xpm>




#if defined(_MSC_VER)
#pragma warning(disable:4305)
#pragma warning(disable:4309)
#endif


#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <shellapi.h>
#include <dbt.h>

#include <winuser.h>

#include <shtypes.h>

/*
#undef NTDDI_VERSION
#define NTDDI_VERSION 0x06100000
#include <shellscalingapi.h>
*/


#define LEDA_TASKBAR_ICON


#define MAX_WIN    256
#define MAX_COLORS 2048

#define XWM_TASKBAR_ICON (WM_USER+1)
#define XWM_FONT_DIALOG  (WM_USER+2)
#define XWM_COLOR_DIALOG (WM_USER+3)
#define XWM_CLIPBOARD_DELETE (WM_USER+4)


LEDA_BEGIN_NAMESPACE

typedef int Window;

typedef void (*redraw_func)(void*,int,int,int,int,int);
typedef void (*special_event_func)(void*,const char*,const char*,int,int);

struct ms_win 
{

  HGLRC        hRC;
  HGLRC        hRCret;
  HDC          hDCret;
  HRGN         hrgnret;


  HWND         hwnd;
  HWND         hwnd_tt;
  HDC          hdc;
  HDC          hdcMem;
  HDC          hdc1;
  HDC          hdc2;
  HDEVNOTIFY   hDeviceNotify;
  HENHMETAFILE hmf;
  HBITMAP      hbm;
  LOGPEN       pen_data;
  HBITMAP      stip_bm;
  int          stip_bgcol;
  HFONT        font;
  HFONT        tmp_font;
  UINT         timer_id;

  int          width;
  int          height;
  int          border_w;

  int          buf_w;
  int          buf_h;

  int          COLOR;
  int          BG_COLOR;
  char*        BG_PIXMAP;
  HBRUSH       BG_BRUSH;

  int          alpha;

  int          LWIDTH;
  int          JSTYLE;
  line_style   LSTYLE;
  text_mode    TMODE;
  drawing_mode MODE;

  int          save_co;
  int          save_lw;
  line_style   save_ls;
  text_mode    save_tm;
  drawing_mode save_mo;

/*
  char  font_name[8];
  char  tmp_font_name[8];
*/
  char  font_name[64];
  char  tmp_font_name[64];

  char* header;
  void* inf;

  int   cursor;
  int   repaint_count;

  int   resizing;

  redraw_func          repaint;
  special_event_func   special_event_handler; // drop, device changed, ...
  unsigned long        special_event_data;

  ms_win(void* ptr, int w, int h, int bg_col, const char* label, 
             redraw_func redraw)
  {
    ZeroMemory(this,sizeof(ms_win));
  
    header = new char[strlen(label) + 1];
    strcpy(header,label);
  
    width   = w;
    height  = h;
    BG_COLOR = bg_col;
    inf     = ptr;

    repaint = redraw;
    repaint_count = 0;
    resizing = false;

    special_event_handler = NULL;
    special_event_data = 0;
  
    hwnd    = NULL;
    hwnd_tt = NULL;
    
    hRC     = NULL;
    hRCret  = NULL;
    hDCret  = NULL;
    hrgnret = NULL;
  
    hdc     = NULL;  // CreateDC("DISPLAY",NULL,NULL,NULL);
    hdcMem  = NULL;
    hdc1    = NULL;
    hdc2    = NULL;
    hmf     = NULL;

    alpha   = 255;
  
    pen_data.lopnStyle   = PS_SOLID;
    pen_data.lopnWidth.x = 1;
    pen_data.lopnWidth.y = 1;
    pen_data.lopnColor   = 0;
  
    stip_bm    = NULL;
    stip_bgcol = white;
  
    BG_PIXMAP = NULL;
    BG_BRUSH  = NULL;

    COLOR    = black;
    LSTYLE   = solid;
    JSTYLE   = 1;
    LWIDTH   = 1;
    MODE     = src_mode;
    TMODE    = transparent;
  
    font     = NULL;
    tmp_font = NULL;
    font_name[0] = '\0';
    tmp_font_name[0] = '\0';
  
    timer_id = 0;
    border_w = 1;
    cursor   = -1;
  
    save_co = COLOR;
    save_ls = LSTYLE;
    save_lw = LWIDTH;
    save_mo = MODE;
    save_tm = TMODE;
  }
  
 ~ms_win() { delete[] header; }
  
};


LEDA_END_NAMESPACE

#endif
