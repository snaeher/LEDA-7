/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  delaunay_hull_demo.c
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
#include <LEDA/geo/random_rat_point.h>

#include <LEDA/geo/d3_hull.h>

/*
#include <LEDA/map.h>
#include <LEDA/plane_alg.h>
*/

#include <math.h>

using namespace leda;



static window W("Delaunay Diagrams and 3D Convex Hulls");

static int  mode;
static bool trace = false;
static bool step  = false;
static bool check = false;
static bool filter = true;

static int maxcoord = 2;
static int anim_speed = 10;


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

   // origin is interior point, not true for delaunay and convex hull

   node v;
   forall_nodes(v,H) 
     if (H.degree(v) > 0) 
     { c_vec += H[v].to_vector();
       n++;
      }

   c_vec /= n;


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

      if (orientation(a,b,c,c_vec) < 0)
      { error_handler(0,"orientation error");
        return false;
       }

      if (orientation(a,b,c,d) < 0)
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

  edge x = border_e;
  do { S.append(x);
       H[x] = vis_count;
       x = H.face_cycle_succ(x);
  } while (x != border_e);


  while (!S.empty())
  { edge e = S.pop();
    edge r = H.reversal(e);
    if (H[r] == vis_count) continue;
    edge r1 = H.face_cycle_succ(r);
    d3_rat_point A = H[source(r)];
    d3_rat_point B = H[source(r1)];
    d3_rat_point C = H[target(r1)];
    if (orientation(A,B,C,p) <= 0)
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

  H.new_edge(a,b,0);
  H.new_edge(a,c,0);
  H.new_edge(b,a,0);
  H.new_edge(b,c,0);

  if (orientation(A,B,C,D) > 0)
   { H.new_edge(c,a,0);
     H.new_edge(c,b,0);
    }
  else
   { H.new_edge(c,b,0);
     H.new_edge(c,a,0);
    }

  H.make_map();

  if (L.empty()) return;


  edge border_e = H.last_adj_edge(c);

  int  dim = 2;

  //int  i = 3;

  while (!L.empty())
  {
    node v = L.pop();

    d3_rat_point p = H[v];

    if (dim == 2) 
    { 
      int orient = orientation(H[a],H[b],H[c],p);

      if (orient == 0)
      { 
        // construct upper tangent
        edge up = border_e;
        do  up = H.face_cycle_pred(up); 
        while (orientation(p,H[source(up)],H[target(up)],D) >= 0);

        up = H.face_cycle_succ(up);
        edge x = H.new_edge(up,v);
        edge y = H.new_edge(v,source(up));
        H.set_reversal(x,y);

        // construct lower tangent
        edge down = border_e;
        while (orientation(p,H[source(down)],H[target(down)],D) >= 0)
             down = H.face_cycle_succ(down);

        x = H.new_edge(down,v);
        y = H.new_edge(v,source(down));
        H.set_reversal(x,y);

        // remove visible edges
        while (target(up) != v)
        { edge e = H.face_cycle_succ(up);
          node u = source(up);
          H.del_edge(H.reversal(up));
          H.del_edge(up);
          if (H.degree(u) == 0) H.del_node(u);
          up = e;
         }

        border_e = H.last_adj_edge(v);

        if (anim)
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

    if (anim)
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
    if (anim)
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
           if (orientation(A,B,C,D) < 0) be = e;
          }
         edge x = H.new_edge(e,v,0,leda::after);
         edge y = H.new_edge(v,source(e),0);
         H.set_reversal(x,y);
         e = H.face_cycle_succ(e);
        }
    while (target(e) != v);

   border_e = be;

   //if (check && !check_hull(H)) break;

   int i = L.length();
   if (i%100 == 0)
   { W.del_messages();
     if (i > 0) 
        W.message(string("%4d", i));
    }
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

/*
static void random_points_on_paraboloid_of_revolution( int n, int& maxc,
list<d3_rat_point>& L, list<rat_point>& L0)
{ L.clear();
  while ( n > 0 )
    { n--; 
      int x = rand_int(-maxc,maxc);
      int y = rand_int(-maxc,maxc);
      L.append(d3_rat_point(x*maxc,y*maxc,x*x + y*y,maxc*maxc));
      L0.append(rat_point(x,y,maxc));
    }
}
*/

static list<d3_rat_point> lift_to_paraboloid(const list<rat_point>& L0)
{ list<d3_rat_point> result;
  rat_point p;
  rational one_half(1,2);
  forall(p,L0) 
  { rational x = 2*(p.xcoord() - one_half);
    rational y = 2*(p.ycoord() - one_half);
    result.append(d3_rat_point(x,y,(x*x+y*y)));
  }
  return result;
}



     
static void generate_input(int n, int input, int maxc, list<d3_rat_point>& L,
list<rat_point>& L0)
{ 
  L.clear();
  W.clear();
  W.message("Generating Points");

  switch (input) {

   case 0: // non co-circular 
           random_points_in_unit_square(n,L0);
           break;

   case 1: // co-circular
           random_points_on_unit_circle(n,L0);
           break;




  }

  L = lift_to_paraboloid(L0);

  d3_rat_point p; 
  forall(p,L)
     W.draw_point(p.xcoordD(),p.ycoordD(),blue);
  W.del_messages();
}



static d3_window* pa;

static void mode_action(int new_mode)
{ //filter = (new_mode & 1);
  //trace = (new_mode & 2);
  //check = (new_mode & 4);
  pa->set_elim((new_mode & 1) != 0);
  pa->set_solid((new_mode & 2) != 0);
  pa->set_draw_edges((new_mode & 4) != 0);
  d3_rat_point::use_filter = (filter) ? 1 : 0;
 }

  

int main() {

 int n = 32;
 int t = 0;
 int input = 0;

 mode  = 1 + 2 + 4;

 filter = 1;
 trace = 0;
 check = 1;

 d3_rat_point::use_filter = (filter) ? 1 : 0;


 list<string> choices;
 choices.append("non co-circular");
 choices.append("co-circular");
 /*
  choices.append("square");
 choices.append("para");
 choices.append("mesh");
 choices.append("sphere");
 choices.append("line");
 choices.append("para of revolution");
 */

 list<string> setup;
 //setup.append("filter");
 //setup.append("trace");
 //setup.append("check");
 setup.append("elim");
 setup.append("solid");
 setup.append("edges");

 list<string> thousands;
 thousands.append("0");
 thousands.append("1000");
 thousands.append("2000");
 thousands.append("4000");
 thousands.append("8000");
 thousands.append("16000");
 thousands.append("32000");

 W.choice_item("input",input,choices);
 //W.choice_item("points",t,thousands);
 W.int_item("number of points",n,4,50);
 W.choice_mult_item("setup",mode,setup,mode_action);

 W.button("gen",1);
 W.button("run",2);

 //W.button("step",3);
 //W.button("join",4);
 //W.button("setup",5);

 //W.button("view from -infty",6);
 //W.button("arbitrary view",7);
 W.button("from -infty",6);
 W.button("rotate",7);

 W.button("exit",0);

 W.init(-maxcoord,maxcoord,-maxcoord);
 W.display(window::center,window::center);

 W.set_node_width(2);

 list<rat_point> L0;
 list<d3_rat_point> L;
 GRAPH<d3_rat_point,int> H;

 d3_window anim(W,H);

 anim.set_elim((mode & 1) != 0);
 anim.set_solid((mode & 2) != 0);
 anim.set_draw_edges((mode & 4) != 0);
 anim.set_speed(anim_speed);

 pa = &anim;


 int but = W.read_mouse();

 
 while (but != 0)
 {

   switch (but) {

     
  
   case 1: { int m = (1 << t)/2 * 1000;
             rand_int.set_seed(m+n);
             generate_input(m+n,input,int(0.6*W.xmax()),L, L0);
             init_hull(L,H);
             node_array<rat_vector> pos(H);
             node v;
             forall_nodes(v,H) pos[v] = H[v].to_vector();
             anim.init(pos);
             W.clear();
             anim.draw();
             break;
           }

   case 6: { init_hull(L,H); d3_hull(H,0);
           node_array<rat_vector> pos(H);
           node v;
           forall_nodes(v,H) pos[v] = H[v].to_vector();
           anim.init(pos);
           anim.set_speed(0);
           W.clear();
           anim.draw();
           break;
   }

   case 7: 
           anim.set_speed(anim_speed);
           W.clear();
           anim.draw();
           break;
          

   case 2: step = false;
           d3_hull(H,trace ? &anim : 0);
           if (check) 
           { d3_rat_point::print_statistics();
             check_hull(H);
             d3_rat_point::print_statistics();
            }
           W.clear();
           anim.draw();
           break;

   case 3: step = true;
           d3_hull(H,trace ? &anim : 0);
           if (check) check_hull(H);
           W.clear();
           anim.draw();
           break;

   case 4: join_coplanar_faces(H);
           W.clear();
           anim.draw();
           break;

   case 5: { panel P;
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



