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


//-----------------------------------------------------------------------------
//
// Basic Graphics for Win32/64
//
// (c) Algorithmic Solutions 1996-2022
//
//-----------------------------------------------------------------------------
//
#include "_x_basic.h"

#if defined(_MSC_VER)
#pragma warning(disable:4313)
#endif

LEDA_BEGIN_NAMESPACE

#include <LEDA/graphics/x_basic.h>


static ms_win* wlist[MAX_WIN];
static int wcount = 0;

static int display_width = 0;
static int display_height = 0;
static int display_dpi = 0;


#define NO_TRANSPARENT_COL RGB(0xFF, 0xFF,0xFF)
#define ICON_TRANSPARENT_COL RGB(0, 0, 0)
#define STD_TRANSPARENT_COL RGB(221, 221, 221)


/* f18a0e88-c30c-11d0-8815-00a0c906bed8 */
GUID GUID_DEVINTERFACE_USB_HUB  =
{ 0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, 0xa0, 0xc9, 0x06, 0xbe, 0xd8 };

/* A5DCBF10-6530-11D2-901F-00C04FB951ED */
GUID GUID_DEVINTERFACE_USB_DEVICE =
{ 0xA5DCBF10, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED };

/* 3ABF6F2D-71C4-462a-8A92-1E6861E6AF27 */
GUID GUID_DEVINTERFACE_USB_HOST_CONTROLLER =
{ 0x3abf6f2d, 0x71c4, 0x462a, 0x8a, 0x92, 0x1e, 0x68, 0x61, 0xe6, 0xaf, 0x27 };
             
/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
GUID GUID_USB_WMI_STD_DATA =
{ 0x4E623B20, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00, 0xA0, 0xC9, 0x59, 0xBB, 0xD2 };

/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
GUID GUID_USB_WMI_STD_NOTIFICATION =
{ 0x4E623B20, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00, 0xA0, 0xC9, 0x59, 0xBB, 0xD2 };


BOOL TranspBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, 
                  HDC hdcSrc,  int nXSrc,  int nYSrc,  COLORREF crTransparent)
{

/*
  We would like to use:
  return TransparentBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, 
                        hdcSrc, nXSrc, nYSrc, nWidth, nHeight, crTransparent);
  but according to the MS documentation this contains a bug on some Windows platforms.
  So we use a work-around (see Microsoft Knowledge Base Article - 79212).
  (But we optimized it a little bit to save one bitmap and one BitBlt operation. :-))
*/
/*
  NOTE: The bitmap hbmTransp could be cached in the x_pixrect struct to speed things
  up a little bit.
*/

   // Create some DCs to hold temporary data.
   HDC hdcTransp  = CreateCompatibleDC(hdcDest);
   HDC hdcBuf     = CreateCompatibleDC(hdcDest);
   HDC hdcCopy    = CreateCompatibleDC(hdcDest);

   // Create a bitmap for each DC. DCs are required for a number of
   // GDI functions.

   // Monochrome Bitmap
   HBITMAP hbmTransp  = CreateBitmap(nWidth, nHeight, 1, 1, NULL);

   // Color Bitmaps
   HBITMAP hbmBuf     = CreateCompatibleBitmap(hdcDest, nWidth, nHeight);
   HBITMAP hbmCopy    = CreateCompatibleBitmap(hdcDest, nWidth, nHeight);

   // Each DC must select a bitmap object to store pixel data.
   SelectObject(hdcTransp, hbmTransp);
   SelectObject(hdcBuf, hbmBuf);
   SelectObject(hdcCopy, hbmCopy);

   // Create hdcTransp: Transparent pixels will be 1, all others will be zero.
   COLORREF cBkColSav = SetBkColor(hdcSrc, crTransparent);
   BitBlt(hdcTransp, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, SRCCOPY);
   SetBkColor(hdcSrc, cBkColSav);

   // We want to compute: (Dest AND Transp) OR (SRC AND (NOT Transp))
   //                   = (Dest AND Transp) OR (NOT ((NOT SRC) OR Transp))

   // hdcBuf = hdcDest
   BitBlt(hdcBuf, 0, 0, nWidth, nHeight, hdcDest, nXDest, nYDest, SRCCOPY);

   // hdcBuf = hdcDest AND hdcTransp
   BitBlt(hdcBuf, 0, 0, nWidth, nHeight, hdcTransp, 0, 0, SRCAND);

   // hdcCopy = NOT hdcSrc
   BitBlt(hdcCopy, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, NOTSRCCOPY);

   // hdcCopy = (NOT hdcSrc) OR hdcTransp
   BitBlt(hdcCopy, 0, 0, nWidth, nHeight, hdcTransp, 0, 0, SRCPAINT);

   // hdcBuf = (hdcDest AND hdcTransp) OR (NOT ((NOT hdcSrc) OR hdcTransp))
   BitBlt(hdcBuf, 0, 0, nWidth, nHeight, hdcCopy, 0, 0, MERGEPAINT);

   // Copy hdcBuf to hdcDest.
   BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcBuf, 0, 0, SRCCOPY);

   // Delete the memory bitmaps.
   DeleteObject(hbmTransp);
   DeleteObject(hbmBuf);
   DeleteObject(hbmCopy);

   // Delete the memory DCs.
   DeleteDC(hdcTransp);
   DeleteDC(hdcBuf);
   DeleteDC(hdcCopy);

   return TRUE;
}


struct x_pixrect {
 int w;
 int h;
 HBITMAP map;
 int alpha;
 float phi;
 int anchor_x;
 int anchor_y;
 COLORREF transparent;

 x_pixrect() : alpha(-1), // default: no alpha channel
               phi(0),
               anchor_x(0),
               anchor_y(0),
               transparent(NO_TRANSPARENT_COL) {}


 void blend(HDC hdcDest, int dest_x, int dest_y, int dest_w, int dest_h, 
            int src_x, int src_y, int src_w, int src_h)
 { 
/*
   BitBlt(hdcDest, dest_x, dest_y, src_w, src_h, 
             hdcSrc, src_x, src_y, SRCCOPY);
   return;
*/
  HDC hdcSrc  = CreateCompatibleDC(hdcDest) ;
  SelectObject (hdcSrc,map);



  SetGraphicsMode(hdcDest,GM_ADVANCED);

  double scale_x = double(dest_w)/src_w;
  double scale_y = double(dest_h)/src_h;

  dest_x -= int(0.5+scale_x*anchor_x);
  dest_y += int(0.5+scale_y*anchor_y);

  XFORM xfp;

  if (phi !=0)
  { xfp.eM11 = (FLOAT)cos(phi);
    xfp.eM12 = (FLOAT)sin(phi);
    xfp.eM21 = (FLOAT)-sin(phi);
    xfp.eM22 = (FLOAT)cos(phi);

    int x0 = dest_x + dest_w/2;
    int y0 = dest_y + dest_h/2;

    xfp.eDx = (FLOAT)(x0 - cos(phi)*x0 + sin(phi)*y0);
    xfp.eDy = (FLOAT)(y0 - cos(phi)*y0 - sin(phi)*x0);

    SetWorldTransform(hdcDest,&xfp);
  }


  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = 255; // global alpha = 255
  bf.AlphaFormat = 0; // no alpha channel

  if (alpha != -1)
  { // use both global alpha and alpha pixel channel
    bf.SourceConstantAlpha = (BYTE)alpha;
    bf.AlphaFormat = AC_SRC_ALPHA;
   }

  AlphaBlend(hdcDest, dest_x, dest_y, dest_w, dest_h, 
              hdcSrc, src_x, src_y, src_w, src_h, bf);


  if (phi != 0)
  { xfp.eM11 = 1;
    xfp.eM12 = 0;
    xfp.eM21 = 0;
    xfp.eM22 = 1;
    xfp.eDx = 0;
    xfp.eDy = 0;
    SetWorldTransform(hdcDest,&xfp);
   }

  DeleteDC (hdcSrc) ;
 }


/*
 void draw(HDC hdcDest, int dest_x, int dest_y, int dest_w, int dest_h, 
           int src_x, int src_y, int src_w, int src_h)
 { 

   if (alpha < 255)
     blend(hdcDest,dest_x,dest_y,dest_w,dest_h,src_x,src_y,src_w,src_h);
   else
    if (transparent == NO_TRANSPARENT_COL)
      BitBlt(hdcDest, dest_x, dest_y, src_w, src_h, 
             hdcSrc, src_x, src_y, SRCCOPY);
    else
      TransparentBlt(hdcDest, dest_x, dest_y, dest_w, dest_h, 
                        hdcSrc, src_x, src_y, src_w, src_h, transparent);
  }
*/

};


struct event {
  int win;
  int kind;
  int x;
  int y;
  int val1;
  int val2;
  unsigned long t;
};

static event cur_event;
static event last_event;
static int putback;
static int trace_events;

static event button_event;

static COLORREF rgb_custom[16];


static HFONT text_font;
static HFONT italic_font;
static HFONT bold_font;
static HFONT fixed_font;
static HFONT button_font;


static HCURSOR  dot_box_cursor;
static HCURSOR  hand_cursor;
static HICON    leda_icon;
static HICON    leda_small_icon;
static HICON    res_icon;
static HICON    res_small_icon;

static HWND     taskbar_icon_win;
static HMENU    taskbar_icon_menu;

static char szAppName1[] = "LEDA Window";
static char szAppName2[] = "LEDA Subwindow";

static Window grab_win;

static int alt_key_down = 0;

static unsigned char dot_box_bits[] = 
  { 0xFF,0xF8,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x82,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0xFF,0xF8,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00};


static unsigned char hand_bits[] = 
  { 0x7F,0xF0,0x00,0x00,
    0x80,0x08,0x00,0x00,
    0x7F,0x84,0x00,0x00,
    0x08,0x02,0x00,0x00,
    0x07,0x82,0x00,0x00,
    0x08,0x02,0x00,0x00,
    0x07,0x85,0x00,0x00,
    0x08,0x08,0x80,0x00,
    0x07,0x90,0x40,0x00,
    0x00,0xE0,0x80,0x00,
    0x00,0x49,0x00,0x00,
    0x00,0x22,0x00,0x00,
    0x00,0x14,0x00,0x00,
    0x00,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00};


static char* message_table[1024];

static void  add_message(const char* msg, UINT id) { 
  message_table[id] = (char*)msg; 
}

static void init_message_table()
{
  for(int i=0; i<1024; i++) message_table[i] = 0;

  add_message("WM_NULL", 0x00);
  add_message("WM_CREATE", 0x01);
  add_message("WM_DESTROY", 0x02);
  add_message("WM_MOVE", 0x03);
  add_message("WM_SIZE", 0x05);
  add_message("WM_ACTIVATE", 0x06);
  add_message("WM_SETFOCUS", 0x07);
  add_message("WM_KILLFOCUS", 0x08);
  add_message("WM_ENABLE", 0x0A);
  add_message("WM_SETREDRAW", 0x0B);
  add_message("WM_SETTEXT", 0x0C);
  add_message("WM_GETTEXT", 0x0D);
  add_message("WM_GETTEXTLENGTH", 0x0E);
  add_message("WM_PAINT", 0x0F);
  add_message("WM_CLOSE", 0x10);
  add_message("WM_QUERYENDSESSION", 0x11);
  add_message("WM_QUIT", 0x12);
  add_message("WM_QUERYOPEN", 0x13);
  add_message("WM_ERASEBKGND", 0x14);
  add_message("WM_SYSCOLORCHANGE", 0x15);
  add_message("WM_ENDSESSION", 0x16);
  add_message("WM_SYSTEMERROR", 0x17);
  add_message("WM_SHOWWINDOW", 0x18);
  add_message("WM_CTLCOLOR", 0x19);
  add_message("WM_WININICHANGE", 0x1A);
  add_message("WM_SETTINGCHANGE", 0x1A);
  add_message("WM_DEVMODECHANGE", 0x1B);
  add_message("WM_ACTIVATEAPP", 0x1C);
  add_message("WM_FONTCHANGE", 0x1D);
  add_message("WM_TIMECHANGE", 0x1E);
  add_message("WM_CANCELMODE", 0x1F);
  add_message("WM_SETCURSOR", 0x20);
  add_message("WM_MOUSEACTIVATE", 0x21);
  add_message("WM_CHILDACTIVATE", 0x22);
  add_message("WM_QUEUESYNC", 0x23);
  add_message("WM_GETMINMAXINFO", 0x24);
  add_message("WM_PAINTICON", 0x26);
  add_message("WM_ICONERASEBKGND", 0x27);
  add_message("WM_NEXTDLGCTL", 0x28);
  add_message("WM_SPOOLERSTATUS", 0x2A);
  add_message("WM_DRAWITEM", 0x2B);
  add_message("WM_MEASUREITEM", 0x2C);
  add_message("WM_DELETEITEM", 0x2D);
  add_message("WM_VKEYTOITEM", 0x2E);
  add_message("WM_CHARTOITEM", 0x2F);

  add_message("WM_SETFONT", 0x30);
  add_message("WM_GETFONT", 0x31);
  add_message("WM_SETHOTKEY", 0x32);
  add_message("WM_GETHOTKEY", 0x33);
  add_message("WM_QUERYDRAGICON", 0x37);
  add_message("WM_COMPAREITEM", 0x39);
  add_message("WM_COMPACTING", 0x41);
  add_message("WM_WINDOWPOSCHANGING", 0x46);
  add_message("WM_WINDOWPOSCHANGED", 0x47);
  add_message("WM_POWER", 0x48);
  add_message("WM_COPYDATA", 0x4A);
  add_message("WM_CANCELJOURNAL", 0x4B);
  add_message("WM_NOTIFY", 0x4E);
  add_message("WM_INPUTLANGCHANGEREQUEST", 0x50);
  add_message("WM_INPUTLANGCHANGE", 0x51);
  add_message("WM_TCARD", 0x52);
  add_message("WM_HELP", 0x53);
  add_message("WM_USERCHANGED", 0x54);
  add_message("WM_NOTIFYFORMAT", 0x55);
  add_message("WM_CONTEXTMENU", 0x7B);
  add_message("WM_STYLECHANGING", 0x7C);
  add_message("WM_STYLECHANGED", 0x7D);
  add_message("WM_DISPLAYCHANGE", 0x7E);
  add_message("WM_GETICON", 0x7F);
  add_message("WM_SETICON", 0x80);

  add_message("WM_NCCREATE", 0x81);
  add_message("WM_NCDESTROY", 0x82);
  add_message("WM_NCCALCSIZE", 0x83);
  add_message("WM_NCHITTEST", 0x84);
  add_message("WM_NCPAINT", 0x85);
  add_message("WM_NCACTIVATE", 0x86);
  add_message("WM_GETDLGCODE", 0x87);
  add_message("WM_NCMOUSEMOVE", 0xA0);
  add_message("WM_NCLBUTTONDOWN", 0xA1);
  add_message("WM_NCLBUTTONUP", 0xA2);
  add_message("WM_NCLBUTTONDBLCLK", 0xA3);
  add_message("WM_NCRBUTTONDOWN", 0xA4);
  add_message("WM_NCRBUTTONUP", 0xA5);
  add_message("WM_NCRBUTTONDBLCLK", 0xA6);
  add_message("WM_NCMBUTTONDOWN", 0xA7);
  add_message("WM_NCMBUTTONUP", 0xA8);
  add_message("WM_NCMBUTTONDBLCLK", 0xA9);

  add_message("WM_KEYFIRST", 0x100);
  add_message("WM_KEYDOWN", 0x100);
  add_message("WM_KEYUP", 0x101);
  add_message("WM_CHAR", 0x102);
  add_message("WM_DEADCHAR", 0x103);
  add_message("WM_SYSKEYDOWN", 0x104);
  add_message("WM_SYSKEYUP", 0x105);
  add_message("WM_SYSCHAR", 0x106);
  add_message("WM_SYSDEADCHAR", 0x107);
  add_message("WM_KEYLAST", 0x108);

  add_message("WM_IME_STARTCOMPOSITION", 0x10D);
  add_message("WM_IME_ENDCOMPOSITION", 0x10E);
  add_message("WM_IME_COMPOSITION", 0x10F);
  add_message("WM_IME_KEYLAST", 0x10F);

  add_message("WM_INITDIALOG", 0x110);
  add_message("WM_COMMAND", 0x111);
  add_message("WM_SYSCOMMAND", 0x112);
  add_message("WM_TIMER", 0x113);
  add_message("WM_HSCROLL", 0x114);
  add_message("WM_VSCROLL", 0x115);
  add_message("WM_INITMENU", 0x116);
  add_message("WM_INITMENUPOPUP", 0x117);
  add_message("WM_MENUSELECT", 0x11F);
  add_message("WM_MENUCHAR", 0x120);
  add_message("WM_ENTERIDLE", 0x121);

  add_message("WM_CTLCOLORMSGBOX", 0x132);
  add_message("WM_CTLCOLOREDIT", 0x133);
  add_message("WM_CTLCOLORLISTBOX", 0x134);
  add_message("WM_CTLCOLORBTN", 0x135);
  add_message("WM_CTLCOLORDLG", 0x136);
  add_message("WM_CTLCOLORSCROLLBAR", 0x137);
  add_message("WM_CTLCOLORSTATIC", 0x138);

  add_message("WM_MOUSEFIRST", 0x200);
  add_message("WM_MOUSEMOVE", 0x200);
  add_message("WM_LBUTTONDOWN", 0x201);
  add_message("WM_LBUTTONUP", 0x202);
  add_message("WM_LBUTTONDBLCLK", 0x203);
  add_message("WM_RBUTTONDOWN", 0x204);
  add_message("WM_RBUTTONUP", 0x205);
  add_message("WM_RBUTTONDBLCLK", 0x206);
  add_message("WM_MBUTTONDOWN", 0x207);
  add_message("WM_MBUTTONUP", 0x208);
  add_message("WM_MBUTTONDBLCLK", 0x209);
  add_message("WM_MOUSEWHEEL", 0x20A);
  add_message("WM_MOUSEHWHEEL", 0x20E);

  add_message("WM_PARENTNOTIFY", 0x210);
  add_message("WM_ENTERMENULOOP", 0x211);
  add_message("WM_EXITMENULOOP", 0x212);
  add_message("WM_NEXTMENU", 0x213);
  add_message("WM_SIZING", 0x214);
  add_message("WM_CAPTURECHANGED", 0x215);
  add_message("WM_MOVING", 0x216);
  add_message("WM_POWERBROADCAST", 0x218);
  add_message("WM_DEVICECHANGE", 0x219);

  add_message("WM_MDICREATE", 0x220);
  add_message("WM_MDIDESTROY", 0x221);
  add_message("WM_MDIACTIVATE", 0x222);
  add_message("WM_MDIRESTORE", 0x223);
  add_message("WM_MDINEXT", 0x224);
  add_message("WM_MDIMAXIMIZE", 0x225);
  add_message("WM_MDITILE", 0x226);
  add_message("WM_MDICASCADE", 0x227);
  add_message("WM_MDIICONARRANGE", 0x228);
  add_message("WM_MDIGETACTIVE", 0x229);
  add_message("WM_MDISETMENU", 0x230);
  add_message("WM_ENTERSIZEMOVE", 0x231);
  add_message("WM_EXITSIZEMOVE", 0x232);
  add_message("WM_DROPFILES", 0x233);
  add_message("WM_MDIREFRESHMENU", 0x234);

  add_message("WM_IME_SETCONTEXT", 0x281);
  add_message("WM_IME_NOTIFY", 0x282);
  add_message("WM_IME_CONTROL", 0x283);
  add_message("WM_IME_COMPOSITIONFULL", 0x284);
  add_message("WM_IME_SELECT", 0x285);
  add_message("WM_IME_CHAR", 0x286);
  add_message("WM_IME_KEYDOWN", 0x290);
  add_message("WM_IME_KEYUP", 0x291);

  add_message("WM_MOUSEHOVER", 0x2A1);
  add_message("WM_NCMOUSELEAVE", 0x2A2);
  add_message("WM_MOUSELEAVE", 0x2A3);

  add_message("WM_CUT", 0x300);
  add_message("WM_COPY", 0x301);
  add_message("WM_PASTE", 0x302);
  add_message("WM_CLEAR", 0x303);
  add_message("WM_UNDO", 0x304);

  add_message("WM_RENDERFORMAT", 0x305);
  add_message("WM_RENDERALLFORMATS", 0x306);
  add_message("WM_DESTROYCLIPBOARD", 0x307);
  add_message("WM_DRAWCLIPBOARD", 0x308);
  add_message("WM_PAINTCLIPBOARD", 0x309);
  add_message("WM_VSCROLLCLIPBOARD", 0x30A);
  add_message("WM_SIZECLIPBOARD", 0x30B);
  add_message("WM_ASKCBFORMATNAME", 0x30C);
  add_message("WM_CHANGECBCHAIN", 0x30D);
  add_message("WM_HSCROLLCLIPBOARD", 0x30E);
  add_message("WM_QUERYNEWPALETTE", 0x30F);
  add_message("WM_PALETTEISCHANGING", 0x310);
  add_message("WM_PALETTECHANGED", 0x311);

  add_message("WM_HOTKEY", 0x312);
  add_message("WM_PRINT", 0x317);
  add_message("WM_PRINTCLIENT", 0x318);

  add_message("WM_HANDHELDFIRST", 0x358);
  add_message("WM_HANDHELDLAST", 0x35F);
  add_message("WM_PENWINFIRST", 0x380);
  add_message("WM_PENWINLAST", 0x38F);
  add_message("WM_COALESCE_FIRST", 0x390);
  add_message("WM_COALESCE_LAST", 0x39F);
  add_message("WM_DDE_FIRST", 0x3E0);
  add_message("WM_DDE_INITIATE", 0x3E0);
  add_message("WM_DDE_TERMINATE", 0x3E1);
  add_message("WM_DDE_ADVISE", 0x3E2);
  add_message("WM_DDE_UNADVISE", 0x3E3);
  add_message("WM_DDE_ACK", 0x3E4);
  add_message("WM_DDE_DATA", 0x3E5);
  add_message("WM_DDE_REQUEST", 0x3E6);
  add_message("WM_DDE_POKE", 0x3E7);
  add_message("WM_DDE_EXECUTE", 0x3E8);
  add_message("WM_DDE_LAST", 0x3E8);

/*
  add_message("WM_USER", 0x400);
  add_message("WM_APP", 0x8000);
*/
}



