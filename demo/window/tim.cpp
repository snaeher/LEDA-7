/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  tim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/window.h>
#include <ctype.h>

using namespace leda;



void scroll_up(window& W, double ymax, double fh)
{ double xmin = W.xmin();
  double xmax = W.xmax();
  double ymin = W.ymin();

  double pp = W.pix_to_real(1);

  char* buf = W.get_pixrect(xmin,ymin+pp,xmax,ymax);
  int w = W.get_pixrect_width(buf);
  int h = W.get_pixrect_height(buf);

  for(int y = 0; y < fh; y+=2)
  { int pix_y = W.real_to_pix(y);
    W.put_pixrect(xmin,ymin+y,buf,0,pix_y,w,h-pix_y);
   }
  W.del_pixrect(buf);
 }


void locomotive(window& W, double x, double y, double f, color col1, color col2)
{ 
  list<point> poly;
  poly.append(point(x+0.5*f,y+1.0*f));
  poly.append(point(x+0.5*f,y+3.0*f));
  poly.append(point(x+4.0*f,y+3.0*f));
  poly.append(point(x+4.0*f,y+5.0*f));
  poly.append(point(x+3.5*f,y+5.0*f));
  poly.append(point(x+3.5*f,y+5.0*f));
  poly.append(point(x+7.0*f,y+5.0*f));
  poly.append(point(x+7.0*f,y+5.0*f));
  poly.append(point(x+6.5*f,y+5.0*f));
  poly.append(point(x+6.5*f,y+1.0*f));

  W.draw_filled_polygon(poly,col1);
  W.draw_polygon(poly,black);
  W.draw_disc  (x+1.75*f, y+f, 0.72*f,col2);
  W.draw_circle(x+1.75*f, y+f, 0.72*f,black);
  W.draw_filled_node(x+1.75*f, y+f,black);
  W.draw_disc  (x+5.25*f, y+f, 0.72*f,red);
  W.draw_circle(x+5.25*f, y+f, 0.72*f,black);
  W.draw_filled_node(x+5.25*f, y+f,black);

  W.draw_filled_rectangle(x+4.6*f,y+3*f,x+5.9*f,y+4.0*f,white);
  W.draw_rectangle       (x+4.6*f,y+3*f,x+5.9*f,y+4.0*f,black);
  W.draw_filled_rectangle(x+1.1*f,y+3*f,x+1.9*f,y+4.0*f,black);

  W.draw_filled_rectangle(x+6.50*f,y+1.3*f,x+7.35*f,y+1.5*f,black);
  W.draw_segment(x+7.35*f,y+1.0*f,x+7.35*f,y+1.8*f,black);
}


void waggon(window& W, double x, double y, double f, color col1, color col2)
{
  list<point> poly;
  poly.append(point(x+0.5*f,y+1.0*f));
  poly.append(point(x+0.5*f,y+4.0*f));
  poly.append(point(x+0.0*f,y+4.0*f));
  poly.append(point(x+0.0*f,y+5.0*f));
  poly.append(point(x+7.0*f,y+5.0*f));
  poly.append(point(x+7.0*f,y+4.0*f));
  poly.append(point(x+6.5*f,y+4.0*f));
  poly.append(point(x+6.5*f,y+1.0*f));

  W.draw_filled_rectangle(x-0.35*f,y+1.3*f,x+0.50*f,y+1.5*f,black);
  W.draw_segment(x-0.35*f,y+1.0*f,x-0.35*f,y+1.8*f,black);

  W.draw_filled_polygon(poly,col1);
  W.draw_polygon(poly,black);
  W.draw_disc  (x+1.75*f, y+f, 0.72*f,col2);
  W.draw_circle(x+1.75*f, y+f, 0.72*f,black);
  W.draw_filled_node(x+1.75*f, y+f,black);
  W.draw_disc  (x+5.25*f, y+f, 0.72*f,col2);
  W.draw_circle(x+5.25*f, y+f, 0.72*f,black);
  W.draw_filled_node(x+5.25*f, y+f,black);

  W.draw_box      (x+1.50*f,y+2.5*f,x+2.75*f,y+3.5*f,white);
  W.draw_rectangle(x+1.50*f,y+2.5*f,x+2.75*f,y+3.5*f,black);
  W.draw_box      (x+4.25*f,y+2.5*f,x+5.50*f,y+3.5*f,white);
  W.draw_rectangle(x+4.25*f,y+2.5*f,x+5.50*f,y+3.5*f,black);

  W.draw_filled_rectangle(x+6.50*f,y+1.3*f,x+7.35*f,y+1.5*f,black);
  W.draw_segment(x+7.35*f,y+1.0*f,x+7.35*f,y+1.8*f,black);
}


