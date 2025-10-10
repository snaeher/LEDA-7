/*******************************************************************************
+
+  _vf2_best.cpp
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
#include <LEDA/graph/morphism/_vf2_best.h>

LEDA_BEGIN_NAMESPACE

#define SORT_NODES    //sort nodes by degree before processing?
#define LINEAR_CHECK  //check equivalency of neighborhood in linear time?
#define SEARCH_LIST   //search the list to find minimum element instead of all

//unfortunately, this cast is necessary to avoid template recursion
#define MATCH(n) (reinterpret_cast<node_vf2_best>(n->match))

void vf2_best::input1(const graph& _g1)
{
  //convert to static graphs for best performance
  sg1 = new graph_vf2_best();
  g1 = &_g1;
  construct_static_graph(*g1, *sg1);
  constructed1 = true;
}

void vf2_best::input1(prep_graph _pg1)
{
  prep_graph_vf2_best_t* pg1 = (prep_graph_vf2_best_t*)_pg1;
  g1 = pg1->first();
  sg1 = pg1->second();
  constructed1 = false;
}

void vf2_best::input2(const graph& _g2)
{
  //convert to static graphs for best performance
  sg2 = new graph_vf2_best();
  g2 = &_g2;
  construct_static_graph(*g2, *sg2);
  constructed2 = true;
}

void vf2_best::input2(prep_graph _pg2)
{
  prep_graph_vf2_best_t* pg2 = (prep_graph_vf2_best_t*)_pg2;
  g2 = pg2->first();
  sg2 = pg2->second();
  constructed2 = false;
}

void vf2_best::clean_up()
{
  if(constructed1)
    delete sg1;
  if(constructed2)
    delete sg2;
}

vf2_best::prep_graph vf2_best::prepare_graph(const graph& g, const node_compat* , const edge_compat*) const
{
  prep_graph_vf2_best_t* pg = new prep_graph_vf2_best_t;
  pg->first() = &g;
  pg->second() = new graph_vf2_best();
  construct_static_graph(g, *pg->second());
  return (prep_graph)pg;
}

void vf2_best::delete_prepared_graph(prep_graph _pg) const
{
  prep_graph_vf2_best_t* pg = (prep_graph_vf2_best_t*)_pg;
  delete pg->second();
  delete pg;
}

//only takes leda::graph, does not support static_graph
void vf2_best::construct_static_graph(const graph& g, graph_vf2_best& sg) const
{
  sg.start_construction(g.number_of_nodes(), g.number_of_edges());

  node_comparator<graph> nc(&g);

  //move to array
  array<node> nodes(g.number_of_nodes());
  node n;
  int i = 0;
  forall_nodes(n, g)
    nodes[i++] = n;

#ifdef SORT_NODES
  //sort by degree
  nodes.sort(nc);
#endif

  //fill in static graph
  node_array<node_vf2_best> mapping(g);
  forall(n, nodes)
  {
    node_vf2_best sn = sg.new_node();
    sn->original = n;
    sn->indegree = g.indeg(n);
    mapping[n] = sn;
  }

  forall(n, nodes)
  {
    node_vf2_best sn = mapping[n];
    for(edge e = g.first_out_edge(n); e != nil; e = g.next_out_edge(e))
      sg.new_edge(sn, mapping[g.opposite(e, n)]);
  }

  sg.finish_construction(); 
}

bool vf2_best::check_pair(node_vf2_best n1, node_vf2_best n2, unsigned int depth)
{
  if( n1->indegree != n2->indegree ||
    sg1->outdeg(n1) != sg2->outdeg(n2))
    return false;

  ll_item<void*>* rollback1in, * rollback1out, * rollback2in, * rollback2out;

  {

  edge_vf2_best e1, e2;
  node_vf2_best t1, t2;
    
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
  n1->match = (void*)n2;
  n2->match = (void*)n1;

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

  if(n2->in.content != NULL && n2->out.content != NULL)
    t2both--;
  if(n2->in.content != NULL)
  {
    t2in--;
    n2->in.draw_back();
  }
  if(n2->out.content != NULL)
  {
    t2out--;
    n2->out.draw_back();
  }

  //remember rollback points
  rollback1in = t1in_nodes.pred;  //last element
  rollback1out = t1out_nodes.pred;
  rollback2in = t2in_nodes.pred;
  rollback2out = t2out_nodes.pred;

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
    if(t2->match == nil && t2->out.content == NULL)
    {
      t2->out.content = (void*)t2;
      t2out_nodes.append(t2->out);
      t2out++;
    }
  }

  forall_in_edges(e2, n2)
  {
    t2 = sg2->opposite(e2, n2);
    if(t2->match == nil && t2->in.content == NULL)
    {
      t2->in.content = (void*)t2;
      t2in_nodes.append(t2->in);
      t2in++;
      if(t2->out.content != NULL)
        t2both++;
    }
  }

  }

  //Recursion
  if(t1in == t2in && t1out == t2out && t1both == t2both)
    if(match_iso(depth + 1) && job == FINDFIRST)
      return true;

  {

  //Backtracking
  
  ll_item<void*>* it;
  
  for(it = t2in_nodes.pred; it != rollback2in; )
  {
    t2in--;
    if(((node_vf2_best)(it->content))->out.content != NULL)
      t2both--;
    it = it->pred;
    it->succ->remove();
  }

  for(it = t2out_nodes.pred; it != rollback2out; )
  {
    t2out--;
    it = it->pred;
    it->succ->remove();
  }

  for(it = t1in_nodes.pred; it != rollback1in; )
  {
    t1in--;
    if(((node_vf2_best)(it->content))->out.content != NULL)
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

  if(n2->in.content != NULL)
  {
    t2in++;
    n2->in.infiltrate();
    if(n2->out.content != NULL)
      t2both++;
  }
  if(n2->out.content != NULL)
  {
    t2out++;
    n2->out.infiltrate();
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

  }

  return false;
}

bool vf2_best::match_iso(int depth)
{
#ifdef COUNT_CALLS
  num_calls++;
#endif

  if(depth == sg2->number_of_nodes() + 1)
  { //recursion successful
    switch(job)
    {
    case FINDFIRST:
      if(node_morph)
      {
        //fill in matching
        node_vf2_best n;
        forall_nodes(n, *sg2)
          (*node_morph)[n->original] = MATCH(n)->original;

        if(edge_morph)
          map_edges(*g1, *g2, *node_morph, *edge_morph);
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
        node_vf2_best n;
        forall_nodes(n, *sg2)
          na[n->original] = MATCH(n)->original;
        map_edges(*g1, *g2, na, ea);
        morphisms->push_back(iso);
      }
    case CARDINALITY:
      num_mappings++;
      break;
    case ENUM:
      {
        morphism iso;
        node_array<node>& na = iso.first();
        edge_array<edge>& ea = iso.second();
        na.init(*g2);
        ea.init(*g2);
        //fill in matching
        node_vf2_best n;
        forall_nodes(n, *sg2)
          na[n->original] = MATCH(n)->original;
        map_edges(*g1, *g2, na, ea);

        if((*callb)(iso))
          job = FINDFIRST;
      }

      num_mappings++;
      break;
    }

    return true;
  }

  if(t1out > 0 && t2out > 0)
  { //P = T1out(s) x min {T2out(s)}
#ifdef SEARCH_LIST
    node_vf2_best t2out_min = (node_vf2_best)t2out_nodes.succ->content;
    for(ll_item<void*>* it = t2out_nodes.succ->succ; it != &t2out_nodes; it = it->succ)
      if(sg2->index((node_vf2_best)(it->content)) < sg2->index(t2out_min))
        t2out_min = (node_vf2_best)(it->content);
#else
    node_vf2_best n, t2out_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->out.content != NULL)
      {
        t2out_min = n;
        break;
      }
#endif
    for(ll_item<void*>* it = t1out_nodes.succ; it != &t1out_nodes; it = it->succ)
      if(check_pair((node_vf2_best)(it->content), t2out_min, depth) && job == FINDFIRST)
        return true;
  }
  else if(t1in > 0 && t2in > 0)
  { //P = T1in(s) x min {T2in(s)}
#ifdef SEARCH_LIST
    //node_vf2_best t2in_min = (node_vf2_best)0xFFFFFFFF;
    //for(ll_item<void*>* it = t2in_nodes.succ; it != &t2in_nodes; it = it->succ)
    //{
    //  if(it->content < t2in_min)
    //    t2in_min = (node_vf2_best)(it->content);
    //}
    node_vf2_best t2in_min = (node_vf2_best)t2in_nodes.succ->content;
    for(ll_item<void*>* it = t2in_nodes.succ->succ; it != &t2in_nodes; it = it->succ)
      if(sg2->index((node_vf2_best)(it->content)) < sg2->index(t2in_min))
        t2in_min = (node_vf2_best)(it->content);
#else
    node_vf2_best n, t2in_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil && n->in.content != NULL)
      {
        t2in_min = n;
        break;
      }
#endif

    for(ll_item<void*>* it = t1in_nodes.succ; it != &t1in_nodes; it = it->succ)
      if(check_pair((node_vf2_best)(it->content), t2in_min, depth) && job == FINDFIRST)
        return true;
  }
  else if(t2in + t2out == 0 && t1in + t1out == 0)
  { //P = ...
    node_vf2_best n, r2_min = nil;
    forall_nodes(n, *sg2)
      if(n->match == nil)
      {
        r2_min = n;
        break;
      }

    forall_nodes(n, *sg1)
      if(n->match == nil)
      {
        if(check_pair(n, r2_min, depth) && job == FINDFIRST)
          return true;
      }
  }
  //no matching possible
  return false;
}

bool vf2_best::call_iso()
{
  //initialization
  t1in = t2in = t1out = t2out = t1both = t2both = 0;
  node_vf2_best n;
  forall_nodes(n, *sg1)
    n->initialize();
  forall_nodes(n, *sg2)
    n->initialize();
  t1in_nodes.clear();
  t1out_nodes.clear();
  t2in_nodes.clear();
  t2out_nodes.clear();

  bool successful = match_iso(1);

  return successful;
}

#undef SORT_NODES 
#undef LINEAR_CHECK
#undef SEARCH_LIST  

LEDA_END_NAMESPACE