static int get_last_error(char* buf, int sz)
{
  int err = GetLastError();

  LPVOID lpMsgBuf;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL 
                );

  strncpy(buf,(char*)lpMsgBuf,sz);

  LocalFree(lpMsgBuf);

  return err;
}
 


static void show_last_error(const char* txt)
{
  //if (GetLastError() == 0) return;

  char buffer[1024];
  strcpy(buffer,txt);
/*
  LPVOID lpMsgBuf = buffer + strlen(txt);
*/
  LPVOID lpMsgBuf;

  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                 FORMAT_MESSAGE_FROM_SYSTEM | 
                 FORMAT_MESSAGE_IGNORE_INSERTS,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                 (LPTSTR) &lpMsgBuf,
                 0,
                 NULL 
                );
//MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
   
  strcpy(buffer+strlen(txt),(char*)lpMsgBuf);

   MessageBox( NULL, buffer, "Error", MB_OK | MB_ICONINFORMATION );
   LocalFree( lpMsgBuf );
}
 

void x_add_taskbar_icon(HWND hwnd, HICON hicon, char* tip)
{
#if defined(LEDA_TASKBAR_ICON)
  NOTIFYICONDATA icon_data;
  icon_data.cbSize = sizeof(NOTIFYICONDATA);
  icon_data.hWnd = hwnd;
  icon_data.uID = 9999;
  icon_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  icon_data.uCallbackMessage = XWM_TASKBAR_ICON;
  icon_data.hIcon = hicon;
  strcpy(icon_data.szTip,tip);

  // seems to be VERY SLOW
  Shell_NotifyIcon(NIM_ADD,&icon_data);

  HMENU hm = CreatePopupMenu();
  char win_label[128];
  GetWindowText(hwnd,win_label,128);
  AppendMenu(hm,MF_STRING,0,"LEDA Setup");
  AppendMenu(hm,MF_SEPARATOR,0,NULL);
  AppendMenu(hm,MF_STRING,XWM_FONT_DIALOG,"Fonts");
  AppendMenu(hm,MF_STRING,XWM_COLOR_DIALOG,"Colors");
  AppendMenu(hm,MF_STRING,XWM_CLIPBOARD_DELETE,"Empty Clipboard");
  taskbar_icon_menu = hm;
#endif
}
  
 
void x_change_taskbar_icon(HWND hwnd,HICON hicon,char* tip)
{
#if defined(LEDA_TASKBAR_ICON)
  NOTIFYICONDATA icon_data;
  icon_data.cbSize = sizeof(NOTIFYICONDATA);
  icon_data.hWnd = hwnd;
  icon_data.uID = 9999;
  icon_data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
  icon_data.uCallbackMessage = XWM_TASKBAR_ICON;
  icon_data.hIcon = hicon;
  strcpy(icon_data.szTip,tip);
  Shell_NotifyIcon(NIM_MODIFY,&icon_data);
#endif
}
  
   
 
void x_del_taskbar_icon(HWND hwnd)
{
#if defined(LEDA_TASKBAR_ICON)
  NOTIFYICONDATA icon_data;
  icon_data.cbSize = sizeof(NOTIFYICONDATA);
  icon_data.hWnd = hwnd;
  icon_data.uID = 9999;
  Shell_NotifyIcon(NIM_DELETE,&icon_data);
#endif
}




inline void SWAP(int& x1, int& x2)
{ int t = x1; x1 = x2; x2 = t; }



static LRESULT WINAPI WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


/*
static bool getDpiXY(UINT& dpiX, UINT& dpiY)  
{ POINT pt;
  pt.x = 0;
  pt.y = 0;
  HMONITOR hm = MonitorFromPoint(pt,MONITOR_DEFAULTTOPRIMARY);

  DEVICE_SCALE_FACTOR pScale;
  GetScaleFactorForMonitor(hm,&pScale);
  printf("scaling = %d   100: %d  150: %d\n",pScale,SCALE_100_PERCENT,
                                                    SCALE_150_PERCENT);

  return GetDpiForMonitor(hm,MDT_EFFECTIVE_DPI,&dpiX,&dpiY) == S_OK;
}
*/


static void show_display_info(void)
{ HDC hdc = GetDC(0);
  int disp_w = GetSystemMetrics(SM_CXSCREEN); 
  int disp_h = GetSystemMetrics(SM_CYSCREEN); 
  int bits = GetDeviceCaps(hdc,BITSPIXEL);
  int clrs = GetDeviceCaps(hdc,NUMCOLORS);
  int xdpi = GetDeviceCaps(hdc,LOGPIXELSX);
  //int ydpi = GetDeviceCaps(hdc,LOGPIXELSY);

{
HWND hwnd = FindWindow("Shell_traywnd",NULL);
RECT r;
GetWindowRect(hwnd,&r);
int taskbar_h = r.bottom - r.top;
cout << "taskbar height = " << taskbar_h << endl;
cout << "r.top = " << r.top << endl;
cout << "r.bottom = " << r.bottom << endl;

}


  float scaling = float(xdpi)/96;

  ReleaseDC(0,hdc);

  cout << string("display: %d x %d  bits = %d  colors = %d  dpi: %d scaling: %.2f", 
                                   disp_w,disp_h,bits,clrs,xdpi,scaling);
  cout << endl;
}


static int rotate_rgb(int col)
{ if (col == invisible) return col;
  int r = (col >> 16) & 0xff;
  int g = (col >>  8) & 0xff;
  int b = (col >>  0) & 0xff;
  return (b << 16) | (g << 8) | r;
 }


void x_do_not_open_display(bool) {}



#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER < 1900)


#define DPI_AWARENESS_CONTEXT int

#endif

#if !defined(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 DPI_AWARENESS_CONTEXT(-4)
#endif


bool user32_dll_func(const char* name, DPI_AWARENESS_CONTEXT param)
{ typedef BOOL (WINAPI *PGNSI)(DPI_AWARENESS_CONTEXT);

  PGNSI pGNSI = (PGNSI) GetProcAddress(GetModuleHandle("user32.dll"),name);
  if (pGNSI == NULL)
  { cerr << name << " not found." << endl;
    return false;
   }

  if (!pGNSI(param))
  { cerr << name << " failed." << endl;
    return false;
   }

  return true;
 }



static void get_face_name(HFONT font, char* name, int sz)
{ HWND hwnd = CreateWindowEx(0,0,0,0,0,0,0,0,NULL,NULL,NULL,NULL);
  HDC hdc = GetDC(hwnd);
  HFONT save = (HFONT)SelectObject(hdc,font);
  GetTextFace(hdc,sz,name);
  SelectObject(hdc,save);
  ReleaseDC(hwnd,hdc);
  DestroyWindow(hwnd);
}

static HFONT create_font(const char* face_name, int sz, int weight, int italic)
{ LOGFONT lf;
  ZeroMemory(&lf,sizeof(LOGFONT));
  strcpy(lf.lfFaceName,face_name);
  lf.lfHeight = sz;
  lf.lfWeight = weight;
  lf.lfItalic = italic;
  HFONT font = CreateFontIndirect(&lf);

  char name[256];
  get_face_name(font,name,256);
  if (strcmp(name,face_name) != 0)
    cout << string("FONT: %s ---> %s",face_name,name) << endl;

  return font;
}


int x_open_display(void)  
{ 
  if (display_width > 0) {
   // display has been opened before
   return 0;
  }

//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  user32_dll_func("SetProcessDpiAwarenessContext",
                   DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);


  display_width = GetSystemMetrics(SM_CXSCREEN); 
  display_height = GetSystemMetrics(SM_CYSCREEN); 

  HWND hwnd = FindWindow("Shell_traywnd",NULL);
  RECT r;
  GetWindowRect(hwnd,&r);

  if (r.bottom == display_height) display_height = r.top;

  display_dpi = GetDeviceCaps(GetDC(0),LOGPIXELSX);

  char* p = getenv("LEDA_DPI");
  if (p != 0) display_dpi = atoi(p);


/*
  HDC hdc = CreateIC("DISPLAY",NULL,NULL,NULL); 
  int bits_per_pixel = GetDeviceCaps(hdc,BITSPIXEL);
  printf("bits per pixel: %d\n",bits_per_pixel);
  int mask_blt= GetDeviceCaps(hdc,RC_BITBLT);
  printf("mask_blt: %d\n",mask_blt);
  fflush(stdout);
  DeleteDC(hdc);
*/

  init_message_table();

  // create fonts

  int sz = int(0.19*display_dpi);
  int bold_sz = int(0.95*sz);
  int fixed_sz = int(0.80*sz);

  //cout << "Font: dpi  = " << display_dpi << " sz = " << sz << endl;

  text_font = create_font("Arial",sz,FW_NORMAL,0);
//text_font = create_font("Trebuchet MS",sz,FW_NORMAL,0);
//text_font = create_font("Droid Sans",sz,FW_NORMAL,0);


  bold_font = create_font("Arial",bold_sz,FW_BOLD,0);
//bold_font = create_font("Trebuchet MS",bold_sz,FW_BOLD,0);
//bold_font = create_font("Droid Sans",bold_sz,FW_BOLD,0);

  italic_font = create_font("Arial",sz,FW_NORMAL,1);
//italic_font = create_font("Trebuchet MS",sz,FW_NORMAL,1);
//italic_font = create_font("Droid Sans",sz,FW_NORMAL,1);


  fixed_font = create_font("Lucida Console",fixed_sz,FW_NORMAL,0);
//fixed_font = create_font("Consolas",sz,FW_NORMAL,0);
//fixed_font = create_font("DejaVu Sans Mono",sz,FW_NORMAL,0);

  button_font = create_font("Arial",sz,FW_NORMAL,0);
//button_font= (HFONT)GetStockObject(DEFAULT_GUI_FONT);
//button_font= (HFONT)GetStockObject(SYSTEM_FONT);

  int i;
  for(i=0; i<16; i++) rgb_custom[i] = RGB(255,255,255);

  // wlist[0]: root window

  ms_win* wp = new ms_win(0,0,0,0,"root window",0);
  wp->font = fixed_font;
  wp->hwnd = GetDesktopWindow();
  wlist[0] = wp;
  wcount = 0;

  for(int j = 1; j < MAX_WIN; j++) wlist[j] = 0;


  // cursor
  // int cw = GetSystemMetrics(SM_CXCURSOR);
  // int ch = GetSystemMetrics(SM_CYCURSOR);

  for(i=0;i<128;i++) dot_box_bits[i] = ~dot_box_bits[i];
  for(i=0;i<128;i++) hand_bits[i] = ~hand_bits[i];

  char* xor_bits = new char[512];
  for(i=0;i<512;i++) xor_bits[i] = 0;

  dot_box_cursor = CreateCursor(NULL,6,6,32,32,dot_box_bits,xor_bits);
  hand_cursor = CreateCursor(NULL,0,1,32,32,hand_bits,xor_bits);

  delete[] xor_bits;

  res_icon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(101));
  res_small_icon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(102));

  if (res_icon) leda_icon = res_icon;

  if (res_small_icon) leda_small_icon = res_icon;


  // register window classes

  WNDCLASSEX    wndclass;

  wndclass.cbSize = sizeof(WNDCLASSEX);


  wndclass.lpszClassName = szAppName1;
  wndclass.style         = CS_OWNDC |CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = (WNDPROC)WndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 4;
  wndclass.hInstance     = NULL;

/*
  if (res_icon != NULL)
  { wndclass.hIcon         = res_icon;
    wndclass.hIconSm       = res_icon;
   }
  else
*/
  { wndclass.hIcon         = leda_icon;
    wndclass.hIconSm       = leda_small_icon;
   }

  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
//wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
  wndclass.hbrBackground = NULL;
  wndclass.lpszMenuName  = NULL;

  RegisterClassEx(&wndclass);


  wndclass.lpszClassName = szAppName2;
  wndclass.style         = CS_SAVEBITS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc   = (WNDPROC)WndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 4;
  wndclass.hInstance     = NULL;
//wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
  wndclass.hIcon         = NULL;
  wndclass.hIconSm       = NULL;
  wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
//wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.hbrBackground = NULL;
  wndclass.lpszMenuName  = NULL;

  RegisterClassEx(&wndclass);

  //show_display_info();

  return 0;
}


char* x_root_pixrect(int x1 ,int y1, int x2, int y2)
{ HDC hdc = CreateDC("DISPLAY",NULL,NULL,NULL); 
  SetGraphicsMode(hdc,GM_ADVANCED);
  HDC hdcMem = CreateCompatibleDC (hdc) ;
  x_pixrect* im = new x_pixrect;
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  im->w = x2-x1+1;
  im->h = y2-y1+1;
  im->map = CreateCompatibleBitmap (hdc,im->w,im->h) ;
  SelectObject (hdcMem,im->map);
  BitBlt (hdcMem,0,0,im->w,im->h,hdc,x1,y1,SRCCOPY) ;
  DeleteDC(hdcMem) ;
  DeleteDC(hdc) ;
  return (char*)im;
}


void x_close_display(void) 
{ 
  if (taskbar_icon_win) 
  { x_del_taskbar_icon(taskbar_icon_win);
    taskbar_icon_win = 0;
   }

  DeleteObject(text_font);  
  DeleteObject(italic_font);  
  DeleteObject(bold_font);  
  DeleteObject(fixed_font); 
  DeleteObject(button_font); 

  DestroyCursor(dot_box_cursor);
  DestroyCursor(hand_cursor);

  if (leda_icon != res_icon) DestroyIcon(leda_icon);
  if (leda_small_icon != res_small_icon) DestroyIcon(leda_icon);

  text_font   = NULL;
  italic_font = NULL;
  bold_font   = NULL;
  fixed_font  = NULL;
  button_font = NULL;

  if (wlist[0]) delete wlist[0];

/*
  for(int i=0; i <= wcount; i++) 
    if (wlist[i]) delete wlist[i];
*/

}

const char* x_display_info(int& width, int& height, int& dpi) 
{ 
  x_open_display();

  width = display_width;
  height = display_height;
  dpi = display_dpi;
  return "mswin"; 
}  


int x_window_bits_saved(Window) { return 1; }


