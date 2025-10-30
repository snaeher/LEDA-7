/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  get_set.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/stream.h>
#include <LEDA/graphics/graphwin.h>
#include "local.h"

#include <stdlib.h>

#define GW GraphWin

LEDA_BEGIN_NAMESPACE

color gw_auto_color = invisible;


static string make_index_label(int i, string format)
{ string s = format.replace_all("%d",string("%d",i));
  s = s.replace_all("%D",string("%d",i+1));
  s = s.replace_all("%c",string("%c",i+'a'));
  s = s.replace_all("%C",string("%c",i+'A'));
  return s;
}


const node_info& GW::get_info(node v) const { return n_info[v]; }
const edge_info& GW::get_info(edge e) const { return e_info[e]; }

point         GW::get_position(node v)    const { return n_info[v].pos;       }
color         GW::get_color(node v)       const { return n_info[v].clr;       }
char*         GW::get_pixmap(node v)      const { return n_info[v].pixmap;    }
gw_node_shape GW::get_shape(node v)       const { return n_info[v].shape;     }
color         GW::get_border_color(node v)const { return n_info[v].border_clr;}
double        GW::get_border_thickness(node v)const {return n_info[v].border_w;}
double        GW::get_radius1(node v)     const { return n_info[v].r1;        }
double        GW::get_radius2(node v)     const { return n_info[v].r2;        }
gw_label_type GW::get_label_type(node v)  const { return n_info[v].label_t;   }
gw_position   GW::get_label_pos(node v)   const { return n_info[v].label_pos; }
color         GW::get_label_color(node v) const { return n_info[v].label_clr; }
bool          GW::get_select(node v)      const { return n_info[v].selected;  }
string        GW::get_user_label(node v)  const { return n_info[v].label;     }

int GW::get_border_width(node v) const 
{ double th = get_border_thickness(v);
  int w = win_p->real_to_pix(th); 
  if (w == 0 && th > 0) w = 1;
  return w;
}

int GW::get_width(node v) const { return win_p->real_to_pix(2*get_radius1(v)); }
int GW::get_height(node v)const { return win_p->real_to_pix(2*get_radius2(v)); }

string GW::get_index_label(node v) const 
{ return make_index_label(gr_p->index(v),node_index_format); }

string GW::get_data(node v) const { return gr_p->get_node_entry_string(v); }

string GW::get_label(node v) const 
{ int lt = get_label_type(v);
  if (lt & user_label)  return get_user_label(v);
  if (lt & data_label)  return get_data(v);
  if (lt & index_label) return get_index_label(v);
  return "";
}



list<point>&  GW::get_poly(edge e)  { return e_info[e].p;  }

const 
list<point>&  GW::get_edge_points(edge e)  const { return e_info[e].p;        }
color         GW::get_color(edge e)        const { return e_info[e].clr;      }
gw_edge_shape GW::get_shape(edge e)        const { return e_info[e].shape;    }
gw_edge_style GW::get_style(edge e)        const { return e_info[e].style;    }
gw_edge_dir   GW::get_direction(edge e)    const { return e_info[e].dir;      }
double        GW::get_thickness(edge e)    const { return e_info[e].width;}
gw_label_type GW::get_label_type(edge e)   const { return e_info[e].label_t;  }
gw_position   GW::get_label_pos(edge e)    const { return e_info[e].label_pos;}
color         GW::get_label_color(edge e)  const { return e_info[e].label_clr;}
bool          GW::get_select(edge e)       const { return e_info[e].selected; }
point         GW::get_source_anchor(edge e)const { return e_info[e].s_anchor; }
point         GW::get_target_anchor(edge e)const { return e_info[e].t_anchor; }
string        GW::get_user_label(edge e)   const { return e_info[e].label;    }


int GW::get_width(edge e) const 
{ double th = get_thickness(e);
  int w = win_p->real_to_pix(th); 
  if (w == 0 && th > 0) w = 1;
  return w;
}

list<point> GW::get_bends(edge e) const 
{ list<point> L = e_info[e].p;
  L.pop();
  L.Pop();
  return L;
}


string GW::get_index_label(edge e) const 
{ return make_index_label(gr_p->index(e),edge_index_format); }

string GW::get_data(edge e) const { return gr_p->get_edge_entry_string(e); }

string GW::get_label(edge e) const { 
  string s;
  int lt = get_label_type(e);
  if (lt & data_label) s = get_data(e);
  if (lt & user_label) s = get_user_label(e);
  if (lt & index_label) 
  { if (s == "") 
       s = get_index_label(e);
    else
       s += string(" [%d]",gr_p->index(e));
   }
  return s;
}


double GW::get_slider_value(edge e, int i) const
{ return e_info[e].slider_f[i]; }


//----------------------------------------------------------------------------

const node_info& GW::get_node_info() const { return n_model; }
const edge_info& GW::get_edge_info() const { return e_model; }


color         GW::get_node_color()       const { return n_model.clr;       }
char*         GW::get_node_pixmap()      const { return n_model.pixmap;    }
gw_node_shape GW::get_node_shape()       const { return n_model.shape; }
color         GW::get_node_border_color()const { return n_model.border_clr;}
double        GW::get_node_border_thickness() const { return n_model.border_w; }
double        GW::get_node_radius1()     const { return n_model.r1;        }
double        GW::get_node_radius2()     const { return n_model.r2;        }
gw_label_type GW::get_node_label_type()  const { return n_model.label_t;   }
color         GW::get_node_label_color() const { return n_model.label_clr; }
gw_position   GW::get_node_label_pos()   const { return n_model.label_pos; }

int GW::get_node_width() const 
{ return win_p->real_to_pix(2*get_node_radius1()); } 

int GW::get_node_height() const 
{ return win_p->real_to_pix(2*get_node_radius2()); } 


int GW::get_node_border_width() const 
{ return win_p->real_to_pix(get_node_border_thickness()); }


color         GW::get_edge_color()      const { return e_model.clr;       }
gw_edge_shape GW::get_edge_shape()      const { return e_model.shape;     }
gw_edge_style GW::get_edge_style()      const { return e_model.style;     }
gw_edge_dir   GW::get_edge_direction()  const { return e_model.dir;     }
double        GW::get_edge_thickness()  const { return e_model.width;     }
gw_label_type GW::get_edge_label_type() const { return e_model.label_t;   }
gw_position   GW::get_edge_label_pos()  const { return e_model.label_pos; }
color         GW::get_edge_label_color()const { return e_model.label_clr; }


int GW::get_edge_width() const 
{ return win_p->real_to_pix(get_edge_thickness()); } 

int GW::get_edge_distance() const    
{ return win_p->real_to_pix(edge_distance); }


//----------------------------------------------------------------------------

int GW::get_gen_nodes() const { return gen_nodes; }
int GW::get_gen_edges() const { return gen_edges; }


grid_style    GW::get_grid_style() const       { return g_style; }
int           GW::get_grid_dist() const        { return grid_dist; }
bool          GW::get_show_status() const      { return show_status; }
int           GW::get_animation_steps() const  { return animation_steps; }
node_move_t   GW::get_animation_mode() const   { return node_move; }
double        GW::get_zoom_factor() const      { return zoom_factor; }
bool          GW::get_zoom_objects() const     { return zoom_objects; }
bool          GW::get_zoom_labels() const      { return zoom_labels; }
bool          GW::get_zoom_hide_labels() const { return zoom_hide_labels; }
int           GW::get_max_move_items() const   { return max_move_items; }
bool          GW::get_ortho_mode() const       { return ortho_mode; }
bool          GW::get_auto_create_target() const { return auto_create_target; }
bool          GW::get_d3_look() const          { return d3_look; }

