/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delaunay_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/delaunay.h>
#include <LEDA/geo/plane_alg.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;



static window W(560,660,"DYNAMIC DELAUNAY AND VORONOI DIAGRAMS OF POINTS");


enum { InsertMode, DeleteMode };
enum { Point, Segment, Circle };

static points_on_seg    = 20;
static points_on_circle = 30;

static int input   = Point;
static int mode    = InsertMode;

static delaunay_triang T;


void draw_node(const rat_point& p) 
{ W.draw_filled_node(p.to_point(),blue2); }

void draw_triang_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),yellow); }

void draw_hull_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),red); }

void draw_diagram_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),green2); }


// Input

void segment_points(segment s, int n)
{ 
  integer x  = n*s.xcoord1();
  integer y  = n*s.ycoord1();
  integer dx = s.xcoord2() - s.xcoord1();
  integer dy = s.ycoord2() - s.ycoord1();

  for(int i=0; i < n; i++)
  { rat_point p(x,y,n);
    T.insert(p);
    draw_node(p);
    x += dx;
    y += dy;
   }
}


void circle_points(circle C, int n)
{ 
  point p = C.point1();
  point q = C.point2();
  point r = C.point3();

  int px = int(p.xcoord());
  int py = int(p.ycoord());
  int qx = int(q.xcoord());
  int qy = int(q.ycoord());
  int rx = int(r.xcoord());
  int ry = int(r.ycoord());

  rat_circle R(rat_point(px,py,1), rat_point(qx,qy,1), rat_point(rx,ry,1));

  double d = (2*LEDA_PI)/n;
  double eps = 0.001;
  
  double a = 0;
  for(int i=0; i < n; i++)
  { rat_point q = R.point_on_circle(a,eps);
    T.insert(q);
    draw_node(q);
    a += d;
  }
}


void get_input(window& W, int inp)
{
   switch (inp) {

    case Point: 
         { point p;
           if (W >> p) 
           { integer x = p.xcoord();
             integer y = p.ycoord();
             rat_point P(x,y,1);
             draw_node(P);
             T.insert(P);
            }
           break;
          }
  
   case Segment: 
        { segment s;
          if (W >> s) segment_points(s,points_on_seg); 
          break;
         }

   case Circle:
        { circle c;
          if (W >> c) circle_points(c,points_on_circle);
          break;
         }
  }
}



void lattice_points(int n)
{
  double dx = W.xmax() - W.xmin();
  double dy = W.ymax() - W.ymin();
  double d  = leda_min(dx,dy)*(1.0-2.0/10.0); // 10% border
 
  double xmin = W.xmin()+(dx-d)/2.0;
  double xmax = xmin+d;
  double ymin = W.ymin()+(dy-d)/2.0;
  double ymax = ymin+d;
 
  if (n < 2) return;
 
  list<rat_point> L;
 
  double s = d/(n-1);
  for (int i=0; i < n; i++) {
    double x = xmin + i*s;
    for (int j = 0; j < n; j++) {
      double y = ymin + j*s;
      rat_point p(int(100*x),int(100*y),100);
      switch (mode) {
  
      case InsertMode: { draw_node(p);
                         T.insert(p);
                         break;
                        }
  
      case DeleteMode: { node v = T.nearest_neighbor(p);
                         if (v != nil && mode == DeleteMode)
                         { draw_node(T.pos(v));
                           T.del(v);
                          }
                         break;
                        }
      }
    }
  }

}
 


void random_points(int n)
{ 
  int dx = int(1+(W.xmax() - W.xmin())/10);
  int dy = int(1+(W.ymax() - W.ymin())/10);

  int xmin = int(W.xmin()+dx);
  int xmax = int(W.xmax()-dx);
  int ymin = int(W.ymin()+dy);
  int ymax = int(W.ymax()-dy);

  list<rat_point> L;

  while (n--)
  { W.del_messages();
    W.message(string(" %3d",n));
    integer x = rand_int(xmin,xmax);
    integer y = rand_int(ymin,ymax);
    rat_point p(x,y);

    switch (mode) {

    case InsertMode: { draw_node(p);
                       T.insert(p);
                       break;
                      }

    case DeleteMode: { node v = T.nearest_neighbor(p);
                       if (v != nil && mode == DeleteMode)
                       { draw_node(T.pos(v));
                         T.del(v);
                        }
                       break;
                      }
    }
  }


 W.set_mode(src_mode);
 W.del_messages();
}
   



void redraw()
{ T.draw_nodes(draw_node);
  T.draw_edges(draw_diagram_edge,draw_triang_edge,draw_hull_edge);
 }

void draw(int=0) 
{ W.start_buffering();
  W.clear();
  redraw(); 
  W.flush_buffer();
  W.stop_buffering();
}

void setgrid(int d)
{ W.set_grid_mode(d);
  redraw();
 }


