/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  sierpinski.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>

using namespace leda;


// global

static double xpos;
static double ypos;
static double w;
static double h;

static int n = 3;


void lineTo(window& W, double dx, double dy) 
{ double x = xpos + dx;
  double y = ypos + dy;
  W.draw_segment(xpos,ypos,x,y);
  xpos = x;
  ypos = y; 
}

void lineN(window& W){ lineTo(W,   0, -2*h); }
void lineS(window& W){ lineTo(W,   0, +2*h); }
void lineE(window& W){ lineTo(W,+2*w,    0); }
void lineW(window& W){ lineTo(W,-2*w,    0); }
 
void lineNW(window& W){ lineTo(W,-w,-h); }
void lineNE(window& W){ lineTo(W,+w,-h); }
void lineSE(window& W){ lineTo(W,+w,+h); }
void lineSW(window& W){ lineTo(W,-w,+h); }


void sierpinski_N(window&,int);
void sierpinski_E(window&,int);
void sierpinski_S(window&,int);
void sierpinski_W(window&,int);

void sierpinskiCurve(window& W, int level, double x, double y,double dx,
                                                              double dy)
{ xpos = x;
  ypos = y;

  w = dx;
  h = dy;

  W.clear();
  W.draw_filled_node(xpos,ypos,red);

  sierpinski_N(W,level);
  lineNE(W);
  sierpinski_E(W,level);
  lineSE(W);
  sierpinski_S(W,level);
  lineSW(W);
  sierpinski_W(W,level);
  lineNW(W);
}


void sierpinski_N(window& W, int i)
{
  if (i == 0) return;

  sierpinski_N(W,i-1);
  lineNE(W);
  sierpinski_E(W,i-1);
  lineN(W);
  sierpinski_W(W,i-1);
  lineNW(W);
  sierpinski_N(W,i-1);
}
 
void sierpinski_E(window& W, int i)
{
  if (i == 0) return;

  sierpinski_E(W,i-1);
  lineSE(W);
  sierpinski_S(W,i-1);
  lineE(W);
  sierpinski_N(W,i-1);
  lineNE(W);
  sierpinski_E(W,i-1);
}
 
void sierpinski_S(window& W, int i)
{
  if (i == 0) return;

  sierpinski_S(W,i-1);
  lineSW(W);
  sierpinski_W(W,i-1);
  lineS(W);
  sierpinski_E(W,i-1);
  lineSE(W);
  sierpinski_S(W,i-1);
}
 
void sierpinski_W(window& W, int i)
{
  if (i == 0) return;

  sierpinski_W(W,i-1);
  lineNW(W);
  sierpinski_N(W,i-1);
  lineW(W);
  sierpinski_S(W,i-1);
  lineSW(W);
  sierpinski_W(W,i-1);
}


void redraw(window* wp)
{
  window& W = *wp;

  double width  = W.xmax() - W.xmin();
  double height = W.ymax() - W.ymin();

  double dx = (0.95*width)/(1<<(n+2));
  double dy = dx;

  double x = 0.025*width;
  double y = W.ymax() - 0.025*height - dy;

  x += dx;
  y -= dy;

  sierpinskiCurve(W,n,x,y,dx,dy);
 }


int main()
{   
 window W(1000,1000,"Sierpinski Curve");

 W.set_color(blue);
 W.set_line_width(2);
 W.set_node_width(4);
 W.set_redraw(redraw);

 panel P("sierpinski(n)");
 P.text_item("\\bf\\blue Sierpinski Curve");
 P.text_item("");
 P.int_item("levels",n,0,10);

 P.button("continue",1);
 P.button("quit",0);

 W.display();

 while (P.open(W) == 1)
 { redraw(&W);
   W.read_mouse();
 }

 
  return 0;
}
