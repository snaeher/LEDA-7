/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  misc.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/system/file.h>
#include "graphwin_undo.h"
#include "local.h"


LEDA_BEGIN_NAMESPACE

void GraphWin::undo_clear() {
  //if (gw_undo) gw_undo->clear(); 
  if (gw_undo) 
  { gw_undo->set_graph(gr_p);
    update_status_line();
  }
}


string  GraphWin::xpm_path(string name)
{ if (name[0] == '/' || name[0] == '.' || is_file(name)) return name;
  return first_file_in_path(name,get_xpm_path(),':');
}


int GraphWin::open_panel(panel& P,bool close)
{ window& W = get_window();

  bool undo_disabled = (undo_button != -1 && !menu_p->is_enabled(undo_button)); 
  bool redo_disabled = (redo_button != -1 && !menu_p->is_enabled(redo_button)); 

  found_node = 0;
  current_node = 0;
  start_node = 0;

  W.disable_panel();

  P.display(W);
  int but = P.read();

  W.enable_panel();

  if (undo_disabled) menu_p->disable_button(undo_button);
  if (redo_disabled) menu_p->disable_button(redo_button);

  if (close) 
  {
    P.close();
/*
    // consume pending events (not necessary)
    int but;
    double x,y;
    window* wp = 0;
    int k = 0;
    unsigned long t;
    while ((k = get_event(wp,but,x,y)) != no_event) {
      cout << string("consume: %s  wp = %d", event_name[k],wp) << endl;
    }
*/
  }


  return but;
}


point GraphWin::grid_point(double x, double y)
{ double d = grid_dist;
  if (d > 0)
  { if (x >= 0)
       x = int(x/d + 0.5) * d;
    else
       x = int(x/d - 0.5) * d;

    if (y >= 0)
       y = int(y/d + 0.5) * d;
    else
       y = int(y/d - 0.5) * d;
   }
  return point(x,y);
}


void GraphWin::set_font(gw_font_type font_t, double size, string fn_user)
{ 
  // size: real coordinate (not pixel)

  if (window::display_type() == "x11") size = 0.75 * size;

  size /= window::screen_dpi_scaling();
  int sz = win_p->real_to_pix(size);

  string font_name;
  switch (font_t) {
    case roman_font:  font_name = string("T%d",sz);
                      break;
    case bold_font:   font_name = string("B%d",sz);
                      break;
    case italic_font: font_name = string("I%d",sz);
                      break;
    case fixed_font:  font_name = string("F%d",sz);
                      break;
    case user_font:   font_name = fn_user;
                      break;
  }
  win_p->set_font(font_name);
}



//----------------------------------------------------------------------------

node GraphWin::read_node() {
  if (gr_p->number_of_nodes() == 0) return nil;
  point p;
  node v = nil;
  message("\\bf Please select a node.");
  do {
    if (win_p->read_mouse(p) == MOUSE_BUTTON(3)) break;
    v=find_node(p);
  }
  while (v == nil);
  message("");
  return v;
}

//----------------------------------------------------------------------------

edge GraphWin::read_edge() {
  if (gr_p->number_of_edges() == 0) return nil;
  point p;
  edge e = nil;
  message("\\bf Please select an edge.");
  do {
    if (win_p->read_mouse(p) == MOUSE_BUTTON(3)) break;
    e=find_edge(p);
  }
  while (e == nil);
  message("");
  return e;
}

//----------------------------------------------------------------------------

void GraphWin::normalize_polygon(list<point>& P, double min_dist) 
{ // removes segments of length zero from P 

  // double min_dist = win_p->pix_to_real(2);

  if (P.empty()) return;

  point q = P.head();

  list_item it;
  forall_items(it,P)
  { if (it == P.first()) continue;
    double d =  P[it].distance(q);
    if (d < min_dist)
       P.del_item(it);
    else 
       q = P[it];
   }

}

//----------------------------------------------------------------------------

