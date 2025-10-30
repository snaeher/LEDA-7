/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _graph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/system/assert.h>


//------------------------------------------------------------------------------
// basic graph operations
//
// by S. Naeher (1995-2022)
//
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

#define REGISTER g_regist
/*
#define REGISTER g_register<graph>
*/

graph::graph() : REGISTER(1,node_data_slots, 1,edge_data_slots)
{ max_n_index = -1;
  max_e_index = -1; 
  max_f_index = -1; 
  parent = 0; 
  undirected = false;
  data_sz[0] = node_data_slots;
  data_sz[1] = edge_data_slots;
  data_sz[2] = 0;
  FaceOf = 0;
  handlers_enabled = true;
  create_event(*this);
 }


graph::graph(int sz1, int sz2) : REGISTER(1,sz1,1,sz2)
{ max_n_index = -1;
  max_e_index = -1; 
  max_f_index = -1; 
  parent = 0; 
  undirected = false;
  data_sz[0] = sz1;
  data_sz[1] = sz2;
  data_sz[2] = 0;
  FaceOf = 0;
  handlers_enabled = true;
  create_event(*this);
 }


void graph::copy_all_entries() const
{ node v;
  forall_nodes(v,*this) copy_node_entry(v->data(0));
  for(v = v_hide.head(); v; v = v_hide.succ(v)) 
    copy_node_entry(v->data(0));

  edge e;
  forall_edges(e,*this) copy_edge_entry(e->data(0));
  for(e = e_hide.head(); e; e = e_hide.succ(e)) 
    copy_edge_entry(e->data(0));
}


void graph::clear_all_entries() const
{ 
  node v;
  forall_nodes(v,*this) clear_node_entry(v->data(0));
  for(v = v_hide.head(); v; v = v_hide.succ(v)) 
    clear_node_entry(v->data(0));

  edge e;
  forall_edges(e,*this) clear_edge_entry(e->data(0));
  for(e = e_hide.head(); e; e = e_hide.succ(e)) 
    clear_edge_entry(e->data(0));
}



void graph::copy_graph(const graph& G)  
{ 
  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  for(int k = 0; k < 3; k++)
  { data_sz[k] = G.data_sz[k];
    //for(int i=1; i<=data_sz[k]; i++) free_data[k].append(i);
   }

  set_node_bound(n);
  set_edge_bound(m);


  max_n_index = -1;
  max_e_index = -1;
  max_f_index = -1;

  e_list.clear();

  FaceOf = 0;

  parent = 0;

  if (n == 0) return;

  //node* node_vec = new node[G.max_n_index+1];
  //edge* edge_vec = new edge[G.max_e_index+1];

  node* node_vec = LEDA_NEW_VECTOR(node,G.max_n_index+1);
  edge* edge_vec = LEDA_NEW_VECTOR(edge,G.max_e_index+1);

  if (node_vec == 0 || edge_vec == 0) 
    LEDA_EXCEPTION(1," copy_graph: out of memory");

  // create all nodes
  node v;
  forall_nodes(v,G) 
     node_vec[index(v)] = new_node(v->data(0));

  // create all edges
  edge e;
  forall_edges(e,G)
  { node a = node_vec[index(source(e))];
    node b = node_vec[index(target(e))];
    edge_vec[index(e)] = add_edge(a,b,e->data(0));
   }


  // insert edges into adjacency lists 
  forall_nodes(v,G) 
  { node a = node_vec[index(v)];
    edge last = nil;

    a->clear_adj_lists();

    if (G.undirected)
       forall_adj_edges(e,v) 
       { edge x = edge_vec[index(e)];
         if (v == source(e))
           a->insert_adj_edge(x,last,0,0,0);
         else
           a->insert_adj_edge(x,last,0,1,0);
         last = x;
        }
    else
      { forall_adj_edges(e,v) 
        { edge x = edge_vec[index(e)];
          a->insert_adj_edge(x,last,0,0,0);
          last = x;
         }
        if (indeg(v) > 0)
        {  if (v->first_adj_edge(1)) // implementation  supports in-edges
           { last = 0;
             forall_in_edges(e,v) 
             { edge x = edge_vec[index(e)];
               a->insert_adj_edge(x,last,1,1,0);
               last = x;
              }
            }
           else
             for(int j=0; j<indeg(v); j++)
               a->push_adj_edge(0,1,1); // indeg(a)++
         }
       }
   }
       

  // update reversal information
  forall_edges(e,G) 
  { edge r = e->reversal();
    if (r) edge_vec[index(e)]->set_reversal(edge_vec[index(r)]);
   }


  // copy faces (if existing)
  face f;
  forall_faces(f,G)
  { face f1 = new_face(f->data(0)); 
    f1->set_first_edge(edge_vec[index(f->first_edge())]);
   }

  //delete[] node_vec;
  //delete[] edge_vec;
  LEDA_DEL_VECTOR(node_vec);
  LEDA_DEL_VECTOR(edge_vec);

}


