/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  edit.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/assert.h>
#include "local.h"
#include "graphwin_undo.h"


LEDA_BEGIN_NAMESPACE

static void dfs(graph& G, node v, node_array<bool>& reached, list<node>& L) { 
  reached[v] = true;
  L.append(v);
  edge e;
  forall_adj_edges(e,v) { 
    node u = G.opposite(v,e);
    if (!reached[u]) dfs(G,u,reached,L);
  }
  forall_in_edges(e,v) { 
    node u = G.opposite(v,e);
    if (!reached[u]) dfs(G,u,reached,L);
  }
}

//----------------------------------------------------------------------------

void gw_setup_component(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil) return;
  graph& G=gw.get_graph();
  list<node> V;
  node_array<bool> n_reached(G,false);
  dfs(G,v,n_reached,V);
  gw_setup(gw,V);
}

//----------------------------------------------------------------------------


void gw_move_nodes(GraphWin& gw, const point& p, const list<node>& V) 
{
  if (V.empty()) return;

  graph&  G = gw.get_graph();
  window& W = gw.get_window();

  node_array<bool> in_V(G,false);
  node v;
  forall(v,V) in_V[v] = true;

  list<edge> E;       // edges to move

  int move_count = 0; // number of items to move

  // undo
  list<edge> changed_edges;

  forall(v,V) 
  { move_count += G.outdeg(v);
    move_count += G.indeg(v);
    edge e;
    forall_adj_edges(e,v) 
    { if (in_V[target(e)]) E.append(e);
      changed_edges.append(e); 
     }
    forall_in_edges(e,v) changed_edges.append(e); 
   }

  gw.changing_layout(V,changed_edges);
  

  bool nodes_only = move_count > gw.get_max_move_items();

  point p1 = gw.grid_point(p.xcoord(),p.ycoord());

  unsigned long t = W.button_press_time();
  int event;

  W.grab_mouse();

  gw.n_animation_start(V,nodes_only);

  do { double x1,y1;
       int val;
       unsigned long t1;
       event = W.read_event(val,x1,y1,t1);
       if (event == motion_event) if (t1 - t < 50) continue;
       t = t1;
       point q = gw.grid_point(x1,y1);
       if (q == p1) continue;
       vector vec = q - p1;
       p1 = q;
       node v;
       forall(v,V) gw.move_node(v,vec);
       edge e;
       forall(e,E) gw.move_edge(e,vec);
       gw.n_animation_step(nodes_only);
  } while (event != button_release_event);

  W.ungrab_mouse();

  gw.n_animation_end();
  if (nodes_only) gw.redraw();
}



void gw_move_node(GraphWin& gw, const point& p) 
{
  graph&  G = gw.get_graph();
  window& W = gw.get_window();

  node v = gw.get_found_node();

  if (v == nil) return;


  if (gw.is_selected(v) && gw.get_selected_nodes().length() > 1)
  { gw_move_nodes(gw,p,gw.get_selected_nodes()); 
    return; 
   }

  if (!gw.check_start_move_node_handler(v)) return;


  // undo

  list<edge> changed_edges;
  edge e;
  forall_adj_edges(e,v) changed_edges.append(e);
  forall_in_edges(e,v) changed_edges.append(e);

  list<node> changed_nodes(v); // changed_nodes += v;
  gw.changing_layout(changed_nodes,changed_edges);

  gw.call_start_move_node_handler(v);


  bool node_only = false;

  if (G.degree(v) > gw.get_max_move_items()
      /* || G.number_of_nodes() > 2* gw.get_max_move_items() */ ) 
     node_only = true;

  bool move_handler = gw.has_move_node_handler();


  if (move_handler)
     { list<node> L; // = G.all_nodes();
       node v;
       forall_nodes(v,G) L.append(v);
       gw.n_animation_start(L,node_only);
      }
  else
     gw.n_animation_start(v,node_only);

  unsigned long t = W.button_press_time();
  unsigned long t1;

  point  q  = gw.get_position(v);
  double x  = p.xcoord();
  double y  = p.ycoord();
  double dx = q.xcoord() - x;
  double dy = q.ycoord() - y;


  point q0 = q;

  int event,val;
  do {
    W.grab_mouse();
    do { event = W.read_event(val,x,y,t1);
         if (event == motion_event) if (t1 - t < 50) continue;
         t = t1;
         q = gw.grid_point(x+dx,y+dy);
         if (gw.check_move_node_handler(v,q))
         { gw.set_position(v,q);
           if (move_handler) gw.call_move_node_handler(v);
           gw.n_animation_step(node_only);
         }
         q0 = q;
       } while (event != button_release_event);
     W.ungrab_mouse();
   } while (!gw.check_end_move_node_handler(v,q));


  gw.n_animation_end();

  if (node_only) gw.redraw();

  gw.call_end_move_node_handler(v);
}


