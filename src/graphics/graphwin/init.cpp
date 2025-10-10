/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  init.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>
#include <LEDA/system/assert.h>
#include "graphwin_undo.h"
#include "local.h"

#include <stdlib.h>

/*
#include <LEDA/graph/graph_alg.h>
*/

LEDA_BEGIN_NAMESPACE

extern __exportF bool PLANAR(graph&, bool embed=false);



static void gw_win_close_handler(window* win)
{ GraphWin* gw = win->get_graphwin();
  exit_handler(*gw);
}



void GraphWin::draw_shapes()
{ line_style save_ls = win_p->get_line_style();
  int        save_lw = win_p->get_line_width();
  const list<shape>& L = get_shapes();
  win_p->set_text_font();
  shape x;
  forall(x,L)
  { win_p->set_line_style(x.lstyle);
    win_p->set_line_width(x.lwidth);
    if (x.poly.empty())
    { // circle
      win_p->draw_filled_circle(x.circ,x.fcol);
      win_p->draw_circle(x.circ,x.bcol);
      win_p->draw_ctext(x.circ.center(),x.label,x.tcol);
      continue;
     }

    win_p->draw_filled_polygon(x.poly,x.fcol);
    win_p->draw_polygon(x.poly,x.bcol);
    rectangle bb = BoundingBox(x.poly);
    win_p->set_clip_rectangle(bb.xmin(),bb.ymin(),bb.xmax(),bb.ymax());
    win_p->draw_text(bb.xmin()+win_p->pix_to_real(2),bb.ymax(),x.label,x.tcol);
    win_p->reset_clipping();
   }
  win_p->set_line_style(save_ls);
  win_p->set_line_width(save_lw);
}

void gw_bg_redraw_func(window* win, double x0, double y0, double x1, double y1)
{ GraphWin* gw = win->get_graphwin();
  if (gw->bg_redraw_obj)
  { gw->bg_redraw_obj->set_params(win,NULL,0,x0,y0,x1,y1);
	gw->bg_redraw_obj->operator()();
  }
  else if (gw->bg_redraw_func) gw->bg_redraw_func(win,x0,y0,x1,y1);

  if (gw->bg_node_redraw_obj) 
  { node v;
    forall(v,gw->bg_redraw_node_list)
    { point pos = gw->get_position(v);
      double rx = gw->get_radius1(v);
      double ry = gw->get_radius2(v);
      gw->bg_node_redraw_obj->set_params(win,NULL,0,pos.xcoord(),pos.ycoord(),rx,ry);
      gw->bg_node_redraw_obj->operator()();
    }
  }
  else if (gw->bg_node_redraw_func) 
  { node v;
    forall(v,gw->bg_redraw_node_list)
    { point pos = gw->get_position(v);
      double rx = gw->get_radius1(v);
      double ry = gw->get_radius2(v);
      gw->bg_node_redraw_func(win,pos.xcoord(),pos.ycoord(),rx,ry);
    }
  }

  gw->draw_shapes();
}



/*
void gw_LEDA_EXCEPTION(int i, const char* s)
{ 
  panel P("GraphWin Error Handler");

  if (i == 0)
    { P.text_item("\\bf Warning");
      P.text_item(string("%-64s",s));
      P.text_item("");
      P.button("continue");
      P.open();
     }
  else
    { P.text_item("\\bf Error");
      P.text_item(string("%-64s",s));
      P.text_item("");
      P.button("exit",0);
      P.button("core",1);
      if (P.open() == 0)
         quit_action(0);
      else 
         abort();
     }
 }
*/


//----------------------------------------------------------------------------


void GraphWin::init(graph* G, window* W) 
{
  gr_p  = G;
  win_p = W;
  win_p->grawin_ptr = this;

  if (W->display_fd() == -1) return;

  if (create_flag & CR_WIN) 
     menu_p = win_p;
  else
   { string win_label("%s %.1f",DefWindowLabel,version());
     if (W->is_open())
        menu_p = new window(W->width(),30,win_label);
     else
        menu_p = new panel(DefWindowLabel);
     menu_p->grawin_ptr = this;
    }

  init_once();

  if (gr_p->is_undirected()) e_model.dir = undirected_edge;

  init_graph();

  win_width = W->width();

  W->set_fixed_font();

  status_win_height = (int)(1.4*W->text_height("H"));
}


void GraphWin::init(graph* G, int win_w, int win_h, string win_label) 
{ init(G,new window(win_w,win_h,win_label)); }



