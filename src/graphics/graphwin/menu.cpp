/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  menu.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

#include <LEDA/graphics/pixmaps/zoom_in20.xpm>
#include <LEDA/graphics/pixmaps/zoom_best_fit20.xpm>
#include <LEDA/graphics/pixmaps/zoom_out20.xpm>
#include <LEDA/graphics/pixmaps/undo20.xpm>
#include <LEDA/graphics/pixmaps/redo20.xpm>

#include <LEDA/system/file.h>

#include "graphwin_undo.h"
#include "local.h"

#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

#if defined(__hpuxcc__)
// because of a bug of hpuxcc ...
typedef void (*embed_func_ptr)(graph&, node_array<double>&,
                                       node_array<double>&); 
#else
typedef void (*embed_func_ptr)(const graph&, node_array<double>&,
                                             node_array<double>&); 
#endif


void GraphWin::call_menu_func(int n) 
{
  graph& G = get_graph();

  gw_base_function* mf = menu_functions[n];

  if ( ! mf->enabled ) return; 

  if (gw_undo == 0) 
  { mf->call(*this); 
    return;
   }

  bool do_undo = (n != undo_button) && (n != redo_button) && (n != done_button);

  if (do_undo) 
  { window* cwin = window::get_call_window();
    panel_item it;
    forall_items(it,*menu_p) 
    { if (menu_p->get_window(it) == cwin) 
      { do_undo = string(menu_p->get_label(it)) != "File";
        //do_undo = false; // no undo for I/O
        break;
       }
     }
   }

  if (do_undo) 
  { gw_undo->start_complex_command();
    changing_layout(G.all_nodes(),G.all_edges());
   }

  mf->call(*this); 

  if (do_undo) gw_undo->finish_complex_command();

  if (gw_undo->size() > 0) 
     menu_p->enable_button(undo_button); 
  else 
     menu_p->disable_button(undo_button);

  if (gw_undo->redo_size() > 0) 
     menu_p->enable_button(redo_button); 
  else 
     menu_p->disable_button(redo_button);

}



void gw_call_menu_func(int i) {
  window*   wp = window::get_call_window();
  GraphWin* gw = wp->get_graphwin();
  bool save_flush = gw->set_flush(true);
  gw->call_menu_func(i);
  gw->set_flush(save_flush);
  gw->update_status_line();
}



int GraphWin::add_menu(string label, window& M, char* pmap)
{ init_menu();
  if (pmap)
    { menu_p->make_menu_bar(2);
      return win_p->button(pmap,pmap,label,M,label);
     }
  else
    return win_p->button(label,M);
}



int GraphWin::add_menu(string label, int sub_menu, char* pmap, const char* hlp) 
{ 
  init_menu();

  assert(sub_menu_counter < MAX_ITEM_NUM);

  menu *m_sub=new menu;
  m_sub->grawin_ptr = this;
  sub_menus[sub_menu_counter]=m_sub;

  int but = sub_menus[sub_menu]->get_button(label);

  if (but >= 0)
   { sub_menus[sub_menu]->set_window(but,m_sub);
     sub_menus[sub_menu]->set_action(but,0);
    }
  else
  { if (pmap)
      { menu_p->make_menu_bar(2);
        sub_menus[sub_menu]->button(pmap,pmap,label,*m_sub,label);
       }
    else
       sub_menus[sub_menu]->button(label,*m_sub,hlp);
   }

  return sub_menu_counter++;  
}


int GraphWin::get_menu(string label) 
{ if (label == "Help") label = " ? ";
  panel_item it = menu_p->get_item(label);
  if (it == 0) return -1;
  menu* m_sub = (menu*)(menu_p->get_window(it));
  if (m_sub == 0) return -1;
  int i = 0;
  while (i < sub_menu_counter && sub_menus[i] != m_sub) i++;
  return (i < sub_menu_counter) ? i : -1;
}




int GraphWin::add_call(gw_base_function* f,string label,int sub_menu,char* pmap)
{ 
  bool make_mbar = true;
  int space = -16;

  if (sub_menu < 0)
  { make_mbar = false;
    if (sub_menu < -1) space = -sub_menu;
    sub_menu = 0;
   }

  space = int(space*window::screen_dpi_scaling());

  init_menu();

  assert(call_entry_counter < MAX_ITEM_NUM);

  int but = sub_menus[sub_menu]->get_button(label);

  menu_functions[call_entry_counter]=f;

  if (but >= 0)
    { sub_menus[sub_menu]->set_value(but,call_entry_counter);
      sub_menus[sub_menu]->set_action(but,gw_call_menu_func);
      sub_menus[sub_menu]->set_window(but,0);
     }
  else
    { 
      if (pmap)
      { if (make_mbar) menu_p->make_menu_bar(2);
        sub_menus[sub_menu]->button(pmap,pmap,label,call_entry_counter,
                                                   gw_call_menu_func,label);
        sub_menus[sub_menu]->hspace(space);
       }
      else
        sub_menus[sub_menu]->button(label,call_entry_counter,gw_call_menu_func);
     }

  f->enabled = true;
  return call_entry_counter++;
}



int GraphWin::add_call(gw_base_function* f,string label,int sub_menu,
                                                        int bm_w, int bm_h,
                                                        unsigned char* bm_bits)
{ 
  init_menu();

  assert(call_entry_counter < MAX_ITEM_NUM);

  int but = sub_menus[sub_menu]->get_button(label);

  menu_functions[call_entry_counter]=f;

  if (but >= 0)
    { sub_menus[sub_menu]->set_value(but,call_entry_counter);
      sub_menus[sub_menu]->set_action(but,gw_call_menu_func);
      sub_menus[sub_menu]->set_window(but,0);
     }
  else
    { menu_p->make_menu_bar(2);
      sub_menus[sub_menu]->button(bm_w,bm_h,bm_bits,label,call_entry_counter,
                                                    gw_call_menu_func,label);
     }

  f->enabled = true;
  return call_entry_counter++;
}