void gw_resize_node(GraphWin& gw, const point& p) 
{ 
  window& W = gw.get_window();

  node v = gw.get_found_node();

  if (v == nil) return;

  // undo
  gw.changing_layout(v);

  double bt = gw.get_border_thickness(v);
  color  bc = gw.get_border_color(v);

  if (bt == 0) gw.set_border_width(v,1);
  if (bc == invisible) gw.set_border_color(v,black);

  gw.n_animation_start(v);

  unsigned long t = W.button_press_time();
  unsigned long t1;

  point  q = gw.get_position(v);
  double x = p.xcoord();
  double y = p.ycoord();
  double d = W.pix_to_real(1);

  long status_save_mask = gw.status_show_mask;

  gw.status_show_mask &= ~STAT_COORD;

  gw_node_shape s = gw.get_shape(v);

  bool show_box = (s == ellipse_node   || s == rhombus_node ||
                   s == roundrect_node || s == ovalrect_node);

  if (show_box)
  { if (x > q.xcoord()) 
       x = q.xcoord() + gw.get_radius1(v);
    else
       x = q.xcoord() - gw.get_radius1(v);
    if (y > q.ycoord()) 
       y = q.ycoord() + gw.get_radius2(v);
    else
       y = q.ycoord() - gw.get_radius2(v);
    W.move_pointer(x,y);
   }

  int event,val;
  do { event = W.read_event(val,x,y,t1);
       if (event == motion_event) if (t1 - t < 50) continue;
       t = t1;
       double r1 = x-q.xcoord();
       double r2 = y-q.ycoord();
       if (s == circle_node) 
       { r1 = sqrt(r1*r1 + r2*r2);
         r2 = r1;
        }
       else
       { r1 = fabs(r1);
         r2 = fabs(r2);
         if (s == square_node) 
         { if (r1 < r2) leda_swap(r1,r2);
           r2 = r1;
          }
        }
       gw.set_radius1(v,r1);
       gw.set_radius2(v,r2);

       gw.n_animation_step();

       if (show_box)
       {  line_style ls = W.set_line_style(dotted);
          W.draw_rectangle(q.xcoord()-r1-d, q.ycoord()-r2-d,
                           q.xcoord()+r1+d, q.ycoord()+r2+d);
          W.set_line_style(ls);
        }

       int w = W.real_to_pix(r1);
       int h = W.real_to_pix(r2);
       string msg("  r1 = %5.2f (%d)   r2 = %5.2f (%d)",r1,w,r2,h);
       W.set_status_string(msg);

     } while (event != button_release_event);

  gw.set_border_thickness(v,bt);
  bc = gw.set_border_color(v,bc);
  gw.n_animation_step();

  gw.n_animation_end();

  gw.status_show_mask = status_save_mask;
  gw.update_status_line();
}

 

//----------------------------------------------------------------------------


static list_item min_poly_dist(const list<point>& P, const point& p) {
  if (P.size() < 2) return nil;
  list_item it,it2,min_it=nil;
  double min=MAXDOUBLE,d;
  double x_min,y_min,x_max,y_max;

  it2=P.succ(it=P.first());

  do {
    const point& p1 = P[it];
    const point& p2 = P[it2];

    if (p1.xcoord() > p2.xcoord()) {
      x_min=p2.xcoord();
      x_max=p1.xcoord();
    }
    else {
      x_min=p1.xcoord();
      x_max=p2.xcoord();
    }
    
    if (p1.ycoord() > p2.ycoord()) {
      y_min=p2.ycoord();
      y_max=p1.ycoord();
    }
    else {
      y_min=p1.ycoord();
      y_max=p2.ycoord();
    }
    
    bool b=true;
    
    if (y_max-y_min > x_max-x_min) {
        if ((p.ycoord()<y_min)||(p.ycoord()>y_max)) b=false;
    }
    else {
      if ((p.xcoord()<x_min)||(p.xcoord()>x_max)) b=false;
    }
      

    if (b) {
      d = line(p1,p2).perpendicular(p).length();
      if (d < min) {
	min=d;
	min_it=it;
      }
    }

    it=it2;
    it2=P.succ(it);
    
  }
  while (it2 != nil);

  return min_it;
}



void gw_move_edge(GraphWin& gw, const point& p) 
{
  edge e = gw.get_found_edge();

  if (e == nil) return;
  
  // undo
  gw.changing_layout(e);

  window& W = gw.get_window();

  // find out whether p is near to a bend of the polygon of e

  list<point>& P = gw.get_poly(e);

  list_item it = gw.find_bend(p,e);

  bool b = gw.set_flush(false);

  if (it == nil) it = P.first(); 

  if ((gw.e_info[e].shape != circle_edge || P.length() == 2) 
        && p.distance(P[it]) > W.pix_to_real(10))
  { it=min_poly_dist(P,p);
    it=P.insert(p,it); 
    gw.set_edge_points(e,P);
   }


  // move bend P[it]

  gw.set_flush(b);

  unsigned long t=W.button_press_time();
  unsigned long t1;
  int val;
  int event;
  double x,y;

  node v = source(e);
  node w = target(e);

  list<edge> L;
  edge e1;
  for(e1=gw.adj_matrix_first(v,w);e1;e1 = gw.adj_matrix_succ[e1]) L.append(e1);
  for(e1=gw.adj_matrix_first(w,v);e1;e1 = gw.adj_matrix_succ[e1]) L.append(e1);

  gw.e_animation_start(L);
  gw.current_edge = e;

  W.grab_mouse();

  do {
    event = W.read_event(val,x,y,t1);
    if (event == motion_event) if (t1 - t < 50) continue;
    P[it] = gw.grid_point(x,y);
    gw.e_animation_step();
    t = t1;
  }
  while (event != button_release_event);

  W.ungrab_mouse();

  gw.current_edge = nil;
  gw.e_animation_end();
  gw.redraw(e);
}


void gw_move_edge_slider(GraphWin& gw, const point& p) 
{
  edge e = gw.found_edge;
  int i = gw.found_slider;
  if (i < 0) return; // just in case ...

  if (gw.start_edge_slider_handler[i])
    (*gw.start_edge_slider_handler[i])(gw,e,gw.e_info[e].slider_f[i]);

  window& W = gw.get_window();

  list<point>& P = gw.get_poly(e);

  point p1 = P.head();
  point p2 = P.tail();

  double  e_len = p1.distance(p2);

  double&  slid_f = gw.e_info[e].slider_f[i];

  line L(p1,p2.rotate90(p1));

  double dist = L.distance(p);
  double delta = slid_f - dist/e_len;

  unsigned long t=W.button_press_time();
  unsigned long t1;
  int val;
  int event;

  double x = p.xcoord();
  double y = p.ycoord();

  gw.e_animation_start(e);

  gw.current_edge = e;

  do { point q(x,y);
       if (orientation(L,q) < 0)
       { double f = delta + L.distance(q)/e_len;
         if (f <= 1)
         { slid_f = f;
           (*gw.edge_slider_handler[i])(gw,e,f);
           gw.e_animation_step();
          }
        }
       //if (gw.action_mask & A_DRAG)
       { event = W.read_event(val,x,y,t1);
         if (event == motion_event) if (t1 - t < 50) continue;
         t = t1;
        }
  } //while ((gw.action_mask & A_DRAG) && event != button_release_event);
    while (event != button_release_event);

  gw.e_animation_end();

  if (event == button_release_event) gw.current_edge = nil;

  if (gw.end_edge_slider_handler[i])
    (*gw.end_edge_slider_handler[i])(gw,e,slid_f);
}