//----------------------------------------------------------------------------

void GraphWin::init_once() {

  sub_menu_counter = 0;

  coord_to_string_handler=NULL;

  undo_graph_handler = NULL;

  pre_new_node_handler=NULL;
  pre_new_edge_handler=NULL;
  pre_move_node_handler=NULL;
  pre_del_node_handler=NULL;
  pre_del_edge_handler=NULL;
  pre_split_edge_handler=NULL;
  pre_move_component_handler=NULL;
  pre_init_graph_handler=NULL;
  pre_start_move_node_handler=NULL;
  pre_end_move_node_handler=NULL;


  post_new_node_handler=NULL;
  post_new_edge_handler=NULL;
  post_move_node_handler=NULL;
  post_del_node_handler=NULL;
  post_del_edge_handler=NULL;
  post_split_edge_handler=NULL;
  post_move_component_handler=NULL;
  post_init_graph_handler=NULL;
  post_start_move_node_handler=NULL;
  post_end_move_node_handler=NULL;

  num_sliders = 0;

  for(int i=0; i<MAX_SLIDERS; i++) 
  { edge_slider_handler[i] = NULL;
    start_edge_slider_handler[i] = NULL;
    end_edge_slider_handler[i] = NULL;
    slider_color[i] = ivory;
   }

//  n_matrix=NULL;

  win_init_done=false;

  done_button = -1;
  exit_button = -1;
  undo_button = -1;
  redo_button = -1;

  init_static();

  init_default();

  found_node = nil;
  found_frame = nil;
  found_edge = nil;
  found_slider = -1;

  current_node = nil;
  current_edge = nil;

  start_node = nil;

  save_n_info = nil;
  save_e_info = nil;

  move_nodes_dest = nil;
  move_nodes_trans = nil;

  node_default_motion = 0;

  node_setup_active = false;
  edge_setup_active = false;

  hide_labels = false;

  bg_redraw_obj = 0;
  bg_redraw_func = 0;
  bg_node_redraw_obj = 0;
  bg_node_redraw_func = 0;

  labelBoxEnabled = false;

  open_frameless = false;
  frame_actions_enabled = true;

  gw_undo = nil;
}

//----------------------------------------------------------------------------

int GraphWin::window_width()
{ if ( win_p && win_p->is_open() ) return win_p->width();
  int w = DefWinSizeX;
  return w;

}

int GraphWin::window_height()
{ if ( win_p && win_p->is_open() ) return win_p->height();
  int h = DefWinSizeY;
  return h;
}