void GraphWin::add_separator(int sub_menu) { 
 sub_menus[sub_menu]->new_line(); 
}


void GraphWin::set_default_menu(long mask) {
  menu_mask = mask;
}



//----------------------------------------------------------------------------
// add_simple_call 
//----------------------------------------------------------------------------

class gw_simple_function : public gw_base_function 
{
  typedef void (*func_t)(GraphWin&);
  typedef void (*call_t)(GraphWin&,func_t);

  func_t func;
  call_t caller;

public :
  gw_simple_function(func_t f, call_t c) : func(f), caller(c) {};

  void call(GraphWin& gw) { 
    if (caller) caller(gw,func); else func(gw); 
  } 
};


int GraphWin::add_simple_call(void (*func)(GraphWin&), string label,
                                                       int menu_id,
                                                       char* pmap) 
{ 
  gw_base_function* bf = new gw_simple_function(func,NULL);
  return add_call(bf,label,menu_id,pmap);
} 

int GraphWin::add_simple_call(void (*func)(GraphWin&), string label,
                                                       int menu_id,
                                                       int bm_w, int bm_h,
                                                       unsigned char* bm_bits) 
{ 
  gw_base_function* bf = new gw_simple_function(func,NULL);
  return add_call(bf,label,menu_id,bm_w,bm_h,bm_bits);
} 




//----------------------------------------------------------------------------
// add_member_call 
//----------------------------------------------------------------------------

class gw_member_function : public gw_base_function 
{
  typedef void (GraphWin::*func_t)();
  typedef void (*call_t)(GraphWin&,func_t);

  func_t func;
  call_t caller;

public :
  gw_member_function(func_t f, call_t c) : func(f), caller(c) {};

  void call(GraphWin& gw) { 
    if (caller) caller(gw,func); else (gw.*func)(); 
  } 
};


int GraphWin::add_member_call(void (GraphWin::*func)(), string label, 
                                                        int menu_id, 
                                                        char* pmap) 
{ gw_base_function* bf = new gw_member_function(func,NULL);
  return add_call(bf,label,menu_id,pmap);
 }


int GraphWin::add_member_call(void (GraphWin::*func)(), string label, 
                                                        int menu_id, 
                                                        int bm_w, int bm_h,
                                                        unsigned char* bm_bits) 
{ gw_base_function* bf = new gw_member_function(func,NULL);
  return add_call(bf,label,menu_id,bm_w,bm_h,bm_bits);
 }




//----------------------------------------------------------------------------


static void gw_help_handler(GraphWin& gw) 
{ window* wp = window::get_call_window();
  int b = window::get_call_button();
  string label = wp->get_button_label(b);
  gw.display_help_text(label);
}



static void make_yes_panel(panel& P, string s, bool proof=false)
{ P.text_item("");
  P.text_item("\\bf This graph is \\blue " + s + "\\black.");
  P.button("ok");
  if (proof) P.fbutton("proof",1);
}


static void make_no_panel(panel& P, string s, bool proof=false)
{ P.text_item("");
  P.text_item("\\bf This graph is \\red not " + s + "\\black.");
  P.button("ok",0);
  if (proof) P.fbutton("proof",1);
}


void gw_test_simplicity(GraphWin& gw)
{ 
  panel P;
  
  if (Is_Simple(gw.get_graph()))
     make_yes_panel(P,"simple");
  else
     make_no_panel(P,"simple");

  gw.open_panel(P);
}



void gw_test_bidirectedness(GraphWin& gw)
{ 
  panel P;
  
  if (Is_Bidirected(gw.get_graph()))
     make_yes_panel(P,"bidirected");
  else
     make_no_panel(P,"bidirected");

  gw.open_panel(P);
}


void gw_test_bipartiteness(GraphWin& gw)
{ graph& G = gw.get_graph();
  list<node> A,B;
  panel P;
  if (Is_Bipartite(G,A,B))
    make_yes_panel(P,"bipartite",true);
  else
    make_no_panel(P,"bipartite",true);
  if (gw.open_panel(P)) gw_test_bipart(gw);
}


void gw_test_connectivity(GraphWin& gw)
{ graph& G = gw.get_graph();
  panel P;
  if (Is_Connected(G))
    make_yes_panel(P,"connected",true);
  else
    make_no_panel(P,"connected",true);
  if (gw.open_panel(P)) gw_test_con(gw);
}



void gw_test_biconnectivity(GraphWin& gw)
{ graph& G = gw.get_graph();
  panel P;
  if (Is_Biconnected(G))
  { make_yes_panel(P,"biconnected");
    gw.open_panel(P);
    return;
   }
  make_no_panel(P,"biconnected",true);
  if (gw.open_panel(P)) gw_test_bicon(gw);
}


static void gw_test_triconnectivity(GraphWin& gw)
{ graph& G = gw.get_graph();
  panel P;
  if (Is_Triconnected(G))
  { make_yes_panel(P,"triconnected");
    gw.open_panel(P);
    return;
   }
  make_no_panel(P,"triconnected",true);
  if (gw.open_panel(P)) gw_test_tricon(gw);
}


void gw_test_planarity(GraphWin& gw)
{ graph& G = gw.get_graph();
  panel P;
  if (Is_Planar(G))
     make_yes_panel(P,"planar",true);
  else
     make_no_panel(P,"planar",true);

  if (gw.open_panel(P)) gw_test_planar(gw);
}


void gw_test_spness(GraphWin& gw)
{ graph& G = gw.get_graph();
  panel P;
  if (Is_Series_Parallel(G))
    { make_yes_panel(P,"series-parallel",true);
      if (gw.open_panel(P)) gw_test_series_parallel(gw);
     }
  else
    { make_no_panel(P,"series-parallel",false);
      gw.open_panel(P);
     }
}


//----------------------------------------------------------------------------

