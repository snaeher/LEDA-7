/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  polygon_logo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/graphics/window.h>
#include <LEDA/geo/logo.h>

using namespace leda;



typedef rat_gen_polygon GEN_POLYGON;
typedef rat_polygon     POLYGON;

void poly_info(window& W, const GEN_POLYGON& P, string str)
{ int p_count = 0;
  int h_count = 0;
  const list<POLYGON>& L = P.polygons();
  POLYGON pol;
  forall(pol,L) 
  { if (pol.orientation() > 0) 
       p_count++;
    else
       h_count++;
   }
  W.del_messages();
  W.message(str + string(" (%2d / %2d)",p_count,h_count));
 }

GEN_POLYGON P1;
GEN_POLYGON P2;
window* wp;
color c1 = grey1;
color c2 = grey2;

int comp1 = 0;
int comp2 = 0;

int op = 0;


void redraw(int op)
{ window& W = *wp;
  W.start_buffering();
  W.clear();

  GEN_POLYGON pol1 = P1;
  GEN_POLYGON pol2 = P2;

  W.draw_filled_polygon(pol1.to_gen_polygon(),c1);
  W.draw_polygon(pol1.to_gen_polygon(),black);

  W.draw_filled_polygon(pol2.to_gen_polygon(),c2);
  W.draw_polygon(pol2.to_gen_polygon(),black);

  W.flush_buffer();

  if (comp1) pol1 = P1.complement();
  if (comp2) pol2 = P2.complement();

  string cmd;
  GEN_POLYGON Q;
  switch (op) {
     case 0: cmd = "Intersection";
             Q = pol1.intersection(pol2);
             break;
     case 1: cmd = "Union";
             Q = pol1.unite(pol2);
             break;
     case 2: cmd = "Difference";
             Q = pol1.diff(pol2);
             break;
     case 3: cmd = "Symmetric Difference";
             Q = pol1.sym_diff(pol2);
             break;
  }

  W.draw_filled_polygon(Q.to_gen_polygon(),green2);
  poly_info(W,Q,cmd);

  W.flush_buffer();
  W.stop_buffering();
}

void redraw1(int co1) { comp1 = co1; redraw(op); }
void redraw2(int co2) { comp2 = co2; redraw(op); }

void redraw0() { redraw(op); }


int main()
{
 list<string> choice_lst0;
 choice_lst0.append("original");
 choice_lst0.append("complement");

 list<string> choice_lst1;
 choice_lst1.append("intersection");
 choice_lst1.append("union");
 choice_lst1.append("difference");
 choice_lst1.append("symdiff");
 choice_lst1.append("null");

 window W("Polygons with Holes");
 wp = &W;

 W.set_redraw(redraw0);

 W.choice_item("polygon1", comp1, choice_lst0, redraw1);
 W.choice_item("polygon2", comp2, choice_lst0, redraw2);
 W.choice_item("operation",op,choice_lst1,redraw);

 W.button("move", 1);
 W.button("quit", 0);

 W.init(-5,5,-4);
 W.display(window::center,window::center);

 gen_polygon logo = leda_logo(2);

 P1 = GEN_POLYGON(logo,512);

 int but = 1;

 while (but == 1)
 { double dx = rand_int(-1000,1000)/500.0;
   double dy = rand_int(-1000,1000)/500.0;
   double phi = 2*LEDA_PI*rand_int(0,1000)/1000.0;
   P2 = GEN_POLYGON(logo.rotate(point(0,1),phi).translate(dx,dy),512);
   redraw(op);
   but = W.read_mouse();
  }

  return 0;
}