void GraphWin::init_default() {

  n_model.init=false;
  e_model.init=false;

  n_model.pos=point(0,0);
  n_model.clr=DefNodeColor;

  n_model.shape=DefNodeShape;

  n_model.border_clr=DefNodeBorderColor;

  int w_width;
  int w_height;

  if (win_p->is_closed()) {
    w_width = window_width() - 8;
    w_height = window_height() - 20;
    x_min = DefXMin;
    x_max = DefXMax;
    y_min = DefYMin;
    y_max = w_height*(x_max-x_min)/w_width+y_min;
  }
  else {
    x_min = win_p->xmin();
    x_max = win_p->xmax();
    y_min = win_p->ymin();
    y_max = win_p->ymax();
    w_width = win_p->width();
  }

  zoom_x0 = x_min;
  zoom_y0 = y_min;
  zoom_x1 = x_max;
  zoom_y1 = y_max;

  n_model.r1 = float(DefNodeRadius1);
  n_model.r2 = float(DefNodeRadius2);


  n_model.border_w = float(DefNodeBorderWidth);

  n_model.label=DefNodeLabel;
  n_model.label_t=DefNodeLabelType;
  n_model.label_clr=DefNodeLabelClr;
  n_model.label_pos=DefNodeLabelPos;
  n_model.pixmap=DefNodePixmap;
  n_model.selected=false;
  n_model.label_curs = -1;

  e_model.clr=DefEdgeColor;
  e_model.shape=DefEdgeShape;
  e_model.style=DefEdgeStyle;
  e_model.dir=DefEdgeDir;
  e_model.width=float(DefEdgeWidth);
  e_model.label=DefEdgeLabel;
  e_model.label_t=DefEdgeLabelType;
  e_model.label_clr=DefEdgeLabelClr;
  e_model.label_pos=DefEdgeLabelPos;
  e_model.s_anchor=DefEdgeAnchor;
  e_model.t_anchor=DefEdgeAnchor;
  e_model.selected=false;
  e_model.label_curs = -1;
  e_model.slider_f=0;

  xmin_def = DefXMin;
  xmax_def = DefXMax;
  ymin_def = DefYMin;

  edge_distance=DefMultiEdgeDist;

  //node_index_style=DefNodeIndexStyle;
  //edge_index_style=DefEdgeIndexStyle;

  node_index_format=DefNodeIndexFormat;
  edge_index_format=DefEdgeIndexFormat;

  edge_border=DefEdgeBorder;

  node_move=DefAnimationMode;
  zoom_factor=DefZoomFactor;
  animation_steps=DefAnimationSteps;
  grid_dist=DefGridDist;
  g_style=DefGridStyle;

  zoom_objects=DefZoomObjects;
  zoom_labels=DefZoomLabels;
  zoom_hide_labels=DefZoomHideLabels;
  zoom_show_grid=false;

  bg_color=DefBgColor;
  bg_pixmap=DefBgPixrect;
  bg_pmname="none";

  show_status=DefShowStatus;

  ortho_mode = DefOrthoMode;
  ortho_first_dir = 0;

  adjust_to_label = DefNodeAdjLabel;


  node_label_font_size = DefNodeLabelFontSize;
  node_label_font_type = DefNodeLabelFontType;
  node_label_user_font = "";

  edge_label_font_size = DefEdgeLabelFontSize;
  edge_label_font_type = DefEdgeLabelFontType;
  edge_label_user_font = "";


  //gw_dirname=".";
  gw_dirname= get_directory();
  graph_name=DefGraphName;

  ps_fontname=DefPSFontName;

  ps_fontlist.clear();
  ps_fontlist.append("Times");
  ps_fontlist.append("Helvetica");
  ps_fontlist.append("Courier");

  ps_factor = 0; // auto

  svg_edge_transparency = 0;
  svg_whole_graph = true;

  print_cmd=DefPrintCmd;

  flush  = false;
  flush0 = DefFlush;

  max_move_items=DefMaxMoveItems;

  buttons_per_line = DefButtonsPerLine;
  if (win_p != menu_p) buttons_per_line =  DefExtraButtonsPerLine;
 
  gen_nodes=DefGenNodes;
  gen_edges=DefGenEdges;

  edit_result=-1;

  auto_create_target = DefAutoCreateTarget;

  menu_mask = M_COMPLETE | M_DEFAULT;

  //menu_mask &= ~M_HELP;

  status_show_mask = DefShowMask;

  d3_look = DefD3Look;

  agd_host = DefAgdHost;
  agd_port = DefAgdPort;

  agd_input_format = DefAgdInputFormat;
  agd_output_format = DefAgdOutputFormat;

  agd_option_diag = DefAgdOptionDiag;

  string str;
  if (get_environment("AGDHOST",str)) agd_host = str;
  if (get_environment("AGDPORT",str)) agd_port = atoi(str);

  reset_actions();
}

//----------------------------------------------------------------------------

void GraphWin::init_matrix() {

  adj_matrix_first.init(*gr_p,nil);
  adj_matrix_succ.init(*gr_p,nil);

  edge e;    
  forall_edges(e,*gr_p) 
  { 
    if (!e_info[e].init) continue;

    node v = source(e);
    node w = target(e);
    adj_matrix_succ[e] = adj_matrix_first(v,w);
    adj_matrix_first(v,w) = e;
   }
}


//----------------------------------------------------------------------------

static random_source ran;

void GraphWin::init_node(node v) {
  node_info &v_inf=n_info[v]; 

  bool old_flush=set_flush(false);

  // clear entries in adj_matrix
  node w;
  forall_nodes(w,*gr_p) { 
    if (adj_matrix_first.defined(v,w)) adj_matrix_first(v,w) = nil;
    if (adj_matrix_first.defined(w,v)) adj_matrix_first(w,v) = nil;
  }

  set_info(v,n_model,N_COMPLETE);
  v_inf.label_curs = -1;

  set_flush(old_flush);

/*
  double x,y;
  ran >> x >> y;
  double xd=x_max-x_min;
  double yd=y_max-y_min;
  v_inf.pos=point(x_min+(0.05+0.9*x)*xd,y_min+(0.05+0.9*y)*yd);
*/

  v_inf.init=true;
}

//----------------------------------------------------------------------------