double      GW::get_xmin() const             { return x_min; }
double      GW::get_ymin() const             { return y_min; }
double      GW::get_xmax() const             { return x_max; }
double      GW::get_ymax() const             { return y_max; }

int GW::set_max_move_items(int i) {
  int old_max_move_items = max_move_items;
  max_move_items=i;  
  return old_max_move_items; 
}

//----------------------------------------------------------------------------

#define GW_HANDLER(h,pre,post)                                               \
  void GW::set_##h##_handler(bool (*f) pre  ) {                        \
    pre_##h##_handler=f; }                                                   \
  void GW::set_##h##_handler(void (*f) post ) {                        \
    post_##h##_handler=f;                                                    \
    if (!f) pre_##h##_handler=NULL;                                          }


GW_HANDLER(new_node,(GW& gw, const point& p),(GW& gw, node v))
GW_HANDLER(new_edge,(GW& gw, node s, node t),(GW& gw, edge e))
GW_HANDLER(move_node,(GW& gw, node v, const point& p),(GW& gw, node v))
GW_HANDLER(del_node,(GW& gw, node v),(GW& gw))
GW_HANDLER(del_edge,(GW& gw, edge e),(GW& gw))
GW_HANDLER(split_edge,(GW& gw, edge e),(GW& gw, node v))
GW_HANDLER(move_component,(GW& gw, node v),(GW& gw, node v))
GW_HANDLER(init_graph,(GW& gw),(GW& gw))
GW_HANDLER(start_move_node,(GW& gw, node v),(GW& gw, node v))
GW_HANDLER(end_move_node,(GW& gw, node v, const point& p),(GW& gw, node v))

void GW::set_undo_graph_handler(void (*f)(GW& gw)) { undo_graph_handler = f; }

bool GW::check_start_move_node_handler(node v) {
  return (pre_start_move_node_handler ? pre_start_move_node_handler(*this,v) : true);
}
void GW::call_start_move_node_handler(node v) {
  if (post_start_move_node_handler) post_start_move_node_handler(*this,v);
}
bool GW::check_end_move_node_handler(node v, const point& p) {
  return (pre_end_move_node_handler ? pre_end_move_node_handler(*this,v,p) : true);
}
void GW::call_end_move_node_handler(node v) {
  if (post_end_move_node_handler) post_end_move_node_handler(*this,v);
}


bool GW::check_new_node_handler(const point& p) {
  return (pre_new_node_handler ? pre_new_node_handler(*this,p) : true);
}
void GW::call_new_node_handler(node v) {
  if (post_new_node_handler) post_new_node_handler(*this,v);
}
bool GW::check_new_edge_handler(node s, node t) {
  return (pre_new_edge_handler ? pre_new_edge_handler(*this,s,t) : true);
}
void GW::call_new_edge_handler(edge e) {
  if (post_new_edge_handler) post_new_edge_handler(*this,e);
}
bool GW::check_move_node_handler(node v, const point& p) {
  return (pre_move_node_handler ? pre_move_node_handler(*this,v,p) : true);
}

void GW::call_move_node_handler(node v) {
  if (post_move_node_handler) post_move_node_handler(*this,v);
}
bool GW::check_del_node_handler(node v) {
  return (pre_del_node_handler ? pre_del_node_handler(*this,v) : true);
}
void GW::call_del_node_handler() {
  if (post_del_node_handler) post_del_node_handler(*this);
}
bool GW::check_del_edge_handler(edge e) {
  return (pre_del_edge_handler ? pre_del_edge_handler(*this,e) : true);
}
void GW::call_del_edge_handler() {
  if (post_del_edge_handler) post_del_edge_handler(*this);
}
bool GW::check_split_edge_handler(edge e) {
  return (pre_split_edge_handler ? pre_split_edge_handler(*this,e) : true);
}
void GW::call_split_edge_handler(node u) {
  if (post_split_edge_handler) post_split_edge_handler(*this,u);
}
bool GW::check_move_component_handler(node v) {
  return (pre_move_component_handler ? pre_move_component_handler(*this,v) : true);
}
void GW::call_move_component_handler(node v) { 
  if (post_move_component_handler) post_move_component_handler(*this,v);
}

bool GW::check_init_graph_handler() {
  if (!gw_undo)
  { menu_p->disable_button(undo_button);
    menu_p->enable_button(redo_button);
   }
  return (pre_init_graph_handler ? pre_init_graph_handler(*this) : true);
}

void GW::call_init_graph_handler() {
  if (post_init_graph_handler) post_init_graph_handler(*this);
}


void GW::call_undo_graph_handler() {
  if (undo_graph_handler) undo_graph_handler(*this);
}



/****************************************************************************/
/*                     Settings for existing nodes                          */
/****************************************************************************/


void GW::set_info(node v, const node_info& n, long mask) {

  node_info& v_inf=n_info[v];
  bool ee=true;


  bool old_flush=set_flush(false);

  if (mask & N_COLOR)  v_inf.clr=n.clr;
  if (mask & N_PIXMAP) v_inf.pixmap=n.pixmap;
  if (mask & N_SHAPE)
    if  (v_inf.shape != n.shape) { ee=false; v_inf.shape=n.shape; }
  if (mask & N_BCOLOR) v_inf.border_clr=n.border_clr; 
  if (mask & N_BWIDTH) v_inf.border_w=n.border_w; 
  if (mask & N_WIDTH)  if (v_inf.r1 != n.r1) { ee=false; v_inf.r1=n.r1; }
  if (mask & N_HEIGHT) if (v_inf.r2 != n.r2) { ee=false; v_inf.r2=n.r2; }

  if (mask & N_LCOLOR) v_inf.label_clr=n.label_clr;
  if (mask & N_LPOS)   v_inf.label_pos=n.label_pos;

  if (mask & N_LTYPE) v_inf.label_t=n.label_t;
  if (mask & N_LABEL) v_inf.label = n.label;

  if (mask & N_POS) {
    double x = v_inf.pos.xcoord();
    double y = v_inf.pos.ycoord();
    double x1,y1;
    if (mask & N_XPOS) { x1=n.pos.xcoord(); if (x != x1) { ee=false; x=x1; } }
    if (mask & N_YPOS) { y1=n.pos.ycoord(); if (y != y1) { ee=false; y=y1; } }
    v_inf.pos=point(x,y);
  }

  if (mask & N_SELECT) set_select(v,n.selected);

  set_flush(old_flush);

  if (old_flush) {
    if (!ee) embed_node_with_edges(v);
    redraw();
  }
  else {
   if ((!ee) && (gr_p->degree(v))) edges_embedded=false;
  }

}

//----------------------------------------------------------------------------

bool GW::set_select(node v, bool b, bool rd) 
{ bool sel = n_info[v].selected;
  if (sel != b)
  { n_info[v].selected = b;
    if (b) 
       selected_nodes.push(v);
    else 
       { list_item it;
         forall_items(it,selected_nodes)
           if (selected_nodes[it] == v) selected_nodes.del_item(it);
        }
    if (rd) 
    { win_p->start_buffering();
      if (b == false)
       { clear_node_box(v);
         draw_node_with_edges(v);
        }
      else
         draw_node(v);
      flush_node(v);
      win_p->stop_buffering();
    }
  }
  return sel;
}


//----------------------------------------------------------------------------

string GW::set_data(node v, string data) { 
  string old_data(get_data(v));
  gr_p->set_node_entry(v,data);
  return old_data;
}

