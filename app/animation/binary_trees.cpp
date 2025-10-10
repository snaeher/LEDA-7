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

  friend void update_func(string s);
  
  void set_node_params(bin_tree_item r)
  { 
    node v = NODE[r];

    if ( T.is_leaf(r) )
    { gw.set_color(v,ivory);
      gw.set_label(v,string("%d",T.key(r)));
      gw.set_shape(v,rectangle_node);
      return;
     }

    gw.set_shape(v,ellipse_node);

    if ( T.is_root(r) )
      gw.set_color(v,grey3);
    else
      gw.set_color(v,grey1);


    int bal = T.get_bal(r); 

    switch ( T.tree_type() ) {

      case  LEDA_AVL_TREE:

               if (bal > 0)
                 gw.set_label(v,string("+%d",bal));
               else
                 gw.set_label(v,string("%d",bal));
/*
               switch (bal) {
                 case  0: gw.set_label(v,"=");
                 case  0: gw.set_label(v,"=");
                          break;
                 case -1: gw.set_label(v,">");
                          break;
                 case  1: gw.set_label(v,"<");
                          break;
                }
*/
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
    if (r == 0) return 0;

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

  void update_layout(node v)
  {
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
  
    color col;
    if (v) col = gw.set_color(v,green2);
  
    gw.set_flush(true);
    gw.set_position(G.node_data());
    if (v) gw.set_color(v,col);
    gw.set_flush(false);

    gw.redraw();
  }

public:

anim_bin_tree(GraphWin& gwin, bin_tree* tptr) : gw(gwin), T(*tptr)
{ gw.message(string("\\bf\\blue %s",T.tree_name()));
//G.clear();
  gw.set_flush(false);
  gw.set_graph(G);
}

~anim_bin_tree() { T.clear(); NODE.clear(); G.clear(); }


void insert(int x) 
{ 
  gw.message(string("\\bf\\blue %s :~~\\black Insert %d",T.tree_name(),x));

  if (T.lookup(LEDA_CAST(x))) return;

  bin_tree_item p = T.insert(LEDA_CAST(x),0);

  if ( T.is_root(p) )
  { double x = (gw.get_xmin() + gw.get_xmax())/2;
    double y = gw.get_ymax();
    NODE[p] = gw.new_node(point(x,y));
  }
  else
  { bin_tree_item f = T.parent(p); 
    bin_tree_item q = T.l_child(f);
    if (p == q) q = T.r_child(f);
    point pos = gw.get_position(NODE[q]);
    bin_tree_item r = T.get_last_node();
    NODE[p] = gw.new_node(pos);
    NODE[r] = gw.new_node(pos);
   }

  update_layout(NODE[p]);
}


void del(int x) 
{ 
  gw.message(string("\\bf\\blue %s :~~\\black Delete %d",T.tree_name(),x));

  bin_tree_item p = T.lookup(LEDA_CAST(x));

  if (p == 0) return;

  node v = NODE[p]; // node to be deleted

  gw.set_flush(true);

  gw.set_color(v,red);
  sleep(0.5);
  gw.del_node(v);

  bin_tree_item f = T.parent(p); 

  if (f) gw.del_node(NODE[f]); 

  gw.set_flush(false);

  T.del_item(p);
  update_layout(0);

  gw.message(string("\\bf\\blue %s\\n\\rm\\black Click on a leaf to delete it.",
                    T.tree_name()));
}



};

anim_bin_tree* T_ptr = 0;


void node_click_action(GraphWin& gw, const point& p)
{
  node v = gw.find_node(p);

  if (v) {
    string label = gw.get_label(v);
    T_ptr->del(atoi(label));
  }
}

void update_func(string s) 
{ cout << s << endl; 
  //T_ptr->update_layout(0);
}


int main()
{

/*
cout << "default width = " << window::default_width() << endl;
cout << "default height = " << window::default_height() << endl;
*/

  GraphWin gw("Binary Trees");

/*
  gw.set_node_width(45);
  gw.set_node_height(45);
  gw.set_node_label_font(roman_font,12);
*/

  gw.set_node_width(50);
  gw.set_node_height(50);
  gw.set_node_label_font(roman_font,11);

  gw.set_node_label_type(no_label);
  gw.set_edge_direction(undirected_edge);
  gw.set_show_status(false);

  gw.set_action(A_LEFT, NULL);
  gw.set_action(A_LEFT | A_NODE, node_click_action);

  gw.display(window::center,window::center);

  int n = 12;
  int input = 0;
  int kind  = 0;

  int anim_steps = 1000;

  if (window::display_type() == "xx") anim_steps = 25;

  // define a panel P to control n, input, and kind

  panel P;
  P.text_item("\\bf\\blue Binary Tree Animation");
  P.text_item("");
  P.choice_item("tree type",kind, "avl-tree","bb-tree","rb-tree","rs-tree");
  P.choice_item("input data",input,"random", "1 2 3 ...");
  P.int_item("anim steps",anim_steps,0,2*anim_steps);
  P.text_item("");
  P.int_item("# inserts",n,0,64);
  P.button("ok",0);
  P.button("quit",1);


  bin_tree* tree[4];
  tree[0] = new avl_tree;
  tree[1] = new bb_tree;
  tree[2] = new rb_tree;
  tree[3] = new rs_tree;

  while ( gw.open_panel(P) == 0)
  {
    gw.set_animation_steps(anim_steps);

    anim_bin_tree T(gw,tree[kind]);

tree[kind]->set_update_func(update_func);

    T_ptr = &T;
  
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

  //gw.message("Press done to continue.");
  gw.message(string("\\bf\\blue %s\\n\\rm\\black Click on a leaf to delete it.",
                    tree[kind]->tree_name()));
   gw.edit();
  }
  
  for(int i=0; i<4; i++) delete tree[i];

  return 0;
}
