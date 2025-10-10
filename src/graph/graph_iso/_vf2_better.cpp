/*******************************************************************************
+
+  _vf2_better.cpp
+
+  Author: Johannes Singler
+
+  Copyright (c) 2005
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+
+  An implementation of VF2 improved in comparison to the original due to the
+  use of more sophisticated data structures
+ 
*******************************************************************************/

#include <LEDA/core/array.h>
#include <LEDA/graph/static_graph.h>
#include <LEDA/graph/node_set.h>
#include <LEDA/graph/morphism/_vf2_better.h>


LEDA_BEGIN_NAMESPACE


#define SORT_NODES
#define LINEAR_CHECK

//unfortunately, this cast is necessary to avoid template recursion
#define MATCH(n) (reinterpret_cast<node_vf2_better2>(n->match))

void vf2_better::input1(const graph& _g1)
{
  //convert to static graphs for better performance
  sg1 = new graph_vf2_better1();
  g1 = &_g1;
  construct_static_graph1(*g1, *sg1);
  constructed1 = true;
}

void vf2_better::input1(prep_graph _pg1)
{
  prep_graph_vf2_better1_t* pg1 = (prep_graph_vf2_better1_t*)_pg1;
  g1 = pg1->first();
  sg1 = pg1->second();
  constructed1 = false;
}

void vf2_better::input2(const graph& _g2)
{
  //convert to static graphs for better performance
  sg2 = new graph_vf2_better2();
  g2 = &_g2;
  construct_static_graph2(*g2, *sg2);
  constructed2 = true;
}

void vf2_better::input2(prep_graph _pg2)
{
  prep_graph_vf2_better2_t* pg2 = (prep_graph_vf2_better2_t*)_pg2;
  g2 = pg2->first();
  sg2 = new graph_vf2_better2();  //no precomputation possible
  construct_static_graph2(*g2, *sg2);
  constructed2 = true;
}

void vf2_better::clean_up()
{
  if(constructed1)
    delete sg1;
  if(constructed2)
    delete sg2;
}

vf2_better::prep_graph vf2_better::prepare_graph(const graph& g, const node_compat*, const edge_compat*) const
{
  prep_graph_vf2_better1_t* pg = new prep_graph_vf2_better1_t;
  pg->first() = &g;
  pg->second() = new graph_vf2_better1();
  construct_static_graph1(g, *pg->second());
  return (prep_graph)pg;
}

void vf2_better::delete_prepared_graph(prep_graph _pg) const
{
  prep_graph_vf2_better1_t* pg = (prep_graph_vf2_better1_t*)_pg;
  delete pg->second();
  delete pg;
}

void vf2_better::construct_static_graph1(const graph& g, graph_vf2_better1& sg) const
{
  sg.start_construction(g.number_of_nodes(), g.number_of_edges());

  node_comparator<graph> nc(&g);
  array<node> nodes(g.number_of_nodes());
  node n;
  int i = 0;
  forall_nodes(n, g)
    nodes[i++] = n;
#ifdef SORT_NODES
  nodes.sort(nc);
#endif

  node_array<node_vf2_better1> mapping(g);
  forall(n, nodes)
  {
    node_vf2_better1 sn = sg.new_node();
    sn->original = n;
    sn->indegree = g.indeg(n);
    mapping[n] = sn;
  }

  forall(n, nodes)
  {
    node_vf2_better1 sn = mapping[n];
    
    for(edge e = g.first_out_edge(n); e != nil; e = g.next_out_edge(e))
      sg.new_edge(sn, mapping[g.opposite(e, n)]);
  }

  sg.finish_construction(); 
}

void vf2_better::construct_static_graph2(const graph& g, graph_vf2_better2& sg) const
{
  sg.start_construction(g.number_of_nodes(), g.number_of_edges());

  node_comparator<graph> nc(&g);
  array<node> nodes(g.number_of_nodes());
  node n;
  int i = 0;
  forall_nodes(n, g)
    nodes[i++] = n;
#ifdef SORT_NODES
  nodes.sort(nc);
#endif

  node_array<node_vf2_better2> mapping(g);
  forall(n, nodes)
  {
    node_vf2_better2 sn = sg.new_node();
    sn->original = n;
    sn->indegree = g.indeg(n);
    mapping[n] = sn;
  }

  forall(n, nodes)
  {
    node_vf2_better2 sn = mapping[n];
    
    for(edge e = g.first_out_edge(n); e != nil; e = g.next_out_edge(e))
      sg.new_edge(sn, mapping[g.opposite(e, n)]);
  }

  sg.finish_construction(); 
}