//----------------------------------------------------------------------------

string GW::set_user_label(node v, string label) 
{ string old_label = n_info[v].label;

  if (old_label.index("@") > -1 || label.index("@") > -1)
  { char* old_pr = set_pixmap(v,0);
    if (old_pr) get_window().del_pixrect(old_pr);
   }

  if (get_flush())
   { n_animation_start(v);
     n_info[v].label = label;
     n_animation_step();
     n_animation_end();
    }
  else
    n_info[v].label = label;
  return old_label;
}


string GW::set_label(node v, string label,bool adjust_node) 
{ set_label_type(v,user_label);
  string old_label = set_user_label(v,label);
  if (adjust_node) adjust_node_to_label(v);
  return old_label;
}


void GW::adjust_node_to_label(node v)
{
  if (get_label_pos(v) != central_pos || get_pixmap(v) != 0) return;

  string label = get_label(v);

  if (label.index("@") > -1) return;

  bool simple_label = true;

  for(int i=0; i<label.length(); i++)
  { char c = label[i];
    if ( c == '$' || c == '%' || c == '\\' )
    { simple_label = false;
      break;
     }
  }

  double tw=0,th=0;

  set_font(node_label_font_type,node_label_font_size,node_label_user_font);

  if (simple_label)
  { 
    tw = win_p->text_width(label);
    th = win_p->text_height(label);
/*
    string txt = label.replace_all("\n"," ");
    list<string> L = break_into_words(txt);

    string s;
    forall(s,L) 
    { int w = win_p->text_width(s);
      int h = win_p->text_height(s);
      if (w > tw) tw = w;
      th += h;
    }
*/
   }
  else
  { double x0,y0,x1,y1,ys;
    list<string> L;
    split_label(v,nil,label,L,x0,y0,x1,y1,ys);
    tw = x1-x0;
    th = y1-y0;
   }

  tw += win_p->pix_to_real(16);
  th += win_p->pix_to_real(16);

  //if ((get_label_type(v) == index_label || label == "") && tw < th) tw = th;
  //if (tw < th) tw = th;

  switch (get_shape(v)) {

  case roundrect_node: 
  case ovalrect_node: 
  case rectangle_node: break;

  case circle_node: { double R = sqrt(tw*tw + th*th);
                      tw = th = R;
                      break;
                     }

  case square_node: { double R = leda_max(tw,th);
                      tw = th = R;
                      break;
                     }

  case ellipse_node: { tw = sqrt(2.0) * tw;
                       th = sqrt(2.0) * th;
                       break;
                      }

  case rhombus_node: { tw = 2.0 * tw;
                       th = 2.0 * th;
                       break;
                      }
  default:
          break;
  }

  if (win_p->is_open())
  { int wpix = win_p->real_to_pix(tw);
    int hpix = win_p->real_to_pix(th);
    if (wpix%2) wpix++;
    if (hpix%2) hpix++;
    tw = win_p->pix_to_real(wpix);
    th = win_p->pix_to_real(hpix);
  }

  set_radius1(v,tw/2);
  set_radius2(v,th/2);
}

//----------------------------------------------------------------------------

#define GW_NODE_SET(f,e,t)                                                  \
t GW::set_##f(node v, t n) {                                          \
    t& e=n_info[v].e;                                                       \
    t old=e;                                                                \
    if (old == n) return n;                                                 \
    e = n;                                                                  \
    if (get_flush()) redraw_node(v);                                        \
    return old;                                                             }


GW_NODE_SET(color,clr,color)
GW_NODE_SET(border_color,border_clr,color)
GW_NODE_SET(label_type,label_t,gw_label_type)
GW_NODE_SET(label_color,label_clr,color)
GW_NODE_SET(label_pos,label_pos,gw_position)

char* GW::set_pixmap(node v, char* n)
{ char* old = n_info[v].pixmap;
  n_info[v].pixmap = n;
  if (old == n || n == 0) return old;

  bool fl = set_flush(false);
  int w = win_p->get_pixrect_width(n)+1;
  int h = win_p->get_pixrect_height(n)+1;
  if (n_info[v].shape == circle_node) { 
     if (w < h) h = w; else w = h;
   }
  set_width(v,w);
  set_height(v,h);
  if (fl) redraw_node(v);
  set_flush(fl);
  return old;
}


//----------------------------------------------------------------------------

double GW::set_radius1(node v, double r) {
  double old_r1 = n_info[v].r1;            
  if (old_r1 == r) return r;
  if (r>=0) n_info[v].r1 = float(r);
  embed_node_with_edges(v);
  if (get_flush()) redraw();
  return old_r1;
}

//----------------------------------------------------------------------------

double GW::set_radius2(node v, double r) {
  double old_r2=n_info[v].r2;
  if (old_r2 == r) return r;
  if (r>=0) n_info[v].r2 = float(r);
  embed_node_with_edges(v);
  if (get_flush()) redraw();
  return old_r2;
}


//----------------------------------------------------------------------------

int GW::set_width(node v, int w) 
{ double r_new;
  int    w_old;

  double r_old = get_radius1(v);
  if ( win_p->is_open() )
    { w_old = win_p->real_to_pix(2*r_old);
      r_new = win_p->pix_to_real(w)/2;
     }
  else
  { double win_w = window_width();
    double f = win_w/(xmax_def - xmin_def);
    w_old = int(2*f*r_old);
    r_new = w/(2*f);
   }

  set_radius1(v,r_new); 
  return w_old;
}



int  GW::set_height(node v, int h)
{ double r_new;
  int    h_old;

  double r_old = get_radius2(v);
  if ( win_p->is_open() )
    { h_old = win_p->real_to_pix(2*r_old);
      r_new = win_p->pix_to_real(h)/2;
     }
  else
  { double win_w = window_width();
    double f = win_w/(xmax_def - xmin_def);
    h_old = int(2*f*r_old);
    r_new = h/(2*f);
   }

  set_radius2(v,r_new); 
  return h_old;
}



double GW::set_border_thickness(node v, double t) {
  double vt = n_info[v].border_w;            
  if (vt != t)
  { n_info[v].border_w = float(t);
    if (get_flush()) redraw(v);
   }
  return vt;
}

int GW::set_border_width(node v, int w) 
{ double f = win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = win_w/(xmax_def - xmin_def);
   }
  return (int)(f*set_border_thickness(v,w/f)); 
}



//----------------------------------------------------------------------------

point GW::set_position(node v, const point& p) 
{
  point old_p = n_info[v].pos;
  if (old_p == p) return p;

  if (get_flush()) 
    move_node_with_edges(v,p,animation_steps);
  else
    { move_node(v,p-old_p);
      embed_node_with_edges(v);
     }

  return old_p;
}

//----------------------------------------------------------------------------

gw_node_shape GW::set_shape(node v, gw_node_shape new_shape) 
{ gw_node_shape& v_shape = n_info[v].shape;
  gw_node_shape old_shape = v_shape;
  if (old_shape == new_shape) return new_shape;
  v_shape = new_shape;
  embed_node_with_edges(v);
  if (get_flush()) redraw();
  return old_shape;
}


    
/****************************************************************************/
/*                     Settings for existing edges                          */
/****************************************************************************/

bool GW::set_select(edge e, bool b, bool rd) 
{ bool sel = e_info[e].selected;
  if (sel != b)
  { e_info[e].selected = b;
    if (b) 
       selected_edges.push(e);
    else 
       { list_item it;
         forall_items(it,selected_edges)
           if (selected_edges[it] == e) selected_edges.del_item(it);
        }
    if (rd) 
    { if (b == false)
         redraw(e);
      else
       { win_p->start_buffering();
         draw_edge(e);
         flush_edge(e);
         win_p->stop_buffering();
       }
    }
  }
  return sel;
}