graph::graph(const graph& G) : REGISTER(1,G.data_sz[0], 1,G.data_sz[1])
{ undirected = G.undirected; 
  handlers_enabled = G.handlers_enabled;
  copy_graph(G); 
  create_event(*this);
 }


graph& graph::operator=(const graph& G)
{ if (&G != this)
  { graph::clear();
    undirected = G.undirected;
    handlers_enabled = G.handlers_enabled;
    copy_graph(G);
   }
  return *this;
}


void graph::join(graph& G)
{ // moves all objects from G to this graph and clears G

  if (G.undirected != undirected)
        LEDA_EXCEPTION(1,"graph::join(G): cannot merge directed with undirected graph.");

  for(int d=0; d<3; d++) {
     if (G.data_sz[d] != data_sz[d])
        LEDA_EXCEPTION(1,"graph::join(G): cannot merge graph with different data sizes.");

  }

  list<node> V = G.all_nodes();

  if (handlers_enabled) pre_join_from_graph_handler(G);
  if (G.handlers_enabled) G.pre_join_to_graph_handler(*this);


  int n = max_node_index();
  int m = max_edge_index();
  int k = max_face_index();


  // rename used/hidden/free objects of G

  node v;
  forall_nodes(v,G) 
  { v->set_id(++n); v->set_owner(this); }

  for (v = G.v_hide.head(); v != nil; v = G.v_hide.succ(v))
  { v->set_id(++n); v->set_owner(this); }

  for(v = G.v_free.head(); v; v = G.v_free.succ(v)) 
  { v->set_id(++n); v->set_owner(this); }


  edge e;
  forall_edges(e,G) e->set_id(++m); 

  for (e = G.e_hide.head(); e != nil; e = G.e_hide.succ(e)) 
    e->set_id(++m);

  for(e = G.e_free.head(); e; e = G.e_free.succ(e)) 
    e->set_id(++m);


  face f;
  forall_faces(f,G) f->set_id(++k); 

  for(f = G.f_free.head(); f; f = G.f_free.succ(f)) f->set_id(++k);

  max_n_index = n;
  max_e_index = m;
  max_f_index = k;


  // concatenate object lists

  v_list.conc(G.v_list);
  e_list.conc(G.e_list);
  f_list.conc(G.f_list);

  v_free.conc(G.v_free);
  e_free.conc(G.e_free);
  f_free.conc(G.f_free);

  v_hide.conc(G.v_hide);
  e_hide.conc(G.e_hide);


  G.max_n_index = -1;
  G.max_e_index = -1;
  G.max_f_index = -1;

  if (handlers_enabled) post_join_from_graph_handler(G,V);
  if (G.handlers_enabled) G.post_join_to_graph_handler(*this,V);
}


  



// subgraph constructors  (do not work for undirected graph)

/*
graph::graph(graph& G, const list<node>& nl, const list<edge>& el)
{ // construct subgraph (nl,el) of graph G

  parent = &G;
  node v,w;
  edge e;

  //node* N = new node[G.max_n_index+1];
  node* N = LEDA_NEW_VECTOR(node,G.max_n_index+1);

  forall(v,nl)
   { if (graph_of(v) != parent) 
      LEDA_EXCEPTION(1,"graph: illegal node in subgraph constructor");
     N[index(v)] = new_node((GenPtr)v);
    }

  forall(e,el)
   { v = source(e);
     w = target(e);
     if ( graph_of(e)!= parent || N[index(v)]==0 || N[index(w)]==0 ) 
      LEDA_EXCEPTION(1,"graph: illegal edge in subgraph constructor");
     new_edge(N[index(v)],N[index(w)],(GenPtr)e);
    }

  undirected = G.undirected;

  //delete[] N;
  LEDA_DEL_VECTOR(N);

 }

graph::graph(graph& G, const list<edge>& el)
{ // construct subgraph of graph G with edge set el

  node  v,w;
  edge  e;

  //node* N = new node[G.max_n_index+1];
  node* N = LEDA_NEW_VECTOR(node,G.max_n_index+1);

  forall_nodes(v,G) N[index(v)] = 0;

  parent = &G;

  forall(e,el)
   { v = source(e);
     w = target(e);
     if (N[index(v)] == 0) N[index(v)] = new_node((GenPtr)v);
     if (N[index(w)] == 0) N[index(w)] = new_node((GenPtr)w);
     if ( graph_of(e) != parent )
      LEDA_EXCEPTION(1,"graph: illegal edge in subgraph constructor");
     new_edge(N[index(v)],N[index(w)],(GenPtr)e);
    }

  undirected = G.undirected;

  //delete[] N;
  LEDA_DEL_VECTOR(N);

 }

*/


