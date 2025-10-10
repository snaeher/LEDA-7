/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_dc_hull.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/d3_window.h>
#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/geo/d3_hull.h>

using namespace leda;



static window W(550,650,"3D Convex Hull Demo");

static int maxcoord = 1000;

void generate_input(int n, int input, int maxc, list<d3_rat_point>& L)
{ L.clear();
  W.clear();
  W.message("Generating Points");

  switch (input) {

   case 0: random_points_in_cube(n,3*maxc/4,L);
           break;

   case 1: random_points_in_ball(n,maxc,L);
           break;

   case 2: random_points_in_square(n,maxc,L);
           break;

   case 3: random_points_on_paraboloid(n,maxc,L);
           break;

   case 4: lattice_points(n,3*maxc/4,L);
           break;

   case 5: random_points_on_sphere(n,maxc,L);
           break;

   case 6: random_points_on_segment(n,maxc,L);
           break;

  }

  d3_rat_point p; 
  forall(p,L)
     W.draw_point(p.xcoordD(),p.ycoordD(),blue);
  W.del_messages();
}



int main() {

 int n = 32;
 int t = 0;
 int input = 0;

 list<string> choices;
 choices.append("cube");
 choices.append("ball");
 choices.append("square");
 choices.append("para");
 choices.append("mesh");
 choices.append("sphere");
 choices.append("line");

 list<string> thousands;
 thousands.append("0");
 thousands.append("1000");
 thousands.append("2000");
 thousands.append("4000");
 thousands.append("8000");
 thousands.append("16000");
 thousands.append("32000");

 W.choice_item("input",input,choices);
 W.choice_item("points",t,thousands);
 W.int_item("    +",n,3,200);

 W.button("gen",1);
 W.button("run",2);
 W.button("setup",3);
 W.button("exit",0);

 W.init(-maxcoord,maxcoord,-3*maxcoord/4);
 W.display();

 W.set_node_width(2);

 list<d3_rat_point> L;
 GRAPH<d3_rat_point,int> H;

 int anim_speed = 10;

 d3_window anim(W,H);
 anim.set_elim(true);
 anim.set_solid(true);
 anim.set_speed(anim_speed);

 int but = W.read_mouse();

 while (but != 0)
 {
   switch (but) {
  
   case 1: { int m = (1 << t)/2 * 1000;
             generate_input(m+n,input,int(W.xmax()/2),L);
             H.clear();
             break;
           }

   case 2: { float t1 = used_time();
             D3_DC_HULL(L,H);
             float t2 = used_time();
             W.clear();
             node_array<rat_vector> pos(H);
             node v;
             forall_nodes(v,H) pos[v] = H[v].to_vector();
             anim.init(pos);
             anim.draw();
             W.message(string("%5.2f sec",t2-t1));
             break;
            }

   case 3: { panel P;
             P.int_item("max xcoord",maxcoord,1,1000);
             P.int_item("anim speed",anim_speed,1,32);
             P.open(W);
             W.init(-maxcoord,maxcoord,-3*maxcoord/4);
             anim.set_speed(anim_speed);
             break;
           }


   }

   but = anim.move();
 }


 return 0;
}