//----------------------------------------------------------------------------

void GW::set_info(edge e, const edge_info& i, long mask) {
  edge_info& e_inf=e_info[e];
  bool old_flush=set_flush(false);

  if (mask & E_COLOR)  e_inf.clr=i.clr;
  if (mask & E_SHAPE)  e_inf.shape=i.shape; 
  if (mask & E_STYLE)  e_inf.style=i.style; 
  if (mask & E_DIR)    e_inf.dir=i.dir; 
  if (mask & E_WIDTH)  e_inf.width=i.width;
  if (mask & E_LCOLOR) e_inf.label_clr=i.label_clr;
  if (mask & E_LPOS)   e_inf.label_pos=i.label_pos;
  
  if (mask & E_LTYPE) e_inf.label_t = i.label_t;
  if (mask & E_LABEL) e_inf.label = i.label;

  if (mask & E_SANCHOR) e_inf.s_anchor = i.s_anchor;
  if (mask & E_TANCHOR) e_inf.t_anchor = i.t_anchor;

  if (mask & E_SELECT) set_select(e,i.selected);

  if (mask & E_POLYGON) {
    set_bends(e,i.p);
    if (old_flush) embed_edges(source(e),target(e));
  }

  set_flush(old_flush);

  if (get_flush()) redraw();
}

//----------------------------------------------------------------------------

gw_edge_shape GW::set_shape(edge e, gw_edge_shape new_shape) {
  gw_edge_shape &e_shape=e_info[e].shape;
  gw_edge_shape old_shape=e_shape;
  if (old_shape == new_shape) return new_shape;
  e_shape=new_shape;
  if (get_flush()) redraw(e);
  return old_shape;
}

void GW::set_edge_points(edge e, const list<point>& P) 
{ list<point>& L = get_poly(e);
  if (&P != &L) L = P;
  if (get_flush()) redraw(e);
}

//----------------------------------------------------------------------------

void GW::set_bends(edge e, const list<point>& P, double min_dist) 
{
  node v=source(e);
  node w=target(e);


  list<point>& L=get_poly(e);

  L = P;

  L.push(get_position(v));
  L.append(get_position(w));

  normalize_polygon(L,min_dist);

  if (L.size() == 1) L.append(get_position(w));

  embed_edges(v,w);

  if (get_flush()) redraw();
}

//----------------------------------------------------------------------------


string GW::set_data(edge e, string data) { 
  string old_data(get_data(e));
  gr_p->set_edge_entry(e,data);
  return old_data;
}

//----------------------------------------------------------------------------

string GW::set_user_label(edge e, string label) {
  string old_label = e_info[e].label;
  e_info[e].label = label;
  if (get_flush()) redraw_edge(e);
  return old_label;
}

string GW::set_label(edge e, string label) {
  string old_label = set_user_label(e,label);
  set_label_type(e,user_label);
  return old_label;
}


//----------------------------------------------------------------------------

color GW::set_label_color(edge e, color c) {
  color& clr=e_info[e].label_clr;
  color old_clr=clr;
  if (old_clr == c) return c;
  clr=c;
  if (get_flush()) redraw_edge(e);
  return old_clr;
}

//----------------------------------------------------------------------------

color GW::set_color(edge e, color c) {
  color &e_clr=e_info[e].clr;
  color old_color=e_clr;
  if (old_color == c) return c;
  e_clr=c;
  if (get_flush()) redraw_edge(e);
  return old_color;
}

//----------------------------------------------------------------------------

double GW::set_thickness(edge e, double t) {
  double et = e_info[e].width;            
  if (et != t)
  { e_info[e].width = float(t);
    if (get_flush()) redraw(e);
   }
  return et;
}

int GW::set_width(edge e, int w) 
{ double f = win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = win_w/(xmax_def - xmin_def);
   }
  return (int)(f*set_thickness(e,w/f)); 
}

//----------------------------------------------------------------------------

gw_edge_style GW::set_style(edge e, gw_edge_style st) {
  gw_edge_style &e_style=e_info[e].style;
  gw_edge_style old_style=e_style;
  if (old_style == st) return st;
  e_style=st;
  if (get_flush()) redraw_edge(e);
  return old_style;
}


gw_edge_dir GW::set_direction(edge e, gw_edge_dir dir) {
  gw_edge_dir &e_dir=e_info[e].dir;
  gw_edge_dir old_dir=e_dir;
  if (old_dir == dir) return dir;
  e_dir=dir;
  if (get_flush()) redraw_edge(e);
  return old_dir;
}

//----------------------------------------------------------------------------

gw_position GW::set_label_pos(edge e, gw_position lp) {
  gw_position &e_lp=e_info[e].label_pos;
  gw_position old_lp=e_lp;
  if (old_lp == lp) return lp;
  e_lp=lp;
  if (get_flush()) redraw_edge(e);
  return old_lp;
}


gw_label_type GW::set_label_type(edge e, gw_label_type lt) {
  gw_label_type &e_lt=e_info[e].label_t;
  gw_label_type old_lt=e_lt;
  e_lt=lt;
  if (get_flush()) redraw_edge(e);
  return old_lt;
}


point GW::set_source_anchor(edge e, point p)
{ point& anchor = e_info[e].s_anchor;
  point old = anchor;
  anchor = p;
  embed_node_with_edges(source(e));
  if (get_flush()) redraw(e);
  return old;
}

point GW::set_target_anchor(edge e, point p)
{ point& anchor = e_info[e].t_anchor;
  point old = anchor;
  anchor = p;
  embed_node_with_edges(target(e));
  if (get_flush()) redraw(e);
  return old;
}


point GW::get_source_anchor_pos(edge e) const
{ node v = source(e);
  point pos = get_position(v);
  point p = get_source_anchor(e);
  return point(pos.xcoord() + get_radius1(v)*p.xcoord(),
               pos.ycoord() + get_radius2(v)*p.ycoord());
}

point GW::get_target_anchor_pos(edge e) const
{ node v = target(e);
  point pos = get_position(v);
  point p = get_target_anchor(e);
  return point(pos.xcoord() + get_radius1(v)*p.xcoord(),
               pos.ycoord() + get_radius2(v)*p.ycoord());
}



point GW::set_source_anchor_pos(edge e, point p)
{ node v = source(e);
  point pos = get_position(v);
  point q((p.xcoord()-pos.xcoord())/get_radius1(v),
          (p.ycoord()-pos.ycoord())/get_radius2(v));
  return set_source_anchor(e,q);
}


point GW::set_target_anchor_pos(edge e, point p)
{ node v = target(e);
  point pos = get_position(v);
  point q((p.xcoord()-pos.xcoord())/get_radius1(v),
          (p.ycoord()-pos.ycoord())/get_radius2(v));
  return set_target_anchor(e,q);
}





double GW::set_slider_value(edge e, double f, int i)
{ double*& S = e_info[e].slider_f;
  if (S == 0)
  { S = new double[MAX_SLIDERS];
    for(int i=0; i < MAX_SLIDERS; i++) S[i] = 0;
   }
  double f0 = S[i];
  if (f < 0) f = 0;
  if (f > 1) f = 1;
  S[i] = f;
  if (get_flush()) redraw(e);
  return f0;
}


