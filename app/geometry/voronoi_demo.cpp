/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  voronoi_demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/rat_geo_alg.h>
#include <LEDA/numbers/rat_vector.h>
#include <LEDA/numbers/vector.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/bitmaps/button32.h>

#include <math.h>


using namespace leda;



static unsigned char* input_bits [] = {
  point_bits,
  line_bits,
  circle_bits
};

static unsigned char* display_bits [] = {
  triang_bits,
  voro_bits,
  f_triang_bits,
  f_voro_bits,
  tree_bits,
  hull_bits,
  empty_circle_bits,
  encl_circle_bits,
  w_annulus_bits,
  a_annulus_bits,
  help_bits
};


enum display_mask {
  DT_MASK   =   1,
  VD_MASK   =   2,
  FDT_MASK  =   4,
  FVD_MASK  =   8,
  MST_MASK  =  16,
  HULL_MASK =  32,
  LEC_MASK  =  64,
  SEC_MASK  = 128,
  MWA_MASK  = 256,
  MAA_MASK  = 512,
  CRUST_MASK  =  1024 
};


typedef rat_point   POINT;
typedef rat_segment SEGMENT;
typedef rat_circle  CIRCLE;
typedef rat_line    LINE;
typedef rat_vector  VECTOR;
typedef integer     COORD;

/*
typedef point   POINT;
typedef segment SEGMENT;
typedef circle  CIRCLE;
typedef line    LINE;
typedef vector  VECTOR;
typedef double  COORD;
*/


static list<POINT>         p_list;
static GRAPH<POINT,int>    T;
static GRAPH<POINT,int>    DT;
static GRAPH<POINT,int>    FDT;
static GRAPH<CIRCLE,POINT> VD;
static GRAPH<CIRCLE,POINT> FVD;
static GRAPH<POINT,int>    MST;


static window* Wp;

static color site_color     = red;
static color triang_color   = black;
static color voro_color     = blue;
static color f_triang_color = green;
static color f_voro_color   = violet;
static color hull_color     = grey1;
static color tree_color     = green2;

static int display = 2; // voronoi diagram
static int input = 0;

static int pix_dist = 16;
static int n_gen = 100; 
static int k_gen = 0; 


void clear_all()
{ //Wp->start_buffering();
  Wp->clear();
  //Wp->flush_buffer();
  //Wp->stop_buffering();
  p_list.clear();
}

void lattice_points(int n)
{ 
  n = (int)sqrt(double(n));

  double x0 = Wp->xmin();
  double y0 = Wp->ymin();
  double x1 = Wp->xmax();
  double y1 = Wp->ymax();

  double dx = x1 - x0;
  double dy = y1 - y0;
  double d  = 0.75 * leda_min(dx,dy);
 
  double xmin = x0+(dx-d)/2;
  double ymin = y0+(dy-d)/2;
//double xmax = x0+d;
//double ymax = y0+d;

  clear_all();
 
  if (n < 2) return;
 
  double s = d/(n-1);
  for (int i=0; i < n; i++)
  { double x = xmin + i*s;
    for (int j = 0; j < n; j++)
    { double y = ymin + j*s;
      POINT p(int(100*x),int(100*y),100);
      Wp->draw_filled_node(p.to_point(),site_color);
      p_list.append(p);
     }
   }
}


void random_circle(int n, int width)
{ 
  double x0 = Wp->xmin();
  double y0 = Wp->ymin();
  double x1 = Wp->xmax();
  double y1 = Wp->ymax();

  point cent((x0+x1)/2,(y0+y1)/2);

  int rmax = int(0.35 * (x1-x0));
  int rmin = int(rmax - Wp->pix_to_real(width));

  if (rmin < 0)  rmin = 0;

  clear_all();

  for(int i=0; i < n; i++)
  { double rad = rand_int(rmin,rmax);
    circle C(cent,rad);
    double a;
    rand_int >> a;
    point q = C.point_on_circle(2*a*LEDA_PI);
    int x = (int)q.xcoord();
    int y = (int)q.ycoord();
    p_list.append(POINT(x,y,1));
    Wp->draw_filled_node(x,y,site_color);
  }
}

 