void pre_move_edge_handler(edge e, node v, node w) 
{ 
  draw();

/*
  int val;
  double x,y;
  while (W.read_event(val,x,y) != button_release_event);
*/

  if (index(source(e)) > index(target(e))) return;

/*
  W.set_mode(xor_mode);
  T.draw_edge(e,draw_diagram_edge,draw_triang_edge,draw_hull_edge);
*/

  point a = T.pos_source(e).to_point();
  point b = T.pos_target(e).to_point();
  point c = T.pos(v).to_point();
  point d = T.pos(w).to_point();

  list<point> L;
  L.append(a);
  L.append(b);
  L.append(c);
  L.append(d);

  point pl,pb,pr,pt;
  Bounding_Box(L,pl,pb,pr,pt);

  double xmin = pl.xcoord();
  double ymin = pb.ycoord();
  double xmax = pr.xcoord();
  double ymax = pt.ycoord();

  segment s1(a,c);
  segment s2(b,d);

  int n = int(5000/(s1.length() + s2.length()));

  double l1 = s1.length()/n;
  double l2 = s2.length()/n;
  double a1 = s1.angle(); 
  double a2 = s2.angle(); 


  W.draw_edge(a,b,white);

  char* pm = W.get_pixrect(xmin,ymin,xmax,ymax);

  W.start_buffering();
  
 
  while (n--)
  { a = a.translate_by_angle(a1,l1);
    b = b.translate_by_angle(a2,l2);
    W.put_pixrect(xmin,ymin,pm);
    W.draw_edge(a,b,black);
    W.flush_buffer(xmin,ymin,xmax,ymax);
   }

   W.stop_buffering();
}


void post_move_edge_handler(edge e, node v, node w) 
{ //if (index(source(e)) > index(target(e))) return;
  //T.draw_edge(e,draw_diagram_edge,draw_triang_edge,draw_hull_edge);
 }



void mark_edge_handler(edge e)
{ T.draw_edge(e,draw_diagram_edge,draw_triang_edge,draw_hull_edge); }




int main(int argc, char** argv)
{
  T.set_pre_move_edge_handler(pre_move_edge_handler);
  T.set_post_move_edge_handler(post_move_edge_handler);
  T.set_mark_edge_handler(mark_edge_handler);

  bool verify = false;

  menu grid_menu;
  grid_menu.button("no grid", 0,setgrid);
  grid_menu.button("dist  5", 5,setgrid);
  grid_menu.button("dist 10",10,setgrid);
  grid_menu.button("dist 15",15,setgrid);
  grid_menu.button("dist 20",20,setgrid);
  grid_menu.button("dist 25",25,setgrid);

  menu lattice_menu;
  lattice_menu.button(" 2x2 ", 2,lattice_points);
  lattice_menu.button(" 4x4 ", 4,lattice_points);
  lattice_menu.button(" 8x8 ", 8,lattice_points);
  lattice_menu.button("16x16",16,lattice_points);
  lattice_menu.button("32x32",32,lattice_points);


  menu rand_menu;
  rand_menu.button(" 125", 125,random_points);  
  rand_menu.button(" 250", 250,random_points);
  rand_menu.button(" 500", 500,random_points);
  rand_menu.button("1000",1000,random_points);
  rand_menu.button("2000",2000,random_points);
  rand_menu.button("4000",4000,random_points);
  rand_menu.button("8000",8000,random_points);


  //W.choice_item("mode", mode, "insert","delete");
  W.choice_item("input",input,"point","segment","circle");

  W.button("grid",   20, grid_menu);
  W.button("rand",   21, rand_menu);
  W.button("lattice",22, lattice_menu);
  W.button("clear",   2);
  W.button("graphwin",3);
  W.button("quit",    0);

  W.display();

  W.init(0,256,0);
  W.set_redraw(redraw);
  W.set_node_width(3);
  W.set_show_coordinates(true);


  draw();

  for(;;)
  {
    double x,y;
    int but = W.read_mouse(x,y);
  
    if (but == 0) break;

    switch (but) {

    case MOUSE_BUTTON(1):
            { 
              switch (mode) {

              case InsertMode: put_back_event();
                               get_input(W,input);
                               draw();
                               break;

              case DeleteMode:
                            { rat_point p = rat_point(integer(x),integer(y));
                              node v = T.nearest_neighbor(p);
                              if (v) T.del(v);
                              break;
                             }

              }
              break;
             }


    case 2: { // clear
              T.clear();
              W.clear();
              break;
             }

    case 3: { // start GraphWin
              GraphWin gw(T,W);
              gw.set_node_label_type(no_label);
              gw.set_node_shape(circle_node);
              gw.set_node_width(12);
              gw.set_node_color(blue2);
              gw.set_directed(true);
              node v;
              forall_nodes(v,T) 
                  gw.set_position(v,T.pos(v).to_point());
              gw.display();
              gw.wait();
              draw();
              W.set_redraw(redraw);
              break;
             }


    }

  }

  rat_point::print_statistics();
  return 0;
}