//------------------------------------------------------------------------------
// destruction
//------------------------------------------------------------------------------


void graph::del_all_nodes() { clear(); }

void graph::del_all_edges()
{

  restore_all_edges();

  edge e;

  e = e_list.head(); 
  while (e) 
  { edge next = e_list.succ(e); 
    dealloc_edge(e);
    e = next;
   }

  e = e_hide.head(); 
  while (e) 
  { edge next = e_hide.succ(e); 
    dealloc_edge(e);
    e = next;
   }

  e = e_free.head(); 
  while (e) 
  { edge next = e_free.succ(e); 
    dealloc_edge(e);
    e = next;
   }

  e_list.clear();
  e_hide.clear();
  e_free.clear();

  max_e_index = -1;

  node v;
  forall_nodes(v,*this) v->clear_adj_lists(); 

  e_list_tmp.clear();
}


void graph::del_all_faces()
{
  face f = f_list.head(); 
  while (f) 
  { face next = f_list.succ(f); 
    dealloc_face(f);
    f = next;
   }

  f = f_free.head(); 
  while (f) 
  { face next = f_free.succ(f); 
    dealloc_face(f);
    f = next;
   }

  f_free.clear();
  f_list.clear();

  if (FaceOf)
  { delete FaceOf;
    FaceOf = 0;
   }

  max_f_index = -1;

  f_list_tmp.clear();
}



void graph::clear()
{ 
  if (handlers_enabled) pre_clear_handler();

  restore_all_nodes();
  restore_all_edges();

  for(int k=0; k<3; k++)
  { graph_map<graph>* m=0;
    forall(m,get_map_list(k))
      if (m->slot_index() > 0) m->clear_table();
   }

  del_all_faces();
  del_all_edges();

  node v = v_list.head(); 
  while (v) 
  { node next = v_list.succ(v); 
    dealloc_node(v);
    v = next;
   }

  v = v_hide.head(); 
  while (v) 
  { node next = v_hide.succ(v); 
    dealloc_node(v);
    v = next;
   }

  v = v_free.head(); 
  while (v) 
  { node next = v_free.succ(v); 
    dealloc_node(v);
    v = next;
   }

  v_list.clear();
  v_hide.clear();
  v_free.clear();

  v_list_tmp.clear();

  max_n_index = -1;

  if (handlers_enabled) post_clear_handler();
}


graph::~graph()
{ 
  destroy_event(*this);

  clear();
  for(int k=0; k<3; k++)
  { graph_map<graph>* m=0;
    forall(m,get_map_list(k)) m->set_owner(0);
   }
}


//------------------------------------------------------------------------------
// accessing node and edge lists
//------------------------------------------------------------------------------


const list<node>& graph::all_nodes() const 
{ ((list<node>&)v_list_tmp).clear();
  node v;
  forall_nodes(v,*this)
      ((list<node>&)v_list_tmp).append(v);
  return v_list_tmp;
}


const list<edge>& graph::all_edges() const 
{ ((list<edge>&)e_list_tmp).clear();
  edge e;
  forall_edges(e,*this)
      ((list<edge>&)e_list_tmp).append(e);
  return e_list_tmp;
}

const list<face>& graph::all_faces() const 
{ ((list<face>&)f_list_tmp).clear();
  face f;
  forall_faces(f,*this)
      ((list<face>&)f_list_tmp).append(f);
  return f_list_tmp;
}



list<edge> graph::out_edges(node v) const
{ list<edge> result;
  edge e;
  forall_out_edges(e,v) result.append(e);
  return result;
}


list<edge> graph::in_edges(node v) const
{ list<edge> result;
  edge e;
  forall_in_edges(e,v) result.append(e);
  return result;
}

list<edge> graph::adj_edges(node v) const
{ list<edge> result;
  edge e;
  forall_adj_edges(e,v) result.append(e);
  return result;
}


list<node> graph::adj_nodes(node v) const
{ list<node> result;
  edge e;
  forall_adj_edges(e,v) result.append(opposite(v,e));
  return result;
}



//------------------------------------------------------------------------------
// update operations
//------------------------------------------------------------------------------