void random_square(int n)
{
  double x0 = Wp->xmin();
  double y0 = Wp->ymin();
  double x1 = Wp->xmax();
  double y1 = Wp->ymax();

  double dx = x1 - x0;
  double dy = y1 - y0;
 
  int xmin = int(x0 + 0.1*dx);
  int xmax = int(x1 - 0.1*dx);
  int ymin = int(y0 + 0.1*dy);
  int ymax = int(y1 - 0.1*dy);

  clear_all();

  for(int i = 0; i<n; i++)
  { COORD x = rand_int(xmin,xmax);
    COORD y = rand_int(ymin,ymax);
    POINT p(x,y);
    Wp->draw_filled_node(p.to_point(),site_color);
    p_list.append(p);
   }
}
   


void insert_point(point p)
{ Wp->draw_filled_node(p,site_color);
  COORD x = p.xcoord();
  COORD y = p.ycoord();
  p_list.append(POINT(x,y,1));
 }

void insert_segment(segment s)
{ 
  double L = s.length();
  int n = Wp->real_to_pix(L)/pix_dist + 1;

  COORD x  = n*s.xcoord1();
  COORD y  = n*s.ycoord1();
  COORD dx = s.xcoord2() - s.xcoord1();
  COORD dy = s.ycoord2() - s.ycoord1();

  for(int i=0; i < n; i++)
  { POINT p(x,y,n);
    p_list.append(p);
      Wp->draw_filled_node(p.to_point(),site_color);
    x += dx;
    y += dy;
   }
}

void insert_circle(circle C)
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

  CIRCLE R(POINT(px,py,1), POINT(qx,qy,1), POINT(rx,ry,1));

  double L = 2*C.radius()*LEDA_PI;
  int n = Wp->real_to_pix(L)/pix_dist + 1;
  double d = (2*LEDA_PI)/n;
  double eps = 0.001;
  
  double a = 0;
  for(int i=0; i < n; i++)
  { POINT q = R.point_on_circle(a,eps);
    p_list.append(q);
    Wp->draw_filled_node(q.to_point(),site_color);
    a += d;
  }
}


void get_input(window& W, int inp)
{
   switch (inp) {
 
    case 0:
         { point p;
           if (W >> p) insert_point(p);
           break;
          }
 
   case 1:
        { segment s;
          if (W >> s) insert_segment(s);
          break;
         }
 
   case 2:
        { circle c;
          if (W >> c) insert_circle(c);
          break;
         }
   }
 
}

            



void draw_sites()
{ POINT p;
  forall(p,p_list) Wp->draw_filled_node(p.to_point(),site_color);
}


void draw_voro_edges(const GRAPH<CIRCLE,POINT>& VD, color col)
{
  edge_array<bool> drawn(VD,false);

  edge e;
  forall_edges(e,VD) 
  { 
    if (drawn[e]) continue;

    drawn[VD.reversal(e)] = drawn[e] = true;

    node v = source(e);
    node w = target(e);

    if (VD.outdeg(v) == 1 && VD.outdeg(w) == 1)
      { LINE l = p_bisector(VD[v].point1(),VD[v].point3());
        Wp->draw_line(l.to_line(),col);
       }
    else
      if (VD.outdeg(v) == 1) 
         { POINT  cw  = VD[w].center();
           VECTOR vec = VD[v].point3() - VD[v].point1();
           POINT  cv  = cw + vec.rotate90();
           Wp->draw_ray(cw.to_point(),cv.to_point(),col);
          }
       else
         if (VD.outdeg(w) == 1) 
          { POINT  cv  = VD[v].center();
            VECTOR vec = VD[w].point3() - VD[w].point1();
            POINT  cw  = cv + vec.rotate90();
            Wp->draw_ray(cv.to_point(),cw.to_point(),col);
           }
         else 
          { POINT  cv  = VD[v].center();
            POINT  cw  = VD[w].center();
            Wp->draw_segment(cv.to_point(),cw.to_point(),col);
         }
            
  }  

}


void draw_graph_edges(const GRAPH<POINT,int>& T, color col)
{ edge_array<bool> drawn(T,false);
  edge e;
  forall_edges(e,T) 
    if (!drawn[e])
    { drawn[e] = true;
      edge r = T.reversal(e);
      if (r) drawn[r] = true;
      POINT p = T[source(e)];
      POINT q = T[target(e)];
      Wp->draw_edge(p.to_point(),q.to_point(),col);
     }
}


void draw_graph_circles(const GRAPH<POINT,int>& T, color col)
{ edge_array<bool> drawn(T,false);
  edge e;
  forall_edges(e,T) 
  { if (drawn[e]) continue;
    drawn[e] = true;
    edge x = T.face_cycle_succ(e);
    POINT p = T[source(e)];
    POINT q = T[target(e)];
    POINT r = T[target(x)];
    if (orientation(p,q,r) > 0)
    { CIRCLE circ(p,q,r);
      Wp->draw_circle(circ.to_float(),col);
     }
   }
}