bool vf2_better::check_pair(node_vf2_better1 n1, node_vf2_better2 n2, unsigned int depth)
{
  if( n1->indegree != n2->indegree ||
    sg1->outdeg(n1) != sg2->outdeg(n2))
    return false;

  edge_vf2_better1 e1;
  edge_vf2_better2 e2;
  node_vf2_better1 t1;
  node_vf2_better2 t2;
    
//        e1
//  sg1   n1  ->  t1
//      |   | match
//      V   V
//  sg2   n2  ->  t2
//        e2

#ifdef LINEAR_CHECK
  int num_matched;

  //check out-edges
  num_matched = 0;
  forall_out_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    {
      MATCH(t1)->reached = true;
      num_matched++;
    }
  }

  forall_out_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->reached)
      num_matched--;
  }

  forall_out_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
      MATCH(t1)->reached = false;
  }

  if(num_matched != 0)
    return false;

  //check in-edges
  num_matched = 0;
  forall_in_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    {
      MATCH(t1)->reached = true;
      num_matched++;
    }
  }

  forall_in_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->reached)
      num_matched--;
  }

  forall_in_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
      MATCH(t1)->reached = false;
  }

  if(num_matched != 0)
    return false;
#else
  bool found;
  //check out-edges
  forall_out_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    {
      found = false;
      forall_out_edges(e2, n2)
        if(sg2->opposite(e2, n2) == MATCH(t1))
        {
          found = true;
          break;
        }
      if(!found)
        return false;
    }
  }

  //check in-edges
  forall_in_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match != nil)
    {
      found = false;
      forall_in_edges(e2, n2)
        if(sg2->opposite(e2, n2) == MATCH(t1))
        {
          found = true;
          break;
        }
      if(!found)
        return false;
    }
  }
#endif

  //add pair
  n1->match = reinterpret_cast<node_vf2_better_helper>(n2);
  n2->match = reinterpret_cast<node_vf2_better_helper>(n1);

  //remove chosen elements from lists, but remember former position
  if(n1->in.content != NULL && n1->out.content != NULL)
    t1both--;
  if(n1->in.content != NULL)
  {
    t1in--;
    n1->in.draw_back();
  }
  if(n1->out.content != NULL)
  {
    t1out--;
    n1->out.draw_back();
  }

  if(n2->in_depth > 0 && n2->out_depth > 0)
    t2both--;
  if(n2->in_depth > 0)
  {
    t2in--;
    t2in_nodes->del_item((PQ_ITEM)n2->in);
    n2->in = nil;
  }
  if(n2->out_depth > 0)
  {
    t2out--;
    t2out_nodes->del_item((PQ_ITEM)n2->out);
    n2->out = nil;
  }

  //remember rollback points
  ll_item<void*>* rollback1in, * rollback1out;
  rollback1in = t1in_nodes.pred;  //last element
  rollback1out = t1out_nodes.pred;

  //add new nodes
  forall_out_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match == nil && t1->out.content == NULL)
    {
      t1->out.content = (void*)t1;
      t1out_nodes.append(t1->out);
      t1out++;
    }
  }

  forall_in_edges(e1, n1)
  {
    t1 = sg1->opposite(e1, n1);
    if(t1->match == nil && t1->in.content == NULL)
    {
      t1->in.content = (void*)t1;
      t1in_nodes.append(t1->in);
      t1in++;
      if(t1->out.content != NULL)
        t1both++;
    }
  }

  forall_out_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->out_depth == 0)
    {
      t2->out_depth = depth;
      t2->out = (void*)t2out_nodes->insert(sg2->index(t2), t2);
      t2out++;
    }
  }

  forall_in_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->in_depth == 0)
    {
      t2->in_depth = depth;
      t2->in = (void*)t2in_nodes->insert(sg2->index(t2), t2);
      t2in++;
      if(t2->out_depth > 0)
        t2both++;
    }
  }

  //Recursion
  if(t1in == t2in && t1out == t2out && t1both == t2both)
    if(match_iso(depth + 1) && job == FINDFIRST)
      return true;

  //Backtracking
  
  forall_in_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->in_depth == depth)
    {
      t2in_nodes->del_item((PQ_ITEM)t2->in);
      t2->in = nil;
      t2->in_depth = 0;
      t2in--;
      if(t2->out_depth > 0)
        t2both--;
    }
  }

  forall_out_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->out_depth == depth)
    {
      t2out_nodes->del_item((PQ_ITEM)t2->out);
      t2->out = nil;
      t2->out_depth = 0;
      t2out--;
    }
  }

  ll_item<void*>* it;

  for(it = t1in_nodes.pred; it != rollback1in; )
  {
    t1in--;
    if(((node_vf2_better1)(it->content))->out.content != nil)
      t1both--;
    it = it->pred;
    it->succ->remove();
  }

  for(it = t1out_nodes.pred; it != rollback1out; )
  {
    t1out--;
    it = it->pred;
    it->succ->remove();
  }

  if(n2->in_depth > 0)
  {
    t2in++;
    n2->in = (void*)t2in_nodes->insert(sg2->index(n2), n2);
    if(n2->out_depth > 0)
      t2both++;
  }
  if(n2->out_depth > 0)
  {
    t2out++;
    n2->out = (void*)t2out_nodes->insert(sg2->index(n2), n2);
  }

  if(n1->in.content != NULL)
  {
    t1in++;
    n1->in.infiltrate();
    if(n1->out.content != NULL)
      t1both++;
  }
  if(n1->out.content != NULL)
  {
    t1out++;
    n1->out.infiltrate();
  }

  n1->match = nil;
  n2->match = nil;

  return false;
}

