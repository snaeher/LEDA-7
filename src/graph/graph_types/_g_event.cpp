/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_event.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

EVENT1<graph&> graph::create_event;


void graph::enable_events()   { handlers_enabled = true;  }

void graph::disable_events()  { handlers_enabled = false; }


void graph::pre_new_node_handler() {
  new_node_pre_event(*this);
}
void graph::post_new_node_handler(node v) {
  new_node_post_event(*this,v);
}

void graph::start_del_node_handler(node v) {
  del_node_start_event(*this,v);
}

void graph::pre_del_node_handler(node v) {
  del_node_pre_event(*this,v);
}
void graph::post_del_node_handler() {
  del_node_post_event(*this);  
}

void graph::pre_new_edge_handler(node v, node w) {
  new_edge_pre_event(*this,v,w);
}
void graph::post_new_edge_handler(edge e) {
  new_edge_post_event(*this,e);
}

void graph::pre_del_edge_handler(edge e) {
  del_edge_pre_event(*this,e);
}
void graph::post_del_edge_handler(node v, node w) {
  del_edge_post_event(*this,v,w);
}

void graph::pre_move_edge_handler(edge e, node v, node w) {
  move_edge_pre_event(*this,e,v,w);
}
void graph::post_move_edge_handler(edge e, node v, node w) {
  move_edge_post_event(*this,e,v,w);
}

void graph::pre_hide_edge_handler(edge e) {
  hide_edge_pre_event(*this,e);
}
void graph::post_hide_edge_handler(edge e) {
  hide_edge_post_event(*this,e);
}

void graph::pre_restore_edge_handler(edge e) {
  restore_edge_pre_event(*this,e);
}
void graph::post_restore_edge_handler(edge e) {
  restore_edge_post_event(*this,e);
}

void graph::pre_clear_handler() {
  clear_pre_event(*this);
}
void graph::post_clear_handler() {
  clear_post_event(*this);
}

void graph::pre_join_from_graph_handler(graph& g) {
  join_from_pre_event(*this,g);
}
void graph::post_join_from_graph_handler(graph& g, const list<node>& N) {
  join_from_post_event(*this,g,N);
}

void graph::pre_join_to_graph_handler(graph& g) {
  join_to_pre_event(*this,g);
}
void graph::post_join_to_graph_handler(graph& g, const list<node>& N) {
  join_to_post_event(*this,g,N);
}


void graph::pre_hide_node_handler(node) {}
void graph::post_hide_node_handler(node) {}
void graph::pre_restore_node_handler(node) {}
void graph::post_restore_node_handler(node) {}

LEDA_END_NAMESPACE
