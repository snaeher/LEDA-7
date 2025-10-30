/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  polygon_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//#define TESTPHASE

#ifndef TESTPHASE

#include <LEDA/geo/polygon.h>
#include <LEDA/geo/gen_polygon.h>

#else

static int sign(int x)
{ if ( x == 0 ) return 0;
  if ( x > 0  ) return 1;
  return -1;
}

static int P0_face     = 1;
static int not_P0_face = 2;
static int P1_face     = 4;
static int not_P1_face = 8;

static bool False(string s)
{ cerr << s; return false; }


#include <LEDA/geo/plane_alg.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/map.h>
#include <math.h>
#include <assert.h>
#include <LEDA/core/string.h>

#include <LEDA/geo/gen_polygon.h>
#include <LEDA/geo/float_kernel.h>

#include <LEDA/geo/float_kernel_names.h>
#include "gen_polygon.c"
#include <LEDA/geo/kernel_names_undef.h>


#include <LEDA/geo/plane_alg.h>
#include <LEDA/grpahs/graph.h>
#include <LEDA/core/map.h>
#include <math.h>
#include <assert.h>

#include <LEDA/geo/float_kernel.h>
#include <LEDA/geo/polygon.h>

#include <LEDA/geo/float_kernel_names.h>
#include "polygon.c"
#include <LEDA/geo/kernel_names_undef.h>



#endif

#include <LEDA/geo/plane.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/button32.h>


using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::cerr;
using std::endl;
#endif


static window W(520,580,"LEDA Polygon Demo");
static int window_width;
static int window_height;

static gen_polygon P;
static polygon Q;  static gen_polygon QG;

//static list<polygon> result;  static gen_polygon R;

static color pcol1 = grey1;
static color pcol2 = grey2;

static int grid_mode = 0;

static void message(string s) 
{ W.set_status_string(s + "\\c "); }


static void draw_poly(const polygon& P,color pcol)
{ if (P.area() > 0) W.draw_filled_polygon(P,pcol);
  else 
  { W.draw_box(W.xmin(),W.ymin(),W.xmax(),W.ymax(),pcol);
    W.draw_filled_polygon(P,white);
  }
       
  W.draw_oriented_polygon(P,black);
}
  
static bool draw_poly(const gen_polygon& P)
{ W.draw_filled_polygon(P,grey1);
  W.draw_oriented_polygon(P,black);
  
  return true;
}


polygon hilbert(int n) 
{
  double x1, y1, x2, y2;

  int gmode = W.get_grid_mode();

  if (gmode > 0)
     { int d = int((W.xmax() - W.xmin())/gmode) - 3;
       x1 = W.xmin() + gmode;   
       y1 = W.ymin() + gmode;
       x2 = W.xmin() + d*gmode;
       y2 = W.ymin() + d*gmode;
      }
  else
     { x1 = W.xmin() + 50;   
       y1 = W.ymin() + 50;
       x2 = W.xmax() - 100;
       y2 = W.ymax() - 100;
     }
 
  message("Computing Hilbert Curve");

  return hilbert(n,x1,y1,x2,y2);
}


void select_poly(int i)
{ W.clear();
  if (i == 0) return;
  if ( i <= 6 ) { P = hilbert(i); return; }

  double x1 = W.xmin() + 50;   
  double y1 = W.ymin() + 50;
  double x2 = W.xmax() - 100;
  double y2 = W.ymax() - 100;
  circle C((x1 + x2)/2,(y1 + y2)/2, (y2 - y1)/2);
  P = n_gon(i,C,0.01);
}
  

void setgrid(int d) { grid_mode = d; }


void redraw(window* wp, double x0, double y0, double x1, double y1)
{ 
  list<point> clip_P;
  clip_P.append(point(x0,y0));
  clip_P.append(point(x1,y0));
  clip_P.append(point(x1,y1));
  clip_P.append(point(x0,y1));
  gen_polygon Q = P.intersection(gen_polygon(clip_P));
  wp->draw_filled_polygon(Q,pcol1);
  wp->draw_oriented_polygon(Q,black);
    
   if (wp->width() != window_width || wp->height() != window_height)
  { window_width = wp->width();
    window_height = wp->height();
  }
}