void GraphWin::init_edge(edge e) {


  edge_info &e_inf=e_info[e];

  bool old_flush=set_flush(false);

  adj_matrix_succ[e] = adj_matrix_first(source(e),target(e));
  adj_matrix_first(source(e),target(e)) = e;

  set_info(e,e_model,E_COMPLETE & ~E_POLYGON);
  e_inf.label_curs = -1;
  e_inf.slider_f = 0;

  set_flush(old_flush);

  list<point>& P=e_inf.p;
  P.clear();
  P.push(get_position(source(e)));
  P.append(get_position(target(e)));

  e_inf.init=true;

}

//----------------------------------------------------------------------------

void GraphWin::init_graph() {
  n_info.init(*gr_p,n_model);
  e_info.init(*gr_p,e_model);

  // id_queue.clear();
  // max_node_id=0;

  selected_nodes.clear();
  selected_edges.clear();  

  init_matrix();

  node v;
  forall_nodes(v,*gr_p) init_node(v);

  edge e;
  forall_edges(e,*gr_p) init_edge(e);
  edges_embedded = (gr_p->number_of_edges()?false:true);

  if (gw_undo) delete gw_undo;
  gw_undo = new graphwin_undo(*this);
}

//----------------------------------------------------------------------------

void GraphWin::update_nodes() {
  int count = 0;
  node v;
  forall_nodes(v,*gr_p) 
  { if (n_info[v].init) continue;
    init_node(v);
    count++;
  }

  //cout << "update_nodes: count = " << count << endl;
}


void GraphWin::update_edges() {

/*
  // sn (2023/03/01)
  // this resets edge infos in gml reader
*/

  e_info.init(*gr_p,e_model);

  edge e;
  forall_edges(e,*gr_p) {
    if (e_info[e].init) continue;
    init_edge(e);
  }

  init_matrix();
  embed_edges();

  if (get_flush()) redraw();
}


void GraphWin::update_graph() 
{ update_nodes();
  update_edges();
 }

//----------------------------------------------------------------------------

int GraphWin::test_update_edges() {
  edge e;
  node v,w;

  graph& G=get_graph();

  forall_nodes(v,G) {

    node_array<unsigned> out_edges(G,0);

    forall_adj_edges(e,v) {

      w=gr_p->target(e);

      // Test, ob e uninitialisiert
      if (!e_info[e].init) return 1;

      // Test, ob fuer e eine Liste in dyn. node_matrix existiert
      if (!adj_matrix_first.defined(v,w)) return 2; 

      // Test, ob e in dieser List ist      
      edge x = adj_matrix_first(v,w);
      while(x && x != e) x = adj_matrix_succ[x];
      if (x == nil) return 3;

      out_edges[w]++;
    }

    // vergleiche eben angelegte Listen mit vorhandenen incidence-listen

    forall_nodes(w,G)
    { int m = out_edges[w];
      if (adj_matrix_first.defined(v,w))
      { edge x = adj_matrix_first(v,w);
        while (x) 
        { m--;
          x = adj_matrix_succ[x];
         }
       }
      if (m > 0) return 4;
     }
    }

  return 0;
}

int GraphWin::test_update_nodes() {
  node v;
  forall_nodes(v,*gr_p) if (!n_info[v].init) return 1;
  return 0;
}

bool GraphWin::test_update_graph() {
  if (test_update_nodes()) return true;
  if (test_update_edges()) return true;
  return false;
}

//----------------------------------------------------------------------------

