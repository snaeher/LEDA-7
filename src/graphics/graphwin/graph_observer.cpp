/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  graph_observer.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/graph_observer.h>
#else
#include "graph_observer.h"
#endif


LEDA_BEGIN_NAMESPACE

graph_observer::graph_observer() : _g(nil) {}

graph_observer::graph_observer(graph& g) : _g(nil) { set_graph(&g); }

graph_observer::~graph_observer() { set_graph(nil); }


void graph_observer::set_graph(graph* g) 
{
  if (_g) detach(_events,21);

  _g = g;

  if (!_g) return;

  _events[0] =
    attach(g->new_node_pre_event,     *this, &graph_observer::on_new_node1);
  _events[1] =
    attach(g->new_node_post_event,    *this, &graph_observer::on_new_node2);
  _events[2] =
    attach(g->new_edge_pre_event,     *this, &graph_observer::on_new_edge1);
  _events[3] =
    attach(g->new_edge_post_event,    *this, &graph_observer::on_new_edge2);
  _events[4] =
    attach(g->del_node_pre_event,     *this, &graph_observer::on_del_node1);
  _events[5] =
    attach(g->del_node_post_event,    *this, &graph_observer::on_del_node2);
  _events[6] =
    attach(g->del_edge_pre_event,     *this, &graph_observer::on_del_edge1);
  _events[7] =
    attach(g->del_edge_post_event,    *this, &graph_observer::on_del_edge2);
  _events[8] =
    attach(g->move_edge_pre_event,    *this, &graph_observer::on_move_edge1);
  _events[9] =
    attach(g->move_edge_post_event,   *this, &graph_observer::on_move_edge2);
  _events[10] =
    attach(g->hide_edge_pre_event,    *this, &graph_observer::on_hide_edge1);
  _events[11] =
    attach(g->hide_edge_post_event,   *this, &graph_observer::on_hide_edge2);
  _events[12] =
    attach(g->restore_edge_pre_event, *this, &graph_observer::on_restore_edge1);
  _events[13] =
    attach(g->restore_edge_post_event,*this, &graph_observer::on_restore_edge2);
  _events[14] =
    attach(g->clear_pre_event,        *this, &graph_observer::on_clear1);
  _events[15] = 
    attach(g->clear_post_event,       *this, &graph_observer::on_clear2);
  _events[16] =
    attach(g->destroy_event,          *this, &graph_observer::on_destroy);
  _events[17] = 
    attach( g->join_from_pre_event,   *this, &graph_observer::on_join_from1);
  _events[18] = 
    attach( g->join_from_post_event,  *this, &graph_observer::on_join_from2);
  _events[19] = 
    attach( g->join_to_pre_event,     *this, &graph_observer::on_join_to1);
  _events[20] = 
    attach( g->join_to_post_event,    *this, &graph_observer::on_join_to2);
}


void graph_observer::on_destroy(graph&) { set_graph(nil); }


LEDA_END_NAMESPACE
