/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  polygon_holes.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/gen_polygon.h>
#include <LEDA/geo/rat_gen_polygon.h>
#include <LEDA/graphics/window.h>


using namespace leda;

void poly_info(window& W, const rat_gen_polygon& P, string str, double x, 
                                                                    double y)
{ int p = 0;
  int h = 0;
  rat_polygon pol;
  forall_polygons(pol,P) 
  { if (pol.orientation() > 0) 
       p++;
    else
       h++;
   }
  W.draw_ctext(x,y,str);
  W.draw_ctext(x,y-0.5,string("(%2d / %2d)",p,h));
 }


int main()
{
 window W("Polygons with Holes");
 //W.init(-1.5,18.5,-12.5);
 W.init(-0.5,21,-15);
 W.display();

 // construct a polygon P1 with holes H1,H2,H3,H4,H5

 list<point> B1,H1,H2,H3,H4,H5;

 // boundary
 B1.append(point(1,0));
 B1.append(point(8,0));
 B1.append(point(8,5.5));
 B1.append(point(1,5.5));

 // holes

 H1.append(point(2.5,0.5));
 H1.append(point(6.5,0.5));
 H1.append(point(6.5,1.0));
 H1.append(point(2.5,1.0));

 H2.append(point(2.5,1.5));
 H2.append(point(6.5,1.5));
 H2.append(point(6.5,2.0));
 H2.append(point(2.5,2.0));

 H3.append(point(2.5,2.5));
 H3.append(point(6.5,2.5));
 H3.append(point(6.5,3.0));
 H3.append(point(2.5,3.0));

 H4.append(point(2.5,3.5));
 H4.append(point(6.5,3.5));
 H4.append(point(6.5,4.0));
 H4.append(point(2.5,4.0));

 H5.append(point(2.5,4.5));
 H5.append(point(6.5,4.5));
 H5.append(point(6.5,5.0));
 H5.append(point(2.5,5.0));


 list<polygon> L1;

 L1.append(polygon(B1));
 L1.append(polygon(H1).complement());
 L1.append(polygon(H2).complement());
 L1.append(polygon(H3).complement());
 L1.append(polygon(H4).complement());
 L1.append(polygon(H5).complement());

 gen_polygon P1_(L1);

 W.start_buffering();

 bool interactive = true;

 for(;;)
 { 
   W.clear(); 


   // P2: P1 rotated by random angle and translated by random vector (dx,dy)

   double dx = rand_int(-100,100)/40.0;
   double dy = rand_int(-100,100)/40.0;
   double phi = 2*LEDA_PI*rand_int(0,1000)/1000.0;

   gen_polygon P2_ = P1_.rotate(point(4.5,2.75),phi).translate(dx,dy);

   rat_gen_polygon P1 = P1_.to_rational();
   rat_gen_polygon P2 = P2_.to_rational();

   // intersection

   rat_gen_polygon I = P1.intersection(P2);

   W.draw_polygon(P1.to_float(),grey1);
   W.draw_polygon(P2.to_float(),grey1);
   W.draw_filled_polygon(I.to_float(),orange);
   W.draw_polygon(I.to_float(),black);
   poly_info(W,I,"Intersection",4.5,-0.5);


   // union

   rat_gen_polygon U = P1.unite(P2);
   W.draw_polygon(P1.translate(10,0).to_float(),grey1);
   W.draw_polygon(P2.translate(10,0).to_float(),grey1);
   W.draw_filled_polygon(U.translate(10,0).to_float(),blue2);
   W.draw_polygon(U.translate(10,0).to_float(),black);
   poly_info(W,U,"Union",14.5,-0.5);


   // diff

   rat_gen_polygon D = P1.diff(P2);
   W.draw_polygon(P1.translate(0,-10).to_float(),grey1);
   W.draw_polygon(P2.translate(0,-10).to_float(),grey1);
   W.draw_filled_polygon(D.translate(0,-10).to_float(),green2);
   W.draw_polygon(D.translate(0,-10).to_float(),black);
   poly_info(W,D,"Difference",4.5,-10.5);


   // symdiff

   rat_gen_polygon S = P1.sym_diff(P2);
   W.draw_polygon(P1.translate(10,-10).to_float(),grey1);
   W.draw_polygon(P2.translate(10,-10).to_float(),grey1);
   W.draw_filled_polygon(S.translate(10,-10).to_float(),violet);
   W.draw_polygon(S.translate(10,-10).to_float(),black);
   poly_info(W,S,"S-Difference",14.5,-10.5);


   W.flush_buffer();

   if (!interactive)
   { if (W.get_mouse() != NO_BUTTON) 
       interactive = true;
     else
       leda_wait(0.5);
    }
   
   if (interactive)
   { int but = W.read_mouse();
     if (W.shift_key_down()) interactive = false;
     if (but == MOUSE_BUTTON(2)) interactive = false;
     if (but == MOUSE_BUTTON(3)) break;
    }

  }

  return 0;
}