/*
static void embed_grid(const graph& G, node_array<double>& x,
                                       node_array<double>& y)
{ int n = G.number_of_nodes();
  if (n == 0) return;
  n = (int) (sqrt((float)n));
  if (n*n < G.number_of_nodes()) ++n;
  double d = 1.0/(n-1);
  int i = 0;
  node v;
  forall_nodes(v,G) 
  { x[v] = d*(i%n);
    y[v] = d*(i/n);
    i++;
  }
}
*/
  
//----------------------------------------------------------------------------

static void gw_snap_to_grid(GraphWin& gw)
{
  window& W = gw.get_window();

  int d = gw.get_grid_dist();
  int s = gw.get_grid_style();
 
  if (d == 0) d = 10;

  panel P;
  P.text_item("\\bf\\blue Snap to Grid");
  P.text_item("");
  P.int_item("grid dist",d,1,40);
  P.choice_item("grid style",s,"invisible", "point grid","line grid");
  P.buttons_per_line(3);
  P.fbutton("snap",1);
  P.button("grid",2);
  P.button("cancel",0);

  int but = P.open(W);

  if (but == 0) return;


  if (but == 2)
  { gw.set_grid_style((grid_style)s);
    gw.set_grid_dist(d);
    return;
   }

  int d0 = gw.set_grid_dist(d);
  gw.snap_to_grid();
  gw.set_grid_dist(d0);

  gw.embed_edges();
  gw.redraw();
}

//----------------------------------------------------------------------------

static void embed_circular(const graph& G, node_array<double>& x,
                                           node_array<double>& y) 
{ 
  int n = G.number_of_nodes();
  if (n == 0) return;
  point  m = point(0.5,0.5);
  double alpha = 0;
  double step  = 6.2832/n;
  node v;
  forall_nodes(v,G) 
  { point p = m.translate_by_angle(alpha,0.5);
    x[v] = p.xcoord();
    y[v] = p.ycoord();
    alpha+=step;
  }
}

//----------------------------------------------------------------------------

static void embed_random(const graph& G, node_array<double>& x,
                                         node_array<double>& y) 
{ random_source ran;
  node v;
  forall_nodes(v,G)  ran >> x[v] >> y[v];
 }


//----------------------------------------------------------------------------

static void embed_bigraph(const list<node>& A, const list<node>& B,
                                               node_array<double>& x, 
                                               node_array<double>& y) 
{ 
  double d=1.0/(A.size()-1);
  double f=0;
  node v;
  forall(v,A) { x[v] = 0; y[v] = f; f+=d; }

  d=1.0/(B.size()-1);
  f=0;
  forall(v,B) { x[v] = 1; y[v] = f; f+=d; }
}

//----------------------------------------------------------------------------



static bool gen_dialog(GraphWin& gw, string s, int* n, int* m, string* l, 
                                                       const list<string>* M,
                                                       bool* directed)
{
  panel P;

  P.text_item("\\bf\\blue Create a " + s + " graph");
  P.text_item("");
  if (n) P.int_item("Nodes",*n,0,200);
  if (m) P.int_item("Edges",*m,0,200);
  if (l) P.string_item("Layout",*l,*M,5);
  if (directed) {
    P.text_item("");
    P.bool_item("directed",*directed);
  }

  P.fbutton("ok",1);
  P.button("cancel",0);

  int but =  gw.open_panel(P);

  return but != 0;
}


static bool gen_dialog_n(GraphWin& gw, string s, string& l, 
                                                 const list<string>& M,
                                                 bool& directed)
{ int n = gw.get_gen_nodes();
  bool res = gen_dialog(gw,s,&n,0,&l,&M,&directed);
  gw.set_gen_nodes(n);
  return res;
 }

static bool gen_dialog_n(GraphWin& gw, string s, bool& directed)
{ int n = gw.get_gen_nodes();
  bool res = gen_dialog(gw,s,&n,0,0,0,&directed);
  gw.set_gen_nodes(n);
  return res;
 }

static bool gen_dialog_nm(GraphWin& gw, string s, string& l, 
                                                  const list<string>& M,
                                                  bool& directed)
{ int n = gw.get_gen_nodes();
  int m = gw.get_gen_edges();
  bool res = gen_dialog(gw,s,&n,&m,&l,&M,&directed);
  gw.set_gen_nodes(n);
  gw.set_gen_edges(m);
  return res;
 }

static bool gen_dialog_nm(GraphWin& gw, string s, string& l, 
                                                  const list<string>& M)
{ int n = gw.get_gen_nodes();
  int m = gw.get_gen_edges();
  bool res = gen_dialog(gw,s,&n,&m,&l,&M,0);
  gw.set_gen_nodes(n);
  gw.set_gen_edges(m);
  return res;
 }




static bool gen_dialog_nm(GraphWin& gw, string s, bool& directed)
{ int n = gw.get_gen_nodes();
  int m = gw.get_gen_edges();
  bool res = gen_dialog(gw,s,&n,&m,0,0,&directed);
  gw.set_gen_nodes(n);
  gw.set_gen_edges(m);
  return res;
 }


//----------------------------------------------------------------------------

static void gen_complete_graph(GraphWin& gw, void (*f)(graph&,int))
{
  list<string> L;
  L.append("random");
  L.append("circle");
  L.append("spring");

  bool directed = false;
 
  string layout = "circle";

  if (!gw.check_init_graph_handler()) return;

  if (!gen_dialog_n(gw,"complete",layout,L,directed)) return; 

  int n = gw.get_gen_nodes();

  bool old_flush=gw.set_flush(false); 

  graph& G = gw.get_graph();

  G.clear();

  gw.set_default_win_coords();
  gw.zoom(0);

  gw_node_shape ns = gw.get_node_shape();
  gw.init_default();
  gw.set_node_shape(ns);

  gw.set_directed(directed);

  f(G,n);

  int m = G.number_of_edges();

  gw.update_graph();

  gw_random_embed(gw);

  if (layout == "circle") 
  { if (n+m < 250) gw.set_flush(true);
    gw_circle_embed(gw);
   }

  if (layout == "spring")
  { if (n+m < 250) gw.set_flush(true);
    gw_spring_embed(gw);
   }

  gw.set_flush(old_flush);
  gw.redraw();
  gw.call_init_graph_handler();
}