/*
#include <LEDA/map.h>

void  CRUST(list<POINT> L, GRAPH<POINT,int>& G) 
{
  map<POINT,bool> delaunay_point(false);

  GRAPH<CIRCLE,POINT> VD;

  VORONOI(L,VD);

// mark delaunay points and append them to list of all points

  node v;
  forall_nodes(v,VD)
  { if (VD.outdeg(v) < 2) continue; 
    
    delaunay_point[p] = true;
    L.append(p);
   }

  DELAUNAY_TRIANG(L,G);

  list<node> vlist;
  forall_nodes(v,G)
     if (delaunay_point[G[v]]) vlist.append(v);

  G.del_nodes(vlist);

  edge e;
  forall_edges(e,G) G[e] = DIAGRAM_EDGE;
   
}
*/


void draw_crust()
{ GRAPH<POINT,int>  G;
  CRUST(p_list,G);
  int save_lw = Wp->set_line_width(2);
  draw_graph_edges(G,blue);
  Wp->set_line_width(save_lw);
}


void draw_delaunay()
{ GRAPH<POINT,int>  DT;
  DELAUNAY_TRIANG(p_list,DT);
  draw_graph_edges(DT,triang_color);
  //draw_graph_circles(DT,brown);
}

void draw_f_delaunay()
{ GRAPH<POINT,int>  FDT;
  F_DELAUNAY_TRIANG(p_list,DT);
  draw_graph_edges(DT,f_triang_color);
}

void draw_voronoi()
{ GRAPH<CIRCLE,POINT> VD;
  VORONOI(p_list,VD);
  draw_voro_edges(VD,voro_color);
}

void draw_f_voronoi()
{ GRAPH<CIRCLE,POINT>  FVD;
  F_VORONOI(p_list,FVD);
  draw_voro_edges(FVD,f_voro_color);
}





void graph_edit(int disp)
{
  int   n = 0;

  panel P;
  P.text_item("\\bf GraphWin");
  P.text_item("");
  P.text_item("The current display selection contains different");
  P.text_item("types of graph. Which one do you want to be loaded ?");
  P.text_item("");

  for(int i=0; i<5; i++)
  { int m = (1<<i);
    if (disp & m)
    { P.button(32,32,display_bits[i],"",m);
      n++;
     }
   }

  P.button(32,32,exit_bits,"exit",0);

  if (n > 1) disp = P.open(*Wp); 

  if (disp == 0) return;

  GRAPH<POINT,int>     G1;
  GRAPH<CIRCLE,POINT>  G2;

  graph* Gp;
 
  node_array<POINT> pos;
  edge_array<int> label;

  if (disp & DT_MASK)  DELAUNAY_TRIANG(p_list,G1);
  if (disp & FDT_MASK) F_DELAUNAY_TRIANG(p_list,G1);
  if (disp & VD_MASK)  VORONOI(p_list,G2);
  if (disp & FVD_MASK) F_VORONOI(p_list,G2);
  if (disp & MST_MASK) MIN_SPANNING_TREE(p_list,G1);
  if (disp & CRUST_MASK) CRUST(p_list,G1);

  if ((disp & DT_MASK) || (disp & FDT_MASK) || (disp & MST_MASK) || (disp & CRUST_MASK))
  { Gp = &G1;
    pos.init(G1);
    label.init(G1);
    node v;
    forall_nodes(v,G1) pos[v] = G1[v];
    edge e;
    forall_edges(e,G1) label[e] = G1[e];
   }
  else
  { Gp = &G2;
    pos.init(G2);
    label.init(G2,-1);
    node v;
    forall_nodes(v,G2)
    { CIRCLE C = G2[v];
      if (!C.is_degenerate()) 
         pos[v] = C.center();
      else
       { edge e = G2.first_adj_edge(v);
         label[e] = label[G2.reversal(e)] = HULL_EDGE;
         node w = G2.target(e);
         if (!G2[w].is_degenerate())
         { POINT cw  = G2[w].center();
           VECTOR vec = G2[v].point3() - G2[v].point1();
           pos[v]  = cw + 20*vec.rotate90();
          }
          else // line
          { POINT a = G2[v].point1();
            POINT m = G2[v].point2();
            POINT b = G2[v].point3();
            VECTOR vec = b-a ;
            pos[v]  = m + 20*vec.rotate90();
            pos[w]  = m - 20*vec.rotate90();
          }
        }
     }
   }


 
  GraphWin gw(*Gp,*Wp);

  if ((disp & MST_MASK) == 0) gw.set_directed(true);

  //global settings (do not work for existing nodes ?)
  //gw.set_node_shape(circle_node);
  //gw.set_node_width(10);
  //gw.set_node_label_type(no_label);

 
  node v;
  forall_nodes(v,*Gp)
  { gw.set_shape(v,circle_node);
    gw.set_width(v,12);
    gw.set_label_type(v,no_label);
    gw.set_position(v,pos[v].to_point());
   }
 
  edge e;
  forall_edges(e,*Gp)
     switch (label[e]) {
        case DIAGRAM_EDGE:     gw.set_color(e,blue);
                               break;
 
        case NON_DIAGRAM_EDGE: gw.set_color(e,green2);
                               break;
 
        case HULL_EDGE:        gw.set_color(e,red);
                               break;
 
        default:               gw.set_color(e,black);
                               break;
      }


  list<POINT> save;
  save.conc(p_list);

  Wp->disable_panel();
  gw.display();
  gw.edit();
  Wp->enable_panel();

  p_list.conc(save);
}
 

