#ifndef GRAPH_OBSERVER_H
#define GRAPH_OBSERVER_H

#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

class graph_observer {

  event_item _events[21];
  graph *_g;

  protected :

  virtual void on_new_node1(graph&) {}
  virtual void on_new_node2(graph&,node) {}

  virtual void on_new_edge1(graph&,node,node) {}
  virtual void on_new_edge2(graph&,edge) {}

  virtual void on_del_node1(graph&,node) {}
  virtual void on_del_node2(graph&) {}

  virtual void on_del_edge1(graph&,edge) {}
  virtual void on_del_edge2(graph&,node,node) {}

  virtual void on_move_edge1(graph&,edge,node,node) {}
  virtual void on_move_edge2(graph&,edge,node,node) {}

  virtual void on_hide_edge1(graph&,edge) {}
  virtual void on_hide_edge2(graph&,edge) {}

  virtual void on_restore_edge1(graph&,edge) {}
  virtual void on_restore_edge2(graph&,edge) {}

  virtual void on_clear1(graph&) {}
  virtual void on_clear2(graph&) {}

  virtual void on_join_from1(graph&,graph&) {}
  virtual void on_join_from2(graph&,graph&,const list<node>&) {}

  virtual void on_join_to1(graph&,graph&) {}
  virtual void on_join_to2(graph&,graph&,const list<node>&) {}

  virtual void on_destroy(graph&);

  public :

  graph_observer();
  graph_observer(graph&);
  virtual ~graph_observer();

  virtual graph* get_graph() const { return _g; }
  virtual void set_graph(graph*);

};

LEDA_END_NAMESPACE

#endif