polygon  mouse_polygon(window& W)
{ W.disable_panel();
  list<point> L = W.read_polygon();
  bool cancel = false;
  while (!cancel && !Is_Simple_Polygon(L))
  { panel msg("Error");
    msg.text_item("");
    msg.text_item("Polygon is \\red not simple\\black .");
    msg.text_item("");
    msg.button("again",0);
    msg.button("cancel",1);
    if (msg.open(W) == 1) 
    { L.clear();
      cancel = true;
     }
    else
      L = W.read_polygon();
   }
  W.enable_panel();
  return polygon(L,polygon::WEAKLY_SIMPLE,polygon::RESPECT_ORIENTATION);   
}


void display_result(string msg)
{ W.draw_filled_polygon(P,red);
  W.draw_oriented_polygon(P,black);

  int n = 0;
  int h = 0;
  double A = 0;

  if (!P.trivial()) {
    polygon p;
    forall(p,P.polygons()) 
      if ( p.orientation() > 0 ) n++; 
      else  h++;
       
    A = P.area();
  }

  string p_string = "polygon";
  if ( n != 1 ) p_string += "s";
  string h_string = "hole";
  if ( h != 1 ) h_string += "s";

  string display_string = string("\\bf %s\\rm ~~~%d " + p_string + " ~~~%d " + h_string, ~msg,n,h);
  if (!P.full()) display_string += string(" ~~~area = %.2f", A);
  else           display_string += " ~~~area = INFINITY";

  message(display_string);  
}