unsigned long GraphWin::read_mouse_rect(double& x0, double& y0, 
                                        double& x1, double& y1) 
{
  window& W = get_window();

  double xx0 = x0;
  double yy0 = y0;
  double xx1 = x0;
  double yy1 = y0;

  double x,y;
  unsigned long t1;
  int val,event;

  char* buf = W.get_window_pixrect();

  W.start_buffering();

  double d = W.pix_to_real(2);

  do { event = win_p->read_event(val,x,y,t1);
       if (x < xx0) xx0 = x;
       if (x > xx1) xx1 = x;
       if (y < yy0) yy0 = y;
       if (y > yy1) yy1 = y;
       W.set_pixrect(buf);
       win_p->draw_rectangle(x0,y0,x,y,black);
       W.flush_buffer(xx0-d,yy0-d,xx1+d,yy1+d);
  } while(event != button_release_event);

  W.stop_buffering();

  W.del_pixrect(buf);

  x1 = x;
  y1 = y;

  return t1;
}

//----------------------------------------------------------------------------

bool GraphWin::define_area(double& x0, double& y0, 
                           double& x1, double& y1, const char* msg) {
  win_p->message(msg);
  point p,q;
  int but;
  but=win_p->read_mouse(x0,y0);
  if (but == MOUSE_BUTTON(3)) {
    win_p->del_messages();
    return false;
  }
  read_mouse_rect(x0,y0,x1,y1);  
  win_p->del_messages();
  return true;
}


//----------------------------------------------------------------------------

list<node> GraphWin::get_nodes_in_area(double x0, double y0, 
                                       double x1, double y1) 
{
  list<node> L;
  if (x0 > x1) { double tmp=x0; x0=x1; x1=tmp; }
  if (y0 > y1) { double tmp=y0; y0=y1; y1=tmp; }
  node v;
  point p;
  forall_nodes(v,*gr_p) 
  { point  p = get_position(v);
    double xx0 = p.xcoord() - get_radius1(v);
    double yy0 = p.ycoord() - get_radius2(v);
    double xx1 = p.xcoord() + get_radius1(v);
    double yy1 = p.ycoord() + get_radius2(v);
    if (xx1 >= x0 && xx0 <= x1 && yy1 >= y0 && yy0 <= y1) L.append(v);
  }  
  return L;
}


list<edge> GraphWin::get_edges_in_area(double x0, double y0, 
                                       double x1, double y1) 
{
  list<edge> L;
  if (x0 > x1) { double tmp=x0; x0=x1; x1=tmp; }
  if (y0 > y1) { double tmp=y0; y0=y1; y1=tmp; }
  edge e;
  forall_edges(e,*gr_p) 
  {
    double xx0,yy0,xx1,yy1;
    compute_bounding_box(e,xx0,yy0,xx1,yy1);
    if (xx1 < x0 || xx0 > x1 || yy1 < y0 || yy0 > y1) continue;

    if (get_poly(e).length() == 2 && get_label(e) == "")
    { segment seg(get_poly(e).head(),get_poly(e).tail());
      int left = 0;
      int right = 0;
      if (orientation(seg,point(x0,y0)) < 0) left++; else right++;
      if (orientation(seg,point(x0,y1)) < 0) left++; else right++;
      if (left == 0 || right == 0)
      { if (orientation(seg,point(x1,y0)) < 0) left++; else right++;
        if (left == 0 || right == 0)
        { if (orientation(seg,point(x1,y1)) < 0) left++; else right++; }
       }
      if (left == 0 || right == 0) continue;
     }

    L.append(e);
  }  
  return L;
}


//----------------------------------------------------------------------------


void GraphWin::acknowledge(string s1, string s2) 
{ s1 = s1.replace_all("\\","/");
  s2 = s2.replace_all("\\","/");
  s1 = s1.replace_all(" ","~");
  s2 = s2.replace_all(" ","~");
  panel P("GraphWin");
  P.text_item("");
  P.text_item("\\bf\\blue " + s1);
  P.text_item("");
  if (s2 != "") P.text_item("\\tt\\black " + s2);
  P.fbutton("continue");
  P.open(get_window());
}