list<edge> graph::insert_reverse_edges()
{ list<edge> L;
  edge e = first_edge();

  if (e != nil)
  { L.append(new_edge(target(e),source(e),e->data(0)));
    copy_edge_entry(e->data(0));
    e = succ_edge(e);
   }

  edge stop = last_edge();

  while (e != stop)
  { L.append(new_edge(target(e),source(e),e->data(0)));
    copy_edge_entry(e->data(0));
    e = succ_edge(e);
   } 

  return L;
}



face graph::add_face(GenPtr inf)
{ face f;
  if ( f_free.empty() )
    { f = construct_face(data_sz[2],inf,++max_f_index);
      f->set_owner(this);
     }
  else
    { f = f_free.pop();
      f->data(0) = inf;
     }

  f_list.append(f);

  graph_map<graph>* m=0;
  forall(m,get_map_list(2)) m->re_init_entry(f);

  return f;
}

void graph::dealloc_face(face f) { destroy_face(f,data_sz[2]); }

void graph::del_face(face f) 
{ f_list.remove(f); 
  f_free.push(f);
  graph_map<graph>* m=0;
  forall(m,get_map_list(2)) 
  { int i = m->slot_index();
    if (i > 0) m->clear_entry(f->data(i));
   }
 }



node graph::add_node(GenPtr inf, node u)
{ node v;
#if GRAPH_REP == 2
  v = v_list.construct(inf,++max_n_index);
#else
  if ( v_free.empty() )
      v = construct_node(data_sz[0],inf,++max_n_index);
  else
    { v = v_free.pop();
      v->data(0) = inf;
     }


  // insert v before u into v_list
  if (u == nil)
    v_list.append(v);
  else
    v_list.insert(v,u);
#endif

  v->set_owner(this);

  graph_map<graph>* m=0;
  forall(m,get_map_list(0)) m->re_init_entry(v);
  
  return v;
}


void graph::dealloc_node(node v) { destroy_node(v,data_sz[0]); }


node graph::new_node(GenPtr i, node u, int dir)
{ if (handlers_enabled) pre_new_node_handler();
  if (u && dir == leda::behind) u = v_list.succ(u);
  node v = add_node(i,u);
  if (handlers_enabled) post_new_node_handler(v);
  return v;
}


void graph::del_node(node v)
{ 

  
  if (graph_of(v) != this) 
        LEDA_EXCEPTION(4,"del_node(v): v is not in G");

  if (handlers_enabled) start_del_node_handler(v);


  if (is_hidden(v)) 
  { //restore v and hidden adjacent edges
    restore_node(v);
    list<edge> L;
    edge e;
    for (e = e_hide.head(); e != nil; e = e_hide.succ(e))
       if (source(e) == v || target(e) == v) L.append(e);
    forall(e,L) restore_edge(e);
  }


  // delete adjacent edges

  edge  e;
  while ((e=v->first_adj_edge(0)) != nil) del_edge(e);

  if (!undirected && indeg(v) > 0)
  {  if (v->first_adj_edge(1)) // implementation  supports in-edges
       while ((e=v->first_adj_edge(1)) != nil) del_edge(e);
     else
     { LEDA_EXCEPTION(0,"del_node: graph_rep does not support in-edges.");
       forall_edges(e,*this)
          if (target(e) == v) del_edge (e);
      }
   }


  if (handlers_enabled) pre_del_node_handler(v);

  if (parent==0) clear_node_entry(v->data(0));
 
  v_list.remove(v);
  v_free.push(v);

  graph_map<graph>* m=0;
  forall(m,get_map_list(0))
  { int i = m->slot_index();
    if (i > 0) m->clear_entry(v->data(i));
   }

  if (handlers_enabled) post_del_node_handler();

}


node graph::merge_nodes(edge e1, node v2)
{ 
  if (undirected)
    LEDA_EXCEPTION(1,"merge_nodes not implemented for undirected graph.");

  node v1 = source(e1);
  if (e1 == v1->last_adj_edge(0)) return merge_nodes(v1,v2);

  edge e;
  forall_out_edges(e,v2) e->set_term(0,v1);
  forall_in_edges(e,v2) e->set_term(1,v1);

  if (v2->first_adj_edge(0)) v1->insert_adj_list(0,e1,v2,0);
  if (v2->first_adj_edge(1)) v1->append_adj_list(1,v2,1);

  del_node(v2);
  return v1;
}


node graph::merge_nodes(node v1, node v2)
{ 
  if (undirected)
    LEDA_EXCEPTION(1,"merge_nodes not implemented for undirected graph.");

  edge e;
  forall_out_edges(e,v2) e->set_term(0,v1);
  forall_in_edges(e,v2) e->set_term(1,v1);

  v1->append_adj_list(0,v2,0);
  v1->append_adj_list(1,v2,1);

  del_node(v2);
  return v1;
}



