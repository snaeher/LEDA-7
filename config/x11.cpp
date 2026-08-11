#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>


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

  XEvent ev;
  do { XNextEvent(dpy, &ev);
       if (ev.type == Expose) {}
  } while (ev.type != ButtonPress);


  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);

  return 0;
}