void gw_slide_edge(GraphWin& gw, const point& p) 
{ gw_move_edge_slider(gw,p); }
 


//----------------------------------------------------------------------------

void gw_select_component(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil) return;
  graph& G=gw.get_graph();
  list<node> V;
  node_array<bool> n_reached(G,false);
  dfs(G,v,n_reached,V);

  // undo
  gw.changing_layout(V);

  bool select=false;
  forall(v,V) if (!gw.is_selected(v)) { select=true; break; }
  bool save_flush=gw.set_flush(false);
  forall(v,V) gw.set_select(v,select);  
  gw.redraw();
  gw.set_flush(save_flush);
}


//----------------------------------------------------------------------------


static void draw_background(window& W, double x0, double y0, 
                                       double x1, double y1,
                                       double dx, double dy)
{ double d0 = W.get_grid_dist();
  char* bpr = W.get_bg_pixrect();
  double gd = -20;
  y1 += W.pix_to_real(100);
  if (d0 == 0 && bpr == 0 && W.real_to_pix(gd) <= -8) W.set_grid_dist(gd);
  W.clear(x0,y0,x1,y1,dx,dy);
  W.set_grid_dist(d0);
}



void gw_move_component(GraphWin& gw, const point& p) 
{
  window& W = gw.get_window();

  node v = gw.get_found_node();
  edge e;

  if (v == nil) {
    edge e=gw.get_found_edge();
    if (e == nil) return;
    v=source(e);
  }

  if (!gw.check_move_component_handler(v)) return;

  graph& G = gw.get_graph();
  list<node> V;
  list<edge> E;
  node_array<bool> reached(G,false);
  dfs(G,v,reached,V);

  forall_edges(e,G) 
     if (reached[source(e)]) E.append(e);

  // undo
  gw.changing_layout(V,E);

  if (V.length() + E.length() > gw.get_max_move_items())
  { // move node only
    gw_move_nodes(gw,p,V);
    return;
   }

  point p1 = gw.grid_point(p.xcoord(),p.ycoord());

  double x = p1.xcoord();
  double y = p1.ycoord();

  char* buf;

  W.start_buffering();
  draw_background(W,W.xmin(),W.ymin(),W.xmax(),W.ymax(),0,0);
  gw.draw_graph(V);
  W.stop_buffering(buf);

  W.grab_mouse();

  W.start_buffering();

  //W.grab_mouse(); // XError

  int event;
  double dx=0, dy=0;

  unsigned long t = W.button_press_time();

  do { double x1,y1;
       int val;
       unsigned long t1;
       event = W.read_event(val,x1,y1,t1);
       if (event == motion_event) if (t1 - t < 50) continue;
       t  = t1;
       //dx = x1-x;
       //dy = y1-y;
       point q = gw.grid_point(x1,y1);
       dx = q.xcoord()-x;
       dy = q.ycoord()-y;
       W.set_pixrect(buf);
       forall(e,E) gw.draw_edge(e,dx,dy);
       forall(v,V) gw.draw_node(v,dx,dy);
       W.flush_buffer();
  } while (event != button_release_event);

  W.ungrab_mouse();
  W.stop_buffering();
  W.del_pixrect(buf);

  bool b = gw.set_flush(false);
  forall(v,V) gw.move_node(v,vector(dx,dy));
  forall(e,E) gw.move_edge(e,vector(dx,dy));
  gw.set_flush(b);
  gw.call_move_component_handler(v);
  gw.redraw();
}

//----------------------------------------------------------------------------

void gw_select_all_nodes(GraphWin& gw,   const point&) 
{ gw.select_all_nodes(); }

void gw_deselect_all_nodes(GraphWin& gw, const point&) 
{ gw.deselect_all_nodes(); }

//----------------------------------------------------------------------------

void gw_select_node_toggle(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil) return;
  gw.set_select(v,!gw.is_selected(v));
}

void gw_select_node(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil  || gw.is_selected(v)) return;
  gw.deselect_all_nodes();
  gw.set_select(v,true);
}

//----------------------------------------------------------------------------

void gw_select_edge_toggle(GraphWin& gw, const point&) {
  edge e=gw.get_found_edge();
  if (e == nil) return;
  gw.set_select(e,!gw.is_selected(e));
}

void gw_select_edge(GraphWin& gw, const point&) {
  edge e=gw.get_found_edge();
  if (e == nil  || gw.is_selected(e)) return;
  gw.deselect_all_edges();
  gw.set_select(e,true);
}

//----------------------------------------------------------------------------

void gw_new_node(GraphWin& gw, const point& p0) {
  point p = gw.grid_point(p0.xcoord(),p0.ycoord());
  if (gw.check_new_node_handler(p)) 
  { bool f = gw.set_flush(true);
    node v = gw.new_node(p);
    gw.set_flush(f);
    gw.call_new_node_handler(v);
   }
}

//----------------------------------------------------------------------------

segment GraphWin::edge_segment(node v, point pv, node w, point pw)
{
  if (v != nil && v == w) 
    return segment(pv,pv);

  double xv = pv.xcoord();
  double yv = pv.ycoord();

  double xw = pw.xcoord();
  double yw = pw.ycoord();

  line L(pv,pw);

  if (v) pv = first_hit(v,L,xw<xv,yw<yv);
  if (w) pw = first_hit(w,L,xv<xw,yv<yw);

  return segment(pv,pw);
}