edge graph::add_edge(node v, node w, GenPtr inf)
{ edge e;

  assert(v != 0);
  assert(w != 0);

  if (graph_of(v) != this) 
     LEDA_EXCEPTION(6, "new_edge(v,w): v not in graph");

  if (graph_of(w) != this) 
     LEDA_EXCEPTION(6, "new_edge(v,w): w not in graph");

#if GRAPH_REP == 2
  e = e_list.construct(inf,++max_e_index);
  e->set_term(0,v);
  e->set_term(1,w);
#else
  if ( e_free.empty() )
      e = construct_edge(data_sz[1],v,w,inf,++max_e_index);
  else
    { e = e_free.pop();
      e->data(0) = inf;
      e->set_term(0,v);
      e->set_term(1,w);
      if (e->reversal()) e->set_reversal(nil);
    }
  e_list.append(e);
#endif

  graph_map<graph>* m=0;
  forall(m,get_map_list(1)) m->re_init_entry(e);

  return e;
}


void graph::dealloc_edge(edge e) { destroy_edge(e,data_sz[1]); }


void graph::del_adj_edge(edge e, node v, node w)
{ if (undirected)
    { v->del_adj_edge(e,0,0);
      w->del_adj_edge(e,0,1);
     }
  else
    { v->del_adj_edge(e,0,0);
      w->del_adj_edge(e,1,1);
     }
 }



void graph::ins_adj_edge(edge e, node v, edge e1, node w, edge e2,int d1,int d2)
{ 
  // insert edge e
  // after(if d1=0)/before(if d1=1) e1 to adj_list of v
  // after(if d2=0)/before(if d2=1) e2 to in_list (adj_list) of w
  // (most general form of new_edge)

  if ( undirected ) 
   { if (v == w)  
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): selfloop in undirected graph.");
     if (e1 && v != source(e1) && v != target(e1)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): v is not adjacent to e1.");
     if (e2 && w != source(e2) && w != target(e2)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): w is not adjacent to e2.");

     v->insert_adj_edge(e,e1,0,0,d1);
     w->insert_adj_edge(e,e2,0,1,d2);
    }
  else
   { if (e1 && v != source(e1)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): v is not source of e1.");
     if (e2 && w != source(e2) && w != target(e2)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): w is not target of e2.");

     v->insert_adj_edge(e,e1,0,0,d1);
     w->insert_adj_edge(e,e2,1,1,d2);
    }
}



edge graph::new_edge(node v, edge e1, node w, edge e2, GenPtr i,int d1,int d2)
{ 
  // add edge (v,w,i)
  // after(if d1=0)/before(if d1=1) e1 to adj_list of v
  // after(if d2=0)/before(if d2=1) e2 to in_list (adj_list) of w
  // (most general form of new_edge)

  if ( undirected ) 
   { if (v == w)  
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): selfloop in undirected graph.");
     if (e1 && v != source(e1) && v != target(e1)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): v is not adjacent to e1.");
     if (e2 && w != source(e2) && w != target(e2)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): w is not adjacent to e2.");
   }
  else
   { if (e1 && v != source(e1)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): v is not source of e1.");
     if (e2 && w != target(e2)) 
        LEDA_EXCEPTION(1,"new_edge(v,e1,w,e2): w is not target of e2.");
    }

  if (handlers_enabled) pre_new_edge_handler(v,w);
  edge e = add_edge(v,w,i);
  ins_adj_edge(e,v,e1,w,e2,d1,d2);
  if (handlers_enabled) post_new_edge_handler(e);
  return e ; 
}

edge graph::new_edge(node v, edge e1, node w, GenPtr i,int d)
{
  // add edge (v,w) after/before e1 to adj_list of v
  // append it to in_list (adj_list) of w

  return new_edge(v,e1,w,nil,i,d,0); 
}

edge graph::new_edge(node v, node w, edge e2, GenPtr i,int d)
{
  // append edge (v,w) to adj_list of v
  // insert it after/before e2 to in_list (adj_list) of w

  return new_edge(v,nil,w,e2,i,d,0); 
}

edge graph::new_edge(edge e1, node w, GenPtr i, int dir)  
{ 
  // add edge (source(e1),w) after/before e1 to adj_list of source(e1)
  // append it to in_list (adj_list) of w

  return new_edge(source(e1),e1,w,nil,i,dir,0);
 }

edge graph::new_edge(node v, edge e2, GenPtr i, int dir)  
{ 
  // append edge(v,target(e2))  to adj_list of v
  // insert it after/before e2 to in_list (adj_list) of target(e2)

  return new_edge(v,nil,target(e2),e2,i,0,dir);
 }

