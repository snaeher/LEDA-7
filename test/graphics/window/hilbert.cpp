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

static double xpos, ypos, w, h;

void A(window&,int);
void B(window&,int);
void C(window&,int);
void D(window&,int);


void lineTo(window& W, double dx, double dy)
{ W.draw_segment(xpos,ypos,xpos+dx,ypos+dy);
  xpos += dx;
  ypos += dy;
 }


void A(window& W, int i)
{ 
  if (i > 0)
  { D(W,i-1); lineTo(W,-w,0);
    A(W,i-1); lineTo(W,0,-h);
    A(W,i-1); lineTo(W,w,0); 
    B(W,i-1);
   }
 }

void B(window& W, int i)
{ 
  if (i > 0)
  { C(W,i-1); lineTo(W,0,h);
    B(W,i-1); lineTo(W,w,0);
    B(W,i-1); lineTo(W,0,-h); 
    A(W,i-1);
   }
 }



void C(window& W, int i)
{ 
  if (i > 0)
  { B(W,i-1); lineTo(W,w,0);
    C(W,i-1); lineTo(W,0,h);
    C(W,i-1); lineTo(W,-w,0); 
    D(W,i-1);
   }
 }

void D(window& W, int i)
{ 
  if (i > 0)
  { A(W,i-1); lineTo(W,0,-h);
    D(W,i-1); lineTo(W,-w,0);
    D(W,i-1); lineTo(W,0,h); 
    C(W,i-1);
   }
 }


int n = 5;

void hilbert(window* wp)
{
   window& W = *wp;

   W.clear();

   double width  = 0.9*(W.xmax() - W.xmin());
   double height = 0.9*(W.ymax() - W.ymin());

   double x0 = W.xmin() + width;
   double y0 = W.ymin() + height;

   w = 0.9*width/(1 << n);
   h = 0.9*height/(1 << n);

   xpos = x0;
   ypos = y0;

   A(W,n);

   W.draw_segment(x0,y0,x0+w,y0);
   W.draw_segment(x0+w,y0,x0+w,ypos);
   W.draw_segment(x0+w,ypos,xpos,ypos);

  }


int main()
{   
 window W("Hilbert Curve");
 W.display();

 W.set_color(blue);
 W.set_line_width(2);

 panel P("hilbert(n)");
 P.text_item("");
 P.int_item("n = ",n,1,10);

 P.button("continue",1);
 P.button("quit",0);

 W.set_redraw(hilbert);

 while (P.open(W) != 0)
 { hilbert(&W);
   W.read_mouse();
  }
 
  return 0;
}
