/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_bintree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/map.h>

using namespace leda;


#include <LEDA/core/impl/bin_tree.h>
#include <LEDA/core/impl/avl_tree.h>
#include <LEDA/core/impl/bb_tree.h>
#include <LEDA/core/impl/rb_tree.h>
#include <LEDA/core/impl/rs_tree.h>


class anim_bin_tree {

  GraphWin& gw;

  bin_tree& T;

  GRAPH<point,int> G;

  map<bin_tree_item,node>  NODE;

  
  void set_node_params(bin_tree_item r)
  { 
    node v = NODE[r];

    if ( T.is_leaf(r) )
    { gw.set_color(v,ivory);
      gw.set_label(v,string("%d",T.key(r)));
      gw.set_shape(v,rectangle_node);
      return;
     }

    if ( T.is_root(r) )
      gw.set_shape(v,rectangle_node);
    else
      gw.set_shape(v,ellipse_node);

    gw.set_color(v,grey1);

    int bal = T.get_bal(r); 

    switch ( T.tree_type() ) {
      case  LEDA_AVL_TREE:
               switch (bal) {
                 case  0: gw.set_label(v,"=");
                          break;
                 case -1: gw.set_label(v,">");
                          break;
                 case  1: gw.set_label(v,"<");
                          break;
                }
               break;

      case  LEDA_BB_TREE: 
               gw.set_label(v,string("%d",bal));
               break;

      case  LEDA_RB_TREE: 
               gw.set_label_type(v,no_label);
               gw.set_color(v,(bal == 0) ? red : grey3);
               break;

     }
  }


  node scan_tree(bin_tree_item r, double x0, double x1, 
                                  double y, double dy)
  { 
    set_node_params(r);

    node   v = NODE[r];
    double x = (x0 + x1)/2;
    G[v] = point(x,y);

    bin_tree_item left  = T.l_child(r);
    bin_tree_item right = T.r_child(r);

    if (left)  G.new_edge(v,scan_tree(left,x0,x,y-dy,dy));
    if (right) G.new_edge(v,scan_tree(right,x,x1,y-dy,dy));

    return v;
  }


public:

anim_bin_tree(GraphWin& gwin, bin_tree* tptr) : gw(gwin), T(*tptr)
{ gw.message(string("\\bf\\blue %s",T.tree_name()));
  //G.clear();
  gw.set_flush(false);
  gw.set_graph(G);
}

~anim_bin_tree() { T.clear(); }


void insert(int x) 
{ 
  if (T.lookup(LEDA_CAST(x))) return;

  bin_tree_item p = T.insert(LEDA_CAST(x),0);

  if ( T.is_root(p) )
     NODE[p] = gw.new_node(point(0,0));
  else
  { bin_tree_item f = T.parent(p); 
    bin_tree_item q = T.l_child(f);
    if (p == q) q = T.r_child(f);
    point pos = gw.get_position(NODE[q]);
    bin_tree_item r = T.get_last_node();
    NODE[p] = gw.new_node(pos);
    NODE[r] = gw.new_node(pos);
   }

  node v = NODE[p];

  // compute drawing area

  double dx = gw.get_window().pix_to_real(4);
  double x0 = gw.get_xmin() + dx;
  double x1 = gw.get_xmax() - dx;
  double y0 = gw.get_ymin();
  double y1 = gw.get_ymax();

  double dy = (y1-y0)/10;

  
    G.del_all_edges();

    scan_tree(T.get_root(),x0,x1,y1-2*dy,dy);

    gw.update_graph();

    color col = gw.set_color(v,green2);

    gw.set_flush(true);
    gw.set_position(G.node_data());
    gw.set_color(v,col);
    gw.set_flush(false);

}


};


int main()
{
  GraphWin gw(1200,1000,"Binary Trees");

/*
  gw.set_node_width(18);
  gw.set_node_height(18);
  gw.set_node_label_type(no_label);
  gw.set_node_label_font(roman_font,10);
*/
  gw.set_node_width(25);
  gw.set_node_height(25);
  gw.set_node_label_type(no_label);
  gw.set_node_label_font(roman_font,16);

  gw.set_edge_direction(undirected_edge);
  gw.set_show_status(false);

  gw.display(window::center,window::center);

  int n = 16;
  int input = 0;
  int kind  = 0;

  // define a panel P to control n, input, and kind

  panel P;
  P.text_item("\\bf\\blue Binary Tree Animation");
  P.text_item("");
  P.choice_item("tree type",kind, "avl-tree","bb-tree","rb-tree");
  P.choice_item("input data",input,"random", "1 2 3 ...");
  P.int_item("# inserts",n,0,64);
  P.button("ok",0);
  P.button("quit",1);


  bin_tree* tree[3];
  tree[0] = new avl_tree;
  tree[1] = new bb_tree;
  tree[2] = new rb_tree;

  while ( gw.open_panel(P) == 0)
  {
    anim_bin_tree T(gw,tree[kind]);
  
    switch (input) {

    case 0: { // random
              for(int i=0;i<n;i++) T.insert(rand_int(0,99));
              break;
             }

    case 1: { // increasing
              for(int i=0;i<n;i++) T.insert(i);
              break;
             }
   }

   gw.message("Press done to continue.");
   gw.edit();
  }
  
  for(int i=0; i<3; i++) delete tree[i];

  return 0;
}
