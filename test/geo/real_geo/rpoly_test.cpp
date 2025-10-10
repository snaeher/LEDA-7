#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/geo/rat_polygon.h>
#include <LEDA/graphics/real_window.h>

using namespace leda;


r_circle_gen_polygon MakeCircle(double cx, double cy, double r)
{
    list<r_circle_segment> segs;

    segs.append(r_circle_segment(rat_point(cx, cy+r, 64), 
                                 rat_point(cx-r, cy, 64), 
                                 rat_point(cx, cy-r, 64)));

    segs.append(r_circle_segment(rat_point(cx, cy-r, 64), 
                                 rat_point(cx+r, cy, 64), 
                                 rat_point(cx, cy+r, 64)));

    return r_circle_gen_polygon(segs, r_circle_gen_polygon::SIMPLE, 
                                r_circle_gen_polygon::DISREGARD_ORIENTATION);
}



int main()
{
   r_circle_gen_polygon P = MakeCircle(5, 0, 1);
   r_circle_gen_polygon Q = MakeCircle(5.5, 0, 1);

   r_circle_gen_polygon R = P.intersection(Q);
   r_circle_gen_polygon T = P.unite(Q);


   window W;
   W.init(1.5,7.0,-4.0);
   W.set_line_width(1);
   W.display();
   W.set_font("F24");
  
   // draw union
  
   W.draw_text(2,0,"Union");
  
   draw_filled(W,T,green2);
   draw(W,P,black);
   draw(W,Q,black);
  
   // draw intersection shifted by (dx,dy)
  
   double dx =  0.0;
   double dy = -2.3;
  
   W.draw_text(2,dy,"Intersection");
  
   draw_filled(W,R.translate(dx,dy),blue2);
   draw(W,P.translate(dx,dy),black);
   draw(W,Q.translate(dx,dy),black);

   W.read_mouse();

   return 0;
}

 