int x_create_buffer(Window win, int w, int h) 
{ ms_win* wp = wlist[win];
  HDC  hdc  = wp->hdc;

  assert(hdc != NULL); 

  if (wp->hdcMem != NULL && w == wp->buf_w && h == wp->buf_h) return 0;

  x_delete_buffer(win);

  HBITMAP hbm = CreateCompatibleBitmap(hdc,w+2,h+2);

  if (hbm == NULL) 
  { char buf[512];
    int err = get_last_error(buf,256);
    fprintf(stderr,"x_create_buffer (%d x %d) error (%d): %s\n", w,h,err,buf);
    fflush(stderr);
    return 0;
   }

  SIZE sz;
  SetBitmapDimensionEx(hbm,w,h,&sz);


  HDC hdcMem = CreateCompatibleDC(hdc) ;
  //if (wp->hbm) DeleteObject(wp->hbm);
  wp->hbm = (HBITMAP)SelectObject(hdcMem,hbm);
  //if (wp->hdcMem) DeleteDC(wp->hdcMem);
  wp->hdcMem = hdcMem;

  HBRUSH hBrush = CreateSolidBrush(wp->BG_COLOR);

  RECT r;
  SetRect(&r,0,0,w+1,h+1);
  FillRect(hdcMem,&r,hBrush);
  DeleteObject(hBrush);
  wp->buf_w = w;
  wp->buf_h= h;
  return 1;
}


int x_create_buffer(Window win) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  RECT r;
  GetClientRect(hwnd,&r);
  return x_create_buffer(win,r.right,r.bottom);
}


void x_delete_buffer(Window win)
{ ms_win* wp = wlist[win];
  HDC  hdcMem  = wp->hdcMem;
  if (hdcMem != NULL)
  { HWND    hwnd = wp->hwnd;
    HBITMAP  hbm = wp->hbm;
    HDC      hdc = GetDC(hwnd);
    DeleteObject(SelectObject(hdcMem,hbm));
    DeleteDC(hdcMem);
    wp->hdc = hdc;
    wp->hdcMem = NULL;
    wp->hbm = NULL;
    ReleaseDC(hwnd,hdc);
  }
}


int x_start_buffering(Window win) 
{ ms_win* wp = wlist[win];
  HDC  hdcMem = wp->hdcMem;
  int new_buf = x_create_buffer(win);
  hdcMem = wp->hdcMem;
  DeleteObject(SelectObject(hdcMem,CreatePenIndirect(&(wp->pen_data))));
  wp->hdc = hdcMem;
  return new_buf;
 }

int x_start_buffering(Window win, int w, int h) 
{ ms_win* wp = wlist[win];
  HDC  hdcMem = wp->hdcMem;

  x_stop_buffering(win);
  x_delete_buffer(win);

  if (x_create_buffer(win,w,h))
  { hdcMem = wp->hdcMem;
    DeleteObject(SelectObject(hdcMem,CreatePenIndirect(&(wp->pen_data))));
    wp->hdc = hdcMem;
    return 1;
   }

  return -1;
 }


void x_set_buffer(Window win, char* pr) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC  hdc = GetDC(hwnd);
  if (pr == 0) 
  { if (wp->hdc1)
    { DeleteDC(wp->hdc);
      wp->hdc = wp->hdc1;
      wp->hdc1 = 0;
     }
   }
  else
  { if (wp->hdc1)
    { fprintf(stderr,"x_set_buffer: nested call.");
      FatalExit(0);
     }
    x_pixrect* im = (x_pixrect*)pr;
    HDC hdc1 = CreateCompatibleDC(hdc) ;
    SelectObject(hdc1,im->map);
    wp->hdc1 = wp->hdc;
    DeleteObject(SelectObject(hdc1,CreatePenIndirect(&(wp->pen_data))));
    wp->hdc = hdc1;
   }
  ReleaseDC(hwnd,hdc);
} 



int x_test_buffer(int win)
{ ms_win* wp = wlist[win];
  return wp->hwnd && (wp->hdcMem == wp->hdc); 
}

//#include <ddraw.h>

void x_flush_buffer(Window win,int x1,int y1,int x2,int y2,int xoff,int yoff)
{       
  ms_win* wp = wlist[win];
  HDC  hdcMem  = wp->hdcMem;

  if (hdcMem)
  { HWND hwnd    = wp->hwnd;
    HDC  hdc     = GetDC(hwnd);
    if (x1 > x2) SWAP(x1,x2);
    if (y1 > y2) SWAP(y1,y2);
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    int w = x2-x1+1;
    int h = y2-y1+1;      
    BitBlt(hdc,x1,y1,w,h,hdcMem,x1+xoff,y1+yoff,SRCCOPY) ;
    ReleaseDC(hwnd,hdc);
  }
}


void x_flush_buffer(int w, int x1, int y1, int x2, int y2) 
{ x_flush_buffer(w,x1,y1,x2,y2,0,0); }



void x_stop_buffering(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = GetDC(hwnd); 
  wp->hdc = hdc;
  DeleteObject(SelectObject(hdc,CreatePenIndirect(&(wp->pen_data))));
  ReleaseDC(hwnd,hdc);
}



void x_stop_buffering(Window win, char** pr)
{ ms_win* wp = wlist[win];
  x_stop_buffering(win);
  x_pixrect* im = NULL;
  HDC  hdcMem  = wp->hdcMem;
  if (hdcMem != NULL)
  { HWND    hwnd = wp->hwnd;
    HBITMAP  hbm = wp->hbm;
    HDC      hdc = GetDC(hwnd);
    im = new x_pixrect;
    im->map = (HBITMAP)SelectObject(hdcMem,hbm);
    SIZE sz;
    GetBitmapDimensionEx(im->map,&sz);
    im->w = sz.cx;
    im->h = sz.cy;
    DeleteDC(hdcMem);
    wp->hdc = hdc;
    wp->hdcMem = NULL;
    wp->hbm = NULL;
    ReleaseDC(hwnd,hdc);
  }
  *pr = (char*)im;
 }


/* windows */

void x_grab_pointer(Window win) 
{ grab_win = win; 
  if (win > 0) 
    SetCapture(wlist[win]->hwnd); 
  else
   ReleaseCapture(); 
}


void x_set_focus(Window win) 
{ ms_win* wp = wlist[win];
  SetFocus(wp->hwnd); 
}

void x_move_pointer(Window win, int x, int y) 
{ x_window_to_screen(win,x,y);
  SetCursorPos(x,y);
}


void x_window_to_screen(Window win, int& x, int& y)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  POINT pt;
  pt.x = x;
  pt.y = y;
  ClientToScreen(hwnd,&pt);
  x = pt.x;
  y = pt.y;
}


void x_screen_to_window(Window win, int& x, int& y)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  POINT pt;
  pt.x = x;
  pt.y = y;
  ScreenToClient(hwnd,&pt);
  x = pt.x;
  y = pt.y;
}




static HWND center_hwnd(Window win, int w, int h)
{
  ms_win* wp = wlist[win];

  // open an invisible window centered over w

  HWND hwnd;

  if (win > 0) 
     hwnd = wp->hwnd;
  else
     hwnd = GetDesktopWindow();


  RECT r;
  GetClientRect(hwnd,&r);
  POINT pt;
  pt.x = (r.right - w)/2;
  pt.y = (r.bottom - h)/2;
  ClientToScreen(hwnd,&pt);

  return CreateWindow (szAppName2,"",WS_POPUPWINDOW,pt.x,pt.y,w,h,hwnd,
                                                           NULL,NULL,NULL);
}




int x_choose_color(Window win, int c)
{ //ms_win* wp = wlist[win];
  //HWND hwnd = wp->hwnd;
  HWND hwndc = center_hwnd(win,400,400); 

  CHOOSECOLOR cc;
  cc.lStructSize = sizeof(CHOOSECOLOR);
  cc.hwndOwner = hwndc;
  cc.hInstance = 0;
  cc.lpCustColors = rgb_custom;
  cc.rgbResult = c;
  cc.Flags = CC_RGBINIT | CC_FULLOPEN;

  int result = 0;

/*
  for(int i=0; i<16; i++) rgb_custom[i] = rgb_table[i];
*/

/*
  if (ChooseColor(&cc)) 
  { rgb_table[c] = cc.rgbResult;
    for(int i=0; i<16; i++) rgb_table[i] = rgb_custom[i];
    //InvalidateRect(hwnd,NULL,TRUE);
    result = 1;
   }
*/


  DestroyWindow(hwndc);
  return result;
}



static UINT_PTR CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
/*
  int w = LOWORD(lParam);
  int h = HIWORD(lParam);
  printf("uiMsg = 0x%02x  w = %d  h = %d\n",uiMsg,w,h);
  fflush(stdout);
*/

//if (uiMsg == WM_NOTIFY && ((OFNOTIFY*)lParam)->hdr.code == CDN_INITDONE)
  if (uiMsg == WM_INITDIALOG)
  { int x = display_width/4;
    int y = display_height/6;
    SetWindowPos(GetParent(hdlg),HWND_TOPMOST,x,y,0,0,SWP_NOSIZE);
/*
    int w = display_width - 100;
    int h = display_height/2;
    MoveWindow(hdlg,0,0,w,h,TRUE);
*/
   }

  return 0;
}


int x_choose_file(Window win, int mode, const char* title, const char* filt, 
                                                           const char* def_ext,
                                                           char* dname,
                                                           char* fname)
{ 

//SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  user32_dll_func("SetThreadDpiAwarenessContext",
                   DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);


  HWND hwndc = center_hwnd(win,430,290); 

/*
cout << "dname = " << dname << " length = " << strlen(dname) << endl;
cout << "fname = " << fname << " length = " << strlen(fname) << endl;
*/

  char file_path[256];
  sprintf(file_path,"%s\\%s",dname,fname);


  // static char custfilt[128];
  // if (custfilt[0] == 0) strcpy(custfilt,filt);

  OPENFILENAME ofn;       // common dialog box structure

  ZeroMemory(&ofn, sizeof(OPENFILENAME));

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndc;
  ofn.lpstrFile = file_path;
  ofn.nMaxFile = 256;
  ofn.lpstrFilter = filt;
  ofn.lpstrDefExt = def_ext;

  ofn.nFilterIndex = 1;

  ofn.lpstrInitialDir = dname;

//ofn.nFilterIndex = 0;
//ofn.lpstrCustomFilter = custfilt;
//ofn.nMaxCustFilter = 128;

  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrTitle = title;

/*
  ofn.lpfnHook = (LPOFNHOOKPROC) OFNHookProc;
  ofn.Flags |= OFN_ENABLEHOOK;
  ofn.Flags |= OFN_ENABLESIZING;
*/

  ofn.Flags |= OFN_EXPLORER;
  ofn.Flags |= OFN_NOCHANGEDIR;
  
  int result = 0;

  if (mode == 0)
  { 
    ofn.Flags |= OFN_PATHMUSTEXIST;
    ofn.Flags |= OFN_HIDEREADONLY;
    ofn.Flags |= OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    { int off = ofn.nFileOffset;
      strcpy(fname,file_path+off);
      file_path[off-1] = 0;
      strcpy(dname,file_path);
      result = 1;
     }
   }
  else
  { 
    ofn.Flags |= OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    { int off = ofn.nFileOffset;
      strcpy(fname,file_path+off);
      file_path[off-1] = 0;
      strcpy(dname,file_path);
      result = 1;
     }
   }

  DestroyWindow(hwndc);

  return result;
}


/*
int x_choose_folder(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  char folder[256];
  folder[0] = '\0';

  BROWSEINFO bi;
  bi.hwndOwner = hwnd;
  bi.pidlRoot = NULL;
  bi.pszDisplayName = folder;
  bi.lpszTitle = "Choose Folder";
  bi.ulFlags = NULL;
  bi.lpfn = NULL;

  SHBrowseForFolder(&bi);
}
*/




static void choose_font(Window win, HFONT* hf, LOGFONT* lf)
{ ms_win* wp = wlist[win];
  HWND hwnd  = wp->hwnd;
  HWND hwndc = center_hwnd(win,440,550); 

  CHOOSEFONT cf;
  cf.lStructSize = sizeof(CHOOSEFONT);
  //cf.lpstrTitle = "Choose Font";
  cf.hwndOwner = hwndc;
  cf.lpLogFont = lf;
  cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_NOSCRIPTSEL;
  if (ChooseFont(&cf))
  { DeleteObject(*hf);
    *hf = CreateFontIndirect(lf);
    InvalidateRect(hwnd,NULL,TRUE);
    printf("\n");
    printf("FaceName       = %s\n", lf->lfFaceName);
    printf("Height         = %ld\n", lf->lfHeight);
    printf("Width          = %ld\n", lf->lfWidth);
    printf("Escapement     = %ld\n", lf->lfEscapement);
    printf("Orientation    = %ld\n", lf->lfOrientation);
    printf("Weight         = %ld\n", lf->lfWeight);
    printf("Italic         = %d\n", lf->lfItalic);
    printf("Underline      = %d\n", lf->lfUnderline);
    printf("StrikeOut      = %d\n", lf->lfStrikeOut);
    printf("CharSet        = %d\n", lf->lfCharSet);
    printf("OutPrecision   = %d\n", lf->lfOutPrecision);
    printf("ClipPrecision  = %d\n", lf->lfClipPrecision);
    printf("Quality        = %d\n", lf->lfQuality);
    printf("PitchAndFamily = %d\n", lf->lfPitchAndFamily);
    printf("\n");
    fflush(stdout);
   }
  DestroyWindow(hwndc);
}



 




static void set_cursor(int i) 
{ 
  LPCSTR c;

  switch (i) {

  case XC_sb_v_double_arrow : 
           c = IDC_SIZENS;
           break;

  case XC_sb_h_double_arrow : 
           c = IDC_SIZEWE;
           break;

  case XC_fleur: 
           c = IDC_SIZEALL;
           break;

  case XC_crosshair: 
  case XC_tcross: 
           c = IDC_CROSS;
           break;

  case XC_watch: 
           c = IDC_WAIT;
           break;

/*
  case XC_right_ptr: 
           c = IDC_UPARROW;
           break;
*/

  case XC_hand2: 
           SetCursor(hand_cursor);
           return;

  case XC_dotbox: 
           SetCursor(dot_box_cursor);
           return;

  default: c = IDC_ARROW;
           break;
  }

  SetCursor(LoadCursor(NULL,c));
}




static void x_create_tmp_file(const char* dname, const char* fname)
{ char path[256];
  sprintf(path,"%s%s",dname,fname);
  CreateFile(path,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,
                                                     FILE_ATTRIBUTE_NORMAL,0);
}


static void setup_fonts(Window win)
{
  char save_dir[256];
  GetCurrentDirectory(256,save_dir);

  char tmp_dir[256];
  //GetTempPath(256,tmp_dir);

  sprintf(tmp_dir,"\\tmp");
  CreateDirectory(tmp_dir,NULL);

  sprintf(tmp_dir+strlen(tmp_dir),"\\leda_fonts");
  CreateDirectory(tmp_dir,NULL);

  x_create_tmp_file(tmp_dir,"\\text.font");
  x_create_tmp_file(tmp_dir,"\\italic.font");
  x_create_tmp_file(tmp_dir,"\\bold.font");
  x_create_tmp_file(tmp_dir,"\\fixed.font");
  x_create_tmp_file(tmp_dir,"\\button.font");

  LOGFONT lf;

  char fname[64];
  strcpy(fname,"text.font");
  if (x_choose_file(win,0,"LEDA Font Setup","Leda Fonts \0*.font\0",".font",
                                                              tmp_dir, fname))
  {
    if (strcmp(fname,"text.font")  == 0) 
      choose_font(win,&text_font,&lf);

    if (strcmp(fname,"italic.font")== 0) 
      choose_font(win,&italic_font,&lf);

    if (strcmp(fname,"bold.font")  == 0) 
      choose_font(win,&bold_font,&lf);

    if (strcmp(fname,"fixed.font") == 0) 
      choose_font(win,&fixed_font,&lf);

    if (strcmp(fname,"button.font")== 0) 
      choose_font(win,&button_font,&lf);
   }
  SetCurrentDirectory(save_dir);
}


static void setup_colors(Window win) 
{ 
  int c = white;
  x_choose_color(win,c); 
}




