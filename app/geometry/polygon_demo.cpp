/*******************************************************************************
+
+  LEDA 7.2  
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


#include <LEDA/geo/rat_polygon.h>
#include <LEDA/geo/rat_gen_polygon.h>

#include <LEDA/geo/plane.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/geo/logo.h>

#include <LEDA/graphics/window.h>
#include <LEDA/graphics/pixmaps/button32.h>

using namespace leda;


static window W("LEDA Polygon Demo");

static int window_width;
static int window_height;

static rat_gen_polygon P;
static rat_polygon Q;

static color pcol1 = grey1;
static color pcol2 = grey2;


static int grid_mode = 0;

static void message(string s) { 
//W.set_status_string(s + "\\c "); 
  W.set_status_string("   " + s); 
}


void coord_handler(window* win, double x, double y) 
{
  BASE_WINDOW* swin = win->get_status_window();
  if (swin == 0 || !swin->is_open()) return;

//double x0 = swin->xmin();
  double y0 = swin->ymin();
  double x1 = swin->xmax();
  double y1 = swin->ymax();

  string coord_str("%7.2f %7.2f",x,y);

  double pix = swin->pix_to_real(1);
  double cx = x1 - swin->text_width(coord_str) - 10*pix;
  double cy = (y1 + y0 + swin->text_height(coord_str))/2;

  cy -= pix;

  swin->clear(cx,y0+pix,x1,y1-pix);
  swin->draw_text(cx,cy,coord_str);

  // status windows are always buffering 
  swin->flush_buffer(cx,y0+pix,x1,y1-pix);
}
 


static void draw_poly(const rat_polygon& P,color pcol)
{ 
  if (P.to_float().area() > 0) 
     W.draw_filled_polygon(P.to_float(),pcol);
  else 
  { W.draw_box(W.xmin(),W.ymin(),W.xmax(),W.ymax(),pcol);
    W.draw_filled_polygon(P.to_float(),white);
  }
       
  //W.draw_oriented_polygon(P.to_float(),black);
  W.draw_polygon(P.to_float(),black);
}
  
static bool draw_poly(const rat_gen_polygon& P)
{ W.draw_filled_polygon(P.to_float(),grey1);
  //W.draw_oriented_polygon(P.to_float(),black);
  W.draw_polygon(P.to_float(),black);
  message(string("size: %d  area: %.2f",P.size(), P.to_float().area()));
  return true;
}


rat_polygon hilbert(int n) 
{
  double x1, y1, x2, y2;

  int gmode = W.get_grid_mode();

  double dx = (W.xmax() - W.xmin())/20;
  double dy = (W.ymax() - W.ymin())/20;

  if (gmode > 0)
     { int d = int((W.xmax() - W.xmin())/gmode) - 3;
       x1 = W.xmin() + gmode;   
       y1 = W.ymin() + gmode;
       x2 = W.xmin() + d*gmode;
       y2 = W.ymin() + d*gmode;
      }
  else
     { x1 = W.xmin() + 1.1*dx;   
       y1 = W.ymin() + 1.2*dy;
       x2 = W.xmax() - 0.9*dx;
       y2 = W.ymax() - 0.8*dy;
     }
 
  message("Computing Hilbert Curve");

  return rat_polygon(hilbert(n,x1,y1,x2,y2));
}



void create_poly()
{ 
  int n = 50;

  panel pan;
  pan.text_item("\\bf Polygon Generator");
  pan.text_item("");

  pan.int_item("# vertices",n,3,10000);

  pan.button("hilbert",1);
  pan.button("logo",2);
  pan.button("n-gon",3);
  pan.button("cancel",0);

  int but = pan.open(W);

  if (but == 0)  return;
  
  if (but == 1)
  { int i = 0;
    while (n > 0) { i++; n /= 4; }
    P = hilbert(i); 
    return; 
   }

  if (but == 2)
  { P = leda_logo(200).translate(500,500); 
    return; 
   }

  if (but == 3)
  { double x1 = W.xmin() + 50;   
    double y1 = W.ymin() + 50;
    double x2 = W.xmax() - 100;
    double y2 = W.ymax() - 100;
    circle C((x1 + x2)/2,(y1 + y2)/2, (y2 - y1)/2);
    P = gen_polygon(n_gon(n,C,0.01));
    return;
   }
}
  

void setgrid(int d) { grid_mode = d; }


void redraw1(window* wp, double x0, double y0, double x1, double y1)
{ 
  list<point> clip_P;
  clip_P.append(point(x0,y0));
  clip_P.append(point(x1,y0));
  clip_P.append(point(x1,y1));
  clip_P.append(point(x0,y1));
  gen_polygon Q = P.to_float().intersection(gen_polygon(clip_P));
  wp->draw_filled_polygon(Q,pcol1);

  //wp->draw_oriented_polygon(Q,black);
  wp->draw_polygon(Q,black);
    
   if (wp->width() != window_width || wp->height() != window_height)
  { window_width = wp->width();
    window_height = wp->height();
  }
}


void redraw(window* wp)
{ 
  window& W = *wp;

  gen_polygon Q = P.to_float();

  W.start_buffering();
  W.draw_filled_polygon(Q,pcol1);
//W.draw_oriented_polygon(Q,black);
  W.draw_polygon(Q,black);
  W.flush_buffer();
  W.stop_buffering();
    
   if (W.width() != window_width || W.height() != window_height)
  { window_width = W.width();
    window_height = W.height();
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
{ W.draw_filled_polygon(P.to_float(),red);
  //W.draw_oriented_polygon(P.to_float(),black);
  W.draw_polygon(P.to_float(),black);

  int n = 0;
  int h = 0;
  double A = 0;

  if (!P.trivial()) {
    rat_polygon p;
    forall(p,P.polygons()) 
      if ( p.orientation() > 0 ) n++; 
      else  h++;
       
    A = P.to_float().area();
  }

  string p_string = "polygon";
  if ( n != 1 ) p_string += "s";
  string h_string = "hole";
  if ( h != 1 ) h_string += "s";

  string display_string = string("\\bf ~~~%s\\rm ~~~%d " + p_string + " ~~~%d " + h_string, ~msg,n,h);
  if (!P.full()) display_string += string(" ~~~area = %.2f", A);
  else           display_string += " ~~~area = INFINITY";

  message(display_string);  
}





int main()
{ 
  int max_c = 1000;

  panel help_panel(700,-1,"Help Panel");
  //help_panel.set_panel_bg_color(ivory);

  help_panel.text_item("\\bf Polygon Demo\\n");
  help_panel.text_item("\
  This program demonstrates LEDA's generalized polygons. It manipulates a \
  \\bf generalized polygon P. \\rm P is initially empty. The draw \
  button allows to give P a new value and the select button allows to \
  select P from a set of predefined polygons. P is updated by boolean \
  operations. The binary boolean operations operate on P and a polygon \
  Q. Q must be input after selecting the boolean operation. The side-of \
  button performs the side-of test on P for a set of random points. \
  Points in P are shown red and points outside P are shown blue. ");

  help_panel.text_item("");
  help_panel.button("continue");



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


  W.button(poly_pr,    poly_pr,    "draw a polygon",       1);  
  W.button(hilb_pr,    hilb_pr,    "create a polygon",     2);  
  W.hspace(50);
  W.button(inter_pr,   inter_pr,   "intersection",         3);
  W.button(union_pr,   union_pr,   "union",                4);
  W.button(diff_pr,    diff_pr,    "difference",           5);
  W.button(sdiff_pr,   sdiff_pr,   "symmetric difference", 6);
  W.button(compl_pr,   compl_pr,   "complement",           7);
  W.button(side_of_pr, side_of_pr, "inside/outside",       8);
  W.button(simple_pr,  simple_pr,  "make weakly simple",   9);

  W.button(grid_pr,    grid_pr,    "settings", 99, grid_menu);
  W.button(help_pr,    help_pr,    "help",     10);
  W.button(exit_pr,    exit_pr,    "exit",     0);

  //W.make_menu_bar();
  W.make_menu_bar(10);


  W.set_redraw(redraw);
  W.set_show_coord_handler(coord_handler);

  W.set_node_width(2);  
  W.set_line_width(1);
  W.init(0,max_c,0,grid_mode);
  W.display(window::center,window::center);
  W.open_status_window(32,grey1);

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

    case 1: { message("Please draw a \\bf simple polygon P.");
              W.clear();
              
              P = rat_gen_polygon(mouse_polygon(W),
                                  rat_gen_polygon::NO_CHECK,
                                  rat_gen_polygon::RESPECT_ORIENTATION);
              draw_poly(P);
              break;
             }

 
    case 2: { W.clear();
              create_poly();
              message("drawing polygon P");
              draw_poly(P);
              break;
             }

    case 3: { draw_poly(P);
              message("\\bf ~~~Intersection:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              draw_poly(Q,pcol2);
              message("Computing Intersection");
              P = P.intersection(Q);
              display_result("P.intersect(Q)");
              break;
             }

    case 4: { draw_poly(P);
              message("\\bf ~~~Union:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              draw_poly(Q,pcol2);
              message("Computing Union");
              P = P.unite(Q);
              display_result("P.unite(Q)");
              break;
             }
 
    case 5: { draw_poly(P);
              message("\\bf ~~~Diff:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              draw_poly(Q,pcol2);
              message("Computing Difference");
              P = P.diff(Q);
              display_result("P.diff(Q)");
              break;
             }
 
    case 6: { draw_poly(P);
              message("\\bf ~~~Symdiff:\\rm Draw a second polygon Q.");
              Q = mouse_polygon(W);
              draw_poly(Q,pcol2);
              message("Computing Symmetric Difference");
              P = P.sym_diff(Q);
              display_result("P.sym_diff(Q)");
              break;
             }

   case 7: { draw_poly(P);
              message("\\bf ~~~Complement");
              
              message("Computing Complement");
     
              P = P.complement();
              display_result("P.complement()");
              break;
             }


 
    case 8: { draw_poly(P);

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
                   message(string("\\bf ~~~Side Of Test:\\rm %5d points.",i));

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


    case 9: {
              message("\\bf ~~~Make Weakly Simple:\\rm Draw a non-simple polygon Q.");
              W.disable_panel();
              list<point> L = W.read_polygon();
              W.enable_panel();

              //Q = polygon(L,polygon::NO_CHECK);

              list<rat_point> L1;
              point p;
              forall(p,L) L1.append(rat_point(p));
              Q = rat_polygon(L1,rat_polygon::NO_CHECK);

              //W.draw_oriented_polygon(Q.to_float(),black);
              W.draw_polygon(Q.to_float(),black);

              message("Computing Weakly Simple Parts");
              P = Q.make_weakly_simple(false, false);
              display_result("P.make_weakly_simple()");
              break;
             }
 


    case 10: { help_panel.open(W);
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
