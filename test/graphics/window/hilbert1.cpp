/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  hilbert.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;

double x, y, dx, dy;

double yoff = 0;

void A(window&,int);
void B(window&,int);
void C(window&,int);
void D(window&,int);


void plot(window& W, double new_x, double new_y)
{ W.draw_segment(x,y+yoff,new_x,new_y+yoff);
  x = new_x;
  y = new_y;
 }


void A(window& W,int i)
{ 
  if (i > 0)
  { D(W,i-1); plot(W,x-dx,y);
    A(W,i-1); plot(W,x,y-dy);
    A(W,i-1); plot(W,x+dx,y); 
    B(W,i-1);
   }
 }

void B(window& W,int i)
{ 
  if (i > 0)
  { C(W,i-1); plot(W,x,y+dy);
    B(W,i-1); plot(W,x+dx,y);
    B(W,i-1); plot(W,x,y-dy); 
    A(W,i-1);
   }
 }



void C(window& W,int i)
{ 
  if (i > 0)
  { B(W,i-1); plot(W,x+dx,y);
    C(W,i-1); plot(W,x,y+dy);
    C(W,i-1); plot(W,x-dx,y); 
    D(W,i-1);
   }
 }

void D(window& W,int i)
{ 
  if (i > 0)
  { A(W,i-1); plot(W,x,y-dy);
    D(W,i-1); plot(W,x-dx,y);
    D(W,i-1); plot(W,x,y+dy); 
    C(W,i-1);
   }
 }



void hilbert(window& W, int n)
{
  W.start_buffering();
  W.clear();

  double lx = W.xmax() - W.xmin();
  double ly = W.ymax() - W.ymin();

  double x0 = W.xmin() + 0.92*lx;
  double y0 = W.ymin() + 0.98*ly;

  dx = 0.92 * lx/(1 << n);
  dy = 0.98 * ly/(1 << n);

  x = x0;
  y = y0;

  A(W,n);

  W.draw_segment(x0,y0+yoff,x0+dx,y0+yoff);
  W.draw_segment(x0+dx,y0+yoff,x0+dx,y+yoff);
  W.draw_segment(x0+dx,y+yoff,x,y+yoff);

  W.flush_buffer();
  W.stop_buffering();
}

void draw_hilbert(int n)
{ window* wp = window::get_call_window();
  hilbert(*wp,n);
}


int n = 5;
void redraw(window* wp) { hilbert(*wp,n); }

static window* wptr;

void scroll_up(int i) {
  yoff -= 1;
  double f = yoff/100;
  wptr->set_scrollbar_pos(f);
  redraw(wptr);
}
void scroll_down(int i) {
  yoff += 1;
  double f = yoff/100;
  wptr->set_scrollbar_pos(f);
  redraw(wptr);
}


void scroll_drag(int i) 
{ //window* wp = window::get_call_window();
/*
  if (i == -1) cout << "drag start" << endl;
  if (i == -2) cout << "drag end" << endl;
*/
  if (i >= 0) yoff = i/10;
  redraw(wptr);
}


int main()
{   
 window W(600,500,"Hilbert");
 wptr = &W;
 W.init(0,100,0);
 W.int_item("n = ",n,1,8,draw_hilbert);
 W.button("up",scroll_up);
 W.button("down",scroll_down);

 W.make_menu_bar(2);

 W.set_redraw(redraw);

 W.display();

 W.open_scrollbar(scroll_up,scroll_down,scroll_drag,0.5,0);


 hilbert(W,n);

 for(;;)
 { int but = W.read_mouse();
   if (but == MOUSE_BUTTON(1)) scroll_up(0);
   if (but == MOUSE_BUTTON(2)) scroll_down(0);
   if (but == MOUSE_BUTTON(3)) break;
 }

 return 0;
}
