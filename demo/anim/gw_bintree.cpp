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

#include <LEDA/core/impl/bin_tree.h>
#include <LEDA/core/impl/avl_tree.h>
#include <LEDA/core/impl/bb_tree.h>
#include <LEDA/core/impl/rb_tree.h>
#include <LEDA/core/impl/rs_tree.h>

using namespace leda;


enum { BIN_TREE, AVL_TREE, BB_TREE, RB_TREE, RS_TREE };

const char* NAME[] = {
  "Binary Tree",
  "AVL Tree",
  "BB[alpha] Tree",
  "Red/Black Tree",
  "Randomized Search Tree" 
};


class anim_bin_tree {

GRAPH<point,int> G;

int       kind;
bin_tree* T;
GraphWin* gwp;

map<bin_tree_node*,node> NODE;


void set_node_params(bin_tree_node* r)
{ 
  node v = NODE[r];

  if (r->is_leaf())
  { gwp->set_color(v,ivory);
    gwp->set_label_type(v,user_label);
    gwp->set_user_label(v,string("%d",T->key(r)));
    gwp->set_shape(v,rectangle_node);
    return;
   }

  if (r == T->get_root())
    gwp->set_shape(v,rectangle_node);
  else
    gwp->set_shape(v,ellipse_node);

  gwp->set_color(v,grey1);

  int bal = r->get_bal();
  gwp->set_label_type(v,no_label);
  switch (kind) {
    case  AVL_TREE:
             gwp->set_label_type(v,user_label);
             switch (bal) {
               case  0: gwp->set_label(v,"=");
                        break;
               case -1: gwp->set_label(v,">");
                        break;
               case  1: gwp->set_label(v,"<");
                        break;
              }
             break;

    case  BB_TREE: 
             gwp->set_label_type(v,user_label);
             gwp->set_label(v,string("%d",bal));
             break;

    case  RB_TREE: 
             gwp->set_color(v,(bal == 0) ? red : grey3);
             break;

   }
}



node scan_tree(bin_tree_node* r, double x0, double x1, double y, double dy)
{ node w  = NODE[r];
  node v;
  if (w == 0)
  { v = gwp->new_node(point(0,0));
    NODE[r] = v;
   }
  else v = w;

  set_node_params(r);

  double x = (x0 + x1)/2;
  G[v] = point(x,y);

  bin_tree_node* left  = T->l_child(r);
  bin_tree_node* right = T->r_child(r);
  edge e = 0;
  if (left)  e = gwp->new_edge(v,scan_tree(left,x0,x,y-dy,dy));
  if (right) e = gwp->new_edge(v,scan_tree(right,x,x1,y-dy,dy));
  if (w == 0) gwp->set_position(v,gwp->get_position(target(e)));
  return v;
}



public:

anim_bin_tree(GraphWin& gw, int k) : NODE(0) 
{ 
  G.clear();
  gw.set_graph(G);
  gw.message(string("\\bf\\blue %s",NAME[k]));
  gwp = &gw; 

  kind = k;
  switch (k) { 
  case BIN_TREE: T = new bin_tree;
                 break;
  case AVL_TREE: T = new avl_tree;
                 break;
  case RB_TREE:  T = new rb_tree;
                 break;
  case BB_TREE:  T = new bb_tree;
                 break;
  case RS_TREE:  T = new rs_tree;
                 break;
  }
}


~anim_bin_tree() { delete T; }


//void insert(int x, int y) 
void insert(long x, long y) 
{ 
  gwp->set_flush(false);

  bin_tree_node* p = T->insert((void*)x,(void*)y); 
  bin_tree_node* q = nil;

  if (T->size() >= 2)
  { bin_tree_node* f = T->parent(p); 
    q = (p == T->l_child(f)) ? T->succ(p) : T->pred(p);
   }

  node v = NODE[p];

  if (v == nil)
  { point pos; 
    if (q) pos = gwp->get_position(NODE[q]);
    v = gwp->new_node(pos);
    NODE[p] = v;
    set_node_params(p);
   }

  edge e;
  forall_edges(e,G) gwp->del_edge(e);

  double dx = gwp->get_window().pix_to_real(4);
  double x1 = gwp->get_xmax() - dx;
  double x0 = gwp->get_xmin() + dx;
  double y0 = gwp->get_ymin();
  double y1 = gwp->get_ymax();
  double dy = (y1-y0)/10;

  scan_tree(T->get_root(),x0,x1,y1-2*dy,dy);

  gwp->update_graph();

  color col = gwp->set_color(v,green2);

  gwp->set_flush(true);
  gwp->set_position(G.node_data());
  gwp->set_color(v,col);
}

};



int main()
{
  GraphWin gw;

  gw.set_node_width(18);
  gw.set_node_height(18);
  gw.set_node_label_type(no_label);
  gw.set_node_label_font(roman_font,10);
  gw.set_edge_direction(undirected_edge);

  gw.display(window::center,window::center);


  int n = 16;
  int input = 0;
  int kind  = RB_TREE;

  panel P("Binary Tree Demo");

  P.choice_item("tree type",kind,
                "unbalanced","avl-tree","bb[alpha]","red-black","randomized");
  P.choice_item("input data",input,"random", "1 2 3 ...");
  P.int_item("# inserts",n,0,64);
  P.button("ok",0);
  P.button("exit",1);
  P.button("forever",2);

  int but = 0;

  for(;;)
  {
    if (but != 2) but = gw.open_panel(P);

    if (but == 1) break;

    anim_bin_tree T(gw,kind);
  
    if (input==0)
     { int i;
       for(i=0;i<n;i++) T.insert(rand_int(0,99),0);
      }
    else
     { int i;
       for(i=0;i<n;i++) T.insert(i,0);
      }

    if (but != 2)
      { gw.message("Press \\blue done \\black to continue.");
        gw.edit();
       }
    else
      { leda_wait(2);
        gw.message("");
        kind =  (kind+1) % 5;
       }
   }
  
  return 0;
}

