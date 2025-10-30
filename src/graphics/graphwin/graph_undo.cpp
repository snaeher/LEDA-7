/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  graph_undo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/stream.h>

#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/graph_undo.h>
#else
#include "graph_undo.h"
#endif


LEDA_BEGIN_NAMESPACE

// ---------------------------------------------------------------------

class graph_undo_item : public undo_item {
  graph_undo_redo_stack* _s;
  protected :
  virtual void g_undo(graph&) = 0;
  void undo() { g_undo(*_s->get_graph()); }
  graph_undo_item(graph_undo_redo_stack& s) : _s(&s) {}
  graph& get_graph() { return *_s->get_graph(); }
  public :
  void recreate_node(node_undo* u, int i) { _s->recreate_node(u,i); }
  void recreate_edge(edge_undo* u, int i) { _s->recreate_edge(u,i); }
};

// ---------------------------------------------------------------------

class undo_del_node : public graph_undo_item {
  node_undo* _u;
  node_undo* _succ;  // succ-node in list of all nodes
  string _entry;
  int _index;
  public :
  void g_undo(graph& g) { 
    node v = _succ ? g.new_node(_succ->v,leda::before) : g.new_node();
    g.set_node_entry(v,_entry);
    _u->v = v;
    recreate_node(_u,_index);
  }
  undo_del_node(graph_undo_redo_stack& s, node v) :
    graph_undo_item(s) , _u(s.get_undo(v)) , 
    _entry(get_graph().get_node_entry_string(v)) , _index(index(v)) {
      graph& g = get_graph();
      node w = g.succ_node(v);
      _succ = w ? s.get_undo(w) : nil; 
    }
};

// ---------------------------------------------------------------------

class undo_new_node : public graph_undo_item {
  node_undo* _u;
  public :
  void g_undo(graph& g)  { 
    g.del_node(_u->v);
    _u->v = nil;
  }
  undo_new_node(graph_undo_redo_stack& s, node v) : 
    graph_undo_item(s) , _u(s.get_undo(v)) {}
};

// ---------------------------------------------------------------------

class undo_adj_edge : public graph_undo_item {

  protected :

  node_undo *_s,*_t;       // source,target
  edge_undo *_u;  
  edge_undo *_adj,*_in;    // adj/in_succ
  edge_undo *_pred;        // pred_edge in E

  undo_adj_edge(graph_undo_redo_stack& s, edge e) : 
    graph_undo_item(s) {    
    graph& g = get_graph();
    _u = s.get_undo(e);
    _u->e = e;
    _s = s.get_undo(g.source(e));
    _t = s.get_undo(g.target(e));
    _adj = g.adj_succ(e) ? s.get_undo(g.adj_succ(e)) : nil;
    _in  = g.in_succ(e)  ? s.get_undo(g.in_succ(e))  : nil;
    _pred = g.pred_edge(e) ? s.get_undo(g.pred_edge(e)) : nil; 
  } 
};

// ---------------------------------------------------------------------

class undo_del_edge : public undo_adj_edge {
  string _entry;
  int _index;
  public :
  void g_undo(graph& g) {
    edge e;
    if (_adj) { 
      if (_in) e = g.new_edge(_adj->e,_in->e,leda::before,leda::before);
      else e = g.new_edge(_adj->e,_t->v,leda::before);
    }
    else {
      if (_in) e = g.new_edge(_s->v,_in->e,leda::before);
      else e = g.new_edge(_s->v,_t->v);
    }
    if (_pred) g.set_edge_position(e,_pred->e);
    else g.set_edge_position(e,nil);
    g.set_edge_entry(e,_entry);
    _u->e = e;
    recreate_edge(_u,_index);
  }

  undo_del_edge(graph_undo_redo_stack& s, edge e) : 
    undo_adj_edge(s,e) , _entry(get_graph().get_edge_entry_string(e)) ,
    _index(index(e)) {} 
};

// ---------------------------------------------------------------------