void GraphWin::window_init(double xmin, double xmax, double ymin, double f)
{

  double scale = f;

  if (f == 0)
  { double xmax0,xmin0;
    if (win_p->is_open())
     { xmax0 = win_p->xmax();
       xmin0 = win_p->xmin();
      }
    else
     { xmax0 = xmax_def;
       xmin0 = xmin_def;
      }
    scale = (xmax - xmin)/(xmax0 - xmin0);
   }

  bool zoom_obj = get_zoom_objects();

  if (!zoom_obj && scale != 1)
  { 
    double r1 = get_node_radius1();
    double r2 = get_node_radius2();
    double bt = get_node_border_thickness();
    double et = get_edge_thickness();

    bool old_flush=set_flush(false);

    node_label_font_size  *= scale;
    edge_label_font_size  *= scale;

    edge_distance *= scale;

    set_node_radius1(r1*scale,false);
    set_node_radius2(r2*scale,false);
    set_node_border_thickness(bt*scale,false);
    set_edge_thickness(et*scale,false);

    node v;
    forall_nodes(v,*gr_p) 
    { n_info[v].r1 *= (float)scale;  
      n_info[v].r2 *= (float)scale;  
      n_info[v].border_w *= (float)scale;  
     }

    edge e;
    forall_edges(e,*gr_p) 
    { e_info[e].width *= (float)scale;  
      double x,y;
      x = e_info[e].s_anchor.xcoord()/(float)scale;
      y = e_info[e].s_anchor.ycoord()/(float)scale;
      e_info[e].s_anchor = point(x,y);
      x = e_info[e].t_anchor.xcoord()/(float)scale;
      y = e_info[e].t_anchor.ycoord()/(float)scale;
      e_info[e].t_anchor = point(x,y);
    }

    edges_embedded = false;

    set_flush(old_flush);
  }

  if (f == 0)
  { if (win_p->is_open()) 
      win_p->init0(xmin,xmax,ymin,win_p->get_bg_pixrect() ? 1 : 0);
    else
      win_p->init(xmin,xmax,ymin);
   }

  update_win_bounds();

  if (!win_p->is_open())
     win_p->clear(0,0);
  else
    { win_p->start_buffering();
      win_p->clear();
      draw_message();
      draw_graph();
      win_p->flush_buffer();
      win_p->stop_buffering();
    }
}

//----------------------------------------------------------------------------

void GraphWin::update_status_line()
{ 
  window& W = get_window();
  graph&  g = get_graph();

  int n = g.number_of_nodes();
  int m = g.number_of_edges();
  int l = 0;

  edge e;
  forall_edges(e,g)
      if (source(e) == target(e)) l++;

  const char* flush_str = get_flush() ? "on" : "off";
  int   g_dist = get_grid_dist();


 string s = " ";
 if (graph_name != DefGraphName)    s += string("%s  ",~graph_name);
 if (status_show_mask & STAT_NODES) s += string("nodes: %-3d ",n);
 if (status_show_mask & STAT_EDGES) s += string("edges: %-3d ",m);

 if (status_show_mask & STAT_UNDO)  
 { if (gw_undo)
   { int usz = gw_undo->size();
     int rsz = gw_undo->redo_size();
     s += string("undo: %d/%d  ",usz,rsz);
    }
  }

 if (status_show_mask & STAT_LOOPS) s += string("loops: %d  ",l);

 if (status_show_mask & STAT_PLANAR)
 {   if (PLANAR(g)) 
        s += "planar: yes  ";
     else
        s += "planar: no   ";
  }

 if (status_show_mask & STAT_FLUSH) s += string("flush: %s  ",flush_str);
 if (status_show_mask & STAT_GRID)  s += string("grid: %-2d  ",g_dist);

 BASE_WINDOW* sw = W.get_status_window();
 if (sw) sw->set_fixed_font();
 W.set_status_string(s);
}


/*
static double grid_x;
static double grid_y;
*/

void GraphWin::coord_handler(window* win, double x, double y) 
{
  GraphWin* gw = win->get_graphwin();
  if (!(gw->status_show_mask & STAT_COORD)) return;

  BASE_WINDOW* swin = win->get_status_window();
  if (swin == 0 || !swin->is_open()) return;

//double x0 = swin->xmin();
  double y0 = swin->ymin();
  double x1 = swin->xmax();
  double y1 = swin->ymax();

  double gd = -gw->grid_dist;

  if (gd != 0)
  { point p = gw->grid_point(x,y);
    x = p.xcoord();
    y = p.ycoord();
   }

  string coord_str("%7.2f %7.2f",x,y);

  if (gw->coord_to_string_handler != NULL)
     coord_str = gw->coord_to_string_handler(x,y);

  double pix = swin->pix_to_real(1);

  double cx = x1 - swin->text_width(coord_str) - 8*pix;
  double cy = (y1 + y0 + swin->text_height(coord_str))/2;

  cy -= pix;

  swin->clear(cx,y0+pix,x1,y1-pix);
  swin->draw_text(cx,cy,coord_str);

  // status windows are always buffering 
  swin->flush_buffer(cx,y0+pix,x1,y1-pix);
}
 



  

  
static void drop_handler(void* ptr, const char* event,
                                    const char* filename, int x, int y)
{
  if (string(event) != "drop") return;

  window*  wp = (window*)ptr;
  GraphWin* gwp = wp->get_graphwin();

  switch (gwp->drop_file(filename,wp->xreal(x),wp->yreal(y))) {
   case 1 : gwp->acknowledge("No such file",            filename);
            break;
   case 2 : gwp->acknowledge("Error in GW format",      filename);
            break;
   case 3 : gwp->acknowledge("Invalid node attributes", filename);
            break;
   case 4 : gwp->acknowledge("Invalid edge attributes", filename);
            break;
   case 5 : gwp->acknowledge("No Layout Information",   filename);
            break;
   default: gwp->undo_clear();
            break;
  }
  //gwp->set_filename(filename);
}
  