void GW::set_edge_slider_handler(void (*f)(GraphWin&,edge,double), int i)
{ if (i < 0  || i >= MAX_SLIDERS)
     LEDA_EXCEPTION(1,"set_edge_slider_handler: index out of range.");
  edge_slider_handler[i] = f; 
 }

void GW::set_start_edge_slider_handler(void (*f)(GraphWin&,edge,double), int i)
{ if (i < 0  || i >= MAX_SLIDERS)
     LEDA_EXCEPTION(1,"set_start_edge_slider_handler: index out of range.");
  start_edge_slider_handler[i] = f; 
 }


void GW::set_end_edge_slider_handler(void (*f)(GraphWin&,edge,double), int i)
{ if (i < 0  || i >= MAX_SLIDERS)
     LEDA_EXCEPTION(1,"set_end_edge_slider_handler: index out of range.");
  end_edge_slider_handler[i] = f; 
 }

void GW::set_edge_slider_color(color c, int i)
{ if (i < 0  || i >= MAX_SLIDERS)
     LEDA_EXCEPTION(1,"set_slider_color: index out of range.");
  slider_color[i] = c; 
 }



//----------------------------------------------------------------------------

/****************************************************************************/
/*                        Settings for new nodes                            */
/****************************************************************************/

void GW::set_node_info(const node_info& n, long mask) {
  if (mask & N_COLOR)  n_model.clr=n.clr;
  if (mask & N_PIXMAP) n_model.pixmap=n.pixmap;
  if (mask & N_SHAPE)  n_model.shape=n.shape;
  if (mask & N_BCOLOR) n_model.border_clr=n.border_clr; 
  if (mask & N_BWIDTH) n_model.border_w=n.border_w; 
  if (mask & N_WIDTH)  n_model.r1=n.r1;
  if (mask & N_HEIGHT) n_model.r2=n.r2;
  if (mask & N_LCOLOR) n_model.label_clr=n.label_clr;
  if (mask & N_LPOS)   n_model.label_pos=n.label_pos;
  if (mask & N_LTYPE)  n_model.label_t=n.label_t;
  if (mask & N_LABEL)  n_model.label=n.label;
  if (mask & N_POS)    n_model.pos=n.pos;
  if (mask & N_SELECT) n_model.selected=n.selected;
}

//----------------------------------------------------------------------------

#define GW_NMODEL_SET(f,e,t)               \
t GW::set_node_##f(t n, bool apply)        \
{ t old=n_model.e;                         \
  n_model.e=n;                             \
  if (apply) set_##f(gr_p->all_nodes(),n); \
  return old;                              \
}

#define GW_NMODEL_SETF(f,e,t)              \
t GW::set_node_##f(t n, bool apply)        \
{ t old=n_model.e;                         \
  n_model.e=float(n);                      \
  if (apply) set_##f(gr_p->all_nodes(),n); \
  return old;                              \
}

GW_NMODEL_SET(color,clr,color)
GW_NMODEL_SET(border_color,border_clr,color)
GW_NMODEL_SET(shape,shape,gw_node_shape)
GW_NMODEL_SET(label_type,label_t,gw_label_type)
GW_NMODEL_SET(label_pos,label_pos,gw_position)
GW_NMODEL_SET(label_color,label_clr,color)

GW_NMODEL_SETF(radius1,r1,double)
GW_NMODEL_SETF(radius2,r2,double)
GW_NMODEL_SETF(border_thickness,border_w,double)



char* GW::set_node_pixmap(char* pmap, bool apply) 
{ char* old = n_model.pixmap;
  n_model.pixmap = pmap;
  int w = win_p->get_pixrect_width(pmap);
  int h = win_p->get_pixrect_height(pmap);
  if (n_model.shape == circle_node) { 
     if (w < h) h = w; else w = h;
   }
  set_node_width(w,apply);
  set_node_height(h,apply);
  if (apply) set_pixmap(gr_p->all_nodes(),pmap);
  return old;
}


int GW::set_node_width(int w, bool apply) 
{ double f = 2*win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = 2*win_w/(xmax_def - xmin_def);
   }
  if (apply) set_width(gr_p->all_nodes(),w);
  return (int)(f*set_node_radius1(w/f, apply)); 
}

int GW::set_node_height(int h, bool apply) 
{ double f = 2*win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = 2*win_w/(xmax_def - xmin_def);
   }
  if (apply) set_height(gr_p->all_nodes(),h);
  return (int)(f*set_node_radius2(h/f, apply)); 
}


int GW::set_node_border_width(int w, bool apply) 
{ double f = win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = win_w/(xmax_def - xmin_def);
   }
  if (apply) set_border_width(gr_p->all_nodes(),w);
  return (int)(f*set_node_border_thickness(w/f)); 
}




/****************************************************************************/
/*                        Settings for new edges                            */
/****************************************************************************/

void GW::set_edge_info(const edge_info& i, long mask) {
  if (mask & E_COLOR)   e_model.clr=i.clr;
  if (mask & E_SHAPE)   e_model.shape=i.shape; 
  if (mask & E_STYLE)   e_model.style=i.style; 
  if (mask & E_DIR)     e_model.dir=i.dir; 
  if (mask & E_WIDTH)   e_model.width=i.width;
  if (mask & E_LTYPE)   e_model.label_t=i.label_t;
  if (mask & E_LABEL)   e_model.label=i.label;
  if (mask & E_LCOLOR)  e_model.label_clr=i.label_clr;
  if (mask & E_LPOS)    e_model.label_pos=i.label_pos;
  if (mask & E_POLYGON) e_model.p=i.p;
  if (mask & E_SELECT)  e_model.selected=i.selected;
}

//----------------------------------------------------------------------------

#define GW_EMODEL_SET(f,el,t)             \
t GW::set_edge_##f(t n, bool apply)       \
{ t old=e_model.el;                       \
  e_model.el=n;                           \
  if (apply) set_##f(gr_p->all_edges(),n);\
  return old;                             \
}

#define GW_EMODEL_SETF(f,el,t)            \
t GW::set_edge_##f(t n, bool apply)       \
{ t old=e_model.el;                       \
  e_model.el=float(n);                    \
  if (apply) set_##f(gr_p->all_edges(),n);\
  return old;                             \
}


GW_EMODEL_SET(color,clr,color)
GW_EMODEL_SET(shape,shape,gw_edge_shape)
GW_EMODEL_SET(style,style,gw_edge_style)
GW_EMODEL_SET(direction,dir,gw_edge_dir)
GW_EMODEL_SETF(thickness,width,double)
GW_EMODEL_SET(label_type,label_t,gw_label_type)
GW_EMODEL_SET(label_pos,label_pos,gw_position)
GW_EMODEL_SET(label_color,label_clr,color)


int GW::set_edge_width(int w, bool apply) 
{ double f = win_p->scale(); 
  if ( !win_p->is_open() )
  { double win_w = window_width();
    f = win_w/(xmax_def - xmin_def);
   }
  if (apply) set_width(gr_p->all_edges(),w);
  return (int)(f*set_edge_thickness(w/f)); 
}



/****************************************************************************/
/*                     Settings for list of nodes                           */
/****************************************************************************/

#define NLIST_SET(f,arg_t)	                                              \
  void GW::set_##f(const list<node>& L, arg_t a) {                      \
    node v;                                                                   \
    bool old_flush=set_flush(false);                                          \
    forall(v,L) set_##f(v,a);                                                 \
    if (old_flush) redraw();                                                  \
    set_flush(old_flush);                                                     }

