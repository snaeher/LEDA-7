/*******************************************************************************
+
+  LEDA 7.2.2  
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

void A(window&,int);
void B(window&,int);
void C(window&,int);
void D(window&,int);


void plot(window& W, double new_x, double new_y)
{ W.draw_segment(x,y,new_x,new_y);
  x = new_x;
  y = new_y;
 }


void A(window& W, int i)
{ 
  if (i > 0)
  { D(W,i-1); plot(W,x-dx,y);
    A(W,i-1); plot(W,x,y-dy);
    A(W,i-1); plot(W,x+dx,y); 
    B(W,i-1);
   }
 }

void B(window& W, int i)
{ 
  if (i > 0)
  { C(W,i-1); plot(W,x,y+dy);
    B(W,i-1); plot(W,x+dx,y);
    B(W,i-1); plot(W,x,y-dy); 
    A(W,i-1);
   }
 }



void C(window& W, int i)
{ 
  if (i > 0)
  { B(W,i-1); plot(W,x+dx,y);
    C(W,i-1); plot(W,x,y+dy);
    C(W,i-1); plot(W,x-dx,y); 
    D(W,i-1);
   }
 }

void D(window& W, int i)
{ 
  if (i > 0)
  { A(W,i-1); plot(W,x,y-dy);
    D(W,i-1); plot(W,x-dx,y);
    D(W,i-1); plot(W,x,y+dy); 
    C(W,i-1);
   }
 }


int n = 5;

void hilbert(window* wp)
{
   window& W = *wp;

   W.start_buffering();

   W.clear();

   double width  = 0.9*(W.xmax() - W.xmin());
   double height = 0.9*(W.ymax() - W.ymin());

   double x0 = W.xmin() + width;
   double y0 = W.ymin() + height;

   dx = 0.9*width/(1 << n);
   dy = 0.9*height/(1 << n);

   x = x0;
   y = y0;

   A(W,n);

   W.draw_segment(x0,y0,x0+dx,y0);
   W.draw_segment(x0+dx,y0,x0+dx,y);
   W.draw_segment(x0+dx,y,x,y);

   W.flush_buffer();
   W.stop_buffering();
 }


int main()
{   
 window W("Hilbert Curve");
 W.display();

 W.set_color(blue);
 W.set_line_width(2);

 panel P("Hilbert");
 P.text_item("\\bf hilbert(n)");
 P.text_item("");
 P.int_item("n  = ",n,1,10);

 P.button("continue",1);
 P.button("quit",0);

 W.set_redraw(hilbert);

 while (P.open(W) != 0)
 { hilbert(&W);
   W.read_mouse();
  }
 
  return 0;
}