/*
static void flush_edge_box(GraphWin& gw, segment s0, node w0, 
                                         segment s1, node w1)
{ window& W = gw.get_window();
  double d1 = W.pix_to_real(2);
  double d2 = W.pix_to_real(2);
  double x0 = s0.xcoord1();
  double y0 = s0.ycoord1();
  double x1 = s0.xcoord2();
  double y1 = s0.ycoord2();
  if (x0 > x1) { double tmp = x0; x0 = x1; x1 =tmp; }
  if (y0 > y1) { double tmp = y0; y0 = y1; y1 =tmp; }
  if (s1.xcoord2() < x0) x0 = s1.xcoord2(); 
  if (s1.xcoord2() > x1) x1 = s1.xcoord2(); 
  if (s1.ycoord2() < y0) y0 = s1.ycoord2(); 
  if (s1.ycoord2() > y1) y1 = s1.ycoord2(); 

  if (w0)
  { d1 += 2*gw.get_radius1(w0);
    d2 += 2*gw.get_radius2(w0);
   }
  if (w1)
  { d1 += 2*gw.get_radius1(w1);
    d2 += 2*gw.get_radius2(w1);
   }

  W.flush_buffer(x0-d1,y0-d2,x1+d1,y1+d2);
}
*/



void gw_new_edge(GraphWin& gw, const point&)
{
  node v = gw.get_found_node();

  if (v == nil) return;

  // show start_node hatched
  gw.start_node = v;
  gw.redraw();

  window& W = gw.get_window();

  W.disable_panel(false);
  W.set_cursor(-1);

  bool save_flush = gw.set_flush(true);


  gw.set_flush(false);

  node u = v;
  node w = nil;

  point p = gw.get_position(v);

  double x0 = p.xcoord() - gw.get_radius1(v);
  double y0 = p.ycoord() - gw.get_radius2(v);
  double x1 = p.xcoord() + gw.get_radius1(v);
  double y1 = p.ycoord() + gw.get_radius2(v);

  point q;

  list<point> L;

  int but;

//double d = W.pix_to_real(1);
  double d = W.pix_to_real(5);

  char* buf = W.get_window_pixrect();

  W.start_buffering();

  segment s0(p,p);
  segment s;

  while (w == nil)
  { 
    double x,y;
    int event;

    for(;;)
    { event = W.read_event(but,x,y);

      q = gw.grid_point(x,y);

      x = q.xcoord();
      y = q.ycoord();

      w = gw.find_node(q);


      if (x-d < x0) x0 = x-d;
      if (x+d > x1) x1 = x+d;
      if (y-d < y0) y0 = y-d;
      if (y+d > y1) y1 = y+d;


      gw.current_node = w;

      if (event == button_press_event) break;

      if (event != motion_event || (w == v && L.size() == 0)) continue;

      s = gw.edge_segment(u,p,w,q);
      //s = gw.edge_segment(u,p,nil,q);

      int pw = W.real_to_pix(x1-x0) + 1;
      int ph = W.real_to_pix(y1-y0) + 1;
      int px = W.xpix(x0);
      int py = W.ypix(y0)-ph+1;

      W.put_pixrect(x0,y0,buf,px,py,pw,ph);

      //W.set_pixrect(buf);

      if (w) 
      { W.set_cursor(XC_arrow);
        point pw = gw.get_position(w);
        gw.draw_node(w);
        double d1 = 2*gw.get_radius1(w);
        double d2 = 2*gw.get_radius2(w);
        double xx0 = pw.xcoord() - d1;
        double yy0 = pw.ycoord() - d2;
        double xx1 = pw.xcoord() + d1;
        double yy1 = pw.ycoord() + d2;
        if (xx0 < x0) x0 = xx0;
        if (yy0 < y0) y0 = yy0;
        if (xx1 > x1) x1 = xx1;
        if (yy1 > y1) y1 = yy1;
       }

      int lw = W.set_line_width(W.real_to_pix(gw.e_model.width));

      if (!L.empty())
      { point ps0 = L.head();
        point ps1;
        forall(ps1,L)
        { if (ps0 != ps1) W.draw_segment(ps0,ps1,gw.e_model.clr);
          ps0 = ps1;
         }
       }

      if (w && gw.get_directed())
      { point t = W.draw_arrow_head(s.target(),s.angle(),gw.e_model.clr);
        s = segment(s.source(),t);
       }

      W.draw_segment(s,gw.e_model.clr);

      W.set_line_width(lw);

      W.flush_buffer(x0,y0,x1,y1);
      s0 = s;
     }


    gw.current_node = nil; 

    if (L.empty()) L.append(s.source());
    L.append(s.target());

    if (but == MOUSE_BUTTON(3)) break;

    if (w == nil && but == MOUSE_BUTTON(1)) 
    { if (gw.auto_create_target && gw.check_new_node_handler(q)) 
      { w = gw.new_node(q);
        gw.call_new_node_handler(w);
       }
    }

    u = w;
    p = q;
  }

  W.set_pixrect(buf);
  W.del_pixrect(buf);

  //gw.draw_node(v);


  if (w) gw.draw_node(w);

  edge e_new = nil;

  if (but != MOUSE_BUTTON(3))
  { L.pop();
    L.Pop();
    if (gw.check_new_edge_handler(v,w) && (v != w || !L.empty())) 
    { edge adj_vw = gw.adj_matrix_first(v,w); 
      edge adj_wv = gw.adj_matrix_first(w,v); 
      e_new = gw.new_edge(v,w,L);
      gw.embed_edges(v,w);
      if (adj_vw || adj_wv)
        { W.clear();
          gw.draw_graph();
         }
      else
         gw.draw_edge(e_new);
     }
   }

  W.flush_buffer();
  W.stop_buffering();

  gw.set_flush(save_flush);

  // hide start node
  gw.start_node = nil;
  gw.redraw();

  if (e_new) gw.call_new_edge_handler(e_new);

  W.enable_panel();
}


//----------------------------------------------------------------------------

void gw_open_root_menu(GraphWin& gw, const point& p)
{ window& W = gw.get_window();

  int x = W.xpix(p.xcoord());
  int y = W.ypix(p.ycoord());

  list<string> L;
  L.append("File");
  L.append("Edit");
  L.append("Graph");
  L.append("Layout");
  L.append("Window");
  L.append("Settings");
/*
  L.append("?");
  L.append("done");
*/


  panel_item pvec[32];
  menu*      mvec[32];
  int i = 0;

{
  menu M;

  string s;
  forall(s,L)
  { panel_item it = gw.menu_p->get_item(s);
    if (it == 0) continue;

    menu* m = (menu*)gw.menu_p->get_window(it);
    panel_action_func f = gw.menu_p->get_action(it);
    int val = gw.menu_p->get_value(it);

    if (m)
     { mvec[i] = m;
       pvec[i] = it;
       M.button(s + "     ",val,*m);
       i++;
      }
    else
      if (f)
        M.button(s,val,f);
      else
        M.button(s,val);
   }

  M.set_graphwin(&gw);

  M.open(W,x,y);
}

  while (--i >= 0) 
    gw.menu_p->set_window(pvec[i],mvec[i]);
    
  gw.redraw();
}


