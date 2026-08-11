#include<LEDA/graphics/window.h>

using namespace leda;

void draw_box(window& W, double x, double y, color clr) { 
  W.draw_box(x,y,x+20,y+20,clr);
}

void draw_segment(window& W, double x, double y, color clr) {  
  W.draw_segment(0,0,x,y,clr);
}

void draw_triangle(window& W, double x, double y, color clr) {  
  point a(x,y);
  point b(x-10,y+20);
  point c(x+10,y+20);
  W.draw_filled_triangle(a,b,c,clr);
}

int main() {

  window W;

  W.display(100,100);

/*
  segment s; 
  W >> s;
  W.draw_segment(s);
*/

  double x_last = 0;
  double y_last = 0;

  //color clr = black;
  //color clr = grey1;
  //color clr = 0xf333333;
  color clr = blue;

  draw_box(W,10,10,black);
  draw_box(W,10,40,red);
  draw_box(W,40,10,green);
  draw_box(W,40,40,blue);

  W.set_mode(xor_mode);

  int e = no_event;
  int val;
  double x,y;
  while ((e=W.read_event(val,x,y)) != button_press_event)
  { 
    if (e != motion_event) continue;

    draw_box(W,x,y,clr);
    if (x_last != 0 && y_last != 0) draw_box(W,x_last,y_last,clr);

/*
    draw_triangle(W,x,y,clr);
    if (x_last != 0 && y_last != 0) draw_triangle(W,x_last,y_last,clr);
*/

/*
    draw_segment(W,x,y,clr);
    if (x_last != 0 && y_last != 0) draw_segment(W,x_last,y_last,clr);
*/

    x_last = x;
    y_last = y;
  }

  W.set_mode(src_mode);


  return 0;
}