edge graph::new_edge(edge e1, edge e2, GenPtr i, int dir1, int dir2)  
{ 
  //add edge (source(e1),target(e2)) 
  //after(dir=0)/before(dir=1) e1 to adj_list of source(e1)
  //after(dir=1)/before(dir=1) e2 to in_list (adj_list) of target(e2)

  return new_edge(source(e1),e1,target(e2),e2,i,dir1,dir2);
 }


edge graph::new_edge(node v, node w, GenPtr i)
{ 
  // append (v,w) it to adj_list of v and to in_list (adj_list) of w

  return new_edge(v,nil,w,nil,i,0,0); 
 }






node graph::split_edge(edge e, GenPtr node_inf, edge& e1, edge& e2)
{
   // splits e into e1 and e2 by putting a new node u on e 


  //node v = source(e);
  node w = target(e);
  node u = add_node(node_inf); 

  e1 = e;
  e2 = add_edge(u,w,e->data(0));

  copy_edge_entry(e2->data(0));

  if (undirected)
    { u->push_adj_edge(e2,0,0);
      w->insert_adj_edge(e2,e,0,1,0);
      w->del_adj_edge(e,0,1);
      e->set_term(1,u);
      u->push_adj_edge(e,0,1);
     }
  else
    { u->push_adj_edge(e2,0,0);
      w->insert_adj_edge(e2,e,1,1,0);
      w->del_adj_edge(e,1,1);
      e->set_term(1,u);
      u->push_adj_edge(e,1,1);
     }

 return u;
}




void graph::del_edge(edge e)
{ node v = source(e);
  node w = target(e);

  if (graph_of(v) != this) 
     LEDA_EXCEPTION(10,"del_edge(e): e is not in G");

  if (is_hidden(e)) restore_edge(e);

  if (handlers_enabled) pre_del_edge_handler(e);

  if (e->reversal()) e->reversal()->set_reversal(nil);

  del_adj_edge(e,v,w);

  if (parent == 0) clear_edge_entry(e->data(0));

  e_list.remove(e);
  e_free.push(e);

  graph_map<graph>* m=0;
  forall(m,get_map_list(1)) 
  { int i = m->slot_index();
    if (i > 0) m->clear_entry(e->data(i));
   }

  if (handlers_enabled) post_del_edge_handler(v,w);
}

 
void graph::hide_edge(edge e)
{
  if (is_hidden(e)) return;
  //LEDA_EXCEPTION(1,"graph::hide_edge: edge is already hidden.");

  if (handlers_enabled) pre_hide_edge_handler(e);


  node v = source(e);
  node w = target(e);

  del_adj_edge(e,v,w);

  e_list.remove(e);
  e_hide.append(e);

  e->hide();

  if (handlers_enabled) post_hide_edge_handler(e);
 }
 
void graph::hide_adj_edge(edge e)
{ if (is_hidden(e)) return;
  node v = source(e);
  node w = target(e);
  del_adj_edge(e,v,w);
  e->hide();
}


 
void graph::hide_node(node v, list<edge>& h_edges)
{
  // appends hidden edges to h_edges  (does not clear h_edges !!)

  if (is_hidden(v)) return;

  if (handlers_enabled) pre_hide_node_handler(v);

  // hide (non-hidden) in/out edges

  edge  e;
  while ((e=v->first_adj_edge(0)) != nil) 
  { hide_edge(e);
    h_edges.append(e);
   }

  if (!undirected)
    while ((e=v->first_adj_edge(1)) != nil)
    { hide_edge(e);
      h_edges.append(e);
     }

  v_list.remove(v);
  v_hide.append(v);

  v->hide();

  if (handlers_enabled) post_hide_node_handler(v);
 }


void graph::hide_node(node v)
{ list<edge> h_edges;
  hide_node(v,h_edges);
 }



void graph::restore_edge(edge e)
{
  node v = source(e);
  node w = target(e);

  if (!is_hidden(e))  
    LEDA_EXCEPTION(1,"graph::restore_edge: edge is not hidden.");

  if (is_hidden(v))  
    LEDA_EXCEPTION(1,"graph::restore_edge: source(e) is hidden.");

  if (is_hidden(w))  
    LEDA_EXCEPTION(1,"graph::restore_edge: target(e) is hidden.");

  if (handlers_enabled) pre_restore_edge_handler(e);

  e_hide.remove(e);
  e_list.push(e);

  if (undirected)
    { v->push_adj_edge(e,0,0);
      w->push_adj_edge(e,0,1);
     }
  else
    { v->push_adj_edge(e,0,0);
      w->push_adj_edge(e,1,1);
     }

  e->unhide();

  if (handlers_enabled) post_restore_edge_handler(e);
}


