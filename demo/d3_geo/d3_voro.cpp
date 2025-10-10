/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_voro.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_delaunay.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/d3_window.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/stream.h>

#include <math.h>


using namespace leda;



enum { file_load, file_save, file_all, file_exit };


static window W("3D Convex Hull Demo");

static int maxcoord = 1000;
static list<d3_rat_point> ptlist;

void redraw()
{ d3_rat_point p; 
  forall(p,ptlist)
     W.draw_point(p.xcoordD(),p.ycoordD(),blue);
}

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

  W.del_messages();
  redraw();
}



static string  dname = ".";
static string  fname = "d3points.dat";
static string  filter = "";
static bool    new_input = false;

static void read_file(string fn)
{ file_istream in(fn);
  ptlist.read(in);
  W.clear();
  redraw();
  new_input = true;
 }

static void write_file(string fn)
{ file_ostream out(fn);
  ptlist.print(out);
 }


static void file_handler(int what)
{
  file_panel FP(W,fname,dname);

  switch (what) {
  case file_load: FP.set_load_handler(read_file);
                  break;
  case file_save: FP.set_save_handler(write_file);
                  break;
  case file_all:  FP.set_load_handler(read_file);
                  FP.set_save_handler(write_file);
                  break;
  }
  if (filter != "") FP.set_pattern(filter);
  FP.open();
}




int main() {

 int n = 32;
 int t = 0;
 int input = 0;

 menu file_menu;
 file_menu.button("Load File",file_load,file_handler);
 file_menu.button("Save File",file_save,file_handler);

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
 W.int_item("    +",n,4,200);

 W.button("gen",1);
 W.button("file",  99,file_menu);
 W.button("run",2);
 W.button("setup",3);
 W.button("exit",0);

 W.set_redraw(redraw);

 W.init(-maxcoord,maxcoord,-maxcoord);
 W.display(window::center,window::center);

 W.set_node_width(2);

 GRAPH<d3_rat_point,int> H;

 int anim_speed = 12;

 d3_window anim(W,H);
 anim.set_elim(false);
 anim.set_solid(false);
 anim.set_speed(anim_speed);

 int but = W.read_mouse();

 while (but != 0)
 {
   switch (but) {
  
   case 1: { int m = (1 << t)/2 * 1000;
             generate_input(m+n,input,int(W.xmax()/2),ptlist);
             new_input = true;
             break;
           }

   case 2: { float t1 = used_time();
             //D3_HULL(ptlist,H,0);
             GRAPH<d3_rat_point,int> G;
             D3_VORONOI(ptlist,G,true);

             node v;
             forall_nodes(v,G)
             { if (G.outdeg(v) == 1)
               { node u = G.target(G.first_adj_edge(v));
                 G[v] = G[u] + G[v].to_vector();
                 //G[v] = G[u].to_float() + G[v].to_float().to_vector().norm();
                }
              }

             H.join(G);
             float t2 = used_time();
             W.clear();
             node_array<rat_vector> pos(H);
             forall_nodes(v,H) pos[v] = H[v].to_vector();
             anim.init(pos);
             anim.set_center(0,0,0);
             edge e;
             forall_edges(e,H)
             { if (H.outdeg(source(e)) == 1 || H.outdeg(target(e)) == 1)
               anim.set_color(e,grey2);
              }
             anim.draw();
             W.message(string("%5.2f sec",t2-t1));
             break;
            }

   case 3: { panel P;
             P.int_item("max xcoord",maxcoord,1,1000);
             P.int_item("anim speed",anim_speed,1,32);
             P.open(W);
             W.init(-maxcoord,maxcoord,-maxcoord);
             anim.set_speed(anim_speed);
             break;
           }


   }

   if (new_input)
   { H.clear();
     d3_rat_point p;
     forall(p,ptlist) H.new_node(p);
     node_array<rat_vector> pos(H);
     node v;
     forall_nodes(v,H) pos[v] = H[v].to_vector();
     anim.init(pos);
     anim.draw();
     new_input = false;
   }

   but = anim.move();
 }


 return 0;
}