void GraphWin::acknowledge(string s) 
{ s = s.replace_all("\\","/");
  panel P("GraphWin");
  P.text_item("");
  P.text_item("\\bf " + s);
  P.text_item("");
  P.button("continue");
  P.open(get_window());
}



bool GraphWin::wait(float sec, const char* msg) {
  message(msg);
  disable_calls();
  //menu_p->disable_panel();
  //menu_p->enable_button(done_button);
  int edit = edit_result;
  edit_result = -1;
  int v;
  double x,y;
  unsigned long t;
  while (edit_result == -1) {
    if (menu_p->read_event(v,x,y,t,int(1000*sec)) == no_event) break;
  }
  message("");
  enable_calls();
  //menu_p->enable_panel();
  leda_swap(edit,edit_result);
  return edit != 0;
}    
    
bool GraphWin::wait(const char* msg) {
  message(msg);
  disable_calls();
  //menu_p->disable_panel();
  //menu_p->enable_button(done_button);
  int edit = edit_result;
  edit_result = -1;
  window *wp;
  double x,y;
  while(edit_result == -1) read_mouse(wp,x,y);
  message("");
  enable_calls();
  //menu_p->enable_panel();
  leda_swap(edit,edit_result);
  return edit != 0;
}    

bool GraphWin::wait() {
  int edit = edit_result;
  disable_calls();
  edit_result = -1;
  window *wp;
  double x,y;
  while(edit_result == -1) read_mouse(wp,x,y);
  enable_calls();
  leda_swap(edit,edit_result);
  return edit != 0;
}    
    
void GraphWin::message(const char* msg)
{ message_str = msg;
  redraw();
 }




// computing bounding boxes


void GraphWin::adjust_bounding_box(edge e, double& x0, double& y0, 
                                           double& x1, double& y1)
{ list<point> L = get_poly(e);

  //double d = get_thickness(e) + get_window().pix_to_real(2);

  //double d = 3*get_thickness(e);
  double d = 10*get_thickness(e);

  if (get_shape(e) == circle_edge && L.length() > 2) 
  { list_item it1 = L.first();
    list_item it2 = L.succ(it1);
    list_item it3 = L.last();
    point p1 = L[it1];
    point p2 = L[it2];
    point p3 = L[it3];
    if (orientation(p1,p2,p3) != 0)
    { circle C(p1,p2,p3);
      point  p = C.center();
      double r = C.radius();
      if (win_p->real_to_pix(r) > 1000) r = win_p->pix_to_real(1000);
      point q1(p.xcoord()-r,p.ycoord());
      point q2(p.xcoord()+r,p.ycoord());
      point q3(p.xcoord(),  p.ycoord()-r);
      point q4(p.xcoord(),  p.ycoord()+r);
      int orient = orientation(p1,p3,p2);
      if (orientation(p1,p3,q1)==orient && q1.xcoord()<x0) x0 = q1.xcoord()-d;
      if (orientation(p1,p3,q2)==orient && q2.xcoord()>x1) x1 = q2.xcoord()+d;
      if (orientation(p1,p3,q3)==orient && q3.ycoord()<y0) y0 = q3.ycoord()-d;
      if (orientation(p1,p3,q4)==orient && q4.ycoord()>y1) y1 = q4.ycoord()+d;
     }
   }

  L.push(get_position(source(e)));
  L.append(get_position(target(e)));

  string s = get_label(e);

  if (s != "")
  { double tw = get_window().text_width(s);
    double th = get_window().text_height(s);
    point pos = compute_label_pos(e,tw,th);
    L.append(pos.translate(   d,   -d));
    L.append(pos.translate(tw-d,   -d));
    L.append(pos.translate(   d,-th+d));
    L.append(pos.translate(tw-d,-th+d));
   }


  point p;
  forall(p,L)
  { double x = p.xcoord();
    double y = p.ycoord();
    if (x-d < x0) x0 = x-d;
    if (x+d > x1) x1 = x+d;
    if (y-d < y0) y0 = y-d;
    if (y+d > y1) y1 = y+d;
   }

}


