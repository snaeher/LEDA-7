/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  d3_hull_d3win.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/geo/d3_rat_point.h>
#include <LEDA/numbers/rat_vector.h>
#include <LEDA/graph/graph.h>
#include <LEDA/geo/rat_point.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/d3_window.h>
#include <LEDA/system/stream.h>

#include <assert.h>

/*
#include <LEDA/map.h>
#include <LEDA/plane_alg.h>
*/

#include <math.h>

using namespace leda;


static window W("3D Convex Hull");

static int  mode = 0;
static bool anim = false;
static bool step  = false;

//static bool check = false;
//static bool filter = true;

static int maxcoord = 1000;
static int d3win_speed = 2;


static int vis_count = 1;


static void init_hull(list<d3_rat_point>& L, GRAPH<d3_rat_point,int>& H)
{
  L.sort(); 
  H.clear();

  list_item it = L.first();
  while (it)
  { d3_rat_point p = L[it];
    H.new_node(p);
    list_item sit = it;
    do sit = L.succ(sit); while (sit && L[sit] == p);
    it = sit;
   }
}


static bool check_hull(GRAPH<d3_rat_point,int>& H)
{  

   rat_vector c_vec(0,0,0,1);
   int n = 0;

   // origin is interior point

/*
   node v;
   forall_nodes(v,H) 
     if (H.degree(v) > 0) 
     { c_vec += H[v].to_vector();
       n++;
      }

   c_vec /= n;
*/

   edge_array<bool> considered(H,false);

   n = H.number_of_edges();

   edge e;
   forall_edges(e,H)
    { 

      if (n%100 == 0)
      { W.del_messages();
        W.message(string("checking edges: %4d", n));
      }
      n--;

      if (considered[e]) continue;

      edge e1 = H.cyclic_adj_succ(e);
      edge e2 = H.cyclic_adj_pred(e);

      considered[e]  = true;
      considered[e1] = true;
      considered[e2] = true;

      d3_rat_point a = H[source(e)];
      d3_rat_point b = H[target(e)];
      d3_rat_point c = H[target(e1)];
      d3_rat_point d = H[target(e2)];

      if (orientation(a,b,c,c_vec) > 0)
      { error_handler(0,"orientation error");
        return false;
       }

      if (orientation(a,b,c,d) > 0)
      { error_handler(0,"local non-convexity");
        return false;
       }


     }

   W.del_messages();

   return true;
}
     


static void extend_visible_cycle(GRAPH<d3_rat_point,int>& H, edge& border_e, 
                                                      d3_rat_point p,
                                                      list<edge>& del_edges)
{ vis_count++;

  del_edges.clear();
 
  list<edge> S;

  // initialize S with visible face cycle

  assert(border_e);

  edge x = border_e;
  do { S.append(x);
       H[x] = vis_count;
       x = H.face_cycle_succ(x);
  } while (x != border_e);

  list_item it1 = S.first();
  list_item it2 = S.succ(it1);
  list_item it3 = S.succ(it2);

  node a = H.source(S[it1]);
  node b = H.source(S[it2]);
  node c = H.source(S[it3]);

  assert(orientation(H[a],H[b],H[c],p) >= 0);

  while (!S.empty())
  { edge e = S.pop();
    edge r = H.reversal(e);
    if (H[r] == vis_count) continue;
    edge r1 = H.face_cycle_succ(r);
    d3_rat_point A = H[source(r)];
    d3_rat_point B = H[source(r1)];
    d3_rat_point C = H[target(r1)];
    int orient  = orientation(A,B,C,p);
    if (orient >= 0)
    { edge x = r;
      do { H[x] = vis_count;
           S.append(x);
           edge y = H.reversal(x);
           if (H[y] == vis_count)
           { del_edges.append(x);
             del_edges.append(y); 
            } 
           x = H.face_cycle_succ(x);
       } while (x != r);
     }
     else border_e = e;
   }
}