static LRESULT WINAPI WndProc (HWND hwnd, UINT message, WPARAM wParam, 
                                                        LPARAM lParam)
{
  if (trace_events == 2 && message != WM_TIMER)
  { const char * msg = 0;
    if (message < 1024) msg = message_table[message];
    if (msg == 0) msg = "null";
    printf("mess = %s (%03x)  hwnd = %p  wParam = %d lParam = %d\n",
            msg, message, hwnd, (int)wParam, (int)lParam);
    fflush(stdout);
  }

  int win = GetWindowLong(hwnd,0);

  cur_event.win = win;
  cur_event.kind = no_event;
  cur_event.x = 0;
  cur_event.y = 0;
  cur_event.val1 = 0;
  cur_event.val2 = 0;
  cur_event.t = 0; 

  ms_win* wp = wlist[win];

  switch (message) { 

      case XWM_TASKBAR_ICON:
         {
           switch(lParam) {

           case WM_MOUSEMOVE:   break;
           case WM_RBUTTONDOWN: 
           case WM_LBUTTONDOWN: { POINT pos;
                                  GetCursorPos(&pos);
                                  TrackPopupMenu(taskbar_icon_menu,0,
                                                 pos.x,pos.y,0,hwnd,NULL);
                                  PostMessage(hwnd,WM_USER,0,0);
                                  break;
                                 }
           }
           break;
          }


      case WM_COMMAND:
        { 
          switch (LOWORD(wParam)) {

          case XWM_FONT_DIALOG:  setup_fonts(win);
                                 break;
          case XWM_COLOR_DIALOG: setup_colors(win);
                                 break;
          case XWM_CLIPBOARD_DELETE:
                                 OpenClipboard(hwnd);
                                 EmptyClipboard();
                                 break;

          }
          break;
         }


      case WM_DROPFILES:
         { HDROP hDrop = (HDROP)wParam;
           int count = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);
           POINT pt;
           DragQueryPoint(hDrop,&pt);
           if (wp->special_event_handler)
           { for(int i=0; i<count; i++)
             { char fname[80];
               DragQueryFile(hDrop,i,fname,sizeof(fname));
               wp->special_event_handler(wp->inf,"drop",fname,pt.x,pt.y);
              }
            }
           else
           { char label[32];
             sprintf(label,"Files dropped at (%ld,%ld)",pt.x,pt.y);
             char txt[1024];
             sprintf(txt,"\n");
             for(int i=0; i<count; i++)
             { char fname[80];
               DragQueryFile(hDrop,i,fname,sizeof(fname));
               sprintf(txt+strlen(txt),"%s\n",fname);
              }
             sprintf(txt+strlen(txt),"\n");
             MessageBox(NULL,txt,label,MB_OK);
            }
           DragFinish(hDrop);
           break;
          }
      
      case WM_DEVICECHANGE :
         { if (trace_events) 
           { printf("DEVICECHANGE win = %d  wp = %d lp = %d\n", 
                                  win,(int)wParam,(int)lParam);
             fflush(stdout);
            }

           switch (wParam)
           {
             case DBT_DEVICEARRIVAL:
             case DBT_DEVICEREMOVECOMPLETE:
             { int action = 0; // 0 : remove  1: arrival
               if (wParam == DBT_DEVICEARRIVAL)
               { action = 1;
                 if (trace_events)
                   printf("DBT_DEVICEARRIVAL lParam = %d\n", (int)lParam);
                }
               else
               { action = 0;
                 if (trace_events)
                   printf("DBT_DEVICEREMOVECOMPLETE lParam = %d\n",(int)lParam);
                 }
     
               switch (((DEV_BROADCAST_HDR*)lParam)->dbch_devicetype)
               {
                 case  DBT_DEVTYP_DEVICEINTERFACE:
                     { char* dbcc_name = 
                          ((DEV_BROADCAST_DEVICEINTERFACE*)lParam)->dbcc_name;

                       char name[32];
                       char* p = strstr(dbcc_name,"VID_");
                       char* q = strstr(p,"#");
                       int len = int(q-p);
                       if (len > 32) len = 32;
                       strncpy(name,p,len);
                       name[len] = '\0';

                       if (trace_events)
                       { printf("INTERFACE: name = %s  a = %d\n",name,action);
                         fflush(stdout);
                        }

                       if (wp->special_event_handler)
                         wp->special_event_handler(wp->inf,"interface",name,action,0);
                       break;
                       
/*
                       char buf[5];

                       char* p = strstr(name,"VID_") + 4;
                       strncpy(buf,p,4);
                       int vid = strtol(buf,0,16);
                       
                       char* q = strstr(name,"PID_") + 4;
                       strncpy(buf,q,4);
                       int pid = strtol(buf,0,16);

                       if (trace_events)
                       { printf("vid = %04X  pid = %04X\n",vid,pid);
                         fflush(stdout);
                        }

                       cur_event.win = 1;
                       cur_event.kind = device_connect_event;
                       cur_event.x = vid;
                       cur_event.y = pid;
                       cur_event.val1 = action;
                       return 0;
*/
                      }
     
                 case  DBT_DEVTYP_HANDLE:
                       if (trace_events)
                       printf("HANDLE\n");
                       //((DEV_BROADCAST_HANDLE*)lParam)
                       break;
     
                 case  DBT_DEVTYP_OEM:
                       if (trace_events)
                       printf("OEM\n");
                       //((DEV_BROADCAST_OEM*)lParam)
                       break;
     
                 case  DBT_DEVTYP_PORT:
                       if (trace_events)
                       printf("PORT\n");
                       //((DEV_BROADCAST_PORT*)lParam)
                       break;
     
                 case  DBT_DEVTYP_VOLUME:
                       unsigned long mask = 
                         ((DEV_BROADCAST_VOLUME*)lParam)->dbcv_unitmask;

                       char buf[128];
                       int n = 0;
                       for(int i=0; i<32; i++)
                       { if ((mask & (1 << i)) == 0) continue; 
                         buf[n++] = char('A' + i);
                         buf[n++] = ':';
                         buf[n++] = ' ';
                        }
                       buf[n-1] = 0;

                       if (trace_events)
                       { printf("VOLUME: drives = %s  a = %d\n", buf, action);
                         fflush(stdout);
                        }

                       if (wp->special_event_handler)
                       { wp->special_event_handler(wp->inf,"volume",buf,action,0);
                         break;
                        }

                       cur_event.win = 1;
                       cur_event.kind = volume_connect_event;
                       cur_event.val1 = action;
                       cur_event.val2 = mask; 
                       return 0;
                }

               break;
              }
     
             case DBT_DEVNODES_CHANGED:
             if (trace_events)
             printf("DBT_DEVNODES_CHANGED wParam = %d lParam = %d\n",(int)wParam,(int)lParam);
              break;
           }

           if (trace_events) fflush(stdout);
           break;
          }

/*
      case TTS_SHOW:
             { printf("TTS_SHOW\n");
               fflush(stdout);
               break;
             }
*/


/*
      case WM_MOVING:
             { printf("WM_MOVING\n");
               fflush(stdout);
               break;
             }

      case WM_WINDOWPOSCHANGING:
             { printf("WM_WINDOWPOSCHANGING\n");
               fflush(stdout);
               break;
             }

      case WM_WINDOWPOSCHANGED:
             { printf("WM_WINDOWPOSCHANGED\n");
               fflush(stdout);
               break;
             }
         

      case WM_STYLECHANGED:
             { printf("WM_STYLECHANGED\n");
               fflush(stdout);
               //return 0;
             }

      case WM_NCCREATE :
           { //printf("WM_NCCREATE\n");
             //fflush(stdout);
             // must return true otherwise window will not open
             return 1; 
            }
*/
          

      case WM_CREATE :
         { if (trace_events) 
           { printf("CREATE win = %d\n", win);
             fflush(stdout);
            }
           break;
          }

      case WM_SETCURSOR:
         { if (LOWORD(lParam) == HTCLIENT) 
           { if (trace_events) 
             { printf("SETCURSOR win = %d  hitcode = %d\n",win,LOWORD(lParam));
               fflush(stdout);
              }
             set_cursor(wp->cursor);
             return 0;
            }
           break;
          }

      case WM_TIMER: 
         { //if (trace_events) printf("TIMER win = %d  id = %d\n", win, wParam);
           cur_event.kind = timer_event;
           cur_event.val1 = (int)wParam;
           return 0;
          }

      case WM_ERASEBKGND: 
         { if (trace_events) {
             printf("ERASE BACKGROUND: win = %d\n",win);
            }
           return 1;
          }


      case WM_PAINT :
         { 
           RECT rect;
           GetUpdateRect(hwnd,&rect,TRUE);

            if (trace_events) 
            { printf("PAINT win = %d  count = %d %ld %ld %ld %ld\n", win,
                            wp->repaint_count, rect.left,rect.top,rect.right,rect.bottom);
              fflush(stdout);
             }

           if (wp->repaint_count++ > 0) // skip first paint event (why ?)
           { 
             int x = rect.left; 
             int y = rect.top;
             int w = rect.right - rect.left + 1;
             int h = rect.bottom - rect.top + 1;

             if (wp->repaint) {
                (wp->repaint)(wp->inf,x,y,w,h,wp->resizing);
             }
             else
              { //printf("EXPOSURE: win = %d %d %d %d %d\n",win,x,y,w,h);
                cur_event.kind = exposure_event;
                cur_event.x   = x;
                cur_event.y   = y;
                cur_event.val1 = w;
                cur_event.val2 = h;
               }
            }

           ValidateRect(hwnd,NULL);
           //return 0;
           break;
          }


      case WM_ENTERSIZEMOVE:
      case WM_EXITSIZEMOVE:
           { if (trace_events) 
             { printf("%s  win = %d\n",message_table[message],win);
               fflush(stdout);
              }

             if (message == WM_ENTERSIZEMOVE)
               wp->resizing = 1;
             else
               wp->resizing = 0;

             if (wp->repaint)
             { RECT rect;
               GetClientRect(hwnd,&rect);
               int x = rect.left; 
               int y = rect.top;
               int w = rect.right - rect.left + 1;
               int h = rect.bottom - rect.top + 1;
               (wp->repaint)(wp->inf,x,y,w,h,0);
              }
             return 0;
           }

/*
      case WM_SIZING:
        { RECT* prect = (RECT*)lParam;
          printf("WM_SIZING win = %d  %ld %ld %ld %ld\n", win,
                        prect->left,prect->top,prect->right,prect->bottom);
          fflush(stdout);
          return 0;
        }
*/


/*
      case WM_SIZE:
         { if (trace_events) printf("SIZE  win = %d\n", w);
           if (w > wcount) 
           { //fprintf(stderr,"SIZE: win out of range.\n");
             return 0;
            }
           if (wp->hdcMem)
           { int buffering = (wp->hdc == wp->hdcMem);
             x_delete_buffer(w);
             x_create_buffer(w);
             if (buffering) wp->hdc = wp->hdcMem;
            }
           return 0;
          }
*/

/*
      case WM_MOVE:
         { cur_event.kind = configure_event;
           cur_event.x = LOWORD (lParam);
           cur_event.y = HIWORD (lParam);
           cur_event.t = 0;
           if (trace_events) 
             { printf("WM_MOVE win = %d  x = %d  y = %d\n", 
                       win, cur_event.x, cur_event.y);
               fflush(stdout);
              }
           return 0;
          }
*/

      case WM_MOUSEMOVE:
         { cur_event.kind = motion_event;
            short x = LOWORD (lParam);
            short y = HIWORD (lParam);
            cur_event.x = x;
            cur_event.y = y;
            cur_event.t = 0;

            if(wp->hwnd_tt)
            { MSG msg;
              msg.hwnd=wp->hwnd;
              msg.message=WM_MOUSEMOVE;
              msg.wParam=wParam;
              msg.lParam=lParam;
              GetCursorPos(&msg.pt);
              msg.time=GetMessageTime();
              SendMessage(wp->hwnd_tt,TTM_RELAYEVENT,0,(LPARAM)&msg);
            }

            if (trace_events) 
            { printf("MOUSEMOVE win = %d  x = %d  y = %d\n", win, cur_event.x, cur_event.y);
               fflush(stdout);
              }
            return 0;
          }

      case WM_NCMOUSEMOVE:
         { 
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);

           cur_event.kind = motion_event;
           cur_event.x = -1;
           cur_event.y = -1;
           cur_event.t = 0;

           if (trace_events) 
           { printf("NCMOUSEMOVE win = %d  x = %d  y = %d\n", win, cur_event.x, cur_event.y);
             fflush(stdout);
            }

           return 0;
          }



      case WM_KILLFOCUS:
         { if (trace_events) printf("KILLFOCUS\n");
           //if (grab_win == w) x_grab_pointer(0);
           return 0;
          }

      case WM_MOUSEWHEEL: // translate to button press event (X11)
         { cur_event.kind = button_press_event;
           POINT pt;
           pt.x = LOWORD (lParam);
           pt.y = HIWORD (lParam);
           short v = HIWORD (wParam);
           ScreenToClient(hwnd,&pt);
           short x = (short)pt.x;
           short y = (short)pt.y;
           cur_event.x = x;
           cur_event.y = y;
           cur_event.t = 0;
           cur_event.val1 = (v > 0) ? 4 : 5;

           if (trace_events) 
           { printf("MOUSEWHEEL win = %d  x = %d  y = %d val = %d\n", 
                     win, cur_event.x, cur_event.y,v);
             fflush(stdout);
            }
            return 0;
          }



      case WM_LBUTTONDBLCLK : // printf("left double click\n");
      case WM_LBUTTONDOWN : {
           cur_event.kind = button_press_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 1;
           //if (alt_key_down) cur_event.val1 = 2;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;

    button_event = cur_event;
    button_event.t = (unsigned long)GetMessageTime();

           //x_grab_pointer(win);

           if (trace_events)
           { printf("LBUTTONDOWN win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                  cur_event.y);
             fflush(stdout);
            }
           return 0;
         }

      case WM_MBUTTONDBLCLK : // printf("middle double click\n");
      case WM_MBUTTONDOWN : {
           cur_event.kind = button_press_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 2;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;


           if (trace_events)
           { printf("MBUTTONDOWN win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                   cur_event.y);
             fflush(stdout);
            }
           return 0;
         }


      case WM_RBUTTONDBLCLK : // printf("right double click\n");
      case WM_RBUTTONDOWN : {
           cur_event.kind = button_press_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 3;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;

           if (trace_events)
           { printf("RBUTTONDOWN win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                  cur_event.y);
             fflush(stdout);
            }
           return 0;
         }

      case WM_LBUTTONUP : {
           cur_event.kind = button_release_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 1;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;

           //x_grab_pointer(0);

           button_event = cur_event;
           button_event.t = (unsigned long)GetMessageTime();

           if (trace_events)
           { printf("LBUTTONUP win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                 cur_event.y);
             fflush(stdout);
            }
           return 0;
         }

      case WM_MBUTTONUP : {
           cur_event.kind = button_release_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 2;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;

           if (trace_events)
           { printf("MBUTTONUP win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                 cur_event.y);
             fflush(stdout);
            }
           return 0;
         }

      case WM_RBUTTONUP : {
           cur_event.kind = button_release_event;
           short x = LOWORD (lParam);
           short y = HIWORD (lParam);
           cur_event.x = x;
           cur_event.y = y;
           cur_event.val1 = 3;
           if (wParam & MK_SHIFT) cur_event.val1 |= 256;
           if (wParam & MK_CONTROL) cur_event.val1 |= 512;
           //if (wParam & MK_ALT) cur_event.val1 |= 1024;
           if (alt_key_down) cur_event.val1 |= 1024;

           if (trace_events)
           { printf("RBUTTONUP win = %d  x = %d  y = %d\n", win, cur_event.x, 
                                                                 cur_event.y);
             fflush(stdout);
            }
           return 0;
         }

      case WM_SYSKEYDOWN: {
              if (trace_events) 
                { printf("SYSKEYDOWN win = %d wparam = %d\n",win,(int)wParam);
                  fflush(stdout);
                 }

              if (wParam == VK_F10)   
              {  //setup_fonts(win);
/*
                int i = 0;
                while (wlist[i] != wp) i++;
                if (wp->alpha > 25)
                 x_set_alpha(i,wp->alpha - 25);
                else
                 x_set_alpha(i,255);
*/
                if (wp->alpha > 25)
                { wp->alpha -= 25;
                  SetWindowLong(hwnd,GWL_EXSTYLE,
                  GetWindowLong(hwnd,GWL_EXSTYLE) | WS_EX_LAYERED);
                  SetLayeredWindowAttributes(hwnd,0, wp->alpha, LWA_ALPHA);
                  }
                else
                { wp->alpha = 255;
                  SetWindowLong(hwnd,GWL_EXSTYLE,
                  GetWindowLong(hwnd,GWL_EXSTYLE) &~ WS_EX_LAYERED);
                 }
                 RedrawWindow(hwnd,NULL,NULL,
                     RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
                 cur_event.kind = no_event;
                 break;
               }

              if (wParam == VK_MENU)   
              { //printf("VK_MENU\n"); fflush(stdout);
                alt_key_down = 1;
                return 0;
               }
          
              break;
      }

      case WM_SYSKEYUP: {
             if (trace_events) 
                { printf("SYSKEYUP win = %d wparam = %d\n",win,(int)wParam);
                  fflush(stdout);
                 }
             //cur_event.kind = key_release_event;
             //cur_event.val1 = alt_key_down | 1024;
             alt_key_down = 0;
             return 0;
      }

/*
      case WM_SYSCHAR: {
              if (trace_events) 
                { printf("SYSCHAR win = %d code = %d\n",win,(TCHAR)wParam);
                  fflush(stdout);
                 }
             alt_key_down = (TCHAR)wParam;
             cur_event.kind = key_press_event;
             cur_event.val1 = alt_key_down | 1024;
             return 0;
      }
*/


      case WM_KEYUP:
      case WM_KEYDOWN: {

             if (message == WM_KEYDOWN)
             { cur_event.kind = key_press_event;
               if (trace_events) 
                { printf("KEYDOWN win = %d  wParam = %x \n",win,(int)wParam);
                  fflush(stdout);
                 }
              }
             else
             { cur_event.kind = key_release_event;
               if (trace_events) 
                { printf("KEYUP   win = %d  wParam = %x \n",win,(int)wParam);
                  fflush(stdout);
                 }
              }

             cur_event.t = 0;

             switch (wParam) {

              case VK_F1: cur_event.val1 = KEY_F1;
                          break;
              case VK_F2: cur_event.val1 = KEY_F2;
                          break;
              case VK_F3: cur_event.val1 = KEY_F3;
                          break;
              case VK_F4: cur_event.val1 = KEY_F4;
                          break;
              case VK_F5: cur_event.val1 = KEY_F5;
                          break;
              case VK_F6: cur_event.val1 = KEY_F6;
                          break;
              case VK_F7: cur_event.val1 = KEY_F7;
                          break;
              case VK_F8: cur_event.val1 = KEY_F8;
                          break;

              case VK_F9: cur_event.val1 = KEY_F9;
                          break;

              case VK_F10: show_display_info();
                           break;

              case VK_F11: 
                          if (message == WM_KEYDOWN)
                          { if (trace_events == 1) trace_events = 0;
                            else trace_events = 1;
                            printf("LEDA EVENTS = %d\n",trace_events);
				  show_display_info();
                            fflush(stdout);
                           }
                          cur_event.kind = no_event;
                          break;

              case VK_F12:
                          if (message == WM_KEYDOWN)
                          { if (trace_events == 2) trace_events = 0;
                            else trace_events = 2;
                            printf("ALL EVENTS = %d\n",trace_events);
                            show_display_info();
                            fflush(stdout);
                           }
                          cur_event.kind = no_event;
                          break;

              case VK_INSERT: cur_event.val1 = KEY_INSERT;
                              break;
              case VK_DELETE: cur_event.val1 = KEY_DELETE;
                              break;
              case VK_NEXT:   cur_event.val1 = KEY_PAGE_DOWN;
                              break;
              case VK_PRIOR:  cur_event.val1 = KEY_PAGE_UP;
                              break;
              case VK_HOME:   cur_event.val1 = KEY_HOME;
                              break;
              case VK_END:    cur_event.val1 = KEY_END;
                              break;
              case VK_UP:     cur_event.val1 = KEY_UP;
                              break;
              case VK_DOWN:   cur_event.val1 = KEY_DOWN;
                              break;
              case VK_LEFT:   cur_event.val1 = KEY_LEFT;
                              break;
              case VK_RIGHT:  cur_event.val1 = KEY_RIGHT;
                              break;
              case VK_TAB:    cur_event.val1 = KEY_TAB;
                              break;
              case VK_ESCAPE: cur_event.val1 = KEY_ESCAPE;
                              break;
              case VK_RETURN: cur_event.val1 = KEY_RETURN;
                              break;
              case VK_BACK:   cur_event.val1 = KEY_BACKSPACE;
                              break;

              case VK_SCROLL: 
              case VK_PRINT:  cur_event.val1 = KEY_PRINT;
                              break;

              default:        cur_event.kind = no_event;
                              break;

             }
             return 0;
           }



      case WM_CHAR: {
           char c = (char)wParam;
           if (trace_events) 
           { printf("CHAR win = %d  c = %d\n",win,c);
             fflush(stdout);
            }
           //if (isprint(c))
           if (!iscntrl(c))
           { cur_event.kind = key_press_event;
             cur_event.val1 = c;
            }
           return 0;
         }
         


      case WM_CLOSE:
           if (trace_events) printf("WM_CLOSE\n");
           cur_event.kind = destroy_event;
           return 0;


      case WM_DESTROY :
           if (trace_events) printf("DESTROY: w = %d\n",win);
           return 0;

     }

  //return (long)DefWindowProc (hwnd, message, wParam, lParam);
  return DefWindowProc (hwnd, message, wParam, lParam);
}


Window x_create_window(void* inf, int width,int height,int bg_col,
                       const char* label, const char* icon_label,
                       int pwin, redraw_func redraw)
{
  int i = 1;
  while (i <= wcount && wlist[i] != 0) i++;

  if (i > wcount) wcount = i;

  if (wcount >= MAX_WIN) 
  { fprintf(stderr, "\n Too many Windows (%d).\n",wcount);
    FatalExit(0);
  }

  ms_win* wp = new ms_win(inf,width,height,bg_col,label,redraw);

  wlist[i] = wp;
  return i;
}


void x_set_clip_rectangle(Window win, int x0, int y0, int w, int h) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (hwnd && wp->hdc)
  { HDC hdc = wp->hdc;
    HRGN hrgn = CreateRectRgn(x0,y0,x0+w,y0+h);
    SelectClipRgn(hdc,hrgn);
    if (hdc == wp->hdcMem) {
      //buffering (set clip rectangle also in original window)
      HDC hdc0 = GetDC(hwnd); 
      SelectClipRgn(hdc0,hrgn);
    }
    DeleteObject(hrgn);
   }
 }


void x_bezier_ellipse(Window win, int x, int y, int r1, int r2)
{
  ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;

  int f1 = int(1.33*r1);
  int f2 = int(0.93*r2);

  POINT p[7];
  p[0].x = x;    p[0].y = y-r2;
  p[1].x = x+f1; p[1].y = y-f2;
  p[2].x = x+f1; p[2].y = y+f2;
  p[3].x = x;    p[3].y = y+r2;
  p[4].x = x-f1; p[4].y = y+f2;
  p[5].x = x-f1; p[5].y = y-f2;
  p[6].x = x;    p[6].y = y-r2;

  PolyBezier(hdc,p,7);
}


void x_clip_mask_rectangle(Window win,int x0,int y0,int x1,int y1,int mode) 
{ int xc[4];
  int yc[4];
  xc[0] = x0; yc[0] = y0;
  xc[1] = x0; yc[1] = y1;
  xc[2] = x1; yc[2] = y1;
  xc[3] = x1; yc[3] = y0;
  x_clip_mask_polygon(win,4,xc,yc,mode); 
}


void x_clip_mask_ellipse(Window win, int x, int y, int r1, int r2, int mode) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;
  if (hwnd == NULL) return;

  BeginPath(hdc);
  //Ellipse(hdc,x-r1,y-r2,x+r1,y+r2);
  x_bezier_ellipse(win,x,y,r1,r2);
  EndPath(hdc);

  switch (mode) {
  case 0: SelectClipPath(hdc,RGN_DIFF);
          break;
  case 1: SelectClipPath(hdc,RGN_OR);
          break;
  case 2: SelectClipPath(hdc,RGN_COPY);
          break;
  case 3: SelectClipPath(hdc,RGN_XOR);
          break;
  }

}


void x_clip_mask_chord(Window win, int x0, int y0, int x1, int y1, int cx, int cy, int r, int mode) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;
  if (hwnd == NULL) return;

  BeginPath(hdc);
  MoveToEx(hdc, x0,y0, 0);
  ArcTo(hdc, cx-r,cy-r, cx+r+1,cy+r+1, x0,y0, x1,y1);
  LineTo(hdc, x1,y1); 
  LineTo(hdc, x0,y0); // make sure that the line (x0,y0) - (x1,y1) is contained
  EndPath(hdc);

  switch (mode) {
  case 0: SelectClipPath(hdc,RGN_DIFF);
          break;
  case 1: SelectClipPath(hdc,RGN_OR);
          break;
  case 2: SelectClipPath(hdc,RGN_COPY);
          break;
  case 3: 
  case 4: SelectClipPath(hdc,RGN_XOR);
          break;
  }

}



void x_clip_mask_polygon(Window win, int n, int* xcoord, int* ycoord, int mode) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;

  if (hwnd == NULL) return;

/*
  if (n == 0)
  { SelectClipRgn(hdc,NULL);
    return;
   }
*/

 POINT* p = 0;

  if (n == 0)
  { //entire window 
    n = 5;
    p = new POINT[n];
    RECT rect;
    GetClientRect(hwnd,&rect);
    p[0].x = rect.left;  p[0].y = rect.top;
    p[1].x = rect.right; p[1].y = rect.top;
    p[2].x = rect.right; p[2].y = rect.bottom;
    p[3].x = rect.left;  p[3].y = rect.bottom;
    p[4].x = rect.left;  p[4].y = rect.top;
  }
 else
 { p = new POINT[n];
   for(int i=0; i < n; i++) 
   { p[i].x = xcoord[i];
     p[i].y = ycoord[i];
    }
  }

  BeginPath(hdc);
  Polygon(hdc,p,n);
  EndPath(hdc);

  switch (mode) {
  case 0: SelectClipPath(hdc,RGN_DIFF);
          break;
  case 1: SelectClipPath(hdc,RGN_OR);
          break;
  case 2: SelectClipPath(hdc,RGN_COPY);
          break;
  case 3:
  case 4: SelectClipPath(hdc,RGN_XOR);
          break;
  }

  if (p) delete[] p;
 }