void GraphWin::compute_bounding_box(edge e, double& x0, double& y0, 
                                            double& x1, double& y1)
{
  point p = get_position(source(e));
  x1 = x0 = p.xcoord();
  y1 = y0 = p.ycoord();
  adjust_bounding_box(e,x0,y0,x1,y1);
}


void GraphWin::compute_bounding_box(node v, double& x0, double& y0, 
                                            double& x1, double& y1)
{
  // computes bounding box for v and its adjacent nodes

  window& w = get_window();

  point q = get_position(v);

  double r1 = get_radius1(v) + 2*get_border_thickness(v);
  double r2 = get_radius2(v) + 2*get_border_thickness(v);

  string lab = get_label(v);
  int ppos = lab.index("@");
  if (ppos > -1) lab = lab(0,ppos);

  double tw = w.text_width(lab);
  double th = w.text_height(lab);

  r1 += leda_max(tw,r1);
  r2 += leda_max(th,r2);

  x0 = q.xcoord() - r1;
  x1 = q.xcoord() + r1;
  y0 = q.ycoord() - r2;
  y1 = q.ycoord() + r2;

  edge e;
  forall_adj_edges(e,v)
     adjust_bounding_box(e,x0,y0,x1,y1);
  forall_in_edges(e,v)
     adjust_bounding_box(e,x0,y0,x1,y1);
}




void GraphWin::adjust_bounding_box(node v, double& x0, double& y0, 
                                           double& x1, double& y1)
{
  window& W = get_window();

  string  s = get_label(v);
  point   p = get_position(v);
  double  x = p.xcoord();
  double  y = p.ycoord();
  double  d = W.pix_to_real(1);
  double r1 = get_radius1(v) + 2*get_border_thickness(v) + d;
  double r2 = get_radius2(v) + 2*get_border_thickness(v) + d;

  bool visible = true;

  if (get_pixmap(v) || get_color(v) != invisible 
                    || get_border_color(v) != invisible)
  { if (x-r1 < x0) x0 = x-r1;
    if (x+r1 > x1) x1 = x+r1;
    if (y-r2 < y0) y0 = y-r2;
    if (y+r2 > y1) y1 = y+r2;
   }
  else visible = false;

  if ((get_label_pos(v) != central_pos || !visible) && s != "")
  { int ppos = s.index("@");
    if (ppos > -1) s = s(0,ppos-1);
    double tw = W.text_width(s);
    double th = W.text_height(s);
    point pos = compute_label_pos(v,tw,th);
    double xt0 = pos.xcoord();
    double yt0 = pos.ycoord() - th;
    double xt1 = pos.xcoord() + tw;
    double yt1 = pos.ycoord();
    if (xt0 < x0) x0 = xt0;
    if (xt1 > x1) x1 = xt1;
    if (yt0 < y0) y0 = yt0;
    if (yt1 > y1) y1 = yt1;
   }

  edge e;
  forall_adj_edges(e,v)
      adjust_bounding_box(e,x0,y0,x1,y1); 
  forall_in_edges(e,v)
      adjust_bounding_box(e,x0,y0,x1,y1); 
}