void gw_open_node_menu(GraphWin& gw, const point& p)
{ window& W = gw.get_window();
//graph& G = gw.get_graph();

  //W.set_cursor(-1);

  node v = gw.get_found_node();

  assert(v != nil);

  gw.current_node = v;
  gw.draw_node(v);

  //int x = W.xpix(p.xcoord());
  //int y = W.ypix(p.ycoord());

  point q = gw.get_position(v);

/*
  int x = W.xpix(q.xcoord() - gw.get_radius1(v)) - 1;
  int y = W.ypix(q.ycoord() - gw.get_radius2(v)) + 2;
*/

  int x = W.xpix(q.xcoord()) + 2;
  int y = W.ypix(q.ycoord() - 1.1*gw.get_radius2(v)) + 4;

  int dy = W.height() - (y+80);
  if (dy < 0) y += dy;

  bool s =  gw.is_selected(v);

  menu  M;

  M.button("setup",1);
  M.button("label",2);
  M.button((s) ? "deselect" : "select", 3);
  M.button("delete", 4);

  if (!gw.node_action_menu.empty())
  { M.separator();
    int i = 100;
    two_tuple<string,gw_action> tt;
    forall(tt,gw.node_action_menu) M.button(tt.first(),i++);
   }

  int but = M.open(W,x,y);

  gw.current_node = nil;
  gw.found_node = v;
  gw.draw_node(v);

  if (but == NO_BUTTON) return;

  bool f = gw.set_flush(true);

  switch (but) {

   case 1: gw_setup_node(gw,p);
           break;
   case 2: gw_edit_node_label(gw,p);
           break;
   case 3: gw_select_node_toggle(gw,p);
           break;
   case 4: gw_del_node(gw,p);
           break;

  default: if (!gw.node_action_menu.empty() && but >= 100)
           { list_item it = gw.node_action_menu.get_item(but-100);
             two_tuple<string,gw_action> tt = gw.node_action_menu[it];
             gw_action func = tt.second();
             func(gw,p);
             break;
            }
  }

  gw.set_flush(f);
  if (gw.get_color(v) == invisible) gw.redraw(v);
}



void gw_open_edge_menu(GraphWin& gw, const point& p)
{ window& W = gw.get_window();

  //W.set_cursor(-1);



  edge e = gw.get_found_edge();

  list<point>& P = gw.get_poly(e);

  list_item it = gw.find_bend(p,e);

  gw.current_edge = e;
  bool s = gw.set_select(e,true);
  gw.draw_edge(e);

  menu  M;

  M.button("setup",1);
  M.button("label",2);
  M.button((s) ? "deselect" : "select", 3);

  if (it)
     M.button("rm bend",4);
  
  if ((P.length() == 3 && it == 0) || P.length() > 3) 
     M.button("rm bends",5);

  M.button("delete", 6);


  if (!gw.edge_action_menu.empty())
  { M.separator();
    int i = 100;
    two_tuple<string,gw_action> tt;
    forall(tt,gw.edge_action_menu) M.button(tt.first(),i++);
   }


  int x = W.xpix(p.xcoord()) + 4;
  int y = W.ypix(p.ycoord()) + 4;

  int dy = W.height() - (y+80);
  if (dy < 0) y += dy;

  int but = M.open(W,x,y);

  gw.current_edge = nil;
  gw.set_select(e,s);
  gw.draw_edge(e);

  if (but == NO_BUTTON) return;

  bool f = gw.set_flush(true);

  switch (but) {

   case 1: gw_setup_edge(gw,p);
           break;

   case 2: gw_edit_edge_label(gw,p);
           break;

   case 3: gw_select_edge_toggle(gw,p);
           break;

   case 4: 
           P.del_item(it);
           P.pop();
           P.Pop();
           gw.changing_layout(e);
           gw.set_bends(e,P);
           break;

   case 5: gw.changing_layout(e);
           gw.remove_bends(e);
           break;

   case 6: gw_del_edge(gw,p);
           break;

  default: if (!gw.edge_action_menu.empty() && but >= 100)
           { list_item it = gw.edge_action_menu.get_item(but-100);
             two_tuple<string,gw_action> tt = gw.edge_action_menu[it];
             gw_action func = tt.second();
             func(gw,p);
             break;
            }

  }

  gw.set_flush(f);
}



void gw_del_node(GraphWin&gw, node v) {
  if (gw.check_del_node_handler(v)) {
    bool f = gw.set_flush(true);
    gw.del_node(v);
    gw.set_flush(f);
    gw.call_del_node_handler();
  }
}


void gw_del_node(GraphWin& gw, const point&) 
{ node v = gw.get_found_node();
  if (v == nil) return;

  if (gw.is_selected(v) && gw.get_selected_nodes().size() > 1) 
  { panel P;
    P.text_item("\\blue Remove all selected nodes ?");
    P.button("remove");
    int cancel_b = P.button("cancel");
    if (P.open(gw.get_window()) != cancel_b)
    { bool b = gw.set_flush(false);
      list<node> L = gw.get_selected_nodes();
      forall(v,L) 
      { if (!gw.check_del_node_handler(v)) continue;
        gw.n_info[v].selected = false;
        gw.del_node(v);
        gw.call_del_node_handler();
       }
      gw.selected_nodes.clear();
      gw.redraw();
      gw.set_flush(b);
    }
  }
  else 
    gw_del_node(gw,v);
} 

//----------------------------------------------------------------------------

void gw_del_edge(GraphWin&gw, edge e) {
  if (gw.check_del_edge_handler(e)) {
    bool f = gw.set_flush(true);
    gw.del_edge(e);
    gw.set_flush(f);
    gw.call_del_edge_handler();
  }
}