int main()
{ 
  int max_c = 1000;

  panel help_panel("Help Panel");
  help_panel.set_panel_bg_color(ivory);

  help_panel.text_item("\
  This program demonstrates generalized polygons. It manipulates a \
  \\bf generalized polygon P. \\rm P is initially empty. The draw \
  button allows to give P a new value and the select button allows to \
  select P from a set of predefined polygons. P is updated by boolean \
  operations. The binary boolean operations operate on P and a polygon \
  Q. Q must be input after selecting the boolean operation. The side-of \
  button performs the side-of test on P for a set of random points. \
  Points in P are shown red and points outside P are shown blue. ");
  help_panel.button("continue");



  menu poly_menu;
  poly_menu.button("hilbert(2) ",2,select_poly);
  poly_menu.button("hilbert(3) ",3,select_poly);
  poly_menu.button("hilbert(4) ",4,select_poly);
  poly_menu.button("hilbert(5) ",5,select_poly);
  poly_menu.button("hilbert(6) ",6,select_poly);
  poly_menu.button("n-gon(10) ",10,select_poly);
  poly_menu.button("n-gon(20) ",20,select_poly);
  poly_menu.button("n-gon(40) ",40,select_poly);
  poly_menu.button("n-gon(80) ",80,select_poly);


  poly_menu.button("current poly",0,select_poly);

  menu grid_menu;
  grid_menu.button("no grid", 0,setgrid);
  grid_menu.button("dist 10",10,setgrid);
  grid_menu.button("dist 20",20,setgrid);
  grid_menu.button("dist 30",30,setgrid);
  grid_menu.button("dist 40",40,setgrid);
  grid_menu.button("dist 50",50,setgrid);

  menu op_menu;

  char* poly_pr    = W.create_pixrect(pen_xpm);
  char* hilb_pr    = W.create_pixrect(maze_xpm);
  char* grid_pr    = W.create_pixrect(grid_xpm);
  char* inter_pr   = W.create_pixrect(intersect_xpm);
  char* union_pr   = W.create_pixrect(puzzle_xpm);
  char* diff_pr    = W.create_pixrect(scissors_xpm);
  char* sdiff_pr   = W.create_pixrect(intersect2_xpm);
  char* compl_pr   = W.create_pixrect(complement_xpm);
  char* side_of_pr = W.create_pixrect(watch_xpm);
  char* simple_pr  = W.create_pixrect(blocks_xpm);
  char* help_pr    = W.create_pixrect(help_xpm);
  char* exit_pr    = W.create_pixrect(door_exit_xpm);


  W.button(poly_pr,    poly_pr,    "draw a polygon",   1);  
  W.button(hilb_pr,    hilb_pr,    "list of polygons", 2, poly_menu);  
  W.button(grid_pr,    grid_pr,    "define a grid",   99, grid_menu);
  W.button(inter_pr,   inter_pr,   "intersect",        3);
  W.button(union_pr,   union_pr,   "union",            4);
  W.button(diff_pr,    diff_pr,    "diff",             5);
  W.button(sdiff_pr,   sdiff_pr,   "symdiff",          6);
  W.button(compl_pr,   compl_pr,   "complement",      10);
  W.button(side_of_pr, side_of_pr, "side_of ",         7);
  W.button(simple_pr,  simple_pr,  "weakly simple",   66);
  W.button(help_pr,    help_pr,    "help",             8);
  W.button(exit_pr,    exit_pr,    "exit",             0);

  W.make_menu_bar();


  W.set_redraw(redraw);
  W.set_node_width(2);  
//W.set_line_width(2);
  W.init(0,max_c,0,grid_mode);
  W.display(window::center,window::center);
  W.open_status_window();

  help_panel.open(W);
  W.clear();

  window_width = W.width();
  window_height = W.height();


  int but;
  while ((but = W.read_mouse()) != 0)
  {
     if (but > 2) 
        W.init(0,max_c,0,grid_mode); // clears window



    switch (but)  {

    case 1: { W.clear();
             message("Please draw a \\bf simple polygon P.");
              
              P = gen_polygon(mouse_polygon(W),gen_polygon::NO_CHECK,gen_polygon::RESPECT_ORIENTATION);
              draw_poly(P);
              message(string("area(P) = %.2f",P.area()));
              break;
             }

 
    case 2: { draw_poly(P);
              message(string("area(P) = %.2f",P.area()));
              break;
             }

    case 3: { draw_poly(P);
              message("\\bf Intersection:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              if (Q.empty()) 
              { message(string("area(P) = %.2f",P.area()));
                break;
               }
              draw_poly(Q,pcol2);
            
              message("Computing Intersection");
              P = P.intersection(Q);
            
              display_result("P.intersect(Q)");
              break;
             }

    case 4: { draw_poly(P);
              message("\\bf Union:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              if (Q.empty()) 
              { message(string("area(P) = %.2f",P.area()));
                break;
               }
              draw_poly(Q,pcol2);
            
              message("Computing Union");
          
              P = P.unite(Q);
              display_result("P.unite(Q)");
              break;
             }
 
    case 5: { draw_poly(P);
              message("\\bf Diff:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              if (Q.empty()) 
              { message(string("area(P) = %.2f",P.area()));
                break;
               }
              draw_poly(Q,pcol2);
            
              message("Computing Difference");
              P = P.diff(Q);
              display_result("P.diff(Q)");
              break;
             }
 
    case 6: { draw_poly(P);
              message("\\bf Symdiff:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              if (Q.empty()) 
              { message(string("area(P) = %.2f",P.area()));
                break;
               }
              draw_poly(Q,pcol2);
           
              message("Computing Symmetric Difference");
          
              P = P.sym_diff(Q);
              display_result("P.sym_diff(Q)");
              break;
             }

   case 10: { draw_poly(P);
              message("\\bf Complement");
              
              message("Computing Complement");
     
              P = P.complement();
              display_result("P.complement()");
              break;
             }



    case 66: {
              message("\\bf Make Weakly Simple:\\rm Draw a non-simple polygon Q.");
              W.disable_panel();
              list<point> L = W.read_polygon();
              W.enable_panel();
              Q = polygon(L,polygon::NO_CHECK);
              W.draw_oriented_polygon(Q,black);
              message("Computing Weakly Simple Parts");
              P = Q.make_weakly_simple(false, false);
              display_result("P.make_weakly_simple()");
              break;
             }
 
 
    case 7: { draw_poly(P);

              int gmode = W.get_grid_mode();

              if (gmode > 0)
              { for(double x = W.xmin(); x < W.xmax(); x += gmode)
                   for(double y = W.ymin(); y < W.ymax(); y += gmode)
                   { point p(x,y);
                     if (P.side_of(p) > 0) 
                        W.draw_filled_node(p,red);
                     else
                         W.draw_filled_node(p,blue);
                    }
                 break;
               }

              int i = 0;
              while (i < 4000)
              {  i++;
                 if (i % 10 == 0)
                   message(string("\\bf Side Of Test:\\rm %5d points.",i));

                if (W.get_button() != NO_BUTTON) break;

                int x0 = (int)W.xmin();
                int x1 = (int)W.xmax();
                int y0 = (int)W.ymin();
                int y1 = (int)W.ymax();

                point p(rand_int(x0,x1),rand_int(y0,y1));

                if (P.side_of(p) > 0) 
                   W.draw_filled_node(p,red);
                else
                   W.draw_filled_node(p,blue);
               }
              message(string("%5d  random points",i));
              break;
             }

    case 8: { help_panel.open(W);
              if (P.size() > 0) 
              { W.clear();
                draw_poly(P);
               }
              break;
             }


    } // switch

  } // while

 return 0;

}