NLIST_SET(color,color)
NLIST_SET(pixmap,char*)
NLIST_SET(shape,gw_node_shape)
NLIST_SET(border_color,color)
NLIST_SET(border_thickness,double)
NLIST_SET(border_width,int)
NLIST_SET(radius1,double)
NLIST_SET(radius2,double)
NLIST_SET(user_label,string)
NLIST_SET(label,string)
NLIST_SET(label_type,gw_label_type)
NLIST_SET(label_pos,gw_position)
NLIST_SET(label_color,color)
NLIST_SET(width,int)
NLIST_SET(height,int)

void GW::set_info(const list<node>& L, const node_info& n, long mask) {
  node v;
  if (&L == &selected_nodes) mask &= ~N_SELECT;
  bool old_flush=set_flush(false);
  forall(v,L) set_info(v,n,mask);
  set_flush(old_flush);
  if (old_flush) redraw();
  
}

//----------------------------------------------------------------------------

/****************************************************************************/
/*                     Settings for list of edges                           */
/****************************************************************************/

#define ELIST_SET(f,arg_t)	                                            \
  void GW::set_##f(const list<edge>& L, arg_t a) {                    \
    edge e;                                                                 \
    bool old_flush=set_flush(false);                                        \
    forall(e,L) set_##f(e,a);                                               \
    if (old_flush) redraw();                                                \
    set_flush(old_flush);                                                   \
                                                                            }
ELIST_SET(color,color)
ELIST_SET(shape,gw_edge_shape)
ELIST_SET(style,gw_edge_style)
ELIST_SET(direction,gw_edge_dir)
ELIST_SET(user_label,string)
ELIST_SET(label,string)
ELIST_SET(label_type,gw_label_type)
ELIST_SET(label_pos,gw_position)
ELIST_SET(label_color,color)
ELIST_SET(width,int)
ELIST_SET(thickness,double)
ELIST_SET(source_anchor,point)
ELIST_SET(target_anchor,point)

void GW::set_info(const list<edge>& L, const edge_info& i, long mask) {
  edge e;
  if (&L == &selected_edges) mask &= ~E_SELECT; 
  bool old_flush=set_flush(false);
  forall(e,L) set_info(e,i,mask);
  set_flush(old_flush);
  if (old_flush) redraw();
}

//--------n--------------------------------------------------------------------

/****************************************************************************/
/*                      Settings for arrays of nodes                        */
/****************************************************************************/

void GW::get_position(node_array<point>& pos) 
{ node v;
  forall_nodes(v,*gr_p) pos[v] = n_info[v].pos;
 }  

void GW::set_position(const node_array<point>& dest) 
{ if (get_flush()) 
     move_nodes(dest);
  else 
    { node v;
      forall_nodes(v,*gr_p) n_info[v].pos=dest[v]; 
      embed_edges();
      //edges_embedded=false;
    }
}  


void GW::set_layout(const node_array<point>& n_dest,
                    const node_array<double>& n_rad1,
                    const node_array<double>& n_rad2,
                    const edge_array<list<point> >& e_dest,
                    const edge_array<point>& e_sanch,
                    const edge_array<point>& e_tanch)
{ 
  if (get_flush()) 
  { 
    unsigned anim = animation_steps;

    if (anim == 0) anim = 1;

    node_array<vector>        p_trans(*gr_p);
    node_array<double>        r1_trans(*gr_p);
    node_array<double>        r2_trans(*gr_p);
    edge_array<list<vector> > e_trans(*gr_p);
    edge_array<vector>        sa_trans(*gr_p);
    edge_array<vector>        ta_trans(*gr_p);
    list<node> L;
    list<edge> E;

    bool save_f = set_flush(false);

    set_layout_start(n_dest,n_rad1,n_rad2,e_dest,e_sanch,e_tanch,
                     L,E,p_trans,r1_trans,r2_trans,e_trans,sa_trans,ta_trans);

    while (--anim > 0)
      set_layout_step(L,E,p_trans,r1_trans,r2_trans,e_trans,sa_trans,ta_trans);

    set_layout_end(L,E,n_dest,n_rad1,n_rad2,e_dest,e_sanch,e_tanch);

    set_flush(save_f);

    return;
   }

  node v;
  forall_nodes(v,*gr_p) 
  { move_node(v,n_dest[v] - n_info[v].pos); 
    n_info[v].r1 = float(n_rad1[v]);
    n_info[v].r2 = float(n_rad2[v]);
   }

  edge e;
  forall_edges(e,*gr_p) 
  { set_bends(e,e_dest[e]);
    e_info[e].s_anchor = e_sanch[e];
    e_info[e].t_anchor = e_tanch[e];
   }

  embed_edges();
}

void GW::set_layout(const node_array<double>& x, 
                    const node_array<double>& y,
                    const node_array<double>& rad1, 
                    const node_array<double>& rad2,
                    const edge_array<list<double> >& xbends,
                    const edge_array<list<double> >& ybends,
                    const edge_array<double>& x_sanch, 
                    const edge_array<double>& y_sanch,
                    const edge_array<double>& x_tanch, 
                    const edge_array<double>& y_tanch)
{ 
  node_array<point> pos(*gr_p);
  node v;
  forall_nodes(v,*gr_p) pos[v] = point(x[v],y[v]);

  edge_array<point> sanch(*gr_p);
  edge_array<point> tanch(*gr_p);

  edge_array<list<point> > bends(*gr_p);
  edge e;
  forall_edges(e,*gr_p) 
  { list_item it1 = xbends[e].first();
    list_item it2 = ybends[e].first();
    while(it1 && it2)
    { bends[e].append(point(xbends[e][it1],ybends[e][it2]));
      it1 = xbends[e].succ(it1);
      it2 = ybends[e].succ(it2);
     }
    sanch[e] = point(x_sanch[e],y_sanch[e]);
    tanch[e] = point(x_tanch[e],y_tanch[e]);
   }

  set_layout(pos,rad1,rad2,bends,sanch,tanch);
}




void GW::set_layout(const node_array<point>& pos,
                    const edge_array<list<point> >& bends, bool reset_anchors) 
{ 
  graph& G = get_graph();

  node_array<double> rad1(G);
  node_array<double> rad2(G);

  edge_array<point> sanch(G);
  edge_array<point> tanch(G);

  node v;
  forall_nodes(v,G)
  { rad1[v] = get_radius1(v);
    rad2[v] = get_radius2(v);
   }

  edge e;
  forall_edges(e,G)
  { sanch[e] = get_source_anchor(e);
    tanch[e] = get_target_anchor(e);
   }


  if (reset_anchors) reset_edge_anchors();

  set_layout(pos,rad1,rad2,bends,sanch,tanch);

}


void GW::set_layout(const node_array<double>& x, const node_array<double>& y,
                    const edge_array<list<double> >& xbends,
                    const edge_array<list<double> >& ybends, bool reset_anchors)
{ 
  node_array<point> pos(*gr_p);
  node v;
  forall_nodes(v,*gr_p) pos[v] = point(x[v],y[v]);

  edge_array<list<point> > bends(*gr_p);
  edge e;
  forall_edges(e,*gr_p) 
  { list_item it1 = xbends[e].first();
    list_item it2 = ybends[e].first();
    while(it1 && it2)
    { bends[e].append(point(xbends[e][it1],ybends[e][it2]));
      it1 = xbends[e].succ(it1);
      it2 = ybends[e].succ(it2);
     }
   }
  set_layout(pos,bends,reset_anchors);
 }


void GW::set_layout(const node_array<point>& pos) 
{ edge_array<list<point> > bends(*gr_p);
  set_layout(pos,bends);
 }