//----------------------------------------------------------------------------

static void gen_random_graph(GraphWin& gw, void (*f)(graph&,int,int)) 
{

  if (!gw.check_init_graph_handler()) return;

  string name = "random";

  bool directed = false;

//string layout = "random";
  string layout = "spring";

  list<string> L;
  L.append("random");
  L.append("circle");
  L.append("spring");

  void (*plan)(graph&,int,int) = random_planar_graph;
  void (*sp)(graph&,int,int) = random_sp_graph;

  if (f == plan)
  { name = "random planar";
    L.append("orthogonal");
    L.append("straight-line");
   }

  if (f == sp) 
  { name = "random series-parallel";
    L.append("series-parallel");
   }
 

  if (!gen_dialog_nm(gw,name,layout,L,directed)) return;
  int n = gw.get_gen_nodes();
  int m = gw.get_gen_edges();

  bool old_flush=gw.set_flush(false); 

  graph& G = gw.get_graph();

  G.clear();

  // causes crash
  //gw.update_graph();

  gw.set_default_win_coords();
  gw.zoom(0);

  gw_node_shape ns = gw.get_node_shape();
  gw.init_default();
  gw.set_node_shape(ns);

  gw.set_directed(directed);

/*
  bool undir = G.is_undirected();
  if (undir) G.make_directed();
*/

  f(G,n,m);

//if (undir) G.make_undirected();

  gw.update_graph();

  gw_random_embed(gw);

  if (n+m < 500) gw.set_flush(true);

  if (layout == "circle")          gw_circle_embed(gw);
  if (layout == "spring")          gw_spring_embed(gw);
  if (layout == "orthogonal")      gw_ortho_embed(gw);
  if (layout == "series-parallel") gw_sp_embed(gw);
  if (layout == "straight-line")   gw_straight_embed1(gw);

  gw.set_flush(old_flush);
  gw.redraw();
  gw.call_init_graph_handler();
}



//----------------------------------------------------------------------------

static void gen_complete_bigraph(graph& G,int a, int b, int, list<node>& A, 
                                                             list<node>& B)
{ complete_bigraph(G,a,b,A,B); }

static void gen_random_bigraph(graph& G,int a, int b, int m, list<node>& A, 
                                                             list<node>& B) 
{ random_bigraph(G,a,b,m,A,B); }


static void gen_bigraph(GraphWin& gw, 
                        void (*f)(graph&,int,int,int,list<node>&,list<node>&)) 
{
  if (!gw.check_init_graph_handler()) return;

  bool directed = false;

  if (f == gen_complete_bigraph) {
    if (!gen_dialog_n(gw,"complete bipartite",directed)) return;
  }
  else {
    if (f == gen_random_bigraph) 
      if (!gen_dialog_nm(gw,"random bipartite",directed)) return;
  }

  int n=gw.get_gen_nodes();
//int e=gw.get_gen_edges();

  bool old_flush=gw.set_flush(false);

  graph& G = gw.get_graph();

  G.clear();

  gw.set_default_win_coords();
  gw.zoom(0);

  gw_node_shape ns = gw.get_node_shape();
  gw.init_default();
  gw.set_node_shape(ns);

  gw.set_directed(directed);

/*
  bool undir = G.is_undirected();
  if (undir) G.make_directed();
*/

  list<node> A,B;
  f(G,n/2,n-n/2,gw.get_gen_edges(),A,B);
  node_array<double> x(G);
  node_array<double> y(G);
  embed_bigraph(A,B,x,y);

//if (undir) G.make_undirected();

  gw.update_graph();
  gw.adjust_coords_to_win(x,y);

  gw.set_position(x,y);
  gw.embed_edges();
  gw.set_flush(old_flush);
  gw.redraw();
  gw.call_init_graph_handler();
}

//----------------------------------------------------------------------------


static void gen_planar_graph(GraphWin& gw, 
              void (*f)(graph&,node_array<double>&,node_array<double>&,int)) 
{
  if (!gw.check_init_graph_handler()) return;

  bool directed = false;

  void (*gg)(graph&,node_array<double>&,node_array<double>&,int) 
          = grid_graph;

  void (*tg)(graph&,node_array<double>&,node_array<double>&,int) 
          = triangulated_planar_graph;

  string s;
  if (f == gg) s = "grid";
  if (f == tg) s = "triangulated planar";


  if (!gen_dialog_n(gw,s,directed)) return;

  int n = gw.get_gen_nodes();

  if (s == "grid") n = int(sqrt(float(n)));

  bool old_flush=gw.set_flush(false); 

  graph& G = gw.get_graph();

  G.clear();

  gw.set_default_win_coords();
  gw.zoom(0);

  gw_node_shape ns = gw.get_node_shape();
  gw.init_default();
  gw.set_node_shape(ns);

  gw.set_directed(directed);


  node_array<double> x;
  node_array<double> y;

/*
  bool undir = G.is_undirected();
  if (undir) G.make_directed();
*/

  f(G,x,y,n);

//if (undir) G.make_undirected();

  gw.update_graph();
  gw.adjust_coords_to_win(x,y);
  gw.set_position(x,y);

  if (gw.get_grid_dist() != 0) gw.snap_to_grid();

  gw.embed_edges();
  gw.set_flush(old_flush);
  gw.redraw();
  gw.call_init_graph_handler();
}



