/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_plandemo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>

using namespace leda;



void highlight(GraphWin& gw, list<node> V, list<edge> E, 
                             node_array<int>& kind)
{
  const graph& G = gw.get_graph();

  bool flush0 = gw.set_flush(false);

  node v;
  forall_nodes(v,G) {
    switch (kind[v]) {

      case  0: gw.set_color(v,grey1);
               gw.set_border_color(v,grey1);
               gw.set_label_color(v,grey2);
               break;
        
      case  2: gw.set_color(v,grey1);
               gw.set_label_type(v,no_label);
               gw.set_width(v,8);
               gw.set_height(v,8);
               break;

      case  3:
      case  4: gw.set_shape(v,rectangle_node);
               gw.set_color(v,red);
               break;

      case -3: gw.set_shape(v,rectangle_node);
               gw.set_color(v,blue2);
               break;
    }
  }

  edge e;
  forall_edges(e,G) gw.set_color(e,grey1);
        
  forall(e,E)
  { gw.set_color(e,black);
    gw.set_width(e,2);
   }

 gw.redraw();
 gw.set_flush(flush0);
}


void set_layout(GraphWin& gw, const node_array<double>& destx,
                              const node_array<double>& desty)
{
  graph& G = gw.get_graph();

  node_array<point> n_dest(G);
  node_array<double> n_rad1(G);
  node_array<double> n_rad2(G);
  edge_array<point> e_sanch(G);
  edge_array<point> e_tanch(G);
  edge_array<list<point> > e_dest(G);

  node v;
  forall_nodes(v,G) 
  { n_dest[v]=point(destx[v],desty[v]);
    n_rad1[v] = gw.get_radius1(v);
    n_rad2[v] = gw.get_radius2(v);
   }

  unsigned anim = gw.get_animation_steps();
  if (anim == 0) anim = 1;

  node_array<vector>        p_trans(G);
  node_array<double>        r1_trans(G);
  node_array<double>        r2_trans(G);
  edge_array<list<vector> > e_trans(G);
  edge_array<vector>        sa_trans(G);
  edge_array<vector>        ta_trans(G);
  list<node> L;
  list<edge> E;

  bool save_f = gw.set_flush(false);

  gw.set_layout_start(n_dest,n_rad1,n_rad2,e_dest,e_sanch,e_tanch,
                     L,E,p_trans,r1_trans,r2_trans,e_trans,sa_trans,ta_trans);

  while (--anim > 0)
  { leda_wait(0.005);
    gw.set_layout_step(L,E,p_trans,r1_trans,r2_trans,e_trans,sa_trans,ta_trans);
   }

  gw.set_layout_end(L,E,n_dest,n_rad1,n_rad2,e_dest,e_sanch,e_tanch);

  gw.set_flush(save_f);
}



int main() 
{
  GraphWin gw("Planarity Test Demo");

  gw.display(window::center,window::center);

  while (gw.edit()) 
  { 
    graph& G = gw.get_graph();

    
    if (Is_Planar(G)) 
    { if (G.number_of_nodes() < 3) continue;
      node_array<double> xcoord(G);
      node_array<double> ycoord(G);
      //STRAIGHT_LINE_EMBEDDING(G,xcoord,ycoord);
      gw_straight_line_embedding(gw,xcoord,ycoord);
      gw.adjust_coords_to_win(xcoord,ycoord);     // !!!
      gw.set_layout(xcoord,ycoord);
    }
    else 
    { list<node> V_k;
      list<edge> E_k;
      node_array<int> kind(G);
      //KURATOWSKI(G,V_k,E_k,kind);
      gw_kuratowski(gw,V_k,E_k,kind);
      
      gw.save_all_attributes();
      highlight(gw,V_k,E_k,kind);
      gw.wait("This Graph is not planar. I show you a\
               Kuratowski Subdivision (click done).");
      gw.restore_all_attributes();
    }

  }
  
  return 0;
}

