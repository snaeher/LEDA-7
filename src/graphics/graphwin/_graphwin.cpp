/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _graphwin.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/string.h>

#include "graphwin_undo.h"
#include "local.h"


//----------------------------------------------------------------------------
// constructors
//----------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE


edge_info::edge_info(const edge_info& inf) : init(inf.init),
                                             clr(inf.clr),
                                             shape(inf.shape),
                                             style(inf.style),
                                             dir(inf.dir),
                                             width(inf.width),
                                             label_t(inf.label_t),
                                             label(inf.label),
                                             label_clr(inf.label_clr),
                                             label_pos(inf.label_pos),
                                             label_curs(inf.label_curs),
                                             p(inf.p),
                                             s_anchor(inf.s_anchor),
                                             t_anchor(inf.t_anchor),
                                             selected(inf.selected), 
                                             slider_f(inf.slider_f)
                                                   
{ //memcpy(this,&inf,sizeof(edge_info));
  //new (&p,0) list<point>(inf.p);
  if (slider_f)
  { slider_f = new double[MAX_SLIDERS];
    for(int i = 0; i< MAX_SLIDERS; i++) slider_f[i] = inf.slider_f[i];
   }
}

const edge_info& edge_info::operator=(const edge_info& inf)
{
  init = inf.init;
  clr = inf.clr;
  shape = inf.shape;
  style = inf.style;
  dir = inf.dir;
  width = inf.width;
  label_t = inf.label_t;
  label = inf.label;
  label_clr = inf.label_clr;
  label_pos = inf.label_pos;
  label_curs = inf.label_curs;
  p = inf.p;
  s_anchor = inf.s_anchor;
  t_anchor = inf.t_anchor;
  selected = inf.selected; 
  if (inf.slider_f)
  { slider_f = new double[MAX_SLIDERS];
    for(int i = 0; i< MAX_SLIDERS; i++) slider_f[i] = inf.slider_f[i];
   }
  return *this;
}

GraphWin::GraphWin(window& W) : node_motion(0)
{ win_p = 0;
  create_flag=CR_GRAPH;
  init(new graph,&W);

}

GraphWin::GraphWin(graph& G, window& W) : node_motion(0)
{ win_p = 0;
  create_flag=CR_NONE;
  init(&G,&W);
}
    

GraphWin::GraphWin(graph& G, const char* label) : node_motion(0)
{ win_p = 0;
  create_flag=CR_WIN;
  string win_label = label;
  if (win_label == "") win_label = string("%s %.1f",DefWindowLabel,version());
  int w = window_width();
  int h = window_height();
  init(&G,w,h,win_label);
}


GraphWin::GraphWin(graph& G, int w, int h, const char* label) : node_motion(0)
{ win_p = 0;
  create_flag=CR_WIN;
  string win_label = label;
  if (win_label == "") win_label = string("%s %.1f",DefWindowLabel,version());
  init(&G,w,h,win_label);  
}


GraphWin::GraphWin(const char* label) : node_motion(0)
{ win_p = 0;
  create_flag=CR_GRAPH | CR_WIN;
  string win_label = label;
  if (win_label == "") win_label = string("%s %.1f",DefWindowLabel,version());
  int w = window_width();
  int h = window_height();
  init(new graph,w,h,win_label);
}


GraphWin::GraphWin(int w, int h, const char* label) : node_motion(0)
{ win_p = 0;
  create_flag=CR_GRAPH | CR_WIN;
  string win_label = label;
  if (win_label == "") win_label = string("%s %.1f",DefWindowLabel,version());
  init(new graph,w,h,win_label);
 }

    
//----------------------------------------------------------------------------

GraphWin::~GraphWin() 
{ 
   if (gw_undo) delete gw_undo;

  if (zoom_in_pr) win_p->del_pixrect(zoom_in_pr);
  if (zoom_fit_pr) win_p->del_pixrect(zoom_fit_pr);
  if (zoom_out_pr) win_p->del_pixrect(zoom_out_pr);
  if (undo_pr) win_p->del_pixrect(undo_pr);
  if (redo_pr) win_p->del_pixrect(redo_pr);

  if (create_flag & CR_GRAPH) delete gr_p;

  if (sub_menu_counter) {
    int i;
    for(i=0;i<call_entry_counter;i++) delete menu_functions[i];
    for(i=1;i<sub_menu_counter;i++) delete sub_menus[i];
  }

  win_p->set_redraw();

  if (menu_p) delete menu_p;

  if (save_n_info) delete save_n_info;
  if (save_e_info) delete save_e_info;
}

//----------------------------------------------------------------------------

void GraphWin::update_win_bounds() {
  x_min=win_p->xmin();
  x_max=win_p->xmax();
  y_min=win_p->ymin();
  y_max=win_p->ymax();

  if (show_status) y_min += win_p->pix_to_real(status_win_height);
}

//----------------------------------------------------------------------------

void GraphWin::display(int x, int y) {

  //win_p->set_cursor(XC_watch);

  init_window(x,y);

  if (!edges_embedded) embed_edges();

  if (gr_p->number_of_nodes() > 0)
  { bool f=set_flush(false);
    call_init_graph_handler();
    set_flush(f);
   }

  if (show_status) y_min -= win_p->pix_to_real(status_win_height);
  // to make up for update_bounds called by window_init (see below)

  window_init(get_xmin(),get_xmax(),get_ymin());

  update_status_line();

  //win_p->set_cursor(-1);
}


void GraphWin::display() { display(MAXINT,MAXINT); }

    
//----------------------------------------------------------------------------