static void gen_d3_grid_graph(GraphWin& gw) 
{
  if (!gw.check_init_graph_handler()) return;

  bool directed = false;

  if (!gen_dialog_n(gw,"3-dimensional grid",directed)) return;

  double n0 = gw.get_gen_nodes();
  int n = int(exp(log(n0)/3));

  bool old_flush=gw.set_flush(false); 

  graph& G = gw.get_graph();

  G.clear();

  gw.set_default_win_coords();
  gw.zoom(0);

  gw_node_shape ns = gw.get_node_shape();
  gw.init_default();
  gw.set_node_shape(ns);

  gw.set_directed(directed);


  node_array<double> x;
  node_array<double> y;
  node_array<double> z;

/*
  bool undir = G.is_undirected();
  if (undir) G.make_directed();
*/

  d3_grid_graph(G,x,y,z,n);

//if (undir) G.make_undirected();

  gw.update_graph();
  gw.adjust_coords_to_win(x,y);
  gw.set_position(x,y);
  if (gw.get_grid_dist() != 0) gw.snap_to_grid();
  gw.embed_edges();

  gw.set_flush(true);
  gw_d3_spring_embed(gw);

  gw.set_flush(old_flush);
  gw.redraw();
  gw.call_init_graph_handler();
}


//----------------------------------------------------------------------------

static void general_embed(GraphWin& gw, embed_func_ptr embedder) 
{
  // if (!gw.check_init_graph_handler()) return;

  graph& G = gw.get_graph();

  double rad = DefNodeRadius1;

  node_array<double> x(G);
  node_array<double> y(G);
  node_array<double> r(G,rad);
  edge_array<list<double> > b(G);
  edge_array<double> a(G,0); 

  embedder(G,x,y);
  gw.adjust_coords_to_win(x,y);
  gw.set_layout(x,y,r,r,b,b,a,a,a,a);
  gw.call_init_graph_handler();
  if (!gw.get_flush()) gw.redraw();
}  

//----------------------------------------------------------------------------

typedef  list<edge> (*aux_func1)(graph&);

static void gw_make(GraphWin& gw, list<edge> (*f)(graph&)) 
{ 
  graph& G = gw.get_graph();

  f(G);

  aux_func1 g = Make_Bidirected;
  if (f == g) G.make_map();

  gw.update_edges();
  if (!gw.get_flush()) {
    gw.embed_edges();
    gw.redraw();
  }
}

//----------------------------------------------------------------------------

static void gw_del_loops(GraphWin& gw) 
{ if (!Delete_Loops(gw.get_graph()).empty()) {
    gw.update_edges();
    if (!gw.get_flush()) {
      gw.embed_edges();
      gw.redraw();
    }
  }
}

//----------------------------------------------------------------------------

static void setup_selected_nodes(GraphWin& gw) 
{ bool old_flush=gw.set_flush(true);
  const list<node>& L=gw.get_selected_nodes();
  if (!L.empty()) 
  { if (!gw_setup(gw,L)) 
      gw.deselect_all_nodes();
   }
  gw.set_flush(old_flush);
}

static void setup_all_nodes(GraphWin& gw) 
{ gw.select_all_nodes();
  setup_selected_nodes(gw);
}

//----------------------------------------------------------------------------

static void node_setup(GraphWin& gw) 
{ bool old_flush=gw.set_flush(true);
  list<node> empty_list;
  gw_setup(gw,empty_list);
  gw.set_flush(old_flush);
}

//----------------------------------------------------------------------------

static void setup_selected_edges(GraphWin& gw) 
{ bool old_flush=gw.set_flush(true);
  const list<edge>& L=gw.get_selected_edges();
  if (!L.empty()) 
  { if (!gw_setup(gw,L)) 
      gw.deselect_all_edges();
   }
  gw.set_flush(old_flush);
}

static void setup_all_edges(GraphWin& gw) 
{ gw.select_all_edges();
  setup_selected_edges(gw);
}

//----------------------------------------------------------------------------

static void edge_setup(GraphWin& gw) {
  bool old_flush=gw.set_flush(true);
  list<edge> empty_list;
  gw_setup(gw,empty_list);
  gw.set_flush(old_flush);
}

//----------------------------------------------------------------------------

static void del_all_nodes(GraphWin& gw) { gw.clear_graph(); }

static void del_selected_nodes(GraphWin& gw) 
{ bool b = gw.set_flush(false);
  list<node> L = gw.get_selected_nodes();
  node v;
  forall(v,L) gw_del_node(gw,v);
  gw.redraw();
  gw.set_flush(b);
}

static void del_all_edges(GraphWin& gw) 
{ bool b = gw.set_flush(false);
  list<edge> L = gw.get_graph().all_edges();
  edge e;
  forall(e,L) gw_del_edge(gw,e);
  gw.redraw();
  gw.set_flush(b);
}

static void del_selected_edges(GraphWin& gw) 
{ bool b = gw.set_flush(false);
  list<edge> L = gw.get_selected_edges();
  edge e;
  forall(e,L) gw_del_edge(gw,e);
  gw.redraw();
  gw.set_flush(b);
}


//----------------------------------------------------------------------------

static void show_faces(GraphWin& gw)
{
  graph& G = gw.get_graph();

  if (!G.is_map())
  { bool convert = gw.get_window().confirm("Show Faces: Graph must be a map. Convert it?");
	if (!convert) return;
	list<edge> dummy; G.make_map(dummy);
	gw.update_graph();
   }

  gw.save_edge_attributes();
  bool flush = gw.set_flush(false);
  int  edist = gw.set_edge_distance(6);

  G.compute_faces();

  int c = 2;
  face f;
  forall_faces(f,G)
  { edge e;
    forall_face_edges(e,f) gw.set_color(e,color(c));
    if (++c == 16) c = 2;
   }

/*
  node v;
  forall_nodes(v,G)
  { edge e;
    c = 0;
    forall_adj_edges(e,v) gw.set_label(e,string("%d",c++));
  }
*/

  gw.redraw();
  string msg("\\bf Faces: %d ~~~Genus: %d",G.number_of_faces(),Genus(G));
  gw.wait(msg + "\\rm\\blue ~~~(press done)");

  gw.set_edge_distance(edist);
  gw.restore_edge_attributes();
  gw.redraw();
  gw.set_flush(flush);
}

//----------------------------------------------------------------------------