void* x_window_handle(Window win)
{ ms_win* wp = wlist[win];
  return wp->hwnd;
 }


int  x_window_opened(Window win) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  return hwnd && IsWindowVisible(hwnd); 
 }

 
void x_set_topmost(Window win)  
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

//SetWindowPos(hwnd,HWND_TOP,0,0,0,0,SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

  BringWindowToTop(hwnd);
  SetActiveWindow(hwnd);
  ShowWindow(hwnd,SW_SHOW);
 }


void x_resize_window(Window win, int xpos, int ypos, int width,int height,int)  
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

/*
cout << "resize window" << endl;
cout << "width = " << width << endl;
cout << "height = " << height << endl;
*/

  char class_name[32];
  GetClassName(hwnd,class_name,32);

  if (strcmp(class_name,szAppName1) == 0)
  { width  += 2*GetSystemMetrics(SM_CXFRAME);
    height += 2*GetSystemMetrics(SM_CYFRAME);
    height += GetSystemMetrics(SM_CYCAPTION);
   }
  else
  { width  += 2*GetSystemMetrics(SM_CXBORDER);
    height += 2*GetSystemMetrics(SM_CYBORDER);
   }

  MoveWindow(hwnd,xpos,ypos,width,height,TRUE);

/*
  SetWindowPos(hwnd,//HWND_TOP,
                    //HWND_TOPMOST,
                    HWND_BOTTOM,
                    xpos,ypos,width,height,
                    SWP_DRAWFRAME | SWP_NOACTIVATE);

  x_clear_window(win,0,0,width,height);
*/

  if (wp->repaint)
     (wp->repaint)(wp->inf,0,0,width,height,0);
}


void x_open_window(Window win, int x, int y, int width, int height, int pwin, bool hidden) 
{ 
  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  HWND hwndparent = 0;
  DWORD win_style = 0;
  DWORD win_ex_style = 0;
  char* class_name = 0;

  int w0 = width;
  int h0 = height;

  if (width  < 0) width  = -width;
  if (height < 0) height = -height;
  
  if (pwin == 0)
  { 
/*
    width  += 2*GetSystemMetrics(SM_CXFRAME);
    height += GetSystemMetrics(SM_CYCAPTION);
    height += 2*GetSystemMetrics(SM_CYFRAME);
*/

    hwndparent = NULL;
    win_style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
    win_ex_style = WS_EX_APPWINDOW;
    class_name = szAppName1;
   }
  else
  { 
    width  += 2*GetSystemMetrics(SM_CXBORDER);
    height += 2*GetSystemMetrics(SM_CYBORDER);

    if (pwin < 0)
    { hwndparent = NULL;
      win_style = WS_POPUPWINDOW | WS_CLIPCHILDREN;
      win_ex_style = WS_EX_TOOLWINDOW;
      class_name = szAppName2;
     }

    if (pwin > 0)
    { hwndparent = wlist[pwin]->hwnd;
      win_style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; 
      win_ex_style = WS_EX_TOOLWINDOW;
      if (wp->border_w > 0)
         win_style |= WS_BORDER; 
      else
        { width  -= 2;
          height -= 2;
         }
      class_name = szAppName2;
     }
   }

  if (w0 < 0) x = x - (width-1);
  if (h0 < 0) y = y - (height-1);


/*
  int dy = disp_height - height;
  int dx = disp_width - width;

  // restrict window size to display size

  if (dy < 0) 
  { float f = float(disp_height)/height; 
    height = int(f*height); 
    width  = int(f*width);
    dy = 0; 
   }

  if (dx < 0) 
  { float f = float(disp_width)/width; 
    width  = int(f*width);
    height = int(f*height); 
    dx = 0; 
   }
  if (pwin == 0)
  { if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    if ( x > dx ) x = dx;
    if ( y > dy ) y = dy;
   }

*/

/*
  int min_x = -width + 100;
  int min_y = -height + 100;
*/

  int min_x = 0;
  int min_y = 0;

  int max_x = display_width - 100;
  int max_y = display_height - 100;

  if (pwin == 0)
  { if (x < min_x) x = min_x;
    if (y < min_y) y = min_y;
    if (x > max_x) x = max_x;
    if (y > max_y) y = max_y;
   }


  if (!hwnd)
  { 
    //win_style |= WS_VISIBLE;

    //win_ex_style |= WS_EX_LAYERED;
    //win_ex_style |= WS_EX_TRANSPARENT;

/*
cout << "open window" << endl;
cout << "width = " << width << endl;
cout << "height = " << height << endl;
*/

    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;
    rect.bottom = height;
    //AdjustWindowRectEx(&rect,win_style,FALSE,win_ex_style);
    AdjustWindowRect(&rect,win_style,FALSE);

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

/*
cout << endl;
cout << "AdjustWindowRect" << endl;
cout << "left  = " << rect.left << endl;
cout << "right = " << width << endl;
cout << "top   = " << rect.top << endl;
cout << "bottom= " << height << endl;
cout << "width = " << width << endl;
cout << "height = " << height << endl;
*/


    hwnd = CreateWindowEx(win_ex_style,         // extended style
                          class_name,           // window class name
                          wp->header,           // frame label
                          win_style,            // window style
                          x,y,                  // window position
                          width,height,         // window size
                          hwndparent,           // handle parent window 
                          NULL,                 // handle menu
                          NULL,                 // handle program copy
                          NULL);                // special parameter

    if (hwnd == 0)  
      show_last_error("CreateWindowEx: ");

    SetWindowLong(hwnd,0,win);

    if (wp->alpha < 255 && wp->alpha > 0)
    { SetLastError(0);
      SetWindowLong(hwnd,GWL_EXSTYLE,
                        GetWindowLong(hwnd,GWL_EXSTYLE) | WS_EX_LAYERED);
      bool ok = (SetLayeredWindowAttributes(hwnd,0, wp->alpha, LWA_ALPHA) != 0);
      //if (!ok)  show_last_error("SetLayeredWindow: ");
     }


    wp->hwnd = hwnd;


    wp->hwnd_tt=CreateWindowEx(0,TOOLTIPS_CLASS,NULL,
                                 WS_POPUP | TTS_ALWAYSTIP,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 10,10,hwnd,NULL,NULL,NULL);
  }


  // set background pixmap

  if (wp->BG_PIXMAP)
  { x_pixrect* im = (x_pixrect*)wp->BG_PIXMAP;
    wp->BG_BRUSH = CreatePatternBrush(im->map);
   }
  else
    wp->BG_BRUSH = CreateSolidBrush(wp->BG_COLOR);


  if (pwin == 0)
   { MoveWindow(hwnd,x,y,width,height,FALSE);
     ShowWindow (hwnd, hidden ? SW_HIDE : SW_SHOWNORMAL);
    }
  else
   { //HWND save_focus = GetFocus();
     SetParent(hwnd,hwndparent);
     if (pwin < 0) 
     { //SetFocus(save_focus);
       SetWindowPos(hwnd,HWND_TOPMOST,x,y,width,height,
                         SWP_NOACTIVATE|SWP_SHOWWINDOW);
       //SetFocus(hwnd);
      }
     if (pwin > 0) 
     { //MoveWindow(hwnd,x,y,width,height,FALSE);
       //ShowWindow(hwnd,SW_SHOWNOACTIVATE);
       SetWindowPos(hwnd,HWND_TOP,x,y,width,height,
                         SWP_NOACTIVATE|SWP_SHOWWINDOW);
       //SetFocus(hwnd);
      }
    }


  // initialize hdc

  HDC hdc = GetDC(hwnd);
  SetGraphicsMode(hdc,GM_ADVANCED);
  wp->hdc = hdc;


  // process initial events

  if (! hidden)
  { // consume available events
    MSG msg;
    while (PeekMessage(&msg,hwnd,0,0,PM_REMOVE) != 0)
    { TranslateMessage(&msg);
      DispatchMessage(&msg);
      //cout << "peek message" << endl;
     }
   }


  // set window params

  wp->font = text_font;
  //SelectObject(hdc, text_font);

  x_set_color(win,wp->COLOR);
  x_set_mode(win,wp->MODE);
  x_set_text_mode(win,wp->TMODE);
  x_set_line_width(win,wp->LWIDTH);
  x_set_line_style(win,wp->LSTYLE);


  if (pwin == 0 && taskbar_icon_win == 0) 
  { x_add_taskbar_icon(hwnd,leda_small_icon,wp->header);
    taskbar_icon_win = hwnd;
   }

  DragAcceptFiles(hwnd,TRUE);

  wp->hDeviceNotify = 0;

  if (pwin ==0)
  { DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

    wp->hDeviceNotify = RegisterDeviceNotification(hwnd,
                                                   &NotificationFilter,
                                                   DEVICE_NOTIFY_WINDOW_HANDLE);
/*
   if (wp->hDeviceNotify == NULL) 
   { printf("RegisterDeviceNotify failed\n");
     fflush(stdout);
    }
*/
  }

}


void x_close_window(Window win) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;

//cout << "x_close_window w = " << win << endl;

  if (wp->hDeviceNotify) UnregisterDeviceNotification(wp->hDeviceNotify); 

  if (taskbar_icon_win == hwnd) 
  { x_del_taskbar_icon(hwnd);
    taskbar_icon_win = 0;
   }

  if (hwnd) ShowWindow(hwnd,SW_HIDE);
  if (grab_win == win) x_grab_pointer(0);
  ReleaseDC(hwnd,hdc);
  wp->hdc = NULL;
 }


void x_destroy_window(Window win) 
{ 
  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  x_delete_buffer(win);
  x_stop_timer(win);

  wlist[win] = 0;

  if (wp->hmf) DeleteEnhMetaFile(wp->hmf);

  if (hwnd) 
  { DestroyWindow(hwnd);
    DestroyWindow(wp->hwnd_tt);
   }

  delete wp;

  if (wcount == win) wcount--;
 }


int x_set_cursor(Window win, int i) 
{ ms_win* wp = wlist[win];
/* printf("x_set_cursor\n");
   fflush(stdout);
*/
  if (wp->cursor == i) return i;
  int i0 = wp->cursor;
  wp->cursor = i;
  set_cursor(i);
  return i0;
 }

void x_set_label(Window win, const char *s)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
/*
  char* p = new char[strlen(s)+1];
  sprintf(p,"%s",s);
  printf("win = %d  wp = %d hwnd = %d s = %s\n",win, wp, hwnd,p);
  fflush(stdout);
  SetWindowText(hwnd,p);
  InvalidateRect(hwnd,NULL,TRUE);
*/
  if (wp->header && strcmp(wp->header,s) == 0) return;
  delete[] wp->header;
  wp->header = new char[strlen(s) + 1];
  strcpy(wp->header,s);
  if (hwnd && IsWindowVisible(hwnd)) SetWindowText(hwnd,s);
 }



void x_start_timer(Window win, int msec)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  x_stop_timer(win);
  wp->timer_id = (UINT)SetTimer(hwnd,1,msec,NULL);
 }

void x_stop_timer(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  UINT id = wp->timer_id;
  if (id) KillTimer(hwnd,id);
  wp->timer_id = 0;
 }



void* x_window_inf(Window win) 
{ ms_win* wp = wlist[win];
  return wp->inf; 
}

int x_window_width(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (hwnd)
     { RECT r;
       GetClientRect(wp->hwnd,&r);
       return r.right; 
      }
   else
     return wp->width;
 }

int x_window_height(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (hwnd)
     { RECT r;
       GetClientRect(wp->hwnd,&r);
       return r.bottom; 
      }
   else
     return wp->height;
 }

int x_window_minimized(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (hwnd)
  { WINDOWPLACEMENT p;
    p.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(wp->hwnd,&p);
    //return p.showCmd == SW_MINIMIZE; 
    return p.showCmd == SW_SHOWMINIMIZED; 
   }
  return 0;
 }



void x_window_frame(Window win, int& x0, int& y0, int& x1, int& y1)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (hwnd)
    { RECT r;
      GetWindowRect(hwnd,&r);
      x0 = r.left;
      y0 = r.top;
      x1 = r.right-1;
      y1 = r.bottom-1;
     }
  else
    { x0 = 0;
      y0 = 0;
      x1 = 0;
      y1 = 0;
     }
}


void x_clear_window(Window win, int x0, int y0, int x1, int y1, int xorig, 
                                                                int yorig)
{ 
  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;
  char* pm = wp->BG_PIXMAP;
  HBRUSH brush = wp->BG_BRUSH;

  if (x0 > x1)  SWAP(x0,x1);
  if (y0 > y1)  SWAP(y0,y1);

  if (!x_window_opened(win)) return;


  if (pm == 0 || (xorig == 0 && yorig == 0))
  { RECT rect;
    SetRect(&rect,x0,y0,x1+1,y1+1);
    FillRect(hdc,&rect,brush);
    return;
   }

  x_pixrect* im = (x_pixrect*)pm;
  int w = im->w;
  int h = im->h;

  HDC hdcMem  = CreateCompatibleDC (hdc) ;
  SelectObject(hdcMem,im->map);

  if (xorig > 0)
     while (xorig > 0) xorig -= w;
  else
     while (xorig+w < 0) xorig += w;

  if (yorig > 0)
     while (yorig > 0) yorig -= h;
  else
     while (yorig+h < 0) yorig += h;

  int xmax,ymax;
  if (x_test_buffer(win))
  { xmax = wp->buf_w;
    ymax = wp->buf_h;
   }
  else
  { RECT r;
    GetClientRect(hwnd,&r);
    xmax = r.right;
    ymax = r.bottom;
   }

/*
  RECT rect;
  SetRect(&rect,xorig,yorig,xmax,ymax);
  FillRect(hdc,&rect,wp->BG_BRUSH);
*/

  for(int y = yorig;  y < ymax; y += h)
    for(int x = xorig; x < xmax; x += w)
      if (x < x1 && x+w > x0 && y < y1 && y+h >y0)
        BitBlt(hdc,x,y,w,h,hdcMem,0,0,SRCCOPY);

  DeleteDC (hdcMem) ;
}