void GraphWin::get_bounding_box(const list<node>& V, const list<edge>& E,
                                double& xmin, double& ymin, 
                                double& xmax, double& ymax)
{
  graph&  G = get_graph();

  xmin = xmax = ymin = ymax = 0;

  if (G.empty()) return;

  point p = get_position(G.first_node());

  xmin = p.xcoord();
  xmax = p.xcoord();
  ymin = p.ycoord();
  ymax = p.ycoord();

  set_font(node_label_font_type,node_label_font_size,node_label_user_font);

  node v;
  forall(v,V)  
    adjust_bounding_box(v,xmin,ymin,xmax,ymax);

  set_font(edge_label_font_type,edge_label_font_size,edge_label_user_font);

  edge e;
  forall(e,E)  
    adjust_bounding_box(e,xmin,ymin,xmax,ymax);
}


void GraphWin::get_extreme_coords(double& xmin, double& ymin, 
                                  double& xmax, double& ymax)
{
  graph&  G = get_graph();
  node    v = G.first_node();

  embed_edges();

  xmin = xmax = ymin = ymax = 0;

  if (v == nil) return;

  point p = get_position(v);

  xmin = p.xcoord();
  xmax = p.xcoord();
  ymin = p.ycoord();
  ymax = p.ycoord();

  forall_nodes(v,G)
  { point  p  = get_position(v);
    double x  = p.xcoord();
    double y  = p.ycoord();
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
   }


  edge e;
  forall_edges(e,G) 
  { list<point>& P = get_poly(e);

    if (get_shape(e) == circle_edge && P.length() > 2) 
    { list_item it1 = P.first();
      list_item it2 = P.succ(it1);
      list_item it3 = P.last();
      point p1 = P[it1];
      point p2 = P[it2];
      point p3 = P[it3];
      if (orientation(p1,p2,p3) != 0)
      { circle C(p1,p2,p3);
        point  p = C.center();
        double r = C.radius();
        if (win_p->real_to_pix(r) > 1000) r = win_p->pix_to_real(1000);
        point q1(p.xcoord()-r,p.ycoord());
        point q2(p.xcoord()+r,p.ycoord());
        point q3(p.xcoord(),  p.ycoord()-r);
        point q4(p.xcoord(),  p.ycoord()+r);
        int orient = orientation(p1,p3,p2);
        if (orientation(p1,p3,q1)==orient && q1.xcoord()<xmin) 
           xmin = q1.xcoord();
        if (orientation(p1,p3,q2)==orient && q2.xcoord()>xmax) 
           xmax = q2.xcoord();
        if (orientation(p1,p3,q3)==orient && q3.ycoord()<ymin) 
           ymin = q3.ycoord();
        if (orientation(p1,p3,q4)==orient && q4.ycoord()>ymax) 
           ymax = q4.ycoord();
       }
     }

    list_item it;
    forall_items(it,P) 
    { if (it == P.first() || it == P.last()) continue;
      point p = P[it];
      double x = p.xcoord();
      double y = p.ycoord();
      if (x < xmin) xmin = x;
      if (x > xmax) xmax = x;
      if (y < ymin) ymin = y; 
      if (y > ymax) ymax = y;
     }
   }
}


void GraphWin::get_extreme_coords(double& xmin, double& ymin, 
                                  double& xmax, double& ymax, 
                                  double& r1, double& r2)
{ embed_edges();
  double x0,y0,x1,y1;
  get_bounding_box(x0,y0,x1,y1);
  get_extreme_coords(xmin,ymin,xmax,ymax);
  r1 = leda_max(xmin-x0,x1-xmax);
  r2 = leda_max(ymin-y0,y1-ymax);
 }
  
  


void GraphWin::get_bounding_box(double& xmin, double& ymin, 
                                double& xmax, double& ymax)
{ graph& G = get_graph();
  get_bounding_box(G.all_nodes(),G.all_edges(),xmin,ymin,xmax,ymax);
  shape x;
  forall(x,shape_list)
  { rectangle bb;

    if (x.poly.empty())
      bb = BoundingBox(x.circ);
    else
      bb = BoundingBox(x.poly);

    if (bb.xmin() < xmin) xmin = bb.xmin();
    if (bb.ymin() < ymin) ymin = bb.ymin();
    if (bb.xmax() > xmax) xmax = bb.xmax();
    if (bb.ymax() > ymax) ymax = bb.ymax();
   }

 }


