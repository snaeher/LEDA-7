#ifndef GRAPH_UNDO_H
#define GRAPH_UNDO_H

#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/graph/graph_observer.h>
#include <LEDA/graph/undo.h>
#else
#include "graph_observer.h"
#include "undo.h"
#endif


LEDA_BEGIN_NAMESPACE

// ---------------------------------------------------------------------

struct graph_undo_object { virtual ~graph_undo_object() {} };

struct node_undo : public graph_undo_object { node v; };
struct edge_undo : public graph_undo_object { edge e; };

// ---------------------------------------------------------------------

class graph_undo_redo_stack : public graph_observer , public undo_redo_stack {

  friend class graph_undo_item;

  node_map<node_undo*> u_nodes;
  edge_map<edge_undo*> u_edges;

  list<graph_undo_object*> graph_undo_objects;

  protected :

  virtual void recreate_node(node_undo*, int);
  virtual void recreate_edge(edge_undo*, int);

  node_undo* create_undo(node);
  edge_undo* create_undo(edge);

  void on_new_node2(graph&, node);
  void on_new_edge2(graph&, edge);
  void on_del_node1(graph&, node);
  void on_del_edge1(graph&, edge);
  void on_move_edge1(graph&, edge, node, node);

  void on_join_from2(graph&, graph&, const list<node>&);
  void on_join_to1(graph&, graph&);

  void on_hide_edge1(graph&, edge);
  void on_restore_edge1(graph&, edge);

  void on_clear1(graph&);

  void internal_clear();

  public :

  EVENT3<graph&,node,int> recreate_node_event;  // int .. previous index
  EVENT3<graph&,edge,int> recreate_edge_event;  

  node_undo* get_undo(node v);
  edge_undo* get_undo(edge e);

  void clear_stack( bool complete_clear, 
		    bool ignore_complex_level);


  virtual void set_graph(graph*);

  graph_undo_redo_stack();
  graph_undo_redo_stack(graph&);

  ~graph_undo_redo_stack();
};

LEDA_END_NAMESPACE

#endif