void draw_convex_hull()
{ GRAPH<POINT,int>  DT;
  edge e = TRIANGULATE_POINTS(p_list,DT);
  if (e == nil) return;
  list<point> pol;
  edge x = e;
  do { pol.append(DT[source(x)].to_point());
       x = DT.face_cycle_succ(x);
     } while (x != e);
  Wp->draw_filled_polygon(pol,hull_color);
  Wp->draw_polygon(pol,black);
}


void draw_min_span_tree()
{ GRAPH<POINT,int>  T;
  MIN_SPANNING_TREE(p_list,T);
  edge_array<bool> drawn(T,false);
  int save_lw = Wp->set_line_width(2);
  draw_graph_edges(T,tree_color);
  Wp->set_line_width(save_lw);
}


void draw_min_encl_circle()
{ CIRCLE C = SMALLEST_ENCLOSING_CIRCLE(p_list);
  circle c = C.to_circle();
  Wp->draw_circle(c,red);
  Wp->draw_point(c.center(),red);
 }


void draw_max_empty_circle()
{ CIRCLE C = LARGEST_EMPTY_CIRCLE(p_list);
  circle c = C.to_circle();
  Wp->draw_circle(c,blue);
  Wp->draw_point(c.center(),blue);
}


void draw_min_area_annulus()
{ POINT a,b,c;
  MIN_AREA_ANNULUS(p_list,a,b,c);
  circle c1(a.to_point(),b.to_point());
  circle c2(a.to_point(),c.to_point());
  Wp->draw_disc(c2,cyan);
  Wp->draw_disc(c1,white);
  Wp->draw_circle(c1,black);
  Wp->draw_circle(c2,black);
  Wp->draw_point(a.to_point(),cyan);
}


void draw_min_width_annulus()
{ POINT a,b,c;
  MIN_WIDTH_ANNULUS(p_list,a,b,c);
  circle c1(a.to_point(),b.to_point());
  circle c2(a.to_point(),c.to_point());
  Wp->draw_disc(c2,orange);
  Wp->draw_disc(c1,white);
  Wp->draw_circle(c1,black);
  Wp->draw_circle(c2,black);
  Wp->draw_point(a.to_point(),orange);
}


void draw_closest_pair()
{
  POINT P,Q;
  CLOSEST_PAIR(p_list,P,Q);

  point p = P.to_point();
  point q = Q.to_point();
  Wp->draw_filled_node(p,orange);
  Wp->draw_filled_node(q,orange);
  Wp->set_line_width(2);
  Wp->draw_edge(p,q,orange);
  Wp->set_line_width(1);
}



void draw_area(int disp, double x0, double y0, double x1, double y1)
{ 
  if (p_list.empty()) return;

  Wp->start_buffering();
  Wp->clear();
  if (disp & MWA_MASK)  draw_min_width_annulus();
  if (disp & MAA_MASK)  draw_min_area_annulus();
  if (disp & HULL_MASK) draw_convex_hull();
  if (disp & DT_MASK)   draw_delaunay();
  if (disp & VD_MASK)   draw_voronoi();
  if (disp & FDT_MASK)  draw_f_delaunay();
  if (disp & FVD_MASK)  draw_f_voronoi();
  if (disp & LEC_MASK)  draw_max_empty_circle();
  if (disp & SEC_MASK)  draw_min_encl_circle();
  if (disp & MST_MASK)  draw_min_span_tree();
  if (disp & CRUST_MASK)  draw_crust();
  draw_sites();
  Wp->flush_buffer(x0,y0,x1,y1);
  Wp->stop_buffering();
}