void GW::set_layout(const node_array<double>& x, const node_array<double>& y) 
{ node_array<point> pos(*gr_p);
  node v;
  forall_nodes(v,*gr_p) pos[v] = point(x[v],y[v]);
  set_layout(pos);
 }

void GW::set_layout() 
{ node_array<point> pos(*gr_p);
  node v;
  forall_nodes(v,*gr_p) pos[v] = get_position(v);
  set_layout(pos);
}


void GW::set_position(const node_array<double>& destx, 
                      const node_array<double>& desty  ) 
{ node_array<point> dest(*gr_p);
  node v;
  forall_nodes(v,*gr_p) dest[v]=point(destx[v],desty[v]);
  set_position(dest);
}  


void GW::set_position(const list<node>& L, const point& p) {
  if (L.size() == 0) return;
  node v;
  if (get_flush()) {
    if (node_move == move_single_node) {
      forall(v,L) move_node_with_edges(v,p,animation_steps);
      return;
    }
    n_animation_start(L);
    int anim = animation_steps;
    if (anim) {
      node_array<vector> trans(*gr_p);
      forall(v,L) trans[v] = (p-get_position(v))*(1.0/(anim+1));
      do {
        forall(v,L) n_info[v].pos = n_info[v].pos + trans[v];
        n_animation_step();
      }
      while (--anim > 0);
    }
    forall(v,L) n_info[v].pos = p;
    n_animation_end();
  }
  else {
    forall(v,L) n_info[v].pos = p;
    edges_embedded = false;
  }
}



/****************************************************************************/
/*                          global Settings                                 */
/****************************************************************************/

#define GW_SET(f,e,t) t GW::set_##f(t n) { t old=e; e=n; return old; }

GW_SET(zoom_labels,zoom_labels,bool)
GW_SET(zoom_hide_labels,zoom_hide_labels,bool)
GW_SET(zoom_objects,zoom_objects,bool)
GW_SET(dirname,gw_dirname,string)
//GW_SET(graphname,graph_name,string)
GW_SET(ps_fontname,ps_fontname,string)
GW_SET(print_cmd,print_cmd,string)
GW_SET(animation_steps,animation_steps,int)
GW_SET(animation_mode,node_move,node_move_t)

//GW_SET(flush,flush,bool)

string GW::set_graphname(string n)
{ string old = graph_name;
  graph_name = n;
  update_status_line();
  return old;
}


ps_zoom_mode GW::get_ps_zoom_mode() const
{ int t = int(-ps_factor);
  return (ps_zoom_mode)t;
}

ps_zoom_mode GW::set_ps_zoom_mode(ps_zoom_mode t)
{ ps_zoom_mode old = get_ps_zoom_mode();
  ps_factor = -int(t);
  return old;
}


bool GW::set_flush(bool f)
{ bool old;
  if (win_p->is_open())
   { old = flush;
     flush = f;
    }
  else
   { old = flush0;
     flush0 = f;
    }
 return old;
}


//----------------------------------------------------------------------------

bool GW::is_call_enabled(int c) 
{ return menu_functions[c]->enabled; }

void GW::enable_call(int c) 
{ menu_functions[c]->enabled = true; }

void GW::disable_call(int c) 
{ menu_functions[c]->enabled = false; }


//----------------------------------------------------------------------------

void GW::enable_calls() {
  panel_item it;
  forall_items(it,*menu_p) menu_p->enable_item(it);
}

void GW::disable_calls() {
  panel_item it;
  forall_items(it,*menu_p) 
  { if (menu_p->get_kind(it) != Button_Item) continue;
    string s = menu_p->get_label(it);
    if (s != "done" && s != "?" && s != "File") 
        menu_p->disable_item(it);
   }
}

//----------------------------------------------------------------------------

bool GW::set_auto_create_target(bool b) {
  bool b0 = auto_create_target;
  auto_create_target = b;
  return b0;
}


bool GW::set_ortho_mode(bool b) {
  bool b0 = ortho_mode;
  ortho_mode = b;
  if (get_flush()) redraw();
  return b0;
}

bool GW::set_d3_look(bool b) {
  bool b0 = d3_look;
  d3_look = b;
  if (get_flush()) redraw();
  return b0;
}

//----------------------------------------------------------------------------

int GW::set_grid_dist(int d)
{ window& W = get_window();
  W.set_grid_dist(-d); // draw grid only (no rounding to grid points !)
  int old_grid = grid_dist;
  grid_dist = d;
  if (d != 0) snap_to_grid();
  return old_grid;
}


int GW::get_grid_size() const {
  double width = get_xmax() - get_xmin();
  double d = get_grid_dist();
  return (d > 0) ? int(width/d) : 0;
}

int GW::set_grid_size(int sz) {
  double width = get_xmax() - get_xmin();
  int d = (sz > 0) ? int(width/sz) : 0;
  double d0 = set_grid_dist(d);
  return (d0 > 0) ? int(width/d0) : 0;
}

grid_style GW::set_grid_style(grid_style s) {
  window& W = get_window();
  grid_style s0 = g_style;
  g_style = s;
  W.set_grid_style(s);
  return s0;
}



int GW::set_grid_mode(int d)
{ if (g_style == invisible_grid) g_style = point_grid;
  return set_grid_dist(d);
}


//----------------------------------------------------------------------------

bool GW::set_show_status(bool b) {
  if (b != show_status && win_p->is_open())
  { if (b) 
      { //win_p->open_status_window(status_win_height,grey1);
        win_p->open_status_window();
        BASE_WINDOW* sw = win_p->get_status_window();
        sw->set_fixed_font();
       }
    else 
      win_p->close_status_window();
    update_win_bounds();
   }
  bool old_show_status=show_status;
  show_status=b;
  return old_show_status;
}

//----------------------------------------------------------------------------

double GW::set_zoom_factor(double f) {
  double old_zoom_factor=zoom_factor;
  f=fabs(f);
  if (f > 0.0) zoom_factor=(f > 1.0 ? f : 1.0/f);
  return old_zoom_factor;
}

//----------------------------------------------------------------------------


color GW::get_bg_color() { return bg_color; }

color GW::set_bg_color(color c) 
{ color old_bg = bg_color;
  bg_color = c;
  win_p->set_bg_color(c);
  if (win_p->is_open()) redraw();
  return old_bg;
}

char* GW::get_bg_pixmap() { return bg_pixmap; }

char* GW::set_bg_pixmap(char* pr, double xorig, double yorig) 
{ char* old_pr = bg_pixmap;
  bg_pixmap = pr;
  win_p->set_bg_pixmap(pr);
  win_p->clear(xorig,yorig);
  if (win_p->is_open()) redraw();
  return old_pr;
}


void GW::set_bg_xpm(const char** xpm_data)
{ char* pr = (xpm_data) ? win_p->create_pixrect_from_xpm(xpm_data) : 0;
  set_bg_pixmap(pr);
}


//----------------------------------------------------------------------------

int GW::set_gen_nodes(int n) {
  if (n < 0) return gen_nodes;
  int old_gen_nodes=gen_nodes;
  gen_nodes=n;
  return old_gen_nodes;
}

//----------------------------------------------------------------------------

int GW::set_gen_edges(int m) {
  if (m < 0) return gen_edges;
  int old_gen_edges=gen_edges;
  gen_edges=m;
  return old_gen_edges;
}

//----------------------------------------------------------------------------

int GW::set_buttons_per_line(int c) {
  int old_buttons_per_line=buttons_per_line;
  menu_p->buttons_per_line(c);
  buttons_per_line=c;
  return old_buttons_per_line;
}  

//----------------------------------------------------------------------------