class undo_new_edge : public graph_undo_item {
  edge_undo* _u;
  public :
  void g_undo(graph& g) { 
    g.del_edge(_u->e); 
    _u->e = nil; 
  }
  undo_new_edge(graph_undo_redo_stack& s, edge e) : 
    graph_undo_item(s) , _u(s.get_undo(e)) {}
};

// ---------------------------------------------------------------------

class undo_move_edge : public undo_adj_edge {
  public :
  void g_undo(graph& g) {
    if (_adj) {
      if (_in) g.move_edge(_u->e,_adj->e,_in->e,leda::before,leda::before);
      else g.move_edge(_u->e,_adj->e,_t->v,leda::before);
    }
    else {
      if (_in) g.move_edge(_u->e,_s->v,_in->e,leda::before);
      else g.move_edge(_u->e,_s->v,_t->v);
    }
    if (_pred) g.set_edge_position(_u->e,_pred->e);
    else g.set_edge_position(_u->e,nil);    
  }
  undo_move_edge(graph_undo_redo_stack& s, edge e) : undo_adj_edge(s,e) {}
};

// ---------------------------------------------------------------------

class undo_hide_edge : public graph_undo_item {
  edge_undo *_u;
  undo_move_edge *_undo_move_edge;
  public :
  void g_undo(graph& g) { 
    g.restore_edge(_u->e);
    _undo_move_edge->g_undo(g);
    delete _undo_move_edge;
  }
  undo_hide_edge(graph_undo_redo_stack& s, edge e) : 
    graph_undo_item(s) , _u(s.get_undo(e)) , 
    _undo_move_edge(new undo_move_edge(s,e)) {}
};

// ---------------------------------------------------------------------

class undo_restore_edge : public graph_undo_item {
  edge_undo *_u;
  public :
  void g_undo(graph& g) { g.hide_edge(_u->e); }
  undo_restore_edge(graph_undo_redo_stack& s, edge e) :
    graph_undo_item(s) , _u(s.get_undo(e)) {}
};

// ---------------------------------------------------------------------

struct node_save {
  node_undo* u;
  string entry;
  int index;
  LEDA_MEMORY(node_save)
};

struct edge_save {
  edge_undo* u;
  string entry;
  int index;
  LEDA_MEMORY(edge_save)
};

inline ostream& operator<<(ostream& out, node_save*) { return out; }
inline ostream& operator<<(ostream& out, edge_save*) { return out; }

inline istream& operator>>(istream& in, node_save*) { return in; }
inline istream& operator>>(istream& in, edge_save*) { return in; }

class undo_clear_graph : public graph_undo_item {
  GRAPH<node_save*, edge_save*> _copy;  
  public :
  void g_undo(graph&);
  undo_clear_graph(graph_undo_redo_stack&);
 ~undo_clear_graph();
};

// ---------------------------------------------------------------------

static void _order_in_edges( graph& c, const graph& g, 
			     const node_array<node>& /*nmap*/, 
			     const edge_array<edge>& emap ) {
  node v;
  forall_nodes(v,g) {
    if (g.indeg(v) > 1) {
      edge e, e1 = g.first_in_edge(v);
      edge ec, e1c = emap[e1];
      if (c.in_pred(e1c))                  // first edge ?
	c.move_edge(e1c,e1c,c.first_in_edge(v),leda::behind,leda::before); 
      while ((e = g.in_succ(e1)) != 0) {
	if (c.in_pred(ec = emap[e]) != e1c) c.move_edge(ec,ec,e1c); 
	e1 = e;
	e1c = ec;
      }
    }
  }
}

static void _copy_hidden_edges( graph& c, const graph& g, 
				const node_array<node>& nmap, 
				edge_array<edge>& emap) {
  list<edge> hidden_edges(g.hidden_edges());
  edge e;
  forall(e,hidden_edges) {
    emap[e] = c.new_edge(nmap[g.source(e)],nmap[g.target(e)]);
  }
}

static void _hide_edges(graph& c, const graph& g, edge_array<edge>& emap) {
  list<edge> hidden_edges(g.hidden_edges());
  edge e;
  forall(e,hidden_edges) c.hide_edge(emap[e]);
}