//------------------------------------------------------------------------------
// drawing
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


void x_line0(int w, int x1, int y1, int x2, int y2)
{ // draws a closed line (including endpoints)

  x_line(w,x1,y1,x2,y2);

  int d = (wlist[w]->LWIDTH - 1)/2;
  x_box(w,x1-d,y1-d,x1+d,y1+d);
  x_box(w,x2-d,y2-d,x2+d,y2+d);

/*
  int js = x_set_join_style(w,3);
  x_line(w,x1,y1,x2,y2);
  x_set_join_style(w,js);
*/
}



void x_line(Window win, int x1, int y1, int x2, int y2)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;

  int jstyle = wp->JSTYLE;
  int lw     = wp->LWIDTH;
  int jleft  = (jstyle & 1);
  int jright = (jstyle & 2);

  if (x1 > x2 || (x1 == x2 && y1 > y2)) 
  { SWAP(x1,x2);
    SWAP(y1,y2);
    SWAP(jleft,jright);
   }

  if (!jleft ) adjust_line(-lw/2-1,x2,y2,x1,y1);
  if (!jright) adjust_line(-lw/2,  x1,y1,x2,y2);
  if ( jright) adjust_line( lw/2,  x1,y1,x2,y2);

  MoveToEx(hdc,x1,y1,NULL);
  LineTo(hdc,x2,y2);
 }


void x_lines(int w, int n, int *x1, int *y1, int* x2, int* y2)
{ while (n--) x_line(w,*x1++,*y1++,*x2++,*y2++); }



void x_rect(Window win, int x1, int y1, int x2, int y2)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
  Rectangle(hdc,x1,y1,x2+1,y2+1);
  SelectObject(hdc,save_brush);
 }


void x_box(Window win, int x1, int y1, int x2, int y2)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  COLORREF col = wp->pen_data.lopnColor;

  RECT r;
  SetRect(&r,x1,y1,x2+1,y2+1);

  HBRUSH hBrush;
  if (wp->stip_bm)
   { hBrush = CreatePatternBrush(wp->stip_bm);
     SetBkColor(hdc,wp->stip_bgcol);
    }
  else
    hBrush = CreateSolidBrush(col);

 
  FillRect(hdc,&r,hBrush);
  DeleteObject(hBrush);
}


void x_circle0(Window win, int x0,int y0,int r0)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  int col = wp->pen_data.lopnColor;
  int lw  = wp->LWIDTH;

  int rin  = r0-lw/2;
  int rout = r0+lw/2;

  if ( (lw % 2) == 0) rout--;

  for (int r = rin; r <= rout; r++)
  { int y = r;
    int x = 0;
    int e = 3-2*y;

    SetPixel(hdc,x0,y0+r,col);
    SetPixel(hdc,x0,y0-r,col);
    SetPixel(hdc,x0+r,y0,col);
    SetPixel(hdc,x0-r,y0,col);


    for (x=1;x<y;)
      { SetPixel(hdc,x0+x,y0+y,col);
        SetPixel(hdc,x0+x,y0-y,col);
        SetPixel(hdc,x0-x,y0+y,col);
        SetPixel(hdc,x0-x,y0-y,col);
        SetPixel(hdc,x0+y,y0+x,col);
        SetPixel(hdc,x0+y,y0-x,col);
        SetPixel(hdc,x0-y,y0+x,col);
        SetPixel(hdc,x0-y,y0-x,col);
        x++;
        if (e>=0) { y--; e = e - 4*y; }
        e = e + 4*x + 2;
       }

    if (x == y)
    { SetPixel(hdc,x0+x,y0+y,col);
      SetPixel(hdc,x0+x,y0-y,col);
      SetPixel(hdc,x0-x,y0+y,col);
      SetPixel(hdc,x0-x,y0-y,col);
     }
  }
}


void x_ellipse(Window win, int x, int y, int r1, int r2)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
  Ellipse(hdc,x-r1,y-r2,x+r1,y+r2);
  //Ellipse(hdc,x-r1,y-r2,x+r1+1,y+r2+1);
  SelectObject(hdc,save_brush);
}



void x_fill_ellipse(Window win, int x, int y, int r1, int r2)
{ ms_win* wp = wlist[win];
  int lw = x_set_line_width(win,1); 
  HDC  hdc = wp->hdc;
  COLORREF col = wp->pen_data.lopnColor;

  HBRUSH hBrush;
  if (wp->stip_bm)
   { hBrush = CreatePatternBrush(wp->stip_bm);
     SetBkColor(hdc,wp->stip_bgcol);
    }
  else
    hBrush = CreateSolidBrush(col);

  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,hBrush);
  
//Ellipse(hdc,x-r1,y-r2,x+r1,  y+r2);
  Ellipse(hdc,x-r1,y-r2,x+r1+1,y+r2+1);

  DeleteObject(SelectObject(hdc,save_brush));
  x_set_line_width(win,lw);
}



void x_polyline(Window win, int n, int* xcoord, int* ycoord, int adjust)
{ if (adjust == 1) return;
  ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  POINT* p = new POINT[n];
  for(int i=0; i < n; i++) 
  { p[i].x = xcoord[i];
    p[i].y = ycoord[i];
   }
  Polyline(hdc,p,n);
  delete[] p;
}


static void x_clip_polyline(Window win, int n, int* xcoord, 
                                            int* ycoord, int mode) 
{ 
  // does not work correctly (uses polygon instead of line)

  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;

  if (hwnd == NULL) return;

  if (n == 0)
  { SelectClipRgn(hdc,NULL);
    return;
   }

  POINT* p = new POINT[n];
  for(int i=0; i < n; i++) 
  { p[i].x = xcoord[i];
    p[i].y = ycoord[i];
   }

  BeginPath(hdc);
  Polyline(hdc,p,n);
  EndPath(hdc);

  switch (mode) {
  case 0: SelectClipPath(hdc,RGN_DIFF);
          break;
  case 1: SelectClipPath(hdc,RGN_OR);
          break;
  case 2: SelectClipPath(hdc,RGN_COPY);
          break;
  case 3:
  case 4: SelectClipPath(hdc,RGN_XOR);
          break;
  }

  delete[] p;
}


void x_polyline(Window win, int n, int* xcoord, int* ycoord, int* clr)
{
  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  HDC hdc = wp->hdc;

  RECT r;
  GetClientRect(hwnd,&r);
  int w = r.right;
  int h = r.bottom;
  int lw = wp->LWIDTH;

  POINT* p = new POINT[n];
  for(int i=0; i < n; i++) 
  { p[i].x = xcoord[i];
    p[i].y = ycoord[i];
   }


  // create mask bitmap

  HBITMAP hbm_mask  = CreateBitmap(w, h, 1, 1, NULL);
  HDC hdcMask = CreateCompatibleDC(hdc) ;
  SelectObject(hdcMask,hbm_mask);
  HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));
  FillRect(hdcMask,&r,hBrush);
  DeleteObject(hBrush);
  SelectObject(hdcMask,CreatePen(PS_SOLID,lw,RGB(0,0,0)));
  Polyline(hdcMask,p,n);

  // create src pixmap

  HDC hdcSrc = CreateCompatibleDC(hdc) ;
  HBITMAP hbm_src = CreateCompatibleBitmap(hdc,w,h);
  SelectObject(hdcSrc,hbm_src);
  MoveToEx(hdcSrc,p[0].x,p[0].y,NULL);
  for(int i=1; i<n;i++) 
  { COLORREF col = GetNearestColor(hdcSrc,clr[i]);
    DeleteObject(SelectObject(hdcSrc,CreatePen(PS_SOLID,2*lw+1,col)));
    LineTo(hdcSrc,p[i].x,p[i].y);
   }

   // 0:DEST  1: SRC 
   MaskBlt(hdc,0,0,w,h,hdcSrc,0,0,hbm_mask,0,0,MAKEROP4(0x00AA0029,SRCCOPY));

/* check bitmap
  HBRUSH  save_brush = (HBRUSH)SelectObject(hdc,CreateSolidBrush(blue));
  // PSDPxax:  pat x (src a (dest x pat))  src=0 --> pat  src=1 ---> dest
  BitBlt(hdc,0,0,w,h,hdcMask,0,0,0x00B8074A); 
  DeleteObject(SelectObject(hdc,save_brush));
 */


  delete[] p;

  DeleteObject(hbm_mask);
  DeleteObject(hbm_src);
  DeleteDC(hdcSrc);
  DeleteDC(hdcMask);
}


void x_polygon(Window win, int n, int* xcoord, int* ycoord)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  POINT* p = new POINT[n];
  for(int i=0; i < n; i++) 
  { p[i].x = xcoord[i];
    p[i].y = ycoord[i];
   }

  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));

  Polygon(hdc,p,n);

  DeleteObject(SelectObject(hdc,save_brush));

  delete[] p;
}




void x_fill_polygon(Window win, int n, int* xcoord, int* ycoord)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;

  COLORREF col = wp->pen_data.lopnColor;

  POINT* p = new POINT[n];
  for(int i=0; i < n; i++) 
  { p[i].x = xcoord[i];
    p[i].y = ycoord[i];
   }

  LOGPEN lp = wp->pen_data;
  lp.lopnStyle = PS_NULL;
  DeleteObject(SelectObject(hdc,CreatePenIndirect(&lp)));

  HBRUSH hBrush;
  if (wp->stip_bm)
    { hBrush = CreatePatternBrush(wp->stip_bm);
      SetBkColor(hdc,wp->stip_bgcol);
     }
  else
     hBrush = CreateSolidBrush(col);

  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,hBrush);

  SetPolyFillMode(hdc,WINDING);
  Polygon(hdc,p,n);

  DeleteObject(SelectObject(hdc,save_brush));
  DeleteObject(SelectObject(hdc,CreatePenIndirect(&wp->pen_data)));

  delete[] p;
}




void x_pixel(Window win, int x, int y)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  SetPixel(hdc,x,y,wp->pen_data.lopnColor);
}


int x_get_pixel(Window win, int x, int y)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  int col = GetPixel(hdc,x,y); 
  return col;
}



void x_pixels(Window win, int n, int* x, int* y)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  while (n--) SetPixel(hdc,x[n],y[n],wp->pen_data.lopnColor);
}

void x_arc(Window win,int mx,int my,int r1,int r2,double start,double angle)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;

  if (angle > 0)
  { start += angle;
    angle = - angle;
   }

  int R  = r1+r2;
  int x0 = int(mx + R*cos(start+angle));
  int y0 = int(my - R*sin(start+angle));
  int x1 = int(mx + R*cos(start));
  int y1 = int(my - R*sin(start));
  Arc(hdc,mx-r1,my-r2,mx+r1+1,my+r2+1,x0,y0,x1,y1);
}

void x_text_underline(Window win, int x, int y, const char* s, int l, int r)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  y += (x_text_height(win,s)-1);
  int x1 = x+x_text_width(win,s,l);
  int x2 = x+x_text_width(win,s,r+1);
  MoveToEx(hdc,x1,y,NULL);
  LineTo(hdc,x2,y);
 }


void x_text(Window win, int x, int y, const char* s0, int l)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;
  COLORREF col = wp->pen_data.lopnColor;
  COLORREF bcol = wp->BG_COLOR;

  if (l > (int)strlen(s0)) l = (int)strlen(s0);

  const char* s = s0;
 
  HFONT save_font = (HFONT)SelectObject(hdc,wp->font);

  SIZE sz;
  GetTextExtentPoint(hdc,s,l,&sz);

  int w = sz.cx;
  int h = sz.cy;

  if (wp->font == fixed_font) y += 3;

  if (wp->TMODE == opaque)
  { SetBkColor(hdc,bcol);
    TextOut(hdc,x,y,s,l);
    SelectObject(hdc,save_font);
    return;
   }

  if (wp->MODE == src_mode)
  { SetBkMode(hdc,TRANSPARENT);
    TextOut(hdc,x,y,s,l);
    SetBkMode(hdc,OPAQUE);
    SelectObject(hdc,save_font);
    return;
   }


  HDC     hdcMem    = CreateCompatibleDC(hdc) ;
  HBITMAP save_hbm  = (HBITMAP)SelectObject(hdcMem,CreateCompatibleBitmap(hdc,w,h));

  save_font = (HFONT)SelectObject(hdcMem,wp->font);

  SetBkMode(hdcMem,OPAQUE);
  SetBkColor(hdcMem,RGB(0,0,0));
  SetTextColor(hdcMem,RGB(255,255,255));
  TextOut(hdcMem,0,0,s,l);

  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,CreateSolidBrush(col));

  if (wp->MODE == xor_mode)
     BitBlt(hdc,x,y,w,h,hdcMem,0,0,0x00A60706);
  else
     BitBlt(hdc,x,y,w,h,hdcMem,0,0,0x00E20746);

  DeleteObject(SelectObject(hdcMem,save_hbm));
  SelectObject(hdcMem,save_font);
  DeleteDC(hdcMem);

  DeleteObject(SelectObject(hdc,save_brush));
}

  
void x_text(Window win, int x, int y, const char* s)
{ x_text(win, x, y, s, (int)strlen(s)); }



void x_ctext(Window win, int x, int y, const char* s)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  HFONT save_font = (HFONT)SelectObject(hdc,wp->font);
  SIZE sz;
  GetTextExtentPoint(hdc,s,(int)strlen(s),&sz);
  int xt = int(x-sz.cx/2.0 + 0.5);
  int yt = int(y-sz.cy/2.0 + 0.5);
  x_text(win,xt,yt,s);
  SelectObject(hdc,save_font);

/*
 int w = x_text_width(win,s);
 int h = x_text_height(win,s);
 x_text(win,x-w/2,y-h/2,s);
*/
}


void x_ctext_underline(Window win, int x, int y, const char* s, int l, int r)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  HFONT save_font = (HFONT)SelectObject(hdc,wp->font);
  SIZE sz;
  GetTextExtentPoint(hdc,s,(int)strlen(s),&sz);
  SelectObject(hdc,save_font);
  x_text_underline(win,x-sz.cx/2,y-sz.cy/2/*+sz.cy%2*/,s,l,r);
}



//------------------------------------------------------------------------------
// pixrects and bitmaps 
//------------------------------------------------------------------------------

static char rev_byte(char c)
{ char c1 = 0;
  if (c & 0x01) c1 |= 0x80;
  if (c & 0x02) c1 |= 0x40;
  if (c & 0x04) c1 |= 0x20;
  if (c & 0x08) c1 |= 0x10;
  if (c & 0x10) c1 |= 0x08;
  if (c & 0x20) c1 |= 0x04;
  if (c & 0x40) c1 |= 0x02;
  if (c & 0x80) c1 |= 0x01;
  return c1;
 }


char* x_create_bitmap(Window win, int w, int h, unsigned char* data, int f)
{ 
  int   bw0 = (w+7)/8;
  int   bw1 = 2*((bw0+1)/2);
  char* buf = new char[bw1*h];
  char* p   = buf;
  char* q   = (char*)data;

  for(int i=0; i<h; i++)
    for(int j=0; j<bw1; j++) 
       *p++ = (j >= bw0) ? 0 : rev_byte(*q++);

  // scale  factor f
  if (f > 1)
  { int bw2 = f*bw1;
    int h2 = f*h;
    char* buf2 = new char[bw2*h2];
  
    for(int i=0; i<h2; i++)
      for(int j=0; j<bw2; j++) 
      { char x = buf[(i/f)*bw1 + j/f]; // source byte
        int y = 0;
        for(int k = 0; k<8; k++) {
          if (x & (1<<k)) y |=  ((1<<f)-1) << f*k;
        }
        char* z = (char*)&y;
        buf2[i*bw2 + j] = z[f-j%f-1];
       }

    delete[] buf;
    buf = buf2;
  }


  BITMAP bm;
  bm.bmType = 0;
  bm.bmWidth = f*w;
  bm.bmHeight = f*h;
  bm.bmWidthBytes = f*bw1;
  bm.bmPlanes = 1;
  bm.bmBitsPixel = 1;
  bm.bmBits = buf;

  x_pixrect* im = new x_pixrect;
  im->w = f*w;
  im->h = f*h;
  im->map = CreateBitmapIndirect(&bm);

  delete[] buf;

  return (char*)im;
}



unsigned char* x_pixrect_to_bgra(int w, char* prect)
{ 
  x_pixrect* im = (x_pixrect*)prect;

  HDC hdc = CreateDC("DISPLAY",NULL,NULL,NULL); 
  SetGraphicsMode(hdc,GM_ADVANCED);

  HDC hdcMem  = CreateCompatibleDC(hdc) ;
  SelectObject (hdcMem,im->map);

  int width = im->w;
  int height = im->h;
  int sz = 4 + 4*width*height;

  unsigned char* buf = new unsigned char[sz];
  unsigned char* p = buf;

  for(int y=0; y<height; y++)
  { for(int x=0; x<width; x++)
    { COLORREF col = GetPixel(hdcMem,x,y); 
      *p++ = (unsigned char)GetBValue(col);
      *p++ = (unsigned char)GetGValue(col);
      *p++ = (unsigned char)GetRValue(col);
      *p++ = 255;
     }
   }

  DeleteDC(hdcMem);
  DeleteDC(hdc);

  return buf;
}


static unsigned int interpolate(float f, unsigned int clr1, unsigned int clr2)
{ unsigned int result = 0;
  for(int i=0; i<4; i++)
  { unsigned int x = (clr1 & 0xff);
    unsigned int y = (clr2 & 0xff);
    unsigned int z = (unsigned int)((1-f)*x + f*y);
    result += z << 8*i;
    clr1 >>= 8;
    clr2 >>= 8;
   }
  return result;
}
    