void gw_new_graph(GraphWin& gw)
{
  static int new_graph_count = 0;

  string name = string("graph%d",++new_graph_count);

  panel P;
  P.buttons_per_line(2);
  P.text_item("");
  P.text_item("\\bf\\blue New Graph");
  P.text_item("");
  P.string_item("Name", name);
  
  if (gw.unsaved_changes())
  { P.text_item("");
    P.text_item("Current graph not written since last change !"); 
  }

  P.fbutton("ok",0);
  P.button("cancel",1);
  if (gw.open_panel(P) != 0) return;

  gw.set_graphname(name);

  if (gw.gw_undo) delete gw.gw_undo;
  gw.clear_graph();
  gw.gw_undo = new graphwin_undo(gw);
}

//----------------------------------------------------------------------------


void GraphWin::init_menu() 
{
  if (sub_menu_counter > 0) return;

  call_entry_counter=0;
  sub_menus[0] = menu_p;
  sub_menu_counter = 1;

  // if (menu_p != win_p) set_buttons_per_line(buttons_per_line);

  if (menu_mask & M_FILE) 
  { 
    int file_menu = add_menu("File");

    add_simple_call(gw_new_graph, "New   ", file_menu);
    add_separator(file_menu);

    if (!(menu_mask & M_READONLY))
    { add_simple_call(gw_load_handler,     "Open   ",file_menu);
/*
      int load_menu = add_menu("Load",file_menu);
      add_simple_call(gw_load_handler,        "GW  Graph", load_menu);
      add_simple_call(gw_load_gml_handler,    "GML Graph", load_menu);
      add_simple_call(gw_load_dimacs_handler, "Dimacs",    load_menu);
*/
     }

    if (window::display_type() != "xx")
    { int save_but = add_simple_call(gw_save_handler,     "Save",file_menu);
/*
      int save_menu = add_menu("Save",file_menu);
      add_simple_call(gw_save_handler,        "GW  graph", save_menu);
      add_simple_call(gw_save_gml_handler,    "GML graph", save_menu);
      add_simple_call(gw_save_dimacs_handler, "Dimacs",    save_menu);
*/

      int ex_menu = add_menu("Export     ",file_menu);
      add_simple_call(gw_save_svg_handler,  "SVG Image",         ex_menu);
      add_separator(ex_menu);
      add_simple_call(gw_save_ps_handler,   "Postscript",       ex_menu);
      add_simple_call(gw_save_tex_handler,  "Latex/Postscript  ", ex_menu);
      add_separator(ex_menu);
      add_simple_call(gw_screenshot_handler,"Screenshot",       ex_menu);
  
      int x = add_simple_call(gw_metafile_handler, "Windows Metafile", ex_menu);
      int y = add_simple_call(gw_clipboard_handler,"Copy to Clipboard",ex_menu);
  
#if !defined(__win32__)
      sub_menus[ex_menu]->disable_button(x);
      sub_menus[ex_menu]->disable_button(y);
#endif
     }

    if (window::display_type() == "xx")
    { 
      add_separator(file_menu);
      add_simple_call(gw_upload_handler, "Upload  ",file_menu);

      int down_menu = add_menu("Download    ",file_menu);
      add_simple_call(gw_save_gml_handler,"GML Graph", down_menu);
      add_simple_call(gw_save_gw_handler, "GW  Graph",  down_menu);
      add_separator(down_menu);
      add_simple_call(gw_save_svg_handler,"SVG Image", down_menu);
    //add_simple_call(gw_save_ps_handler, "PostScript", down_menu);

/*  disable save and export in xx mode
      int i = sub_menus[file_menu]->get_button("Save");
      sub_menus[file_menu]->disable_button(i);
    
      int j = sub_menus[file_menu]->get_button("Export     ");
      sub_menus[file_menu]->disable_button(j);
*/
     }

/*
    add_separator(file_menu);
    add_simple_call(gw_print_ps_handler,"Print  ", file_menu);
*/

    add_separator(file_menu);

    int help_menu = add_menu("Help",file_menu);

    add_member_call(&GraphWin::help_about,       "About",    help_menu);
    add_member_call(&GraphWin::help_mouse_left,  "Button 1", help_menu);
    add_member_call(&GraphWin::help_mouse_middle,"Button 2", help_menu);
    add_member_call(&GraphWin::help_mouse_right, "Button 3", help_menu);

    add_separator(file_menu);
    exit_button = add_simple_call(exit_handler,"Exit",   file_menu);

   }

  if (menu_mask & M_EDIT) 
  { 
    int edit_menu = add_menu("Edit");

/*
    int x = add_simple_call(edge_setup,"Faces",edit_menu);
    sub_menus[edit_menu]->disable_button(x);
    add_separator(edit_menu);
*/

    int n_edit_menu = add_menu("Nodes",edit_menu);
    add_simple_call(setup_selected_nodes, "selected", n_edit_menu);
    add_simple_call(setup_all_nodes, "existing", n_edit_menu);
    add_simple_call(node_setup,"defaults",n_edit_menu);

    int e_edit_menu = add_menu("Edges",edit_menu);
    add_simple_call(setup_selected_edges,"selected", e_edit_menu);
    add_simple_call(setup_all_edges,"existing", e_edit_menu);
    add_simple_call(edge_setup,"defaults",e_edit_menu);
    add_separator(edit_menu);


    int s_edit_menu = add_menu("Selection   ",edit_menu);

    add_member_call(&GraphWin::select_all_nodes,  "all nodes",  s_edit_menu);
    add_member_call(&GraphWin::deselect_all_nodes,"clear nodes",s_edit_menu);
    add_separator(s_edit_menu);
    add_member_call(&GraphWin::select_all_edges,  "all edges",  s_edit_menu);
    add_member_call(&GraphWin::deselect_all_edges,"clear edges",s_edit_menu);

/*
    add_member_call(&GraphWin::select_all_nodes,"all nodes", s_edit_menu);
    add_member_call(&GraphWin::select_all_edges,"all edges", s_edit_menu);
    add_member_call(&GraphWin::deselect_all,"clear", s_edit_menu);
*/

/*
    int c_edit_menu = add_menu("Clipboard   ",edit_menu);
    int x = add_simple_call(edge_setup,"Copy",c_edit_menu);
    sub_menus[c_edit_menu]->disable_button(x);
    x = add_simple_call(edge_setup,"Cut",c_edit_menu);
    sub_menus[c_edit_menu]->disable_button(x);
    x = add_simple_call(edge_setup,"Paste",c_edit_menu);
    sub_menus[c_edit_menu]->disable_button(x);
*/
  }


  if (menu_mask & M_GRAPH) {

    int graph_menu = add_menu("Graph");
  
    add_member_call(&GraphWin::clear_graph, "Clear   ", graph_menu);
    add_separator(graph_menu);

    int gen_menu = add_menu("Create    ",graph_menu);

    typedef list<edge> edge__list;

    void (*f0)(graph&,int);
    void (*f1)(graph&,int,int);
    void (*f2)(graph&,int,int,int,list<node>&,list<node>&);
    void (*f3)(graph&,node_array<double>&,node_array<double>&,int);
    edge__list (*f4)(graph&);


    // complete

    f0 = complete_ugraph;
    gw_add_call(*this,f0,gen_complete_graph,"complete graph",gen_menu);

    f2 = gen_complete_bigraph;
    gw_add_call(*this,f2,gen_bigraph,"complete bipartite",gen_menu);

    add_separator(gen_menu);


    // random

    //f1 = random_graph;
    //f1 = random_simple_loopfree_graph;
    f1 = random_simple_undirected_graph;
    gw_add_call(*this,f1,gen_random_graph,"random graph",gen_menu);

    f1 = random_planar_graph;
    gw_add_call(*this,f1,gen_random_graph,"random planar",gen_menu);

    f2 = gen_random_bigraph;
    gw_add_call(*this,f2,gen_bigraph,"random bipartite",gen_menu);

    add_separator(gen_menu);


    //special

    f1 = random_sp_graph;
    gw_add_call(*this,f1,gen_random_graph,"series parallel",gen_menu);

    f3 = triangulated_planar_graph;
    gw_add_call(*this,f3,gen_planar_graph,"triangulated",gen_menu);

    f3 = grid_graph;
    gw_add_call(*this,f3,gen_planar_graph,"grid graph",gen_menu);

    add_separator(gen_menu);
    int d3_menu = add_menu("3d",gen_menu);
    add_simple_call(gen_d3_grid_graph,"grid graph",d3_menu);


    int make_menu = add_menu("Make   ",graph_menu);

    f4 = Make_Simple;
    gw_add_call(*this,f4,gw_make,"simple",     make_menu);
    f4 = Make_Connected;
    gw_add_call(*this,f4,gw_make,"connected",  make_menu);
    f4 = Make_Biconnected;
    gw_add_call(*this,f4,gw_make,"biconnected", make_menu);
    f4 = Make_Bidirected;
    gw_add_call(*this,f4,gw_make,"bidirected (map)", make_menu);



    int test_menu = add_menu("Test   ",graph_menu);
 
    add_simple_call(gw_test_simplicity,     "simple",          test_menu);
    add_simple_call(gw_test_bidirectedness, "bidirected",      test_menu);
    add_simple_call(gw_test_bipartiteness,  "bipartite",       test_menu);
    add_simple_call(gw_test_connectivity,   "connected",       test_menu);
    add_simple_call(gw_test_biconnectivity, "biconnected",     test_menu);
    add_simple_call(gw_test_triconnectivity,"triconnected",    test_menu);
    add_simple_call(gw_test_spness,         "series-parallel", test_menu);
    add_simple_call(gw_test_planarity,      "planar",          test_menu);
 

    int clear_menu = add_menu("Delete  ",graph_menu);
    add_simple_call(gw_del_loops,      "loops",          clear_menu);
    add_simple_call(del_selected_nodes,"selected nodes", clear_menu);
    add_simple_call(del_selected_edges,"selected edges", clear_menu);
    add_simple_call(del_all_nodes,     "all nodes",      clear_menu);
    add_simple_call(del_all_edges,     "all edges",      clear_menu);

    int plane_menu = add_menu("Embed  ",graph_menu);
    add_member_call(&GraphWin::sort_edges_by_angle,"edges by angle",plane_menu);
    add_simple_call(show_faces,"show faces",plane_menu);

/*
    int alg_menu = add_menu("Algorithms    ",graph_menu);
    add_simple_call(gw_maxflow, "Maxflow", alg_menu);
*/

 }



  if (menu_mask & M_LAYOUT) {

    int layout_menu = add_menu("Layout");

    // planar layouts

    int pl_menu = add_menu("Planar Layouts   ",layout_menu);

    add_simple_call(gw_ortho_embed,     "Orthogonal",      pl_menu);
    add_simple_call(gw_visrep_embed,    "Visibility Representation",pl_menu);
    add_simple_call(gw_sp_embed,        "Series-Parallel", pl_menu);
    add_simple_call(gw_straight_embed1, "Straight-Line (Fary/FPP)",pl_menu);
    add_simple_call(gw_straight_embed2, "Straight-Line (Schnyder)",pl_menu);
/*
    add_simple_call(gw_d3_sp_embed,     "3D Series-Parallel", pl_menu);
*/


    // spring layouts

    int sl_menu = add_menu("Spring Layouts   ",layout_menu);

    add_simple_call(gw_spring_embed,"2D Spring Embedder",sl_menu);
    add_simple_call(gw_d3_spring_embed,"3D Spring Embedder", sl_menu);


    // simple layouts

    int si_menu = add_menu("Simple Layouts   ",layout_menu);

    gw_add_call(*this,(embed_func_ptr)embed_circular,general_embed,
                                                     "Circular Layout",si_menu);
    gw_add_call(*this,(embed_func_ptr)embed_random,  general_embed,
                                                     "Random Layout", si_menu);


    add_separator(layout_menu);

    // layout tools
    int tool_menu = add_menu("Layout Tools   ",layout_menu);
    add_simple_call(gw_snap_to_grid,      "Snap To Grid",    tool_menu);
    add_member_call(&GraphWin::set_layout,"Remove Bends",    tool_menu);
    add_member_call(&GraphWin::reset_edge_anchors,"Reset Anchors", tool_menu);
    add_separator(tool_menu);
    add_simple_call(gw_place_into_box,    "Fit into Box",    tool_menu);
    add_simple_call(gw_place_into_win,    "Fit into Window", tool_menu);
    add_separator(tool_menu);
    add_simple_call(gw_shrink_graph,      "Shrink Graph",    tool_menu);
    add_simple_call(gw_expand_graph,      "Expand Graph",    tool_menu);
    add_simple_call(gw_center_graph,      "Center Graph",    tool_menu);


    // AGD
/*
    add_separator(layout_menu);
    bool silent = true; // later
    init_agd_menu(layout_menu,silent);
*/


    // GraVis
    // add_separator(layout_menu);
    // init_gravis_menu(layout_menu);
  }



  if (menu_mask & M_WINDOW) 
  {
    int win_menu = add_menu("Window");

  //add_member_call(&GraphWin::redraw,  "Redraw",       win_menu);
  //add_separator(win_menu);

    add_simple_call(gw_zoom_default,   "Default Window  ", win_menu);
    add_simple_call(gw_center_graph,   "Center Graph",    win_menu);
    add_separator(win_menu);
    add_simple_call(gw_zoom_graph,     "Zoom Graph",      win_menu);
    add_simple_call(gw_zoom_area,      "Zoom Area",       win_menu);
    add_simple_call(gw_zoom_down,      "Zoom Out",        win_menu);
    add_simple_call(gw_zoom_up,        "Zoom In",         win_menu);
    add_simple_call(gw_unzoom,         "Zoom Undo",        win_menu);
    add_separator(win_menu);
    add_simple_call(gw_hyperbol_embed, "Hyperbolic View", win_menu);
  }



  if (menu_mask & M_SETTINGS)
  {
    int opt_menu = add_menu("Settings");

    add_simple_call(gw_window_setup,    "Window Setup",  opt_menu);
    add_simple_call(gw_global_setup,    "Global Options  ",opt_menu);
    add_simple_call(gw_agd_setup,       "AGD Server",    opt_menu);
    add_separator(opt_menu);
    add_simple_call(gw_animation_setup, "Animation",     opt_menu);
    add_simple_call(gw_zooming_setup,   "Zooming",       opt_menu);
    add_separator(opt_menu);
    add_simple_call(gw_postscript_setup,"PostScript",    opt_menu);
    add_separator(opt_menu);
    add_simple_call(gw_svg_setup,       "SVG Output",    opt_menu);
    add_separator(opt_menu);
    add_simple_call(node_setup,       "Node Defaults", opt_menu);
    add_simple_call(edge_setup,       "Edge Defaults", opt_menu);
   }


  if (menu_mask & M_HELP)
  {
  //int help_menu = add_menu("Help");
    int help_menu = add_menu(" ? ",0,0,"help");

    add_member_call(&GraphWin::help_about, "About", help_menu);

    //add_member_call(&GraphWin::help_news,  "News",  help_menu);
    //add_separator(help_menu);

    int but_menu = add_menu("Help    ",help_menu);
    add_member_call(&GraphWin::help_mouse_left,  "Button 1", but_menu);
    add_member_call(&GraphWin::help_mouse_middle,"Button 2", but_menu);
    add_member_call(&GraphWin::help_mouse_right, "Button 3", but_menu);

    if (help_list.length() > 0)
    { add_separator(help_menu);
      string s;
      forall(s,help_list)
         add_simple_call(gw_help_handler,s,help_menu);
    }
   }


  zoom_in_pr = win_p->create_pixrect(zoom_in20);
  add_simple_call(gw_zoom_up,"zoom in",-1,zoom_in_pr);

  zoom_fit_pr = win_p->create_pixrect(zoom_best_fit20);
  add_simple_call(gw_zoom_graph,"best fit",-1,zoom_fit_pr);

  zoom_out_pr = win_p->create_pixrect(zoom_out20);
  add_simple_call(gw_zoom_down,"zoom out",-5,zoom_out_pr);


  if (menu_mask & M_UNDO)
  { 
    //undo_button = add_simple_call(undo_handler,"<<");
    undo_pr = win_p->create_pixrect(undo20);
    undo_button = add_simple_call(undo_handler,"undo",-1,undo_pr);
    menu_p->set_button_help_str(undo_button,"undo");
    menu_p->set_button_repeat(undo_button,150);
    menu_p->disable_button(undo_button);

    //redo_button = add_simple_call(redo_handler,">>");
    redo_pr = win_p->create_pixrect(redo20);
    redo_button = add_simple_call(redo_handler,"redo",-3,redo_pr);
    menu_p->set_button_help_str(redo_button,"redo");
    menu_p->set_button_repeat(redo_button,250);
    menu_p->disable_button(redo_button);
   }
 

  if (menu_mask & M_DONE)
  { done_button=add_simple_call(done_handler,"done");
    //menu_p->set_button_help_str(done_button,"done");
    menu_p->hspace(1);
   }

  menu_p->make_menu_bar(2);

/*
  if ((menu_mask & M_DEFAULT) && (win_p == menu_p || win_p->is_open()))
     menu_p->make_menu_bar(2);
  else
     menu_p->buttons_per_line(3);
*/

}

bool GraphWin::unsaved_changes() 
{ if (gr_p->empty()) return false;
  if (gw_undo) return !(gw_undo->empty() && gw_undo->redo_empty());
  panel_item it = menu_p->get_button_item(undo_button);
  return menu_p->is_enabled(it); 
}


LEDA_END_NAMESPACE
