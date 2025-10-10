/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  delaunay_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/geo/rat_point_set.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;



static window W(560,660,"DYNAMIC DELAUNAY AND VORONOI DIAGRAMS OF POINTS");


enum { InsertMode, DeleteMode, LocateMode, NearestMode, RangeMode};
enum { Point, Segment, Circle, Triangle, Rectangle};

static int points_on_seg    = 20;
static int points_on_circle = 20;

static int display = 1;
static int input   = Point;
static int mode    = InsertMode;

static bool incremental = false;


static rat_point_set T;


void draw_node(const rat_point& p) 
{ W.draw_filled_node(p.to_point(),black); }

void highlight_node(const rat_point& p) 
{ // precond: xor mode
  int nw = W.get_node_width();
  draw_node(p);
  W.set_node_width(nw+2);
  W.draw_filled_node(p.to_point(),red); 
  W.set_node_width(nw);
}

void unhighlight_node(const rat_point& p) 
{ // precond: xor mode
  int nw = W.get_node_width();
  W.set_node_width(nw+2);
  W.draw_filled_node(p.to_point(),red); 
  W.set_node_width(nw);
  draw_node(p);
}



void draw_voro_edge(const rat_point& p, const rat_point& q) 
{ W.draw_segment(p.to_point(),q.to_point(),blue); }

void draw_voro_ray(const rat_point& p, const rat_point& q) 
{ W.draw_ray(p.to_point(),q.to_point(),blue); }

void draw_triang_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),yellow); }

void draw_hull_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),red); }

void draw_diagram_edge(const rat_point& p, const rat_point& q) 
{ W.draw_edge(p.to_point(),q.to_point(),green2); }

void draw_poly(const list<rat_point>& L)
{ list<point> P;
  rat_point p;
  forall(p,L) P.append(p.to_point());
  // W.draw_filled_polygon(P,yellow);
  // W.draw_polygon(P,black); 
  W.draw_polygon(P,red);
}


void show_nodes(const list<node>& L)
{ node v;
  forall(v,L)
    highlight_node(T.pos(v));
  int val; double x; double y;
  while (W.read_event(val,x,y) != button_release_event);
  forall(v,L)
    unhighlight_node(T.pos(v));
}


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
           if (W >> p) W.draw_node(p);
           integer x = p.xcoord();
           integer y = p.ycoord();
           T.insert(rat_point(x,y,1));
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

   case Triangle:
   case Rectangle:
        { polygon P;
          if (W >> P) 
          { list<segment> L = P.edges();
            segment s;
            forall(s,L) segment_points(s,int(s.length()/4));
           }
          break;
         }
  }
}


void range_search(int range)
{ point p;
  W >> p; 
 
   switch(range) {

   case Circle:
        { point q;
  	  W.read_mouse_circle(p,q); 

          rat_point pr((int) p.xcoord(),(int) p.ycoord(),1);
          rat_point qr((int) q.xcoord(),(int) q.ycoord(),1);
          rat_point qr90 = qr.rotate90(pr);
          rat_point qr180 = qr90.rotate90(pr);
          rat_circle C(qr,qr90,qr180);
  
          circle c(qr.to_point(),qr90.to_point(),qr180.to_point());

          W.set_mode(xor_mode);
          W.draw_circle(c);
          show_nodes(T.range_search(C));
          W.draw_circle(c);
          W.set_mode(src_mode);

          break;
         }


   case Triangle:
        { point q,r;
          W.read_mouse_seg(p,q);
          W.set_mode(xor_mode);
          W.draw_segment(p,q); 
          W.set_mode(src_mode);
          W.read_mouse_seg(q,r);

          rat_point pr((int) p.xcoord(),(int) p.ycoord(),1);
          rat_point qr((int) q.xcoord(),(int) q.ycoord(),1);
          rat_point rr((int) r.xcoord(),(int) r.ycoord(),1);
          list<node> L = T.range_search(pr,qr,rr);
          W.set_mode(xor_mode);
          W.draw_segment(p,r); 
          W.draw_segment(q,r);
          show_nodes(L);
          W.draw_segment(p,q); 
          W.draw_segment(p,r); 
          W.draw_segment(q,r);
          W.set_mode(src_mode);
          break;
       }

   case Rectangle:
        { point q;  
          W.read_mouse_rect(p,q);

          rat_point pr((int) p.xcoord(),(int) p.ycoord(),1);
          rat_point qr((int) q.xcoord(),(int) q.ycoord(),1);

          list<node> L = T.range_search(pr,qr);

          W.set_mode(xor_mode);
          W.draw_rectangle(p,q);
          show_nodes(L);
          W.draw_rectangle(p,q);
          W.set_mode(src_mode);
          break;
       }

   }
}