static void scale_bgra(float f, unsigned int*& bgra, int& width, int& height)
{
    int width2 = int(f*width);
    int height2 = int(f*height);
    unsigned int* bgra2 = new unsigned int[width2*height2];

    for(int y=0; y<height2; y++)
    { for(int x=0; x<width2; x++)
      { // bilineare interpolation 

        float xf = x/f;
        float yf = y/f;

        int x1 = int(xf);
        int y1 = int(yf);

        xf -= x1;
        yf -= y1;

        int i = y1*width+x1;

        unsigned int C11 = bgra[i];
        unsigned int C12 = (x1 < width-1)  ? bgra[i+1] : C11;
        unsigned int C21 = (y1 < height-1) ? bgra[i+width] : C11;
        unsigned int C22 = (y1 < height-1 && x1 < width-1) ? bgra[i+width+1] : C11;

        unsigned int R1 = interpolate(xf,C11,C12);
        unsigned int R2 = interpolate(xf,C21,C22);
        bgra2[y*width2+x] = interpolate(yf,R1,R2);
       }
     }

    delete[] bgra;

    bgra = bgra2;
    width = width2;
    height = height2;
}

static void resize_bgra(unsigned char* bgra1, int width1, int height1,
                        unsigned char* bgra2, int width2, int height2)
{

  unsigned int* p = (unsigned int*)bgra1;
  unsigned int* q = (unsigned int*)bgra2;


  double fx = double(width2)/width1;
  double fy = double(height2)/height1;

  for(int y=0; y<height2; y++)
  { for(int x=0; x<width2; x++)
    { // bilineare interpolation 
      double xf = x/fx;
      double yf = y/fy;
      int x1 = int(xf);
      int y1 = int(yf);

      xf -= x1;
      yf -= y1;

      int i = y1*width1+x1;

      unsigned int C11 = p[i];

      unsigned int C12 = C11;
      if (x1 < width1-1)  C12 = p[i+1];

      unsigned int C21 = C11;
      unsigned int C22 = C12;

      if (y1 < height1-1) 
      { C21 = p[i+width1];
        if (x1 < width1-1) C22 = p[i+width1+1];
       }

      unsigned int R1 = interpolate(float(xf),C11,C12);
      unsigned int R2 = interpolate(float(xf),C21,C22);
      q[y*width2+x] = interpolate(float(yf),R1,R2);
     }
   }
}




char* x_create_pixrect_from_bgra(Window win, int width, int height, 
                                                        unsigned char* data,
                                                        float scale_f)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

/*
  printf("pixrect_from_bgra: width = %d  height = %d\n",width, height);
  printf("hwnd = %d\n",hwnd);
  fflush(stdout);
*/

  unsigned char* buf = 0;
  int len = 0;

  if (scale_f == 1)
  { len = 4*width*height;
    buf = new unsigned char[len];
    memcpy(buf,data,len);
   }
  else
  { int width2 = int(0.5 + scale_f*width);
    int height2 = int(0.5 + scale_f*height);
    len = 4*width2*height2;
    buf = new unsigned char[len];
    resize_bgra(data,width,height,buf,width2,height2);
    width = width2;
    height = height2;
   }

  for(int i=0; i<len; i+=4) 
  { unsigned b = buf[i+0];
    unsigned g = buf[i+1];
    unsigned r = buf[i+2];
    unsigned a = buf[i+3];
/*
    if (rgba) std::swap(b,r);
*/
    // pre-multiply alpha values

    buf[i+0] = (a*b)/0xff;
    buf[i+1] = (a*g)/0xff;
    buf[i+2] = (a*r)/0xff;
  }

//HDC hdc = wp->hdc;
  HDC hdc = CreateDC("DISPLAY",NULL,NULL,NULL); 
  SetGraphicsMode(hdc,GM_ADVANCED);

  x_pixrect* im = new x_pixrect;
  im->w = width;
  im->h = height;
  im->alpha = 255; // use alpha channel
  im->map = CreateCompatibleBitmap(hdc,im->w,im->h) ;

  BITMAPINFO bm_info;
  BITMAPINFOHEADER* pm = &bm_info.bmiHeader;
  ZeroMemory(pm,sizeof(BITMAPINFOHEADER));
  pm->biSize = sizeof(BITMAPINFOHEADER);
  pm->biWidth = width;
  pm->biHeight = -height;
  pm->biPlanes = 1;
  pm->biBitCount = 32;
  pm->biCompression = BI_RGB;  // BI_PNG, BI_JPEG

  int result =  SetDIBits(hdc,im->map, 0,        // uStartScan (UINT)
                                       height,   // cscanLines (UINT)
                                       buf,      // lpvBits (void*)
                                       &bm_info, // BITMAPINFO*
                                       DIB_RGB_COLORS);


  if (result == 0) 
  { printf("bgra error:  w = %d  h = %d\n",width, height);
    DeleteObject(im->map);
    delete im;
    im = 0;
   }

  DeleteDC(hdc);

  delete[] buf;
  return (char*)im;
}


/*
char* x_create_pixrect_from_rgb(Window win, int width, int height,
                                                       unsigned char* data)
{ 
  ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  size_t len = (size_t)3*width*height;
  unsigned char* buf = new unsigned char[len];
  memcpy(buf,data,len);

  for(size_t i=0; i<len; i+=3) 
  { unsigned b = buf[i+0];
  //unsigned g = buf[i+1];
    unsigned r = buf[i+2];
    buf[i+0] = r;
    buf[i+2] = b;
   }

  HDC hdc = CreateDC("DISPLAY",NULL,NULL,NULL); 
  SetGraphicsMode(hdc,GM_ADVANCED);

  x_pixrect* im = new x_pixrect;
  im->w = width;
  im->h = height;
  im->alpha = -1; 
  im->map = CreateCompatibleBitmap(hdc,im->w,im->h) ;

  BITMAPINFO bm_info;
  BITMAPINFOHEADER* pm = &bm_info.bmiHeader;
  ZeroMemory(pm,sizeof(BITMAPINFOHEADER));
  pm->biSize = sizeof(BITMAPINFOHEADER);
  pm->biWidth = width;
  pm->biHeight = -height;
  pm->biPlanes = 1;
  pm->biBitCount = 24;
  pm->biCompression = BI_RGB;  // BI_PNG, BI_JPEG

  int result = SetDIBits(hdc,im->map, 0,        // uStartScan (UINT)
                                      height,   // cscanLines (UINT)
                                      buf,      // lpvBits (void*) 
                                      &bm_info, // BITMAPINFO*
                                      DIB_RGB_COLORS);


 if (result == 0) 
 { printf("rgb error:  w = %d  h = %d\n",width, height);
   show_last_error("create_pixrect_from_rgb: ");
   DeleteObject(im->map);
   delete im;
   im = 0;
  }

  DeleteDC(hdc);

  delete[] buf;

  return (char*)im;
}
*/



void x_text_to_clipboard(Window win, const char* txt)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  OpenClipboard(hwnd);
  EmptyClipboard();

  HGLOBAL clipboard_data = GlobalAlloc(GMEM_DDESHARE,strlen(txt)+1);
  char* ptr = (char*)GlobalLock(clipboard_data);
  strcpy(ptr,txt);
  GlobalUnlock(clipboard_data);

  SetClipboardData(CF_TEXT,clipboard_data);
  CloseClipboard();
 }


char* x_text_from_clipboard(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  OpenClipboard(hwnd);
  char* txt = (char*)GetClipboardData(CF_TEXT);
  CloseClipboard();
  return txt;
 }



void x_pixrect_to_clipboard(Window win, char* rect)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  x_pixrect* im = (x_pixrect*)rect;
  OpenClipboard(hwnd);
  EmptyClipboard();
  SetClipboardData(CF_BITMAP,im->map);
  im->map = 0;
  CloseClipboard();
 }


char* x_pixrect_from_clipboard(Window win)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  OpenClipboard(hwnd);

  //UINT fmt = EnumClipboardFormats(0);
  //while (fmt != 0 && fmt != CF_BITMAP) EnumClipboardFormats(fmt);
  //if (fmt == 0) return 0;

  HBITMAP hbm = (HBITMAP)GetClipboardData(CF_BITMAP);

  if (hbm == NULL) 
  { CloseClipboard();
    return 0;
   }

  //BITMAP bm;
  //GetObject(hbm,sizeof(BITMAP),(LPSTR)&bm);
  //hbm = CreateBitmapIndirect(&bm);

  SIZE sz;
  GetBitmapDimensionEx(hbm,&sz);
  x_pixrect* im = new x_pixrect;
  im->map = hbm;
  im->w = sz.cx;
  im->h = sz.cy;

  CloseClipboard();
  return (char*)im;
 }


char* x_create_pixrect_from_bits(Window win, int w, int h, unsigned char* data,
                                                           int fc, int bc)
{ return x_create_bitmap(win,w,h,data,1); }


char* x_create_pixrect(Window win, int x1, int y1, int x2, int y2)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  HDC hdcMem  = CreateCompatibleDC (hdc) ;
  x_pixrect* im = new x_pixrect;
  if (x1 > x2) SWAP(x1,x2);
  if (y1 > y2) SWAP(y1,y2);
  im->w = x2-x1+1;
  im->h = y2-y1+1;
  im->map = CreateCompatibleBitmap (hdc,im->w,im->h) ;
  SelectObject (hdcMem,im->map);
  BitBlt (hdcMem,0,0,im->w,im->h,hdc,x1,y1,SRCCOPY) ;
  DeleteDC (hdcMem) ;
  im->alpha = -1; // no alpha
  return (char*)im;
}


void x_insert_pixrect(Window win, int x,int y,int w,int h, char* rect)
{ // (x,y): pos of lower left corner !
  ms_win* wp = wlist[win];
  x_pixrect* im = (x_pixrect*)rect;
  y = y - h + 1;
  im->blend(wp->hdc,x,y,w,h, 0,0,im->w,im->h);
 }


void x_insert_pixrect(Window win, int x, int y, char* rect)
{ // (x,y) pos of lower left corner !
  ms_win* wp = wlist[win];
  x_pixrect* im = (x_pixrect*)rect;
  y = y - im->h + 1;
  im->blend(wp->hdc,x,y,im->w,im->h, 0,0,im->w,im->h);
}

void x_insert_pixrect(Window win, char* rect)
{ // place into upper left corner !
  ms_win* wp = wlist[win];
  x_pixrect* im = (x_pixrect*)rect;
/*
  x_set_color(win,black);
  x_rect(win,0,0,im->w,im->h);
*/
  im->blend(wp->hdc,0,0,im->w,im->h,  0,0,im->w,im->h);

}


void x_insert_pixrect(Window win, int x, int y, 
                      char* rect, int src_x, int src_y, int w, int h)
{ // (x,y) pos of lower left corner !
  ms_win* wp = wlist[win];
  x_pixrect* im = (x_pixrect*)rect;
  y = y - h + 1;
  im->blend(wp->hdc,x,y,w,h, src_x,src_y,w,h);
}




void x_insert_bitmap(Window win, int x, int y, char* rect)
{ // (x,y) pos of lower left corner !
  ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  HDC hdcMem  = CreateCompatibleDC (hdc) ;
  x_pixrect* im = (x_pixrect*)rect;
  SelectObject (hdcMem,im->map);

  COLORREF col = wp->pen_data.lopnColor;
  HBRUSH  save_brush = (HBRUSH)SelectObject(hdc,CreateSolidBrush(col));

  if (wp->MODE == xor_mode)
     BitBlt (hdc,x,y-im->h+1,im->w,im->h,hdcMem,0,0,0x00A60706);
  else
     BitBlt (hdc,x,y-im->h+1,im->w,im->h,hdcMem,0,0,0x00E20746);

  DeleteDC (hdcMem);

  DeleteObject(SelectObject(hdc,save_brush));
}



void x_delete_bitmap(char* rect)
{ if (rect)
  { x_pixrect* im = (x_pixrect*)rect;
    DeleteObject(im->map);
    delete im;
   }
 }

void x_delete_pixrect(char* rect) { x_delete_bitmap(rect); }



/*
void x_copy_pixrect(Window win, int x1, int y1, int x2, int y2, int x, int y)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  HDC hdcMem  = CreateCompatibleDC (hdc) ;
  int w = x2-x1+1;
  int h = y2-y1+1;
  HBITMAP map = CreateCompatibleBitmap (hdc,w,h) ;
  SelectObject (hdcMem,map);
  BitBlt (hdcMem,0,0,w,h,hdc,x1,y1,SRCCOPY) ;
  BitBlt (hdc,x,y,w,h,hdcMem,0,0,MAKEROP4(SRCCOPY,0)) ;
  DeleteDC (hdcMem) ;
}
*/


void x_get_pixrect_size(char* rect, int& w, int& h)
{ if (rect == 0)
    w = h = 0;
  else
  { x_pixrect* im = (x_pixrect*)rect;
    w = im->w;
    h = im->h;
   }
}

void x_get_pixrect_anchor(char* rect, int& x, int& y)
{ if (rect)
  { x_pixrect* im = (x_pixrect*)rect;
    x = im->anchor_x;
    y = im->anchor_y;
   }
}


// fonts and text

static HFONT x_create_font(const char* fname)
{
  //cout << "x_create_font: |" << fname << "|" << endl;

  LOGFONT lf;

  ZeroMemory(&lf,sizeof(LOGFONT));

  int i=0;
  int h=0;

  if (isupper(fname[0]) && isdigit(fname[1])) {

    switch (fname[0]) {

    case 'T': //lf.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
              //strcpy(lf.lfFaceName,"Trebuchet MS");
              strcpy(lf.lfFaceName,"Droid Sans");
              //strcpy(lf.lfFaceName,"Arial");
              lf.lfWeight = FW_NORMAL;
              break;
  
    case 'I': //lf.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
              //strcpy(lf.lfFaceName,"Trebuchet MS");
              //strcpy(lf.lfFaceName,"Arial");
              strcpy(lf.lfFaceName,"Droid Sans");
              lf.lfWeight = FW_NORMAL;
              lf.lfItalic = 1;
              break;
  
    case 'B': //lf.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
              strcpy(lf.lfFaceName,"Arial");
              lf.lfWeight = FW_BOLD;
              break;
  
    case 'F': //lf.lfPitchAndFamily = FF_MODERN | FIXED_PITCH;
              //strcpy(lf.lfFaceName,"Droid Sans Mono");
              //strcpy(lf.lfFaceName,"Consolas");
              strcpy(lf.lfFaceName,"Lucida Console");
              lf.lfWeight = FW_NORMAL;
              break;

    case 'A':
              lf.lfPitchAndFamily = 82;
              lf.lfWeight = 400;
              lf.lfCharSet = 1;
              lf.lfOutPrecision = 3;
              lf.lfClipPrecision = 2;
              lf.lfQuality = 1;
              strcpy(lf.lfFaceName,"Empire BT");
              break;

    default : 
              break;
    }

    i = 1;
  }
  else
  { i=0;
    while (fname[i] && fname[i] != '#' && i<LF_FACESIZE-1) i++;
    strncpy(lf.lfFaceName,fname,i+1);
    lf.lfFaceName[i] = '\0';
   }

  while (fname[i] && isdigit(fname[i])) h = 10*h + fname[i++] - '0';
  if (fname[i] != 0) return NULL; // bad font name
  if (h > 500) h = 500;

  lf.lfHeight = h;

  return CreateFontIndirect(&lf);
}



static void x_set_font(Window win, HFONT hf, const char* fname)
{ ms_win* wp = wlist[win];

  if (hf == NULL || wp->font == hf) return;

  wp->font = hf;
  strcpy(wp->font_name,fname);

/*
  HDC hdc = wp->hdc;
  if (hdc != NULL)
  { HFONT save_font = (HFONT)SelectObject(hdc,hf);
    char face_name[256];
    GetTextFace(hdc,256,face_name);
    cout << "FONT: " << face_name << endl;
  }
*/

}



int x_set_font(Window win, const char* fname)
{ 
  ms_win* wp = wlist[win];

  if (strcmp(fname,wp->font_name) != 0)
  { HFONT hf = x_create_font(fname);
    if (hf == NULL) return 0;
    x_set_font(win,hf,fname);
   }
  return 1;
}


void x_set_text_font(Window win)    { x_set_font(win,text_font,"");   }
void x_set_italic_font(Window win)  { x_set_font(win,italic_font,""); }
void x_set_bold_font(Window win)    { x_set_font(win,bold_font,"");   }
void x_set_fixed_font(Window win)   { x_set_font(win,fixed_font,"");  }
void x_set_button_font(Window win)  { x_set_font(win,button_font,""); }


void x_restore_font(Window win)
{ ms_win* wp = wlist[win];
  x_set_font(win,wp->tmp_font,wp->tmp_font_name);
}

void x_save_font(Window win)
{ ms_win* wp = wlist[win];
/*
  if (wp->tmp_font != NULL) DeleteObject(wp->tmp_font);
*/
  wp->tmp_font = wp->font;
  strcpy(wp->tmp_font_name,wp->font_name);
}


static void x_text_dimensions(Window win,const char* s, int len, SIZE& sz) 
{ ms_win* wp = wlist[win];
  HDC  hdc  = wp->hdc;

  if (hdc == 0)
  { // win still closed
    x_open_display();
    hdc = CreateDC("DISPLAY",NULL,NULL,NULL); 
    SetGraphicsMode(hdc,GM_ADVANCED);
   }

  HFONT save_font = (HFONT)SelectObject(hdc,wp->font);
  GetTextExtentPoint(hdc,s,len,&sz);
  SelectObject(hdc,save_font);

  if (hdc != wp->hdc) DeleteDC(hdc);
}


int x_text_width(Window win,const char* s, int len)
{ if (len == 0) return 0;
  if (len > (int)strlen(s)) len = (int)strlen(s);
  SIZE sz;
  x_text_dimensions(win,s,len,sz);
  return sz.cx;
 }


int x_text_height(Window win,const char* s) 
{ ms_win* wp = wlist[win];
  SIZE sz;
//x_text_dimensions(win,s,(int)strlen(s),sz);
  x_text_dimensions(win,"H",1,sz);
  return int(0.5 + sz.cy);
 }


int x_text_width(Window win,const char* s)
{ return x_text_width(win,s,(int)strlen(s)); }



/* drawing parameters */

int x_set_bg_color(Window win, int col) 
{ ms_win* wp = wlist[win];
  int save = rotate_rgb(wp->BG_COLOR);
  wp->BG_COLOR = rotate_rgb(col);
  if (wp->BG_PIXMAP == 0)
  { if (wp->BG_BRUSH) DeleteObject(wp->BG_BRUSH);
    wp->BG_BRUSH = CreateSolidBrush(wp->BG_COLOR);
   }
  return save;
}