static void copy_graph( graph& c, const graph& g, 
			node_array<node>& nmap, edge_array<edge>& emap,
			bool order_in_edges = true,
			bool copy_hidden_edges = true,
			bool hide_edges = true) {
  c.clear();
  node v;
  edge e;
  forall_nodes(v,g) nmap[v] = c.new_node();
  forall_nodes(v,g) { 
    forall_adj_edges(e,v) emap[e] = c.new_edge(nmap[v],nmap[g.target(e)]);
  }

  if (order_in_edges) _order_in_edges(c,g,nmap,emap);
  if (copy_hidden_edges) {
    _copy_hidden_edges(c,g,nmap,emap);
    if (hide_edges) _hide_edges(c,g,emap);
  }
} 


undo_clear_graph::~undo_clear_graph() {

  node v;
  forall_nodes(v,_copy) delete _copy[v];

  edge e;
  forall_edges(e,_copy) delete _copy[e];

  list<edge> hidden_edges(_copy.hidden_edges());

  forall(e,hidden_edges) delete _copy[e];
}



void undo_clear_graph::g_undo(graph& g) {
  node v;
  edge e;

  node_array<node> v_in_g(_copy,nil);
  edge_array<edge> e_in_g(_copy,nil);
  
  copy_graph(g,_copy,v_in_g,e_in_g,false,true,false);

  forall_nodes(v,_copy) {
    node_save* ns = _copy[v];
    ns->u->v = v_in_g[v];
    g.set_node_entry(ns->u->v,ns->entry);
    recreate_node(ns->u,ns->index);
  }

  forall_edges(e,_copy) {
    edge_save* es = _copy[e];
    es->u->e = e_in_g[e];
    g.set_edge_entry(es->u->e,es->entry);
    recreate_edge(es->u,es->index);
  }

  list<edge> hidden_edges(_copy.hidden_edges());

  forall(e,hidden_edges) {
    edge_save* es = _copy[e];
    es->u->e = e_in_g[e];
    g.set_edge_entry(es->u->e,es->entry);
    recreate_edge(es->u,es->index);
  }

  _hide_edges(g,_copy,e_in_g);
  _order_in_edges(g,_copy,v_in_g,e_in_g);  
}

undo_clear_graph::undo_clear_graph(graph_undo_redo_stack& s) : 
  graph_undo_item(s) {

  graph& g = get_graph();

  node v;
  edge e;

  node_array<node> nmap(g,nil);
  edge_array<edge> emap(g,nil);
  
  copy_graph(_copy,g,nmap,emap);
  
  forall_nodes(v,g) {
    node_save*& ns = _copy[nmap[v]];
    ns = new node_save;
//    cerr << "saving node " << g.index(v) << " to " << ns 
//	 << " and " << s.get_undo(v) << endl;
    ns->u     = s.get_undo(v);
    ns->entry = g.get_node_entry_string(v);
    ns->index = g.index(v);
  }

  forall_edges(e,g) {
    edge_save*& es = _copy[emap[e]];
    es = new edge_save;
    es->u     = s.get_undo(e);
    es->entry = g.get_edge_entry_string(e);
    es->index = g.index(e);
  }

  list<edge> hidden_edges(g.hidden_edges());
  forall(e,hidden_edges) {
    edge_save*& es = _copy[emap[e]];
    es = new edge_save;
    es->u     = s.get_undo(e);
    es->entry = g.get_edge_entry_string(e);
    es->index = g.index(e);
  }
}

// ---------------------------------------------------------------------

node_undo* graph_undo_redo_stack::get_undo(node v) { return u_nodes[v]; }
edge_undo* graph_undo_redo_stack::get_undo(edge e) { return u_edges[e]; }

void graph_undo_redo_stack::recreate_node(node_undo* u, int i) {
//   cerr << "in on_recreate_node #" << g.index(u->v) << " " 
//        << get_undo(u->v) << endl << flush; // ==nil?
  u_nodes[u->v] = u;
  push(new undo_new_node(*this,u->v)); 
  recreate_node_event(*get_graph(),u->v,i);
}