void gw_del_edge(GraphWin& gw, const point& p) 
{

  edge e = gw.get_found_edge();

  if (e == nil) return;

/*
  list<point>& P = gw.get_poly(e);
  list_item it = gw.find_bend(p,e);

  if (it != nil)
  { //delete bend
    gw.e_animation_start(e);
    P.del_item(it);
    gw.e_animation_step();
    gw.e_animation_end();
    return;
   }
*/

  if (gw.is_selected(e) && gw.get_selected_edges().size() > 1) 
    { panel P;
      P.text_item("\\blue Remove all selected edges ?");
      P.button("remove");
      int cancel_b = P.button("cancel");
      if (P.open(gw.get_window()) != cancel_b)
      { bool b = gw.set_flush(false);
        list<edge> L = gw.get_selected_edges();
        forall(e,L) 
        { if (!gw.check_del_edge_handler(e)) continue;
          gw.e_info[e].selected = false;
          gw.del_edge(e);
          gw.call_del_edge_handler();
         }
        gw.selected_edges.clear();
        gw.redraw();
        gw.set_flush(b);
      }
    }
  else 
    gw_del_edge(gw,e);
} 

//----------------------------------------------------------------------------

void gw_del_component(GraphWin& gw, const point& p) {
  gw.deselect_all_nodes();
  gw_select_component(gw,p);
  gw_del_node(gw,p);
  gw.deselect_all_nodes();
}

//----------------------------------------------------------------------------

void gw_setup_node(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil) return;

  if (!gw.is_selected(v)) {
    gw.deselect_all_nodes();
    gw.select(v);
  }

  list<node> V = gw.get_selected_nodes();

  if (!gw.node_setup_active) 
  { // no active setup panel 
    if (!gw_setup(gw,V)) gw.deselect_all_nodes();
   }
}

//----------------------------------------------------------------------------

void gw_setup_edge(GraphWin& gw, const point&) {
  edge e=gw.get_found_edge();
  if (e == nil) return;


  if (!gw.is_selected(e)) {
    gw.deselect_all_edges();
    gw.select(e);
  }

  list<edge> E = gw.get_selected_edges();

  if (!gw.edge_setup_active) 
  { // no active setup panel 
    if (!gw_setup(gw,E)) gw.deselect_all_edges();
   }
}



//----------------------------------------------------------------------------


void gw_scroll_graph(GraphWin& gw, const point& p) 
{
  window& W = gw.get_window();
  W.scroll_window(p,scroll_redraw);
  gw.update_win_bounds();
//gw.redraw();
  gw.draw_message();
 }


//----------------------------------------------------------------------------

void gw_select_area(GraphWin& gw, const point& p) 
{ 
  graph&  G = gw.get_graph();

  double x0 = p.xcoord();
  double y0 = p.ycoord();
  double x1,y1;
  gw.read_mouse_rect(x0,y0,x1,y1);

  if (x0 > x1) leda_swap(x0,x1);
  if (y0 > y1) leda_swap(y0,y1);

  list<node> L;
  node v;
  forall_nodes(v,G)
  { point p = gw.get_position(v);
    double x = p.xcoord();
    double y = p.ycoord();
    if (x0 <= x && x <= x1 && y0 <= y && y <= y1) L.append(v);
   }

  bool sel=false;
  forall(v,L) if (!gw.is_selected(v)) { sel=true; break; } 
  forall(v,L) gw.set_select(v,sel,false);
  gw.redraw();

}

//----------------------------------------------------------------------------

void gw_split_edge(GraphWin& gw, const point& p) {
  edge e=gw.get_found_edge();
  if (e == nil) return;
  if (!gw.check_split_edge_handler(e)) return;
  window& W=gw.get_window();
  W.message("new node");
  color old_color=gw.set_color(e,blue);
  string s[]={ "split","leave","cancel" };
  int choice=W.read_vpanel("Split edge?",3,s);
  gw.set_color(e,old_color);
  W.del_messages();
  if (choice == 2) return;
  if (choice == 0) {
    node u=gw.new_node(p);
    node v=source(e);
    node w=target(e);
    gw.del_edge(e);
    gw.new_edge(v,u);
    gw.new_edge(u,w);
    gw.call_split_edge_handler(u);
  }
  else {
    if (gw.check_new_node_handler(p)) 
          gw.call_new_node_handler(gw.new_node(p));
  }
}

//----------------------------------------------------------------------------


void gw_del_selected_nodes(GraphWin& gw, const point&) {
  node v=gw.get_found_node();
  if (v == nil) return;
  if (gw.is_selected(v)) {
    list<node> L(gw.get_selected_nodes());
    bool b = gw.set_flush(false);
    forall(v,L) { 
      if (gw.check_del_node_handler(v)) {
        gw.del_node(v);
        gw.call_del_node_handler();
      }
    }
   gw.redraw();
   gw.set_flush(b);
  }
}
  


//----------------------------------------------------------------------------


gw_action GraphWin::set_action(long mask, gw_action f) 
{ gw_action old = action[mask & A_MASK];
  action[(mask | A_IMMEDIATE) & A_MASK] = 0;
  action[mask & A_MASK] = f;
  return old;
}


gw_action GraphWin::get_action(long mask) 
{ return action[mask & A_MASK]; }


void GraphWin::clear_actions() 
{ for(int i=0; i<A_STOP; i++) action[i] = NULL; 
}