void graph::restore_adj_edge(edge e)
{ node v = source(e);
  node w = target(e);
  if (!is_hidden(e)) return;  
  { v->push_adj_edge(e,0,0);
    w->push_adj_edge(e,1,1);
   }
  e->unhide();
}


void graph::restore_node(node v)
{
  if (!is_hidden(v))  
    LEDA_EXCEPTION(1,"graph::restore_node: node is not hidden.");

  if (handlers_enabled) pre_restore_node_handler(v);

  v_hide.remove(v);
  v_list.push(v);
  v->unhide();

  if (handlers_enabled) post_restore_node_handler(v);
}


list<edge> graph::hidden_edges() const
{ list<edge> L;
  for (edge e = e_hide.head(); e != nil; e = e_hide.succ(e))
     L.append(e);
  return L;
 }

list<node> graph::hidden_nodes() const
{ list<node> L;
  for (node v = v_hide.head(); v != nil; v = v_hide.succ(v))
     L.append(v);
  return L;
 }


void graph::restore_all_edges()
{ edge e = e_hide.head(); 
  while (e)
  { edge succ = e_hide.succ(e);
    restore_edge(e);
    e = succ;
  }
 }

void graph::restore_all_nodes()
{ node v = v_hide.head(); 
  while (v)
  { node succ = v_hide.succ(v);
    restore_node(v);
    v = succ;
  }
 }



void graph::move_edge(edge e,edge e1,edge e2,int d1,int d2)
{ if (is_hidden(e)) 
       LEDA_EXCEPTION(1,"graph::move_edge:  cannot move hidden edge.");
  node v0 = source(e);
  node w0 = target(e);
  node v = source(e1);
  node w = target(e2);
  if (handlers_enabled) pre_move_edge_handler(e,v,w);
  if (e == e1)
  { e1 = adj_succ(e);
    d1 = leda::before;
    if (e1 == nil)
    { e1 = adj_pred(e);
      d1 = leda::behind;
     }
   }
  if (e == e2)
  { e2 = in_succ(e);
    d2 = leda::before;
    if (e2 == nil)
    { e2 = in_pred(e);
      d2 = leda::behind;
     }
   }
  del_adj_edge(e,source(e),target(e));
  e->set_term(0,v);
  e->set_term(1,w);
  ins_adj_edge(e,v,e1,w,e2,d1,d2);
  if (handlers_enabled) post_move_edge_handler(e,v0,w0);
 }


void graph::move_edge(edge e,edge e1,node w,int dir)
{ if (is_hidden(e)) 
       LEDA_EXCEPTION(1,"graph::move_edge:  cannot move hidden edge.");
  node v0 = source(e);
  node w0 = target(e);
  node v  = source(e1);
  if (e == e1)
  { e1 = adj_succ(e);
    dir = leda::before;
    if (e1 == nil)
    { e1 = adj_pred(e);
      dir = leda::behind;
     }
   }
  if (handlers_enabled) pre_move_edge_handler(e,v,w);
  del_adj_edge(e,v0,w0);
  e->set_term(0,v);
  e->set_term(1,w);
  ins_adj_edge(e,v,e1,w,nil,dir,0);
  if (handlers_enabled) post_move_edge_handler(e,v0,w0);
}

void graph::move_edge(edge e,node v, edge e2,int dir)
{ if (is_hidden(e)) 
       LEDA_EXCEPTION(1,"graph::move_edge:  cannot move hidden edge.");
  node v0 = source(e);
  node w0 = target(e);
  node w = target(e2);
  if (e == e2)
  { e2 = in_succ(e);
    dir = leda::before;
    if (e2 == nil)
    { e2 = in_pred(e);
      dir = leda::behind;
     }
   }
  if (handlers_enabled) pre_move_edge_handler(e,v,w);
  del_adj_edge(e,v0,w0);
  e->set_term(0,v);
  e->set_term(1,w);
  ins_adj_edge(e,v,nil,w,e2,0,dir);
  if (handlers_enabled) post_move_edge_handler(e,v0,w0);
}

void graph::move_edge(edge e, node v, node w)
{ if (is_hidden(e)) 
       LEDA_EXCEPTION(1,"graph::move_edge:  cannot move hidden edge.");
  node v0 = source(e);
  node w0 = target(e);
  if (handlers_enabled) pre_move_edge_handler(e,v,w);

  //del_adj_edge(e,source(e),target(e));
  //ins_adj_edge(e,v,nil,w,nil,0,0);

  if (v != source(e)) 
  { source(e)->del_adj_edge(e,0,0);
    v->insert_adj_edge(e,nil,0,0,0);
    e->set_term(0,v);
   }

  if (w != target(e)) 
  { if (undirected)
      { target(e)->del_adj_edge(e,0,1);
        w->insert_adj_edge(e,nil,0,1,0);
       }
    else
      { target(e)->del_adj_edge(e,1,1);
        w->insert_adj_edge(e,nil,1,1,0);
       }
    e->set_term(1,w);
   }

  if (handlers_enabled) post_move_edge_handler(e,v0,w0);
}