void draw(int disp)
{ draw_area(disp,Wp->xmin(),Wp->ymin(),Wp->xmax(),Wp->ymax()); }


void redraw(window* wp, double x0, double y0, double x1, double y1) 
{ draw_area(display,x0,y0,x1,y1); }


int main()
{

//
// unsigned int IEEE_DEFAULT_MASK = 0x023f;
// asm volatile ("fldcw %0" : /* no output */ : "m"(IEEE_DEFAULT_MASK));
//

  bool use_filter = true;

  int pix = int(0.5 + window::dpi()/96.0);

/*
  window W("VORONOI DIAGRAMS");
*/

  int w = int(7.5*window::screen_dpi());
  int h = int(7.0*window::screen_dpi());

  window W(w,h,"VORONOI DIAGRAMS");


  Wp = &W;

  W.set_bitmap_colors(black,blue);

  string help_string;
  help_string += "This program demonstrates some of the algorithms ";
  help_string += "for two dimensional geometry of points based on "; 
  help_string += "Delaunay triangulations and Voronoi Diagrams. In ";
  help_string += "particular, ...";

  panel help_win;

  help_win.text_item("\\bf Voronoi Demo");
  help_win.text_item("");
  help_win.text_item("K. Mehlhorn and S. Naeher (1997)");
  help_win.text_item("");
  help_win.text_item(help_string);
  help_win.button("ok");
  


  string display_help = "Select here what to display.";
  string input_help   = "Select kind of mouse input.";

  //W.choice_mult_item("",display,11,32,32,display_bits,draw, display_help);
  W.choice_mult_item("",display,11,32,32,display_bits,draw);

  //W.choice_item("",input,3,32,32,input_bits,input_help);
  W.choice_item("",input,3,32,32,input_bits);

  W.button("points",   1, "Opens a point generator panel.");
  W.button("clear",    3, "Clears point set and window.");
  W.button("graphwin", 4, "Loads graph into GraphWin.");
  W.button("settings", 2, "Opens an option setting dialog.");
  W.button("exit",     0, "Exits the program.");
  W.button("about",    5, "Displays information about this program.");

  W.display();

  W.init(0,1000,0);
  W.set_redraw(redraw);
  W.set_node_width(3*pix);
  W.set_show_coordinates(true);

  int grid_width = 0;

  //W.display_help_text("voronoi_demo");


for(;;)
{
  point p;
  int but = W.read_mouse(p);

  rat_point::use_filter = use_filter;

  if (but == 0) break;

  switch (but) {
            
    case MOUSE_BUTTON(1): { put_back_event();
                            get_input(W,input);
                            break;
                           }
        
    case 1: { // generate points
              panel P;
              P.text_item("\\bf Generate input points");
              P.text_item("");
              P.choice_item("",k_gen,"random","lattice","near circle");
              P.int_item("",n_gen,0,500);
              P.button("create",0);
              P.button("cancel",1);
              if (P.open(W) == 0)
              { switch (k_gen) {
                   case 0: random_square(n_gen);
                           break;
                   case 1: lattice_points(n_gen);
                           break;
                   case 2: random_circle(n_gen,pix_dist);
                           break;
                  }
                }
              break;
             }
   

    case 2: { // settings
              panel SP("SETTINGS");
              SP.bool_item("use filter",use_filter);
              SP.int_item("grid", grid_width,0,50,10);
              SP.int_item("pix dist", pix_dist,1,64);
              SP.color_item("sites ", site_color);
              SP.color_item("voro  ", voro_color);
              SP.color_item("triang", triang_color);
              SP.color_item("hull",   hull_color);
              SP.color_item("tree",   tree_color);
              SP.button("continue");
              SP.open(W);
              W.set_grid_mode(grid_width);
              continue;
              break;
             }

    case 3: { // clear
              clear_all();
              continue;
              break;
             }

    case 4: { // start GraphWin
              graph_edit(display);
              break;
             }

    case 5: { // help
              //help_win.open(W);
              W.display_help_text("voronoi_demo");
              break;
             }

    } 

    draw(display);
 }

 rat_point::print_statistics();

 return 0;
}