static void special_event_handler(void* w_ptr, const char* event, 
                                               const char* arg,
                                               int x, int y)
{ 
  window*  wp = (window*)w_ptr;
  GraphWin* gwp = wp->get_graphwin();

  if (string(event) == "drop")
  { drop_handler(w_ptr,event,arg,x,y);
    return;
   }

  if (string(event) == "upload")
  { //cout << "upload_event: " << event << "  " << arg << endl;
    string fname = arg;

    if (!is_file(fname))
    { if (fname == "null")
      { gwp->message("\\bf Upload canceled.");
        leda_wait(1.5);
        gwp->del_messages();
       }
      else
        gwp->message("Cannot open " + fname);
    }
    else
    {
      gwp->message("Reading " + fname);
      leda_wait(1.0);

      gwp->load_graph(fname);

      gwp->del_messages();
   
/*
      if (fname.tail(4) == ".gml")
        { //gname = fname.del(".gml");
          int err = gwp->read_gml(fname);
          if (err)
            gwp->acknowledge(string("Error (%d) Reading GML-File ",err),fname);
         }
      else
        if (fname.tail(7) == ".dimacs")
          { //gname = fname.del(".dimacs");
            int err = gwp->read_dimacs(fname);
            if (err)
              gwp->acknowledge(string("Error (%d) Reading Dimacs File ",err),fname);
           }
        else
          { //gname = fname.del(".gw");
            int err = gwp->read_gw(fname);
            if (err && err < 5)
              gwp->acknowledge(string("Error (%d) Reading GW-File ",err), fname);
            if (err == 5) { // no layout
              gw_spring_embed(*gwp,true);
            }
           }
  
       int i = fname.last_index("/");
       int j = fname.last_index(".");

       string gname = fname.substring(i+1,j);

       gwp->set_graphname(gname);
       //gwp->zoom_graph();
*/

     }
  }

}


void GraphWin::init_window(int x, int y) 
{
  init_menu();

  if (!(menu_mask & M_DEFAULT) && (win_p == menu_p || win_p->is_open()))
       menu_p->make_menu_bar(2);

  //win_p->set_special_event_handler(drop_handler);

  win_p->set_special_event_handler(special_event_handler);

  win_p->set_show_coord_handler(coord_handler);

  win_p->set_bg_color(bg_color);
  win_p->set_bg_pixmap(bg_pixmap);
  win_p->set_show_cursor(false);
  win_p->set_redraw(GraphWin::s_redraw);
  win_p->set_bg_redraw(gw_bg_redraw_func);
  win_p->set_mode(src_mode);


  win_p->set_window_close_handler(gw_win_close_handler);


  win_p->set_clear_on_resize(false);

  win_p->init(x_min,x_max,y_min);
  win_p->set_grid_style(g_style);
  win_p->set_grid_dist(-grid_dist);

  if (win_p != menu_p)
  { show_status = false;
    if (x == MAXINT && y == MAXINT)
    { x = 0; 
      y = 0;
      if (win_p->is_open())
      { int wx = win_p->xpos();
        int wy = win_p->ypos();
        if (wy < 100) wy = 100;
        win_p->resize(wx,wy,win_p->width(),win_p->height());
        x =  wx;
        y = -wy;
       }
     }
    menu_p->display(x,y);
  }
  else
  { if (x == MAXINT && y == MAXINT)
    { x = window::center;
      y = window::center;
     }
    if (open_frameless) 
    { win_p->clear_panel();
      win_p->display(*win_p,x,y);
     }
    else
     win_p->display(x,y);
   }


  update_win_bounds();
  win_init_done=true;

  flush=flush0;

  if (show_status) 
  { show_status = false;
    set_show_status(true);
   }

/*
  bool silent = true;
  if (menu_mask & M_LAYOUT) init_agd_menu(get_menu("Layout"),silent);
*/
}

LEDA_END_NAMESPACE