edge graph::rev_edge(edge e)
{ 
  if (is_hidden(e)) 
       LEDA_EXCEPTION(1,"graph::rev_edge: cannot reverse hidden edge.");

  node v = source(e);
  node w = target(e);

  if (handlers_enabled) pre_move_edge_handler(e,w,v);

/*
  if (undirected)
  { edge s = e->succ_adj_edge(0);
    edge p = e->pred_adj_edge(0);
    e->succ_adj_edge[0] = e->succ_adj_edge(1); 
    e->pred_adj_edge[0] = e->pred_adj_edge(1); 
    e->succ_adj_edge[1] = s;
    e->pred_adj_edge[1] = p;
    e->set_term(0,w);
    e->set_term(1,v);
   }
  else
*/
  { del_adj_edge(e,v,w);
    e->set_term(0,w);
    e->set_term(1,v);
    ins_adj_edge(e,w,nil,v,nil,0,0);
   }

  if (handlers_enabled) post_move_edge_handler(e,v,w);

  return e; 
}


void graph::rev_all_edges()              
{ if (!undirected) 
  { list<edge> L = all_edges();
    edge e;
    forall(e,L) rev_edge(e);
   }
}


void graph::set_node_position(node v, node p)
{  if (v == p) return;
   node s = (p == nil) ? v_list.head() : v_list.succ(p);
   if (v != s)
   { v_list.remove(v);
     v_list.insert(v,s);
    }
}


void graph::set_edge_position(edge e, edge p)
{  if (e == p) return;
   edge s;
   if (p == nil) 
      s = e_list.head();
   else
      s = e_list.succ(p);
   if (e != s)
   { e_list.remove(e);
     e_list.insert(e,s);
    }
}



void graph::del_nodes(const list<node>& L) 
{ node v;
  forall(v,L) del_node(v);
 }

void graph::del_edges(const list<edge>& L) 
{ edge e;
  forall(e,L) del_edge(e);
 }

void graph::make_undirected()
{
  if (undirected) return; 

  int loop_count = 0;

  edge e;
  forall_edges(e,*this)
  { if (source(e) != target(e)) continue;
    del_edge(e);
    loop_count++;
   }
 
  if (loop_count > 0)
    LEDA_EXCEPTION(0,string(" %d selfloops deleted in ugraph constructor",loop_count));


  /* adj_list(v) = out_list(v) + in_list(v) forall nodes v  */

  node v;
  forall_nodes(v,*this) v->append_adj_list(0,v,1); 

  undirected = true; 
}


void graph::make_directed()
{ 
  if (!undirected) return;

  // for every node v delete entering edges from adj_list(v)
  // and put them back into in_list(v)

  node v;
  forall_nodes(v,*this)
  { edge e = v->first_adj_edge(0);
    while (e)
      if (v == target(e))
         { edge e1 = e->succ_adj_edge(1);
           v->del_adj_edge(e,0,1);
           v->push_adj_edge(e,1,1); 
           e = e1;
          }
      else 
         e = e->succ_adj_edge(0);
   }

  undirected = false; 

}



void init_node_data(const graph& G,int i, GenPtr x)
{ node v;
  forall_nodes(v,G) v->data(i) = x;
 }



node graph::choose_node() const
{ int n = number_of_nodes();
  if (n == 0) return nil;
  int r = rand_int(0,n-1);
  node v = first_node();
  while (r--) v = succ_node(v);
  return v;
}


edge graph::choose_edge() const
{ int m = number_of_edges();
  if (m == 0) return nil;
  int r = rand_int(0,m-1);
  edge e = first_edge();
  while (r--) e = succ_edge(e);
  return e;
}


face graph::choose_face() const
{ int l = number_of_faces();
  if (l == 0) return nil;
  int r = rand_int(0,l-1);
  face f = first_face();
  while (r--) f = succ_face(f);
  return f;
}



#if GRAPH_REP == 2
void graph::set_node_bound(int n)
{ v_list.init(n,data_sz[0]); }

void graph::set_edge_bound(int m)
{ e_list.init(m,data_sz[1]); }

#else

void graph::set_node_bound(int) {}
void graph::set_edge_bound(int) {}

#endif

LEDA_END_NAMESPACE