char* make_train(window& W, int f, color c0, color c1, color c2, color c3)
{ locomotive(W,0,10,f,c0,c3);
  waggon(W,  8*f,10,f,c1,c0);
  waggon(W, 16*f,10,f,c2,c1);
  waggon(W, 24*f,10,f,c3,c2);
  char* map = W.get_pixrect(0.25*f,10+0.25*f,33*f,10+5.75*f);
  W.clear();
  return map;
}


int main()
{
   int scale = 64;
   color col = blue;

   color loc_col  = blue2;
   color wag1_col = yellow;
   color wag2_col = green2;
   color wag3_col  = red;

   //window W(1250,1000,"Tim's erstes Schreibprogramm");

   window W("Tim's erstes Schreibprogramm");

   W.init(0,1000,0);

   W.set_text_mode(opaque);

   W.set_node_width(1);
   W.open(window::center,window::center);

   panel P;
   P.color_item("Lokomotive",loc_col);
   P.color_item("Wagen 1",wag1_col);
   P.color_item("Wagen 2",wag2_col);
   P.color_item("Wagen 3",wag3_col);
   P.color_item("Textfarbe",col);
   P.int_item("Texthoehe",scale,1,128);

   P.button("ok",0);
   P.button("exit",1);

   W.set_font(string("F%02d",scale));

   int fw = (int)W.text_width("H");
   int fh = (int)W.text_height("H");

   int clen = 960/fw;

   int xmin = 20;
   int xmax = xmin + (clen-1)*fw;
   int ymin = fh;
   int ymax = int(W.ymax() - 100);

   int x = xmin;
   int y = ymax;

   double train_x = 500;
   //double train_d = -3;
   double train_d = -0.5;
   int    train_steps = 150;

   int F = 15;

   char* train_image = make_train(W,F,loc_col,wag1_col,wag2_col,wag3_col);



   W.draw_hline(ymax+10);
   W.put_pixrect(train_x,W.ymax()-5.5*(F+1),train_image);
   W.draw_point(train_x,W.ymax()-5.5*(F+1));

   double pp = W.pix_to_real(2);

   W.draw_text(x+pp,y+pp,"_",col);

   bool running = true;

   while (running)
   { int val;
     double a,b;
/*
     unsigned long t;
     int e = W.read_event(val,a,b,t,50);
*/

     int e = W.get_event(val,a,b);

     switch (e) 
     { 
        case button_press_event:
                { if (val  == MOUSE_BUTTON(3))
                  { int but = P.open(W);
                    if (but == 1) {
                      running = false;
                      break;
                    }
                      
                    W.set_font(string("F%02d",scale));

                    train_image = make_train(W,F,loc_col,wag1_col,wag2_col,
                                                                  wag3_col);
                    fw = (int)W.text_width("H");
                    fh = (int)W.text_height("H");
                    clen = 960/fw;
                    xmax = xmin + (clen-1)*fw;
                   }
                  W.clear();
                  W.draw_hline(ymax+10);
                  W.put_pixrect(train_x,W.ymax()-5.5*(F+1),train_image);
                  x = xmin;
                  y = ymax;
                  W.draw_text(x+pp,y+pp,"_",col);
                  W.draw_hline(ymax+10);
                  break;
                 }
           

        case key_press_event: 
                { train_steps = 0;

                  switch (val) {

                     case KEY_BACKSPACE:
                       { W.draw_text(x,y,"   ",col);
                         x -= fw;
                         if (x < xmin)
                         { x = xmax;
                           y += fh;
                          }
                         if (y > ymax)
                         { y = ymax;
                           x = xmin;
                          }
                         break;
                        }
   
                     case KEY_RETURN:
                       { W.draw_text(x,y," ",col);
                         x = xmax+1;
                         break;
                        }
   
                     default:
                       { val = toupper(val);
                         W.draw_text(x,y,string(val),col);
                         x += fw;
                         break;
                        }
                   }

                  if (x > xmax)
                  { x = xmin;
                    y -= fh;
                    if (y < ymin)
                    { scroll_up(W,ymax,fh);
                      y += fh;
                     }
                   }

                  W.draw_text(x+pp,y+pp,"_ ",col);
                  break;
               }


        case no_event:
                  { if (train_steps < 100)
                    { train_x += train_d;
                      double y = W.ymax() - 5.5*(F+1);
                      W.put_pixrect(train_x,y,train_image);
                      if (train_x < W.xmin()-480) train_x = W.xmax(); 
                      train_steps++;
                     }
                    break;
                   }

       }

   }

  return 0;

}