void draw_nodes()
{ T.draw_nodes(draw_node); }

void draw_edges(int disp)
{
  string label;

  if (disp & 1)
    { if (label != "") label += " + ";
      label += " Delaunay Diagram ";
      T.draw_edges(draw_diagram_edge,draw_triang_edge,draw_hull_edge);
     }

  if (disp & 2)
    { if (label != "") label += " + ";
      label += " Voronoi Diagram ";
      T.draw_voro_edges(draw_voro_edge,draw_voro_ray);
     }

  if (disp & 4)
    { if (label != "") label += " + ";
      label += " Min Spanning Tree ";
      T.draw_edges(T.minimum_spanning_tree(),draw_diagram_edge);  
     }

  if (disp & 8) 
    { if (label != "") label += " + ";
      label += " Convex Hull ";
      if ((disp & 1) == 0) T.draw_hull(draw_poly);
     }

  W.set_frame_label(label);
}



void draw(int disp) 
{ W.start_buffering();
  W.clear();
  draw_nodes();
  draw_edges(disp);
  W.flush_buffer();
  W.stop_buffering();
}

void redraw() { draw(display); }


void lattice_points(int n)
{
  double dx = W.xmax() - W.xmin();
  double dy = W.ymax() - W.ymin();
  double d  = leda_min(dx,dy)*(1.0-2.0/10.0); // 10% border
 
  double xmin = W.xmin()+(dx-d)/2.0;
  double ymin = W.ymin()+(dy-d)/2.0;
//double xmax = xmin+d;
//double ymax = ymin+d;
 
  if (n < 2) return;
 
  W.clear();
  //W.set_mode(xor_mode);

  if (incremental) draw_nodes();

  list<rat_point> L;
 
  double s = d/(n-1);
  for (int i=0; i < n; i++) {
    double x = xmin + i*s;
    for (int j = 0; j < n; j++) {
      double y = ymin + j*s;
      rat_point p(int(100*x),int(100*y),100);
      switch (mode) {
  
      case InsertMode: { draw_node(p);
                         if (incremental)
                            T.insert(p);
                         else
                            L.append(p);
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

  if (mode == InsertMode && !incremental) T.init(L);
 
  //W.set_mode(src_mode);
}
 


void random_points(int n)
{ 
  int dx = int(1+(W.xmax() - W.xmin())/10);
  int dy = int(1+(W.ymax() - W.ymin())/10);

  int xmin = int(W.xmin()+dx);
  int xmax = int(W.xmax()-dx);
  int ymin = int(W.ymin()+dy);
  int ymax = int(W.ymax()-dy);

  W.clear();
  //W.set_mode(xor_mode);

  if (incremental) draw_nodes();

  list<rat_point> L;

  while (n--)
  { W.del_messages();
    W.message(string(" %3d",n));
    integer x = rand_int(xmin,xmax);
    integer y = rand_int(ymin,ymax);
    rat_point p(x,y);

    switch (mode) {

    case InsertMode: { draw_node(p);
                       if (incremental)
                          T.insert(p);
                       else
                          L.append(p);
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

  if (mode == InsertMode && !incremental) T.init(L);

 //W.set_mode(src_mode);
 W.del_messages();
}
   






void setgrid(int d)
{ W.set_grid_mode(d);
  draw(display);
 }

edge find_edge(const rat_point& p, const rat_point& q)
{ edge result = nil;
  edge e;
  forall_edges(e,T)
    if (T.pos_source(e) == p && T.pos_target(e) == q) result = e;
  return result;
}


int main()
{
  //bool verify = false;

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


  list<string> disp_choices;
  disp_choices.append("Delaunay");
  disp_choices.append("Voronoi");
  disp_choices.append("MS_Tree");
  disp_choices.append("ConvHull");


  W.choice_item("mode", mode, "insert","delete","locate","nearest","range");
  W.choice_item("input",input,"point","segment","circle","triang","rect");
  W.choice_mult_item("display",display,disp_choices,draw);
  //W.bool_item("verify",verify);

  W.button("grid",   19, grid_menu);
  W.button("rand",    1, rand_menu);
  W.button("lattice", 4, lattice_menu);
  W.button("clear",   2);
//W.button("error",   3);
  W.button("GraphWin", 5);
  W.button("quit",     0);

  W.display();

  W.init(0,256,0);
  W.set_redraw(redraw);
  W.set_node_width(2);
  W.set_show_coordinates(true);

  draw(display);

  for(;;)
  {
    double x,y;
    int but = W.read_mouse(x,y);
  
/*
     if (verify) 
       T.set_verify();
     else
       T.unset_verify();
*/

    if (but == 0) break;
  
    switch (but) {

    case MOUSE_BUTTON(1):
            { 
              switch (mode) {

              case InsertMode: put_back_event();
                               get_input(W,input);
                               draw(display);
                               break;

              case LocateMode:
              case NearestMode:
              case DeleteMode:
                            { rat_point p = rat_point(integer(x),integer(y));
                              node v = T.nearest_neighbor(p);
                              if (v)
                              { W.set_mode(xor_mode);
                                show_nodes((list<node>)v);
                                W.set_mode(src_mode);
                                if (mode == DeleteMode) 
                                { T.del(v);
                                  draw(display);
                                 }
                               }
                              break;
                             }

              case RangeMode: put_back_event();
                              range_search(input);
                              break;
              }
              break;
             }

    case 1: // random points
    case 4: // lattice points
             //draw_edges(display);
             draw(display);
             break;
   

    case 2: { // clear
              T.clear();
              W.clear();
              break;
             }


    case 5: { // start GraphWin

              GRAPH<rat_circle,rat_point> VD;
              graph* g = &T;
              if (display == 2) // Voronoi
              { T.compute_voronoi(VD);
                g = &VD;
               }

              //GraphWin gw(*g,W);

              GraphWin gw(*g);
              gw.set_directed(true);

              if (g->number_of_nodes() > 100) gw.set_flush(false);

              node v;
              forall_nodes(v,*g) 
              { rat_point p;
                if (g == &VD)
                {  rat_circle c = VD.inf(v);
                   if (c.orientation() != 0) 
                       p = c.center();
                   else
                    { rat_point a = c.point1();
                      rat_point b = c.point2();
                      rat_point c = center(a,b);
                      p = b.rotate90(a);
                     }
                   
                 }
                else p = T.pos(v);
                //gw.set_label_pos(v,northwest_pos);
                gw.set_position(v,p.to_point());
                gw.set_label_type(v,no_label);
                gw.set_width(v,8);
                gw.set_height(v,8);
              }

              edge e;
              forall_edges(e,T)
              { switch (T[e]) {
                 case DIAGRAM_EDGE:      
                      gw.set_color(e,green2);
                      break;
                 case NON_DIAGRAM_EDGE:  
                      gw.set_color(e,yellow);
                      break;
                 case HULL_EDGE:         
                      gw.set_color(e,red);
                      break;
                }
                //if (e == T.get_cur_edge()) gw.set_width(e,2);
              }

              W.disable_panel();

              gw.place_into_win();
              gw.display(0,0);
              //gw.fill_window();
              gw.edit();

              W.enable_panel();

              draw(display);
             }

    }

  }

  rat_point::print_statistics();
  return 0;
}
