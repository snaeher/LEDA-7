/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  d3_hull_anim.c
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


static window W("3D Convex Hull Demo");

static int  mode;

static bool step  = false;
static bool check = false;
static bool filter = true;

static int maxcoord = 1000;
static int anim_speed = 20;

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
/*
      if (n%100 == 0)
      { W.del_messages();
        W.message(string("checking edges: %4d", n));
      }
      n--;
*/

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

/*
   W.del_messages();
*/

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




static void d3_hull(GRAPH<d3_rat_point,int>& H, d3_window *anim)
{
  bool trace = (anim != 0);

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

  if (trace)
  { anim->move();
    if (step)
    while (anim->move() == NO_BUTTON);
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

        if (trace)
        { anim->move();
          if (step)
             while (anim->move() == NO_BUTTON);
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

    if (trace)
    { anim->move();
      if (step)
         while (anim->move() == NO_BUTTON);
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
    if (trace)
    { anim->move();
      if (step)
         while (anim->move() == NO_BUTTON);
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
/*
  W.clear();
  W.message("Generating Points");
*/

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

/*
  W.del_messages();
*/
}



int main()
{
//int n = 150;
  int n = 200;

//int input = 5;
  int input = 1;


  if (getenv("LEDA_OPEN_MAXIMIZED"))
  { W.display(W,window::center,window::center); // frameless
    W.init(-2.5*maxcoord,2.5*maxcoord,-1.15*maxcoord);
    anim_speed = 15;
   }
  else
  { W.display(window::center,window::center);
    W.init(-1.5*maxcoord,1.5*maxcoord,-1.65*maxcoord);
    anim_speed = 20;
   }



  GRAPH<d3_rat_point,int> H;
  d3_window anim(W,H);
 
  anim.set_message("");
 
  anim.set_max_coord(1.5*maxcoord);
 
  anim.set_elim(false);
  anim.set_solid(false);
  anim.set_draw_edges(true);
  anim.set_speed(anim_speed);

  anim.set_zooming(false);
 
  int steps = 100;

  bool trace = true;

  double x = 1.50 * maxcoord;
  double y = 0.75 * maxcoord;

  int but;
  unsigned long t;
 
  for(;;)
  {
    anim.set_mouse_position(x,y);

    anim.set_elim(false);
    anim.set_solid(false);
   
    list<d3_rat_point> L;
    generate_input(n,input,int(maxcoord),L);
   
    init_hull(L,H);
   
    node_array<rat_vector> pos(H);
    node v;
    forall_nodes(v,H) pos[v] = H[v].to_vector();
    anim.init(pos);

    anim.draw();
  //while (anim.move() == NO_BUTTON) {}

    for(int i=0; i<steps; i++) {
      if (W.read_event(but,x,y,t,10) == button_press_event) break;
      anim.move(0.75*x,0.75*y);
    }
   

    if (trace)
    { anim.set_elim(true);
      anim.set_solid(true);
      d3_hull(H,&anim);
     }
    else
      d3_hull(H,0);

    anim.draw();

/*
  //while (anim.move() == NO_BUTTON) {}
    for(int i=0; i<steps; i++) {
      if (W.read_event(but,x,y,t,10) == button_press_event) break;
      anim.move(0.75*x,0.75*y);
    }
*/
   

    anim.set_elim(true);
    anim.set_solid(true);
  //while (anim.move() == NO_BUTTON) {}
    while (W.read_event(but,x,y,t,10) != button_press_event) {
      anim.move(0.75*x,0.75*y);
    }

  }
  
  return 0;
}
