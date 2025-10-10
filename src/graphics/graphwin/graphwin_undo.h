#ifndef LEDA_GRAPHWIN_UNDO_H
#define LEDA_GRAPHWIN_UNDO_H

#include <LEDA/graphics/graphwin.h>

#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/graph_undo.h>
#else
#include "graph_undo.h"
#endif

LEDA_BEGIN_NAMESPACE

class graphwin_undo : public graph_undo_redo_stack {

  friend class GraphWin;

  GraphWin   *_gw;

//   event_item events[1];

//   void on_recreate_node(graph& g, node_undo* u, int i);
//   void on_recreate_edge(graph& g, edge_undo* u, int i);
  void recreate_node(node_undo* u, int i);
  void recreate_edge(edge_undo* u, int i);

  void on_del_node1(graph&, node);
  void on_del_edge1(graph&, edge);
  void on_join_to1(graph&, graph&);
  void on_clear1(graph&);

  void on_changing_layout(GraphWin&, const list<node>&, const list<edge>&);

  public :

  graphwin_undo(GraphWin& gw);
  graphwin_undo(const graphwin_undo& x) : _gw(x._gw) { }
  graphwin_undo& operator=(const graphwin_undo& x) { _gw = x._gw; return *this; }
  ~graphwin_undo();

  GraphWin* get_graphwin() const;
  void set_graphwin(GraphWin *gw);

};

LEDA_END_NAMESPACE


#endif