void GraphWin::reset_actions() {

  clear_actions();

  // left button (create,move,scroll,setup,zoom)

  set_action( A_LEFT                               , gw_new_node);
//set_action( A_LEFT   |                    A_EDGE , gw_new_node);

  set_action( A_LEFT   |                    A_NODE , gw_new_edge);
  set_action( A_LEFT   |                    A_FRAME, gw_new_edge);

  set_action( A_LEFT   | A_CTRL           | A_EDGE , gw_zoom_edge);

/*
  set_action( A_LEFT   | A_SHIFT | A_DRAG | A_FRAME, gw_resize_node);
  set_action( A_LEFT   |           A_DRAG | A_FRAME, gw_move_node);
*/
  set_action( A_LEFT   |           A_DRAG | A_FRAME, gw_resize_node);

  set_action( A_LEFT   |           A_DRAG | A_NODE , gw_move_node);

  set_action( A_LEFT   |           A_DRAG | A_EDGE , gw_move_edge);
  set_action( A_LEFT   |           A_DRAG          , gw_scroll_graph);

  set_action( A_LEFT   |           A_DRAG | A_SLIDER, gw_move_edge_slider);



  set_action( A_LEFT   | A_SHIFT | A_DRAG | A_NODE , gw_move_component);
  set_action( A_LEFT   | A_DOUBLE| A_DRAG | A_NODE , gw_move_component);



  set_action( A_LEFT   | A_DOUBLE|          A_NODE , gw_setup_node);
  set_action( A_LEFT   | A_DOUBLE|          A_EDGE , gw_setup_edge);

  set_action( A_LEFT   | A_SHIFT |          A_NODE , gw_edit_node_label);
  set_action( A_LEFT   | A_SHIFT |          A_EDGE , gw_edit_edge_label);



  set_action( A_LEFT   | A_DOUBLE,                   gw_zoom_down_to_point);
  set_action( A_LEFT   | A_DOUBLE | A_SHIFT,         gw_zoom_up_to_point);



  // middle button (selections)

  set_action( A_MIDDLE |             A_IMMEDIATE ,     gw_zoom_area);

  set_action( A_MIDDLE |                    A_NODE , gw_select_node_toggle);
  set_action( A_MIDDLE |                    A_FRAME, gw_select_node_toggle);
  set_action( A_MIDDLE |                    A_EDGE , gw_select_edge_toggle);

  set_action( A_MIDDLE |           A_DRAG          , gw_select_area);
  set_action( A_MIDDLE |           A_DRAG | A_NODE , gw_select_area);
  set_action( A_MIDDLE |           A_DRAG | A_EDGE , gw_select_area);

  set_action( A_MIDDLE | A_SHIFT |          A_NODE , gw_select_component);

  set_action( A_MIDDLE | A_DOUBLE|          A_NODE , gw_select_component);




  // right button

/*
  set_action( A_RIGHT  |                    A_NODE , gw_del_node);
  set_action( A_RIGHT  |                    A_EDGE , gw_del_edge);
  set_action( A_RIGHT  | A_SHIFT |          A_NODE , gw_del_component);
  set_action( A_RIGHT  | A_DOUBLE|          A_NODE , gw_del_component);
*/


 // context menus

  set_action( A_RIGHT  |           A_IMMEDIATE          , gw_open_root_menu);
  set_action( A_RIGHT  |           A_IMMEDIATE | A_NODE , gw_open_node_menu);
  set_action( A_RIGHT  |           A_IMMEDIATE | A_FRAME, gw_open_node_menu);
  set_action( A_RIGHT  |           A_IMMEDIATE | A_EDGE , gw_open_edge_menu);


  set_action( A_SCROLL_UP | A_IMMEDIATE , gw_zoom_up_step);
  set_action( A_SCROLL_DN | A_IMMEDIATE , gw_zoom_down_step);
}


//----------------------------------------------------------------------------


void GraphWin::handle_button_event(int but, double x, double y)
{
  point p(x,y);

  // cout << "handle_button_event" << p << ": but = " << but << endl;

  long MASK=0;

  // alt key + left button --> middle button
  if (win_p->alt_key_down() && but == MOUSE_BUTTON(1)) but = MOUSE_BUTTON(2);

  switch (but) {
     case MOUSE_BUTTON(1) : MASK |= A_LEFT;   break;
     case MOUSE_BUTTON(2) : MASK |= A_MIDDLE; break;
     case MOUSE_BUTTON(3) : MASK |= A_RIGHT;  break;
     case MOUSE_BUTTON(4) : MASK |= A_SCROLL_UP;  break;
     case MOUSE_BUTTON(5) : MASK |= A_SCROLL_DN;  break;
  }


  if (MASK != A_SCROLL_UP && MASK != A_SCROLL_DN)
  {
    if (found_node == nil && found_edge == nil)
      if (!find_node(p)) find_edge(p);

    if (found_node == nil && found_edge == nil)
    { point q = grid_point(p.xcoord(),p.ycoord());
      if (q != p)
      { p = q;
        find_node(p);
       }
     }

    if (found_frame !=nil)
       MASK |= A_FRAME;
    else 
       if (found_node != nil) 
          MASK |= A_NODE;
       else 
          if (found_edge != nil) 
          { if (find_slider(p,found_edge) >= 0)
               MASK |= A_SLIDER;
            else
               MASK |= A_EDGE;
           }
   }


  if (win_p->shift_key_down()) MASK |= A_SHIFT;
  if (win_p->ctrl_key_down())  MASK |= A_CTRL;
//if (win_p->alt_key_down())   MASK |= A_ALT;

    
  gw_action func = action[MASK|A_IMMEDIATE];

  if (func) 
      MASK |= A_IMMEDIATE;
  else
      { bool dclick = false;
        bool drag = false;
    
        if ( but == MOUSE_BUTTON(1) || 
             but == MOUSE_BUTTON(2) || 
             but == MOUSE_BUTTON(3) )
          win_p->read_mouse(x,y,300,300,dclick,drag);

    
        if (drag)   MASK |= A_DRAG;
        if (dclick) MASK |= A_DOUBLE;
    
        func = action[MASK];
      }

  if (func)
  { action_mask = MASK;

    if (gw_undo == 0) 
         func(*this,p);
    else
       { gw_undo->start_complex_command();
	 func(*this,p);
         gw_undo->finish_complex_command();
 
 	 if (gw_undo->size() > 0) 
            menu_p->enable_button(undo_button); 
 	 else 
            menu_p->disable_button(undo_button);
 
 	 if (gw_undo->redo_size() > 0) 
            menu_p->enable_button(redo_button); 
 	 else 
            menu_p->disable_button(redo_button);
        }
   }

  update_status_line();
}

