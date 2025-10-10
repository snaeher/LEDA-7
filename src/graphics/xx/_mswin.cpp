#include <LEDA/core/string.h>
#include <LEDA/graphics/x_window.h>
#include <LEDA/graphics/pixmaps/win_icon.xpm>
#include <LEDA/graphics/pixmaps/win_small_icon.xpm>


#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include <windows.h>


LEDA_BEGIN_NAMESPACE


extern int  OPEN_DISPLAY(int& width,int& height,int& depth,int& dpi);

extern void CLOSE_DISPLAY();

extern void UPDATE_DISPLAY(unsigned int* pixels, int w, int h,
                           int x0, int y0, int x1, int y1, 
                           int x, int y);

extern int  NEXT_EVENT(string& event, int& val, int& x, int& y,unsigned long& t,
                                                               int msec);
extern void SET_CURSOR(int id);

extern void SEND_TEXT(string txt);


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
static event button_event;

//static int trace_events = 1;
static int trace_events = 0;

static int alt_key_down = 0;

static HWND hwnd = 0;
static HCURSOR cursor = 0;

static char* class_name = "LEDA Window";


static LRESULT WINAPI WndProc (HWND hwnd, UINT message, WPARAM wParam, 
                                                        LPARAM lParam)
{
/*
  if (trace_events == 2 && message != WM_TIMER)
  { char * msg = 0;
    if (message < 1024) msg = message_table[message];
    if (msg == 0) msg = "null";
    printf("mess = %s (%03x)  hwnd = %p  wParam = %d lParam = %d\n",
            msg, message, hwnd, (int)wParam, (int)lParam);
    fflush(stdout);
  }
*/

  int win = GetWindowLong(hwnd,0);

  cur_event.win = win;
  cur_event.kind = no_event;
  cur_event.x = 0;
  cur_event.y = 0;
  cur_event.val1 = 0;
  cur_event.val2 = 0;
  cur_event.t = 0; 

  switch (message) { 

      case WM_CREATE :
         { if (trace_events) 
           { printf("CREATE win = %d\n", win);
             fflush(stdout);
            }
           break;
          }

      case WM_SETCURSOR :
         { if (trace_events) 
           { printf("SETCURSOR win = %d\n", win);
             fflush(stdout);
            }
           SetCursor(cursor);
           return 0;
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
           int x = rect.left; 
           int y = rect.top;
           int w = rect.right - rect.left + 1;
           int h = rect.bottom - rect.top + 1;

            if (trace_events) {
             printf("PAINT: win = %d %d %d %d %d\n",win,x,y,w,h);
             fflush(stdout);
            }

/*
           if (wp->repaint) {
              (wp->repaint)(wp->inf,x,y,w,h,wp->resizing);
           }
           else
*/

           cur_event.kind = exposure_event;
           cur_event.x   = x;
           cur_event.y   = y;
           cur_event.val1 = w;
           cur_event.val2 = h;

           ValidateRect(hwnd,NULL);
           break;
          }


      case WM_ENTERSIZEMOVE:
      case WM_EXITSIZEMOVE:
           { if (trace_events) 
             { //printf("%s  win = %d\n",message_table[message],win);
               printf("SIZEMOVE:  win = %d\n",win);
               fflush(stdout);
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
         { if (trace_events) {
             printf("SIZE  win = %d\n", w);
             fflush(stdout);
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

           if (trace_events) 
           { printf("MOUSEMOVE win = %d  x = %d  y = %d\n", 
                               win, cur_event.x, cur_event.y);
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
           { printf("NCMOUSEMOVE win = %d  x = %d  y = %d\n", 
                                 win, cur_event.x, cur_event.y);
             fflush(stdout);
            }

           return 0;
          }



      case WM_KILLFOCUS:
         { if (trace_events) printf("KILLFOCUS\n");
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

           //x_ungrab_pointer();

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

              case VK_F10: //display_info();
                           break;

              case VK_F11: 
                          if (message == WM_KEYDOWN)
                          { if (trace_events == 1) trace_events = 0;
                            else trace_events = 1;
                            printf("LEDA EVENTS = %d\n",trace_events);
			    //display_info();
                            fflush(stdout);
                           }
                          cur_event.kind = no_event;
                          break;

              case VK_F12:
                          if (message == WM_KEYDOWN)
                          { if (trace_events == 2) trace_events = 0;
                            else trace_events = 2;
                            printf("ALL EVENTS = %d\n",trace_events);
                            //display_info();
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

           if (!iscntrl(c))
           { cur_event.kind = key_press_event;
             cur_event.val1 = c;
            }
           return 0;
         }
         


      case WM_CLOSE:
           if (trace_events) 
           { printf("WM_CLOSE\n");
             fflush(stdout);
            }
           cur_event.kind = destroy_event;
           exit(0);
           return 0;


      case WM_DESTROY :
           if (trace_events) {
             printf("DESTROY: w = %d\n",win);
             fflush(stdout);
            }
           return 0;

     }

  return DefWindowProc (hwnd, message, wParam, lParam);
}


void CLOSE_DISPLAY()
{ if (hwnd == 0) return;
  ShowWindow(hwnd,SW_HIDE);
  HDC hdc = GetDC(hwnd);
  ReleaseDC(hwnd,hdc);
  DestroyWindow(hwnd);
  hwnd = 0;
}


int OPEN_DISPLAY(int mode,int& disp_width,int& disp_height, int& depth,
                                                             int& dpi)
{
  HDC hdc0 = GetDC(0);
  int screen_width  = GetSystemMetrics(SM_CXSCREEN); 
  int screen_height = GetSystemMetrics(SM_CYSCREEN); 

  int width  = int(0.85*screen_width);
  int height = int(0.9*screen_height);

  dpi = GetDeviceCaps(hdc0,LOGPIXELSX);
  depth = 24;

  disp_width = width;
  disp_height = height;

  // register window classes

  WNDCLASSEX    wndclass;
  wndclass.cbSize = sizeof(WNDCLASSEX);

  wndclass.lpszClassName = class_name;
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

  RegisterClassEx(&wndclass);

  cursor = LoadCursor(NULL,IDC_ARROW);

  // open window

  DWORD win_style = 0;
  DWORD win_ex_style = 0;

  
  width  += 2*GetSystemMetrics(SM_CXFRAME);
  height += 2*GetSystemMetrics(SM_CYFRAME);
  height += GetSystemMetrics(SM_CYCAPTION);

  win_style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
  win_ex_style = WS_EX_APPWINDOW;

  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = width;
  rect.bottom = height;
  AdjustWindowRectEx(&rect,win_style,FALSE,win_ex_style);

  width = rect.right;
  height = rect.bottom;

  char* label = "LEDA";
  int x = screen_width-width;
  int y = 0;

    hwnd = CreateWindowEx(win_ex_style,         // extended style
                          class_name,           // window class name
                          label,                // frame label
                          win_style,            // window style
                          x,y,                  // window position
                          width,height,         // window size
                          NULL,                 // handle parent window 
                          NULL,                 // handle menu
                          NULL,                 // handle program copy
                          NULL);                // special parameter

    if (hwnd == 0)   {
      // error
     }

    int win = 0;
    SetWindowLong(hwnd,0,win);

    MoveWindow(hwnd,x,y,width,height,FALSE);
    ShowWindow(hwnd,SW_SHOWNORMAL);


  // process initial events

    MSG msg;
    while (PeekMessage(&msg,hwnd,0,0,PM_REMOVE) != 0)
    { TranslateMessage(&msg);
      DispatchMessage(&msg);
      //cout << "peek message" << endl;
     }

/*
  // draw a circle
  HDC hdc = GetDC(hwnd);
  HBRUSH save_brush = (HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
  LOGPEN pen_data;
  pen_data.lopnStyle   = PS_SOLID;
  pen_data.lopnWidth.x = 2;
  pen_data.lopnWidth.y = 2;
  pen_data.lopnColor   = 0xff0000;
  DeleteObject(SelectObject(hdc,CreatePenIndirect(&pen_data)));
  Ellipse(hdc,500,500,300,300);
  SelectObject(hdc,save_brush);
*/

  return 0;
}


//------------------------------------------------------------------------------


void UPDATE_DISPLAY(unsigned int* pixels, int w, int h,
                    int x0, int y0, int x1, int y1,
                    int x, int y)
{
  HDC hdc = GetDC(hwnd);

  // restrict rectangle coordinates to pixel map 

  if (x0 < 0) { x -= x0; x0 = 0; }
  if (y0 < 0) { y -= y0; y0 = 0; }
  if (x1 > w-1) x1 = w - 1;
  if (y1 > h-1) y1 = h - 1;

  if (x0 > x1 || y0 > y1) return;

  BITMAPINFO bm_info;
  BITMAPINFOHEADER* pm = &bm_info.bmiHeader;
  ZeroMemory(pm,sizeof(BITMAPINFOHEADER));
  pm->biSize = sizeof(BITMAPINFOHEADER);
  pm->biWidth = w;
  pm->biHeight = -h;  // negative !
  pm->biPlanes = 1;
  pm->biBitCount = 32;
  pm->biCompression = BI_RGB;  // BI_PNG, BI_JPEG

  int rect_w = x1-x0+1;
  int rect_h = y1-y0+1;

  SetDIBitsToDevice(hdc,
                    x,y,           // xDest,yDest
                    rect_w,        // subimage width
                    rect_h,        // subimage height
                    x0,            // xSrc
                    h-(y0+rect_h), // ySrc (lower left / bottom up !)
                    0,             // startscan
                    h,             // cLines
                    (char*)pixels, // pixels
                    &bm_info,
                    DIB_RGB_COLORS);
}



//------------------------------------------------------------------------------
// events 
//------------------------------------------------------------------------------

static int get_next_event(int& win, int& x, int& y, int& val1, int& val2, 
                                                               unsigned long& t)
{ 
  // blocking: wait for an event

  MSG msg;
  cur_event.kind = no_event;

  int res = GetMessage (&msg,NULL,0,0);

  if (res > 0)
  { TranslateMessage(&msg);
    DispatchMessage(&msg);
   }

  if (cur_event.kind != exposure_event)
      cur_event.t = (unsigned long)msg.time;

/*
if (cur_event.kind != no_event && cur_event.kind != timer_event) {
printf("get_next_event: win = %d  kind = %s\n", cur_event.win,
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


  return cur_event.kind;
}



static int get_next_event(int& win, int& x, int& y, int& val1, int& val2, 
                                                               unsigned long& t,
                                                               int msec)
{ // get next event with timeout

  MSG msg;
  if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
  { int k = get_next_event(win,x,y,val1,val2,t); 
    if (k != no_event) return k;
   }

  SetTimer(hwnd,2,msec,NULL);

  int k = no_event;
  while (k == no_event) k = get_next_event(win,x,y,val1,val2,t);

  KillTimer(hwnd,2);

  if (k == timer_event && val1 == 2) 
   { win = 0;
     k = no_event;
    }

  return k;
}



int NEXT_EVENT(string& ev, int& val, int& x, int& y, unsigned long& t, int msec)
{
  ev = "";

  int win = 0;
  int val2 = 0;

  int e = no_event;

  if (msec > 0)  {
    // blocking with timeout
    e = get_next_event(win,x,y,val,val2,t,msec);
  }

  if (msec == 0) {
    // blocking / no timeoute 
    e = get_next_event(win,x,y,val,val2,t);
  }

  if (msec < 0)  {
    // non-blocking
    MSG msg;
    if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
      e = get_next_event(win,x,y,val,val2,t); 
  }

  return e;
}

//-----------------------------------------------------------------------------
// mouse cursor
//-----------------------------------------------------------------------------

void SET_CURSOR(int id)
{
  LPCSTR c = IDC_ARROW;

  switch (id) {

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
           c = IDC_CROSS;
           break;

  case XC_watch:
           c = IDC_WAIT;
           break;

  case XC_hand2:
           c = IDC_HAND;
           break;

  case XC_dotbox:
           c = IDC_SIZEALL;
           break;

  case XC_top_left_corner:
           c = IDC_SIZENWSE;
           break;

  case XC_top_right_corner:
           c = IDC_SIZENESW;
           break;

  case XC_bottom_left_corner:
           c = IDC_SIZENESW;
           break;

  case XC_bottom_right_corner:
           c = IDC_SIZENWSE;
           break;
  }

  cursor = LoadCursor(NULL,c);
  
  SetCursor(cursor);
}

void SEND_TEXT(string txt) {}


LEDA_END_NAMESPACE
