#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

int main()
{
  Display* dpy = XOpenDisplay(NULL);

  if (!dpy) {
    fprintf(stderr,"Cannot open X11 display.\n");
    return 1;
  }

  int scr = DefaultScreen(dpy);

  Visual* visual = DefaultVisual(dpy,scr);
  Colormap c_map = DefaultColormap(dpy,scr);

  Window win = XCreateSimpleWindow(dpy, RootWindow(dpy,scr), 
                                        500, 500, 800, 500, 1, 
                                        BlackPixel(dpy,scr), 
                                        WhitePixel(dpy,scr));

  XSelectInput(dpy, win, ExposureMask | ButtonPressMask);
  XMapWindow(dpy, win);

  XftFont* font = XftFontOpenName(dpy, scr,"Droid Sans:size=32:antialias=true");
  if (!font) {
    fprintf(stderr,"Cannot load Xft font.\n");
    return 1;
  }

  XftColor color;
  if (!XftColorAllocName(dpy, visual, c_map, "#000080", &color)) {
    fprintf(stderr,"Cannot allocate Xft color.\n");
    return 1;
  }

  XftDraw* draw = XftDrawCreate(dpy, win, visual, c_map);

  XEvent ev;
  do { XNextEvent(dpy, &ev);
       if (ev.type == Expose)
       { const char* s = "Hello Xft !";
         XftDrawStringUtf8(draw, &color, font, 50, 250, (FcChar8*)s,strlen(s));
       }
  } while (ev.type != ButtonPress);


  XftColorFree(dpy, visual, c_map, &color);
  XftDrawDestroy(draw);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);

  return 0;
}