static void d3_hull(GRAPH<d3_rat_point,int>& H, d3_window *d3win)
{
  if (H.number_of_nodes() == 0) return;

  H.del_all_edges();

  list<node> L = H.all_nodes();

  // search for 3 non-collinear points

  node a = L.pop();
  node b = L.pop();
  node c = L.pop();

  while (collinear(H[a],H[b],H[c]) && !L.empty()) 
  { H.del_node(b);
    b = c;
    c = L.pop();
   }

  if (collinear(H[a],H[b],H[c])) // all points are collinear
  { edge x = H.new_edge(a,c,0);
    edge y = H.new_edge(c,a,0);
    H.set_reversal(x,y);
    H.del_node(b);
    return;
   }


  // construct triangle (a,b,c)

  d3_rat_point A = H[a];
  d3_rat_point B = H[b];
  d3_rat_point C = H[c];

  d3_rat_point D = point_on_positive_side(A,B,C);

  H.set_reversal(H.new_edge(a,b,0),H.new_edge(b,a,0));
  H.set_reversal(H.new_edge(b,c,0),H.new_edge(c,b,0));
  H.set_reversal(H.new_edge(c,a,0),H.new_edge(a,c,0));

  edge border_e = H.last_adj_edge(c);

  if (anim)
  { d3win->move();
    if (step) {
      while (d3win->move() == NO_BUTTON);
    }
  }


  if (L.empty()) return;

  int dim = 2;

  while (!L.empty())
  {
    node v = L.pop();

    d3_rat_point p = H[v];

    if (dim == 2) 
    { 
      int orient = orientation(A,B,C,p);

      if (orient == 0)
      { 
        // construct upper tangent
        edge up = border_e;
        while (orientation(p,H[source(up)],H[target(up)],D) <= 0)
           up = H.face_cycle_succ(up);
      
        edge x = H.new_edge(up,v,0,leda::after);
        edge y = H.new_edge(v,source(up));
        H.set_reversal(x,y);
      
        // construct lower tangent
        edge down = H.face_cycle_pred(border_e);
        while (orientation(p,H[source(down)],H[target(down)],D) <= 0)
           down = H.face_cycle_pred(down);
      
        down = H.face_cycle_succ(down);
      
        x = H.new_edge(down,v,0,leda::after);
        y = H.new_edge(v,source(down));
        H.set_reversal(x,y);

        // remove visible edges
        while (target(down) != v)
        { edge e = down;
          down = H.face_cycle_succ(down);
          node u = source(e);
          H.del_edge(H.reversal(e));
          H.del_edge(e);
          if (H.degree(u) == 0) H.del_node(u);
         }
     
        border_e = H.first_adj_edge(v);

        if (anim)
        { d3win->move();
          if (step) {
             while (d3win->move() == NO_BUTTON);
          }
        }
 
        continue;
       }

     if (orient < 0) border_e = H.reversal(border_e);

     dim = 3;
    }

    // 3-dimensional case
    // compute edges visible from p

    list<edge> del_edges;

    extend_visible_cycle(H,border_e,p,del_edges); 

    if (anim)
    { d3win->move();
      if (step) {
         while (d3win->move() == NO_BUTTON);
      }
    }
 


    // remove visible faces

    edge e;
    forall(e,del_edges)
    { node v = source(e);
      node w = target(e);
      H.del_edge(e);
      if (H.degree(v) == 0) H.del_node(v);
      if (H.degree(w) == 0) H.del_node(w);
     }

    // and re-triangulate
 
/*
    if (anim)
    { d3win->move();
      if (step)
         while (d3win->move() == NO_BUTTON);
     }
*/

    edge be = nil;

    e = border_e;
    do { d3_rat_point A = p;
         d3_rat_point B = H[source(e)];
         d3_rat_point C = H[target(e)];
         if ( be == nil && !L.empty() )
         { d3_rat_point D = H[L.head()];
           if (orientation(A,B,C,D) > 0) be = e;
          }
         edge x = H.new_edge(e,v,0,leda::after);
         edge y = H.new_edge(v,source(e),0);
         H.set_reversal(x,y);
         e = H.face_cycle_succ(e);
        }
    while (target(e) != v);

   border_e = be;

   //if (check && !check_hull(H)) break;

/*
   int i = L.length();
   if (i%100 == 0)
   { W.del_messages();
     if (i > 0) 
        W.message(string("%4d", i));
    }
*/
 }

}


static void join_coplanar_faces(GRAPH<d3_rat_point,int>& H)
{
  list<edge> L;
  edge_array<bool> marked(H,false);

  edge e;
  forall_edges(e,H)
  { if (marked[e]) continue;
    edge r = H.reversal(e);
    edge e1 = H.face_cycle_succ(e);
    edge r1 = H.face_cycle_succ(r);

    d3_rat_point A = H[source(e)];
    d3_rat_point B = H[source(e1)];
    d3_rat_point C = H[target(e1)];
    d3_rat_point D = H[target(r1)];

    if (orientation(A,B,C,D) == 0) 
    { L.append(e);
      L.append(r);
      marked[e] = marked[r] = true;
     }
   }

   forall(e,L) H.del_edge(e);
}


     
static void generate_input(int n, int input, int maxc, list<d3_rat_point>& L)
{ 
  L.clear();
  W.clear();
  W.message("Generating Points");


  switch (input) {

   case 0: random_points_in_cube(n,3*maxc/4,L);
           break;

   case 1: random_points_in_ball(n,maxc,L);
           break;

   case 2: random_points_on_sphere(n,maxc,L);
           break;


   case 3: lattice_points(n,3*maxc/4,L);
           break;

   case 4: random_points_on_paraboloid(n,maxc,L);
           break;


   case 5: random_points_in_square(n,maxc,L);
           break;

/*
   case 6: random_points_on_segment(n,maxc,L);
           break;
*/

  }

  d3_rat_point p; 
  forall(p,L)
     W.draw_point(p.xcoordD(),p.ycoordD(),blue);
  W.del_messages();
}



static d3_window* d3win_ptr;