char* x_set_bg_pixmap(Window win, char* pm) 
{ ms_win* wp = wlist[win];
  char* save = wp->BG_PIXMAP;
  wp->BG_PIXMAP = pm; 

  if (wp->BG_BRUSH) DeleteObject(wp->BG_BRUSH);

  if (pm)
    wp->BG_BRUSH = CreatePatternBrush(((x_pixrect*)pm)->map);
  else
    wp->BG_BRUSH = CreateSolidBrush(wp->BG_COLOR);

  return save; 
}


void x_set_stipple(Window win, char* bits, int c) 
{ ms_win* wp = wlist[win];

  if (wp->stip_bm) DeleteObject(wp->stip_bm);
  wp->stip_bm = NULL;

  if (bits)
  { BITMAP bm;
    bm.bmType = 0;
    bm.bmWidth = 16;
    bm.bmHeight = 16;
    bm.bmWidthBytes = 2;
    bm.bmPlanes = 1;
    bm.bmBitsPixel = 1;
    bm.bmBits = bits;
    wp->stip_bm = CreateBitmapIndirect(&bm);
    wp->stip_bgcol = c;
   }
}

int x_set_color(Window win, int col)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  int old_col = rotate_rgb(wp->COLOR);
  wp->COLOR = rotate_rgb(col);
  if (hwnd) 
  { HDC hdc = wp->hdc;
    //wp->pen_data.lopnColor = GetNearestColor(hdc,wp->COLOR);
    wp->pen_data.lopnColor = wp->COLOR;
    DeleteObject(SelectObject(hdc,CreatePenIndirect(&wp->pen_data)));
    SetTextColor(hdc,wp->pen_data.lopnColor);
   }
  return old_col;
 }


drawing_mode x_set_mode(Window win, drawing_mode mod) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  drawing_mode old_mod = wp->MODE;
  if (mod == old_mod) return old_mod;
  wp->MODE = mod;
  if (hwnd)
  { HDC hdc = wp->hdc;
    switch (mod) {
    case src_mode: SetROP2(hdc, R2_COPYPEN);
                   break;
    case and_mode: SetROP2(hdc, R2_MASKPEN);
                   break;
    case or_mode:  SetROP2(hdc, R2_MERGEPEN);
                   break;
    case xor_mode: SetROP2(hdc, R2_NOTXORPEN);
                   break;
    case diff_mode:
                   break;
    }
   }

  return old_mod;
 }


int x_set_line_width(Window win, int lw) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  int old_lw = wp->LWIDTH;
  if (lw == old_lw) return old_lw;
  wp->LWIDTH = lw;
  if (hwnd)
  { HDC hdc = wp->hdc;
    wp->pen_data.lopnWidth.x = lw;
    wp->pen_data.lopnWidth.y = lw;
    DeleteObject(SelectObject(hdc,CreatePenIndirect(&wp->pen_data)));
   }
  return old_lw;
}


line_style x_set_line_style(Window win, line_style ls)
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  line_style old_ls = wp->LSTYLE;
  if (ls == old_ls) return old_ls;
  wp->LSTYLE = ls;
  if (hwnd)
  { HDC hdc = wp->hdc;
    switch (ls) {
    case solid:  wp->pen_data.lopnStyle = PS_SOLID;
                 break;
    case dashed: wp->pen_data.lopnStyle = PS_DASH;
                 break;
    case dotted: wp->pen_data.lopnStyle = PS_DOT;
                 break;
    case dashed_dotted: 
                 wp->pen_data.lopnStyle = PS_DASHDOT;
                 break;
    }
    DeleteObject(SelectObject(hdc,CreatePenIndirect(&wp->pen_data)));
   }
  return old_ls;
 }


text_mode x_set_text_mode(Window win, text_mode tm) 
{ ms_win* wp = wlist[win];
  text_mode save = wp->TMODE;
  wp->TMODE = tm;  
  return save;
 }


int x_set_join_style(Window win, int js) 
{ ms_win* wp = wlist[win];
  int save = wp->JSTYLE;
  wp->JSTYLE = js;  
  return save;
 }



int x_get_color(Window win)      
{ ms_win* wp = wlist[win];
  return rotate_rgb(wp->COLOR);
}

drawing_mode x_get_mode(Window win)       
{ ms_win* wp = wlist[win];
  return wp->MODE; 
}

int x_get_line_width(Window win) 
{ ms_win* wp = wlist[win];
  return wp->LWIDTH;
}

line_style x_get_line_style(Window win) 
{ ms_win* wp = wlist[win];
  return wp->LSTYLE;
}

text_mode x_get_text_mode(Window win)  
{ ms_win* wp = wlist[win];
  return wp->TMODE;
}

int x_get_cursor(Window win)     
{ ms_win* wp = wlist[win];
  return wp->cursor;
}

int x_get_resizing(Window win)     
{ ms_win* wp = wlist[win];
  return wp->resizing;
}


int x_get_border_color(Window)
{ // not implemented
  return black; 
}

int x_get_border_width(Window win) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;
  if (GetParent(hwnd) == NULL)
    //return GetSystemMetrics(SM_CYFRAME); 
    return GetSystemMetrics(SM_CXFRAME);
  else
    return 1;
}





void x_set_read_gc(Window win)
{ ms_win* wp = wlist[win];
  wp->save_mo = x_set_mode(win,xor_mode);
  wp->save_ls = x_set_line_style(win,solid);
  wp->save_lw = x_set_line_width(win,1);
  x_set_color(win,black);
 }


void x_reset_gc(Window win)
{ ms_win* wp = wlist[win];
  x_set_mode(win,wp->save_mo);
  x_set_line_style(win,wp->save_ls);
  x_set_line_width(win,wp->save_lw);
 }



/* events */

static int wait_next_event(Window& win, int& x, int& y, int& val1,
                                                        int& val2, 
                                                        unsigned long& t)
{
  // blocking

  if (putback)
  { int w = last_event.win;
    if (wlist[w] == 0) putback = false;
   }

  if (putback) 
   { cur_event = last_event;
     putback = 0;
    }
  else
  { MSG msg;
    cur_event.kind = no_event;

    int res = GetMessage (&msg,NULL,0,0);

    if (res > 0)
    { TranslateMessage(&msg);
      DispatchMessage(&msg);
     }

    if (cur_event.kind != exposure_event)
        cur_event.t = (unsigned long)msg.time;

    if (cur_event.kind != no_event) last_event = cur_event;
   }

/*
if (cur_event.kind != no_event && cur_event.kind != timer_event) {
printf("x_get_next_event: win = %d  kind = %s\n", cur_event.win,
                                                  event_name[cur_event.kind]);
fflush(stdout);
}
*/

  if (button_event.kind != 0)
  { cur_event = button_event;
    button_event.kind = 0;
   }

  win = cur_event.win;
  x    = cur_event.x;
  y    = cur_event.y;
  val1 = cur_event.val1;
  val2 = cur_event.val2;
  t    = cur_event.t;


  if ((wlist[win] != 0))
    return cur_event.kind;
  else
    return no_event;
}


int x_get_next_event(Window& win, int& x, int& y, int& val1, int& val2, 
                                                             unsigned long& t,
                                                             int msec)
{ 

  if (msec < 0)
  { //non-blocking (check)
    MSG msg;
    if (putback || PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
       return wait_next_event(win,x,y,val1,val2,t); 
    else
       return no_event;
   }


  if (msec == 0)
  { // blocking
    return wait_next_event(win,x,y,val1,val2,t);
   }


  // get next event with timeout

  MSG msg;
  if (putback || PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
  { int k = x_get_next_event(win,x,y,val1,val2,t); 
    if (k != no_event) return k;
   }

  //HWND hwnd = wlist[*win]->hwnd;
  HWND hwnd = wlist[1]->hwnd;

  SetTimer(hwnd,2,msec,NULL);

  int k = no_event;
  while (k == no_event) k = wait_next_event(win,x,y,val1,val2,t);

  KillTimer(hwnd,2);

  if (k == timer_event && val1 == 2) 
   { win = 0;
     k = no_event;
    }

  return k;
}


void x_put_back_event(void) { putback = 1; }


void x_set_border_width(Window win, int b) 
{ ms_win* wp = wlist[win];
  wp->border_w = b;
 }

void x_set_pixrect_anchor(char* rect, int x, int y) 
{ x_pixrect* im = (x_pixrect*)rect;
  im->anchor_x = x;
  im->anchor_y = y;
}

void x_set_pixrect_alpha(char* rect, int a) 
{ x_pixrect* im = (x_pixrect*)rect;
  im->alpha = a;
}

void x_set_pixrect_phi(char* rect, float phi) 
{ x_pixrect* im = (x_pixrect*)rect;
  im->phi = phi;
}

void x_set_alpha(Window win, int a) 
{ ms_win* wp = wlist[win];
  HWND hwnd = wp->hwnd;

  if (a < 0) a = 0;
  if (a > 255) a = 255;

  if (hwnd == 0) // not created
  { wp->alpha = a;
    return;
  }

  if (wp->alpha == a) return;

  if (a == 255)
    SetWindowLong(hwnd,GWL_EXSTYLE, 
                       GetWindowLong(hwnd,GWL_EXSTYLE) &~ WS_EX_LAYERED);
  else
   { SetWindowLong(hwnd,GWL_EXSTYLE,
                        GetWindowLong(hwnd,GWL_EXSTYLE) | WS_EX_LAYERED);
     SetLayeredWindowAttributes(hwnd,0, a, LWA_ALPHA);
   }

   RedrawWindow(hwnd,NULL,NULL,
                     RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
   wp->alpha = a;
}



//------------------------------------------------------------------------------
// not implemented
//------------------------------------------------------------------------------

void x_set_border_color(Window, int) 
{ /* not implemented */ }

void x_set_bg_origin(Window, int, int)
{ /* not implemented */ }

void x_fill_arc(Window, int, int, int, int, double, double)
{ /* not implemented */ }


void x_set_icon_pixmap(Window win, char* prect)
{ ms_win* wp = wlist[win];

  x_pixrect* im = (x_pixrect*)prect;

  HBITMAP mask = CreateCompatibleBitmap(wp->hdc,im->w,im->h);

  ICONINFO iInfo;
  ZeroMemory(&iInfo,sizeof(iInfo));
  iInfo.fIcon = TRUE;
  iInfo.xHotspot = im->w/2;
  iInfo.yHotspot = im->h/2;
  iInfo.hbmMask = mask;
  iInfo.hbmColor  = im->map;

  HICON hIcon = CreateIconIndirect(&iInfo);

/*
  if (hIcon == 0) {
    show_last_error("CreateIconIndirect: ");
    return;
  }
*/

  SendMessage(wp->hwnd,WM_SETICON, ICON_BIG,(LPARAM)hIcon);    
  SendMessage(wp->hwnd,WM_SETICON, ICON_SMALL,(LPARAM)hIcon);    
 }



void x_set_icon_label(Window, const char*) 
{ /* not implemented */ }

void x_minimize_window(Window win) 
{ ms_win* wp = wlist[win];
  ShowWindow(wp->hwnd, SW_MINIMIZE);
}

void x_maximize_window(Window win) 
{ ms_win* wp = wlist[win];
  ShowWindow(wp->hwnd, SW_MAXIMIZE);
}

void x_flush_display(void)
{ /* not implemented */ }


void x_send_text(const char*)
{ /* not implemented */ }


char* x_create_bitmap(Window win, int x1, int y1, int x2, int y2)
{ /* not implemented */
  return 0; 
 }


/*
char* x_pixrect_to_bitmap(Window win, char* rect)
{ ms_win* wp = wlist[win];
  HDC  hdc = wp->hdc;

  x_pixrect* im = (x_pixrect*)rect;

  int w = im->w;
  int h = im->h;

  HDC hdcMem_src = CreateCompatibleDC(hdc);
  SelectObject(hdcMem_src,im->map);

  unsigned char* data = new unsigned char[h*(w/8+1)];
  char* bitmap = x_create_bitmap(win,w,h,data,1);

  HDC hdcMem_tgt = CreateCompatibleDC(hdc);
  SelectObject(hdcMem_tgt,((x_pixrect*)bitmap)->map);

  BitBlt(hdcMem_tgt,0,0,w,h,hdcMem_src,0,0,SRCCOPY) ;

  DeleteDC(hdcMem_src);
  DeleteDC(hdcMem_tgt);

  return bitmap;
}
*/


/*
static int x_load_font(Window win, const char* fname, HFONT& hfont)   
{ ms_win* wp = wlist[win];
  HFONT hf = x_create_font(fname);
  if (hf == NULL) return 0;
  if (wp->font == hfont) wp->font = hf;
  DeleteObject(hfont);
  hfont = hf;
  return 1;
}

int x_load_text_font(Window win, const char* fn)   
{ return x_load_font(win,fn,text_font);  }

int x_load_italic_font(Window win, const char* fn) 
{ return x_load_font(win,fn,italic_font);}

int x_load_bold_font(Window win, const char* fn)   
{ return x_load_font(win,fn,bold_font);  }

int x_load_fixed_font(Window win, const char* fn)  
{ return x_load_font(win,fn,fixed_font); }

int x_load_button_font(Window win, const char* fn) 
{ return x_load_font(win,fn,button_font);}
*/



void x_open_metafile(Window win, const char* fname)
{ ms_win* wp = wlist[win];
  x_close_metafile(win);
  wp->hdc2 = wp->hdc;
  if (wp->hmf) 
  { DeleteEnhMetaFile(wp->hmf);
    wp->hmf = NULL;
   }

printf("open metafile: %s\n",fname);

/*
  RECT rect;
  GetClientRect(wp->hwnd,&rect);
  //SetRect(&rect,0,0,1000,1000);
  wp->hdc = CreateEnhMetaFile(NULL,fname,&rect,"LEDA\0window\0\0");
  assert(hdc_mf);
*/

  wp->hdc = CreateEnhMetaFile(NULL,fname,NULL,"LEDA\0window\0\0");
}

void x_close_metafile(Window win)
{ ms_win* wp = wlist[win];
  if (wp->hdc2 == NULL) return;
  wp->hmf = CloseEnhMetaFile(wp->hdc);

if (wp->hmf == NULL) {
LPVOID lpMsgBuf;

FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
);
MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
LocalFree( lpMsgBuf );
}
 
  wp->hdc = wp->hdc2;
  wp->hdc2= NULL;
 }

void x_metafile_to_clipboard(Window win)
{ ms_win* wp = wlist[win];
  if (wp->hmf == NULL) return;
  OpenClipboard(wp->hwnd);
  EmptyClipboard();
  SetClipboardData(CF_ENHMETAFILE,wp->hmf);
  CloseClipboard();
 }

  

void x_load_metafile(Window win, int x0, int y0, int x1, int y1, 
                                                         const char* fname)
{ ms_win* wp = wlist[win];
  HDC hdc = wp->hdc;
  RECT rect;
  if (x0 > x1) SWAP(x0,x1);
  if (y0 > y1) SWAP(y0,y1);
  SetRect(&rect,x0,y0,x1,y1);
  HENHMETAFILE hmf = GetEnhMetaFile(fname);
  if (hmf)
  { PlayEnhMetaFile(hdc,hmf,&rect);
    DeleteEnhMetaFile(hmf);
   }
}


void x_set_special_event_handler(Window win, 
                                 special_event_func fun, 
                                 unsigned long data)
{ wlist[win]->special_event_handler = fun;
  wlist[win]->special_event_data = data; }

static bool is_file(const char* name)
{ DWORD att = GetFileAttributes(name);
  if (att == 0xFFFFFFFF) return false;
  return !(att & FILE_ATTRIBUTE_DIRECTORY); 
}

/*
int x_get_open_cmd(const char* suffix, char* buf, unsigned long buf_sz)
{
  if (string(suffix) == ".html" || string(suffix) == ".htm") 
  { const char* path1 = 
        "C:\\Program Files\\Google\\Chrome\\Application\\chrome.exe";
    const char* path2 = 
        "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";

    if (is_file(path1)) 
    { strcpy(buf,path1);
      return 1;
     }

    if (is_file(path2)) 
    { strcpy(buf,path2);
      return 1;
     }
  }

  //char buf[256];

  DWORD sz = buf_sz;
  HKEY hKey;
  RegOpenKeyEx(HKEY_CLASSES_ROOT, suffix, 0, KEY_QUERY_VALUE, &hKey);
  RegQueryValueEx(hKey,NULL, NULL,NULL, (unsigned char*)buf,&sz);
  RegCloseKey(hKey);
  RegOpenKeyEx(HKEY_CLASSES_ROOT, buf,    0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,             "shell", 0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,              "open", 0, KEY_QUERY_VALUE, &hKey);
  RegOpenKeyEx(hKey,           "command", 0, KEY_QUERY_VALUE, &hKey);
  sz = buf_sz;
  RegQueryValueEx(hKey,NULL, NULL,NULL, (unsigned char*)buf,&sz);
  RegCloseKey(hKey);
  return 1;
}
*/




void x_set_tooltip(int win, int id, int x0, int y0, int x1, int y1, 
                                                    const char* text)
{ ms_win* wp=wlist[win];

  if (x0 > x1) SWAP(x0,x1);
  if (y0 > y1) SWAP(y0,y1);

//printf("add tooltip %d at (%d,%d,%d,%d)  txt = %s\n",id,x0,y0,x1,y1,text);
  
  RECT rect;
  rect.left=x0;
  rect.top=y0;
  rect.right=x1;
  rect.bottom=y1;

  TOOLINFO ti;
  ZeroMemory(&ti,sizeof(ti));
  ti.cbSize=sizeof(ti);
  ti.uId=id,
  ti.uFlags=0;
  ti.hwnd=wp->hwnd;
  ti.lpszText=TEXT((char*)text);
  ti.rect=rect;

  SendMessage(wp->hwnd_tt,TTM_ADDTOOL,0,(LPARAM)&ti);
  //SendMessage(wp->hwnd_tt,TTM_SETDELAYTIME,TTDT_AUTOMATIC,250);
  SendMessage(wp->hwnd_tt,TTM_SETDELAYTIME,TTDT_INITIAL,500);
  SendMessage(wp->hwnd_tt,TTM_SETDELAYTIME,TTDT_AUTOPOP,3000);
}


void x_del_tooltip(int win, int id)
{ ms_win* wp=wlist[win];

  TOOLINFO ti;
  ZeroMemory(&ti,sizeof(ti));
  ti.uId=id,
  ti.cbSize=sizeof(ti);
  ti.uFlags=0;
  ti.hwnd=wp->hwnd;

  SendMessage(wp->hwnd_tt,TTM_DELTOOL,0,(LPARAM)&ti);    
}


LEDA_END_NAMESPACE