int GW::set_edge_distance(int d) {
  double dist = win_p->pix_to_real(d);
  if (dist == edge_distance) return d;
  double old_distance=edge_distance;
  edge_distance=dist;
  embed_edges();
  if (get_flush()) redraw();
  return win_p->real_to_pix(old_distance);
}


void GW::get_node_label_font(gw_font_type& t, int& sz) const    
{ sz = win_p->real_to_pix(node_label_font_size); 
  t = node_label_font_type;
 }

void GW::set_node_label_font(gw_font_type t, int sz)
{ node_label_font_type = t;
  node_label_font_size = win_p->pix_to_real(sz);
  node_label_user_font = "";
 }

void GW::get_edge_label_font(gw_font_type& t, int& sz) const    
{ sz = win_p->real_to_pix(edge_label_font_size); 
  t = edge_label_font_type;
 }

void GW::set_edge_label_font(gw_font_type t, int sz)
{ edge_label_font_type = t;
  edge_label_font_size = win_p->pix_to_real(sz);
  edge_label_user_font = "";
 }


//----------------------------------------------------------------------------

/****************************************************************************/
/*                           reset - functions                              */
/****************************************************************************/


void GW::reset_nodes(long mask) {
  bool old_flush=set_flush(false);

  node_info v_inf;

  v_inf.pos       = point(0,0);
  v_inf.clr       = DefNodeColor;
  v_inf.shape     = DefNodeShape;
  v_inf.border_clr= DefNodeBorderColor;
  v_inf.border_w  = float(DefNodeBorderWidth);

  v_inf.r1        = float(DefNodeRadius1);
  v_inf.r2        = float(DefNodeRadius2);

  v_inf.label     = DefNodeLabel;
  v_inf.label_t   = DefNodeLabelType;
  v_inf.label_clr = DefNodeLabelClr;
  v_inf.label_pos = DefNodeLabelPos;
  v_inf.selected  = false;

  node v;
  forall_nodes(v,*gr_p) {
    set_info(v,v_inf,mask);
  }

  set_flush(old_flush);
  if (old_flush) redraw();
}



//----------------------------------------------------------------------------

void GW::reset_edges(long mask) {
  bool old_flush=set_flush(false);

  edge_info e_inf;

  e_inf.clr       = DefEdgeColor;
  e_inf.shape     = DefEdgeShape;
  e_inf.style     = DefEdgeStyle;
  e_inf.dir       = DefEdgeDir;
  e_inf.width     = (float)DefEdgeWidth;
  e_inf.label     = DefEdgeLabel;
  e_inf.label_t   = DefEdgeLabelType;
  e_inf.label_clr = DefEdgeLabelClr;
  e_inf.label_pos = DefEdgeLabelPos;
  e_inf.selected  = false;

  edge e;
  forall_edges(e,*gr_p) {
    e_inf.p.clear();
    e_inf.p.push(get_position(source(e)));
    e_inf.p.append(get_position(target(e)));
    set_info(e,e_inf,mask);
  }                     

  set_flush(old_flush); 
  if (old_flush) redraw();
}

//----------------------------------------------------------------------------

void GW::reset() {
  bool old_flush=set_flush(false);
  reset_nodes();
  reset_edges();
  set_flush(old_flush);
  if (old_flush) redraw();
}    

//----------------------------------------------------------------------------

void GW::set_select(const list<node>& L, bool sel)
{ node v;
  forall(v,L) set_select(v,sel,false);
  if (win_p->is_open()) redraw();
}

void GW::set_select(const list<edge>& L, bool sel)
{ edge e;
  forall(e,L) set_select(e,sel,false);
  if (win_p->is_open()) redraw();
}


void GW::select_all_nodes() 
{ //set_select(gr_p->all_nodes(),true); 
  node v;
  forall_nodes(v,*gr_p)
    if (get_pixmap(v) || (get_color(v) != invisible && 
                          get_border_color(v) != invisible))
       set_select(v,true,false);
  if (win_p->is_open()) redraw();
}

void GW::select_all_edges() 
{ //set_select(gr_p->all_edges(),true); 
  edge e;
  forall_edges(e,*gr_p)
    if (get_color(e) != invisible) set_select(e,true,false);
  if (win_p->is_open()) redraw();
}

void GW::deselect_all_nodes() { set_select(gr_p->all_nodes(),false); }
void GW::deselect_all_edges() { set_select(gr_p->all_edges(),false); }


void GW::save_node_attributes()
{ if (save_n_info) delete save_n_info;
  save_n_info = new node_map<node_info>;
  node v;
  forall_nodes(v,*gr_p) (*save_n_info)[v] = n_info[v];
 }


void GW::save_edge_attributes()
{ if (save_e_info) delete save_e_info;
  save_e_info = new edge_map<edge_info>;
  edge e;
  forall_edges(e,*gr_p) (*save_e_info)[e] = e_info[e];
 }


void GW::save_all_attributes()
{ save_node_attributes();
  save_edge_attributes();
 }


void GW::restore_all_attributes()
{ bool f0 = set_flush(false);
  restore_node_attributes();
  restore_edge_attributes();
  if (f0) redraw();
  set_flush(f0);
 }



void GW::restore_node_attributes()
{ if (save_n_info)
  { node v;
    forall_nodes(v,*gr_p) n_info[v] = (*save_n_info)[v];
   }
  if (get_flush()) redraw();
 }

void GW::restore_edge_attributes()
{ if (save_e_info)
  { edge e;
    forall_edges(e,*gr_p) e_info[e] = (*save_e_info)[e];
   }
  if (get_flush()) redraw();
 }



void GW::get_default_win_coords(double& xmin, double& xmax, double& ymin)
{ xmin = xmin_def;
  xmax = xmax_def;
  ymin = ymin_def;
 }



void GW::set_default_win_coords(double xmin, double xmax, double ymin)
{ xmin_def = xmin;
  xmax_def = xmax;
  ymin_def = ymin;
 }

void GW::set_default_win_coords() {
  set_default_win_coords(DefXMin,DefXMax,DefYMin);
}


string GW::set_node_index_format(string format)
{ string old_f = node_index_format;
  node_index_format = format;
  if (get_flush()) redraw();
  return old_f;
 }


string GW::set_edge_index_format(string format)
{ string old_f = edge_index_format;
  edge_index_format = format;
  if (get_flush()) redraw();
  return old_f;
 }

bool GW::set_edge_border(bool b)
{ bool old_b = edge_border;
  edge_border = b;
  if (get_flush()) redraw();
  return old_b;
 }



void GW::set_window(window& W)
{ win_p = &W; 
  redraw();
 }



void GW::set_graph(graph& G)
{ bool fl = set_flush(false);
  if (create_flag & CR_GRAPH) delete gr_p;
  create_flag &= ~CR_GRAPH;
  gr_p = &G;
  init_graph();
  set_flush(fl);
  if (fl) redraw();
}


string GW::get_xpm_path()
{ const char* lroot = getenv("LEDAROOT");
  if (lroot == 0) lroot = "/LEDA/INSTALL";
  string path = string(lroot) + "/incl/LEDA/graphics/pixmaps/button32"
              + ":" + string(lroot) + "/incl/LEDA/graphics/pixmaps/balls";
  char* xpmp = getenv("GW_XPM_PATH");
  if (xpmp) path = string(xpmp) + ":" + path;
  return path;
 }

bool GW::enable_label_box(bool b)
{
  bool b0 = labelBoxEnabled;
  labelBoxEnabled = b;
  return b0;
}
LEDA_END_NAMESPACE