void GraphWin::flush_node(node v)
{ window& W = get_window();
  point  p  = get_position(v);
  double r1 = get_radius1(v);
  double r2 = get_radius2(v);
  double d  = get_border_thickness(v) + W.pix_to_real(3);
  r1 += d;
  r2 += d;
  double x0 = p.xcoord() - r1;
  double y0 = p.ycoord() - r2;
  double x1 = p.xcoord() + r1;
  double y1 = p.ycoord() + r2;

  string  s = get_label(v);

  if (get_label_pos(v) != central_pos && s != "")
  { int ppos = s.index("@");
    if (ppos > -1) s = s(0,ppos-1);
    if (s != "") 
    { double tw = W.text_width(s);
      double th = W.text_height(s);
      point pos = compute_label_pos(v,tw,th);
      double xt0 = pos.xcoord();
      double yt0 = pos.ycoord() - th;
      double xt1 = pos.xcoord() + tw;
      double yt1 = pos.ycoord();
      if (xt0 < x0) x0 = xt0;
      if (xt1 > x1) x1 = xt1;
      if (yt0 < y0) y0 = yt0;
      if (yt1 > y1) y1 = yt1;
     }
   }
  W.flush_buffer(x0,y0,x1,y1);
}

void GraphWin::flush_edge(edge e)
{ double x0,y0,x1,y1;
  compute_bounding_box(e,x0,y0,x1,y1);
  win_p->flush_buffer(x0,y0,x1,y1);
}





static bool is_north[9];
static bool is_south[9];
static bool is_west[9];
static bool is_east[9];


void GraphWin::init_static() 
{
  is_north[central_pos  ]=false; is_south[central_pos  ]=false;
  is_north[northwest_pos]=true;  is_south[northwest_pos]=false;
  is_north[north_pos    ]=true;  is_south[north_pos    ]=false;
  is_north[northeast_pos]=true;  is_south[northeast_pos]=false;
  is_north[east_pos     ]=false; is_south[east_pos     ]=false;
  is_north[southeast_pos]=false; is_south[southeast_pos]=true;
  is_north[south_pos    ]=false; is_south[south_pos    ]=true;
  is_north[southwest_pos]=false; is_south[southwest_pos]=true;
  is_north[west_pos     ]=false; is_south[west_pos     ]=false;

  is_west[central_pos  ]=false; is_east[central_pos  ]=false;
  is_west[northwest_pos]=true;  is_east[northwest_pos]=false;
  is_west[north_pos    ]=false; is_east[north_pos    ]=false;
  is_west[northeast_pos]=false; is_east[northeast_pos]=true;
  is_west[east_pos     ]=false; is_east[east_pos     ]=true;
  is_west[southeast_pos]=false; is_east[southeast_pos]=true;
  is_west[south_pos    ]=false; is_east[south_pos    ]=false;
  is_west[southwest_pos]=true;  is_east[southwest_pos]=false;
  is_west[west_pos     ]=true;  is_east[west_pos     ]=false;
}



