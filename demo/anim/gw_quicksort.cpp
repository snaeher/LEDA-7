/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_quicksort.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/basic.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>

using namespace leda;


static int speed = 10;
static int swap_count = 0;
static int cmp_count = 0;
static bool zoom = false;

static GRAPH<int,int> G;


inline void statistics(GraphWin& gw)
{ gw.message(string("\\bf\\black compares:\\blue %d ~~~~~\
                         \\black swaps:\\blue %d",cmp_count,swap_count));
}


inline bool is_smaller(GraphWin& gw, node a, node b) 
{ cmp_count++;
  statistics(gw);
  return G[a] < G[b];
 }


static void zoom_to_interval(GraphWin& gw, node v, node w)
{ 
  double f = (gw.get_ymax()-gw.get_ymin())/(gw.get_xmax()-gw.get_xmin());

  point p = gw.get_position(v);
  point q = gw.get_position(w);
  double r = gw.get_node_radius1();

  double x0 = p.xcoord() - 2*r;
  double x1 = q.xcoord() + 2*r;

  while (x1-x0 < 16*r) 
  { x0 -= r;
    x1 += r;
   }

  double dy =  f*(x1-x0)/2;
  double y0 = p.ycoord() - dy;
  double y1 = q.ycoord() + dy;

  gw.set_animation_steps(12);
  gw.zoom_area(x0,y0,x1,y1);
}



void swap_nodes(GraphWin& gw, array<node>& A, int i, int j) 
{ 
  if (i == j) return;

  node v = A[i]; 
  node w = A[j]; 

  graph& G = gw.get_graph();
  window& W = gw.get_window();

  node_array<point> pos(G);
  node u;
  forall_nodes(u,G) pos[u] = gw.get_position(u);

  double xv = pos[v].xcoord();
  double yv = pos[v].ycoord();

  double xw = pos[w].xcoord();
  double yw = pos[w].ycoord();

  double dx = xw - xv;
  double dy = 2*(gw.get_node_radius1() + gw.get_node_border_thickness());

  int xpix = W.real_to_pix(dx);
  int ypix = W.real_to_pix(dy);


  pos[v] = point(xv,yv+dy);
  pos[w] = point(xw,yw+dy);
  //gw.set_animation_steps(32*ypix/speed);
  gw.set_animation_steps(ypix/speed);
  gw.set_position(pos);

  pos[v] = point(xw,yw+dy);
  pos[w] = point(xv,yv+dy);
  //gw.set_animation_steps(32*xpix/speed);
  gw.set_animation_steps(xpix/speed);
  gw.set_position(pos);

  pos[v] = point(xw,yw);
  pos[w] = point(xv,yv);
  //gw.set_animation_steps(32*ypix/speed);
  gw.set_animation_steps(ypix/speed);
  gw.set_position(pos);

  A[i] = w; 
  A[j] = v; 

  swap_count++;
  statistics(gw);
}


//static int ps_count = 0;

void quicksort(GraphWin& gw, array<node>& A, int l, int r)
{ 
  if (l > r) return;

  if (zoom && l < r) zoom_to_interval(gw,A[l],A[r]);

  node split = A[l];
  gw.set_color(split,red);

  int i = l;
  int j = r+1;

  while (i < j)
  { 
    while (++i <= r && is_smaller(gw,A[i],split)) 
    { gw.set_color(A[i],green2); 
      leda_wait(5.0/speed); 
     }

    while (is_smaller(gw,split,A[--j])) 
    { gw.set_color(A[j],blue2);  
      leda_wait(5.0/speed);
     }

    if (i <= j)
    { gw.set_color(A[i],blue2);
      gw.set_color(A[j],green2);
      swap_nodes(gw,A,i,j);
     }
  }

  swap_nodes(gw,A,l,j);

//gw.save_ps(string("ps-%d.ps",ps_count++));

  gw.set_color(A[j],grey1);  

  leda_wait(5.0/speed);
   
  for(i = l; i <= r; i++) 
    if (i != j) gw.set_color(A[i],ivory);

  quicksort(gw,A,l,j-1);
  quicksort(gw,A,j+1,r);
}




int main()
{
  GraphWin gw(G,1100,550,"Quicksort");

  int grid_dist = 10;

/*
  if (window::display_type() == "xx") speed = 32;
*/

  gw.set_default_menu(M_FILE | M_HELP | M_DONE);
  gw.set_show_status(false);
  gw.set_zoom_objects(true);

  int n = 16;
  int input = 0;

  panel P;
  P.text_item("\\bf\\blue Quicksort Demo");
  P.text_item("");
  P.int_item("size",n,1,32);
  P.int_item("speed",speed,1,20);
  P.choice_item("input",input,"random","1...n","n...1", "const");
  P.bool_item("zoom",zoom);
  P.text_item("");
  P.button("ok",0);
  P.button("quit",1);
  P.button("forever",2);

  gw.display(window::center,window::center);

  //bool forever = false;
  int  but = 0;
  
  do 
  { 
    swap_count = 0;
    cmp_count = 0;

    G.clear();
/*
    gw.update_graph();
*/

    if (but != 2) but = gw.open_panel(P);


    if (but == 1) break;

    array<node> A(n);

    switch (input) {

      case 0: for(int i=0;i<n;i++) A[i] = G.new_node(i+1);
              A.permute();
              break;

      case 1: for(int i=0;i<n;i++) A[i] = G.new_node(i+1);
              break;

      case 2: for(int i=0;i<n;i++) A[i] = G.new_node(n-i);
              break;

      case 3: for(int i=0;i<n;i++) A[i] = G.new_node(7);
              break;
    }

    gw.update_graph();

    gw.set_flush(false);
    gw.set_grid_dist(grid_dist);
    gw.set_node_shape(square_node);
    gw.set_node_label_type(data_label);
    gw.set_node_radius1(0.85*grid_dist);
    gw.set_node_radius2(0.85*grid_dist);
  
    for(int i=0; i<n; i++) {
      gw.set_position(A[i],point(100+2*i*grid_dist,200));
    }

    gw.set_node_label_font(fixed_font,gw.get_node_height()/2);

    zoom_to_interval(gw,A[0],A[n-1]);

    gw.set_flush(true);

    quicksort(gw,A,0,n-1);

    if (zoom) zoom_to_interval(gw,A[0],A[n-1]);
  
  } while (but == 2 || gw.wait("\\bf Press \\blue done \\black to continue\
                                or \\red exit \\black (file menu) to quit."));
  
 return 0;
}