void graph_undo_redo_stack::recreate_edge(edge_undo* u, int i) {
//   cerr << "in on_recreate_edge #" << g.index(u->e) << " " 
//        << get_undo(u->e) << endl << flush; // ==nil?
  u_edges[u->e] = u;
  push(new undo_new_edge(*this,u->e)); 
  recreate_edge_event(*get_graph(),u->e,i);
}

node_undo* graph_undo_redo_stack::create_undo(node v) {
  node_undo* &u = u_nodes[v];
  u = new node_undo;
  u->v = v;
  graph_undo_objects.push(u);
  return u;
}

edge_undo* graph_undo_redo_stack::create_undo(edge e) {
  edge_undo* &u = u_edges[e];
  u = new edge_undo;
  u->e = e;
  graph_undo_objects.push(u);
  return u;
}

void graph_undo_redo_stack::on_new_node2(graph&, node v) {
  if (!(processing_undo() || processing_redo())) { // else recreation
    create_undo(v);
    push(new undo_new_node(*this,v)); 
  }
}

void graph_undo_redo_stack::on_new_edge2(graph&, edge e) {
  if (!(processing_undo() || processing_redo())) {
    create_undo(e);
    push(new undo_new_edge(*this,e));
  }
}

void graph_undo_redo_stack::on_del_node1(graph&, node v) { 
  push(new undo_del_node(*this,v)); 
}

void graph_undo_redo_stack::on_del_edge1(graph&, edge e) { 
  push(new undo_del_edge(*this,e)); 
}

void graph_undo_redo_stack::on_move_edge1(graph&, edge e, node, node) {
  push(new undo_move_edge(*this,e));
}

void graph_undo_redo_stack::on_hide_edge1(graph&, edge e) {
  push(new undo_hide_edge(*this,e));
}

void graph_undo_redo_stack::on_restore_edge1(graph&, edge e) {
  push(new undo_restore_edge(*this,e));
}

void graph_undo_redo_stack::on_clear1(graph&) {
  push(new undo_clear_graph(*this));
}

class undo_join_from_graph : public graph_undo_item {
  list<node_undo*> _nodes;
  public : 
  undo_join_from_graph(graph_undo_redo_stack& s, const list<node>& N) :
    graph_undo_item(s) {
    node v;
    forall(v,N) _nodes.append(s.get_undo(v));
  }
  void g_undo(graph& g) {
    node_undo *u;
    forall(u,_nodes) g.del_node(u->v);
  }

 ~undo_join_from_graph() {}
};

void graph_undo_redo_stack::on_join_from2(graph& g, graph&, const list<node>& N) {
  node v;
  edge e;
  node_array<bool> in_N(g,false);
  forall(v,N) { create_undo(v); in_N[v] = true; }
  forall_edges(e,g) if (in_N[source(e)]) create_undo(e);
  push(new undo_join_from_graph(*this,N));
}

void graph_undo_redo_stack::on_join_to1(graph&,graph&) {
  push(new undo_clear_graph(*this));
}

void graph_undo_redo_stack::internal_clear() {
  graph_undo_object *u;
  forall(u,graph_undo_objects) delete u;
  graph_undo_objects.clear();
}

void graph_undo_redo_stack::clear_stack( bool complete_clear, 
					 bool ignore_complex_level) {
  if (complete_clear) internal_clear();
  undo_redo_stack::clear_stack(complete_clear,ignore_complex_level);
}

void graph_undo_redo_stack::set_graph(graph *g) {
  if (get_graph()) {
    internal_clear();
    clear(); 
    graph_observer::set_graph(nil);
  }
  if (!g) return;
  
  graph_observer::set_graph(g);

  graph& G = *get_graph();

  u_nodes.init(G,nil);
  u_edges.init(G,nil);
  node v;
  edge e;
  forall_nodes(v,G) create_undo(v);
  forall_edges(e,G) create_undo(e);
}

graph_undo_redo_stack::graph_undo_redo_stack() {}
graph_undo_redo_stack::graph_undo_redo_stack(graph& g) { set_graph(&g); }

graph_undo_redo_stack::~graph_undo_redo_stack() { internal_clear(); }

LEDA_END_NAMESPACE