bool vf2_better::match_iso(int depth)
{
#ifdef COUNT_CALLS
  num_calls++;
#endif

  if(depth == sg1->number_of_nodes() + 1)
  { //recursion successful
    switch(job)
    {
    case FINDFIRST:
      {
        if(node_morph)
        {
          //fill in matching
          node_vf2_better2 n;
          forall_nodes(n, *sg2)
            (*node_morph)[n->original] = MATCH(n)->original;
        }
      }
      break;
    case FINDALL:
      {
        morphism* iso = new morphism();
        node_array<node>& na = iso->first();
        edge_array<edge>& ea = iso->second();
        na.init(*g2);
        ea.init(*g2);
        //fill in matching
        node_vf2_better2 n;
        forall_nodes(n, *sg2)
          na[n->original] = MATCH(n)->original;
        map_edges(*g1, *g2, na, ea);
        morphisms->push_back(iso);
      }
    case CARDINALITY:
      break;
    case ENUM:
      {
        morphism iso;
        node_array<node>& na = iso.first();
        edge_array<edge>& ea = iso.second();
        na.init(*g2);
        ea.init(*g2);
        //fill in mapping
        node_vf2_better2 n;
        forall_nodes(n, *sg2)
          na[n->original] = MATCH(n)->original;
        map_edges(*g1, *g2, na, ea);

        if((*callb)(iso))
          job = FINDFIRST;  //break recursion
      }
      break;
    }
    num_mappings++;

    return true;
  }

  if(t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
#ifdef SEARCH_PQUEUE
    node_vf2_better2 t2out_min = t2out_nodes->inf(t2out_nodes->find_min());
#else
    node_vf2_better2 n, t2out_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->out_depth > 0)
      {
        t2out_min = n;
        break;
      }
#endif
    assert(t2out_min != nil);

    for(ll_item<void*>* it = t1out_nodes.succ; it != &t1out_nodes; it = it->succ)
    {
      if(check_pair((node_vf2_better1)(it->content), t2out_min, depth) && job == FINDFIRST)
        return true;
    }
  }
  else if(t1in > 0 && t2in > 0)
  { //P = T1in(s) x min {T2in(s)}
#ifdef SEARCH_PQUEUE
    node_vf2_better2 t2in_min = t2in_nodes->inf(t2in_nodes->find_min());
#else
    node_vf2_better2 n, t2in_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->in_depth > 0)
      {
        t2in_min = n;
        break;
      }
#endif

    assert(t2in_min != nil);

    for(ll_item<void*>* it = t1in_nodes.succ; it != &t1in_nodes; it = it->succ)
    {
      if(check_pair((node_vf2_better1)(it->content), t2in_min, depth) && job == FINDFIRST)
        return true;
    }
  }
  else if(t1in + t2in + t1out + t2out == 0)
  { //P = ...
    node_vf2_better2 n2, r2_min = nil;
    forall_nodes(n2, *sg2)
      if(n2->match == nil)
      {
        r2_min = n2;
        break;
      }

    node_vf2_better1 n1;
    forall_nodes(n1, *sg1)
      if(n1->match == nil)
      {
        if(check_pair(n1, r2_min, depth) && job == FINDFIRST)
          return true;
      }
  }
  //no matching possible
  return false;
}

bool vf2_better::call_iso()
{
  //initialization
#ifdef PQUEUE
  t2in_nodes = new PQ();
  t2out_nodes = new PQ();
#else
  t2in_nodes = new b_priority_queue<node_vf2_better2>(0, sg2->max_node_index());
  t2out_nodes = new b_priority_queue<node_vf2_better2>(0, sg2->max_node_index());
#endif
  t1in = t2in = t1out = t2out = t1both = t2both = 0;
  node_vf2_better1 n1;
  forall_nodes(n1, *sg1)
    n1->initialize();
  node_vf2_better2 n2;
  forall_nodes(n2, *sg2)
    n2->initialize();
  t1in_nodes.clear();
  t1out_nodes.clear();

  bool successful = match_iso(1);

  delete t2in_nodes;
  delete t2out_nodes;

  return successful;
}

#undef SORT_NODES
#undef LINEAR_CHECK


LEDA_END_NAMESPACE