point GraphWin::compute_label_pos(edge e, double tw, double th)
{
  gw_position label_pos = get_label_pos(e);
  string label = get_label(e);
  double e_thick = get_thickness(e);
  double d = win_p->pix_to_real(3);

  point q,r;

  const list<point>& P = get_poly(e);
  list_item it0=P.first();
  list_item it1=P.succ(it0);
  list_item it2=P.last();

  point a = P[it0];
  point b = P[it1];
  point c = P[it2];

  gw_edge_shape shape = get_shape(e);

  if (P.length() < 3) shape = poly_edge;

  if (shape == circle_edge && orientation(a,b,c) != 0)
  { circle C(a,b,c);
    point cen = C.center();
    if (C.orientation() > 0)
    { a = cen.rotate(b,LEDA_PI/2);
      b = cen.rotate(b,-LEDA_PI/2);
     }
    else
    { a = cen.rotate(b,-LEDA_PI/2);
      b = cen.rotate(b,LEDA_PI/2);
     }
  }

  double x0 = a.xcoord();
  double y0 = a.ycoord();
  double x1 = b.xcoord();
  double y1 = b.ycoord();

  double x = (x0+x1)/2;
  double y = (y0+y1)/2;

  double xoff = tw/2;
  double yoff = th/2;

  if (label_pos == central_pos) return point(x-xoff,y+yoff);

  tw += d;
  th += (e_thick + d);

  bool rturn = is_east[label_pos]; 
  double dx = x1 - x0;
  double dy = y1 - y0;
  double rx=tw*0.55;
  double ry=th*0.7;
  double rxrx=rx*rx;
  double ryry=ry*ry;

  if (fabs(dy) < fabs(dx)) // slope < 1
    { double m = dy/dx;
      double t=sqrt(m*m*rxrx+ryry);
      if (rturn)
         { if (dx < 0)  t = -t; }
      else
         { if (dx > 0)  t = -t; }
       x += m*rxrx/t; 
      y -= ryry/t;
     }
  else
    { double m = dx/dy;
      double t=sqrt(rxrx+m*m*ryry);
      if (rturn)
        { if (dy < 0)  t = -t; }
      else
        { if (dy > 0)  t = -t; }
      x += rxrx/t; 
      y -= m*ryry/t;
     }

  return point(x-xoff,y+yoff);
}



point GraphWin::compute_label_pos(node v, double tw, double th)
{ 
  gw_position   label_pos = get_label_pos(v);

  point p = get_position(v);

  double pix = win_p->pix_to_real(1);

  if (label_pos == central_pos) {
    double dx = tw/2 + pix;
    double dy = th/2;
    return p.translate(-dx,dy);
   }

  double d  = 0;
  double x  = p.xcoord();
  double y  = p.ycoord();
  double r1 = get_radius1(v);
  double r2 = get_radius2(v);


/*
  gw_node_shape shape = get_shape(v);

  if (shape == square_node || shape == rectangle_node 
      || label_pos == north_pos || label_pos == south_pos 
      || label_pos == west_pos  || label_pos == east_pos )
     d = 2*pix;
  else
     d = -3*pix;
*/

  d = 2*pix;


  if (is_east[label_pos]) 
     x += r1+d;
  else
    if (is_west[label_pos]) 
       x -= r1+tw+d;
    else 
       x -= tw/2;

  if (is_north[label_pos]) 
     y += r2+th+d;
  else
    if (is_south[label_pos]) 
       y -= r2+d;
    else 
       y += th/2;
    
  return point(x,y);
}


int GraphWin::text_color(int col)
{ 
  return color(col).text_color();

/*
  if (col == black ||
      col == red   ||
      col == blue  ||
      col == violet||
      col == brown ||
      col == pink  ||
      col == green2||
      col == blue2 ||
      col == grey3 )
     return white;
  else
     //return black;
     //return color(64,64,64);
     return color(48,48,48);
*/
}


point GraphWin::snap_point(const point& pos)
{ double d = grid_dist;
  double x = int((pos.xcoord()+0.5*d)/d) * d;
  double y = int((pos.ycoord()+0.5*d)/d) * d;
  return point(x,y);
}


void GraphWin::snap_to_grid()
{
  graph& G = get_graph();

  node_array<point> pos(G);
  edge_array<list<point> > bends(G);

  node v;
  forall_nodes(v,G) pos[v] = snap_point(get_position(v));

  edge e;
  forall_edges(e,G) 
  { list<point> L = get_bends(e);
    point q;
    forall(q,L) bends[e].append(snap_point(q));
   }

  set_layout(pos,bends,true);
}

LEDA_END_NAMESPACE