void GraphWin::edit_loop(int event, int but, double x, double y)
{ 
  window& W = get_window();

  assert(event != no_event);


  if (event == motion_event) find_node(point(x,y));

  if (found_node) 
  { 
/*
   if (found_frame && get_action(A_LEFT | A_DRAG | A_FRAME) != NULL)
     W.set_cursor(XC_tcross);
   else
*/
   W.set_cursor(XC_right_ptr);
  }
  else
    { edge e = find_edge(point(x,y));
      if (e) 
      { list_item it = find_bend(point(x,y),e);
        if (it)
          W.set_cursor(XC_tcross);
        else
          W.set_cursor(XC_hand2);
       }
      else
        //W.set_cursor(XC_hand1);
        W.set_cursor(-1);
     }

  if (event == button_press_event && but < 0) { 
    // handle only mouse buttons (not panel or menu buttons)
    handle_button_event(but,x,y);
  }

}


bool GraphWin::edit() 
{
  window& W = get_window();

  undo_clear();

  edit_result = -1;

  while (edit_result == -1)
  { double x,y;
    int but;
    int event = W.read_event(but,x,y);
    edit_loop(event,but,x,y);
   }

  W.set_cursor(-1);

  return edit_result != 0;
}


//----------------------------------------------------------------------------
  

void gw_zoom_edge(GraphWin& gw, const point& p) 
{ edge e = gw.get_found_edge();
  point q = gw.get_position(target(e));
  gw.zoom_to_point(p,q,1.0); 
}



void gw_edit_node_label(GraphWin& gw, const point&)
{ 
  window& W = gw.get_window();
  node v = gw.get_found_node();
  if (v == nil) return;

  gw.changing_layout(v);

  W.disable_panel();

  W.set_show_coord_handler(nil);
  W.set_status_string("\\bf ~Edit Node Label\\rm ~~~~\\blue esc\\black : quit\
                       ~~~~\\blue return\\black : newline");

  string& s = gw.n_info[v].label;
  gw_position& lpos = gw.n_info[v].label_pos;
  int& cursor = gw.n_info[v].label_curs;

  gw_label_type lt0 = gw.n_info[v].label_t;

  gw_label_type lt  = gw_label_type(gw.n_info[v].label_t | user_label);

  gw.n_info[v].label_t = lt;

  double rx0 = gw.n_info[v].r1;
  double ry0 = gw.n_info[v].r2;

  double tw = W.text_width(s);
  double th = W.text_height(s);
  point p = gw.compute_label_pos(v,tw,th);

  double x = p.xcoord();
  double y = p.ycoord();

  W.set_focus();

  gw.n_animation_start(v);

  int ppos = s.index("@");

  cursor = (ppos > -1) ? ppos : s.length();

  if (s == "")
  { s = " ";
    gw.n_animation_step();
    s = "";
   }

  gw.n_animation_step();

  int timeout = 500;
  int count = 0;

  int c;

  while ((c = W.string_edit(x,y,s,cursor,timeout)) >= 0) 
  { 
    if (c == KEY_F1) lpos = northwest_pos;
    else if (c == KEY_F2) lpos = north_pos;
    else if (c == KEY_F3) lpos = northeast_pos;
    else if (c == KEY_F4) lpos = west_pos;
    else if (c == KEY_F5) lpos = central_pos;
    else if (c == KEY_F6) lpos = east_pos;
    else if (c == KEY_F7) lpos = southwest_pos;
    else if (c == KEY_F8) lpos = south_pos;
    else if (c == KEY_F9) lpos = southeast_pos;

    int save_curs = cursor;
    if (timeout == 0) 
     { if (++count % 2) cursor = -2; // hide cursor  
       timeout = 500;
      }
    else 
     { bool adj_label = gw.get_adjust_to_label(); // true
       gw.set_label(v,s,adj_label);
       if (rx0 > gw.n_info[v].r1) gw.n_info[v].r1 = float(rx0);
       if (ry0 > gw.n_info[v].r2) gw.n_info[v].r2 = float(ry0);
       gw.n_info[v].label_t = lt;
       count = 0;
      }
    gw.n_animation_step();
    cursor = save_curs;
   }

  if (s == "") gw.n_info[v].label_t = lt0;
  gw.n_animation_step();

  gw.n_animation_end();
  cursor =  -1;
  gw.redraw();
  W.enable_panel();
  W.set_show_coord_handler(GraphWin::coord_handler);
  gw.update_status_line();
}



void gw_edit_edge_label(GraphWin& gw, const point&)
{ 
  window& W = gw.get_window();
  edge e = gw.get_found_edge();
  if (e == nil) return;

  gw.changing_layout(e);

  W.disable_panel();
  
  W.set_show_coord_handler(nil);
  W.set_status_string("\\bf ~Edit Edge Label\\rm ~~~~\\blue esc\\black : quit\
                       ~~~~\\blue return\\black : newline");


  string& s = gw.e_info[e].label;
  gw_position& lpos = gw.e_info[e].label_pos;
  int& cursor = gw.e_info[e].label_curs;


  double tw = W.text_width(s);
  double th = W.text_height(s);
  point p = gw.compute_label_pos(e,tw,th);

  double x = p.xcoord();
  double y = p.ycoord();

  W.set_focus();

  gw.e_animation_start(e);

  cursor = s.length();

  if (s == "")
  { s = " ";
    gw.e_animation_step();
    s = "";
   }

  gw.e_animation_step();

  int timeout = 500;
  int count = 0;

  int c;

  while ((c = W.string_edit(x,y,s,cursor,timeout)) >= 0) 
  { 
    if (c == KEY_F1 || c == KEY_UP) lpos = west_pos;
    else if (c == KEY_F2) lpos = central_pos;
    else if (c == KEY_F3 || c == KEY_DOWN) lpos = east_pos;

    int save_curs = cursor;
    if (timeout == 0) 
      { if (++count % 2) cursor = -2; // hide cursor  
        timeout = 500;
       }
    else 
      count = 0;
    gw.e_animation_step();
    cursor = save_curs;
   }

  gw.e_animation_end();
  cursor =  -1;
  gw.redraw();
  W.enable_panel();
  W.set_show_coord_handler(GraphWin::coord_handler);
  gw.update_status_line();
}

LEDA_END_NAMESPACE
