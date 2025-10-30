/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_observ.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/gw_observer.h>

LEDA_BEGIN_NAMESPACE

/* static void write_msg(string s) { cout << s << endl;} */

static void write_msg(GraphWin* gw, string s) 
{ if (gw->get_window().is_open())
  { if (s == "")
      gw->get_window().reset_frame_label(); 
    else
      gw->get_window().set_tmp_label(s); 
   }
}

void gw_observer::update_layout()
{ graph& G = get_graph();
  if (!online) return;
  if (draw_graph)
  { G.disable_events();
    draw_graph(*this);
    G.enable_events();
   }
  else 
    redraw();
}


void gw_observer::update_layout(GraphWin& gw)
{ graph& G = get_graph();

  update_graph();

  bool fl = set_flush(false);
  node v;
  forall_nodes(v,G)
    set_position(v,gw.get_position(v));
  set_flush(fl);

  if (draw_graph)
  { G.disable_events();
    draw_graph(*this);
    G.enable_events();
   }
  else 
    redraw();
}


void gw_observer::new_node1(graph&)     
{ write_msg(this,"new node"); }

void gw_observer::new_node2(graph&,  node v)    
{ write_msg(this,"new node");
  vector vec = get_vector(v);
  init_node(v); 
  if (vec.dim() >= 2) set_position(v,point(vec[0],vec[1]));
  update_layout();
  write_msg(this,"");
}

void gw_observer::del_node1(graph&, node v)    
{ write_msg(this,"delete node");
  del_node = v; 
}

void gw_observer::del_node2(graph&)            
{ update_layout();
  del_node = 0;  
  write_msg(this,"");
}

void gw_observer::new_edge1(graph&, node,node)
{ write_msg(this,"new edge"); }

void gw_observer::new_edge2(graph&, edge e)     
{ init_edge(e);
  node v = source(e);
  node w = target(e);
  embed_edges(v,w);
  edges_embedded=true;
  //redraw_edge(e);
  update_layout();
  write_msg(this,"");
}

void gw_observer::del_edge1(graph&, edge) 
{ write_msg(this,"delete edge"); }

void gw_observer::del_edge2(graph&, node, node) 
{ if (!del_node) update_layout();
  write_msg(this,"");
}

void gw_observer::move_edge1(graph&, edge, node, node)    
{ write_msg(this,"move edge"); }

void gw_observer::move_edge2(graph& g, edge e, node v, node w)    
{ 
  bool fl = set_flush(false);

  g.disable_events();

  node a = new_node(get_position(v));
  set_color(a,invisible);
  set_border_color(a,invisible);
  set_label_type(a,no_label);

  node b = new_node(get_position(w));
  set_color(b,invisible);
  set_border_color(b,invisible);
  set_label_type(b,no_label);

  edge x = new_edge(a,b);
  set_color(x,red);

  color ec = set_color(e,invisible);

  node_array<point> pos(g);
  node u;
  forall_nodes(u,g) pos[u] = get_position(u);
  pos[a] = get_position(source(e));
  pos[b] = get_position(target(e));

  set_flush(fl);
  set_layout(pos);

  g.del_node(a);
  g.del_node(b);
  g.enable_events();
  update_graph();

  set_color(e,ec);

  update_layout();
  write_msg(this,"");
}

void gw_observer::clr_graph1(graph&)  
{ write_msg(this,"clear graph"); }

void gw_observer::clr_graph2(graph&)  
{ init_graph(); 
  update_layout(); 
  write_msg(this,"");
}



void gw_observer::layout_changed(GraphWin& gw, const list<node>& V, 
                                               const list<edge>& /*E*/) 
{ write_msg(this,"layout changing");
  graph& G = get_graph();
  update_graph();
  node_array<point> pos(G);
  node v;
  forall_nodes(v,G) pos[v] = get_position(v);
  forall(v,V) pos[v] = gw.get_position(v);
  set_layout(pos);
  redraw();
  write_msg(this,"");
 }
    

void gw_observer::init_events(graph& G) 
{ 
  e_vec = new event_item[13];
  e_num = 13;

  e_vec[0] = attach(G.new_node_pre_event, *this, &gw_observer::new_node1);
  e_vec[1] = attach(G.new_node_post_event,*this, &gw_observer::new_node2);

  e_vec[2] = attach(G.new_edge_pre_event, *this, &gw_observer::new_edge1);
  e_vec[3] = attach(G.new_edge_post_event,*this, &gw_observer::new_edge2);

  //e_vec[4] = attach(G.del_node_pre_event, *this, &gw_observer::del_node1);
  e_vec[4] = attach(G.del_node_start_event, *this, &gw_observer::del_node1);
  e_vec[5] = attach(G.del_node_post_event,*this, &gw_observer::del_node2);

  e_vec[6] = attach(G.del_edge_pre_event, *this, &gw_observer::del_edge1);
  e_vec[7] = attach(G.del_edge_post_event,*this, &gw_observer::del_edge2);
 
  e_vec[8] = attach(G.move_edge_pre_event, *this, &gw_observer::move_edge1);
  e_vec[9] = attach(G.move_edge_post_event,*this, &gw_observer::move_edge2);
 
  e_vec[10] = attach(G.clear_pre_event,  *this, &gw_observer::clr_graph1);
  e_vec[11] = attach(G.clear_post_event, *this, &gw_observer::clr_graph2);

  e_vec[12] = attach(GraphWin::changing_layout_event, *this, 
                                                 &gw_observer::layout_changed);
}



gw_observer::gw_observer(graph& G, int w, int h, const char* s) 
                                   : GraphWin(G,w,h,s), node_pos(0), online(true)
{ draw_graph = 0;
  del_node = 0;
  set_show_status(false);
  set_flush(false);
  node v;
  forall_nodes(v,G)
  { vector vec = get_vector(v);
    if (vec.dim() >= 2) set_position(v,point(vec[0],vec[1]));
   }
  set_flush(true);
  update_layout();
  init_events(G);
  GraphWin::set_default_menu(M_FILE|M_WINDOW|M_SETTINGS|M_DONE|M_READONLY);
}


gw_observer::gw_observer(graph& G, const node_position_base& pos, int w, int h, 
                                                                  const char* s)
                          : GraphWin(G,w,h,s), node_pos(&pos), online(true)
{ draw_graph = 0;
  del_node = 0;
  set_show_status(false);
  set_flush(false);
  node v;
  forall_nodes(v,G)
  { vector vec = get_vector(v);
    if (vec.dim() >= 2) set_position(v,point(vec[0],vec[1]));
   }
  set_flush(true);
  update_layout();
  init_events(G);
  GraphWin::set_default_menu(M_FILE|M_WINDOW|M_SETTINGS|M_DONE|M_READONLY);
}




gw_observer::gw_observer(GraphWin& gw, int w, int h, const char* s)
                         : GraphWin(gw.get_graph(),w,h,s), node_pos(0), online(true)
{ 
  graph& G = gw.get_graph();
  draw_graph = 0;
  del_node = 0;
  set_show_status(false);
  set_flush(false);
  node v;
  forall_nodes(v,G)
  { vector vec = get_vector(v);
    if (vec.dim() >= 2) set_position(v,point(vec[0],vec[1]));
   }
  set_flush(true);
  update_layout();
  init_events(G);
  GraphWin::set_default_menu(M_FILE|M_WINDOW|M_SETTINGS|M_DONE|M_READONLY);
}

LEDA_END_NAMESPACE