static void mode_action(int new_mode)
{ anim = (new_mode & 1) != 0;
  d3win_ptr->set_elim((new_mode & 2) != 0);
  d3win_ptr->set_solid((new_mode & 4) != 0);
  d3win_ptr->set_draw_edges((new_mode & 8) != 0);

  //check = (new_mode & 16) != 0;
  //filter = (new_mode & 32) != 0;
  //d3_rat_point::use_filter = (filter) ? 1 : 0;
  d3win_ptr->draw();
 }

  

int main() {

 int n = 250;
 int t = 0;
 int input = 1;

 mode  = 1 + 2 + 4 + 8; // anim, elim, solid, edges

 anim = (mode & 1) != 0;
 //check = (mode & 16) != 0;
 //filter = (mode & 32) != 0;

 //d3_rat_point::use_filter = (filter) ? 1 : 0;

 if (window::display_type() == "xx") d3win_speed = 25;


 list<string> choices;
 choices.append("cube");
 choices.append("ball");
 choices.append("sphere");
 choices.append("mesh");
 choices.append("parabol");
 choices.append("square");
/*
 choices.append("line");
*/

 list<string> options;
 options.append("anim");
 options.append("elim");
 options.append("solid");
 options.append("edges");
/*
 options.append("check");
 options.append("filter");
*/

 list<string> thousands;
 thousands.append("0");
 thousands.append("1000");
 thousands.append("2000");
 thousands.append("4000");
 thousands.append("8000");
 thousands.append("16000");
 thousands.append("32000");

 W.choice_item("input",input,choices);
/*
 W.choice_item("points",t,thousands);
 W.int_item("    +",n,4,500);
*/
 W.int_item("points  ",n,0,1000);
 W.choice_mult_item("options",mode,options,mode_action);

 W.button("generate",1);
 W.button("run",2);

//W.button("step",3);
//W.button("join",4);
//W.button("setup",5);
//W.button("file",11);
//W.button("forever",3);

 W.button("exit",0);

 W.set_node_width(2);

 W.display(window::center,window::center);

 W.init(-1.55*maxcoord,1.55*maxcoord,-1.30*maxcoord);


 list<d3_rat_point> L;
 GRAPH<d3_rat_point,int> H;

 d3_window d3win(W,H);

 d3win.set_max_coord(1.2*maxcoord);

 d3win.set_elim((mode & 2) != 0);
 d3win.set_solid((mode & 4) != 0);
 d3win.set_draw_edges((mode & 8) != 0);
 d3win.set_speed(d3win_speed);

 double x = 1.0 * maxcoord;
 double y = 0.7 * maxcoord;

 d3win.set_mouse_position(x,y);

 d3win_ptr = &d3win;

 int but = W.read_mouse();

 //bool forever = (but == 3);

 while (but != 0)
 {
   switch (but) {
  
   case 11:
   case 1: { 
             if (but == 11) //file
             { string fname;
               panel P;
               P.text_item("\\bf\\blue File Input");
               P.text_item("");
               P.string_item("file",fname);
               P.text_item("");
               P.button("read",1);
               P.button("cancel",0);
               if (P.open(W) == 1) 
               { file_istream in(fname);
                 L.read(in);
                }
              }
             else
             { int m = (1 << t)/2 * 1000;
               rand_int.set_seed(m+n);
               generate_input(m+n,input,int(maxcoord),L);
              }


             init_hull(L,H);
             node_array<rat_vector> pos(H);
             node v;
             forall_nodes(v,H) pos[v] = H[v].to_vector();
             d3win.init(pos);

/*
             d3_point p0(0,0,0);
             d3_point p1 = p0.translate(300,0,0);
             d3_point p2 = p0.translate(0,300,0);
             d3_point p3 = p0.translate(0,0,300);
             d3_point p4 = p0.translate(-300,0,0);
             forall_nodes(v,H) 
               if (inside_sphere(p3,p2,p1,p4,H[v].to_d3_point())) 
                   d3win.set_color(v,red);
*/

             W.clear();
             d3win.draw();
             break;
           }
  
   case 2: step = false;
           d3_hull(H,anim ? &d3win : 0);
/*
           if (check) 
           { d3_rat_point::print_statistics();
             check_hull(H);
             d3_rat_point::print_statistics();
            }
*/
           W.clear();
           d3win.draw();
           //if (forever) but = 3;
           break;

   case 3: step = true;
           //anim = true;
           d3_hull(H,&d3win);
/*
           if (check) check_hull(H);
*/
           W.clear();
           d3win.draw();
           break;

   case 4: join_coplanar_faces(H);
           W.clear();
           d3win.draw();
           break;

   case 5: { panel P;
             P.int_item("max xcoord",maxcoord,1,1000);
             P.int_item("d3win speed",d3win_speed,1,2*d3win_speed);
             P.button("ok");
             P.open(W);
             d3win.set_speed(d3win_speed);
             break;
           }


   }

/*
  if (forever)
  { if (d3win.move() == MOUSE_BUTTON(3)) break;
    leda_wait(1.5);
   }
  else
*/
    but = d3win.move();
 }


 return 0;

}