bool GraphWin::open() {
  display(); 
  return edit();
}

bool GraphWin::open(int x, int y) {
  display(x,y); 
  return edit();
}

//----------------------------------------------------------------------------

void GraphWin::close() { 
  win_p->close();
  menu_p->close(); 
}

//----------------------------------------------------------------------------

node GraphWin::new_node(const point& p) {

  node v = gr_p->new_node();

  if (!n_info[v].init) init_node(v);
  n_info[v].pos=p;

  if (get_flush()) 
  { window& W = get_window();
    W.start_buffering();
    draw_node(v);
    flush_node(v);
    W.stop_buffering();
    update_status_line();
   }

  return v;
}

//----------------------------------------------------------------------------

edge GraphWin::new_edge(node v, node w, const list<point>& P) 
{
  bool old_flush=get_flush();

  edge e=gr_p->new_edge(v,w);
  if (e == nil) return nil;

  if (!e_info[e].init) init_edge(e);

// if ((v == w) && (P.size() == 0)) {  ... selfloop ...}

  point v_p=get_position(v);
  point w_p=get_position(w);
  list<point>& L=get_poly(e);
  L=P;
  L.push(v_p);
  L.append(w_p);
  normalize_polygon(L);
  if (L.size() == 1) L.append(w_p);	// (v_p == w_p)

  if (old_flush) {
    embed_edges(v,w);
    redraw();
    update_status_line();
  }
  else edges_embedded=false;

  return e;
}

//----------------------------------------------------------------------------

edge GraphWin::new_edge(node v, node w) { 
  list<point> P;
  return new_edge(v,w,P);
}

//----------------------------------------------------------------------------

void GraphWin::internal_del_edge(edge e) {
  e_info[e].init=false;         // only because for new edges are NO new
                                // infos in map e_info created.

  deselect(e);

  // remove e from adjacency matrix

  edge x = adj_matrix_first(source(e),target(e));

  edge pred = nil;
  while(x && x != e) 
  { pred = x;
    x = adj_matrix_succ[x];
   }

  if (x)
  { if (pred) 
      adj_matrix_succ[pred] = adj_matrix_succ[x];
    else 
      adj_matrix_first(source(e),target(e)) = adj_matrix_succ[x];
   }
}

void GraphWin::del_edge(edge e) {

  bool before = edges_embedded;
  node v = source(e);
  node w = target(e);

  bool old_flush=set_flush(false);

  if (e == found_edge) found_edge = nil;

  if (!gw_undo) internal_del_edge(e);

  gr_p->del_edge(e);

  if (old_flush) 
    { embed_edges(v,w);
      redraw();
      edges_embedded=before;
      update_status_line();
    }
  else
    edges_embedded=false;

  set_flush(old_flush);
}

//----------------------------------------------------------------------------

void GraphWin::internal_del_node(node v) {

  n_info[v].init=false;		// only because for new nodes are NO new  
				// infos in map n_info created.
  if (is_selected(v)) deselect(v);

}

void GraphWin::del_node(node v) {

  bool fl = set_flush(false);

  if (v == found_node) found_node = nil;
  if (v == found_frame) found_frame = nil;

  if (!gw_undo) 
  { edge e;
    forall_adj_edges(e,v) internal_del_edge(e);	
    forall_in_edges(e,v)  internal_del_edge(e);	
    internal_del_node(v); 
   }

  gr_p->del_node(v);

  if (fl) 
  { redraw();
    update_status_line();
   }

  set_flush(fl);
}

//----------------------------------------------------------------------------


void GraphWin::clear_graph() 
{ gr_p->clear();
  //init_graph();
  update_graph();
/*
  double x0 = get_xmin();
  double y0 = get_ymin();
  double x1 = get_xmax();
  win_init(x0,x1,y0);
*/
  if (get_flush()) redraw();
  call_init_graph_handler();
 }

//----------------------------------------------------------------------------

void GraphWin::changing_layout(const list<node>& N, const list<edge>& E) {
  if (gw_undo) gw_undo->on_changing_layout(*this,N,E);
  changing_layout_event(*this,N,E);
}

void GraphWin::changing_layout(const list<node>& N) {
  list<edge> E;
  if (gw_undo) gw_undo->on_changing_layout(*this,N,E);
  changing_layout_event(*this,N,E);
}

void GraphWin::changing_layout(const list<edge>& E) {
  list<node> N;
  if (gw_undo) gw_undo->on_changing_layout(*this,N,E);
  changing_layout_event(*this,N,E);
}


void GraphWin::undo() {
  if (gw_undo && !gw_undo->empty()) {
    bool b = set_flush(false);
    gw_undo->undo();
    redraw();
    if (b) redraw();
    set_flush(b);
//gw_undo->start_complex_command();
//gw_undo->finish_complex_command();

/*
graphwin_undo* gw_undo_save = gw_undo;
gw_undo = 0;
call_undo_graph_handler();
gw_undo = gw_undo_save;
get_graph().enable_events();
*/

call_undo_graph_handler();

  }
}

void GraphWin::redo() {
  if (gw_undo && !gw_undo->redo_empty()) {
    bool b = set_flush(false);
    gw_undo->redo();
    if (b) redraw();
    set_flush(b);
//gw_undo->start_complex_command();
//gw_undo->finish_complex_command();

/*
get_graph().disable_events();
graphwin_undo* gw_undo_save = gw_undo;
gw_undo = 0;
call_undo_graph_handler();
gw_undo = gw_undo_save;
get_graph().enable_events();
*/

call_undo_graph_handler();

  }
}

LEDA_END_NAMESPACE
