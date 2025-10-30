/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  graphwin_undo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/graphwin_undo.h>
#else
#include "graphwin_undo.h"
#endif


LEDA_BEGIN_NAMESPACE

class save_node_info : public undo_item 
{
  GraphWin* _gw;
  node_undo *_u;
  node_info _i;

public: 

  save_node_info(graph_undo_redo_stack& s, GraphWin& gw, node v, 
		 const node_info& i) :
    _gw(&gw) , _u(s.get_undo(v)) , _i(i) {}

  void undo() { 
    _gw->set_info(_u->v,_i,N_COMPLETE & ~N_SELECT); 
  }
};


class save_edge_info : public undo_item 
{
  GraphWin* _gw;
  edge_undo *_u;
  edge_info _i;

public : 

  save_edge_info(graph_undo_redo_stack& s, GraphWin& gw, edge e, 
		 const edge_info& i) : _gw(&gw) , _u(s.get_undo(e)) , _i(i) 
  { if (_i.p.size() > 1) 
    { _i.p.pop(); 
      _i.p.Pop(); 
     }
   }

  void undo() { 
    _gw->set_info(_u->e,_i,E_COMPLETE & ~E_SELECT); 
  }
};


class release_change_event : public undo_item {

  GraphWin& _gw;
  list<node_undo*> N_;
  list<edge_undo*> E_;

  public :

  release_change_event(graph_undo_redo_stack& s, GraphWin& gw, 
		       const list<node>& N,const list<edge>& E) :
    _gw(gw) {
    node v;
    edge e;
    forall(v,N) N_.append(s.get_undo(v));
    forall(e,E) E_.append(s.get_undo(e));
  }

  void undo() {
    list<node> N;
    list<edge> E;
    node_undo *uv;
    edge_undo *ue;
    forall(uv,N_) N.append(uv->v);
    forall(ue,E_) E.append(ue->e);
    _gw.changing_layout(N,E);
  }

  ~release_change_event() {}

};

// ---------------------------------------------------------------
// ---------------------------------------------------------------

void graphwin_undo::on_clear1(graph& g) {

  node v;
  edge e;
  forall_edges(e,g) {
    push(new save_edge_info(*this,*_gw,e,_gw->get_info(e)));
    _gw->internal_del_edge(e);
  }
  forall_nodes(v,g) {
    push(new save_node_info(*this,*_gw,v,_gw->get_info(v)));
    _gw->internal_del_node(v);
  }
  graph_undo_redo_stack::on_clear1(g);
}

void graphwin_undo::on_join_to1(graph& g, graph& h) {

  // Kommt einem Loeschen aller Knoten und Kanten gleich (ohne expl. Event)

  node v;
  edge e;
  forall_edges(e,g) {
    push(new save_edge_info(*this,*_gw,e,_gw->get_info(e)));
    _gw->internal_del_edge(e);
  }
  forall_nodes(v,g) {
    push(new save_node_info(*this,*_gw,v,_gw->get_info(v)));
    _gw->internal_del_node(v);
  }
  graph_undo_redo_stack::on_join_to1(g,h);
}

void graphwin_undo::recreate_node(node_undo* u, int i) {
  graph_undo_redo_stack::recreate_node(u,i);
  _gw->init_node(u->v);
}

void graphwin_undo::recreate_edge(edge_undo* u, int i) {
  graph_undo_redo_stack::recreate_edge(u,i);
  _gw->init_edge(u->e);
}


void graphwin_undo::on_del_node1(graph& g, node v) {
  push(new save_node_info(*this,*_gw,v,_gw->get_info(v)));
  _gw->internal_del_node(v);
  graph_undo_redo_stack::on_del_node1(g,v);
}

void graphwin_undo::on_del_edge1(graph& g, edge e) {
  push(new save_edge_info(*this,*_gw,e,_gw->get_info(e)));
  _gw->internal_del_edge(e);
  graph_undo_redo_stack::on_del_edge1(g,e);
}

void graphwin_undo::on_changing_layout(GraphWin&,const list<node>& N,const list<edge>& E) {
  node v;
  edge e;
  forall(v,N) {
    push(new save_node_info(*this,*_gw,v,_gw->get_info(v)));
  }
  forall(e,E) {
    push(new save_edge_info(*this,*_gw,e,_gw->get_info(e)));
  }
  push(new release_change_event(*this,*_gw,N,E));
}

graphwin_undo::graphwin_undo(GraphWin& gw) : _gw(0) {
  set_graphwin(&gw);
}

graphwin_undo::~graphwin_undo() { set_graphwin(0); }

GraphWin* graphwin_undo::get_graphwin() const { return _gw; }

void graphwin_undo::set_graphwin(GraphWin *gw) 
{
  if (_gw) 
  { clear();
    set_graph(0);
   }
  
  _gw = gw;
  
  if (!_gw) return;

  set_graph(&(_gw->get_graph()));
  //events[0] = attach(_gw->changing_layout_event,*this,&on_changing_layout);
}

LEDA_END_NAMESPACE
